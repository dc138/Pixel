cmake_minimum_required (VERSION 3.21)
project (Pixel)

set(default_build_type "Release")

if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
  message(STATUS "Setting build type to '${default_build_type}' as none was specified.")
  set(CMAKE_BUILD_TYPE "${default_build_type}" CACHE STRING "Choose the type of build." FORCE)
  set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release" "MinSizeRel" "RelWithDebInfo")
endif()

file(GLOB_RECURSE Pixel_src
     "src/*.cpp"
)

add_library(Pixel ${Pixel_src})

add_compile_options(
  -Wall -Werror -Werror -Wno-unused-parameter
   $<$<CONFIG:RELEASE>:-Ofast>
   $<$<CONFIG:RELEASE>:-flto>
   $<$<CONFIG:DEBUG>:-O0>
   $<$<CONFIG:DEBUG>:-ggdb3>
)

add_compile_definitions(
  $<$<CONFIG:RELEASE>:NDEBUG>
)

set_property(TARGET Pixel PROPERTY CXX_STANDARD 20)
set_property(TARGET Pixel PROPERTY C_STANDARD 17)

target_include_directories(Pixel PUBLIC include)
target_include_directories(Pixel PUBLIC lib)
target_precompile_headers(Pixel PUBLIC include/pch.hpp)

# Libraries

add_subdirectory(lib/glfw EXCLUDE_FROM_ALL)
target_link_libraries(Pixel PRIVATE glfw)

add_definitions(-DGLEW_STATIC)
add_subdirectory(lib/glew EXCLUDE_FROM_ALL)
target_link_libraries(Pixel PRIVATE libglew_static)

add_subdirectory(lib/imgui EXCLUDE_FROM_ALL)
target_link_libraries(Pixel PRIVATE imgui)

add_subdirectory(lib/glm EXCLUDE_FROM_ALL)
target_link_libraries(Pixel PRIVATE glm)

add_subdirectory(lib/stb-image EXCLUDE_FROM_ALL)
target_link_libraries(Pixel PRIVATE stb-image)

