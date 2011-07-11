
/// @file libym_networks/tests/blif/NullBlifHandler.cc
/// @brief NullBlifHandler の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// $Id: nullparser.cc 2507 2009-10-17 16:24:02Z matsunaga $
///
/// Copyright (C) 2005-2011 Yusuke Matsunaga
/// All rights reserved.


#include "NullBlifHandler.h"


BEGIN_NAMESPACE_YM_BLIF

// @brief コンストラクタ
NullBlifHandler::NullBlifHandler()
{
}

// @brief デストラクタ
NullBlifHandler::~NullBlifHandler()
{
}

// @brief 初期化
// @retval true 処理が成功した．
// @retval false エラーが起こった．
bool
NullBlifHandler::init()
{
  return true;
}

// @brief .model 文の処理
// @param[in] loc1 .model の位置情報
// @param[in] loc2 文字列の位置情報
// @param[in] name 文字列
// @retval true 処理が成功した．
// @retval false エラーが起こった．
bool
NullBlifHandler::model(const FileRegion& loc1,
		       const FileRegion& loc2,
		       const char* name)
{
  return true;
}

// @brief .inputs 文中の文字列の処理
// @param[in] name_id 文字列のID番号
// @retval true 処理が成功した．
// @retval false エラーが起こった．
bool
NullBlifHandler::inputs_elem(ymuint32 name_id)
{
  return true;
}

// @brief .outputs 文中の文字列の処理
// @param[in] name_id 文字列のID番号
// @retval true 処理が成功した．
// @retval false エラーが起こった．
bool
NullBlifHandler::outputs_elem(ymuint32 name_id)
{
  return true;
}

// @brief .names 文の処理
// @param[in] name_id_array 各識別子のID番号の配列
// @param[in] nc キューブ数
// @param[in] cover_pat 入力カバーを表す文字列
// @param[in] opat 出力の極性
// @retval true 処理が成功した．
// @retval false エラーが起こった．
// @note cover_pat は ni 個ごとに1行のパタンを表す．
// 各要素のとりうる値は '0', '1', '-' を表す．
// @note opat は '0' か '1' のどちらか
bool
NullBlifHandler::names(const vector<ymuint32>& name_id_array,
		       ymuint32 nc,
		       const char* cover_pat,
		       char opat)
{
  return true;
}

// @brief .gate 文の開始
// @param[in] loc1 .gate の位置情報
// @param[in] loc2 セル名の位置情報
// @param[in] name セル名
// @retval true 処理が成功した．
// @retval false エラーが起こった．
bool
NullBlifHandler::gate_begin(const FileRegion& loc1,
			    const FileRegion& loc2,
			    const char* name)
{
  return true;
}

// @brief .gate 文中のピン割り当ての処理
// @param[in] loc1 ピン名の位置情報
// @param[in] f_name ピン名
// @param[in] loc2 ノード名の位置情報
// @param[in] a_name ノード名
// @retval true 処理が成功した．
// @retval false エラーが起こった．
bool
NullBlifHandler::gate_assign(const FileRegion& loc1,
			     const char* f_name,
			     const FileRegion& loc2,
			     const char* a_name)
{
  return true;
}

// @brief .gate 文の終了
bool
NullBlifHandler::gate_end()
{
  return true;
}

// @brief .latch 文の処理
// @param[in] name1_id 最初の識別子のID番号
// @param[in] name2_id 次の識別子のID番号
// @param[in] loc4 リセット値の位置情報
// @param[in] rval リセット時の値('0'/'1') 未定義なら ' '
// @retval true 処理が成功した．
// @retval false エラーが起こった．
bool
NullBlifHandler::latch(ymuint32 name1_id,
		       ymuint32 name2_id,
		       const FileRegion& loc4,
		       char rval)
{
  return true;
}

// @brief .end 文の処理
// @param[in] loc 位置情報
// @retval true 処理が成功した．
// @retval false エラーが起こった．
bool
NullBlifHandler::end(const FileRegion& loc)
{
  return true;
}

// @brief 通常終了時の処理
void
NullBlifHandler::normal_exit()
{
}

// @brief エラー終了時の処理
void
NullBlifHandler::error_exit()
{
}

END_NAMESPACE_YM_BLIF