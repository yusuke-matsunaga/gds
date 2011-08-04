#ifndef PARSETREEVIEW_H
#define PARSETREEVIEW_H

/// @file libym_cell/tests/dotlib_ptview/ParseTreeView.h
/// @brief ParseTreeView のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2005-2011 Yusuke Matsunaga
/// All rights reserved.


#include <QTreeView>
#include "dotlib/dotlib_nsdef.h"
#include "ParseTreeModel.h"

class QItemSelection;

//////////////////////////////////////////////////////////////////////
/// @class ParseTreeView ParseTreeView.h
/// @brief verilog のパース木を表示するウィジェット
//////////////////////////////////////////////////////////////////////
class ParseTreeView :
  public QTreeView
{
  Q_OBJECT

public:

  /// @brief コンストラクタ
  /// @param[in] parent 親のウィジェット
  ParseTreeView(QWidget* parent = NULL);

  /// @brief デストラクタ
  ~ParseTreeView();


public:

  /// @brief モデルをセットする．
  void
  setModel(nsYm::nsCell::nsDotlib::ParseTreeModel* model);


signals:

  /// @brief トークンが選択されたときに発生するシグナル．
  /// @param[in] start_line 開始位置の行番号
  /// @param[in] start_column 開始位置のコラム番号
  /// @param[in] end_line 終了位置の行番号
  /// @param[in] end_column 終了位置のコラム番号
  void
  select_token(int start_line,
	       int start_column,
	       int end_line,
	       int end_column);


protected:

  /// @brief selection が変化したときに呼ばれる関数
  virtual
  void
  selectionChanged(const QItemSelection& selected,
		   const QItemSelection& deselected);


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // 内容を表すモデル．
  nsYm::nsCell::nsDotlib::ParseTreeModel* mModel;

};

#endif // PARSETREEVIEW_H
