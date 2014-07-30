#ifndef YMYMLOGIC_SMTSOLVER_H
#define YMYMLOGIC_SMTSOLVER_H

/// @file YmLogic/SmtSolver.h
/// @brief SmtSolver のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2013 Yusuke Matsunaga
/// All rights reserved.


#include "YmLogic/smt_nsdef.h"
#include "YmUtils/ShString.h"


BEGIN_NAMESPACE_YM_SMT

//////////////////////////////////////////////////////////////////////
/// @class SmtSolver SmtSolver.h "YmLogic/SmtSolver.h"
/// @brief SMT ソルバの雛形基底クラス
///
/// 実際にはここで定義されている仮想関数を実装したクラスを作る必要がある．
//////////////////////////////////////////////////////////////////////
class SmtSolver
{
public:

  /// @brief インスタンスを生成する静的関数
  static
  SmtSolver*
  new_solver();

  /// @brief デストラクタ
  virtual
  ~SmtSolver() { }


public:
  //////////////////////////////////////////////////////////////////////
  // 継承クラスが実装する仮想関数
  //////////////////////////////////////////////////////////////////////

  /// @brief 使用するロジックを設定する
  /// @param[in] logic 設定するロジック
  /// @retval true 処理が成功した．
  /// @retval false 処理が失敗した．
  ///
  /// エラーとなる原因は以下のとおり
  ///  - 既に set_logic() が呼ばれていた．
  ///  - (unspported) サポートしていないロジックだった．
  virtual
  bool
  set_logic(tSmtLogic logic) = 0;

  /// @brief 型を作る．(単純型)
  /// @return 作成した型番号を返す．
  ///
  /// エラーが起きた場合には kSmtSort_None を返す．
  /// エラーとなる原因は以下のとおり
  ///  - set_logic() が呼ばれていない．
  virtual
  tSmtSortId
  make_sort() = 0;

  /// @brief 型を作る．(複合型)
  /// @param[in] elem_list 要素の型のリスト
  /// @return 作成した型を返す．
  ///
  /// エラーが起きた場合には kSmtSort_None を返す．
  /// エラーとなる原因は以下のとおり
  ///  - set_logic() が呼ばれていない．
  virtual
  tSmtSortId
  make_sort(const vector<tSmtSortId>& elem_list) = 0;

  /// @brief 型番号から SmtSort を得る．
  /// @param[in] id 型番号
  /// @return SmtSort を返す．
  ///
  /// エラーが起きた場合には NULL を返す．
  /// エラーとなる原因は以下のとおり
  ///  - set_logic() が呼ばれていない．
  ///  - set_logic() で使える組み込み型ではない．
  ///  - 存在しない型番号だった．
  virtual
  const SmtSortInfo*
  get_sort(tSmtSortId id) = 0;

  /// @brief 変数を作る．
  /// @param[in] sort 変数の型
  /// @param[in] type 変数の種類
  /// @return 作成した変数を返す．
  ///
  /// エラーが起きた場合には NULL を返す．
  /// エラーとなる原因は以下のとおり
  ///  - set_logic() が呼ばれていない．
  virtual
  const SmtVar*
  make_var(tSmtSortId sort,
	   tSmtVarType type = kSmtVar_Global) = 0;

  /// @brief 関数を作る．(引数なし)
  /// @param[in] output_sort 出力の型
  /// @return 作成した関数を返す．
  ///
  /// エラーが起きた場合には NULL を返す．
  /// エラーとなる原因は以下のとおり
  ///  - set_logic() が呼ばれていない．
  virtual
  const SmtFun*
  make_fun(tSmtSortId output_sort) = 0;

  /// @brief 関数を作る．
  /// @param[in] input_sort_list 入力の型のリスト
  /// @param[in] output_sort 出力の型
  /// @return 作成した関数を返す．
  ///
  /// エラーが起きた場合には NULL を返す．
  /// エラーとなる原因は以下のとおり
  ///  - set_logic() が呼ばれていない．
  virtual
  const SmtFun*
  make_fun(const vector<tSmtSortId>& input_sort_list,
	   tSmtSortId output_sort) = 0;

  /// @brief 内容を持った関数を作る．(引数なし)
  /// @param[in] body 本体を式
  /// @return 作成した関数を返す．
  ///
  /// エラーが起きた場合には NULL を返す．
  /// エラーとなる原因は以下のとおり
  ///  - set_logic() が呼ばれていない．
  virtual
  const SmtFun*
  make_fun(const SmtTerm* body) = 0;

  /// @brief 内容を持った関数を作る．
  /// @param[in] input_var_list 入力の変数のリスト
  /// @param[in] body 本体を式
  /// @return 作成した関数を返す．
  ///
  /// エラーが起きた場合には NULL を返す．
  /// エラーとなる原因は以下のとおり
  ///  - set_logic() が呼ばれていない．
  virtual
  const SmtFun*
  make_fun(const vector<const SmtVar*>& input_var_list,
	   const SmtTerm* body) = 0;

  /// @brief 組み込み関数用のチェッカーを得る．
  /// @param[in] fun_type 関数の型
  virtual
  const SmtFunValidator*
  get_fun_validator(tSmtFunType fun_type) = 0;

  /// @brief \<numeral\> 型の term を作る．
  /// @param[in] val 値
  /// @return 作成した式を返す．
  virtual
  const SmtTerm*
  make_numeral_term(ymuint32 val) = 0;

  /// @brief \<decimal\> 型の term を作る．
  /// @param[in] val 値
  /// @return 作成した式を返す．
  virtual
  const SmtTerm*
  make_decimal_term(const ShString& val) = 0;

  /// @brief \<hexadecimal\> 型の term を作る．
  /// @param[in] val 値
  /// @return 作成した式を返す．
  virtual
  const SmtTerm*
  make_hexadecimal_term(const ShString& val) = 0;

  /// @brief \<binary\> 型の term を作る．
  /// @param[in] val 値
  /// @return 作成した式を返す．
  virtual
  const SmtTerm*
  make_binary_term(const ShString& val) = 0;

  /// @brief \<string\> 型の term を作る．
  /// @param[in] val 値
  /// @return 作成した式を返す．
  virtual
  const SmtTerm*
  make_string_term(const ShString& val) = 0;

  /// @brief 変数型の term を作る．
  /// @param[in] var 変数
  /// @return 作成した式を返す．
  virtual
  const SmtTerm*
  make_var_term(const SmtVar* var) = 0;

  /// @brief 関数呼び出しの term を作る．(引数なし)
  /// @param[in] fun 関数
  /// @return 作成した式を返す．
  ///
  /// エラーが起きたら NULL を返す．
  /// エラーとなる原因は以下のとおり
  ///  - fun の引数の数か型が arg_list と合わない．
  virtual
  const SmtTerm*
  make_fun_term(const SmtFun* fun) = 0;

  /// @brief 関数呼び出しの term を作る．
  /// @param[in] fun 関数
  /// @param[in] arg_list 引数のリスト
  /// @return 作成した式を返す．
  ///
  /// エラーが起きたら NULL を返す．
  /// エラーとなる原因は以下のとおり
  ///  - fun の引数の数か型が arg_list と合わない．
  virtual
  const SmtTerm*
  make_fun_term(const SmtFun* fun,
		const vector<const SmtTerm*>& arg_list) = 0;

  /// @brief 関数呼び出しの term を作る．(組み込み関数，引数なし)
  /// @param[in] fun_type 関数の型
  /// @return 作成した式を返す．
  ///
  /// エラーが起きたら NULL を返す．
  /// エラーとなる原因は以下のとおり
  ///  - fun の引数の数か型が arg_list と合わない．
  virtual
  const SmtTerm*
  make_fun_term(tSmtFunType fun_type) = 0;

  /// @brief 関数呼び出しの term を作る．(組み込み関数)
  /// @param[in] fun_type 関数の型
  /// @param[in] arg_list 引数のリスト
  /// @return 作成した式を返す．
  ///
  /// エラーが起きたら NULL を返す．
  /// エラーとなる原因は以下のとおり
  ///  - fun の引数の数か型が arg_list と合わない．
  virtual
  const SmtTerm*
  make_fun_term(tSmtFunType fun_type,
		const vector<const SmtTerm*>& arg_list) = 0;

  /// @brief forall の term を作る．
  /// @param[in] var_list 束縛変数のリスト
  /// @param[in] body 本体の式
  /// @return 作成した式を返す．
  ///
  /// エラーとなることはないはず．
  /// body 中に現れない変数が var_list に含まれていても問題ない．
  virtual
  const SmtTerm*
  make_forall_term(const vector<const SmtVar*>& var_list,
		   const SmtTerm* body) = 0;

  /// @brief exists の term を作る．
  /// @param[in] var_list 束縛変数のリスト
  /// @param[in] body 本体の式
  /// @return 作成した式を返す．
  ///
  /// エラーとなることはないはず．
  /// body 中に現れない変数が var_list に含まれていても問題ない．
  virtual
  const SmtTerm*
  make_exists_term(const vector<const SmtVar*>& var_list,
		   const SmtTerm* body) = 0;

  /// @brief 充足可能性を調べる．
  /// @param[in] assert_list 仮定する式のリスト
  virtual
  tSmtLibResponse
  check_sat(const vector<const SmtTerm*>& assert_list) = 0;

  /// @brief 直前の check_sat() の証明を得る．
  /// @note check_sat() の結果が UNSAT の時のみ意味を持つ．
  virtual
  tSmtLibResponse
  get_proof() = 0;

  /// @brief 直前の check_sat() の unsat-core を得る．
  /// @param[out] term_list unsat-core の式を格納するリスト
  /// @note check_sat() の結果が UNSAT の時のみ意味を持つ．
  virtual
  tSmtLibResponse
  get_unsat_core(vector<const SmtTerm*>& term_list) = 0;

  /// @brief 直前の check_sat() の値割り当てを得る．
  /// @param[in] expr_list expr 式のリスト
  /// @todo 値をどうやって返す？
  virtual
  tSmtLibResponse
  get_value(const vector<const SmtTerm*>& expr_list) = 0;

  /// @brief 直前の check_sat() の値割り当てを得る．
  virtual
  tSmtLibResponse
  get_assignment() = 0;

};

END_NAMESPACE_YM_SMT

#endif // YMYMLOGIC_SMTSOLVER_H