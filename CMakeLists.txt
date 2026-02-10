cmake_minimum_required(VERSION 3.16)

# 确保编译器会搜索本地的 components 文件夹
set(EXTRA_COMPONENT_DIRS "components")

include($ENV{IDF_PATH}/tools/cmake/project.cmake)
project(hello_world)