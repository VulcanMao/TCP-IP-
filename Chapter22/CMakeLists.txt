#设置所需最小版本为 2.8.11
cmake_minimum_required(VERSION 2.8.11)

#设置工程名
project(Chapter22)

# Linux 上使用
#SET(CMAKE_BUILD_TYPE "Debug")  
#SET(CMAKE_CXX_FLAGS_DEBUG "$ENV{CXXFLAGS} -O0 -Wall -g2 -ggdb")  
#SET(CMAKE_CXX_FLAGS_RELEASE "$ENV{CXXFLAGS} -O3 -Wall")  

#创建工程
add_executable("22_OverlappedRecv" "OverlappedRecv_win.cpp" )

#添加依赖项
target_link_libraries("22_OverlappedRecv"
	ws2_32.lib
)

#创建工程
add_executable("22_OverlappedSend" "OverlappedSend_win.cpp" )

#添加依赖项
target_link_libraries("22_OverlappedSend"
	ws2_32.lib
)

#创建工程
add_executable("22_CmplRoutinesRecv" "CmplRoutinesRecv_win.cpp" )

#添加依赖项
target_link_libraries("22_CmplRoutinesRecv"
	ws2_32.lib
)