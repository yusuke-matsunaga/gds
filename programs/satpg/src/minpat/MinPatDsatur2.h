#ifndef MINPATDSATUR2_H
#define MINPATDSATUR2_H

/// @file MinPatDsatur2.h
/// @brief MinPatDsatur2 のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2015 Yusuke Matsunaga
/// All rights reserved.


#include "MinPatBase.h"


BEGIN_NAMESPACE_YM_SATPG

//////////////////////////////////////////////////////////////////////
/// @class MinPatDsatur2 MinPatDsatur2.h "MinPatDsatur2.h"
/// @brief 'Dsatur' っぽい MinPat
//////////////////////////////////////////////////////////////////////
class MinPatDsatur2 :
  public MinPatBase
{
public:

  /// @brief コンストラクタ
  MinPatDsatur2();

  /// @brief デストラクタ
  ~MinPatDsatur2();


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////


private:
  //////////////////////////////////////////////////////////////////////
  // 内部で用いられる関数
  //////////////////////////////////////////////////////////////////////

  /// @brief 初期化を行う．
  /// @param[in] fault_list 検出された故障のリスト
  /// @param[in] tvmgr テストベクタマネージャ
  /// @param[in] fsim2 2値の故障シミュレータ(検証用)
  virtual
  void
  init(const vector<const TpgFault*>& fault_list,
       TvMgr& tvmgr,
       Fsim& fsim2);

  /// @brief 対象の全故障数を返す．
  virtual
  ymuint
  fault_num();

  /// @brief 最初の故障を選ぶ．
  virtual
  const TpgFault*
  get_first_fault();

  /// @brief 次に処理すべき故障を選ぶ．
  /// @param[in] fgmgr 故障グループを管理するオブジェクト
  /// @param[in] group_list 現在のグループリスト
  ///
  /// 故障が残っていなければ NULL を返す．
  virtual
  const TpgFault*
  get_next_fault(FgMgr& fgmgr,
		 const vector<ymuint>& group_list);

  /// @brief 故障を追加するグループを選ぶ．
  /// @param[in] fgmgr 故障グループを管理するオブジェクト
  /// @param[in] fault 故障
  /// @param[in] group_list 現在のグループリスト
  ///
  /// グループが見つからなければ fgmgr.group_num() を返す．
  virtual
  ymuint
  find_group(FgMgr& fgmgr,
	     const TpgFault* fault,
	     const vector<ymuint>& group_list);


private:
  //////////////////////////////////////////////////////////////////////
  // 内部で用いられるデータ構造
  //////////////////////////////////////////////////////////////////////

  // 故障ごとの情報
  struct FaultStruct
  {
    // 故障
    const TpgFault* mFault;

    // 選択済みマーク
    bool mSelected;

    // 衝突している故障番号のリスト
    vector<ymuint> mConflictList;

    // 衝突数
    ymuint mConflictNum;

    // 衝突したグループ番号のマップ
    vector<bool> mConflictMap;

  };


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // 故障用の作業領域のリスト
  vector<FaultStruct> mFaultStructList;

  // 故障番号から FaultStruct の位置を引くためのマップ
  vector<ymuint> mFaultMap;

  // 故障数
  ymuint mFaultNum;

  // 未処理の故障数
  ymuint mRemainNum;

  // 前回選ばれた故障
  ymuint mPrevFpos;

  // 前回選ばれたグループ番号
  ymuint mPrevGid;
};

END_NAMESPACE_YM_SATPG

#endif // MINPATDSATUR2_H