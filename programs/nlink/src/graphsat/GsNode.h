#ifndef GSNODE_H
#define GSNODE_H

/// @file GsNode.h
/// @brief GsNode のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2015 Yusuke Matsunaga
/// All rights reserved.


#include "nlink_nsdef.h"


BEGIN_NAMESPACE_YM_NLINK

class GsEdge;

//////////////////////////////////////////////////////////////////////
/// @class GsNode GsNode.h "GsNode.h"
/// @brief GraphSat 用の節点を表すクラス
//////////////////////////////////////////////////////////////////////
class GsNode
{
  friend class GsGraph;

public:

  /// @brief コンストラクタ
  GsNode();

  /// @brief デストラクタ
  ~GsNode();


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief ID番号を返す．
  ymuint
  id() const;

  /// @brief 枝数を返す．
  ymuint
  edge_num() const;

  /// @brief 枝を返す．
  /// @param[in] pos 位置番号 ( 0 <= pos < edge_num() )
  GsEdge*
  edge(ymuint pos) const;

  /// @brief 端子マークを得る．
  ymuint
  terminal_mark() const;

  /// @brief 'visited' フラグを返す．
  bool
  visited() const;

  /// @brief 'visited' フラグを消す．
  void
  clear_visited();

  /// @brief 'visited' フラグをつける．
  void
  set_visited();


private:
  //////////////////////////////////////////////////////////////////////
  // 内部で用いられる関数
  //////////////////////////////////////////////////////////////////////


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // ID番号
  ymuint mId;

  // 端子マーク
  // 0: 無印
  // 1: 始点
  // 2: 終点
  ymuint mTermMark;

  // 枝数
  ymuint mEdgeNum;

  // 枝のリスト
  GsEdge** mEdgeList;

  // 探索用の作業フラグ
  bool mVisited;

};


//////////////////////////////////////////////////////////////////////
// インライン関数の定義
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
inline
GsNode::GsNode()
{
}

// @brief デストラクタ
inline
GsNode::~GsNode()
{
}

// @brief ID番号を返す．
inline
ymuint
GsNode::id() const
{
  return mId;
}

// @brief 端子マークを得る．
inline
ymuint
GsNode::terminal_mark() const
{
  return mTermMark;
}

// @brief 枝数を返す．
inline
ymuint
GsNode::edge_num() const
{
  return mEdgeNum;
}

// @brief 枝を返す．
// @param[in] pos 位置番号 ( 0 <= pos < edge_num() )
inline
GsEdge*
GsNode::edge(ymuint pos) const
{
  ASSERT_COND( pos < edge_num() );
  return mEdgeList[pos];
}

// @brief 'visited' フラグを返す．
inline
bool
GsNode::visited() const
{
  return mVisited;
}

// @brief 'visited' フラグを消す．
inline
void
GsNode::clear_visited()
{
  mVisited = false;
}

// @brief 'visited' フラグをつける．
inline
void
GsNode::set_visited()
{
  mVisited = true;
}

END_NAMESPACE_YM_NLINK

#endif // GSNODE_H
