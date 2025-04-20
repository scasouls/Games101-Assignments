cmake_minimum_required(VERSION 3.10)

project(Homework4)

set(CMAKE_CXX_STANDARD 17)

find_package(OpenCV REQUIRED)
find_package(Eigen3 REQUIRED)

add_executable(Homework4
    main.cpp
)

target_link_libraries(Homework4 PRIVATE ${OpenCV_LIBS})
target_include_directories(Homework4 PRIVATE
    ${OpenCV_INCLUDE_DIRS}
    ${EIGEN3_INCLUDE_DIRS}
)