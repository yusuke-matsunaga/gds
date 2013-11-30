
/// @file SupOp.cc
/// @brief SupOp の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2005-2011 Yusuke Matsunaga
/// All rights reserved.


#include "SupOp.h"


BEGIN_NAMESPACE_YM_ZDD

//////////////////////////////////////////////////////////////////////
// クラス SupOp
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
// @param[in] mgr マネージャ
SupOp::SupOp(ZddMgrImpl* mgr) :
  ZddOp(mgr)
{
}

// @brief デストラクタ
SupOp::~SupOp()
{
}

// @brief 演算を行う関数
// @param[in] edge_list 根の枝のリスト
// @return edge_list の根からたどれる ZDD のノード数を返す．
ymuint
SupOp::apply(const vector<ZddEdge>& edge_list)
{
  ymuint n = mgr()->max_level();
  mMarks.clear();
  mMarks.resize(n, 0);
  mSupList.clear();
  for (vector<ZddEdge>::const_iterator p = edge_list.begin();
       p != edge_list.end(); ++ p) {
    ZddEdge e = *p;
    apply_sub(e);
  }
  for (vector<ZddEdge>::const_iterator p = edge_list.begin();
       p != edge_list.end(); ++ p) {
    ZddEdge e = *p;
    clear_mark(e);
  }
  sort(mSupList.begin(), mSupList.end());
  return mSupList.size();
}

// @brief サポート変数のリストを得る．
// @param[out] var_list 結果を格納する変数
ymuint
SupOp::to_list(vector<VarId>& var_list)
{
  ymuint n = mSupList.size();
  var_list.resize(n);
  for (ymuint i = 0; i < n; ++ i) {
    ymuint level = mSupList[i];
    var_list[i] = mgr()->varid(level);
  }
  return n;
}

// @brief サポート変数のリストを得る．
// @param[out] var_list 結果を格納する変数
ymuint
SupOp::to_list(list<VarId>& var_list)
{
  ymuint n = mSupList.size();
  for (ymuint i = 0; i < n; ++ i) {
    ymuint level = mSupList[i];
    var_list.push_back(mgr()->varid(level));
  }
  return n;
}

// @brief 次の GC で回収されるノードに関連した情報を削除する．
void
SupOp::sweep()
{
  // 何もしない．
}

// @brief apply() の下請け関数
void
SupOp::apply_sub(ZddEdge e)
{
  // ちょっと分かりにくいけど node->edge1() への再帰を
  // for-loop に変換している．
  for ( ; ; ) {
    ZddNode* node = e.get_node();
    if ( node == 0 || node->mark() ) {
      return;
    }
    node->mark(1);

    ymuint level = node->level();
    if ( mMarks[level] == 0 ) {
      mSupList.push_back(level);
      mMarks[level] = 1;
    }

    apply_sub(node->edge0());
    e = node->edge1();
  }
}

END_NAMESPACE_YM_ZDD
