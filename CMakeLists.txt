cmake_minimum_required(VERSION 3.30)
project(mySTL)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 设置项目根目录为包含路径
include_directories(${PROJECT_SOURCE_DIR})

# 启用测试
enable_testing()

# 创建库目标 - 将所有头文件添加到库中（仅用于IDE识别）
add_library(mySTL INTERFACE)
target_include_directories(mySTL INTERFACE ${PROJECT_SOURCE_DIR})

# 自动收集所有测试文件
file(GLOB_RECURSE TEST_SOURCES "test/*_test.cpp")

# 为每个测试文件创建单独的可执行目标
foreach(test_source ${TEST_SOURCES})
    # 获取完整的相对路径作为测试名称的一部分，避免冲突
    file(RELATIVE_PATH test_relative_path ${PROJECT_SOURCE_DIR} ${test_source})
    string(REPLACE "/" "_" test_name ${test_relative_path})
    string(REPLACE ".cpp" "" test_name ${test_name})

    # 创建测试可执行文件
    add_executable(${test_name} ${test_source})

    # 链接到 mySTL 库
    target_link_libraries(${test_name} PRIVATE mySTL)

    # 添加为CTest测试
    add_test(NAME ${test_name} COMMAND ${test_name})

    message(STATUS "Added test: ${test_name} from ${test_source}")
endforeach()