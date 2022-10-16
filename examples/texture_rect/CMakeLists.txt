# Include Flint headers.
include_directories(../../src)

# Build executable.
add_executable(texture_rect ${SOURCE_FILES} main.cpp)

# Link libs.
if (WIN32) # Windows
    target_link_libraries(texture_rect glfw Vulkan::Vulkan pathfinder poly2tri)
else () # Linux
    target_link_libraries(texture_rect -lglfw -lvulkan -lpathfinder -lpoly2tri)
endif ()
