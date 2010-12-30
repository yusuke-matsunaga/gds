/// @file libym_verilog/parser/main/Parser_decl.cc
/// @brief Parser の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// $Id: Parser_decl.h 2507 2009-10-17 16:24:02Z matsunaga $
///
/// Copyright (C) 2005-2010 Yusuke Matsunaga
/// All rights reserved.


#include "Parser.h"

#include "PtiFactory.h"
#include "PtiDecl.h"


BEGIN_NAMESPACE_YM_VERILOG

//////////////////////////////////////////////////////////////////////
// PtiIOHead の生成
//////////////////////////////////////////////////////////////////////

// @brief IO 宣言のヘッダの生成
// @param[in] fr ファイル位置の情報
// @param[in] type IO の種類
// @param[in] sign 符号付きのとき true となるフラグ
// @return 生成された IO宣言ヘッダ
void
Parser::new_IOHead(const FileRegion& fr,
		   tPtIOType type,
		   bool sign,
		   PtrList<PtAttrInst>* ai_list)
{
  PtiIOHead* head = mFactory.new_IOHead(fr, type, sign);
  add_io_head(head, ai_list);
}

// @brief IO 宣言のヘッダの生成 (reg 型)
// @param[in] fr ファイル位置の情報
// @param[in] type IO の種類
// @param[in] sign 符号付きのとき true となるフラグ
// @return 生成された IO宣言ヘッダ
void
Parser::new_RegIOHead(const FileRegion& fr,
		      tPtIOType type,
		      bool sign,
		      PtrList<PtAttrInst>* ai_list)
{
  PtiIOHead* head = mFactory.new_RegIOHead(fr, type, sign);
  add_io_head(head, ai_list);
}

// @brief IO 宣言のヘッダの生成 (ネット型)
// @param[in] fr ファイル位置の情報
// @param[in] type IO の種類
// @param[in] net_type 補助的なネット型
// @param[in] sign 符号付きのとき true となるフラグ
// @return 生成された IO宣言ヘッダ
void
Parser::new_NetIOHead(const FileRegion& fr,
		      tPtIOType type,
		      tVpiNetType net_type,
		      bool sign,
		      PtrList<PtAttrInst>* ai_list)
{
  PtiIOHead* head = mFactory.new_NetIOHead(fr, type, net_type, sign);
  add_io_head(head, ai_list);
}

// @brief IO 宣言のヘッダの生成 (変数型)
// @param[in] fr ファイル位置の情報
// @param[in] type IO の種類
// @param[in] var_type 補助的な変数型
// @return 生成された IO宣言ヘッダ
void
Parser::new_VarIOHead(const FileRegion& fr,
		      tPtIOType type,
		      tVpiVarType var_type,
		      PtrList<PtAttrInst>* ai_list)
{
  PtiIOHead* head = mFactory.new_VarIOHead(fr, type, var_type);
  add_io_head(head, ai_list);
}

// @brief 範囲付きの IO 宣言のヘッダの生成
// @param[in] fr ファイル位置の情報
// @param[in] type IO の種類
// @param[in] sign 符号付きのとき true となるフラグ
// @param[in] left 範囲の左側の式
// @param[in] right 範囲の右側の式
// @return 生成された IO宣言ヘッダ
void
Parser::new_IOHead(const FileRegion& fr,
		   tPtIOType type,
		   bool sign,
		   PtExpr* left,
		   PtExpr* right,
		   PtrList<PtAttrInst>* ai_list)
{
  PtiIOHead* head = mFactory.new_IOHead(fr, type, sign, left, right);
  add_io_head(head, ai_list);
}

// @brief 範囲付きの IO 宣言のヘッダの生成 (reg 型)
// @param[in] fr ファイル位置の情報
// @param[in] type IO の種類
// @param[in] sign 符号付きのとき true となるフラグ
// @param[in] left 範囲の左側の式
// @param[in] right 範囲の右側の式
// @return 生成された IO宣言ヘッダ
void
Parser::new_RegIOHead(const FileRegion& fr,
		      tPtIOType type,
		      bool sign,
		      PtExpr* left,
		      PtExpr* right,
		      PtrList<PtAttrInst>* ai_list)
{
  PtiIOHead* head = mFactory.new_RegIOHead(fr, type, sign, left, right);
  add_io_head(head, ai_list);
}

// @brief 範囲付きの IO 宣言のヘッダの生成 (ネット型)
// @param[in] fr ファイル位置の情報
// @param[in] type IO の種類
// @param[in] net_type 補助的なネット型
// @param[in] sign 符号付きのとき true となるフラグ
// @param[in] left 範囲の左側の式
// @param[in] right 範囲の右側の式
// @return 生成された IO宣言ヘッダ
void
Parser::new_NetIOHead(const FileRegion& fr,
		      tPtIOType type,
		      tVpiNetType net_type,
		      bool sign,
		      PtExpr* left,
		      PtExpr* right,
		      PtrList<PtAttrInst>* ai_list)
{
  PtiIOHead* head = mFactory.new_NetIOHead(fr, type, net_type, sign, left, right);
  add_io_head(head, ai_list);
}

// @brief IO宣言リストにIO宣言ヘッダを追加する．
inline
void
Parser::add_io_head(PtiIOHead* head,
		    PtrList<PtAttrInst>* attr_list)
{
  reg_attrinst(head, attr_list);
  mCurIOHeadList->push_back(head);
  mIOItemList.clear();
}


//////////////////////////////////////////////////////////////////////
// PtIOItem の生成
//////////////////////////////////////////////////////////////////////

// @brief IO 宣言の要素の生成
// @param[in] fr ファイル位置の情報
// @param[in] name 要素名
void
Parser::new_IOItem(const FileRegion& fr,
		   const char* name)
{
  add_io_item( mFactory.new_IOItem(fr, name) );
}

// @brief 初期値付き IO 宣言の要素の生成
// @param[in] fr ファイル位置の情報
// @param[in] name 要素名
// @param[in] init_value 初期値を表す式
void
Parser::new_IOItem(const FileRegion& fr,
		   const char* name,
		   PtExpr* init_value)
{
  add_io_item( mFactory.new_IOItem(fr, name, init_value) );
}

// @brief IO宣言リストにIO宣言要素を追加する．
void
Parser::add_io_item(PtIOItem* item)
{
  mIOItemList.push_back(item);
}


//////////////////////////////////////////////////////////////////////
// PtDeclHead ( parameter ) の生成
//////////////////////////////////////////////////////////////////////

// @brief パラメータ宣言のヘッダの生成 (型指定なし)
// @param[in] fr ファイル位置の情報
void
Parser::new_ParamH(const FileRegion& fr,
		   PtrList<PtAttrInst>* ai_list,
		   bool paramport)
{
  PtiDeclHead* head = mFactory.new_ParamH(fr);
  add_paramport_head(head, ai_list, paramport);
}

// @brief 範囲指定型パラメータ宣言のヘッダの生成
// @param[in] fr ファイル位置の情報
// @param[in] sign 符号付きのとき true となるフラグ
// @param[in] left 範囲の左側の式
// @param[in] right 範囲の右側の式
void
Parser::new_ParamH(const FileRegion& fr,
		   bool sign,
		   PtExpr* left,
		   PtExpr* right,
		   PtrList<PtAttrInst>* ai_list,
		   bool paramport)
{
  PtiDeclHead* head = mFactory.new_ParamH(fr, sign, left, right);
  add_paramport_head(head, ai_list, paramport);
}

// @brief 組み込み型パラメータ宣言のヘッダの生成
// @param[in] fr ファイル位置の情報
// @param[in] var_type データ型
void
Parser::new_ParamH(const FileRegion& fr,
		   tVpiVarType var_type,
		   PtrList<PtAttrInst>* ai_list,
		   bool paramport)
{
  PtiDeclHead* head = mFactory.new_ParamH(fr, var_type);
  add_paramport_head(head, ai_list, paramport);
}

// @brief parameter port 宣言ヘッダを追加する．
inline
void
Parser::add_paramport_head(PtiDeclHead* head,
			   PtrList<PtAttrInst>* attr_list,
			   bool paramport)
{
  reg_attrinst(head, attr_list);
  if ( paramport ) {
    mParamPortHeadList.push_back(head);
  }
  else {
    mCurParamHeadList->push_back(head);
  }
  mDeclItemList.clear();
}


//////////////////////////////////////////////////////////////////////
// PtDeclHead ( localparam ) の生成
//////////////////////////////////////////////////////////////////////

// @brief local param 宣言のヘッダの生成 (型指定なし)
// @param[in] fr ファイル位置の情報
void
Parser::new_LocalParamH(const FileRegion& fr,
			PtrList<PtAttrInst>* ai_list)
{
  PtiDeclHead* head = mFactory.new_LocalParamH(fr);
  add_localparam_head(head, ai_list);
}

// @brief 範囲指定型 local param 宣言のヘッダの生成
// @param[in] fr ファイル位置の情報
// @param[in] sign 符号付きのとき true となるフラグ
// @param[in] left 範囲の左側の式
// @param[in] right 範囲の右側の式
void
Parser::new_LocalParamH(const FileRegion& fr,
			bool sign,
			PtExpr* left,
			PtExpr* right,
			PtrList<PtAttrInst>* ai_list)
{
  PtiDeclHead* head = mFactory.new_LocalParamH(fr, sign, left, right);
  add_localparam_head(head, ai_list);
}

// @brief 組み込み型パラメータ宣言のヘッダの生成
// @param[in] fr ファイル位置の情報
// @param[in] var_type データ型
void
Parser::new_LocalParamH(const FileRegion& fr,
			tVpiVarType var_type,
			PtrList<PtAttrInst>* ai_list)
{
  PtiDeclHead* head = mFactory.new_LocalParamH(fr, var_type);
  add_localparam_head(head, ai_list);
}

// @brief localparam 宣言ヘッダを追加する．
inline
void
Parser::add_localparam_head(PtiDeclHead* head,
			    PtrList<PtAttrInst>* attr_list)
{
  reg_attrinst(head, attr_list);
  mCurLparamHeadList->push_back(head);
  mDeclItemList.clear();
}


//////////////////////////////////////////////////////////////////////
// PtDeclHead の生成
//////////////////////////////////////////////////////////////////////

// @brief specparam 宣言のヘッダの生成
// @param[in] fr ファイル位置の情報
// @return 生成された specparam
void
Parser::new_SpecParamH(const FileRegion& fr,
		       PtrList<PtAttrInst>* ai_list)
{
  PtiDeclHead* head = mFactory.new_SpecParamH(fr);
  add_decl_head(head, ai_list);
}

// @brief 範囲指定型 specparam 宣言のヘッダの生成
// @param[in] fr ファイル位置の情報
// @param[in] left 範囲の左側の式
// @param[in] right 範囲の右側の式
void
Parser::new_SpecParamH(const FileRegion& fr,
		       PtExpr* left,
		       PtExpr* right,
		       PtrList<PtAttrInst>* ai_list)
{
  PtiDeclHead* head = mFactory.new_SpecParamH(fr, left, right);
  add_decl_head(head, ai_list);
}

// @brief イベント宣言のヘッダの生成
// @param[in] fr ファイル位置の情報
// @return 生成されたイベント
void
Parser::new_EventH(const FileRegion& fr,
		   PtrList<PtAttrInst>* ai_list)
{
  PtiDeclHead* head = mFactory.new_EventH(fr);
  add_decl_head(head, ai_list);
}

// @brief genvar 宣言のヘッダの生成
// @param[in] fr ファイル位置の情報
void
Parser::new_GenvarH(const FileRegion& fr,
		    PtrList<PtAttrInst>* ai_list)
{
  PtiDeclHead* head = mFactory.new_GenvarH(fr);
  add_decl_head(head, ai_list);
}

// @brief 変数宣言のヘッダの生成
// @param[in] fr ファイル位置の情報
// @param[in] var_type データ型
void
Parser::new_VarH(const FileRegion& fr,
		 tVpiVarType var_type,
		 PtrList<PtAttrInst>* ai_list)
{
  PtiDeclHead* head = mFactory.new_VarH(fr, var_type);
  add_decl_head(head, ai_list);
}

// @brief 1ビット型 reg 宣言のヘッダの生成
// @param[in] fr ファイル位置の情報
// @param[in] sign 符号付きの時 true となるフラグ
void
Parser::new_RegH(const FileRegion& fr,
		 bool sign,
		 PtrList<PtAttrInst>* ai_list)
{
  PtiDeclHead* head = mFactory.new_RegH(fr, sign);
  add_decl_head(head, ai_list);
}

// @brief 範囲指定型 reg 宣言のヘッダの生成
// @param[in] fr ファイル位置の情報
// @param[in] sign 符号付きの時 true となるフラグ
// @param[in] left 範囲の左側の式
// @param[in] right 範囲の右側の式
void
Parser::new_RegH(const FileRegion& fr,
		 bool sign,
		 PtExpr* left,
		 PtExpr* right,
		 PtrList<PtAttrInst>* ai_list)
{
  PtiDeclHead* head = mFactory.new_RegH(fr, sign, left, right);
  add_decl_head(head, ai_list);
}

// @brief 1ビット型 net 宣言のヘッダの生成
// @param[in] fr ファイル位置の情報
// @param[in] type net の型
// @param[in] sign 符号の有無を表すフラグ
void
Parser::new_NetH(const FileRegion& fr,
		 tVpiNetType type,
		 bool sign,
		 PtrList<PtAttrInst>* ai_list)
{
  PtiDeclHead* head = mFactory.new_NetH(fr, type, sign);
  add_decl_head(head, ai_list);
}

// @brief 1ビット型 net 宣言のヘッダの生成 (strength あり)
// @param[in] fr ファイル位置の情報
// @param[in] type net の型
// @param[in] sign 符号の有無を表すフラグ
// @param[in] strength 信号強度
void
Parser::new_NetH(const FileRegion& fr,
		 tVpiNetType type,
		 bool sign,
		 PtStrength* strength,
		 PtrList<PtAttrInst>* ai_list)
{
  PtiDeclHead* head = mFactory.new_NetH(fr, type, sign, strength);
  add_decl_head(head, ai_list);
}

// @brief 1ビット型 net 宣言のヘッダの生成 (遅延あり)
// @param[in] fr ファイル位置の情報
// @param[in] type net の型
// @param[in] sign 符号の有無を表すフラグ
// @param[in] delay 遅延
void
Parser::new_NetH(const FileRegion& fr,
		 tVpiNetType type,
		 bool sign,
		 PtDelay* delay,
		 PtrList<PtAttrInst>* ai_list)
{
  PtiDeclHead* head = mFactory.new_NetH(fr, type, sign, delay);
  add_decl_head(head, ai_list);
}

// @brief 1ビット型 net 宣言のヘッダの生成 (strength, 遅延あり)
// @param[in] fr ファイル位置の情報
// @param[in] type net の型
// @param[in] sign 符号の有無を表すフラグ
// @param[in] strength 信号強度
// @param[in] delay 遅延
void
Parser::new_NetH(const FileRegion& fr,
		 tVpiNetType type,
		 bool sign,
		 PtStrength* strength,
		 PtDelay* delay,
		 PtrList<PtAttrInst>* ai_list)
{
  PtiDeclHead* head = mFactory.new_NetH(fr, type, sign, strength, delay);
  add_decl_head(head, ai_list);
}

// @brief 範囲指定型 net 宣言のヘッダの生成
// @param[in] fr ファイル位置の情報
// @param[in] type net の型
// @param[in] vstype vector/scalar 指定
// @param[in] sign 符号の有無を表すフラグ
// @param[in] left 範囲の左側の式
// @param[in] right 範囲の右側の式
void
Parser::new_NetH(const FileRegion& fr,
		 tVpiNetType type,
		 tVpiVsType vstype,
		 bool sign,
		 PtExpr* left,
		 PtExpr* right,
		 PtrList<PtAttrInst>* ai_list)
{
  PtiDeclHead* head = mFactory.new_NetH(fr, type, vstype, sign, left, right);
  add_decl_head(head, ai_list);
}

// @brief 範囲指定型 net 宣言のヘッダの生成 (strengthあり)
// @param[in] fr ファイル位置の情報
// @param[in] type net の型
// @param[in] vstype vector/scalar 指定
// @param[in] sign 符号の有無を表すフラグ
// @param[in] left 範囲の左側の式
// @param[in] right 範囲の右側の式
// @param[in] strength 信号強度
void
Parser::new_NetH(const FileRegion& fr,
		 tVpiNetType type,
		 tVpiVsType vstype,
		 bool sign,
		 PtExpr* left,
		 PtExpr* right,
		 PtStrength* strength,
		 PtrList<PtAttrInst>* ai_list)
{
  PtiDeclHead* head = mFactory.new_NetH(fr, type, vstype, sign, left, right,
					strength);
  add_decl_head(head, ai_list);
}

// @brief 範囲指定型 net 宣言のヘッダの生成 (遅延あり)
// @param[in] fr ファイル位置の情報
// @param[in] type net の型
// @param[in] vstype vector/scalar 指定
// @param[in] sign 符号の有無を表すフラグ
// @param[in] left 範囲の左側の式
// @param[in] right 範囲の右側の式
// @param[in] delay 遅延
void
Parser::new_NetH(const FileRegion& fr,
		 tVpiNetType type,
		 tVpiVsType vstype,
		 bool sign,
		 PtExpr* left,
		 PtExpr* right,
		 PtDelay* delay,
		 PtrList<PtAttrInst>* ai_list)
{
  PtiDeclHead* head = mFactory.new_NetH(fr, type, vstype, sign, left, right,
					delay);
  add_decl_head(head, ai_list);
}

// @brief 範囲指定型 net 宣言のヘッダの生成 (strength, 遅延あり)
// @param[in] fr ファイル位置の情報
// @param[in] type net の型
// @param[in] vstype vector/scalar 指定
// @param[in] sign 符号の有無を表すフラグ
// @param[in] left 範囲の左側の式
// @param[in] right 範囲の右側の式
// @param[in] strength 信号強度
// @param[in] delay 遅延
void
Parser::new_NetH(const FileRegion& fr,
		 tVpiNetType type,
		 tVpiVsType vstype,
		 bool sign,
		 PtExpr* left,
		 PtExpr* right,
		 PtStrength* strength,
		 PtDelay* delay,
		 PtrList<PtAttrInst>* ai_list)
{
  PtiDeclHead* head = mFactory.new_NetH(fr, type, vstype, sign, left, right,
					strength, delay);
  add_decl_head(head, ai_list);
}

// @brief 宣言リストに宣言ヘッダを追加する．
inline
void
Parser::add_decl_head(PtiDeclHead* head,
		      PtrList<PtAttrInst>* attr_list)
{
  reg_attrinst(head, attr_list);
  mCurDeclHeadList->push_back(head);
  mDeclItemList.clear();
}


//////////////////////////////////////////////////////////////////////
// PtDeclItem の生成
//////////////////////////////////////////////////////////////////////

// @brief 宣言要素の生成
// @param[in] fr ファイル位置の情報
// @param[in] name 名前
void
Parser::new_DeclItem(const FileRegion& fr,
		     const char* name)
{
  add_decl_item( mFactory.new_DeclItem(fr, name) );
}

// @brief 初期値付き宣言要素の生成
// @param[in] fr ファイル位置の情報
// @param[in] name 名前
// @param[in] init_value 初期値を表す式
void
Parser::new_DeclItem(const FileRegion& fr,
		     const char* name,
		     PtExpr* init_value)
{
  add_decl_item( mFactory.new_DeclItem(fr, name, init_value) );
}

// @brief 配列型宣言要素の生成
// @param[in] fr ファイル位置の情報
// @param[in] name 名前
// @param[in] range_array 配列の各次元の範囲のリスト
void
Parser::new_DeclItem(const FileRegion& fr,
		     const char* name,
		     PtrList<PtRange>* range_list)
{
  add_decl_item( mFactory.new_DeclItem(fr, name, to_array(range_list)) );
}

// @brief 宣言リストに宣言要素を追加する．
inline
void
Parser::add_decl_item(PtDeclItem* item)
{
  mDeclItemList.push_back(item);
}


//////////////////////////////////////////////////////////////////////
// PtRange の生成
//////////////////////////////////////////////////////////////////////

// @brief 範囲の生成
// @param[in] msb MSB を表す式
// @param[in] lsb LSB を表す式
PtRange*
Parser::new_Range(const FileRegion& fr,
		  PtExpr* msb,
		  PtExpr* lsb)
{
  return mFactory.new_Range(fr, msb, lsb);
}

END_NAMESPACE_YM_VERILOG
