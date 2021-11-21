cmake_minimum_required (VERSION 3.15)

project(soft_body)
SET(CMAKE_CXX_STANDARD 20)

find_package (Eigen3 3.3 REQUIRED)
add_subdirectory(external/glad)
add_subdirectory(external/glfw)
find_package(Eigen3 REQUIRED NO_MODULE)

add_executable(${PROJECT_NAME} src/main.cpp)
target_include_directories(${PROJECT_NAME} PUBLIC src)
target_link_libraries(${PROJECT_NAME} PUBLIC glad glfw Eigen3::Eigen)
