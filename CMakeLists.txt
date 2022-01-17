cmake_minimum_required(VERSION 3.16.1)
project(simple-vulkan-renderer)

# Has to be Standard 17.
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/bin")

add_executable(simple-vulkan-renderer
        src/main.cpp
        src/app.h
        src/app.cpp
        )

# Windows
# ---------------------------------------
set(Vulkan_LIBRARY_DIR "C:/VulkanSDK/1.2.182.0/Lib")
set(Vulkan_INCLUDE_DIR "C:/VulkanSDK/1.2.182.0/Include")

set(GLFW_INCLUDE_DIR "${CMAKE_CURRENT_LIST_DIR}/external/inc")
set(GLFW_LIBRARY_DIR "${CMAKE_CURRENT_LIST_DIR}/external/lib/lib-vc2019")

set(GLM_INCLUDE_DIR "${CMAKE_CURRENT_LIST_DIR}/third_party")

include_directories(
        ${Vulkan_INCLUDE_DIR}
        ${GLFW_INCLUDE_DIR}
        ${GLM_INCLUDE_DIR}
)

target_link_libraries(simple-vulkan-renderer
        ${Vulkan_LIBRARY_DIR}/vulkan-1.lib
        ${GLFW_LIBRARY_DIR}/glfw3.lib
        )
# ---------------------------------------

# Linux
# ---------------------------------------
#include_directories(
#        ${CMAKE_CURRENT_LIST_DIR}/external
#)
#
## sudo apt install libglfw3-dev
## sudo apt install libvulkan-dev
## sudo apt install libvulkan1 mesa-vulkan-drivers vulkan-utils
#target_link_libraries(simple-vulkan-renderer -lglfw -lvulkan)
# ---------------------------------------
