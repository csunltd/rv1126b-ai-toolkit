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

include (${common_root}/font_engine/api.cmake)
## API ヘッダーファイルパス
set(api_inc 
#    ${CMAKE_SYSROOT}/usr/include/rkfacial 
    ${FONTENGINE_INCLUDE_DIRS} 
    )
## API ソースファイルパス
file(GLOB api_srcs 
    ${FONTENGINE_SOURCE_DIRS} 
#    ./*.c ./*.cpp 
    )
## API 静的ライブラリファイルパス
link_directories(
   ${FONTENGINE_LIBS_DIRS} 
)
## 依存する共有ライブラリファイル
set(sysLib_list 
    ${FONTENGINE_LIBS} 
    )

#--------------------------
# test-font_engine
#--------------------------
set(demoTag test-font_engine)
add_executable(${demoTag} ${demoTag}.cpp ${api_srcs}) #-o
target_link_libraries(${demoTag} ${sysLib_list}) #-l
target_include_directories(${demoTag} PRIVATE ${api_inc}) #-I


