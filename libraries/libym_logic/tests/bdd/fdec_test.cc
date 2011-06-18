
/// @file libym_logic/tests/bdd/fdec_test.cc
/// @brief 関数分解用のテスト
/// @author Yusuke Matsunaga (松永 裕介)
///
/// $Id: fdec_test.cc 700 2007-05-31 00:41:30Z matsunaga $
///
/// Copyright (C) 2005-2011 Yusuke Matsunaga
/// All rights reserved.


#include "ym_logic/Dg.h"

#include "bddtest.h"

using namespace std;
using namespace nsYm::nsBdd;

int
main(int argc,
     char** argv)
{
#if 0
  if ( argc != 2 ) {
    cout << "USAGE : " << argv[0] << " <expr>" << endl;
    return 2;
  }

  BddMgr& mgr = BddMgr::default_mgr();
  Bdd f = str2bdd(mgr, argv[1]);
  cout << "f:" << f.sop() << endl << endl;

  DgMgr dgmgr(mgr);
  Dg dg = dgmgr.decomp(f);

  dg.display(cout);
  cout << endl;

  PrimDecList dec_list;
  dg.enum_djdec(0, 4, dec_list);
  dump_primdec_list(dec_list, cout);
#endif

  return 0;
}
