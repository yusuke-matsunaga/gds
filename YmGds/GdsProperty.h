﻿#ifndef GDS_GDSPROPERTY_H
#define GDS_GDSPROPERTY_H

/// @file YmGds/GdsProperty.h
/// @brief GdsProperty のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2005-2012, 2014 Yusuke Matsunaga
/// All rights reserved.


#include "YmGds/gds_nsdef.h"
#include "YmGds/GdsString.h"


BEGIN_NAMESPACE_YM_GDS

//////////////////////////////////////////////////////////////////////
/// @class GdsProperty GdsProperty.h "GdsProperty.h"
/// @brief (PROPATTR, PROPVALUE) を表すクラス
//////////////////////////////////////////////////////////////////////
class GdsProperty
{
  friend class GdsParser;

private:

  /// @brief コンストラクタ
  /// @param[in] attr PROPATTR の値
  /// @param[in] value PROPVALUE の値
  GdsProperty(ymuint attr,
	      GdsString* value);

  /// @brief デストラクタ
  ~GdsProperty();


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief PROPATTR の値を返す．
  ymuint
  attr() const;

  /// @brief PROPVALUE の値を返す．
  const char*
  value() const;

  /// @brief 次の要素を返す．
  const GdsProperty*
  next();


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // PROPATTR の値
  ymuint8 mAttr;

  // PROPVALUE の値
  GdsString* mValue;

  // 次の要素
  GdsProperty* mLink;

};


//////////////////////////////////////////////////////////////////////
// インライン関数の定義
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
// @param[in] attr PROPATTR の値
// @param[in] value PROPVALUE の値
inline
GdsProperty::GdsProperty(ymuint attr,
			 GdsString* value) :
  mAttr(attr),
  mValue(value),
  mLink(NULL)
{
}

// @brief デストラクタ
inline
GdsProperty::~GdsProperty()
{
}

// @brief PROPATTR の値を返す．
inline
ymuint
GdsProperty::attr() const
{
  return mAttr;
}

// @brief PROPVALUE の値を返す．
inline
const char*
GdsProperty::value() const
{
  return mValue->str();
}

// @brief 次の要素を返す．
inline
const GdsProperty*
GdsProperty::next()
{
  return mLink;
}

END_NAMESPACE_YM_GDS

#endif // GDS_GDSPROPERTY_H
