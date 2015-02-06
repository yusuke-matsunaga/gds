
/// @file IrNode.cc
/// @brief IrNode の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2015 Yusuke Matsunaga
/// All rights reserved.


#include "IrNode.h"


BEGIN_NAMESPACE_YM_YMSL

//////////////////////////////////////////////////////////////////////
// クラス IrNode
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
// @param[in] opcode オペコード
// @param[in] type 型
IrNode::IrNode(OpCode opcode,
	       const Type* type) :
  mOpCode(opcode),
  mType(type)
{
}

// @brief デストラクタ
IrNode::~IrNode()
{
}

// @brief opcode を返す．
OpCode
IrNode::opcode() const
{
  return mOpCode;
}

// @brief 型を返す．
const Type*
IrNode::type() const
{
  return mType;
}

// @brief オペランド数を返す．
//
// 演算子のみ有効
ymuint
IrNode::operand_num() const
{
  return 0;
}

// @brief オペランドを返す．
// @param[in] pos 位置 ( 0 <= pos < operand_num() )
//
// 演算子のみ有効
IrNode*
IrNode::operand(ymuint pos) const
{
  ASSERT_NOT_REACHED;
  return NULL;
}

// @brief 整数値を返す．
int
IrNode::int_val() const
{
  ASSERT_NOT_REACHED;
  return 0;
}

// @brief 実数値を返す．
double
IrNode::float_val() const
{
  ASSERT_NOT_REACHED;
  return 0.0;
}

// @brief 文字列を返す．
const char*
IrNode::string_val() const
{
  ASSERT_NOT_REACHED;
  return NULL;
}

// @brief 変数を返す．
const Var*
IrNode::var() const
{
  ASSERT_NOT_REACHED;
  return NULL;
}

// @brief 書き込む値を返す．
//
// kOpStore, kOpArrayStore, kOpMemberStore のみ有効
IrNode*
IrNode::store_val() const
{
  ASSERT_NOT_REACHED;
  return NULL;
}

// @brief 配列本体の式を返す．
//
// kOpArrayLoad, kOpArrayStore のみ有効
IrNode*
IrNode::array_expr() const
{
  ASSERT_NOT_REACHED;
  return NULL;
}

// @brief 配列のインデックスを返す．
//
// kOpArrayLoad, kOpArrayStore のみ有効
IrNode*
IrNode::array_index() const
{
  ASSERT_NOT_REACHED;
  return NULL;
}

// @brief オブジェクトを指す式を返す．
//
// kOpMemberLoad, kOpMemberStore のみ有効
IrNode*
IrNode::obj_expr() const
{
  ASSERT_NOT_REACHED;
  return NULL;
}

// @brief メンバのインデックスを返す．
//
// kOpMemberLoad, kOpMemberStore のみ有効
ymuint
IrNode::member_index() const
{
  ASSERT_NOT_REACHED;
  return 0;
}

// @brief 関数本体を返す．
//
// kOpFuncCall のみ有効
const Function*
IrNode::function() const
{
  ASSERT_NOT_REACHED;
  return NULL;
}

// @brief 関数の引数の数を得る．
//
// kOpFuncCall のみ有効
ymuint
IrNode::arglist_num() const
{
  ASSERT_NOT_REACHED;
  return 0;
}

// @brief 関数の引数を得る．
// @param[in] pos 位置 ( 0 <= pos < arglist_num() )
//
// kOpFuncCall のみ有効
IrNode*
IrNode::arglist_elem(ymuint pos) const
{
  ASSERT_NOT_REACHED;
  return NULL;
}

// @brief ジャンプ先のノードを得る．
//
// kOpJump, kOpBranchXXX のみ有効
IrNode*
IrNode::jump_addr() const
{
  ASSERT_NOT_REACHED;
  return NULL;
}

// @brief 分岐条件
IrNode*
IrNode::branch_cond() const
{
  ASSERT_NOT_REACHED;
  return NULL;
}

// @brief 返り値
IrNode*
IrNode::return_val() const
{
  ASSERT_NOT_REACHED;
  return NULL;
}

// @brief 定義済みの時に true を返す．
//
// kOpLabel のみ意味を持つ．
bool
IrNode::is_defined() const
{
  ASSERT_NOT_REACHED;
  return false;
}

// @brief 定義済みにする．
//
// kOpLabel のみ意味を持つ．
void
IrNode::set_defined()
{
  ASSERT_NOT_REACHED;
}

END_NAMESPACE_YM_YMSL