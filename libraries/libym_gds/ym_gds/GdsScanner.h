#ifndef YM_GDS_GDSSCANNER_H
#define YM_GDS_GDSSCANNER_H

/// @file ym_gds/GdsScanner.h
/// @brief GdsScanner のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2005-2012 Yusuke Matsunaga
/// All rights reserved.


#include "ym_gds/gds_nsdef.h"


BEGIN_NAMESPACE_YM_GDS

//////////////////////////////////////////////////////////////////////
/// @class GdsScanner GdsScanner.h "ym_gds/GdsScanner.h"
/// @brief GDS-II の読み込みを行うクラス
//////////////////////////////////////////////////////////////////////
class GdsScanner
{
public:

  /// @brief コンストラクタ
  /// @param[in] is 入力ストリーム
  /// @param[in] mgr GdsRecord 管理用のオブジェクト
  GdsScanner(istream& is,
	     GdsRecMgr& mgr);

  /// @brief デストラクタ
  ~GdsScanner();


public:

  /// @brief レコード一つ分の読み込みを行う．
  /// @retval record 読み込んだレコード
  /// @retval NULL エラーが起った場合や末尾に達した場合
  GdsRecord*
  read_rec();

  /// @brief 現在の位置を返す．
  ymuint32
  cur_pos() const;


private:

  // ストリームから1バイト読んで符号なし整数に変換する．
  ymuint8
  read_1byte_uint();

  // ストリームから2バイト読んで符号なし整数に変換する．
  ymuint16
  read_2byte_uint();


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // 入力ストリーム
  istream& mIs;

  // 入力ストリームから読み込んだバイト数
  ymuint32 mPos;

  // GdsRecord の管理用オブジェクト
  GdsRecMgr& mMgr;

};

END_NAMESPACE_YM_GDS

#endif // YM_GDS_GDSSCANNER_H
