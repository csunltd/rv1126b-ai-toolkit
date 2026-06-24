
##/usr/share/cmake-3.18/Modules/ ディレクトリ配下の Find*.cmake は、この方法で検索できます
##具体的な変数（例：${CURL_INCLUDE_DIRS}、${CURL_LIBRARIES}）は、対応する Find*.cmake ファイル内で定義されます（通常は先頭の説明に記載されています）
find_package(CURL REQUIRED)

# source code path
file(GLOB HTTP_SOURCE_DIRS
    ${CMAKE_CURRENT_LIST_DIR}/*.c 
    ${CMAKE_CURRENT_LIST_DIR}/*.cpp 
    )

# headfile path
set(HTTP_INCLUDE_DIRS
    ${CURL_INCLUDE_DIRS}
    ${CMAKE_CURRENT_LIST_DIR} 
    )

# c/c++ flags
set(HTTP_LIBS 
    ${CURL_LIBRARIES}
    stdc++ 
    )
