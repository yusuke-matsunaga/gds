
/// @file SatEngine.cc
/// @brief SatEngine の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2005-2010, 2012-2014 Yusuke Matsunaga
/// All rights reserved.


#include "SatEngine.h"

#include "DetectOp.h"
#include "UntestOp.h"
#include "DtpgStats.h"
#include "TpgNode.h"
#include "TpgFault.h"
#include "BackTracer.h"
#include "LitMap.h"
#include "logic/SatSolver.h"
#include "logic/SatStats.h"


BEGIN_NAMESPACE_YM_SATPG

BEGIN_NONAMESPACE

// @brief 故障挿入回路を表す CNF 式を作る．
// @param[in] solver SAT ソルバー
// @param[in] ivar 入力の変数
// @param[in] fvar 故障変数
// @param[in] ovar 出力の変数
// @param[in] fval 故障値
void
make_flt_cnf(SatSolver& solver,
	     VarId ivar,
	     VarId fvar,
	     VarId ovar,
	     int vval)
{
  Literal l0(ivar, false);
  Literal l1(fvar, false);
  Literal l2(ovar, false);
  if ( vval == 0 ) {
    solver.add_clause( l0, ~l2);
    solver.add_clause(~l1, ~l2);
    solver.add_clause(~l0,  l1, l2);
  }
  else {
    solver.add_clause(~l0, l2);
    solver.add_clause(~l1, l2);
    solver.add_clause( l0, l1, ~l2);
  }
}

// @brief ノードに正常回路用の変数を設定する．
// @param[in] solver SAT ソルバー
// @param[in] node 対象のノード
void
set_gvar(SatSolver& solver,
	 TpgNode* node)
{
  // ノードそのものに割り当てる．
  VarId gvar = solver.new_var();
  node->set_gvar(gvar);
}

// @brief ノードに正常回路用の変数を設定する．
// @param[in] solver SAT ソルバー
// @param[in] node 対象のノード
void
set_fvar(SatSolver& solver,
	 TpgNode* node)
{
  // ノードそのものに割り当てる．
  VarId fvar = solver.new_var();
  VarId dvar = solver.new_var();
  node->set_fvar(fvar, dvar);
}

END_NONAMESPACE


// @brief コンストラクタ
SatEngine::SatEngine()
{
  mOutP = NULL;
  mTimerEnable = false;
}

// @brief デストラクタ
SatEngine::~SatEngine()
{
}

// @brief 使用する SAT エンジンを指定する．
void
SatEngine::set_mode(const string& type,
		    const string& option,
		    ostream* outp)
{
  mType = type;
  mOption = option;
  mOutP = outp;
}


BEGIN_NONAMESPACE

END_NONAMESPACE

// @brief 統計情報をクリアする．
void
SatEngine::clear_stats()
{
  mStats.mCnfGenCount = 0;
  mStats.mCnfGenTime.set(0.0, 0.0, 0.0);

  mStats.mDetCount = 0;
  mStats.mDetTime.set(0.0, 0.0, 0.0);
  clear_sat_stats(mStats.mDetStats);
  clear_sat_stats(mStats.mDetStatsMax);

  mStats.mRedCount = 0;
  mStats.mRedTime.set(0.0, 0.0, 0.0);
  clear_sat_stats(mStats.mRedStats);
  clear_sat_stats(mStats.mRedStatsMax);

  mStats.mAbortCount = 0;
  mStats.mAbortTime.set(0.0, 0.0, 0.0);
}

// @brief 統計情報を得る．
// @param[in] stats 結果を格納する構造体
void
SatEngine::get_stats(DtpgStats& stats) const
{
  stats = mStats;
}

// @breif 時間計測を制御する．
void
SatEngine::timer_enable(bool enable)
{
  mTimerEnable = enable;
}

BEGIN_NONAMESPACE

struct Lt
{
  bool
  operator()(TpgNode* left,
	     TpgNode* right)
  {
    return left->output_id2() < right->output_id2();
  }
};

END_NONAMESPACE

// @brief 故障位置を与えてその TFO の TFI リストを作る．
// @param[in] solver SAT ソルバ
// @param[in] fnode_list 故障位置のノードのリスト
// @param[in] max_id ノード番号の最大値
//
// 結果は mTfoList に格納される．
// 故障位置の TFO が mTfoList の [0: mTfoEnd1 - 1] に格納される．
void
SatEngine::mark_region(SatSolver& solver,
		       const vector<TpgNode*>& fnode_list,
		       ymuint max_id)
{
  mMarkArray.clear();
  mMarkArray.resize(max_id, 0U);

  mTfoList.clear();
  mTfoList.reserve(max_id);

  mInputList.clear();
  mOutputList.clear();

  // 故障に一時的なID番号を割り振る．
  // 故障のあるノードの TFO を mTfoList に入れる．
  // TFO の TFI のノードを mTfiList に入れる．
  ymuint nf = fnode_list.size();
  for (ymuint i = 0; i < nf; ++ i) {
    TpgNode* fnode = fnode_list[i];
    if ( !tfo_mark(fnode) ) {
      set_tfo_mark(fnode);
      if ( fnode->is_input() ) {
	mInputList.push_back(fnode);
      }
      set_gvar(solver, fnode);
      set_fvar(solver, fnode);
    }
  }

  for (ymuint rpos = 0; rpos < mTfoList.size(); ++ rpos) {
    TpgNode* node = mTfoList[rpos];
    ymuint nfo = node->active_fanout_num();
    for (ymuint i = 0; i < nfo; ++ i) {
      TpgNode* fonode = node->active_fanout(i);
      if ( !tfo_mark(fonode) ) {
	set_tfo_mark(fonode);
	set_gvar(solver, fonode);
	set_fvar(solver, fonode);
      }
    }
  }

  mTfoEnd = mTfoList.size();
  for (ymuint rpos = 0; rpos < mTfoList.size(); ++ rpos) {
    TpgNode* node = mTfoList[rpos];
    ymuint ni = node->fanin_num();
    for (ymuint i = 0; i < ni; ++ i) {
      TpgNode* finode = node->fanin(i);
      if ( !tfo_mark(finode) && !tfi_mark(finode) ) {
	set_tfi_mark(finode);
	set_gvar(solver, finode);
      }
    }
  }

  sort(mOutputList.begin(), mOutputList.end(), Lt());
}

// @brief 節の作成用の作業領域の使用を開始する．
// @param[in] exp_size 予想されるサイズ
void
SatEngine::tmp_lits_begin(ymuint exp_size)
{
  mTmpLits.clear();
  if ( exp_size > 0 ) {
    mTmpLits.reserve(exp_size);
  }
}

// @brief 作業領域にリテラルを追加する．
void
SatEngine::tmp_lits_add(Literal lit)
{
  mTmpLits.push_back(lit);
}

// @brief 作業領域の冊を SAT ソルバに加える．
void
SatEngine::tmp_lits_end(SatSolver& solver)
{
  solver.add_clause(mTmpLits);
}

// @brief タイマーをスタートする．
void
SatEngine::cnf_begin()
{
  if ( mTimerEnable ) {
    mTimer.reset();
    mTimer.start();
  }
}

// @brief タイマーを止めて CNF 作成時間に加える．
void
SatEngine::cnf_end()
{
  if ( mTimerEnable ) {
    mTimer.stop();
    mStats.mCnfGenTime += mTimer.time();
  }
  ++ mStats.mCnfGenCount;
}

// @brief 正常回路のノードの入出力の関係を表す CNF を作る．
// @param[in] solver SATソルバ
// @param[in] node 対象のノード
void
SatEngine::make_gnode_cnf(SatSolver& solver,
			  TpgNode* node)
{
  make_node_cnf(solver, node, GvarLitMap(node));
}

// @brief 故障回路のノードの入出力の関係を表す CNF を作る．
// @param[in] solver SATソルバ
// @param[in] node 対象のノード
void
SatEngine::make_fnode_cnf(SatSolver& solver,
			  TpgNode* node)
{
  make_node_cnf(solver, node, FvarLitMap(node));
}

// @brief 故障回路のノードの入出力の関係を表す CNF を作る．
// @param[in] solver SATソルバ
// @param[in] node 対象のノード
void
SatEngine::make_fnode_cnf(SatSolver& solver,
			  TpgNode* node,
			  VarId ovar)
{
  make_node_cnf(solver, node, Fvar2LitMap(node, ovar));
}

// @brief ノードの入出力の関係を表す CNF を作る．
// @param[in] solver SATソルバ
// @param[in] node 対象のノード
// @param[in] litmap 入出力のリテラルを保持するクラス
void
SatEngine::make_node_cnf(SatSolver& solver,
			 TpgNode* node,
			 const LitMap& litmap)
{
  if ( node->is_input() ) {
    return;
  }

  Literal output = litmap.output();

  if ( node->is_output() ) {
    Literal input = litmap.input(0);
    solver.add_clause( input, ~output);
    solver.add_clause(~input,  output);
    return;
  }

  ymuint ni = node->fanin_num();
  switch ( node->gate_type() ) {
  case kTgGateBuff:
    solver.add_clause( litmap.input(0), ~output);
    solver.add_clause(~litmap.input(0),  output);
    break;

  case kTgGateNot:
    solver.add_clause( litmap.input(0),  output);
    solver.add_clause(~litmap.input(0), ~output);
    break;

  case kTgGateAnd:
    switch ( ni ) {
    case 2:
      solver.add_clause(~litmap.input(0), ~litmap.input(1), output);
      break;

    case 3:
      solver.add_clause(~litmap.input(0), ~litmap.input(1), ~litmap.input(2), output);
      break;

    case 4:
      solver.add_clause(~litmap.input(0), ~litmap.input(1), ~litmap.input(2), ~litmap.input(3), output);
      break;

    default:
      {
	vector<Literal> tmp(ni + 1);
	for (ymuint i = 0; i < ni; ++ i) {
	  tmp[i] = ~litmap.input(i);
	}
	tmp[ni] = output;
	solver.add_clause(tmp);
      }
      break;
    }
    for (ymuint i = 0; i < ni; ++ i) {
      solver.add_clause(litmap.input(i), ~output);
    }
    break;

  case kTgGateNand:
    switch ( ni ) {
    case 2:
      solver.add_clause(~litmap.input(0), ~litmap.input(1), ~output);
      break;

    case 3:
      solver.add_clause(~litmap.input(0), ~litmap.input(1), ~litmap.input(2), ~output);
      break;

    case 4:
      solver.add_clause(~litmap.input(0), ~litmap.input(1), ~litmap.input(2), ~litmap.input(3), ~output);
      break;

    default:
      {
	vector<Literal> tmp(ni + 1);
	for (ymuint i = 0; i < ni; ++ i) {
	  tmp[i] = ~litmap.input(i);
	}
	tmp[ni] = ~output;
	solver.add_clause(tmp);
      }
      break;
    }
    for (ymuint i = 0; i < ni; ++ i) {
      solver.add_clause(litmap.input(i), output);
    }
    break;

  case kTgGateOr:
    switch ( ni ) {
    case 2:
      solver.add_clause(litmap.input(0), litmap.input(1), ~output);
      break;

    case 3:
      solver.add_clause(litmap.input(0), litmap.input(1), litmap.input(2), ~output);
      break;

    case 4:
      solver.add_clause(litmap.input(0), litmap.input(1), litmap.input(2), litmap.input(3), ~output);
      break;

    default:
      {
	vector<Literal> tmp(ni + 1);
	for (ymuint i = 0; i < ni; ++ i) {
	  tmp[i] = litmap.input(i);
	}
	tmp[ni] = ~output;
	solver.add_clause(tmp);
      }
      break;
    }
    for (ymuint i = 0; i < ni; ++ i) {
      solver.add_clause(~litmap.input(i), output);
    }
    break;

  case kTgGateNor:
    switch ( ni ) {
    case 2:
      solver.add_clause(litmap.input(0), litmap.input(1), output);
      break;

    case 3:
      solver.add_clause(litmap.input(0), litmap.input(1), litmap.input(2), output);
      break;

    case 4:
      solver.add_clause(litmap.input(0), litmap.input(1), litmap.input(2), litmap.input(3), output);
      break;

    default:
      {
	vector<Literal> tmp(ni + 1);
	for (ymuint i = 0; i < ni; ++ i) {
	  tmp[i] = litmap.input(i);
	}
	tmp[ni] = output;
	solver.add_clause(tmp);
      }
      break;
    }
    for (ymuint i = 0; i < ni; ++ i) {
      solver.add_clause(~litmap.input(i), ~output);
    }
    break;

  case kTgGateXor:
    if ( ni == 2 ) {
      solver.add_clause(~litmap.input(0),  litmap.input(1),  output);
      solver.add_clause( litmap.input(0), ~litmap.input(1),  output);
      solver.add_clause( litmap.input(0),  litmap.input(1), ~output);
      solver.add_clause(~litmap.input(0), ~litmap.input(1), ~output);
    }
    else {
      vector<Literal> tmp(ni + 1);
      ymuint nip = (1U << ni);
      for (ymuint p = 0; p < nip; ++ p) {
	ymuint c = 0;
	for (ymuint i = 0; i < ni; ++ i) {
	  if ( p & (1U << i) ) {
	    tmp[i] = litmap.input(i);
	  }
	  else {
	    tmp[i] = ~litmap.input(i);
	    ++ c;
	  }
	}
	if ( (c % 2) == 0 ) {
	  tmp[ni] = ~output;
	}
	else {
	  tmp[ni] = output;
	}
	solver.add_clause(tmp);
      }
    }
    break;

  case kTgGateXnor:
    if ( ni == 2 ) {
      solver.add_clause(~litmap.input(0),  litmap.input(1), ~output);
      solver.add_clause( litmap.input(0), ~litmap.input(1), ~output);
      solver.add_clause( litmap.input(0),  litmap.input(1),  output);
      solver.add_clause(~litmap.input(0), ~litmap.input(1),  output);
    }
    else {
      vector<Literal> tmp(ni + 1);
      ymuint nip = (1U << ni);
      for (ymuint p = 0; p < nip; ++ p) {
	ymuint c = 0;
	for (ymuint i = 0; i < ni; ++ i) {
	  if ( p & (1U << i) ) {
	    tmp[i] = litmap.input(i);
	  }
	  else {
	    tmp[i] = ~litmap.input(i);
	    ++ c;
	  }
	}
	if ( (c % 2) == 0 ) {
	  tmp[ni] = output;
	}
	else {
	  tmp[ni] = ~output;
	}
	solver.add_clause(tmp);
      }
    }
    break;

  default:
    assert_not_reached(__FILE__, __LINE__);
    break;
  }
}

// @brief ノードの故障差関数を表すCNFを作る．
void
SatEngine::make_dlit_cnf(SatSolver& solver,
			 TpgNode* node)
{
  if ( node->is_output() ) {
    return;
  }

  Literal dlit(node->dvar());

  // ゲートの種類ごとの処理
  ymuint ni = node->fanin_num();
  switch ( node->gate_type() ) {
  case kTgGateBuff:
  case kTgGateNot:
    // なにもしない．
    break;

  case kTgGateAnd:
  case kTgGateNand:
    for (ymuint i = 0; i < ni; ++ i) {
      TpgNode* inode = node->fanin(i);
      if ( inode->has_fvar() ) {
#if 0
	// side input が 0 かつ故障差が伝搬していなければ dlit も 0
	Literal iglit(inode->gvar(), false);
	Literal idlit(inode->dvar(), false);
	solver.add_clause(iglit, idlit, ~dlit);
#endif
      }
      else {
	// side input が 0 なら dlit も 0
	Literal iglit(inode->gvar(), false);
	solver.add_clause(iglit, ~dlit);
      }
    }
    break;

  case kTgGateOr:
  case kTgGateNor:
    for (ymuint i = 0; i < ni; ++ i) {
      TpgNode* inode = node->fanin(i);
      if ( inode->has_fvar() ) {
#if 0
	// side input が 1 かつ故障差が伝搬していなければ dlit も 0
	Literal iglit(inode->gvar(), false);
	Literal idlit(inode->dvar(), false);
	solver.add_clause(~iglit, idlit, ~dlit);
#endif
      }
      else {
	// side input が 1 なら dlit も 0
	Literal iglit(inode->gvar(), false);
	solver.add_clause(~iglit, ~dlit);
      }
    }
    break;

  case kTgGateXor:
  case kTgGateXnor:
    // なにもしない．
    break;
  }

  // 全ゲートタイプ共通
  // 全てのファンインに故障差が伝搬していなければ
  // このゲートの出力にも故障差は伝搬しない．
  tmp_lits_begin(ni + 1);
  tmp_lits_add(~dlit);
  for (ymuint i = 0; i < ni; ++ i) {
    TpgNode* inode = node->fanin(i);
    if ( inode->has_fvar() ) {
      Literal idlit(inode->dvar(), false);
      tmp_lits_add(idlit);
    }
  }
  tmp_lits_end(solver);
}

// @brief ノードの故障差関数を表すCNFを作る．
void
SatEngine::make_dlit_cnf(SatSolver& solver,
			 TpgNode* node,
			 const vector<TpgNode*>& fnode_list,
			 const vector<VarId>& flt_var)
{
  if ( node->is_output() ) {
    return;
  }

  ymuint nf = fnode_list.size();

  Literal dlit(node->dvar());

  // ゲートの種類ごとの処理
  ymuint ni = node->fanin_num();
  switch ( node->gate_type() ) {
  case kTgGateBuff:
  case kTgGateNot:
    // なにもしない．
    break;

  case kTgGateAnd:
  case kTgGateNand:
    for (ymuint i = 0; i < ni; ++ i) {
      TpgNode* inode = node->fanin(i);
      if ( inode->has_fvar() ) {
	// side input が 0 かつ故障差が伝搬していなければ dlit も 0
	Literal iglit(inode->gvar(), false);
	Literal idlit(inode->dvar(), false);

	bool found = false;
	for (ymuint fid = 0; fid < nf; ++ fid) {
	  if ( fnode_list[fid] == inode ) {
	    Literal flit(flt_var[fid], false);
#if 0
	    solver.add_clause(flit, iglit, idlit, ~dlit);
#endif
	    found = true;
	    break;
	  }
	}
	if ( !found ) {
	  solver.add_clause(iglit, idlit, ~dlit);
	}
      }
      else {
	// side input が 0 なら dlit も 0
	Literal iglit(inode->gvar(), false);
	solver.add_clause(iglit, ~dlit);
      }
    }
    break;

  case kTgGateOr:
  case kTgGateNor:
    for (ymuint i = 0; i < ni; ++ i) {
      TpgNode* inode = node->fanin(i);
      if ( inode->has_fvar() ) {
	// side input が 1 かつ故障差が伝搬していなければ dlit も 0
	Literal iglit(inode->gvar(), false);
	Literal idlit(inode->dvar(), false);

	bool found = false;
	for (ymuint fid = 0; fid < nf; ++ fid) {
	  if ( fnode_list[fid] == inode ) {
	    Literal flit(flt_var[fid], false);
#if 0
	    solver.add_clause(flit, ~iglit, idlit, ~dlit);
#endif
	    found = true;
	    break;
	  }
	}
	if ( !found ) {
	  solver.add_clause(~iglit, idlit, ~dlit);
	}
      }
      else {
	// side input が 1 なら dlit も 0
	Literal iglit(inode->gvar(), false);
	solver.add_clause(~iglit, ~dlit);
      }
    }
    break;

  case kTgGateXor:
  case kTgGateXnor:
    // なにもしない．
    break;
  }

  // 全てのファンインに故障差が伝搬していなければ
  // このゲートの出力にも故障差は伝搬しない．
  // ただし，このゲートの出力に故障がある場合を
  // 考慮しなければならない．
  tmp_lits_begin(ni + 1);
  tmp_lits_add(~dlit);
  for (ymuint i = 0; i < ni; ++ i) {
    TpgNode* inode = node->fanin(i);
    if ( inode->has_fvar() ) {
      Literal idlit(inode->dvar(), false);
      tmp_lits_add(idlit);
    }
  }

  for (ymuint fid = 0; fid < nf; ++ fid) {
    if ( fnode_list[fid] == node ) {
      tmp_lits_add(Literal(flt_var[fid], false));
    }
  }

  tmp_lits_end(solver);
}

// @brief 一つの SAT問題を解く．
Bool3
SatEngine::solve(SatSolver& solver,
		 TpgFault* f,
		 BackTracer& bt,
		 DetectOp& dop,
		 UntestOp& uop)
{
  SatStats prev_stats;
  solver.get_stats(prev_stats);

  USTime time(0, 0, 0);
  Bool3 ans = _solve(solver, time);

  SatStats sat_stats;
  solver.get_stats(sat_stats);

  sub_sat_stats(sat_stats, prev_stats);

  if ( ans == kB3True ) {
    // パタンが求まった．

    // バックトレースを行う．
    TestVector* tv = bt(f->node(), mModel, mInputList, mOutputList);

    // パタンの登録などを行う．
    dop(f, tv);

    stats_detect(sat_stats, time);
  }
  else if ( ans == kB3False ) {
    // 検出不能と判定された．
    uop(f);

    stats_undetect(sat_stats, time);
  }
  else { // ans == kB3X つまりアボート
    stats_abort(sat_stats, time);
  }

  return ans;
}

// @brief 一つの SAT問題を解く．
Bool3
SatEngine::_solve(SatSolver& solver,
		  USTime& time)
{
  if ( mTimerEnable ) {
    mTimer.reset();
    mTimer.start();
  }

  Bool3 ans = solver.solve(mTmpLits, mModel);

  if ( mTimerEnable ) {
    mTimer.stop();
    time += mTimer.time();
  }

  return ans;
}

// @brief 検出した場合の処理
void
SatEngine::stats_detect(const SatStats& sat_stats,
			const USTime& time)
{
  ++ mStats.mDetCount;
  mStats.mDetTime += time;
  add_sat_stats(mStats.mDetStats, sat_stats);
  max_sat_stats(mStats.mDetStatsMax, sat_stats);
}

// @brief 検出不能と判定した場合の処理
void
SatEngine::stats_undetect(const SatStats& sat_stats,
			  const USTime& time)
{
  ++ mStats.mRedCount;
  mStats.mRedTime += time;
  add_sat_stats(mStats.mRedStats, sat_stats);
  max_sat_stats(mStats.mRedStatsMax, sat_stats);
}

// @brief アボートした場合の処理
void
SatEngine::stats_abort(const SatStats& sat_stats,
		       const USTime& time)
{
  ++ mStats.mAbortCount;
  mStats.mAbortTime += time;
}

// SatStats をクリアする．
void
SatEngine::clear_sat_stats(SatStats& stats)
{
  stats.mRestart = 0;
  stats.mVarNum = 0;
  stats.mConstrClauseNum = 0;
  stats.mConstrLitNum = 0;
  stats.mLearntClauseNum = 0;
  stats.mLearntLitNum = 0;
  stats.mConflictNum = 0;
  stats.mDecisionNum = 0;
  stats.mPropagationNum = 0;
}

// SatStats をたす．
void
SatEngine::add_sat_stats(SatStats& dst_stats,
			 const SatStats& src_stats)
{
  dst_stats.mRestart += src_stats.mRestart;
  dst_stats.mVarNum += src_stats.mVarNum;
  dst_stats.mConstrClauseNum += src_stats.mConstrClauseNum;
  dst_stats.mConstrLitNum += src_stats.mConstrLitNum;
  dst_stats.mLearntClauseNum += src_stats.mLearntClauseNum;
  dst_stats.mLearntLitNum += src_stats.mLearntLitNum;
  dst_stats.mConflictNum += src_stats.mConflictNum;
  dst_stats.mDecisionNum += src_stats.mDecisionNum;
  dst_stats.mPropagationNum += src_stats.mPropagationNum;
}

// SatStats を引く
void
SatEngine::sub_sat_stats(SatStats& dst_stats,
			 const SatStats& src_stats)
{
  dst_stats.mRestart -= src_stats.mRestart;
  dst_stats.mVarNum -= src_stats.mVarNum;
  dst_stats.mConstrClauseNum -= src_stats.mConstrClauseNum;
  dst_stats.mConstrLitNum -= src_stats.mConstrLitNum;
  dst_stats.mLearntClauseNum -= src_stats.mLearntClauseNum;
  dst_stats.mLearntLitNum -= src_stats.mLearntLitNum;
  dst_stats.mConflictNum -= src_stats.mConflictNum;
  dst_stats.mDecisionNum -= src_stats.mDecisionNum;
  dst_stats.mPropagationNum -= src_stats.mPropagationNum;
}

// SatStats の各々の最大値をとる．
void
SatEngine::max_sat_stats(SatStats& dst_stats,
			 const SatStats& src_stats)
{
  if ( dst_stats.mRestart < src_stats.mRestart ) {
    dst_stats.mRestart = src_stats.mRestart;
  }
  if ( dst_stats.mVarNum < src_stats.mVarNum ) {
    dst_stats.mVarNum += src_stats.mVarNum;
  }
  if ( dst_stats.mConstrClauseNum < src_stats.mConstrClauseNum ) {
    dst_stats.mConstrClauseNum += src_stats.mConstrClauseNum;
  }
  if ( dst_stats.mConstrLitNum < src_stats.mConstrLitNum ) {
    dst_stats.mConstrLitNum += src_stats.mConstrLitNum;
  }
  if ( dst_stats.mLearntClauseNum < src_stats.mLearntClauseNum ) {
    dst_stats.mLearntClauseNum += src_stats.mLearntClauseNum;
  }
  if ( dst_stats.mLearntLitNum < src_stats.mLearntLitNum ) {
    dst_stats.mLearntLitNum += src_stats.mLearntLitNum;
  }
  if ( dst_stats.mConflictNum < src_stats.mConflictNum ) {
    dst_stats.mConflictNum += src_stats.mConflictNum;
  }
  if ( dst_stats.mDecisionNum < src_stats.mDecisionNum ) {
    dst_stats.mDecisionNum += src_stats.mDecisionNum;
  }
  if ( dst_stats.mPropagationNum < src_stats.mPropagationNum ) {
    dst_stats.mPropagationNum += src_stats.mPropagationNum;
  }
}

// @brief ノードの変数割り当てフラグを消す．
void
SatEngine::clear_node_mark()
{
  for (vector<TpgNode*>::iterator p = mTfoList.begin();
       p != mTfoList.end(); ++ p) {
    TpgNode* node = *p;
    node->clear_var();
  }
}

END_NAMESPACE_YM_SATPG
