#ifndef TESTBLIFHANDLER_H
#define TESTBLIFHANDLER_H

/// @file TestBlifHandler.h
/// @brief TestBlifHandler のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2005-2011 Yusuke Matsunaga
/// All rights reserved.


#include "BlifHandler.h"


BEGIN_NAMESPACE_YM_BLIF

//////////////////////////////////////////////////////////////////////
/// @class TestBlifHandler TestBlifHandler.h
/// @brief テスト用の BlifHandler
//////////////////////////////////////////////////////////////////////
class TestBlifHandler :
  public BlifHandler
{
public:

  /// @brief コンストラクタ
  TestBlifHandler(ostream* stream);

  /// @brief デストラクタ
  virtual
  ~TestBlifHandler();


public:

  /// @brief 初期化
  virtual
  bool
  init();

  /// @brief .model 文の処理
  /// @param[in] loc1 .model の位置情報
  /// @param[in] loc2 文字列の位置情報
  /// @param[in] name 文字列
  /// @retval true 処理が成功した．
  /// @retval false エラーが起こった．
  virtual
  bool
  model(const FileRegion& loc1,
	const FileRegion& loc2,
	const char* name);

  /// @brief .inputs 文中の文字列の処理
  /// @param[in] name_id 文字列の情報
  /// @retval true 処理が成功した．
  /// @retval false エラーが起こった．
  virtual
  bool
  inputs_elem(ymuint32 name_id);

  /// @brief .outputs 文中の文字列の処理
  /// @param[in] name 文字列の情報
  /// @retval true 処理が成功した．
  /// @retval false エラーが起こった．
  virtual
  bool
  outputs_elem(ymuint32 name_id);

  /// @brief .names 文の処理
  /// @param[in] name_id_array 各識別子のID番号の配列
  /// @param[in] nc キューブ数
  /// @param[in] cover_pat 入力カバーを表す文字列
  /// @param[in] opat 出力の極性
  /// @retval true 処理が成功した．
  /// @retval false エラーが起こった．
  /// @note cover_pat は ni 個ごとに1行のパタンを表す．
  /// 各要素のとりうる値は '0', '1', '-' を表す．
  /// @note opat は '0' か '1' のどちらか
  virtual
  bool
  names(const vector<ymuint32>& name_id_array,
	ymuint32 nc,
	const char* cover_pat,
	char opat);

  /// @brief .gate 文の処理
  /// @param[in] cell セル
  /// @param[in] onode_id 出力ノードのID番号
  /// @param[in] inode_id_array 入力ノードのID番号の配列
  /// @retval true 処理が成功した．
  /// @retval false エラーが起こった．
  virtual
  bool
  gate(const Cell* cell,
       ymuint32 onode_id,
       const vector<ymuint32>& inode_id_array);

#if 0
  /// @brief .gate 文の開始
  /// @param[in] loc1 .gate の位置情報
  /// @param[in] loc2 セル名の位置情報
  /// @param[in] cell セル
  /// @retval true 処理が成功した．
  /// @retval false エラーが起こった．
  virtual
  bool
  gate_begin(const FileRegion& loc1,
	     const FileRegion& loc2,
	     const Cell* cell);

  /// @brief .gate 文中のピン割り当ての処理
  /// @param[in] loc1 ピン名の位置情報
  /// @param[in] pin ピン
  /// @param[in] name_id ノード名のID番号
  /// @retval true 処理が成功した．
  /// @retval false エラーが起こった．
  virtual
  bool
  gate_assign(const FileRegion& loc1,
	      const CellPin* pin,
	      ymuint name_id);

  /// @brief .gate 文の終了
  virtual
  bool
  gate_end();
#endif

  /// @brief .latch 文の処理
  /// @param[in] name1_id 最初の識別子のID番号
  /// @param[in] name2_id 次の識別子のID番号
  /// @param[in] loc4 リセット値の位置情報
  /// @param[in] rval リセット時の値('0'/'1') 未定義なら ' '
  /// @retval true 処理が成功した．
  /// @retval false エラーが起こった．
  virtual
  bool
  latch(ymuint32 name1_id,
	ymuint32 name2_id,
	const FileRegion& loc4,
	char rval);

  /// @brief .end 文の処理
  /// @param[in] loc 位置情報
  /// @retval true 処理が成功した．
  /// @retval false エラーが起こった．
  virtual
  bool
  end(const FileRegion& loc);

  /// @brief 通常終了時の処理
  virtual
  void
  normal_exit();

  /// @brief エラー終了時の処理
  virtual
  void
  error_exit();


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  ostream* mStreamPtr;

};

END_NAMESPACE_YM_BLIF

#endif // TESTBLIFHANLDER_H
