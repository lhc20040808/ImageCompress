cmake_minimum_required(VERSION 3.10.2)

project("imagecompress")

set(lib_dir ${PROJECT_SOURCE_DIR}/../../../libs)
set(include_dir ./include)

#添加头文件目录
include_directories(${include_dir})

add_library(native-lib SHARED native-lib.c)
add_library(libjpeg SHARED IMPORTED)

find_library(log-lib log)
find_library(graphics jnigraphics)

set_target_properties(libjpeg PROPERTIES IMPORTED_LOCATION ${lib_dir}/${ANDROID_ABI}/libjpeg.so)
#set_target_properties(libjpeg PROPERTIES IMPORTED_LOCATION ${CMAKE_CURRENT_SOURCE_DIR}/libs/${ANDROID_ABI}/libjpeg.so)

target_link_libraries(native-lib libjpeg ${log-lib} ${graphics})

