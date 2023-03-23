# Include Flint headers.
include_directories(../../src)

# Build executable.
add_executable(i18n ${SOURCE_FILES} main.cpp)

# Link libs.
if (WIN32) # Windows
    target_link_libraries(i18n glfw Vulkan::Vulkan pathfinder poly2tri harfbuzz icu zlib)
else () # Linux
    target_link_libraries(i18n -lglfw -lvulkan -lpathfinder -lpoly2tri -lharfbuzz -licuut -lzlib)
endif ()
