#设置所需最小版本为 2.8.11
cmake_minimum_required(VERSION 2.8.11)

#设置工程名
project(Chapter21)

# Linux 上使用
#SET(CMAKE_BUILD_TYPE "Debug")  
#SET(CMAKE_CXX_FLAGS_DEBUG "$ENV{CXXFLAGS} -O0 -Wall -g2 -ggdb")  
#SET(CMAKE_CXX_FLAGS_RELEASE "$ENV{CXXFLAGS} -O3 -Wall")  

#创建工程
add_executable("21_server" "AsynNotiEchoServ_win.cpp" )

#添加依赖项
target_link_libraries("21_server"
	ws2_32.lib
)

#创建工程
add_executable("21_client" "echo_client_win.cpp" )

#添加依赖项
target_link_libraries("21_client"
	ws2_32.lib
)