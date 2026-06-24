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
#    opencv_calib3d 
#    opencv_dnn 
#    opencv_features2d 
#    opencv_flann 
#    opencv_highgui 
#    opencv_ml 
#    opencv_objdetect 
#    opencv_photo 
#    opencv_stitching
#    opencv_videoio 
#    opencv_video  
)

# source code path
file(GLOB PERSON_POSE_SOURCE_DIRS
    ${CMAKE_CURRENT_LIST_DIR}/*.cpp 
    )

# static Library paths
set(PERSON_POSE_LIBS_DIRS
    ${CMAKE_CURRENT_LIST_DIR}
    ${OpenCV_LIBS_DIRS}
    )

# headfile path
set(PERSON_POSE_INCLUDE_DIRS
    ${OpenCV_INCLUDE_DIRS} 
    ${CMAKE_CURRENT_LIST_DIR} 
    )

# c/c++ flags
set(PERSON_POSE_LIBS
    person_pose 
    rknnrt
    ${OpenCV_LIBS} 
    pthread
    stdc++ 
    )
