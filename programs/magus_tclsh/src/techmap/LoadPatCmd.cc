
/// @file magus/techmap/LoadPatCmd.cc
/// @brief LoadPatCmd の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// $Id: LoadPatCmd.cc 2274 2009-06-10 07:45:29Z matsunaga $
///
/// Copyright (C) 2005-2010 Yusuke Matsunaga
/// All rights reserved.


#include "LoadPatCmd.h"
#include "ym_tclpp/TclPopt.h"


BEGIN_NAMESPACE_MAGUS_TECHMAP

//////////////////////////////////////////////////////////////////////
// load_pat コマンド
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
LoadPatCmd::LoadPatCmd(MagMgr* mgr,
		       TechmapData* data) :
  TechmapCmd(mgr, data)
{
  set_usage_string("<pat-file>");
}

// @brief デストラクタ
LoadPatCmd::~LoadPatCmd()
{
}

// @brief コマンドを実行する仮想関数
int
LoadPatCmd::cmd_proc(TclObjVector& objv)
{
  ymuint objc = objv.size();
  if ( objc != 2 ) {
    print_usage();
    return TCL_ERROR;
  }

  // コマンド行の引数からファイル名をとってくる．
  string filename = objv[1];

  // ファイル名の展開を行う．
  string ex_filename;
  bool stat = tilde_subst(filename, ex_filename);
  if ( !stat ) {
    // ファイル名文字列の中に誤りがあった．
    return TCL_ERROR;
  }

  ifstream is;
  is.open(ex_filename.c_str(), ios::binary);
  if ( !is ) {
    TclObj emsg;
    emsg << "Could not open " << filename;
    set_result(emsg);
    return TCL_ERROR;
  }
  if ( !techmap().load_library(is) ) {
    TclObj emsg;
    emsg << "Error occured in reading " << filename;
    set_result(emsg);
    return TCL_ERROR;
  }

  return TCL_OK;
}

END_NAMESPACE_MAGUS_TECHMAP
