#ifndef GBMSOLVER_H
#define GBMSOLVER_H

/// @file GbmSolver.h
/// @brief GbmSolver のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2013 Yusuke Matsunaga
/// All rights reserved.


#include "ymtools.h"
#include "GbmMgr.h"
#include "GbmNodeHandle.h"
#include "ym_logic/TvFunc.h"


BEGIN_NAMESPACE_YM

//////////////////////////////////////////////////////////////////////
/// @class GbmSolver GbmSolver.h "GbmSolver.h"
/// @brief GBM を解くクラス
//////////////////////////////////////////////////////////////////////
class GbmSolver
{
public:

  /// @brief コンストラクタ
  GbmSolver();

  /// @brief デストラクタ
  virtual
  ~GbmSolver();


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief 単純なマッチング問題を解く
  /// @param[in] mgr GbmMgr
  /// @param[in] output Reconfigurable Network の出力
  /// @param[in] func マッチング対象の関数
  /// @param[out] conf_bits configuration ビットの値を収める配列
  bool
  solve(const GbmMgr& mgr,
	GbmNodeHandle output,
	const TvFunc& func,
	vector<bool>& conf_bits);

  /// @brief 入力順を考慮したマッチング問題を解く
  /// @param[in] mgr GbmMgr
  /// @param[in] output Reconfigurable Network の出力
  /// @param[in] func マッチング対象の関数
  /// @param[out] conf_bits configuration ビットの値を収める配列
  /// @param[out] iorder 入力順序
  /// @note iorder[0] に func の0番めの入力に対応した GbmMgr の入力番号が入る．
  bool
  solve(const GbmMgr& mgr,
	GbmNodeHandle output,
	const TvFunc& func,
	vector<bool>& conf_bits,
	vector<ymuint>& iorder);


private:
  //////////////////////////////////////////////////////////////////////
  // 内部で用いられる関数
  //////////////////////////////////////////////////////////////////////

  /// @brief 実際に問題を解く仮想関数
  /// @param[in] mgr GbmMgr
  /// @param[in] output Reconfigurable Network の出力
  /// @param[in] func マッチング対象の関数
  /// @param[out] conf_bits configuration ビットの値を収める配列
  virtual
  bool
  _solve(const GbmMgr& mgr,
	 GbmNodeHandle output,
	 const TvFunc& func,
	 vector<bool>& conf_bits) = 0;

  /// @brief 入力順を考慮したマッチング問題を解く
  /// @param[in] mgr GbmMgr
  /// @param[in] output Reconfigurable Network の出力
  /// @param[in] func マッチング対象の関数
  /// @param[out] conf_bits configuration ビットの値を収める配列
  /// @param[out] iorder 入力順序
  /// @note iorder[0] に func の0番めの入力に対応した GbmMgr の入力番号が入る．
  virtual
  bool
  _solve(const GbmMgr& mgr,
	 GbmNodeHandle output,
	 const TvFunc& func,
	 vector<bool>& conf_bits,
	 vector<ymuint>& iorder) = 0;

  /// @brief 検証を行う．
  /// @param[in] mgr GbmMgr
  /// @param[in] output Reconfigurable Network の出力
  /// @param[in] func マッチング対象の関数
  /// @param[in] conf_bits configuration ビットの値を収める配列
  /// @param[in] iorder 入力順序
  bool
  verify(const GbmMgr& mgr,
	 GbmNodeHandle output,
	 const TvFunc& func,
	 const vector<bool>& conf_bits,
	 const vector<ymuint>& iorder);

};

END_NAMESPACE_YM

#endif // GBMSOLVER_H
