# CMake 最低版本要求
cmake_minimum_required(VERSION 3.10)

# 项目名称
project(testLLM)

# 设置C++标准 
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 设置构建类型为Debug 
set(CMAKE_BUILD_TYPE Debug)

# 添加可执行文件
add_executable(testLLM testLLM.cpp 
../sdk/src/util/myLog.cpp
../sdk/src/DeepSeekProvider.cpp)

# 设置可执行文件输出目录
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR})

# 添加头文件搜索路径
include_directories(${CMAKE_PROJECT_INCLUDE_DIR}/../sdk/include)

# 查找OpenSSL包
find_package(OpenSSL REQUIRED)

# 包含OpenSSL头文件路径
include_directories(${OPENSSL_INCLUDE_DIR})

# 添加 CPPHTTPLIB_OPENSSL_SUPPORT 定义
target_compile_definitions(testLLM PRIVATE CPPHTTPLIB_OPENSSL_SUPPORT)

# 链接库
target_link_libraries(testLLM jsoncpp fmt spdlog gtest OpenSSL::SSL OpenSSL::Crypto)
