##/usr/share/cmake-3.18/Modules/ ディレクトリ配下の Find*.cmake は、この方法で検索できます
##具体的な変数（例：${OpenCV_INCLUDE_DIRS}、${OpenCV_LIBS}）は、対応する Find*.cmake ファイル内で定義されます（通常は先頭の説明に記載されています）
#find_package(OpenCV REQUIRED)
#
set(OpenCV_INCLUDE_DIRS
    ${CMAKE_SYSROOT}/usr/include/ 
    ${CMAKE_SYSROOT}/usr/include/opencv4/ 
)
set(OpenCV_LIBS_DIRS
    ${CAMKE_SYSROOT}/usr/lib/aarch64-linux-gnu/lapack
    ${CAMKE_SYSROOT}/usr/lib/aarch64-linux-gnu/blas
)
set(OpenCV_LIBS
    opencv_core 
    opencv_imgproc 
    opencv_imgcodecs
)

##/usr/lib/aarch64-linux-gnu/pkgconfig/ 配下の .pc ファイルは、この方法で検索できます
find_package(PkgConfig)
pkg_search_module(FREETYPE2 REQUIRED freetype2)
#
#set(FREETYPE2_INCLUDE_DIRS
#    ${CMAKE_SYSROOT}/usr/include/ 
#    ${CMAKE_SYSROOT}/usr/include/opencv4/ 
#)
#set(FREETYPR2_LIBS
#    freetype2 
#)

# source code path
file(GLOB FONTENGINE_SOURCE_DIRS
    ${CMAKE_CURRENT_LIST_DIR}/*.c 
    ${CMAKE_CURRENT_LIST_DIR}/*.cpp 
    )

# static Library paths
set(FONTENGINE_LIBS_DIRS
    ${CMAKE_CURRENT_LIST_DIR}
    ${OpenCV_LIBS_DIRS}
    )

# headfile path
set(FONTENGINE_INCLUDE_DIRS
    ${OpenCV_INCLUDE_DIRS} 
    ${FREETYPE2_INCLUDE_DIRS}
    ${CMAKE_CURRENT_LIST_DIR} 
    ${CMAKE_CURRENT_LIST_DIR}/iconv 
    )

# c/c++ flags
set(FONTENGINE_LIBS 
    ${OpenCV_LIBS} 
    ${FREETYPE2_LIBS} 
    freetype 
    stdc++ 
    )
