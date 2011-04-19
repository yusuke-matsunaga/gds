#ifndef LIBYM_CELL_DOTLIB_BUSHANDLER_H
#define LIBYM_CELL_DOTLIB_BUSHANDLER_H

/// @file libym_cell/dotlib/BusHandler.h
/// @brief BusHandler のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2005-2011 Yusuke Matsunaga
/// All rights reserved.


#include "GroupHandler.h"


BEGIN_NAMESPACE_YM_CELL_DOTLIB

//////////////////////////////////////////////////////////////////////
/// @class BusHandler BusHandler.h "BusHandler.h"
/// @brief bus グループのハンドラ
//////////////////////////////////////////////////////////////////////
class BusHandler :
  public GroupHandler
{
public:

  /// @brief コンストラクタ
  /// @param[in] parser パーサー
  BusHandler(DotLibParser& parser);

  /// @brief デストラクタ
  virtual
  ~BusHandler();


public:
  //////////////////////////////////////////////////////////////////////
  // GroupHandler の仮想関数
  //////////////////////////////////////////////////////////////////////

  /// @brief グループ名を読み込んだ時の処理
  /// @param[in] attr_name 属性名
  /// @param[in] token_list トークンのリスト
  virtual
  bool
  begin_group(const string& attr_name,
	      const vector<Token>& token_list);

  /// @brief グループ内のステートメントをすべて処理したときに呼ばれる関数
  virtual
  bool
  end_group();

};

END_NAMESPACE_YM_CELL_DOTLIB

#endif // LIBYM_CELL_DOTLIB_BUSHANDLER_H
