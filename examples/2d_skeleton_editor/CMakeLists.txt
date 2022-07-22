# Include Flint headers.
include_directories(../../src)

# Build executable.
add_executable(2d-skeleton-editor ${SOURCE_FILES} main.cpp)

# Windows
if (WIN32)
    target_link_libraries(2d-skeleton-editor glfw Vulkan::Vulkan pathfinder poly2tri)
else () # Linux
    ## sudo apt install libglfw3-dev
    ## sudo apt install libvulkan-dev
    ## sudo apt install libvulkan1 mesa-vulkan-drivers vulkan-utils
    target_link_libraries(2d-skeleton-editor -lglfw -lvulkan -lpathfinder)
endif ()
