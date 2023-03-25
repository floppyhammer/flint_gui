# Include Flint headers.
include_directories(../../src)

# Build executable.
add_executable(scrolling ${SOURCE_FILES} main.cpp)

# Link libs.
target_link_libraries(scrolling ${LINKING_LIBS})
