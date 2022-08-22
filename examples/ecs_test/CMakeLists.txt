# Include Flint headers.
include_directories(../../src)

# Build executable.
add_executable(ecs_test ${SOURCE_FILES} main.cpp)

# Windows
if (WIN32)
    target_link_libraries(ecs_test glfw Vulkan::Vulkan pathfinder poly2tri)
else () # Linux
    ## sudo apt install libglfw3-dev
    ## sudo apt install libvulkan-dev
    ## sudo apt install libvulkan1 mesa-vulkan-drivers vulkan-utils
    target_link_libraries(ecs_test -lglfw -lvulkan -lpathfinder -lpoly2tri)
endif ()
