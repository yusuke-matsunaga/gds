
/// @file Ast.cc
/// @brief Ast の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2014 Yusuke Matsunaga
/// All rights reserved.


#include "Ast.h"
#include "AstSymbol.h"
#include "AstModule.h"


BEGIN_NAMESPACE_YM_YMSL

//////////////////////////////////////////////////////////////////////
// クラス Ast
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
// @param[in] loc ファイル位置
Ast::Ast(const FileRegion& loc) :
  mLoc(loc)
{
}

// @brief デストラクタ
Ast::~Ast()
{
}

// @brief ファイル位置を得る．
const FileRegion&
Ast::file_region() const
{
  return mLoc;
}


//////////////////////////////////////////////////////////////////////
// クラス AstSymbol
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
// @param[in] val 値
// @param[in] loc ファイル位置
AstSymbol::AstSymbol(ShString val,
		     const FileRegion& loc) :
  Ast(loc),
  mVal(val)
{
}

// @brief デストラクタ
AstSymbol::~AstSymbol()
{
}

// @brief 文字列型の値を返す．
ShString
AstSymbol::str_val() const
{
  return mVal;
}


//////////////////////////////////////////////////////////////////////
// クラス AstModule
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
// @param[in] module モジュール名
// @param[in] alias エイリアス名
// @param[in] loc ファイル位置
AstModule::AstModule(AstSymbol* module,
		     AstSymbol* alias,
		     const FileRegion& loc) :
  Ast(loc),
  mModuleName(module),
  mAliasName(alias)
{
}

// @brief デストラクタ
AstModule::~AstModule()
{
}

// @brief モジュール名を返す．
AstSymbol*
AstModule::module_name() const
{
  return mModuleName;
}

// @brief エイリアス名を返す．
//
// 場合によっては NULL を返す．
AstSymbol*
AstModule::alias_name() const
{
  return mAliasName;
}


//////////////////////////////////////////////////////////////////////
// misc
//////////////////////////////////////////////////////////////////////

// @brief ValueType を出力する．
ostream&
operator<<(ostream& s,
	   ValueType vt)
{
  switch ( vt ) {
  case kVoidType:    s << "void"; break;
  case kBooleanType: s << "boolean"; break;
  case kIntType:     s << "int"; break;
  case kFloatType:   s << "float"; break;
  case kStringType:  s << "string"; break;
  case kUserType:    s << "__user__"; break;
  }
  return s;
}

END_NAMESPACE_YM_YMSL
