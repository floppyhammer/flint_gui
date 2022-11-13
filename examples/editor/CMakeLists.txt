# Include Flint headers.
include_directories(../../src)

# Build executable.
add_executable(editor ${SOURCE_FILES} main.cpp)

# Link libs.
if (WIN32) # Windows
    target_link_libraries(editor glfw Vulkan::Vulkan pathfinder poly2tri)
else () # Linux
    target_link_libraries(editor -lglfw -lvulkan -lpathfinder -lpoly2tri)
endif ()
