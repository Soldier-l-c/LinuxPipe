#cmake .
# 生成release项目，用于命令行
# cmake --build . --config Release  

# CMake 最低版本号要求
cmake_minimum_required (VERSION 2.8)

# 项目信息
project (LinuxPipe)

SET(CMAKE_CXX_FLAGS_RELEASE "$ENV{CXXFLAGS} -O3 -Wall -MT -std=c++11 -pthread")
SET(CMAKE_CXX_FLAGS_DEBUG "$ENV{CXXFLAGS} -O0 -Wall -g -ggdb -std=c++11 -pthread")

#生成unicode项目
add_definitions(-DUNICODE -D_UNICODE)

if(MSVC)
#输出变量
message("message $ENV{RAPID_JSON_FOR_LINUX_PIPE_OBJ}")
#增加包含目录
include_directories("$ENV{RAPID_JSON_FOR_LINUX_PIPE_OBJ}")
endif()

# 查找目录下的所有源文件
# 并将名称保存到 DIR_SRCS 变量
aux_source_directory(. DIR_SRCS)

# 指定生成目标
add_executable(${PROJECT_NAME} ${DIR_SRCS})

#连接lib须在指定生成目标之后
FIND_PACKAGE(Threads)
TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${CMAKE_THREAD_LIBS_INIT})