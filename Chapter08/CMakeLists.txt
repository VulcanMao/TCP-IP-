#设置所需最小版本为 2.8.11
cmake_minimum_required(VERSION 2.8.11)

#设置工程名
project(Chapter08)

#支持c++11
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x") 
# 或者
# add_definitions(-std=c++11)

# Linux 上使用
#SET(CMAKE_BUILD_TYPE "Debug")  
#SET(CMAKE_CXX_FLAGS_DEBUG "$ENV{CXXFLAGS} -O0 -Wall -g2 -ggdb")  
#SET(CMAKE_CXX_FLAGS_RELEASE "$ENV{CXXFLAGS} -O3 -Wall")  

#创建工程
add_executable("08_gethostbyname" "gethostbyname_win.cpp" )

#添加依赖项
target_link_libraries("08_gethostbyname"
	ws2_32.lib
)

#创建工程
add_executable("08_gethostbyaddr" "gethostbyaddr_win.cpp" )

#添加依赖项
target_link_libraries("08_gethostbyaddr"
	ws2_32.lib
)