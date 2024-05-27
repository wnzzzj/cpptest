cmake_minimum_required(VERSION 3.10)

# 设置项目名称和版本号
project(TaxesCard VERSION 1.0)

# 指定 C++ 标准
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# 添加源文件
add_executable(TaxesCard TaxesCard.cpp)

# 检查编译器并设置编译选项
if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    target_compile_options(TaxesCard PRIVATE -Wall -Wextra -pedantic)
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    target_compile_options(TaxesCard PRIVATE /W4)
endif()

# 添加调试信息
set(CMAKE_BUILD_TYPE Debug)

# 包含头文件目录（如果有其他头文件目录，可以在这里添加）
include_directories(${PROJECT_SOURCE_DIR}/include)

# 链接额外的库（如果有其他库依赖，可以在这里添加）
# target_link_libraries(TaxesCard PRIVATE <library_name>)
