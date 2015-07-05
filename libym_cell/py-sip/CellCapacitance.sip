
/// @file CellCapacitance.sip
/// @brief CellCapacitance の sip ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2014 Yusuke Matsunaga
/// All rights reserved.

class CellCapacitance
{
%TypeHeaderCode
#include "cell/CellCapacitance.h"
using nsYm::CellCapacitance;
%End

public:

  /// @brief 空のコンストラクタ
  CellCapacitance();

  /// @brief double からの変換コンストラクタ
  CellCapacitance(double v);

  /// @brief 無限大の値を作る．
  static
  CellCapacitance
  infty();

  /// @brief 値を得る．
  double
  value() const;

  /// @brief 加算
  CellCapacitance
  operator+(const CellCapacitance& src) const;

  /// @brief 減算
  CellCapacitance
  operator-(const CellCapacitance& src) const;

};

/// @brief 等価比較演算子
/// @param[in] left, right オペランド
/// @relates CellCapacitance
bool
operator==(const CellCapacitance& left,
	   const CellCapacitance& right);

/// @brief 大小比較演算子
/// @param[in] left, right オペランド
/// @relates CellCapacitance
bool
operator<(const CellCapacitance& left,
	  const CellCapacitance& right);
