# Include Flint headers.
include_directories(../../src)

# Build executable.
add_executable(texture_rect ${SOURCE_FILES} main.cpp)

# Link libs.
target_link_libraries(texture_rect ${LINKING_LIBS})
