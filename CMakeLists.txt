cmake_minimum_required(VERSION 3.16 FATAL_ERROR)
project(Raytracer VERSION 0.1.0)
set(CMAKE_POLICY_DEFAULT_CMP0077 NEW)
set( CMAKE_EXPORT_COMPILE_COMMANDS ON)

file(GLOB_RECURSE CPP_FILES ${CMAKE_CURRENT_LIST_DIR}/src/*.cpp)

add_executable(Raytracer ${CPP_FILES})
set_property(TARGET Raytracer PROPERTY CXX_STANDARD 20)
set_property(TARGET Raytracer PROPERTY CXX_STANDARD_REQUIRED ON)
target_compile_options(Raytracer PUBLIC "/arch:AVX512")

# set(GLM_ENABLE_FAST_MATH ON)
set(GLM_ENABLE_CXX_20 ON)
# set(GLM_ENABLE_SIMD_AVX2 ON)
# set(GLM_ENABLE_SIMD_SSE4_2 ON)
add_subdirectory(${CMAKE_SOURCE_DIR}/minifb)
add_subdirectory(${CMAKE_SOURCE_DIR}/glm)

target_link_libraries(Raytracer PUBLIC minifb glm)
target_include_directories(Raytracer PUBLIC ${CMAKE_CURRENT_LIST_DIR}/src minifb glm/glm)
