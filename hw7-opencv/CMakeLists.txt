cmake_minimum_required(VERSION 3.10)

project(Homework7)

set(CMAKE_CXX_STANDARD 17)

find_package(OpenCV REQUIRED)

add_executable(Homework7
    main.cpp
    Vector.cpp
    Scene.cpp
    Renderer.cpp
    BVH.cpp
)

target_link_libraries(Homework7 PRIVATE ${OpenCV_LIBS})
target_include_directories(Homework7 PRIVATE
    ${OpenCV_INCLUDE_DIRS}
)

file(COPY ${PROJECT_SOURCE_DIR}/models/ DESTINATION ${PROJECT_BINARY_DIR}/../models/)