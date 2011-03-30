
/// @file libym_bdn/BdNetwork.cc
/// @brief BdNetwork の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2005-2011 Yusuke Matsunaga
/// All rights reserved.


#include "ym_bdn/BdNetwork.h"
#include "ym_bdn/BdnPort.h"
#include "ym_bdn/BdnDff.h"
#include "ym_bdn/BdnLatch.h"
#include "ym_bdn/BdnNode.h"
#include "ym_bdn/BdnNodeHandle.h"
#include "BdnAuxData.h"


BEGIN_NAMESPACE_YM_BDN

///////////////////////////////////////////////////////////////////////
// クラス BdNetwork
///////////////////////////////////////////////////////////////////////

// コンストラクタ
BdNetwork::BdNetwork() :
  mAlloc(4096),
  mHashTable(NULL),
  mHashSize(0),
  mAvailLogic(NULL),
  mLevel(0U)

{
  alloc_table(1024);
}

// コピーコンストラクタ
BdNetwork::BdNetwork(const BdNetwork& src) :
  mAlloc(4096),
  mHashTable(NULL),
  mHashSize(0),
  mAvailLogic(NULL),
  mLevel(0U)
{
  copy(src);
}

// 代入演算子
const BdNetwork&
BdNetwork::operator=(const BdNetwork& src)
{
  if ( &src != this ) {
    clear();
    copy(src);
  }
  return *this;
}

// デストラクタ
BdNetwork::~BdNetwork()
{
  clear();
  delete [] mHashTable;
}

// 複製する．
void
BdNetwork::copy(const BdNetwork& src)
{
  ymuint n = src.max_node_id();
  vector<BdnNode*> nodemap(n);

  // ポートの生成
  ymuint np = src.port_num();
  for (ymuint i = 0; i < np; ++ i) {
    const BdnPort* src_port = src.port(i);
    ymuint bw = src_port->bit_width();
    BdnPort* dst_port = new_port(src_port->name(), bw);
    for (ymuint j = 0; j < bw; ++ j) {
      const BdnNode* src_input = src_port->input(j);
      if ( src_input ) {
	BdnNode* dst_input = new_port_input(dst_port, j);
	nodemap[src_input->id()] = dst_input;
      }
      const BdnNode* src_output = src_port->output(j);
      if ( src_output ) {
	BdnNode* dst_output = new_port_output(dst_port, j);
	nodemap[src_output->id()] = dst_output;
      }
    }
  }

  // D-FFノードの生成
  const BdnDffList& src_dff_list = src.dff_list();
  for (BdnDffList::const_iterator p = src_dff_list.begin();
       p != src_dff_list.end(); ++ p) {
    const BdnDff* src_dff = *p;
    BdnDff* dst_dff = new_dff(src_dff->name());

    const BdnNode* src_output = src_dff->output();
    BdnNode* dst_output = dst_dff->output();
    nodemap[src_output->id()] = dst_output;

    const BdnNode* src_input = src_dff->input();
    BdnNode* dst_input = dst_dff->input();
    nodemap[src_input->id()] = dst_input;

    const BdnNode* src_clock = src_dff->clock();
    BdnNode* dst_clock = dst_dff->clock();
    nodemap[src_clock->id()] = dst_clock;

    const BdnNode* src_set = src_dff->set();
    BdnNode* dst_set = dst_dff->set();
    nodemap[src_set->id()] = dst_set;

    const BdnNode* src_reset = src_dff->reset();
    BdnNode* dst_reset = dst_dff->reset();
    nodemap[src_reset->id()] = dst_reset;
  }

  // ラッチノードの生成
  const BdnLatchList& src_latch_list = src.latch_list();
  for (BdnLatchList::const_iterator p = src_latch_list.begin();
       p != src_latch_list.end(); ++ p) {
    const BdnLatch* src_latch = *p;
    BdnLatch* dst_latch = new_latch(src_latch->name());

    const BdnNode* src_output = src_latch->output();
    BdnNode* dst_output = dst_latch->output();
    nodemap[src_output->id()] = dst_output;

    const BdnNode* src_input = src_latch->input();
    BdnNode* dst_input = dst_latch->input();
    nodemap[src_input->id()] = dst_input;

    const BdnNode* src_enable = src_latch->enable();
    BdnNode* dst_enable = dst_latch->enable();
    nodemap[src_enable->id()] = dst_enable;
  }

  // 論理ノードの生成
  vector<BdnNode*> node_list;
  src.sort(node_list);
  ymuint nl = node_list.size();
  for (ymuint i = 0; i < nl; ++ i) {
    BdnNode* src_node = node_list[i];

    BdnNode* src_inode0 = src_node->fanin0();
    BdnNode* input0 = nodemap[src_inode0->id()];
    assert_cond(input0, __FILE__, __LINE__);

    BdnNode* src_inode1 = src_node->fanin1();
    BdnNode* input1 = nodemap[src_inode1->id()];
    assert_cond(input1, __FILE__, __LINE__);

    BdnNodeHandle tmp_h = new_logic(src_node->fcode(),
				    BdnNodeHandle(input0, false),
				    BdnNodeHandle(input1, false));
    assert_cond(tmp_h.inv() == false, __FILE__, __LINE__);
    BdnNode* dst_node = tmp_h.node();
    nodemap[src_node->id()] = dst_node;
  }

  // 外部出力ノードのファンインの設定
  const BdnNodeList& src_output_list = src.output_list();
  for (BdnNodeList::const_iterator p = src_output_list.begin();
       p != src_output_list.end(); ++ p) {
    const BdnNode* src_onode = *p;
    const BdnNode* src_inode = src_onode->output_fanin();
    BdnNode* dst_inode = NULL;
    if ( src_inode ) {
      dst_inode = nodemap[src_inode->id()];
    }
    BdnNodeHandle dst_handle(dst_inode, src_onode->output_fanin_inv());
    BdnNode* dst_onode = nodemap[src_onode->id()];
    set_output_fanin(dst_onode, dst_handle);
  }
}

// 空にする．
void
BdNetwork::clear()
{
  mName = string();
  mPortArray.clear();
  mDffItvlMgr.clear();
  mDffList.clear();
  mLatchItvlMgr.clear();
  mLatchList.clear();
  mNodeItvlMgr.clear();
  mInputList.clear();
  mOutputList.clear();
  mLnodeList.clear();
  mLevel = 0U;

  // mNodeArray, mDffArray, mLatchArray はクリアしない．
}

BEGIN_NONAMESPACE

// sort() の下請け関数
// node のファンアウトのうち，すべてのファンインがマークされている
// ノードを node_list に追加する．
void
sort_sub(BdnNode* node,
	 vector<bool>& marks,
	 vector<BdnNode*>& node_list)
{
  const BdnFanoutList& fo_list = node->fanout_list();
  for (BdnFanoutList::const_iterator p = fo_list.begin();
       p != fo_list.end(); ++ p) {
    BdnEdge* e = *p;
    BdnNode* onode = e->to();
    if ( !marks[onode->id()] && onode->is_logic() &&
	 marks[onode->fanin0()->id()] &&
	 marks[onode->fanin1()->id()] ) {
      marks[onode->id()] = true;
      node_list.push_back(onode);
    }
  }
}

END_NONAMESPACE

// @brief ソートされたノードのリストを得る．
void
BdNetwork::sort(vector<BdnNode*>& node_list) const
{
  node_list.clear();
  node_list.reserve(lnode_num());

  // 処理済みの印を表す配列
  ymuint n = max_node_id();
  vector<bool> marks(n, false);

  // 外部入力のみをファンインにするノードを node_list に追加する．
  for (BdnNodeList::const_iterator p = mInputList.begin();
       p != mInputList.end(); ++ p) {
    BdnNode* node = *p;
    marks[node->id()] = true;
    sort_sub(node, marks, node_list);
  }
  // node_list のノードを一つずつとりだし，ファンアウトが node_list
  // に積めるかチェックする．
  for (ymuint rpos = 0; rpos < node_list.size(); ++ rpos) {
    BdnNode* node = node_list[rpos];
    sort_sub(node, marks, node_list);
  }
  // うまくいっていれば全ての論理ノードが node_list に入っているはず．
  assert_cond(node_list.size() == lnode_num(), __FILE__, __LINE__);
}


BEGIN_NONAMESPACE

// rsort() の下請け関数
// node のすべてのファンアウトがマークされていたら node_list に積む．
void
rsort_sub(BdnNode* node,
	  vector<bool>& marks,
	  vector<BdnNode*>& node_list)
{
  if ( node == NULL ||
       !node->is_logic() ||
       marks[node->id()] ) return;

  const BdnFanoutList& fo_list = node->fanout_list();
  for (BdnFanoutList::const_iterator p = fo_list.begin();
       p != fo_list.end(); ++ p) {
    BdnEdge* e = *p;
    BdnNode* onode = e->to();
    if ( !marks[onode->id()] ) {
      return;
    }
  }
  marks[node->id()] = true;
  node_list.push_back(node);
}

END_NONAMESPACE

// @brief 逆順でソートされたノードのリストを得る．
void
BdNetwork::rsort(vector<BdnNode*>& node_list) const
{
  node_list.clear();
  node_list.reserve(lnode_num());

  // 処理済みの印をあらわす配列
  ymuint n = max_node_id();
  vector<bool> marks(n, false);

  // 外部出力のみをファンアウトにするノードを node_list に追加する．
  for (BdnNodeList::const_iterator p = mOutputList.begin();
       p != mOutputList.end(); ++ p) {
    BdnNode* node = *p;
    marks[node->id()] = true;
    BdnNode* inode = node->output_fanin();
    rsort_sub(inode, marks, node_list);
  }

  // node_list からノードを取り出し，同様の処理を行う．
  for (ymuint rpos = 0; rpos < node_list.size(); ++ rpos) {
    BdnNode* node = node_list[rpos];
    rsort_sub(node->fanin0(), marks, node_list);
    rsort_sub(node->fanin1(), marks, node_list);
  }
  // うまくいっていればすべての論理ノードが node_list に入っているはず．
  assert_cond(node_list.size() == lnode_num(), __FILE__, __LINE__);
}

// @brief ポートを作る．
BdnPort*
BdNetwork::new_port(const string& name,
		    ymuint bit_width)
{
  void* p = mAlloc.get_memory(sizeof(BdnPort));
  void* q = mAlloc.get_memory(sizeof(BdnNode*) * bit_width);
  BdnNode** input_array = new (q) BdnNode*[bit_width];
  void* r = mAlloc.get_memory(sizeof(BdnNode*) * bit_width);
  BdnNode** output_array = new (r) BdnNode*[bit_width];
  BdnPort* port = new (p) BdnPort(name, bit_width, input_array, output_array);

  for (ymuint i = 0; i < bit_width; ++ i) {
    port->mInputArray[i] = NULL;
    port->mOutputArray[i] = NULL;
  }

  port->mId = mPortArray.size();
  mPortArray.push_back(port);

  return port;
}

// @brief D-FF を作る．
// @param[in] name 名前
// @return 生成されたD-FFを返す．
BdnDff*
BdNetwork::new_dff(const string& name)
{
  // 空いているIDを探して配列へ登録
  int id = mDffItvlMgr.avail_num();
  assert_cond( id >= 0, __FILE__, __LINE__);
  mDffItvlMgr.erase(id);

  ymuint uid = static_cast<ymuint>(id);
  while( mDffArray.size() <= uid ) {
    // mDffArray の大きさが小さいときのための埋め草
    BdnDff* dff = alloc_dff();
    dff->mId = mDffArray.size();
    mDffArray.push_back(dff);
  }
  BdnDff* dff = mDffArray[uid];

  dff->mName = name;

  BdnNode* output = dff->output();
  reg_node(output);
  mInputList.push_back(output);

  BdnNode* input = dff->input();
  reg_node(input);
  mOutputList.push_back(input);

  BdnNode* clock = dff->clock();
  reg_node(clock);
  mOutputList.push_back(clock);

  BdnNode* set = dff->set();
  reg_node(set);
  mOutputList.push_back(set);

  BdnNode* reset = dff->reset();
  reg_node(reset);
  mOutputList.push_back(reset);

  mDffList.push_back(dff);

  return dff;
}

// @brief ラッチを作る．
// @param[in] name 名前
// @return 生成されたラッチを返す．
BdnLatch*
BdNetwork::new_latch(const string& name)
{
  // 空いているIDを探して配列へ登録する．
  int id = mLatchItvlMgr.avail_num();
  assert_cond( id >= 0, __FILE__, __LINE__);
  mLatchItvlMgr.erase(id);

  ymuint uid = static_cast<ymuint>(id);
  while ( mLatchArray.size() <= uid ) {
    // mLatchArray の大きさが小さいときのための埋め草
    BdnLatch* latch = alloc_latch();
    latch->mId = mLatchArray.size();
    mLatchArray.push_back(latch);
  }
  BdnLatch* latch = mLatchArray[uid];

  mLatchList.push_back(latch);

  latch->mName = name;

  BdnNode* output = latch->output();
  reg_node(output);
  mInputList.push_back(output);

  BdnNode* input = latch->input();
  reg_node(input);
  mOutputList.push_back(input);

  BdnNode* enable = latch->enable();
  reg_node(enable);
  mOutputList.push_back(enable);

  return latch;
}

// @brief 外部入力を作る．
// @param[in] port ポート
// @param[in] bitpos ビット位置
// @return 作成したノードを返す．
// @note エラー条件は以下の通り
//  - bitpos が port のビット幅を越えている．
//  - port の bitpos にすでにノードがある．
BdnNode*
BdNetwork::new_port_input(BdnPort* port,
			  ymuint bitpos)
{
  assert_cond( bitpos < port->bit_width(), __FILE__, __LINE__);
  assert_cond( port->mInputArray[bitpos] == NULL, __FILE__, __LINE__);

  BdnNode* node = alloc_portnode(port, bitpos);
  node->set_type(BdnNode::kINPUT);
  port->mInputArray[bitpos] = node;

  reg_node(node);

  mInputList.push_back(node);

  return node;
}

// @brief 外部出力ノードを作る．
// @param[in] port ポート
// @param[in] bitpos ビット位置
// @return 作成したノードを返す．
// @note エラー条件は以下の通り
//  - bitpos が port のビット幅を越えている．
//  - port の bitpos にすでにノードがある．
BdnNode*
BdNetwork::new_port_output(BdnPort* port,
			   ymuint bitpos)
{
  assert_cond( bitpos < port->bit_width(), __FILE__, __LINE__);
  assert_cond( port->mOutputArray[bitpos] == NULL, __FILE__, __LINE__);

  BdnNode* node = alloc_portnode(port, bitpos);
  node->set_type(BdnNode::kOUTPUT);
  port->mOutputArray[bitpos] = node;

  mOutputList.push_back(node);

  return node;
}

// @brief 出力ノードの内容を変更する
// @param[in] 変更対象の出力ノード
// @param[in] inode 入力のノード
void
BdNetwork::set_output_fanin(BdnNode* node,
			    BdnNodeHandle inode_handle)
{
  assert_cond( node->is_output(), __FILE__, __LINE__);

  bool inv = inode_handle.inv();
  node->set_inv(inv);

  BdnNode* inode = inode_handle.node();
  connect(inode, node, 0);
}

// @brief 論理ノードを作る．
// @param[in] fcode 機能コード
// @param[in] inode1_handle 1番めの入力ノード+極性
// @param[in] inode2_handle 2番めの入力ノード+極性
// @return 作成したノードを返す．
// @note fcode の出力極性を正規化する．
// @note すでに同じ機能コード，同じファンインを持つノードがあればそれを返す．
BdnNodeHandle
BdNetwork::new_logic(ymuint fcode,
		     BdnNodeHandle inode1_handle,
		     BdnNodeHandle inode2_handle)
{
  return set_logic(NULL, fcode, inode1_handle, inode2_handle);
}

// @brief AND ノードを作る．
// @param[in] inode1_handle 1番めの入力ノード+極性
// @param[in] inode2_handle 2番めの入力ノード+極性
// @return 作成したノードを返す．
// @note すでに構造的に同じノードがあればそれを返す．
BdnNodeHandle
BdNetwork::new_and(BdnNodeHandle inode1_handle,
		   BdnNodeHandle inode2_handle)
{
  return new_logic(0x8, inode1_handle, inode2_handle);
}

// @brief AND ノードを作る．
// @param[in] inode_handle_list 入力ノード+極性のリスト
// @return 作成したノードを返す．
// @note すでに構造的に同じノードがあればそれを返す．
BdnNodeHandle
BdNetwork::new_and(const vector<BdnNodeHandle>& inode_handle_list)
{
  return make_tree(0x8, 0, inode_handle_list.size(), inode_handle_list);
}

// @brief OR ノードを作る．
// @param[in] inode1_handle 1番めの入力ノード+極性
// @param[in] inode2_handle 2番めの入力ノード+極性
// @return 作成したノードを返す．
// @note すでに構造的に同じノードがあればそれを返す．
BdnNodeHandle
BdNetwork::new_or(BdnNodeHandle inode1_handle,
		  BdnNodeHandle inode2_handle)
{
  return new_logic(0xe, inode1_handle, inode2_handle);
}

// @brief OR ノードを作る．
// @param[in] inode_handle_list 入力ノード+極性のリスト
// @return 作成したノードを返す．
// @note すでに構造的に同じノードがあればそれを返す．
BdnNodeHandle
BdNetwork::new_or(const vector<BdnNodeHandle>& inode_handle_list)
{
  return make_tree(0xe, 0, inode_handle_list.size(), inode_handle_list);
}

// @brief XOR ノードを作る．
// @param[in] inode1_handle 1番めの入力ノード+極性
// @param[in] inode2_handle 2番めの入力ノード+極性
// @return 作成したノードを返す．
// @note すでに構造的に同じノードがあればそれを返す．
BdnNodeHandle
BdNetwork::new_xor(BdnNodeHandle inode1_handle,
		   BdnNodeHandle inode2_handle)
{
  return new_logic(0x6, inode1_handle, inode2_handle);
}

// @brief XOR ノードを作る．
// @param[in] inode_handle_list 入力ノード+極性のリスト
// @return 作成したノードを返す．
// @note すでに構造的に同じノードがあればそれを返す．
BdnNodeHandle
BdNetwork::new_xor(const vector<BdnNodeHandle>& inode_handle_list)
{
  return make_tree(0x6, 0, inode_handle_list.size(), inode_handle_list);
}

// @brief バランス木を作る．
// @param[in] fcode 機能コード
// @param[in] start 開始位置
// @param[in] num 要素数
// @param[in] node_list ノードのリスト
BdnNodeHandle
BdNetwork::make_tree(ymuint fcode,
		     ymuint start,
		     ymuint num,
		     const vector<BdnNodeHandle>& node_list)
{
  switch ( num ) {
  case 0:
    assert_not_reached(__FILE__, __LINE__);

  case 1:
    return node_list[0];

  case 2:
    return new_logic(fcode, node_list[0], node_list[1]);

  default:
    break;
  }

  ymuint nh = num / 2;
  BdnNodeHandle l = make_tree(fcode, start, nh, node_list);
  BdnNodeHandle r = make_tree(fcode, start + nh, num - nh, node_list);
  return new_logic(fcode, l, r);
}

// @brief 論理ノードの内容を変更する．
// @param[in] node 変更対象の論理ノード
// @param[in] fcode 機能コード
// @param[in] inode1_handle 1番めの入力ノード+極性
// @param[in] inode2_handle 2番めの入力ノード+極性
// @note fcode の出力極性を正規化する．
// @note 実際には新しいノードを作ってそこへのリンクを内部で持つ．
void
BdNetwork::change_logic(BdnNode* node ,
			ymuint fcode,
			BdnNodeHandle inode1_handle,
			BdnNodeHandle inode2_handle)
{
  BdnNodeHandle new_handle = set_logic(node, fcode,
				       inode1_handle, inode2_handle);
  if ( new_handle == BdnNodeHandle(node, false) ) {
    // 変化なし
    return;
  }

  // node のファンアウト先の内容を変える．
  const BdnFanoutList& fo_list = node->fanout_list();
  for (BdnFanoutList::const_iterator p = fo_list.begin();
       p != fo_list.end(); ++ p) {
    BdnEdge* edge = *p;
    BdnNode* onode = edge->to();
    if ( onode->is_logic() ) {
      BdnNodeHandle inode1_handle(onode->fanin0());
      BdnNodeHandle inode2_handle(onode->fanin1());
      if ( edge->pos() == 0 ) {
	inode1_handle = new_handle;
      }
      else {
	inode2_handle = new_handle;
      }
      change_logic(onode, onode->fcode(), inode1_handle, inode2_handle);
    }
    else if ( onode->is_output() ) {
      set_output_fanin(onode, new_handle);
    }
  }
}


BEGIN_NONAMESPACE

// BdnNode のハッシュ関数
inline
ymuint
hash_func(ymuint fcode,
	  const BdnNode* node1,
	  const BdnNode* node2)
{
  return (node1->id() * 3 + node2->id() << 3) | (fcode >> 1);
}

END_NONAMESPACE


// 論理ノードの内容を設定する．
BdnNodeHandle
BdNetwork::set_logic(BdnNode* node,
		     ymuint fcode,
		     BdnNodeHandle inode0_handle,
		     BdnNodeHandle inode1_handle)
{
  // 入力の極性を fcode に反映させる．
  if ( inode0_handle.inv() ) {
    ymuint tmp0 = fcode & 0x5; // 0101
    ymuint tmp1 = fcode & 0xa; // 1010
    fcode = (tmp0 << 1) | (tmp1 >> 1);
  }
  if ( inode1_handle.inv() ) {
    ymuint tmp0 = fcode & 0x3; // 0011
    ymuint tmp1 = fcode & 0xc; // 1100
    fcode = (tmp0 << 2) | (tmp1 >> 2);
  }

  BdnNode* inode0 = inode0_handle.node();
  BdnNode* inode1 = inode1_handle.node();

  // 境界条件の検査
  if ( inode0 == NULL ) {
    if ( inode1 == NULL ) {
      if ( (fcode & 1U) == 1U ) {
	return BdnNodeHandle::make_one();
      }
      else {
	return BdnNodeHandle::make_zero();
      }
    }
    if ( (fcode & 1U) == 1U ) {
      if ( (fcode & 4U) == 4U ) {
	// 11 なので定数1
	return BdnNodeHandle::make_one();
      }
      else {
	// 10 なので ~(inode1)
	return BdnNodeHandle(inode1, true);
      }
    }
    else {
      if ( (fcode & 4U) == 4U ) {
	// 01 なので inode1
	return BdnNodeHandle(inode1, false);
      }
      else {
	// 00 なので定数0
	return BdnNodeHandle::make_zero();
      }
    }
  }
  if ( inode1 == NULL ) {
    if ( (fcode & 1U) == 1U ) {
      if ( (fcode & 2U) == 2U ) {
	// 11 なので定数1
	return BdnNodeHandle::make_one();
      }
      else {
	// 10 なので ~(inode0)
	return BdnNodeHandle(inode0, true);
      }
    }
    else {
      if ( (fcode & 2U) == 2U ) {
	// 01 なので inode0
	return BdnNodeHandle(inode0, false);
      }
      else {
	// 00 なので定数0
	return BdnNodeHandle::make_zero();
      }
    }
  }
  if ( inode0 == inode1 ) {
    if ( (fcode & 1U) == 1U ) {
      if ( (fcode & 8U) == 8U ) {
	return BdnNodeHandle::make_one();
      }
      else {
	return BdnNodeHandle(inode0, true);
      }
    }
    else {
      if ( (fcode & 8U) == 8U ) {
	return BdnNodeHandle(inode0, false);
      }
      else {
	return BdnNodeHandle::make_zero();
      }
    }
  }

  // 出力極性の正規化
  bool inv = (fcode & 1U) == 1U;
  if ( inv ) {
    fcode ^= 0xf;
  }

  // ファンインの順番の正規化
  if ( inode0->id() > inode1->id() ) {
    BdnNode* tmp = inode0;
    inode0 = inode1;
    inode1 = tmp;
    if ( fcode & 2U ) {
      if ( (fcode & 4U) == 0U ) {
	fcode &= 8U;
	fcode |= 4U;
      }
    }
    else if ( fcode & 4U ) {
      fcode &= 8U;
      fcode |= 2U;
    }
  }

  // 同じ構造を持つノードが既にないか調べる．
  ymuint pos = hash_func(fcode, inode0, inode1);
  ymuint idx = pos % mHashSize;
  for (BdnNode* node1 = mHashTable[idx]; node1; node1 = node1->mLink) {
    if ( node1->fcode() == fcode &&
	 node1->fanin0() == inode0 &&
	 node1->fanin1() == inode1 ) {
      // 同じノードがあった．
      return BdnNodeHandle(node1, inv);
    }
  }

  if ( node == NULL ) {
    // 新しいノードを作る．
    node = alloc_logicnode();

    reg_node(node);

    // 論理ノードリストに登録
    mLnodeList.push_back(node);

    if ( lnode_num() >= mNextLimit ) {
      alloc_table(mHashSize * 2);
      // サイズが変わったのでインデックスを再計算する．
      idx = pos % mHashSize;
    }
  }
  else {
    // ハッシュ表から取り除く
    ymuint pos0 = hash_func(node->fcode(), node->fanin0(), node->fanin1());
    ymuint idx0 = pos0 % mHashSize;
    BdnNode* prev = mHashTable[idx0];
    if ( prev == node ) {
      mHashTable[idx0] = node->mLink;
    }
    for (BdnNode* node0 = 0; (node0 = prev->mLink); prev = node0) {
      if ( node0 == node ) {
	prev->mLink = node->mLink;
	break;
      }
    }
    // エラーチェック(node0 == NULL) はしていない．
  }

  node->set_type(BdnNode::kLOGIC);
  node->set_fcode(fcode);
  connect(inode0, node, 0);
  connect(inode1, node, 1);

  // ハッシュ表に登録する．
  node->mLink = mHashTable[idx];
  mHashTable[idx] = node;

  return BdnNodeHandle(node, inv);
}

// from を to の pos 番目のファンインとする．
// to の pos 番目にすでに接続があった場合には自動的に削除される．
void
BdNetwork::connect(BdnNode* from,
		   BdnNode* to,
		   ymuint pos)
{
  // BdnNode::mFaoutList を変更するのはここだけ

  BdnEdge* edge = &(to->mFanins[pos]);
  BdnNode* old_from = edge->from();
  if ( old_from ) {
    old_from->mFanoutList.erase(edge);
    old_from->scan_po();
  }
  edge->set_from(from);
  if ( from ) {
    from->mFanoutList.push_back(edge);
    from->scan_po();
  }

  // 構造が変わったのでレベルの情報は無効化される．
  mLevel = 0U;
}

// @brief D-FF の領域を確保する．
BdnDff*
BdNetwork::alloc_dff()
{
  void* p = mAlloc.get_memory(sizeof(BdnDff));
  BdnDff* dff = new (p) BdnDff;

  BdnNode* output = alloc_dffnode(dff);
  output->set_type(BdnNode::kDFF_OUTPUT);

  BdnNode* input = alloc_dffnode(dff);
  input->set_type(BdnNode::kDFF_INPUT);

  BdnNode* clock = alloc_dffnode(dff);
  clock->set_type(BdnNode::kDFF_CLOCK);

  BdnNode* set = alloc_dffnode(dff);
  set->set_type(BdnNode::kDFF_SET);

  BdnNode* reset = alloc_dffnode(dff);
  reset->set_type(BdnNode::kDFF_RESET);

  dff->set(output, input, clock, set, reset);

  return dff;
}

// @brief ラッチの領域を確保する．
BdnLatch*
BdNetwork::alloc_latch()
{
  void* p = mAlloc.get_memory(sizeof(BdnLatch));
  BdnLatch* latch = new (p) BdnLatch;

  BdnNode* output = alloc_latchnode(latch);
  output->set_type(BdnNode::kLATCH_OUTPUT);

  BdnNode* input = alloc_latchnode(latch);
  input->set_type(BdnNode::kLATCH_INPUT);

  BdnNode* enable = alloc_latchnode(latch);
  enable->set_type(BdnNode::kLATCH_ENABLE);

  latch->set(output, input, enable);

  return latch;
}

// @brief ポートに関連するノードを作成する．
// @param[in] port 関連するポート
// @param[in] bitpos ビット位置
// @return 作成されたノードを返す．
BdnNode*
BdNetwork::alloc_portnode(BdnPort* port,
			  ymuint bitpos)
{
  void* q = mAlloc.get_memory(sizeof(BdnPortData));
  BdnPortData* data = new (q) BdnPortData(port, bitpos);
  assert_cond( data->port() == port, __FILE__, __LINE__);
  void* p = mAlloc.get_memory(sizeof(BdnNode));
  BdnNode* node = new (p) BdnNode(data);
  assert_cond( node->port() == port, __FILE__, __LINE__);
  return node;
}

// @brief D-FF に関連するノードを作成する．
// @param[in] dff 関連する D-FF
// @return 作成されたノードを返す．
BdnNode*
BdNetwork::alloc_dffnode(BdnDff* dff)
{
  void* q = mAlloc.get_memory(sizeof(BdnDffData));
  BdnDffData* data = new (q) BdnDffData(dff);
  void* p = mAlloc.get_memory(sizeof(BdnNode));
  BdnNode* node = new (p) BdnNode(data);

  return node;
}

// @brief ラッチに関連するノードを作成する．
// @param[in] latch 関連するラッチ
// @return 作成されたノードを返す．
BdnNode*
BdNetwork::alloc_latchnode(BdnLatch* latch)
{
  void* q = mAlloc.get_memory(sizeof(BdnLatchData));
  BdnLatchData* data = new (q) BdnLatchData(latch);
  void* p = mAlloc.get_memory(sizeof(BdnNode));
  BdnNode* node = new (p) BdnNode(data);

  return node;
}

// 新しいノードを作成する．
// 作成されたノードを返す．
BdnNode*
BdNetwork::alloc_logicnode()
{
  BdnNode* node = mAvailLogic;
  if ( node != NULL ) {
    mAvailLogic = node->mLink;
  }
  else {
    void* p = mAlloc.get_memory(sizeof(BdnNode));
    node = new (p) BdnNode(NULL);
  }

  node->mFlags = 0U;

  return node;
}

// @brief ノードに ID を割り当てる．
void
BdNetwork::reg_node(BdnNode* node)
{
  // 空いているIDを探してノード配列へ登録
  int id = mNodeItvlMgr.avail_num();
  assert_cond( id >= 0, __FILE__, __LINE__);
  mNodeItvlMgr.erase(id);

  ymuint uid = static_cast<ymuint>(id);
  node->mId = uid;

  if ( mNodeArray.size() == uid ) {
    mNodeArray.push_back(node);
  }
  else {
    assert_cond( mNodeArray.size() > uid, __FILE__, __LINE__);
    mNodeArray[uid] = node;
  }
}

// node を削除する．
void
BdNetwork::delete_node(BdnNode* node)
{
  // new_node の逆の処理を行なう．
  mNodeItvlMgr.add(static_cast<int>(node->mId));

  if ( node->is_logic() ) {
    mLnodeList.erase(node);
    node->mLink = mAvailLogic;
    mAvailLogic = node;
  }
  else if ( node->is_input() ) {
    mInputList.erase(node);
  }
  else if ( node->is_output() ) {
    mOutputList.erase(node);
  }

  // mNodeArray 内のエントリはクリアしない．
  // id の再利用と同様に BdnNode も再利用する．
}

// @brief 段数を求める．
ymuint
BdNetwork::level() const
{
  if ( (mLevel & 1U) == 0U ) {
    for (BdnNodeList::const_iterator p = mInputList.begin();
	 p != mInputList.end(); ++ p) {
      BdnNode* node = *p;
      node->mLevel = 0U;
    }

    vector<BdnNode*> node_list;
    sort(node_list);
    for (vector<BdnNode*>::const_iterator p = node_list.begin();
	 p != node_list.end(); ++ p) {
      BdnNode* node = *p;
      BdnNode* inode0 = node->fanin0();
      ymuint l = inode0->mLevel;
      BdnNode* inode1 = node->fanin1();
      ymuint l1 = inode1->mLevel;
      if ( l < l1 ) {
	l = l1;
      }
      node->mLevel = l + 1;
    }

    ymuint max_l = 0;
    for (BdnNodeList::const_iterator p = mOutputList.begin();
	 p != mOutputList.end(); ++ p) {
      BdnNode* node = *p;
      BdnNode* inode = node->fanin0();
      if ( inode ) {
	ymuint l1 = inode->mLevel;
	if ( max_l < l1 ) {
	  max_l = l1;
	}
      }
    }
    mLevel = (max_l << 1) | 1U;
  }
  return (mLevel >> 1);
}

// @brief ハッシュ表を確保する．
void
BdNetwork::alloc_table(ymuint req_size)
{
  BdnNode** old_table = mHashTable;
  ymuint old_size = mHashSize;

  if ( mHashSize == 0 ) {
    mHashSize = 1024;
  }
  while ( mHashSize < req_size ) {
    mHashSize <<= 1;
  }
  mHashTable = new BdnNode*[mHashSize];
  for (ymuint i = 0; i < mHashSize; ++ i) {
    mHashTable[i] = NULL;
  }
  if ( old_size > 0 ) {
    for (ymuint i = 0; i < old_size; ++ i) {
      BdnNode* next = NULL;
      for (BdnNode* node = old_table[i]; node; node = next) {
	next = node->mLink;
	ymuint pos = hash_func(node->fcode(), node->fanin0(), node->fanin1());
	ymuint idx = pos % mHashSize;
	node->mLink = mHashTable[idx];
	mHashTable[idx] = node;
      }
    }
    delete [] old_table;
  }
  mNextLimit = static_cast<ymuint32>(mHashSize * 1.8);
}

END_NAMESPACE_YM_BDN
