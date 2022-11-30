# Include Flint headers.
include_directories(../../src)

# Build executable.
add_executable(tabs ${SOURCE_FILES} main.cpp)

# Link libs.
if (WIN32) # Windows
    target_link_libraries(tabs glfw Vulkan::Vulkan pathfinder poly2tri)
else () # Linux
    target_link_libraries(tabs -lglfw -lvulkan -lpathfinder -lpoly2tri)
endif ()
