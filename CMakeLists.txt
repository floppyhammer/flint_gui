cmake_minimum_required(VERSION 3.16.1)

project(flint)

# Has to be 17 or above.
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/bin")

# Enable Flint's debug mode, which includes visual indicators.
add_compile_definitions(FLINT_DEBUG)

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
        third_party/fribidi/*.c
        third_party/ttf2mesh/ttf2mesh.c)

# Include external and third-party headers.
include_directories(third_party)

# GLFW library.
include_directories(external/glfw/include)
add_subdirectory(external/glfw)

# Pathfinder library.
add_compile_definitions(PATHFINDER_USE_VULKAN)
include_directories(external/pathfinder-cpp/include)
add_subdirectory(external/pathfinder-cpp)

include_directories(third_party/harfbuzz-8.3.0/src)
add_subdirectory(third_party/harfbuzz-8.3.0)

include_directories(third_party/glm-0.9.9.8)
add_subdirectory(third_party/glm-0.9.9.8)

include_directories(third_party/GSL-4.0.0/include)
add_subdirectory(third_party/GSL-4.0.0)

include_directories(third_party/stduuid-1.2.3)

add_subdirectory(third_party/poly2tri)
add_subdirectory(third_party/zlib)

if (WIN32)
    # Fix "too many characters in constant" error when using CMake with MSVC.
    string(APPEND CMAKE_CXX_FLAGS " /utf-8")

    set(LINKING_LIBS glfw Vulkan::Vulkan pathfinder poly2tri harfbuzz icu zlib)
else ()
    # icu4c.
    find_package(ICU 70.0 COMPONENTS uc i18n REQUIRED)

    set(LINKING_LIBS glfw Vulkan::Vulkan pathfinder poly2tri harfbuzz ICU::uc ICU::i18n zlib)
endif ()

if (ON)
    # Alternative to icu4c. If the target platform comes with ICU, we just use it.
    # Else, we build fribidi. It's too cumbersome to build ICU ourselves.
    include_directories(third_party/fribidi)
    include_directories(third_party/fribidi/generated)
    #add_compile_definitions(DONT_HAVE_FRIBIDI_CONFIG_H)
    #add_compile_definitions(DONT_HAVE_FRIBIDI_UNICODE_VERSION_H)
    add_compile_definitions(HAVE_STRINGIZE=1)
    add_compile_definitions(FRIBIDI_BUILD)
endif ()

add_library(flint ${SOURCE_FILES})

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
