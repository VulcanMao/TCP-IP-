#设置所需最小版本为 2.8.11
cmake_minimum_required(VERSION 2.8.11)

#设置工程名
project(Chapter09)

#支持c++11
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x") 
# 或者
# add_definitions(-std=c++11)

# Linux 上使用
#SET(CMAKE_BUILD_TYPE "Debug")  
#SET(CMAKE_CXX_FLAGS_DEBUG "$ENV{CXXFLAGS} -O0 -Wall -g2 -ggdb")  
#SET(CMAKE_CXX_FLAGS_RELEASE "$ENV{CXXFLAGS} -O3 -Wall")  

#创建工程
add_executable("09_get_buf" "get_buf_win.cpp" )

#添加依赖项
target_link_libraries("09_get_buf"
	ws2_32.lib
)

add_executable("09_sock_type" "sock_type_win.cpp" )
target_link_libraries("09_sock_type"
	ws2_32.lib
)

add_executable("09_reuseadr_eserver" "reuseadr_eserver_win.cpp" )
target_link_libraries("09_reuseadr_eserver"
	ws2_32.lib
)


