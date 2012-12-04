#ifndef SRC_DTPG_DTPGCMD_H
#define SRC_DTPG_DTPGCMD_H

/// @file src/atpg_tclsh/DtpgCmd.h
/// @brief DtpgCmd のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2005-2010, 2012 Yusuke Matsunaga
/// All rights reserved.


#include "AtpgCmd.h"


BEGIN_NAMESPACE_YM_SATPG

//////////////////////////////////////////////////////////////////////
// テストパタン生成を行うコマンド
//////////////////////////////////////////////////////////////////////
class DtpgCmd :
  public AtpgCmd
{
public:

  /// @brief コンストラクタ
  DtpgCmd(AtpgMgr* mgr);

  /// @brief デストラクタ
  virtual
  ~DtpgCmd();


protected:

  /// @brief コマンド処理関数
  virtual
  int
  cmd_proc(TclObjVector& objv);


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // verbose オプションの解析用オブジェクト
  TclPoptInt* mPoptVerbose;

  // sat モードオプションの解析用オブジェクト
  TclPopt* mPoptSat;

  // satrec モードオプションの解析様オブジェクト
  TclPopt* mPoptSatRec;

  // minisat モードオプションの解析様オブジェクト
  TclPopt* mPoptMiniSat;

  // fsim モードオプションの解析用オブジェクト
  TclPoptStr* mPoptFsim;

  // print_stats オプションの解析用オブジェクト
  TclPopt* mPoptPrintStats;

};

END_NAMESPACE_YM_SATPG

#endif // SRC_DTPG_DTPGCMD_H
