#ifndef ASTFOR_H
#define ASTFOR_H

/// @file AstFor.h
/// @brief AstFor のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2014 Yusuke Matsunaga
/// All rights reserved.


#include "AstBlockStmt.h"


BEGIN_NAMESPACE_YM_YMSL

//////////////////////////////////////////////////////////////////////
/// @class AstFor AstFor.h "AstFor.h"
/// @brief for 文を表す Ast
//////////////////////////////////////////////////////////////////////
class AstFor :
  public AstBlockStmt
{
public:

  /// @brief コンストラクタ
  /// @param[in] init 初期化文
  /// @param[in] cond 条件式
  /// @param[in] next 増加文
  /// @param[in] block 本体
  /// @param[in] loc ファイル位置
  AstFor(AstStatement* init,
	 AstExpr* cond,
	 AstStatement* next,
	 AstBlock* block,
	 const FileRegion& loc);

  /// @brief デストラクタ
  virtual
  ~AstFor();


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

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

  /// @brief 内容を表示する．(デバッグ用)
  /// @param[in] s 出力ストリーム
  /// @param[in] indent インデントレベル
  virtual
  void
  print(ostream& s,
	ymuint indent = 0) const;


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // 初期化文
  AstStatement* mInit;

  // 条件
  AstExpr* mCond;

  // 増加文
  AstStatement* mNext;

};

END_NAMESPACE_YM_YMSL


#endif // YMSLASTFOR_H