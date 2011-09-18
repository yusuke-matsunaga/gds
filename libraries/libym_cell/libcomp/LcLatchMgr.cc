
/// @file LcLatchMgr.cc
/// @brief LcLatchMgr の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2005-2011 Yusuke Matsunaga
/// All rights reserved.


#include "LcLatchMgr.h"
#include "LcClass.h"
#include "LcGroup.h"
#include "ym_cell/Cell.h"
#include "ym_logic/LogExpr.h"
#include "ym_logic/NpnMapM.h"


BEGIN_NAMESPACE_YM_CELL_LIBCOMP

//////////////////////////////////////////////////////////////////////
// クラスLcLatchMgr
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
// @param[in] libcomp 親の LibComp
LcLatchMgr::LcLatchMgr(LibComp& libcomp) :
  LcGroupMgr(libcomp)
{
}

// @brief デストラクタ
LcLatchMgr::~LcLatchMgr()
{
}

// @brief 初期化する．
void
LcLatchMgr::init()
{
  clear();

  { // クリアなし，プリセットなしのラッチクラスの登録
    vector<TvFunc> f_list;
    f_list.push_back(TvFunc::posi_literal(4, 0));
    f_list.push_back(TvFunc::posi_literal(4, 1));
    f_list.push_back(TvFunc::const_zero(4));
    f_list.push_back(TvFunc::const_zero(4));
    f_list.push_back(TvFunc::posi_literal(4, 2));
    f_list.push_back(TvFunc::const_zero(4));
    f_list.push_back(TvFunc::posi_literal(4, 3));
    f_list.push_back(TvFunc::const_zero(4));
    TvFuncM f(f_list);
    LcGroup* group = find_group(f);
    LcClass* cclass = group->parent();
    mLatchClass[0] = cclass->id();
  }
  { // クリアあり，プリセットなしのラッチクラスの登録
    vector<TvFunc> f_list;
    f_list.push_back(TvFunc::posi_literal(5, 0));
    f_list.push_back(TvFunc::posi_literal(5, 1));
    f_list.push_back(TvFunc::posi_literal(5, 2));
    f_list.push_back(TvFunc::const_zero(5));
    f_list.push_back(TvFunc::posi_literal(5, 3));
    f_list.push_back(TvFunc::const_zero(5));
    f_list.push_back(TvFunc::posi_literal(5, 4));
    f_list.push_back(TvFunc::const_zero(5));
    TvFuncM f(f_list);
    LcGroup* group = find_group(f);
    LcClass* cclass = group->parent();
    mLatchClass[1] = cclass->id();
  }
  { // クリアなし，プリセットありのラッチクラスの登録
    vector<TvFunc> f_list;
    f_list.push_back(TvFunc::posi_literal(5, 0));
    f_list.push_back(TvFunc::posi_literal(5, 1));
    f_list.push_back(TvFunc::const_zero(5));
    f_list.push_back(TvFunc::posi_literal(5, 2));
    f_list.push_back(TvFunc::posi_literal(5, 3));
    f_list.push_back(TvFunc::const_zero(5));
    f_list.push_back(TvFunc::posi_literal(5, 4));
    f_list.push_back(TvFunc::const_zero(5));
    TvFuncM f(f_list);
    LcGroup* group = find_group(f);
    LcClass* cclass = group->parent();
    mLatchClass[2] = cclass->id();
  }
  { // クリアあり，プリセットありのラッチクラスの登録
    vector<TvFunc> f_list;
    f_list.push_back(TvFunc::posi_literal(6, 0));
    f_list.push_back(TvFunc::posi_literal(6, 1));
    f_list.push_back(TvFunc::posi_literal(6, 2));
    f_list.push_back(TvFunc::posi_literal(6, 3));
    f_list.push_back(TvFunc::posi_literal(6, 4));
    f_list.push_back(TvFunc::const_zero(6));
    f_list.push_back(TvFunc::posi_literal(6, 5));
    f_list.push_back(TvFunc::const_zero(6));
    TvFuncM f(f_list);
    LcGroup* group = find_group(f);
    LcClass* cclass = group->parent();
    mLatchClass[3] = cclass->id();
  }
}

// @brief 定義済みのラッチクラス番号を返す．
// @param[in] id 番号
// - 0: クリアなし，プリセットなし
// - 1: クリアあり，プリセットなし
// - 2: クリアなし，プリセットあり
// - 3: クリアあり，プリセットあり
ymuint
LcLatchMgr::latch_class(ymuint id) const
{
  return mLatchClass[id];
}

// @brief セルのシグネチャ関数を作る．
// @param[in] cell セル
// @param[out] f シグネチャ関数
void
LcLatchMgr::gen_signature(const Cell* cell,
		       TvFuncM& f)
{
  // + 2 は状態変数
  ymuint ni2 = cell->input_num2() + 2;
  ymuint no2 = cell->output_num2();

  vector<TvFunc> f_list(no2 * 2 + 4);
  {
    LogExpr expr = cell->data_in_expr();
    f_list[0] = expr.make_tv(ni2);
  }
  {
    LogExpr expr = cell->enable_expr();
    f_list[1] = expr.make_tv(ni2);
  }
  {
    LogExpr expr = cell->clear_expr();
    f_list[2] = expr.make_tv(ni2);
  }
  {
    LogExpr expr = cell->preset_expr();
    f_list[3] = expr.make_tv(ni2);
  }
  for (ymuint i = 0; i < no2; ++ i) {
    LogExpr lexpr = cell->logic_expr(i);
    f_list[i * 2 + 4] = lexpr.make_tv(ni2);
    LogExpr texpr = cell->tristate_expr(i);
    f_list[i * 2 + 5] = texpr.make_tv(ni2);
  }
  f = TvFuncM(f_list);
}

// @brief 代表関数を求める．
// @param[in] f 関数
// @param[out] repfunc 代表関数
// @param[out] xmap 変換
void
LcLatchMgr::find_repfunc(const TvFuncM& f,
			 TvFuncM& repfunc,
			 NpnMapM& xmap)
{
  default_repfunc(f, repfunc, xmap);
}

END_NAMESPACE_YM_CELL_LIBCOMP
