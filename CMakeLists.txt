cmake_minimum_required(VERSION 3.16.1)
project(flint)

# Has to be 17 or above.
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/bin")

# More generally, you are using find_package and then later you're using add_subdirectory
# to try to incorporate these external projects. That's not how it works. find_package will
# look for a pre-existing binary of the package, while add_subdirectory isn't designed to
# just swallow entire existing external CMake projects.
# Cf. https://stackoverflow.com/questions/56795645/how-can-i-add-vulkan-to-a-cross-platform-cmake-project
find_package(Vulkan REQUIRED FATAL_ERROR)

# We need to include Vulkan headers manually because of the above reason.
include_directories("C:/VulkanSDK/1.3.204.0/Include")

file(GLOB_RECURSE SOURCE_FILES CONFIGURE_DEPENDS src/*.cpp src/*.h)

add_compile_definitions(PATHFINDER_USE_VULKAN)

# Include external and third party headers.
include_directories(external/eigen third_party)

# GLFW library.
include_directories(external/glfw/include)
add_subdirectory(external/glfw)

# Pathfinder library.
include_directories(external/pathfinder-cpp/include)
add_subdirectory(external/pathfinder-cpp)

add_subdirectory(third_party/poly2tri)

add_library(flint ${SOURCE_FILES})

add_subdirectory(examples/2d_skeleton_editor)
