cmake_minimum_required(VERSION 3.0.0)
project(R308 VERSION 0.1.0)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ../)

include(CTest)
find_package(OpenGL REQUIRED)
find_package(GLEW REQUIRED)
find_package(PkgConfig REQUIRED)

pkg_search_module(GLFW REQUIRED glfw3)
pkg_search_module(ASSIMP REQUIRED assimp)

include_directories(src "ext/stb" ${ASSIMP_INCLUDE_DIRS} ${OPENGL_INCLUDE_DIRS} ${GLEW_INCLUDE_DIRS} ${GLFW_INCLUDE_DIRS})
enable_testing()
file(GLOB SOURCES "ext/stb/stb.c" "src/*.cpp" "src/*.hpp")

add_executable(R308 src/main.cpp ${SOURCES})
target_link_libraries(R308 ${ASSIMP_LIBRARIES} ${OPENGL_LIBRARIES} ${GLEW_LIBRARIES} ${GLFW_LIBRARIES})

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)

