# Include Flint headers.
include_directories(../../src)

# Build executable.
add_executable(buttons ${SOURCE_FILES} main.cpp)

# Link libs.
target_link_libraries(buttons ${LINKING_LIBS})
