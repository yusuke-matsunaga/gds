#ifndef BTGEDGE_H
#define BTGEDGE_H

/// @file BtgEdge.h
/// @brief BtgEdge のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2013 Yusuke Matsunaga
/// All rights reserved.


#include "ymtools.h"


BEGIN_NAMESPACE_YM

class BtgNode;

//////////////////////////////////////////////////////////////////////
/// @class BtgEdge BtgEdge.h "BtgEdge.h"
/// @brief BtgMatch の枝を表すクラス
//////////////////////////////////////////////////////////////////////
class BtgEdge
{
public:

  /// @brief コンストラクタ
  /// @param[in] node1 節点グループ1 側のノード
  /// @param[in] node2 節点グループ2 側のノード
  /// @param[in] weight 重み
  BtgEdge(BtgNode* node1,
	  BtgNode* node2,
	  ymuint weight);

  /// @brief デストラクタ
  ~BtgEdge();


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief 節点グループ1 側のノードを返す．
  BtgNode*
  node1() const;

  /// @brief 節点グループ2 側のノードを返す．
  BtgNode*
  node2() const;

  /// @brief 重みを返す．
  ymuint
  weight() const;


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // 節点グループ1 側のノード
  BtgNode* mNode1;

  // 節点グループ2 側のノード
  BtgNode* mNode2;

  // 重み
  ymuint32 mWeight;

};


//////////////////////////////////////////////////////////////////////
// インライン関数の定義
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
// @param[in] node1 節点グループ1 側のノード
// @param[in] node2 節点グループ2 側のノード
// @param[in] weight 重み
BtgEdge::BtgEdge(BtgNode* node1,
		 BtgNode* node2,
		 ymuint weight) :
  mNode1(node1),
  mNode2(node2),
  mWeight(weight)
{
}

// @brief デストラクタ
BtgEdge::~BtgEdge()
{
}

// @brief 節点グループ1 側のノードを返す．
BtgNode*
BtgEdge::node1() const
{
  return mNode1;
}

// @brief 節点グループ2 側のノードを返す．
BtgNode*
BtgEdge::node2() const
{
  return mNode2;
}

// @brief 重みを返す．
ymuint
BtgEdge::weight() const
{
  return mWeight;
}

END_NAMESPACE_YM

#endif // BTGEDGE_H
