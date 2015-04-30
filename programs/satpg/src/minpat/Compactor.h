#ifndef COMPACTOR_H
#define COMPACTOR_H

/// @file Compactor.h
/// @brief Compactor のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2015 Yusuke Matsunaga
/// All rights reserved.


#include "satpg_nsdef.h"


BEGIN_NAMESPACE_YM_SATPG

class FgMgr;

//////////////////////////////////////////////////////////////////////
/// @class Compactor Compactor.h "Compactor.h"
/// @brief 故障グループを圧縮するクラス
//////////////////////////////////////////////////////////////////////
class Compactor
{
public:

  /// @brief コンストラクタ
  Compactor();

  /// @brief デストラクタ
  ~Compactor();


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief 故障グループを圧縮する．
  /// @param[in] fmgr 故障グループマネージャ
  /// @param[in] max_node_id ノード番号の最大値
  /// @param[in] group_list もとのグループ番号リスト
  /// @param[in] new_group_list 圧縮後のグループ番号リスト
  void
  run(FgMgr& fgmgr,
      ymuint max_node_id,
      const vector<ymuint>& group_list,
      vector<ymuint>& new_group_list);

  /// @brief verbose フラグを設定する．
  /// @param[in] verbose 指定する値
  void
  set_verbose(ymuint verbose);


private:
  //////////////////////////////////////////////////////////////////////
  // 内部で用いられる関数
  //////////////////////////////////////////////////////////////////////

  /// @brief phase-0
  /// @param[in] fmgr 故障グループマネージャ
  /// @param[inout] group_list 選択されたグループ番号のリスト
  ///
  /// 他のグループに追加条件なしで移動できる故障を見つけ，移動させる．
  void
  phase0(FgMgr& fgmgr,
	 vector<ymuint>& group_list);

  /// @brief phase-1
  /// @param[in] fmgr 故障グループマネージャ
  /// @param[inout] group_list 選択されたグループ番号のリスト
  ///
  /// 他のグループに移動させることでグループを削除する．
  void
  phase1(FgMgr& fgmgr,
	 vector<ymuint>& group_list);

  /// @brief phase-2
  /// @param[in] fmgr 故障グループマネージャ
  /// @param[inout] group_list 選択されたグループ番号のリスト
  ///
  /// 要素数の少ないグループの故障を他のグループに移動する．
  void
  phase2(FgMgr& fgmgr,
	 vector<ymuint>& group_list);


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // verbose フラグ
  ymuint mVerbose;

  // ノード番号の最大値
  ymuint mMaxNodeId;

};

END_NAMESPACE_YM_SATPG

#endif // COMPACTOR_H