#ifndef ASTVAREXPR_H
#define ASTVAREXPR_H

/// @file AstVarExpr.h
/// @brief AstVarExpr のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2014 Yusuke Matsunaga
/// All rights reserved.


#include "AstExpr.h"


BEGIN_NAMESPACE_YM_YMSL

//////////////////////////////////////////////////////////////////////
/// @class AstVarExpr AstVarExpr.h "AstVarExpr.h"
/// @brief 変数を表す AstExpr
//////////////////////////////////////////////////////////////////////
class AstVarExpr :
  public AstExpr
{
public:

  /// @brief コンストラクタ
  /// @param[in] var_name 変数名
  /// @param[in] loc ファイル位置
  AstVarExpr(AstIdentifier* var_name,
	     const FileRegion& loc);

  /// @brief デストラクタ
  virtual
  ~AstVarExpr();


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief 変数の参照を解決する．
  virtual
  void
  resolve_var(YmslScope* parent_scope);

  /// @brief 式の型を解析する．
  /// @return 引数の方が間違っていたら false を返す．
  ///
  /// 結果としてキャスト演算が挿入される場合もある．
  virtual
  bool
  type_analysis();

  /// @brief 式の型を返す．
  virtual
  ValueType
  type();

  /// @brief 命令コードのサイズを計算する．
  virtual
  ymuint
  calc_size();

  /// @brief 命令コードを生成する．
  /// @param[in] driver ドライバ
  /// @param[in] code_list 命令コードの格納先
  /// @param[inout] addr 命令コードの現在のアドレス
  ///
  /// addr の値は更新される．
  virtual
  void
  compile(YmslDriver& driver,
	  YmslCodeList& code_list,
	  Ymsl_INT& addr);


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // 変数名
  AstIdentifier* mVarName;

  // 変数宣言
  AstVarDecl* mVarDecl;

};

END_NAMESPACE_YM_YMSL


#endif // ASTVAREXPR_H