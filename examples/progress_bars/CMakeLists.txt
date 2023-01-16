# Include Flint headers.
include_directories(../../src)

# Build executable.
add_executable(progress_bars ${SOURCE_FILES} main.cpp)

# Link libs.
if (WIN32) # Windows
    target_link_libraries(progress_bars glfw Vulkan::Vulkan pathfinder poly2tri harfbuzz icu)
else () # Linux
    target_link_libraries(progress_bars -lglfw -lvulkan -lpathfinder -lpoly2tri -lharfbuzz -licuut)
endif ()
