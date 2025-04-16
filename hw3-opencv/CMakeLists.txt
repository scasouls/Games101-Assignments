cmake_minimum_required(VERSION 3.10)
project(Homework3)

set(CMAKE_CXX_STANDARD 17)

find_package(OpenCV REQUIRED)
find_package(Eigen3 REQUIRED)

add_executable(Homework3
    main.cpp
    rasterizer.cpp
    Triangle.cpp
    Texture.cpp
)

target_link_libraries(Homework3 PRIVATE ${OpenCV_LIBS})
target_include_directories(Homework3 PRIVATE
    ${OpenCV_INCIUDE_DIRS}
    ${EIGEN3_INCLUDE_DIRS}
)

file(COPY ${PROJECT_SOURCE_DIR}/models/ DESTINATION ${PROJECT_BINARY_DIR}/../models/)