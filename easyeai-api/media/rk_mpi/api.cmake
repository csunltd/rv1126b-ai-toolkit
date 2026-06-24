# source code path
file(GLOB RKMPI_SOURCE_DIRS
    ${CMAKE_CURRENT_LIST_DIR}/*.c 
    ${CMAKE_CURRENT_LIST_DIR}/*.cpp 
    )

# headfile path
set(RKMPI_INCLUDE_DIRS
    ${CMAKE_CURRENT_LIST_DIR} 
    #ここをコメントアウトしているのは、/usr/include/rockchip/ 内の rk_type.h を使用しないためです
    #/usr/include/rockchip 
    )

# c/c++ flags
set(RKMPI_LIBS 
    rockit
    )
