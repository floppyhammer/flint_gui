# Include Flint headers.
include_directories(../../src)

# Build executable.
add_executable(container_layout ${SOURCE_FILES} main.cpp)

# Link libs.
if (WIN32) # Windows
    target_link_libraries(container_layout glfw Vulkan::Vulkan pathfinder poly2tri harfbuzz)
else () # Linux
    target_link_libraries(container_layout -lglfw -lvulkan -lpathfinder -lpoly2tri -lharfbuzz)
endif ()
