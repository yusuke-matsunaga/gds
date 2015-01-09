#ifndef YMSLTYPE_H
#define YMSLTYPE_H

/// @file AstValueType.h
/// @brief AstValueType のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2014 Yusuke Matsunaga
/// All rights reserved.


#include "ymsl_int.h"
#include "YmUtils/HashFunc.h"
#include "YmUtils/ShString.h"


BEGIN_NAMESPACE_YM_YMSL

//////////////////////////////////////////////////////////////////////
/// @brief 型の種類
//////////////////////////////////////////////////////////////////////
enum TypeId {
  kVoidType,
  kBooleanType,
  kIntType,
  kFloatType,
  kStringType,
  kArrayType,
  kSetType,
  kMapType,
  kFuncType,
  kClassType,
  kEnumType,
  kUserDefType
};

/// @brief ValueType を出力する．
ostream&
operator<<(ostream& s,
	   TypeId vt);


//////////////////////////////////////////////////////////////////////
/// @class YmslType YmslType.h "YmslType.h"
/// @brief 値の型を表すクラス
//////////////////////////////////////////////////////////////////////
class YmslType
{
public:

  /// @brief コンストラクタ
  /// @param[in] id ID番号
  YmslType(ymuint id);

  /// @brief デストラクタ
  virtual
  ~YmslType();


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief 型を得る．
  virtual
  TypeId
  type() const = 0;

  /// @brief ID番号を得る
  ///
  /// おもにハッシュ関数用
  ymuint
  id() const;

  /// @brief 値を表す文字列を返す．
  ///
  /// class/enum/userdef のみ有効
  virtual
  string
  str() const;

  /// @brief 要素の型を得る．
  ///
  /// array/set/map のみ有効
  virtual
  const YmslType*
  elem_type() const;

  /// @brief キーの型を得る．
  ///
  /// map のみ有効
  virtual
  const YmslType*
  key_type() const;

  /// @brief 関数の出力の型を返す．
  ///
  /// function のみ有効
  virtual
  const YmslType*
  function_type() const;

  /// @brief 関数の入力数を返す．
  ///
  /// function のみ有効
  virtual
  ymuint
  function_input_num() const;

  /// @brief 関数の入力の型を返す．
  /// @param[in] pos 入力番号 ( 0 <= pos < function_input_num() )
  ///
  /// function のみ有効
  virtual
  const YmslType*
  function_input_type(ymuint pos) const;

  /// @brief フィールド(メンバ変数)の数を得る．
  ///
  /// class のみ有効
  virtual
  ymuint
  field_num() const;

  /// @brief フィールド(メンバ変数)の型を得る．
  /// @param[in] index インデックス ( 0 <= index < field_num() )
  ///
  /// class のみ有効
  virtual
  const YmslType*
  field_type(ymuint index) const;

  /// @brief フィールド(メンバ変数)の名前を得る．
  /// @param[in] index インデックス ( 0 <= index < field_num() )
  ///
  /// class のみ有効
  virtual
  ShString
  field_name(ymuint index) const;

  /// @brief フィールド(メンバ変数)のインデックスを得る．
  /// @param[in] name フィールド名
  ///
  /// class のみ有効
  /// 該当するフィールドがなければ -1 を返す．
  virtual
  int
  field_index(ShString name) const;

  /// @brief メソッド(メンバ関数)の数を得る．
  ///
  /// class のみ有効
  virtual
  ymuint
  method_num() const;

  /// @brief メソッド(メンバ関数)の型を得る．
  /// @param[in] index インデックス ( 0 <= index < method_num() )
  ///
  /// class のみ有効
  virtual
  const YmslType*
  method_type(ymuint index) const;

  /// @brief メソッド(メンバ関数)の名前を得る．
  /// @param[in] index インデックス ( 0 <= index < method_num() )
  ///
  /// class のみ有効
  virtual
  ShString
  method_name(ymuint index) const;

  /// @brief メソッド(メンバ関数)のインデックスを得る．
  /// @param[in] name メソッド名
  ///
  /// class のみ有効
  /// 該当するメソッドがなければ -1 を返す．
  virtual
  int
  method_index(ShString name) const;

  /// @brief 列挙型の数を得る．
  ///
  /// enum のみ有効
  virtual
  ymuint
  enum_num() const;

  /// @brief 列挙型の要素名を得る．
  /// @param[in] index インデックス ( 0 <= index < enum_num() )
  ///
  /// enum のみ有効
  virtual
  ShString
  enum_name(ymuint index) const;

  /// @brief 列挙型のインデックスを得る．
  /// @param[in] name 列挙型の名前
  ///
  /// enum のみ有効
  /// 該当する名前がなければ -1 を返す．
  virtual
  int
  enum_index(ShString name) const;


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // ID番号
  ymuint mId;

};

// @brief ID番号を得る
//
// おもにハッシュ関数用
inline
ymuint
YmslType::id() const
{
  return mId;
}

END_NAMESPACE_YM_YMSL


BEGIN_NAMESPACE_YM

//////////////////////////////////////////////////////////////////////
// HashFunc<const YmslType*> の特殊化
//////////////////////////////////////////////////////////////////////
template<>
struct
HashFunc<const nsYmsl::YmslType*>
{
  ymuint
  operator()(const nsYmsl::YmslType* key) const
  {
    return key->id();
  }
};

END_NAMESPACE_YM

#endif // YMSLTYPE_H
