
/// @file SatEngineMulti2.cc
/// @brief SatEngineMulti2 の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2005-2010, 2012-2014 Yusuke Matsunaga
/// All rights reserved.


#include "SatEngineMulti2.h"

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

// @brief コンストラクタ
SatEngineMulti2::SatEngineMulti2()
{
  mTgGrasp = true;
  mExtTgGrasp = true;
  mUseDominator = true;
  mUseLocalDominator = false;
  mSkipThreshold = 3;
}

// @brief デストラクタ
SatEngineMulti2::~SatEngineMulti2()
{
}

// @brief オプションを設定する．
void
SatEngineMulti2::set_option(const string& option_str)
{
  for (string::size_type next = 0; ; ++ next) {
    string::size_type pos = option_str.find(':', next);
    if ( pos == next ) {
      continue;
    }
    string option = option_str.substr(next, pos - next);
    if ( option == "TG-GRASP" ) {
      mTgGrasp = true;
      mExtTgGrasp = false;
    }
    else if ( option == "EXT-TG-GRASP" ) {
      mTgGrasp = true;
      mExtTgGrasp = true;
    }
    else if ( option == "NEMESIS" ) {
      mTgGrasp = false;
    }
    else if ( option == "DOM" ) {
      mUseLocalDominator = false;
    }
    else if ( option == "NODOM" ) {
      mUseDominator = false;
    }
    else if ( option == "LOCALDOM" ) {
      mUseLocalDominator = true;
    }
    if ( pos == string::npos ) {
      break;
    }
    next = pos;
  }
}

BEGIN_NONAMESPACE

struct FnodeInfo
{
  FnodeInfo(ymuint ni) :
    mO0(0),
    mO1(0),
    mI0(ni, 0),
    mI1(ni, 0)
  {
  }

  ymuint32 mO0;
  ymuint32 mO1;

  vector<ymuint32> mI0;
  vector<ymuint32> mI1;
};

END_NONAMESPACE

// @brief テストパタン生成を行なう．
// @param[in] flist 故障リスト
// @param[in] max_id ノード番号の最大値 + 1
void
SatEngineMulti2::run(const vector<TpgFault*>& flist,
		     ymuint max_id,
		     BackTracer& bt,
		     DetectOp& dop,
		     UntestOp& uop)
{
  SatSolver solver(sat_type(), sat_option(), sat_outp());

  bt.set_max_id(max_id);

  ymuint nf = flist.size();

  vector<FnodeInfo*> fnode_info(max_id, NULL);
  vector<TpgNode*> fnode_list;
  fnode_list.reserve(nf);
  for (ymuint i = 0; i < nf; ++ i) {
    TpgFault* f = flist[i];
    int fval = f->val();
    TpgNode* node = f->node();
    if ( fnode_info[node->id()] == NULL ) {
      fnode_info[node->id()] = new FnodeInfo(node->fanin_num());
      fnode_list.push_back(node);
    }
    FnodeInfo* finfo = fnode_info[node->id()];
    if ( f->is_output_fault() ) {
      if ( fval == 0 ) {
	finfo->mO0 = i + 1;
      }
      else {
	finfo->mO1 = i + 1;
      }
    }
    else {
      ymuint pos = f->pos();
      if ( fval == 0 ) {
	finfo->mI0[pos] = i + 1;
      }
      else {
	finfo->mI1[pos] = i + 1;
      }
    }
  }

  // 故障を活性化するとき true にする変数．
  vector<VarId> flt_var(nf);
  for (ymuint i = 0; i < nf; ++ i) {
    flt_var[i] = solver.new_var();
    TpgFault* f = flist[i];
    f->clear_untest_num();
  }

  mark_region(solver, fnode_list, max_id);

  const vector<TpgNode*>& olist = output_list();
  ymuint no = olist.size();

  vector<ymuint> mark(max_id, 0);
  if ( no > mSkipThreshold ) {
    no = mSkipThreshold;
  }
  for (ymuint opos = 0; opos < no; ++ opos) {
    TpgNode* onode = olist[opos];
    ymuint oid = onode->output_id2();

    cnf_begin();

    //////////////////////////////////////////////////////////////////////
    // 正常回路の CNF を生成
    //////////////////////////////////////////////////////////////////////
    for (ymuint i = 0; i < tfo_tfi_size(); ++ i) {
      TpgNode* node = tfo_tfi_node(i);
      if ( node->is_in_TFI_of(oid) && mark[node->id()] == 0 ) {
	mark[node->id()] = oid + 1;

	make_gnode_cnf(solver, node);
      }
    }

    //////////////////////////////////////////////////////////////////////
    // 故障回路の CNF を生成
    //////////////////////////////////////////////////////////////////////
    for (ymuint i = 0; i < tfo_size(); ++ i) {
      TpgNode* node = tfo_tfi_node(i);
      if ( mark[node->id()] != (oid + 1) ) {
	continue;
      }

      FnodeInfo* finfo = fnode_info[node->id()];
      if ( finfo != NULL ) {
	ymuint ni = node->fanin_num();
	vector<VarId> ivars(ni);
	for (ymuint i = 0; i < ni; ++ i) {
	  TpgNode* inode = node->fanin(i);
	  ymuint32 f0_id = finfo->mI0[i];
	  ymuint32 f1_id = finfo->mI1[i];
	  if ( f0_id == 0 ) {
	    if ( f1_id == 0 ) {
	      ivars[i] = inode->fvar();
	    }
	    else {
	      VarId tmp_var = solver.new_var();
	      make_flt1_cnf(solver, inode->fvar(), flt_var[f1_id - 1], tmp_var);
	      ivars[i] = tmp_var;
	    }
	  }
	  else {
	    if ( f1_id == 0 ) {
	      VarId tmp_var = solver.new_var();
	      make_flt0_cnf(solver, inode->fvar(), flt_var[f0_id - 1], tmp_var);
	      ivars[i] = tmp_var;
	    }
	    else {
	      VarId tmp_var = solver.new_var();
	      make_flt01_cnf(solver, inode->fvar(), flt_var[f0_id - 1], flt_var[f1_id - 1], tmp_var);
	      ivars[i] = tmp_var;
	    }
	  }
	}

	ymuint32 f0_id = finfo->mO0;
	ymuint32 f1_id = finfo->mO1;
	VarId ovar = node->fvar();
	if ( f0_id == 0 ) {
	  if ( f1_id == 0 ) {
	    ;
	  }
	  else {
	    ovar = solver.new_var();
	    make_flt1_cnf(solver, ovar, flt_var[f1_id - 1], node->fvar());
	  }
	}
	else {
	  if ( f1_id == 0 ) {
	    ovar = solver.new_var();
	    make_flt0_cnf(solver, ovar, flt_var[f0_id - 1], node->fvar());
	  }
	  else {
	    ovar = solver.new_var();
	    make_flt01_cnf(solver, ovar, flt_var[f0_id - 1], flt_var[f1_id - 1], node->fvar());
	  }
	}
	make_node_cnf(solver, node, Fvar3LitMap(ivars, ovar));
      }
      else {
	make_fnode_cnf(solver, node);
      }

      Literal glit(node->gvar(), false);
      Literal flit(node->fvar(), false);
      Literal dlit(node->dvar(), false);

      if ( mTgGrasp ) {
	// XOR(glit, flit, dlit) を追加する．
	// 要するに正常回路と故障回路で異なっているとき dlit が 1 となる．
	solver.add_clause(~glit, ~flit, ~dlit);
	solver.add_clause( glit,  flit, ~dlit);

	solver.add_clause(~glit,  flit,  dlit);
	solver.add_clause( glit, ~flit,  dlit);

	if ( mExtTgGrasp ) {
	  if ( finfo != NULL ) {

	    // 全てのファンインに故障差が伝搬していなければ
	    // このゲートの出力にも故障差は伝搬しない．
	    // ただし，このゲートの出力に故障がある場合を
	    // 考慮しなければならない．
	    ymuint ni = node->fanin_num();
	    tmp_lits_begin(ni * 3 + 3);
	    tmp_lits_add(~dlit);
	    if ( finfo->mO0 > 0 ) {
	      tmp_lits_add(Literal(flt_var[finfo->mO0 - 1], false));
	    }
	    if ( finfo->mO1 > 0 ) {
	      tmp_lits_add(Literal(flt_var[finfo->mO1 - 1], false));
	    }
	    for (ymuint i = 0; i < ni; ++ i) {
	      TpgNode* inode = node->fanin(i);
	      if ( inode->has_fvar() ) {
		Literal idlit(inode->dvar(), false);
		tmp_lits_add(idlit);
	      }
	      if ( finfo->mI0[i] > 0 ) {
		tmp_lits_add(Literal(flt_var[finfo->mI0[i] - 1], false));
	      }
	      if ( finfo->mI1[i] > 0 ) {
		tmp_lits_add(Literal(flt_var[finfo->mI1[i] - 1], false));
	      }
	    }
	  }
	  else {
	    make_dlit_cnf(solver, node);
	  }
	}
      }
      else {
#if 0
	// XOR(glit, flit, dlit) を追加する．
	// 要するに正常回路と故障回路で異なっているとき dlit が 1 となる．
	solver.add_clause(~glit, ~flit, ~dlit);
	solver.add_clause( glit,  flit, ~dlit);

	if ( !node->is_output() ) {
	  ymuint nfo = node->active_fanout_num();
	  tmp_lits_begin(nfo + 1);
	  tmp_lits_add(~dlit);
	  for (ymuint j = 0; j < nfo; ++ j) {
	    TpgNode* onode = node->active_fanout(j);
	    tmp_lits_add(Literal(onode->dvar(), false));
	  }
	  tmp_lits_end(solver);
	}
#endif
      }
    }

    cnf_end();

    // 個々の故障に対するテスト生成を行なう．
    for (ymuint fid = 0; fid < nf; ++ fid) {
      TpgFault* f = flist[fid];

      if ( f->status() == kFsDetected ) {
	// 他の故障のパタンで検出済みになっている場合がある．
	continue;
      }

      if ( f->untest_num() > mSkipThreshold ) {
	// 他の出力での検出不能回数がしきい値を越えたらスキップする．
	continue;
      }

      if ( mTimerEnable ) {
	mTimer.reset();
	mTimer.start();
      }

      tmp_lits_begin();

      //////////////////////////////////////////////////////////////////////
      // 故障の検出条件
      //////////////////////////////////////////////////////////////////////
      tmp_lits_add(Literal(onode->dvar(), false));

      // 該当の故障に対する変数のみ1にする．
      for (ymuint j = 0; j < nf; ++ j) {
	bool inv = (j != fid);
	tmp_lits_add(Literal(flt_var[j], inv));
      }

      TpgNode* fnode = f->node();

      // 故障ノードの TFO 以外の dlit を0にする．
      mTmpNodeList.clear();
      mTmpNodeList.reserve(tfo_tfi_size());
      set_tmp_mark(fnode);
      mTmpNodeList.push_back(fnode);
      for (ymuint rpos = 0; rpos < mTmpNodeList.size(); ++ rpos) {
	TpgNode* node = mTmpNodeList[rpos];
	ymuint nfo = node->active_fanout_num();
	for (ymuint i = 0; i < nfo; ++ i) {
	  TpgNode* fonode = node->active_fanout(i);
	  if ( !tmp_mark(fonode) ) {
	    set_tmp_mark(fonode);
	    mTmpNodeList.push_back(fonode);
	  }
	}
      }
      for (ymuint i = 0; i < tfo_tfi_size(); ++ i) {
	TpgNode* node = tfo_tfi_node(i);
	if ( node->has_fvar() && !tmp_mark(node) ) {
	  Literal dlit(node->dvar(), true);
	  tmp_lits_add(dlit);
	}
      }
      for (vector<TpgNode*>::iterator p = mTmpNodeList.begin();
	   p != mTmpNodeList.end(); ++ p) {
	TpgNode* node = *p;
	clear_tmp_mark(node);
      }
      mTmpNodeList.clear();

      // dominator ノードの dvar は1でなければならない．
      if ( mUseLocalDominator ) {
	for (TpgNode* node = f->node(); node != NULL; node = node->imm_dom(oid)) {
	  Literal dlit(node->dvar(), false);
	  tmp_lits_add(dlit);
	}
      }
      else {
	for (TpgNode* node = f->node(); node != NULL; node = node->imm_dom()) {
	  Literal dlit(node->dvar(), false);
	  tmp_lits_add(dlit);
	}
      }

      SatStats prev_stats;
      solver.get_stats(prev_stats);

      Bool3 ans = solver.solve(mTmpLits, mModel);

      SatStats sat_stats;
      solver.get_stats(sat_stats);

      sub_sat_stats(sat_stats, prev_stats);

      USTime time(0, 0, 0);
      if ( mTimerEnable ) {
	mTimer.stop();
	time = mTimer.time();
      }

      if ( ans == kB3True ) {

	// バックトレースを行う．
	TestVector* tv = bt(f->node(), mModel, mInputList, mOutputList);

	// パタンの登録などを行う．
	dop(f, tv);

	stats_detect(sat_stats, time);
      }
      else if ( ans == kB3False ) {
	if ( opos == no - 1 ) {
	  uop(f);
	}
	else {
	  f->inc_untest_num();
	}
	stats_undetect(sat_stats, time);
      }
      else {
	stats_abort(sat_stats, time);
      }
    }

    bool faults_remain = false;
    for (ymuint fid = 0; fid < nf; ++ fid) {
      TpgFault* f = flist[fid];

      if ( f->status() != kFsDetected ) {
	faults_remain = true;
	break;
      }
    }

    if ( !faults_remain ) {
      break;
    }
  }

  bool faults_remain = false;
  for (ymuint fid = 0; fid < nf; ++ fid) {
    TpgFault* f = flist[fid];

    if ( f->status() != kFsDetected ) {
      faults_remain = true;
      break;
    }
  }

  if ( faults_remain ) {
    cnf_begin();

    //////////////////////////////////////////////////////////////////////
    // 正常回路の CNF を生成
    //////////////////////////////////////////////////////////////////////
    for (ymuint i = 0; i < tfo_tfi_size(); ++ i) {
      TpgNode* node = tfo_tfi_node(i);
      if ( mark[node->id()] == 0 ) {
	make_gnode_cnf(solver, node);
      }
    }

    //////////////////////////////////////////////////////////////////////
    // 故障回路の CNF を生成
    //////////////////////////////////////////////////////////////////////
    for (ymuint i = 0; i < tfo_size(); ++ i) {
      TpgNode* node = tfo_tfi_node(i);
      if ( mark[node->id()] > 0 ) {
	continue;
      }

      FnodeInfo* finfo = fnode_info[node->id()];
      assert_cond( finfo == NULL, __FILE__, __LINE__);

      make_fnode_cnf(solver, node);

      Literal glit(node->gvar(), false);
      Literal flit(node->fvar(), false);
      Literal dlit(node->dvar(), false);

      if ( mTgGrasp ) {
	// XOR(glit, flit, dlit) を追加する．
	// 要するに正常回路と故障回路で異なっているとき dlit が 1 となる．
	solver.add_clause(~glit, ~flit, ~dlit);
	solver.add_clause( glit,  flit, ~dlit);

	solver.add_clause(~glit,  flit,  dlit);
	solver.add_clause( glit, ~flit,  dlit);

	if ( mExtTgGrasp ) {
	  make_dlit_cnf(solver, node);
	}
      }
      else {
#if 0
	// XOR(glit, flit, dlit) を追加する．
	// 要するに正常回路と故障回路で異なっているとき dlit が 1 となる．
	solver.add_clause(~glit, ~flit, ~dlit);
	solver.add_clause( glit,  flit, ~dlit);

	if ( !node->is_output() ) {
	  ymuint nfo = node->active_fanout_num();
	  tmp_lits_begin(nfo + 1);
	  tmp_lits_add(~dlit);
	  for (ymuint j = 0; j < nfo; ++ j) {
	    TpgNode* onode = node->active_fanout(j);
	    tmp_lits_add(Literal(onode->dvar(), false));
	  }
	  tmp_lits_end(solver);
	}
#endif
      }
    }

    cnf_end();

    tmp_lits_begin(output_list().size());
    for (vector<TpgNode*>::const_iterator p = output_list().begin();
	 p != output_list().end(); ++ p) {
      TpgNode* node = *p;
      Literal dlit(node->dvar(), false);
      tmp_lits_add(dlit);
    }
    tmp_lits_end(solver);

    for (ymuint fid = 0; fid < nf; ++ fid) {
      TpgFault* f = flist[fid];

      if ( f->status() == kFsDetected ) {
	continue;
      }

      tmp_lits_begin();

      // 該当の故障に対する変数のみ1にする．
      for (ymuint j = 0; j < nf; ++ j) {
	bool inv = (j != fid);
	tmp_lits_add(Literal(flt_var[j], inv));
      }

      TpgNode* fnode = f->node();

      // 故障ノードの TFO 以外の dlit を0にする．
      mTmpNodeList.clear();
      mTmpNodeList.reserve(tfo_tfi_size());
      set_tmp_mark(fnode);
      mTmpNodeList.push_back(fnode);
      for (ymuint rpos = 0; rpos < mTmpNodeList.size(); ++ rpos) {
	TpgNode* node = mTmpNodeList[rpos];
	ymuint nfo = node->active_fanout_num();
	for (ymuint i = 0; i < nfo; ++ i) {
	  TpgNode* fonode = node->active_fanout(i);
	  if ( !tmp_mark(fonode) ) {
	    set_tmp_mark(fonode);
	    mTmpNodeList.push_back(fonode);
	  }
	}
      }
      for (ymuint i = 0; i < tfo_tfi_size(); ++ i) {
	TpgNode* node = tfo_tfi_node(i);
	if ( node->has_fvar() && !tmp_mark(node) ) {
	  Literal dlit(node->dvar(), true);
	  tmp_lits_add(dlit);
	}
      }
      for (vector<TpgNode*>::iterator p = mTmpNodeList.begin();
	   p != mTmpNodeList.end(); ++ p) {
	TpgNode* node = *p;
	clear_tmp_mark(node);
      }
      mTmpNodeList.clear();

      if ( mUseDominator ) {
	// dominator ノードの dvar は1でなければならない．
	for (TpgNode* node = f->node(); node != NULL; node = node->imm_dom()) {
	  Literal dlit(node->dvar(), false);
	  tmp_lits_add(dlit);
	}
      }
      else {
	tmp_lits_add(Literal(f->node()->dvar(), false));
      }

      solve(solver, f, bt, dop, uop);
    }
  }

  clear_node_mark();

  for (ymuint i = 0; i < fnode_list.size(); ++ i) {
    delete fnode_info[i];
  }
}

END_NAMESPACE_YM_SATPG
