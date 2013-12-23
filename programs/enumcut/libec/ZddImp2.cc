
/// @file ZddImp2.cc
/// @brief ZddImp2 の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2005-2011 Yusuke Matsunaga
/// All rights reserved.


#include "ZddImp2.h"

#include "ym_networks/BdnMgr.h"
#include "ym_networks/BdnNode.h"


BEGIN_NAMESPACE_YM

// @brief コンストラクタ
// @param[in] mgr
ZddImp2::ZddImp2(ZddMgr& mgr) :
  mMgr(mgr)
{
}

// @brief デストラクタ
ZddImp2::~ZddImp2()
{
}

// @brief カット列挙を行う．
// @param[in] network 対象のネットワーク
// @param[in] limit カットサイズの制限
void
ZddImp2::operator()(BdnMgr& network,
		    ymuint limit)
{
  ymuint n = network.max_node_id();

  mCuts.clear();
  mCuts.resize(n);

  mpz_class nc_all = 0;

  const BdnNodeList& input_list = network.input_list();
  for (BdnNodeList::const_iterator p = input_list.begin();
       p != input_list.end(); ++ p) {
    const BdnNode* node = *p;

    Zdd cut = mMgr.make_base();
    cut.swap(VarId(node->id()));

    mCuts[node->id()] = cut;
    mpz_class nc = cut.count();
    nc_all += nc;
    cout << "Node#" << node->id() << endl;
    cut.print_set(cout);
    cout << endl;
  }

  vector<const BdnNode*> node_list;
  network.sort(node_list);
  for (vector<const BdnNode*>::iterator p = node_list.begin();
       p != node_list.end(); ++ p) {
    const BdnNode* node = *p;

    const BdnNode* node0 = node->fanin(0);
    const BdnNode* node1 = node->fanin(1);
    Zdd cut0 = mCuts[node0->id()];
    Zdd cut1 = mCuts[node1->id()];
    Zdd cut = mMgr.merge(cut0, cut1, limit);
    cut = mMgr.make_minimal(cut);
    Zdd cut2 = mMgr.make_base();
    cut2.swap(VarId(node->id()));
    cut |= cut2;
    mCuts[node->id()] = cut;
    mpz_class nc = cut.count();
    nc_all += nc;
    cout << "Node#" << node->id() << endl;
    //cut.print_set(cout);
    cout << nc << " cuts, "
	 << cut.node_count() << " nodes" << endl;
    cout << endl;
  }
  cout << "Total " << nc_all << " cuts" << endl;
}

END_NAMESPACE_YM
