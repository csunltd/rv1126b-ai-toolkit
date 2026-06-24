cmake_minimum_required(VERSION 3.10.2)

STRING(REGEX REPLACE ".*/(.*)" "\\1" CURRENT_FOLDER ${CMAKE_CURRENT_SOURCE_DIR})
MESSAGE("current project: " ${CURRENT_FOLDER})

set(CMAKE_SYSTEM_NAME Linux)
set(CMAKE_CROSSCOMPILING TRUE)

cmake_host_system_information(RESULT arch_value QUERY OS_PLATFORM)
message(STATUS "architecture: " ${arch_value})

if((NOT "${arch_value}" STREQUAL "armv7l") AND (NOT "${arch_value}" STREQUAL "aarch64"))
    include ($ENV{HOME}/configs/cross.cmake)
endif()

#${CMAKE_SYSROOT} 変数を使用する場合、project は set(CMAKE_TOOLCHAIN_FILE "${HOST_DIR}/share/buildroot/toolchainfile.cmake") の後に設定する必要があります
project(${CURRENT_FOLDER})
add_definitions(-Wall -Wpointer-arith)
# use c++ 11
#set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fPIC -Wall")
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -fPIC -Wall")

# ======================== easyeai_api =========================
set(toolkit_root ${CMAKE_CURRENT_SOURCE_DIR}/../../easyeai-api)
set(algorithm_root ${toolkit_root}/algorithm)
set(common_root    ${toolkit_root}/common)
set(media_root     ${toolkit_root}/media)

include (${common_root}/system_opt/api.cmake)
include (${media_root}/gst_opt/api.cmake)
## API ヘッダーファイルパス
set(api_inc 
    ${SYSOPT_INCLUDE_DIRS} 
    ${GSTOPT_INCLUDE_DIRS} 
    ${LOGMGR_INCLUDE_DIRS} 
    ${FQUEUE_INCLUDE_DIRS} 
    ${CODEC_INCLUDE_DIRS} 
    ${RTSP_INCLUDE_DIRS} 
    )
## API ソースファイルパス
file(GLOB api_srcs 
    ${SYSOPT_SOURCE_DIRS} 
    )
## API 静的ライブラリファイルパス
link_directories( 
    ${GSTOPT_LIBS_DIRS} 
    ${FQUEUE_LIBS_DIRS} 
    ${CODEC_LIBS_DIRS} 
    ${RTSP_LIBS_DIRS} 
    ${LOGMGR_LIBS_DIRS} 
)
## API が依存するライブラリパラメータ
set(sysLib_list 
    ${SYSOPT_LIBS} 
    ${GSTOPT_LIBS} 
    ${FQUEUE_LIBS} 
    ${CODEC_LIBS} 
    ${RTSP_LIBS} 
    ${LOGMGR_LIBS} 
    )
# ======================== easyeai_api =========================

# ========================== custom ============================
# solution project
## /usr/share/cmake-3.18/Modules/ ディレクトリ配下の Find*.cmake は、この方法で検索できます
#find_package(OpenCV REQUIRED)

## /usr/lib/aarch64-linux-gnu/pkgconfig/ 配下の .pc ファイルは、この方法で検索できます
## 使用方法：
## find_package(PkgConfig)
## pkg_search_module(<prefix> REQUIRED [xxxx])
## pkg_search_module(<prefix1> REQUIRED [xxxx])
## pkg_search_module(<prefix2> REQUIRED [xxxx] [xxxxxxxxxxx])
## 備考：
##   <prefix>_FOUND：モジュールが見つかった場合、この変数は 1 になります
##   <prefix>_LIBRARIES：モジュールのリンクライブラリ名（-l を含みません）
##   <prefix>_LINK_LIBRARIES：モジュールのリンクライブラリ名（絶対パスを含みます）
##   <prefix>_LIBRARY_DIRS：モジュールのリンクライブラリパス（-L を含みません）
##   <prefix>_INCLUDE_DIRS：モジュールのヘッダーファイルパス（-I を含みません）
##   <prefix>_LDFLAGS：リンク flags
##   <prefix>_LDFLAGS_OTHER：追加のリンク flags
##   <prefix>_CFLAGS：すべてのコンパイラ cflags
##   <prefix>_CFLAGS_OTHER：その他のコンパイラ cflags
## pkg_check_modules と pkg_search_module の相違点／共通点：
##   共通点：機能は同じで、複数のモジュールを検索できます。例：pkg_check_modules(AAAAA REQUIRED libxml-2.0 libxml2 libxml>=2)
##   相違点：
##       pkg_check_modules: すべてのモジュールが存在する場合にのみ有効です
##       pkg_search_module: 最初のモジュールが存在すれば有効です

#find_package(PkgConfig)
#pkg_check_modules(GTK3 REQUIRED gtk+-3.0)
#pkg_search_module(GST1.0 REQUIRED gstreamer-1.0)
#pkg_check_modules(GST1.0_VIDEO REQUIRED gstreamer-video-1.0)

# -I
set(custom_inc
    ./src
    #${OpenCV_INCLUDE_DIRS} 
    #${GTK3_INCLUDE_DIRS} 
    #${GST1.0_INCLUDE_DIRS} 
    #${GST1.0_VIDEO_INCLUDE_DIRS} 
    ${PROJECT_SOURCE_DIR}/include
)
# -L
link_directories(
   #${PROJECT_SOURCE_DIR}/libs 
)
# -l
set(custom_libs
    rga 
    #${OpenCV_LIBS} 
    #${GTK3_LIBRARIES} 
    #${GST1.0_LIBRARIES} 
    #${GST1.0_VIDEO_LIBRARIES} 
)
# source code
aux_source_directory(./src dir_srcs)
aux_source_directory(./src/camera dir_srcs)
# ========================== custom ============================


#--------------------------
# execute file
#--------------------------
add_executable(${CURRENT_FOLDER} ${dir_srcs} ${api_srcs})   #-o
target_include_directories(${CURRENT_FOLDER} PRIVATE ${custom_inc} ${api_inc})
target_link_libraries(${CURRENT_FOLDER} ${custom_libs} ${sysLib_list})

