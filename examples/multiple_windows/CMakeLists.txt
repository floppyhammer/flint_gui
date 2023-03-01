# Include Flint headers.
include_directories(../../src)

# Build executable.
add_executable(multiple_windows ${SOURCE_FILES} main.cpp)

# Link libs.
if (WIN32) # Windows
    target_link_libraries(multiple_windows glfw Vulkan::Vulkan pathfinder poly2tri harfbuzz icu)
else () # Linux
    target_link_libraries(multiple_windows -lglfw -lvulkan -lpathfinder -lpoly2tri -lharfbuzz -licuut)
endif ()
