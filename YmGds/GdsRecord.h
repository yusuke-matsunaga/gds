﻿#ifndef GDS_GDSRECORD_H
#define GDS_GDSRECORD_H

/// @file YmGds/GdsRecord.h
/// @brief GdsRecord のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2005-2012, 2014 Yusuke Matsunaga
/// All rights reserved.


#include "YmGds/gds_nsdef.h"


BEGIN_NAMESPACE_YM_GDS

//////////////////////////////////////////////////////////////////////
/// @class GdsRecord GdsRecord.h "YmGds/GdsRecord.h"
/// @brief GDS-II のひとつのレコードを表すクラス
//////////////////////////////////////////////////////////////////////
class GdsRecord
{
  friend class GdsRecMgr;

private:
  //////////////////////////////////////////////////////////////////////
  // 可変サイズなので無闇に生成/破壊させない．
  //////////////////////////////////////////////////////////////////////

  /// @brief コンストラクタ
  GdsRecord();

  /// @brief デストラクタ
  ~GdsRecord();


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief 先頭のオフセットを取り出す．
  ymuint32
  offset() const;

  /// @brief サイズを取り出す．
  ymuint32
  size() const;

  /// @brief レコード型を取り出す．
  GdsRtype
  rtype() const;

#if 0
  /// @brief レコード型を表すトークンを返す．
  int
  rtype_token() const;
#endif

  /// @brief レコード型を表す文字列を返す．
  const char*
  rtype_string() const;

  /// @brief データ型を取り出す．
  GdsDtype
  dtype() const;

  /// @brief データ型を表す文字列を返す．
  const char*
  dtype_string() const;

  /// @brief データサイズを取り出す．
  ymuint32
  dsize() const;

  /// @brief データバッファを取り出す．
  const ymuint8*
  data() const;

  /// @brief pos 番目のバイトデータを返す．
  /// @param[in] pos 位置
  ymuint8
  get_1byte(ymuint32 pos) const;

  /// @brief pos 番目の 2バイトのデータを符号つき数(2の補数表現)に変換する．
  /// @param[in] pos 位置
  /// @note kGds2Int 用の変換関数
  ymint16
  get_2byte_int(ymuint32 pos) const;

  /// @brief pos 番目の 4バイトのデータを符号つき数(2の補数表現)に変換する．
  /// @param[in] pos 位置
  /// @note kGds4Int 用の変換関数
  ymint32
  get_4byte_int(ymuint32 pos) const;

  /// @brief pos 番目の 4バイトのデータを浮動小数点数に変換する．
  /// @param[in] pos 位置
  /// @note kGds4Real 用の変換関数
  double
  get_4byte_real(ymuint32 pos) const;

  /// @brief pos 番目の 8バイトのデータを浮動小数点数に変換する．
  /// @param[in] pos 位置
  /// @note kGds8Real 用の変換関数
  double
  get_8byte_real(ymuint32 pos) const;

  /// @brief データを文字列に変換する．
  /// @note 末尾に '\\0'(EndOfString) があるとは限らない．
  /// @note GdsString 用の変換関数
  string
  get_string() const;


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // このレコードの先頭のオフセット
  ymuint32 mOffset;

  // このレコード全体のサイズ
  ymuint32 mSize;

  // レコード型
  GdsRtype mRtype;

  // データ型
  GdsDtype mDtype;

  // データ本体を格納する配列(のダミー定義)
  ymuint8 mData[1];

};


//////////////////////////////////////////////////////////////////////
// インライン関数の定義
//////////////////////////////////////////////////////////////////////

// 先頭のオフセットを取り出す．
inline
ymuint32
GdsRecord::offset() const
{
  return mOffset;
}

// サイズを取り出す．
inline
ymuint32
GdsRecord::size() const
{
  return mSize;
}

// レコード型を取り出す．
inline
GdsRtype
GdsRecord::rtype() const
{
  return mRtype;
}

// データ型を取り出す．
inline
GdsDtype
GdsRecord::dtype() const
{
  return mDtype;
}

// データサイズを取り出す．
inline
ymuint32
GdsRecord::dsize() const
{
  return size() - 4;
}

// @brief データバッファを取り出す．
inline
const ymuint8*
GdsRecord::data() const
{
  return mData;
}

// pos 番目のバイトデータを返す．
inline
ymuint8
GdsRecord::get_1byte(ymuint32 pos) const
{
  return mData[pos];
}


END_NAMESPACE_YM_GDS

#endif
