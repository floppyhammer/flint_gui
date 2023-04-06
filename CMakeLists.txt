cmake_minimum_required(VERSION 3.16.1)

project(flint)

# Has to be 17 or above.
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/bin")

# Enable Flint's debug mode, which includes visual indicators.
add_compile_definitions(FLINT_DEBUG)

# Fix "too many characters in constant" error when using CMake with MSVC.
string(APPEND CMAKE_CXX_FLAGS " /utf-8")

# More generally, you are using find_package and then later you're using add_subdirectory
# to try to incorporate these external projects. That's not how it works. find_package will
# look for a pre-existing binary of the package, while add_subdirectory isn't designed to
# just swallow entire existing external CMake projects.
# Cf. https://stackoverflow.com/questions/56795645/how-can-i-add-vulkan-to-a-cross-platform-cmake-project
find_package(Vulkan REQUIRED)

# We need to include Vulkan headers manually because of the above reason.
include_directories(${Vulkan_INCLUDE_DIRS})

file(GLOB_RECURSE SOURCE_FILES CONFIGURE_DEPENDS
        src/*.cpp
        third_party/tinyfiledialogs/*.c
        third_party/fribidi/*.c)

add_compile_definitions(PATHFINDER_USE_VULKAN)

# Include external and third-party headers.
include_directories(external third_party)

# GLFW library.
include_directories(external/glfw/include)
add_subdirectory(external/glfw)

# Pathfinder library.
include_directories(external/pathfinder-cpp/include)
add_subdirectory(external/pathfinder-cpp)

include_directories(external/harfbuzz/src)
add_subdirectory(external/harfbuzz)

include_directories(external/glm)
add_subdirectory(external/glm)

include_directories(third_party/poly2tri)
add_subdirectory(third_party/poly2tri)

include_directories(third_party/zlib-1.2.13)
add_subdirectory(third_party/zlib-1.2.13)

include_directories(external/entt/src)
add_subdirectory(external/entt)

# icu4c.
if (NOT WIN32)
    # For non-Windows platforms, we need to compile ICU from source.
    file(APPEND GLOB_RECURSE SOURCE_FILES CONFIGURE_DEPENDS
            third_party/icu4c/common/*.cpp)

    add_compile_definitions(U_STATIC_IMPLEMENTATION)
    add_compile_definitions(U_COMMON_IMPLEMENTATION)
    add_compile_definitions(ICU_STATIC_DATA)
    include_directories(third_party/icu4c/common)
endif ()

# Alternative to icu4c. If the target platform comes with ICU, we just use it.
# Else, we build fribidi. It's too cumbersome to build ICU ourselves.
include_directories(third_party/fribidi)
include_directories(third_party/fribidi/generated)
#add_compile_definitions(DONT_HAVE_FRIBIDI_CONFIG_H)
#add_compile_definitions(DONT_HAVE_FRIBIDI_UNICODE_VERSION_H)
add_compile_definitions(HAVE_STRINGIZE=1)
add_compile_definitions(FRIBIDI_BUILD)

add_library(flint ${SOURCE_FILES})

set(LINKING_LIBS glfw Vulkan::Vulkan pathfinder poly2tri harfbuzz icu zlib)

# Add examples.
add_subdirectory(examples/file_selection)
add_subdirectory(examples/2d_skeleton)
add_subdirectory(examples/3d_model_viewer)
add_subdirectory(examples/container_layout)
add_subdirectory(examples/texture_rect)
add_subdirectory(examples/scrolling)
add_subdirectory(examples/tabs)
add_subdirectory(examples/progress_bars)
add_subdirectory(examples/i18n)
add_subdirectory(examples/labels)
add_subdirectory(examples/multiple_windows)
add_subdirectory(examples/ecs)
