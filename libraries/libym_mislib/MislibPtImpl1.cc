
/// @file libym_cell/mislib/MislibPtImpl1.cc
/// @brief MislibPtImpl1 の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// $Id: MislibPtImpl1.cc 2507 2009-10-17 16:24:02Z matsunaga $
///
/// Copyright (C) 2005-2011 Yusuke Matsunaga
/// All rights reserved.


#include "MislibPtImpl1.h"


BEGIN_NAMESPACE_YM_CELL_MISLIB

//////////////////////////////////////////////////////////////////////
// トークンを表す基底クラス
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
// @param[in] loc 位置情報
MislibPt::MislibPt(const FileRegion& loc) :
  mLoc(loc)
{
}

// デストラクタ
MislibPt::~MislibPt()
{
}

// @brief 論理式を表す型のときに true を返す．
// @note 定数0と定数1は false を返す．
// @note デフォルトでは false を返す．
bool
MislibPt::is_expr() const
{
  return false;
}

// 文字列を取り出す
// デフォルトでは空文字列を返す．
ShString
MislibPt::str() const
{
  return ShString(static_cast<const char*>(0));
}

// 数値を取り出す
// デフォルトでは 0.0 を返す．
double
MislibPt::num() const
{
  return 0.0;
}

// 1番目の子供を取り出す．
// デフォルトでは NULL を返す．
const MislibPt*
MislibPt::child1() const
{
  return NULL;
}

// 2番目の子供を取り出す．
// デフォルトでは NULL を返す．
const MislibPt*
MislibPt::child2() const
{
  return NULL;
}

// @brief 対応する論理式を生成する．
// @param[in] name_map 端子名をキーにして端子番号を取り出す連想配列
LogExpr
MislibPt::to_expr(const hash_map<ShString, ymuint>& name_map) const
{
  // デフォルトではエラーとなる．
  assert_not_reached(__FILE__, __LINE__);
  return LogExpr();
}

// ピン名を表すオブジェクトを取り出す．
// デフォルトでは NULL を返す．
MislibPt*
MislibPt::name() const
{
  return NULL;
}

// 極性情報を表すオブジェクトを取り出す．
// デフォルトでは NULL を返す．
MislibPt*
MislibPt::phase() const
{
  return NULL;
}

// 入力負荷を表すオブジェクトを取り出す．
// デフォルトでは NULL を返す．
MislibPt*
MislibPt::input_load() const
{
  return NULL;
}

// 最大駆動負荷を表すオブジェクトを取り出す．
// デフォルトでは NULL を返す．
MislibPt*
MislibPt::max_load() const
{
  return NULL;
}

// 立ち上がり固定遅延を表すオブジェクトを取り出す．
// デフォルトでは NULL を返す．
MislibPt*
MislibPt::rise_block_delay() const
{
  return NULL;
}

// 立ち上がり負荷依存遅延を表すオブジェクトを取り出す．
// デフォルトでは NULL を返す．
MislibPt*
MislibPt::rise_fanout_delay() const
{
  return NULL;
}

// 立ち下がり固定遅延を表すオブジェクトを取り出す．
// デフォルトでは NULL を返す．
MislibPt*
MislibPt::fall_block_delay() const
{
  return NULL;
}

// 立ち下がり負荷依存遅延を表すオブジェクトを取り出す．
// デフォルトでは NULL を返す．
MislibPt*
MislibPt::fall_fanout_delay() const
{
  return NULL;
}

// 次のピンを設定する．
// デフォルトでは何もしない．
void
MislibPt::set_next(MislibPt* pin)
{
}

// 次のピンを取り出す．
// デフォルトでは NULL を返す．
MislibPt*
MislibPt::next() const
{
  return NULL;
}

// 末尾に要素を追加する．
// デフォルトでは何もしない．
void
MislibPt::push_back(MislibPt* pin)
{
}

// 先頭の要素を取り出す．
// デフォルトでは NULL を返す．
MislibPt*
MislibPt::top() const
{
  return NULL;
}

// @brief 面積を表すオブジェクトを返す．
// @note デフォルトでは NULL を返す．
MislibPt*
MislibPt::area() const
{
  return NULL;
}

// @brief 出力ピン名を表すオブジェクトを返す．
// @note デフォルトでは NULL を返す．
MislibPt*
MislibPt::opin_name() const
{
  return NULL;
}

// @brief 出力の論理式を表すオブジェクトを返す．
// @note デフォルトでは NULL を返す．
MislibPt*
MislibPt::opin_expr() const
{
  return NULL;
}

// @brief 入力ピンのリストを表すオブジェクトを返す．
// @note デフォルトでは NULL を返す．
MislibPt*
MislibPt::ipin_list() const
{
  return NULL;
}

// 位置を出力する．
void
MislibPt::dump_loc(ostream& s) const
{
  s << "<LOC>" << loc() << "</LOC>" << endl;
}


//////////////////////////////////////////////////////////////////////
// 文字列を表すクラス
//////////////////////////////////////////////////////////////////////

// コンストラクタ
MislibPtStr::MislibPtStr(const FileRegion& loc,
			 ShString str) :
  MislibPt(loc),
  mStr(str)
{
}

// デストラクタ
MislibPtStr::~MislibPtStr()
{
}

// 種類を取り出す．
tType
MislibPtStr::type() const
{
  return kStr;
}

// @brief 論理式を表す型のときに true を返す．
bool
MislibPtStr::is_expr() const
{
  return true;
}

// 文字列を取り出す
ShString
MislibPtStr::str() const
{
  return mStr;
}

// @brief 対応する論理式を生成する．
// @param[in] name_map 端子名をキーにして端子番号を取り出す連想配列
LogExpr
MislibPtStr::to_expr(const hash_map<ShString, ymuint>& name_map) const
{
  hash_map<ShString, ymuint>::const_iterator p = name_map.find(str());
  assert_cond( p != name_map.end(), __FILE__, __LINE__);
  return LogExpr::make_posiliteral(p->second);
}

// 内容を出力する．
// デバッグ用
void
MislibPtStr::dump(ostream& s) const
{
  s << "<STR>" << endl;
  dump_loc(s);
  s << ShString(str()) << endl
    << "</STR>" << endl;
}


//////////////////////////////////////////////////////////////////////
// 数値を表すクラス
//////////////////////////////////////////////////////////////////////

// コンストラクタ
MislibPtNum::MislibPtNum(const FileRegion& loc,
			 double num) :
  MislibPt(loc),
  mNum(num)
{
}

// デストラクタ
MislibPtNum::~MislibPtNum()
{
}

// 種類を取り出す．
tType
MislibPtNum::type() const
{
  return kNum;
}

// 数値を取り出す
double
MislibPtNum::num() const
{
  return mNum;
}

// 内容を出力する．
// デバッグ用
void
MislibPtNum::dump(ostream& s) const
{
  s << "<NUM>" << endl;
  dump_loc(s);
  s << num() << endl
    << "</NUM>" << endl;
}


//////////////////////////////////////////////////////////////////////
// NONINV 極性を表すクラス
//////////////////////////////////////////////////////////////////////

// コンストラクタ
MislibPtNoninv::MislibPtNoninv(const FileRegion& loc) :
  MislibPt(loc)
{
}

// デストラクタ
MislibPtNoninv::~MislibPtNoninv()
{
}

// 種類を取り出す．
tType
MislibPtNoninv::type() const
{
  return kNoninv;
}

// 内容を出力する．
// デバッグ用
void
MislibPtNoninv::dump(ostream& s) const
{
  s << "<NONINV>" << endl;
  dump_loc(s);
  s << "</NONINV>" << endl;
}


//////////////////////////////////////////////////////////////////////
// INV 極性を表すクラス
//////////////////////////////////////////////////////////////////////

// コンストラクタ
MislibPtInv::MislibPtInv(const FileRegion& loc) :
  MislibPt(loc)
{
}

// デストラクタ
MislibPtInv::~MislibPtInv()
{
}

// 種類を取り出す．
tType
MislibPtInv::type() const
{
  return kInv;
}

// 内容を出力する．
// デバッグ用
void
MislibPtInv::dump(ostream& s) const
{
  s << "<INV>" << endl;
  dump_loc(s);
  s << "</INV>" << endl;
}


//////////////////////////////////////////////////////////////////////
// UNKNOWN 極性を表すクラス
//////////////////////////////////////////////////////////////////////

// コンストラクタ
MislibPtUnknown::MislibPtUnknown(const FileRegion& loc) :
  MislibPt(loc)
{
}

// デストラクタ
MislibPtUnknown::~MislibPtUnknown()
{
}

// 種類を取り出す．
tType
MislibPtUnknown::type() const
{
  return kUnknown;
}

// 内容を出力する．
// デバッグ用
void
MislibPtUnknown::dump(ostream& s) const
{
  s << "<UNKNOWN>" << endl;
  dump_loc(s);
  s << "</UNKNOWN>" << endl;
}


//////////////////////////////////////////////////////////////////////
// 定数0を表すクラス
//////////////////////////////////////////////////////////////////////

// コンストラクタ
MislibPtConst0::MislibPtConst0(const FileRegion& loc) :
  MislibPt(loc)
{
}

// デストラクタ
MislibPtConst0::~MislibPtConst0()
{
}

// 種類を取り出す．
tType
MislibPtConst0::type() const
{
  return kConst0;
}

// @brief 論理式を表す型のときに true を返す．
bool
MislibPtConst0::is_expr() const
{
  return true;
}

// @brief 対応する論理式を生成する．
// @param[in] name_map 端子名をキーにして端子番号を取り出す連想配列
LogExpr
MislibPtConst0::to_expr(const hash_map<ShString, ymuint>& name_map) const
{
  return LogExpr::make_zero();
}

// 内容を出力する．
// デバッグ用
void
MislibPtConst0::dump(ostream& s) const
{
  s << "<CONST0>" << endl;
  dump_loc(s);
  s << "</CONST0>" << endl;
}


//////////////////////////////////////////////////////////////////////
// 定数1を表すクラス
//////////////////////////////////////////////////////////////////////

// コンストラクタ
MislibPtConst1::MislibPtConst1(const FileRegion& loc) :
  MislibPt(loc)
{
}

// デストラクタ
MislibPtConst1::~MislibPtConst1()
{
}

// 種類を取り出す．
tType
MislibPtConst1::type() const
{
  return kConst1;
}

// @brief 論理式を表す型のときに true を返す．
bool
MislibPtConst1::is_expr() const
{
  return true;
}

// @brief 対応する論理式を生成する．
// @param[in] name_map 端子名をキーにして端子番号を取り出す連想配列
LogExpr
MislibPtConst1::to_expr(const hash_map<ShString, ymuint>& name_map) const
{
  return LogExpr::make_one();
}

// 内容を出力する．
// デバッグ用
void
MislibPtConst1::dump(ostream& s) const
{
  s << "<CONST1>" << endl;
  dump_loc(s);
  s << "</CONST1>" << endl;
}


//////////////////////////////////////////////////////////////////////
/// MislibPtのリストを表すクラス
//////////////////////////////////////////////////////////////////////

// コンストラクタ
MislibPtList::MislibPtList() :
  MislibPt(FileRegion())
{
  mTop = NULL;
  mEnd = NULL;
}

// デストラクタ
MislibPtList::~MislibPtList()
{
}

// 種類を取り出す．
tType
MislibPtList::type() const
{
  return kList;
}

// 末尾に要素を追加する．
void
MislibPtList::push_back(MislibPt* obj)
{
  if ( mEnd ) {
    mEnd->set_next(obj);
    mEnd = obj;
  }
  else {
    mTop = mEnd = obj;
  }
}

// 先頭の要素を取り出す．
MislibPt*
MislibPtList::top() const
{
  return mTop;
}

// 内容を出力する．
// デバッグ用
void
MislibPtList::dump(ostream& s) const
{
  s << "<PT_LIST>" << endl;
  dump_loc(s);

  for (MislibPt* tmp = top(); tmp; tmp = tmp->next()) {
    tmp->dump(s);
  }

  s << "</PT_LIST>" << endl;
}

END_NAMESPACE_YM_CELL_MISLIB
