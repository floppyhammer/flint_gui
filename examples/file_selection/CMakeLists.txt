# Include Flint headers.
include_directories(../../src)

# Build executable.
add_executable(file_selection ${SOURCE_FILES} main.cpp)

# Link libs.
if (WIN32) # Windows
    target_link_libraries(file_selection glfw Vulkan::Vulkan pathfinder poly2tri harfbuzz)
else () # Linux
    target_link_libraries(file_selection -lglfw -lvulkan -lpathfinder -lpoly2tri -lharfbuzz)
endif ()
