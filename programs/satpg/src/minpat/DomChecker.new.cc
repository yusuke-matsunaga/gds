﻿
/// @file DomChecker.cc
/// @brief DomChecker の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2005-2011, 2013-2014, 2015 Yusuke Matsunaga
/// All rights reserved.


#include "DomChecker.h"

#include "FaultAnalyzer.h"

#include "TpgFault.h"
#include "TvMgr.h"
#include "TestVector.h"
#include "Fsim.h"
#include "DetOp.h"
#include "NodeSet.h"

#include "GvalCnf.h"
#include "FvalCnf.h"
#include "SatEngine.h"


BEGIN_NAMESPACE_YM_SATPG

BEGIN_NONAMESPACE

const bool verify_dom_check = false;

struct FaultLt
{
  FaultLt(DomChecker& checker) :
    mChecker(checker)
  {
  }

  bool
  operator()(ymuint left,
	     ymuint right)
  {
    return mChecker.dom_cand_size(left) < mChecker.dom_cand_size(right);
  }

  DomChecker& mChecker;

};

struct FidGt
{
  FidGt(DomChecker& checker) :
    mChecker(checker)
  {
  }

  bool
  operator()(ymuint left,
	     ymuint right)
  {
    return mChecker.dom_cand_size(left) > mChecker.dom_cand_size(right);
  }

  DomChecker& mChecker;

};

struct FaultGt
{
  FaultGt(DomChecker& checker) :
    mChecker(checker)
  {
  }

  bool
  operator()(const TpgFault* left,
	     const TpgFault* right)
  {
    return mChecker.dom_cand_size(left->id()) > mChecker.dom_cand_size(right->id());
  }

  DomChecker& mChecker;

};

// 2つのリストが共通要素を持つとき true を返す．
// リストはソートされていると仮定する．
inline
bool
check_intersect(const vector<ymuint>& list1,
		const vector<ymuint>& list2)
{
  ymuint n1 = list1.size();
  ymuint n2 = list2.size();
  ymuint i1 = 0;
  ymuint i2 = 0;
  ymuint v1 = list1[i1];
  ymuint v2 = list2[i2];
  for ( ; ; ) {
    if ( v1 < v2 ) {
      ++ i1;
      if ( i1 >= n1 ) {
	return false;
      }
      v1 = list1[i1];
    }
    else if ( v1 > v2 ) {
      ++ i2;
      if ( i2 >= n2 ) {
	return false;
      }
      v2 = list2[i2];
    }
    else {
      return true;
    }
  }
  return false;
}

const TpgNode*
common_node(const TpgNode* node1,
	    const TpgNode* node2)
{
  ymuint id1 = node1->id();
  ymuint id2 = node2->id();
  for ( ; ; ) {
    if ( node1 == node2 ) {
      return node1;
    }
    if ( id1 < id2 ) {
      node1 = node1->imm_dom();
      if ( node1 == NULL ) {
	return NULL;
      }
      id1 = node1->id();
    }
    else if ( id1 > id2 ) {
      node2 = node2->imm_dom();
      if ( node2 == NULL ) {
	return NULL;
      }
      id2 = node2->id();
    }
  }
}

END_NONAMESPACE


//////////////////////////////////////////////////////////////////////
// クラス DomChecker
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
// @param[in] analyzer 故障の情報を持つクラス
// @param[in] fsim 故障シミュレータ
// @param[in] tvmgr テストベクタのマネージャ
DomChecker::DomChecker(FaultAnalyzer& analyzer,
		       TvMgr& tvmgr,
		       Fsim& fsim) :
  mAnalyzer(analyzer),
  mTvMgr(tvmgr),
  mFsim(fsim)
{
  mVerbose = mAnalyzer.verbose();
  mMaxNodeId = mAnalyzer.max_node_id();
  mMaxFaultId = mAnalyzer.max_fault_id();
  mFaultDataArray.resize(mMaxFaultId);
  mDetFlag.resize(mMaxFaultId, 0UL);

  for (ymuint i = 0; i < mMaxFaultId; ++ i) {
    mFaultDataArray[i].mDetCount = 0;
  }
}

// @brief デストラクタ
DomChecker::~DomChecker()
{
}

// @brief verbose フラグを設定する．
// @param[in] verbose 表示を制御するフラグ
void
DomChecker::set_verbose(int verbose)
{
  mVerbose = verbose;
}

// @brief 支配故障を求める．
//
// 結果は mDomFaultList に格納される．
void
DomChecker::get_dom_faults(ymuint method,
			   const vector<const TpgFault*>& src_list,
			   vector<const TpgFault*>& dom_fault_list)
{
  StopWatch local_timer;
  local_timer.start();

  mDomCheckCount = 0;

  do_fsim(src_list);

  USTime fsim_time = local_timer.time();

  make_cand_list(src_list);

  ymuint fault_num = src_list.size();

  vector<ymuint> fault_list(fault_num);
  for (ymuint i = 0; i < fault_num; ++ i) {
    fault_list[i] = src_list[i]->id();
  }

  // 被支配故障につけるマーク
  vector<bool> dom_flag(mMaxFaultId, false);

  vector<ymuint> fault_list1;
  drop_dom_faults(fault_list, 0, dom_flag, fault_list1);

  vector<ymuint> fault_list2;
  drop_dom_faults(fault_list1, 1, dom_flag, fault_list2);

  vector<ymuint> fault_list3;
  drop_dom_faults(fault_list2, 2, dom_flag, fault_list3);

  mSingleNum = 0;
  dom_fault_list.clear();
  dom_fault_list.reserve(fault_list3.size());
  for (ymuint i = 0; i < fault_list3.size(); ++ i) {
    ymuint f_id = fault_list3[i];
    dom_fault_list.push_back(mAnalyzer.fault(f_id));
    const FaultInfo& fi = mAnalyzer.fault_info(f_id);
    if ( fi.single_cube() ) {
      ++ mSingleNum;
    }
  }

  ymuint dom_fault_num = dom_fault_list.size();
  local_timer.stop();
  if ( mVerbose ) {
    cout << "Total    " << setw(8) << src_list.size() << " original faults" << endl;
    cout << "Total    " << setw(8) << dom_fault_num << " dominator faults" << endl;
    cout << "         " << setw(8) << mSingleNum << " single cube faults" << endl;
    cout << "         " << setw(8) << mStats[0].mSingleSat + mStats[1].mSingleSat + mStats[2].mSingleSat
	 << " sigle cube test" << endl;
    cout << "         " << setw(8) << mStats[0].mNoDom + mStats[1].mNoDom + mStats[2].mNoDom
	 << " non-dominance" << endl;
    cout << "         " << setw(8) << mStats[0].mSat + mStats[1].mSat + mStats[2].mSat
	 << " dominance test" << endl;
    cout << "         " << setw(8) << mDomCheckCount << " smart dom check" << endl;
    cout << "         " << setw(8) << mPat << " patterns simulated" << endl;
    cout << "CPU time for dominance test" << local_timer.time() << endl
	 << "  CPU time (fsim)    " << fsim_time << endl;
    cout << "  CPU time (success) " << mSuccessTime << "(MAX " << mSuccessMax << ")" << endl
	 << "  CPU time (failure) " << mFailureTime << "(MAX " << mFailureMax << ")" << endl
	 << "  CPU time (abort)   " << mAbortTime   << "(MAX " << mAbortMax << ")" << endl;
  }
}

// @brief 故障シミュレーションを行い，故障検出パタンを記録する．
// @param[in] fault_list 故障リスト
void
DomChecker::do_fsim(const vector<const TpgFault*>& fault_list)
{
  vector<TestVector*> cur_array;
  cur_array.reserve(kPvBitLen);

  DetOp op;

  ymuint nf = fault_list.size();
  ymuint npat = 0;
  for (ymuint i = 0; i < nf; ++ i) {
    const TpgFault* fault = fault_list[i];
    const FaultInfo& fi = mAnalyzer.fault_info(fault->id());
    TestVector* tv = fi.testvector();
    cur_array.push_back(tv);
    if ( cur_array.size() == kPvBitLen ) {
      if ( mVerbose > 1 ) {
	cout << "\rFSIM: " << setw(6) << npat;
	cout.flush();
      }
      mFsim.ppsfp(cur_array, op);
      const vector<pair<ymuint, PackedVal> >& det_list = op.det_list();
      get_dom_cand(det_list);
      cur_array.clear();
      op.clear_det_list();
      npat += kPvBitLen;
    }
  }
  if ( !cur_array.empty() ) {
    mFsim.ppsfp(cur_array, op);
    const vector<pair<ymuint, PackedVal> >& det_list = op.det_list();
    get_dom_cand(det_list);
    op.clear_det_list();
    npat += cur_array.size();
    cur_array.clear();
  }

  vector<TestVector*> cur_array2(kPvBitLen);
  for (ymuint i = 0; i < kPvBitLen; ++ i) {
    TestVector* tv = mTvMgr.new_vector();
    cur_array2[i] = tv;
  }

  for ( ; ; ) {
    for (ymuint i = 0; i < kPvBitLen; ++ i) {
      cur_array2[i]->set_from_random(mRandGen);
    }
    mFsim.ppsfp(cur_array2, op);
    ymuint nchg = 0;
    const vector<pair<ymuint, PackedVal> >& det_list = op.det_list();
    nchg += get_dom_cand(det_list);
    op.clear_det_list();
    npat += kPvBitLen;
    if ( nchg == 0 ) {
      break;
    }
    if ( mVerbose > 1 ) {
      cout << "\rFSIM: " << setw(6) << npat;
      cout.flush();
    }
  }
  if ( mVerbose > 1 ) {
    cout << endl;
  }

  mPat = npat;

  // 乱数パタンは削除しておく．
  for (ymuint i = 0; i < kPvBitLen; ++ i) {
    mTvMgr.delete_vector(cur_array2[i]);
  }
}

// シミュレーション結果を用いて被支配故障の候補リストを作る．
//
// 基本的に f1 のビットベクタが f2 のビットベクタに含まれているときに
// f1 が f2 を支配している可能性がある．
// 複数回のシミュレーションでその候補を絞ってゆく．
// この関数は変更のあった故障の数を返す．
//
// 逆に支配故障のリストを直接作れないかと考えたが，この場合，
// 最初に候補となる故障は det_list に含まれない(ビットベクタが0)の故障
// を含むので効率のよいやり方が思いつかなかった．
ymuint
DomChecker::get_dom_cand(const vector<pair<ymuint, PackedVal> >& det_list)
{
  ymuint n = det_list.size();
  ymuint nchg = 0;

  // det_list の内容を mDetFlag に転写する．
  for (ymuint i = 0; i < n; ++ i) {
    ymuint f_id = det_list[i].first;
    PackedVal bv = det_list[i].second;
    mDetFlag[f_id] = bv;
  }

  // 新しくリストを作る時の作業用
  // 要素数の最大値を指定しておく．
  // reserve() を各故障に対してやってしまうとメモリが無駄になる．
  vector<ymuint> tmp_list;
  tmp_list.reserve(n);

  // 検出結果を用いて支配される故障の候補リストを作る．
  for (ymuint i1 = 0; i1 < n; ++ i1) {
    ymuint f1_id = det_list[i1].first;
    PackedVal bv1 = det_list[i1].second;
    FaultData& fd1 = mFaultDataArray[f1_id];
    if ( fd1.mDetCount == 0 ) {
      // 初めて検出された場合
      tmp_list.clear();
      for (ymuint i2 = 0; i2 < n; ++ i2) {
	if ( i2 == i1 ) {
	  continue;
	}
	ymuint f2_id = det_list[i2].first;
	PackedVal bv2 = det_list[i2].second;
	if ( (bv1 & bv2) == bv1 ) {
	  // bv1 が 1 の部分は bv2 も 1
	  tmp_list.push_back(f2_id);
	}
      }
      // 最後に要素数が確定してから mDomCandList1 にコピーする．
      fd1.mDomCandList1 = tmp_list;
    }
    else {
      // 二回目以降
      // dst_list の要素で bv1 を含まないものを落とす．
      vector<ymuint>& dst_list = fd1.mDomCandList1;
      ymuint n = dst_list.size();
      ymuint wpos = 0;
      for (ymuint rpos = 0; rpos < n; ++ rpos) {
	ymuint f2_id = dst_list[rpos];
	PackedVal bv2 = mDetFlag[f2_id];
	if ( (bv1 & bv2) == bv1 ) {
	  if ( wpos < rpos ) {
	    dst_list[wpos] = f2_id;
	  }
	  ++ wpos;
        }
      }
      if ( wpos < dst_list.size() ) {
	nchg += dst_list.size() - wpos;
	dst_list.erase(dst_list.begin() + wpos, dst_list.end());
      }
    }

    // 直接は使わないが，ヒューリスティックの評価値として
    // 検出回数を用いる．
    fd1.mDetCount += count_ones(bv1);
  }

  // mDetFlag をクリアしておく．
  for (ymuint i1 = 0; i1 < n; ++ i1) {
    ymuint f1_id = det_list[i1].first;
    mDetFlag[f1_id] = 0UL;
  }

  return nchg;
}

// @brief 各故障ごとの候補リストを作る．
void
DomChecker::make_cand_list(const vector<const TpgFault*>& fault_list)
{
  // 構造的に独立な故障対を対象外にする．
  ymuint nf = fault_list.size();
  for (ymuint i1 = 0; i1 < nf; ++ i1) {
    const TpgFault* f1 = fault_list[i1];
    FaultData& fd1 = mFaultDataArray[f1->id()];
    const vector<ymuint>& input_list1 = mAnalyzer.input_list(f1->id());

    vector<ymuint>& dst_list = fd1.mDomCandList1;
    ymuint wpos = 0;
    for (ymuint rpos = 0; rpos < dst_list.size(); ++ rpos) {
      ymuint f2_id = dst_list[rpos];
      const vector<ymuint>& input_list2 = mAnalyzer.input_list(f2_id);
      bool intersect = check_intersect(input_list1, input_list2);
      if ( !intersect ) {
	// 共通部分を持たない故障は独立
	continue;
      }
      if ( wpos < rpos ) {
	dst_list[wpos] = f2_id;
      }
      ++ wpos;
    }
    if ( wpos < dst_list.size() ) {
      dst_list.erase(dst_list.begin() + wpos, dst_list.end());
    }
  }

  // 故障を直接の被支配故障候補数の多い順に並べる．
  vector<const TpgFault*> tmp_fault_list = fault_list;
  {
    FaultGt comp(*this);
    sort(tmp_fault_list.begin(), tmp_fault_list.end(), comp);
  }

  // 被支配故障の候補から支配故障の候補を作る．
  for (ymuint i = 0; i < nf; ++ i) {
    const TpgFault* f1 = tmp_fault_list[i];
    const TpgNode* f1node = f1->node();
    FaultData& fd1 = mFaultDataArray[f1->id()];
    const vector<ymuint>& input_list1 = mAnalyzer.input_list2(f1->id());
    const vector<ymuint>& cand_list = fd1.mDomCandList1;
    for (ymuint j = 0; j < cand_list.size(); ++ j) {
      ymuint f2_id = cand_list[j];
      const TpgFault* f2 = mAnalyzer.fault(f2_id);
      const TpgNode* f2node = f2->node();
      FaultData& fd2 = mFaultDataArray[f2_id];
      const vector<ymuint>& input_list2 = mAnalyzer.input_list2(f2_id);
      if ( check_intersect(input_list1, input_list2) ) {
	if ( common_node(f1node, f2node) != NULL ) {
	  fd2.mDomCandList2[0].push_back(f1->id());
	}
	else {
	  fd2.mDomCandList2[1].push_back(f1->id());
	}
      }
      else {
	fd2.mDomCandList2[2].push_back(f1->id());
      }
    }
  }
}

BEGIN_NONAMESPACE

class FaultIdxLt
{
public:

  FaultIdxLt(DomChecker& checker,
	     ymuint idx) :
    mChecker(checker),
    mIdx(idx)
  {
  }

  bool
  operator()(ymuint left,
	     ymuint right)
  {
    return mChecker.dom_cand2_size(left, mIdx) < mChecker.dom_cand2_size(right, mIdx);
  }


private:

  DomChecker& mChecker;

  ymuint mIdx;

};

END_NONAMESPACE

void
DomChecker::drop_dom_faults(const vector<ymuint>& fault_list,
			    ymuint idx,
			    vector<bool>& dom_flag,
			    vector<ymuint>& dst_fault_list)
{
  vector<ymuint> tmp_fault_list = fault_list;
  FaultLt comp(*this);
  sort(tmp_fault_list.begin(), tmp_fault_list.end(), comp);

  mStats[idx].clear();
  ymuint fault_num = fault_list.size();
  ymuint cur_num = fault_num;
  for (ymuint i1 = 0; i1 < fault_num; ++ i1) {
    ymuint f1_id = tmp_fault_list[i1];
    if ( dom_flag[f1_id] ) {
      continue;
    }

    if ( mVerbose > 1 ) {
      cout << "\rDOM" << idx << ": " << setw(6) << i1 << " / " << setw(6) << fault_num
	   << " / " << setw(6) << cur_num;
      cout.flush();
    }

    FaultData& fd1 = mFaultDataArray[f1_id];

    // f1 を支配する可能性のある故障番号のリスト
    const vector<ymuint>& cand_list = fd1.mDomCandList2[idx];
    if ( cand_list.empty() ) {
      continue;
    }

    if ( check_faults_dominance(f1_id, cand_list, dom_flag, mStats[idx]) ) {
      dom_flag[f1_id] = true;
      -- cur_num;
    }
  }

  // dom_flag がついた故障を落とす．
  dst_fault_list.clear();
  dst_fault_list.reserve(cur_num);
  for (ymuint i = 0; i < fault_num; ++ i) {
    ymuint f_id = tmp_fault_list[i];
    if ( dom_flag[f_id] ) {
      mAnalyzer.clear_fault_info(f_id);
    }
    else {
      dst_fault_list.push_back(f_id);
    }
  }

  if ( mVerbose ) {
    print_stats(cout, mStats[idx]);
  }
}

// @brief f1_id を支配する故障を fault_list の中から探す．
//
// 支配されていたら true を返す．
bool
DomChecker::check_faults_dominance(ymuint f1_id,
				   const vector<ymuint>& fault_list,
				   const vector<bool>& dom_flag,
				   DomStats& dom_stats)
{
  const TpgFault* f1 = mAnalyzer.fault(f1_id);

  GvalCnf gval_cnf(mMaxNodeId);
  FvalCnf fval_cnf(mMaxNodeId, gval_cnf);
  SatEngine engine(string(), string(), NULL);

  const FaultInfo& fi1 = mAnalyzer.fault_info(f1_id);

  if ( fi1.single_cube() ) {
    // f1 を検出しない CNF を作成
    engine.add_negation(gval_cnf, fi1.sufficient_assignment());
  }
  else {
    FvalCnf fval_cnf(mMaxNodeId, gval_cnf);
    // f1 を検出しない CNF を作成
    engine.make_fval_cnf(fval_cnf, f1, mAnalyzer.node_set(f1_id), kVal0);
  }

  for (ymuint i2 = 0; i2 < fault_list.size(); ++ i2) {
    ymuint f2_id = fault_list[i2];
    if ( dom_flag[f2_id] ) {
      continue;
    }

    const FaultInfo& fi2 = mAnalyzer.fault_info(f2_id);
    const TpgFault* f2 = fi2.fault();

    ++ dom_stats.mSingleSat;

    // これが f2 の十分割当のもとで成り立ったら支配しない
    if ( engine.check_sat(gval_cnf, fi2.sufficient_assignment()) == kB3True ) {
      ++ dom_stats.mNoDom;
      continue;
    }
    if ( fi2.single_cube() ) {
      // これ以上のチェックは必要ない．
      ++ dom_stats.mSingleDom;
      return true;
    }

    ++ dom_stats.mSat;
    if ( check_fault_dominance(f2, f1) ) {
      ++ dom_stats.mDom;
      return true;
    }
  }

  return false;
}

// @brief f1 が f2 を支配しているか調べる．
bool
DomChecker::check_fault_dominance(const TpgFault* f1,
				  const TpgFault* f2)
{
  StopWatch timer;
  timer.start();

  const TpgNode* fnode1 = f1->node();
  const TpgNode* fnode2 = f2->node();
  const TpgNode* dom_node = common_node(fnode1, fnode2);

  Bool3 sat_stat = kB3X;
  if ( dom_node != NULL ) {
    // 伝搬経路に共通な dominator がある時
    ++ mDomCheckCount;

    SatEngine engine(string(), string(), NULL);
    GvalCnf gval_cnf(mMaxNodeId);

    // 共通部分のノード集合
    NodeSet node_set0;
    node_set0.mark_region(mMaxNodeId, dom_node);

    // 故障1に固有のノード集合
    NodeSet node_set1;
    node_set1.mark_region2(mMaxNodeId, fnode1, dom_node);

    // 故障2に固有のノード集合
    NodeSet node_set2;
    node_set2.mark_region2(mMaxNodeId, fnode2, dom_node);

    FvalCnf fval_cnf0(mMaxNodeId, gval_cnf);
    FvalCnf fval_cnf1(mMaxNodeId, gval_cnf);
    FvalCnf fval_cnf2(mMaxNodeId, gval_cnf);

    engine.make_fval_cnf2(fval_cnf0, fval_cnf1, fval_cnf2,
			  dom_node, f1, f2,
			  node_set0, node_set1, node_set2);

    Literal dlit1(fval_cnf1.dvar(dom_node));
    Literal dlit2(fval_cnf2.dvar(dom_node));

    vector<Literal> assumption(2);
    assumption[0] = dlit1;
    assumption[1] = ~dlit2;
    sat_stat = engine.check_sat(assumption);

    if ( verify_dom_check ) {
      SatEngine engine(string(), string(), NULL);
      GvalCnf gval_cnf(mMaxNodeId);
      FvalCnf fval_cnf1(mMaxNodeId, gval_cnf);
      const NodeSet& _node_set1 = mAnalyzer.node_set(f1->id());
      engine.make_fval_cnf(fval_cnf1, f1, _node_set1, kVal1);

      const FaultInfo& fi2 = mAnalyzer.fault_info(f2->id());
      if ( fi2.single_cube() ) {
	// f2 を検出しない CNF を生成
	engine.add_negation(gval_cnf, fi2.sufficient_assignment());
      }
      else {
	FvalCnf fval_cnf2(mMaxNodeId, gval_cnf);
	const NodeSet& node_set2 = mAnalyzer.node_set(f2->id());
	// f1 を検出して f2 を検出しない CNF を生成
	engine.make_fval_cnf(fval_cnf2, f2, node_set2, kVal0);
      }
      Bool3 sat_stat2 = engine.check_sat();
      if ( sat_stat != sat_stat2 ) {
	cout << endl;
	cout << "Error" << endl;
	cout << f1 << " " << f2 << endl;
	cout << " sat_stat  = " << sat_stat << endl
	     << " sat_stat2 = " << sat_stat2 << endl;
	{
	  cout << "dom_node = " << dom_node->id() << endl;
	  cout << "NodeSet0" << endl;
	  for (ymuint i = 0; i < node_set0.tfo_size(); ++ i) {
	    cout << " " << node_set0.tfo_tfi_node(i)->id();
	  }
	  cout << endl;
	  cout << "NodeSet1" << endl;
	  for (ymuint i = 0; i < node_set1.tfo_size(); ++ i) {
	    cout << " " << node_set1.tfo_tfi_node(i)->id();
	  }
	  cout << endl;
	  cout << "NodeSet2" << endl;
	  for (ymuint i = 0; i < node_set2.tfo_size(); ++ i) {
	    cout << " " << node_set2.tfo_tfi_node(i)->id();
	  }
	  cout << endl;
	}
      }
    }
  }
  else {
    SatEngine engine(string(), string(), NULL);
    GvalCnf gval_cnf(mMaxNodeId);
    FvalCnf fval_cnf1(mMaxNodeId, gval_cnf);
    const NodeSet& node_set1 = mAnalyzer.node_set(f1->id());
    engine.make_fval_cnf(fval_cnf1, f1, node_set1, kVal1);

    const FaultInfo& fi2 = mAnalyzer.fault_info(f2->id());
    if ( fi2.single_cube() ) {
      // f2 を検出しない CNF を生成
      engine.add_negation(gval_cnf, fi2.sufficient_assignment());
    }
    else {
      FvalCnf fval_cnf2(mMaxNodeId, gval_cnf);
      const NodeSet& node_set2 = mAnalyzer.node_set(f2->id());
      // f1 を検出して f2 を検出しない CNF を生成
      engine.make_fval_cnf(fval_cnf2, f2, node_set2, kVal0);
    }

    sat_stat = engine.check_sat();
  }

  timer.stop();
  USTime time = timer.time();
  if ( sat_stat == kB3False ) {
    mSuccessTime += time;
    if ( mSuccessMax.usr_time_usec() < time.usr_time_usec() ) {
      mSuccessMax = time;
    }
    return true;
  }
  else if ( sat_stat == kB3True ) {
    mFailureTime += time;
    if ( mFailureMax.usr_time_usec() < time.usr_time_usec() ) {
      mFailureMax = time;
    }
    return false;
  }
  else {
    mAbortTime += timer.time();
    if ( mAbortMax.usr_time_usec() < time.usr_time_usec() ) {
      mAbortMax = time;
    }
    return false;
  }
}

// @brief シミュレーション時の検出パタン数を返す．
ymuint
DomChecker::det_count(ymuint f_id)
{
  ASSERT_COND( f_id < mMaxFaultId );
  return mFaultDataArray[f_id].mDetCount;
}

// @brief 非支配故障候補数を返す．
ymuint
DomChecker::dom_cand_size(ymuint f_id)
{
  ASSERT_COND( f_id < mMaxFaultId );
  return mFaultDataArray[f_id].mDomCandList1.size();
}

// @brief 支配故障候補数を返す．
ymuint
DomChecker::dom_cand2_size(ymuint f_id,
			   ymuint idx)
{
  ASSERT_COND( f_id < mMaxFaultId );
  return mFaultDataArray[f_id].mDomCandList2[idx].size();
}

// @brief DomStats を出力する．
void
DomChecker::print_stats(ostream& s,
			const DomStats& stats)
{
  s << " ("
    << setw(6) << stats.mNoDom << ", "
    << setw(6) << stats.mSingleDom << ") / "
    << setw(6) << stats.mSingleSat << "  ("
    << setw(6) << stats.mDom << " / "
    << setw(6) << stats.mSat << ")" << endl;
 }

END_NAMESPACE_YM_SATPG