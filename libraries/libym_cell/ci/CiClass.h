#ifndef CICLASS_H
#define CICLASS_H

/// @file CiClass.h
/// @brief CiClass のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2005-2011 Yusuke Matsunaga
/// All rights reserved.


#include "ym_cell/CellClass.h"
#include "ym_utils/Alloc.h"


BEGIN_NAMESPACE_YM_CELL

//////////////////////////////////////////////////////////////////////
/// @class CiClass CiClass.h "CiClass.h"
/// @brief CellClass の実装クラスの基底クラス
//////////////////////////////////////////////////////////////////////
class CiClass :
  public CellClass
{
public:

  /// @brief コンストラクタ
  CiClass();

  /// @brief デストラクタ
  virtual
  ~CiClass();


public:
  //////////////////////////////////////////////////////////////////////
  // 一般的な情報を取得する関数
  //////////////////////////////////////////////////////////////////////

  /// @brief ID番号を返す．
  /// @note CellLibrary::npn_class(id) で返されるオブジェクトの id() は id となる．
  virtual
  ymuint
  id() const;

  /// @brief 同位体変換リストを得る．
  virtual
  const vector<NpnMapM>&
  idmap_list() const;


public:
  //////////////////////////////////////////////////////////////////////
  // このクラスに属しているセルグループの情報を取得する関数
  //////////////////////////////////////////////////////////////////////

  /// @brief セルグループの数を返す．
  virtual
  ymuint
  group_num() const;

  /// @brief セルグループを返す．
  /// @param[in] pos 位置番号 ( 0 <= pos < group_num() )
  virtual
  const CellGroup*
  cell_group(ymuint pos) const;


public:
  //////////////////////////////////////////////////////////////////////
  // 情報を設定する関数
  //////////////////////////////////////////////////////////////////////

  /// @brief 初期化する．
  /// @param[in] id ID番号
  /// @param[in] idmap_list 同位体変換リスト
  /// @param[in] group_list グループのリスト
  /// @param[in] alloc メモリアロケータ
  void
  init(ymuint id,
       const vector<NpnMapM>& idmap_list,
       const vector<const CellGroup*>& group_list,
       AllocBase& alloc);


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // ID番号
  ymuint32 mId;

  // 同位体変換リスト
  vector<NpnMapM> mIdmapList;

  // グループの数
  ymuint32 mGroupNum;

  // グループの配列
  const CellGroup** mGroupList;

};

END_NAMESPACE_YM_CELL

#endif // CICLASS_H
