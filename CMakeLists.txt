cmake_minimum_required(VERSION 3.16.1)
project(flint)

# Has to be 17 or above.
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/bin")

find_package(Vulkan REQUIRED FATAL_ERROR)

file(GLOB_RECURSE SOURCE_FILES CONFIGURE_DEPENDS src/*.cpp src/*.h)

# Include external and third party headers.
include_directories(external/eigen third_party)

# GLFW library.
include_directories(external/glfw/include)
add_subdirectory(external/glfw)

add_executable(flint ${SOURCE_FILES})

# Windows
if (WIN32)
    target_link_libraries(flint glfw Vulkan::Vulkan)
else () # Linux
    ## sudo apt install libglfw3-dev
    ## sudo apt install libvulkan-dev
    ## sudo apt install libvulkan1 mesa-vulkan-drivers vulkan-utils
    target_link_libraries(flint -lglfw -lvulkan)
endif ()
