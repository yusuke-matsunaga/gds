
/// @file libym_networks/tests/blif/reader_test.cc
/// @brief BlifNetworkReader のテスト
/// @author Yusuke Matsunaga (松永 裕介)
///
/// $Id: reader_test.cc 2507 2009-10-17 16:24:02Z matsunaga $
///
/// Copyright (C) 2005-2011 Yusuke Matsunaga
/// All rights reserved.


#include "BlifNetwork.h"
#include "BlifNetworkReader.h"
#include "ym_utils/MsgMgr.h"
#include "ym_utils/MsgHandler.h"


int
main(int argc,
     char** argv)
{
  using namespace std;
  using namespace nsYm;
  using namespace nsYm::nsNetworks::nsBlif;

  if ( argc != 2 ) {
    cerr << "USAGE : " << argv[0] << " blif-file" << endl;
    return 2;
  }
  string filename = argv[1];

  try {
    StreamMsgHandler* msg_handler = new StreamMsgHandler(&cerr);
    MsgMgr::reg_handler(msg_handler);
    BlifNetworkReader reader;

    BlifNetwork network;

    if ( !reader.read(filename, network) ) {
      cerr << "Error in reading " << filename << endl;
      return 4;
    }

    network.write_blif(cout);

  }
  catch ( const AssertError& x) {
    cout << x << endl;
  }

  return 0;
}