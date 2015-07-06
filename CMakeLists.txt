# ===================================================================
# CMAKE のおまじない
# ===================================================================
cmake_minimum_required (VERSION 3.2)


# ===================================================================
# プロジェクト名，バージョンの設定
# ===================================================================
project (gds)
set (GDS_VERSION_MAJOR 1)
set (GDS_VERSION_MINOR 0)
set (GDS_VERSION "${GDS_VERSION_MAJOR}.${GDS_VERSION_MINOR}")


# ===================================================================
# CTest モジュールの読み込み
# ===================================================================
include (CTest)


# ===================================================================
# パッケージの検査
# ===================================================================

include (FindDoxygen)

find_package(BISON REQUIRED)

find_package (GTest)

find_package (YmTools REQUIRED)


# ===================================================================
# インクルードパスの設定
# ===================================================================
include_directories(
  ${PROJECT_SOURCE_DIR}
  ${PROJECT_BINARY_DIR}
  ${YmTools_INCLUDE_DIRS}
  )


# ===================================================================
#  ターゲットの設定
# ===================================================================
add_library(ym_gds
  src/GdsAref.cc
  src/GdsBoundary.cc
  src/GdsBox.cc
  src/GdsData.cc
  src/GdsDumper.cc
  src/GdsElement.cc
  src/GdsFormat.cc
  src/GdsNode.cc
  src/GdsParser.cc
  src/GdsPath.cc
  src/GdsRecMgr.cc
  src/GdsRecTable.cc
  src/GdsRecord.cc
  src/GdsRefBase.cc
  src/GdsScanner.cc
  src/GdsSref.cc
  src/GdsStruct.cc
  src/GdsText.cc
  src/Msg.cc
  )

target_link_libraries(ym_gds
  ym_utils
  )

add_executable(gdsparse
  tests/gdsparse.cc
  )

target_link_libraries(gdsparse
  ym_gds
  )

add_executable(gdsprint
  tests/gdsprint.cc
  )

target_link_libraries(gdsprint
  ym_gds
  )

add_executable(gdsencode
  tests/gdsencode.cc
  )

target_link_libraries(gdsencode
  ym_gds
  )


# ===================================================================
#  インストールターゲットの設定
# ===================================================================

install(
  TARGETS ym_gds
  DESTINATION lib
#  EXPORT ymtools-targets
  )

install(
  DIRECTORY YmGds
  DESTINATION include
  )
