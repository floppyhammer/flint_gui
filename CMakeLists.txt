cmake_minimum_required(VERSION 3.16.1)
project(flint)

# Has to be above 17.
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/bin")

find_package(Vulkan REQUIRED FATAL_ERROR)

file(GLOB_RECURSE SOURCE_FILES CONFIGURE_DEPENDS src/*.cpp src/*.h)

add_executable(flint ${SOURCE_FILES})

set(GLFW_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/external/inc")
set(GLM_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/third_party")

include_directories(
        ${GLFW_INCLUDE_DIR}
        ${GLM_INCLUDE_DIR}
)

# Windows
if (WIN32)
    set(GLFW_LIBRARY_DIR "${CMAKE_CURRENT_SOURCE_DIR}/external/lib/lib-vc2019")

    target_link_libraries(flint
            Vulkan::Vulkan
            ${GLFW_LIBRARY_DIR}/glfw3.lib
            )
else () # Linux
    ## sudo apt install libglfw3-dev
    ## sudo apt install libvulkan-dev
    ## sudo apt install libvulkan1 mesa-vulkan-drivers vulkan-utils
    target_link_libraries(vulkan-demo-app -lglfw -lvulkan)
endif ()
