# ===================================================================
# CMAKE のおまじない
# ===================================================================
cmake_minimum_required (VERSION 3.2)


# ===================================================================
# プロジェクト名，バージョンの設定
# ===================================================================
project (YmTools)
set (YMTOOLS_VERSION_MAJOR 1)
set (YMTOOLS_VERSION_MINOR 0)
set (YMTOOLS_VERSION "${YMTOOLS_VERSION_MAJOR}.${YMTOOLS_VERSION_MINOR}")

# 名前空間
set (YMTOOLS_NAMESPACE "nsYm")

# リリースモード
#set (CMAKE_BUILD_TYPE RELEASE)

# デバッグモード
#set (CMAKE_BUILD_TYPE DEBUG)


# ===================================================================
# オプション
# ===================================================================

# google-perftools を使う時にこのオプションを ON にする．
option (OPT_GPERFTOOLS "use google-perftools" OFF)


# ===================================================================
# CTest モジュールの読み込み
# ===================================================================
include (CTest)


# ===================================================================
# パッケージの検査
# ===================================================================
list (APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

include (FindDoxygen)

find_package(BISON REQUIRED)

#include (FindPythonInterp)
#include (FindPythonLibs)

#include (FindQt4)
#find_package(Qt4 4.8.0 REQUIRED QtCore QtGui QtXml)
#include(${QT_USE_FILE})

#set( CMAKE_INCLUDE_CURRENT_DIR ON )
#find_package(Qt5Widgets)
#find_package(OpenGL)

find_package(TCL)

if ( OPT_GPERFTOOLS )
  find_package(Gperftools)
endif (OPT_GPERFTOOLS)

find_package (GTest)

include (FindReadline)
include (FindCurses)

if ( UNIX )
  set ( YM_UNIX 1 )
endif ( UNIX )
if ( APPLE )
  set ( YM_APPLE 1 )
endif ( APPLE )
if ( WIN32 )
  set ( YM_WIN32 1 )
endif ( WIN32 )


# ===================================================================
# ヘッダファイルの検査
# ===================================================================
include (CheckIncludeFileCXX)
check_include_file_cxx (stdlib.h    HAVE_STDLIB_H)
check_include_file_cxx (string.h    HAVE_STRING_H)
check_include_file_cxx (strings.h   HAVE_STRINGS_H)
check_include_file_cxx (unistd.h    HAVE_UNISTD_H)
check_include_file_cxx (limits.h    HAVE_LIMITS_H)
check_include_file_cxx (float.h     HAVE_FLOAT_H)
check_include_file_cxx (math.h      HAVE_MATH_H)
check_include_file_cxx (sys/time.h  HAVE_SYS_TIME_H)
check_include_file_cxx (time.h      HAVE_TIME_H)
check_include_file_cxx (sys/stat.h  HAVE_SYS_STAT_H)
check_include_file_cxx (pwd.h       HAVE_PWD_H)
check_include_file_cxx (sys/param.h HAVE_SYS_PARAM_H)


# ===================================================================
# ライブラリ関数の検査
# ===================================================================
include (CheckFunctionExists)
check_function_exists (getrusage    HAVE_GETRUSAGE)
check_function_exists (times        HAVE_TIMES)


# ===================================================================
# 組み込み型のサイズの検査
# ===================================================================
include (CheckTypeSize)
check_type_size (short       SHORT_SIZE)
check_type_size (int         INT_SIZE)
check_type_size (long        LONG_SIZE)
check_type_size ("long long" LLONG_SIZE)
check_type_size ("void *"    VOIDP_SIZE)

if ( SHORT_SIZE EQUAL 2 )
  set( INT16_TYPE short )
  set( UINT16_TYPE "unsigned short" )
elseif ( INT_SIZE EQUAL 2 )
  set( INT16_TYPE int )
  set( UINT16_TYPE "unsigned int" )
else ()
  # エラー
  message ( FATAL_ERROR
    "No valid type for 16-bit integer" )
endif ()

if ( SHORT_SIZE EQUAL 4 )
  set( INT32_TYPE short )
  set( UINT32_TYPE "unsigned short" )
elseif ( INT_SIZE EQUAL 4 )
  set( INT32_TYPE int )
  set( UINT32_TYPE "unsigned int" )
elseif ( LONG_SIZE EQUAL 4 )
  set( INT32_TYPE long )
  set( UINT32_TYPE "unsigned long" )
else ()
  # エラー
  message ( FATAL_ERROR
    "No valid type for 32-bit integer" )
endif ()

if ( INT_SIZE EQUAL 8 )
  set( INT64_TYPE int )
  set( UINT64_TYPE "unsigned int" )
elseif ( LONG_SIZE EQUAL 8 )
  set( INT64_TYPE long )
  set( UINT64_TYPE "unsigned long" )
elseif ( LLONG_SIZE EQUAL 8 )
  set( INT64_TYPE "lon long" )
  set( UINT64_TYPE "unsigned long long" )
else ()
  # エラー
  message ( FATAL_ERROR
    "No valid type for 64-bit integer" )
endif ()

if ( INT_SIZE EQUAL VOIDP_SIZE )
  set( PTRINT_TYPE int )
elseif ( LONG_SIZE EQUAL VOIDP_SIZE )
  set( PTRINT_TYPE long )
elseif ( LLONG_SIZE EQUAL VOIDP_SIZE )
  set( PTRINT_TYPE "long long" )
else ()
  # エラー
  message ( FATAL_ERROR
    "No valid type for pointer size integer" )
endif ()


# ===================================================================
# サブディレクトリの設定
# ===================================================================
add_subdirectory (libym_utils)
add_subdirectory (libym_algo)
add_subdirectory (libym_logic)
add_subdirectory (libym_cell)
add_subdirectory (libym_verilog)
add_subdirectory (libym_networks)

if (TCL_FOUND)
  add_subdirectory (libym_tclpp)
endif (TCL_FOUND)


# ===================================================================
# YmTools.h の生成
# ===================================================================

configure_file(
  "${PROJECT_SOURCE_DIR}/etc/YmTools.h.in"
  "${PROJECT_BINARY_DIR}/YmTools.h"
  )


# ===================================================================
# doxygen 起動用のコマンド
# ===================================================================

if (DOXYGEN_FOUND)
  configure_file (
    "${PROJECT_SOURCE_DIR}/etc/Doxyfile.in"
    "${PROJECT_BINARY_DIR}/Doxyfile"
    )

  add_custom_target(dox
    "${DOXYGEN_EXECUTABLE}" ">" "${PROJECT_BINARY_DIR}/doxygen.log"
    DEPENDS "${PROJECT_BINARY_DIR}/Doxyfile"
    WORKING_DIRECTORY "${PROJECT_BINARY_DIR}"
    COMMENT "generating doxygen documents"
    )
endif (DOXYGEN_FOUND)


# ===================================================================
# パッケージコンフィグレーションファイルの生成
# ===================================================================

configure_file(
  ${PROJECT_SOURCE_DIR}/etc/YmToolsConfig.cmake.in
  ${PROJECT_BINARY_DIR}/YmToolsConfig.cmake
  @ONLY
  )

configure_file(
  ${PROJECT_SOURCE_DIR}/etc/YmToolsConfig-version.cmake.in
  ${PROJECT_BINARY_DIR}/YmToolsConfig-version.cmake
  @ONLY
  )


# ===================================================================
# インストールターゲット
# ===================================================================

install (
  FILES "${PROJECT_BINARY_DIR}/YmTools.h"
  DESTINATION include
  )

foreach ( _header "acc_user.h" "veriuser.h" "vpi_user.h" )
  install (
    FILES "${PROJECT_SOURCE_DIR}/include/${_header}"
    DESTINATION include
    )
endforeach ()

install (
  EXPORT ymtools-targets
  DESTINATION lib/ymtools-${YMTOOLS_VERSION}
  )

install (FILES
  ${PROJECT_BINARY_DIR}/YmToolsConfig.cmake
  ${PROJECT_BINARY_DIR}/YmToolsConfig-version.cmake
  DESTINATION lib/ymtools-${YMTOOLS_VERSION}
  )


# ===================================================================
# end-of-CMakeLists.txt
# ===================================================================
