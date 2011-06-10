
/// @file libym_networks/BdnDumper.cc
/// @brief BdnDumper の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2005-2011 Yusuke Matsunaga
/// All rights reserved.


#include "ym_networks/BdnDumper.h"

#include "ym_networks/BdnMgr.h"
#include "ym_networks/BdnPort.h"
#include "ym_networks/BdnDff.h"
#include "ym_networks/BdnLatch.h"
#include "ym_networks/BdnNode.h"
#include "ym_networks/BdnNodeHandle.h"


BEGIN_NAMESPACE_YM_BDN

BEGIN_NONAMESPACE

// 出力ノードの内容を出力する．
void
dump_output(ostream& s,
	    const BdnNode* node)
{
  const BdnNode* inode = node->output_fanin();
  bool oinv = node->output_fanin_inv();
  if ( inode ) {
    // 普通のノードの場合
    if ( oinv ) {
      s << "~";
    }
    s << inode->id_str();
  }
  else {
    // 定数ノードの場合
    if ( oinv ) {
      s << "1";
    }
    else {
      s << "0";
    }
  }
}

END_NONAMESPACE

// @brief コンストラクタ
BdnDumper::BdnDumper()
{
}

// @brief デストラクタ
BdnDumper::~BdnDumper()
{
}

// @brief BdnMgr の内容をダンプする関数
// @param[in] s 出力先のストリーム
// @param[in] network 対象のネットワーク
void
BdnDumper::operator()(ostream& s,
		      const BdnMgr& network)
{
  ymuint np = network.port_num();
  for (ymuint i = 0; i < np; ++ i) {
    const BdnPort* port = network.port(i);
    s << "Port#" << port->id() << ": " << port->name() << endl;
  }

  const BdnDffList& dff_list = network.dff_list();
  for (BdnDffList::const_iterator p = dff_list.begin();
       p != dff_list.end(); ++ p) {
    const BdnDff* dff = *p;
    const BdnNode* output = dff->output();
    const BdnNode* input = dff->input();
    const BdnNode* clock = dff->clock();
    const BdnNode* clear = dff->clear();
    const BdnNode* preset = dff->preset();
    s << "DFF#" << dff->id() << ": " << dff->name() << " ("
      << "OUTPUT=" << output->id_str()
      << ", INPUT=";
    dump_output(s, input);
    s << ", CLOCK=";
    dump_output(s, clock);
    if ( clear->output_fanin() ) {
      s << ", CLEAR=";
      dump_output(s, clear);
    }
    if ( preset->output_fanin() ) {
      s << ", PRESET=";
      dump_output(s, preset);
    }
    s << ")" << endl;
  }

  const BdnLatchList& latch_list = network.latch_list();
  for (BdnLatchList::const_iterator p = latch_list.begin();
       p != latch_list.end(); ++ p) {
    const BdnLatch* latch = *p;
    const BdnNode* output = latch->output();
    const BdnNode* input = latch->input();
    const BdnNode* enable = latch->enable();
    const BdnNode* clear = latch->clear();
    const BdnNode* preset = latch->preset();
    s << "LATCH#" << latch->id() << ": " << latch->name() << " ("
      << "OUTPUT = " << output->id_str()
      << ", INPUT = ";
    dump_output(s, input);
    if ( enable->output_fanin() ) {
      s << ", ENABLE = ";
      dump_output(s, enable);
    }
    if ( clear->output_fanin() ) {
      s << ", CLEAR = ";
      dump_output(s, clear);
    }
    if ( preset->output_fanin() ) {
      s << ", PRESET = ";
      dump_output(s, preset);
    }
    s << ")" << endl;
  }

  const BdnNodeList& input_list = network.input_list();
  for (BdnNodeList::const_iterator p = input_list.begin();
       p != input_list.end(); ++ p) {
    const BdnNode* node = *p;
    assert_cond( node->is_input(), __FILE__, __LINE__);
    s << node->id_str() << ": ";
    switch ( node->input_type() ) {
    case BdnNode::kPRIMARY_INPUT:
      s << " PORT#" << node->port()->id();
      if ( node->port()->bit_width() > 1 ) {
	s << "[" << node->port_bitpos() << "]";
      }
      break;

    case BdnNode::kDFF_OUTPUT:
      s << " OUTPUT@DFF#" << node->dff()->id();
      break;

    case BdnNode::kLATCH_OUTPUT:
      s << " OUTPUT@LATCH#" << node->latch()->id();
      break;

    default:
      assert_not_reached(__FILE__, __LINE__);
      break;
    }
    s << endl;
  }

  const BdnNodeList& lnode_list = network.lnode_list();
  for (BdnNodeList::const_iterator p = lnode_list.begin();
       p != lnode_list.end(); ++ p) {
    const BdnNode* node = *p;
    assert_cond( node->is_logic(), __FILE__, __LINE__);
    s << node->id_str();
    s << " :  = LOGIC[";
    const char* f0 = node->fanin0_inv() ? "~" : "";
    const char* f1 = node->fanin1_inv() ? "~" : "";
    const char* op = node->is_xor() ? "^" : "&";
    s << f0 << node->fanin0()->id_str()
      << " " << op << " "
      << f1 << node->fanin1()->id_str()
      << "]" << endl;
  }

  const BdnNodeList& output_list = network.output_list();
  for (BdnNodeList::const_iterator p = output_list.begin();
       p != output_list.end(); ++ p) {
    const BdnNode* node = *p;
    assert_cond( node->is_output(), __FILE__, __LINE__);
    s << node->id_str() << ": ";
    switch ( node->output_type() ) {
    case BdnNode::kPRIMARY_OUTPUT:
      s << "PORT#" << node->port()->id();
      if ( node->port()->bit_width() > 1 ) {
	s << "[" << node->port_bitpos() << "]";
      }
      break;

    case BdnNode::kDFF_DATA:
      s << "DATA@DFF#" << node->dff()->id();
      break;

    case BdnNode::kDFF_CLOCK:
      s << "CLOCK@DFF#" << node->dff()->id();
      break;

    case BdnNode::kDFF_CLEAR:
      s << "CLEAR@DFF#" << node->dff()->id();
      break;

    case BdnNode::kDFF_PRESET:
      s << "PRESET@DFF#" << node->dff()->id();
      break;

    case BdnNode::kLATCH_DATA:
      s << "DATA@LATCH#" << node->latch()->id();
      break;

    case BdnNode::kLATCH_ENABLE:
      s << "ENABLE@LATCH#" << node->latch()->id();
      break;

    case BdnNode::kLATCH_CLEAR:
      s << "CLEAR@LATCH#" << node->latch()->id();
      break;

    case BdnNode::kLATCH_PRESET:
      s << "PRESET@LATCH#" << node->latch()->id();
      break;

    default:
      assert_not_reached(__FILE__, __LINE__);
      break;
    }
    s << " = ";
    dump_output(s, node);
    s << endl;
  }
}

END_NAMESPACE_YM_BDN