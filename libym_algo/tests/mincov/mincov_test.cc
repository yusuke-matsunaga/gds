﻿
/// @file mincov_test.cc
/// @brief mincov_test の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2014 Yusuke Matsunaga
/// All rights reserved.


#include "utils/MinCov.h"
#include "espresso/sparse.h"
#include "espresso/mincov.h"

BEGIN_NAMESPACE_YM

// c が 0 から 9 までの数字なら true を返す．
bool
is_num(char c)
{
  return c >= '0' && c <= '9';
}

// c が空白なら true を返す．
bool
is_spc(char c)
{
  return c == ' ' || c == '\t';
}

// 数字を読み込む．
bool
parse_num(char* buff,
	  ymuint& rpos,
	  int& num)
{
  ymuint pos0 = rpos;

  // 最初だけ '-' がありうる
  if ( buff[rpos] == '-' ) {
    ++ rpos;
  }

  ymuint pos1 = rpos;
  for ( ; is_num(buff[rpos]); ++ rpos) {
    ;
  }
  if ( rpos == pos1 ) {
    return false;
  }

  char old_char = buff[rpos];
  buff[rpos] = '\0';
  num = atoi(&buff[pos0]);
  buff[rpos] = old_char;
  return true;
}

// 1行をパーズする．
// 形式は <num> <sp> <num> (<sp> <num>)
// <num> ::= '-'?[1-9][0-9]*
// <sp>  ::= ' '|'\t'
// ただし，'#' で始まる行はコメント行とみなす．
// 読み込んだ数字の数を返す．
// エラーの場合には -1 を返す．
// コメント行の場合には 0 を返す．
int
parse_line(char* buff,
	   int& num1,
	   int& num2)
{
  if ( buff[0] == '#' ) {
    return 0;
  }

  ymuint count = 0;
  ymuint pos = 0;

  if ( !parse_num(buff, pos, num1) ) {
    return -1;
  }

  while ( is_spc(buff[pos]) ) {
    ++ pos;
  }

  if ( !parse_num(buff, pos, num2) ) {
    return -1;
  }

  return 1;
}

bool
parse_data(istream& s,
	   vector<pair<int, int> >& pair_list)
{
  char buff[1024];
  bool go_on = true;
  while ( go_on && s.getline(buff, 1024, '\n') ) {
    int num1;
    int num2;
    int stat = parse_line(buff, num1, num2);
    if ( stat == -1 ) {
      cerr << "Error: " << buff << endl;
      return false;
    }
    else if ( stat == 0 ) {
      // コメント行
    }
    else if ( stat == 1 ) {
      if ( num1 == -1 && num2 == -1 ) {
	go_on = false;
      }
      else {
	pair_list.push_back(make_pair(num1, num2));
      }
    }
  }
  return true;
}

int
mincov_test(int argc,
	    char** argv)
{
  bool heuristic = false;
  bool block_partition = true;
  bool debug = false;
  bool lp_solve = false;
  bool ilp_solve = false;
  bool espresso = false;
  bool cs_lp = false;
  ymuint depth = 0;
  ymuint base = 1;
  while ( argc > base && argv[base][0] == '-' ) {
    if ( strcmp(argv[1], "-h") == 0 ) {
      heuristic = true;
      ++ base;
    }
    else if ( strcmp(argv[base], "-b") == 0 ) {
      block_partition = true;
      ++ base;
    }
    else if ( strcmp(argv[base], "-nb") == 0 ) {
      block_partition = false;
      ++ base;
    }
    else if ( strcmp(argv[base], "-d") == 0 ) {
      debug = true;
      ++ base;
      depth = atoi(argv[base]);
      ++ base;
    }
    else if ( strcmp(argv[base], "-lp") == 0 ) {
      lp_solve = true;
      ++ base;
    }
    else if ( strcmp(argv[base], "-ilp") == 0 ) {
      ilp_solve = true;
      ++ base;
    }
    else if ( strcmp(argv[base], "-espresso") == 0 ) {
      espresso = true;
      ++ base;
    }
    else if ( strcmp(argv[base], "-cs_lp") == 0 ) {
      cs_lp = true;
      ++ base;
    }
    else {
      cerr << argv[1] << " : invalid option" << endl;
      return 1;
    }
  }

  vector<pair<int, int> > pair_list;

  if ( argc > base ) {
    ifstream s(argv[base]);
    if ( !s ) {
      cerr << argv[base] << " : no such file" << endl;
      return 1;
    }
    parse_data(s, pair_list);
  }
  else {
    parse_data(cin, pair_list);
  }

  ymuint max_r = 0;
  ymuint max_c = 0;
  for (vector<pair<int, int> >::iterator p = pair_list.begin();
       p != pair_list.end(); ++ p) {
    ymuint r = p->second;
    ymuint c = p->first;
    if ( max_r < r ) {
      max_r = r;
    }
    if ( max_c < c ) {
      max_c = c;
    }
  }

#if 0
  try {
#endif
    if ( lp_solve || ilp_solve ) {
      cout << "min: ";
      const char* plus = "";
      for (ymuint i = 0; i <= max_c; ++ i) {
	cout << plus << "1 x" << i;
	plus = " + ";
      }
      cout << ";" << endl;
      vector<vector<ymuint> > row_list(max_r + 1);
      for (vector<pair<int, int> >::iterator p = pair_list.begin();
	   p != pair_list.end(); ++ p) {
	ymuint r = p->second;
	ymuint c = p->first;
	row_list[r].push_back(c);
      }
      ymuint cnum = 1;
      for (ymuint i = 0; i <= max_r; ++ i) {
	const vector<ymuint>& row = row_list[i];
	ymuint n = row.size();
	if ( n == 0 ) {
	  continue;
	}
	cout << "C" << cnum << ": ";
	++ cnum;
	const char* plus = "";
	for (ymuint j = 0; j < n; ++ j) {
	  cout << plus << "1 x" << row[j];
	  plus = " + ";
	}
	cout << " >= 1;" << endl;
      }
      for (ymuint i = 0; i <= max_c; ++ i) {
	cout << "C" << cnum << ": x" << i << " >= 0;" << endl;
	++ cnum;
	cout << "C" << cnum << ": x" << i << " <= 1;" << endl;
	++ cnum;
      }
      if ( ilp_solve ) {
	for (ymuint i = 0; i <= max_c; ++ i) {
	  cout << "int x" << i << ";" << endl;
	}
      }
      return 0;
    }
    if ( espresso ) {
      vector<vector<ymuint> > row_list(max_r + 1);
      for (vector<pair<int, int> >::iterator p = pair_list.begin();
	   p != pair_list.end(); ++ p) {
	ymuint r = p->second;
	ymuint c = p->first;
	row_list[r].push_back(c);
      }
      sm_matrix* A = sm_alloc();
      ymuint rownum = 0;
      for (ymuint i = 0; i <= max_r; ++ i) {
	const vector<ymuint>& rows = row_list[i];
	if ( rows.empty() ) {
	  continue;
	}
	for (ymuint j = 0; j < rows.size(); ++ j) {
	  sm_insert(A, rownum, rows[j]);
	}
	++ rownum;
      }
      sm_row* solution = sm_minimum_cover(A, NULL, 0, depth);
      sm_element* pe;
      sm_foreach_row_element(solution, pe) {
	cout << " " << pe->col_num;
      }
      return 0;
    }
    if ( cs_lp ) {
      vector<vector<ymuint> > row_list(max_r + 1);
      vector<vector<ymuint> > col_list(max_c + 1);
      for (vector<pair<int, int> >::iterator p = pair_list.begin();
	   p != pair_list.end(); ++ p) {
	ymuint r = p->second;
	ymuint c = p->first;
	row_list[r].push_back(c);
	col_list[c].push_back(r);
      }

      // 各行の最小重みを表す変数
      vector<ymuint> row_weights(max_r + 1);

      // 分割された列の重みを表す変数
      vector<ymuint> subcol_weights_top(max_c + 1);
      vector<ymuint> subcol_num(max_c + 1);

      ymuint vid = 0;
      for (ymuint i = 0; i <= max_r; ++ i) {
	const vector<ymuint>& rows = row_list[i];
	ymuint n = rows.size();
	if ( n > 0 ) {
	  row_weights[i] = vid;
	  ++ vid;
	}
	else {
	  row_weights[i] = -1;
	}
      }
      for (ymuint i = 0; i <= max_c; ++ i) {
	const vector<ymuint>& cols = col_list[i];
	ymuint n = cols.size();
	subcol_weights_top[i] = vid;
	subcol_num[i] = n;
	vid += n;
      }

      cout << "max: ";
      const char* plus = "";
      for (ymuint i = 0; i <= max_r; ++ i) {
	if ( row_weights[i] == -1 ) {
	  continue;
	}
	cout << plus << "x" << row_weights[i];
	plus = " + ";
      }
      cout << ";" << endl;
      vector<ymuint> subcol_idx(max_c + 1);
      for (ymuint i = 0; i <= max_c; ++ i) {
	subcol_idx[i] = 0;
      }
      ymuint cnum = 1;
      for (ymuint i = 0; i <= max_r; ++ i) {
	const vector<ymuint>& rows = row_list[i];
	if ( rows.empty() ) {
	  continue;
	}
	for (ymuint j = 0; j < rows.size(); ++ j) {
	  ymuint col = rows[j];
	  ymuint idx = subcol_idx[col];
	  ++ subcol_idx[col];
	  cout << "C" << cnum << ": ";
	  ++ cnum;
	  cout << "x" << subcol_weights_top[col] + idx;
	  cout << " >= x" << row_weights[i] << ";" << endl;
	}
	cout << "C" << cnum << ": ";
	++ cnum;
	cout << "x" << row_weights[i] << " >= 0;" << endl;
      }
      for (ymuint i = 0; i <= max_c; ++ i) {
	ymuint n = subcol_num[i];
	if ( n == 0 ) {
	  continue;
	}
	cout << "C" << cnum << ": ";
	++ cnum;
	const char* plus = "";
	ymuint base = subcol_weights_top[i];
	for (ymuint j = 0; j < n; ++ j) {
	  cout << plus << "x" << base + j;
	  plus = " + ";
	}
	cout << " = 1;" << endl;
	for (ymuint j = 0; j < n; ++ j) {
	  cout << "C" << cnum << ": ";
	  ++ cnum;
	  cout << "x" << base + j << " >= 0;" << endl;
	}
      }

      return 0;
    }

    MinCov mincov;

    mincov.set_partition(block_partition);

    if ( debug ) {
      mincov.set_debug(true);
      mincov.set_max_depth(depth);
    }

    mincov.set_size(max_r + 1, max_c + 1);
    for (vector<pair<int, int> >::iterator p = pair_list.begin();
	 p != pair_list.end(); ++ p) {
      mincov.insert_elem(p->second, p->first);
    }

    vector<ymuint32> solution;
    ymuint cost = 0;
    if ( heuristic ) {
      cost = mincov.heuristic(solution);
    }
    else {
      cost = mincov.exact(solution);
    }

    cout << "Cost = " << cost << endl;
    for (vector<ymuint32>::iterator p = solution.begin();
	 p != solution.end(); ++ p) {
      cout << " " << *p;
    }
    cout << endl;
#if 0
  }
  catch ( AssertError& x ) {
    cerr << x;
  }
#endif
}

END_NAMESPACE_YM


int
main(int argc,
     char** argv)
{
  return nsYm::mincov_test(argc, argv);
}
