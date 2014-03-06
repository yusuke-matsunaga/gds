
/// @file Verifier.cc
/// @brief Verifier の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2014 Yusuke Matsunaga
/// All rights reserved.


#include "Verifier.h"


BEGIN_NAMESPACE_YM_SATPG


//////////////////////////////////////////////////////////////////////
// クラス Fop1Ver
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
// @param[in] fsim 故障シミュレータ
Fop1Ver::Fop1Ver(Fsim& fsim) :
  mFsim(fsim)
{
}

// @brief デストラクタ
Fop1Ver::~Fop1Ver()
{
}

// @brief 故障を検出したときの処理
// @param[in] f 故障
void
Fop1Ver::operator()(TpgFault* f)
{
  mFsim.set_skip(f);
  mDetSet.insert(f);
}

// @brief det_flag を下ろす．
void
Fop1Ver::clear_det_flag()
{
  mFsim.clear_skip();
  mDetSet.clear();
}

// @brief 故障が見つかったら true を返す．
bool
Fop1Ver::is_detected(TpgFault* f)
{
  return mDetSet.count(f) > 0;
}


//////////////////////////////////////////////////////////////////////
// クラス Verifier
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
// @param[in] fault_mgr 故障マネージャ
// @param[in] fsim 故障シミュレータ
Verifier::Verifier(FaultMgr& fault_mgr,
		   Fsim& fsim) :
  mFaultMgr(fault_mgr),
  mFsim(fsim),
  mOp(fsim)
{
}

// @brief デストラクタ
Verifier::~Verifier()
{
}

// @brief 与えられたパタンリストで全検出済み故障を検出できるか調べる．
// @param[in] pat_list パタンのリスト
bool
Verifier::check(const vector<TestVector*>& pat_list)
{
  vector<FsimOp1*> op_list(1, &mOp);
  mOp.clear_det_flag();
  mFsim.clear_skip();
  for (vector<TestVector*>::const_iterator p = pat_list.begin();
       p != pat_list.end(); ++ p) {
    TestVector* tv = *p;
    mFsim.sppfp(tv, op_list);
  }

  bool no_error = true;
  const vector<TpgFault*>& f_list = mFaultMgr.det_list();
  for (vector<TpgFault*>::const_iterator p = f_list.begin();
       p != f_list.end(); ++ p) {
    TpgFault* f = *p;
    if ( !mOp.is_detected(f) ) {
      cout << "Error: " << f << " has no patterns" << endl;
      no_error = false;
    }
  }

  return no_error;
}

END_NAMESPACE_YM_SATPG