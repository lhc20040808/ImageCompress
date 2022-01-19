cmake_minimum_required(VERSION 3.10.2)

project("imagecompress")

set(libjpeg_dir ${PROJECT_SOURCE_DIR}/compress/libs)
set(INC_DIR ./include)
set(libjpeg_lib_dir ${libjpeg_dir})

#添加头文件目录
include_directories(${INC_DIR})

find_library(log-lib log)
find_library(graphics jnigraphics)

add_library(native-lib SHARED native-lib.cpp compress.c)
add_library(libjpeg SHARED IMPORTED)

set_target_properties(libjpeg PROPERTIES IMPORTED_LOCATION ${libjpeg_lib_dir}/${ANDROID_ABI}/libjpeg.so)

target_link_libraries(native-lib libjpeg ${log-lib} ${graphics})

