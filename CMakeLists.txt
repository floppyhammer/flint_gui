cmake_minimum_required(VERSION 3.16)

project(flint_gui
        VERSION 0.1
        LANGUAGES C CXX)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/bin")

# Identify Linux.
if (UNIX AND NOT APPLE AND NOT ANDROID)
    set(LINUX ON)
endif ()

option(FLINT_GUI_VULKAN "Use Vulkan instead of OpenGL" OFF)
option(FLINT_GUI_VISUAL_DEBUG "Enable visual debug" OFF)
option(FLINT_GUI_FRIBIDI "Use fribidi instead of icu" OFF)

if (APPLE)
    set(FLINT_GUI_FRIBIDI ON)
endif ()

file(GLOB_RECURSE FLINT_GUI_SOURCE_FILES CONFIGURE_DEPENDS
        src/*.cpp
        third_party/tinyfiledialogs/tinyfiledialogs.c)

if (FLINT_GUI_FRIBIDI)
    file(GLOB FLINT_GUI_SOURCE_FILES CONFIGURE_DEPENDS ${FLINT_GUI_SOURCE_FILES} third_party/fribidi/*.c)
endif ()

add_library(flint_gui STATIC ${FLINT_GUI_SOURCE_FILES})

# Has to be 17 or above.
target_compile_features(flint_gui PUBLIC cxx_std_17)

# Include external and third-party headers.
target_include_directories(flint_gui PUBLIC "third_party")

if (FLINT_GUI_VULKAN)
    set(PATHFINDER_VULKAN_SUPPORT ON CACHE BOOL "" FORCE)
else ()
    set(PATHFINDER_VULKAN_SUPPORT OFF CACHE BOOL "" FORCE)
endif ()
target_include_directories(flint_gui PUBLIC "third_party/pathfinder-cpp")
add_subdirectory("third_party/pathfinder-cpp")

target_include_directories(flint_gui PUBLIC "third_party/harfbuzz-8.4.0/src")
add_subdirectory("third_party/harfbuzz-8.4.0")

target_include_directories(flint_gui PUBLIC "third_party/GSL-4.0.0/include")
add_subdirectory("third_party/GSL-4.0.0")

target_include_directories(flint_gui PUBLIC "third_party/stduuid-1.2.3")

add_subdirectory("third_party/poly2tri")
add_subdirectory("third_party/zlib")

target_link_libraries(flint_gui PUBLIC pathfinder poly2tri harfbuzz zlibstatic)

if (NOT FLINT_GUI_FRIBIDI)
    if (WIN32)
        # Fix "too many characters in constant" error when using CMake with MSVC.
        string(APPEND CMAKE_CXX_FLAGS " /utf-8")

        target_link_libraries(flint_gui PUBLIC icu)
    elseif (LINUX)
        # icu4c.
        find_package(ICU 70.0 COMPONENTS uc i18n REQUIRED)

        target_link_libraries(flint_gui PUBLIC ICU::uc ICU::i18n)
    endif ()
endif ()

if (FLINT_GUI_FRIBIDI)
    # Alternative to icu4c. If the target platform comes with ICU, we just use it.
    # Else, we build fribidi. It's too cumbersome to build ICU ourselves.
    target_include_directories(flint_gui PUBLIC "third_party/fribidi")
    target_include_directories(flint_gui PUBLIC "third_party/fribidi/generated")
    #add_compile_definitions(DONT_HAVE_FRIBIDI_CONFIG_H)
    #add_compile_definitions(DONT_HAVE_FRIBIDI_UNICODE_VERSION_H)
    target_compile_definitions(flint_gui PUBLIC HAVE_STRINGIZE=1 HAVE_STDLIB_H)
    target_compile_definitions(flint_gui PUBLIC FRIBIDI_BUILD)

    target_compile_definitions(flint_gui PUBLIC FLINT_USE_FRIBIDI)

    if (APPLE)
        add_compile_definitions(HAVE_STRINGS_H)
    endif ()
endif ()

if (FLINT_GUI_VISUAL_DEBUG)
    add_compile_definitions(FLINT_GUI_VISUAL_DEBUG)
endif ()

# Copy the assets to the binary directory.
file(COPY "assets" DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})

# Add examples.
add_subdirectory(examples/file_selection)
add_subdirectory(examples/container_layout)
add_subdirectory(examples/texture_rect)
add_subdirectory(examples/scrolling)
add_subdirectory(examples/tab)
add_subdirectory(examples/progress_bar)
add_subdirectory(examples/i18n)
add_subdirectory(examples/label_alignment)
add_subdirectory(examples/multiple_window)
add_subdirectory(examples/button)
add_subdirectory(examples/tree)
add_subdirectory(examples/popup_menu)
add_subdirectory(examples/collapse_containers)
