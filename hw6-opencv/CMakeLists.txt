cmake_minimum_required(VERSION 3.10)

project(Homework6)

set(CMAKE_CXX_STANDARD 17)

find_package(OpenCV REQUIRED)

add_executable(Homework6
    main.cpp
    Vector.cpp
    Scene.cpp
    Renderer.cpp
    BVH.cpp
)

target_link_libraries(Homework6 PRIVATE ${OpenCV_LIBS})
target_include_directories(Homework6 PRIVATE
    ${OpenCV_INCLUDE_DIRS}
)

file(COPY ${PROJECT_SOURCE_DIR}/models/ DESTINATION ${PROJECT_BINARY_DIR}/../models/)