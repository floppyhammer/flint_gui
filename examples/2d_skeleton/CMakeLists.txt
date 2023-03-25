# Include Flint headers.
include_directories(../../src)

# Build executable.
add_executable(2d_skeleton ${SOURCE_FILES} main.cpp)

target_link_libraries(2d_skeleton  ${LINKING_LIBS})
