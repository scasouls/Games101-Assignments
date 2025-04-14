cmake_minimum_required(VERSION 3.10)
project(Homework2)

set(CMAKE_CXX_STANDARD 17)

# 使用 vcpkg 自动集成 OpenCV 和 Eigen
find_package(OpenCV REQUIRED)
find_package(Eigen3 REQUIRED)

add_executable(Homework2
    main.cpp
    rasterizer.cpp
    Triangle.cpp
)

target_link_libraries(Homework2 PRIVATE ${OpenCV_LIBS})
target_include_directories(Homework2 PRIVATE
    ${OpenCV_INCLUDE_DIRS}
    ${EIGEN3_INCLUDE_DIRS}
)
