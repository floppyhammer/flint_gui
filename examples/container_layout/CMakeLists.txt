# Include Flint headers.
include_directories(../../src)

# Build executable.
add_executable(container_layout ${SOURCE_FILES} main.cpp)

# Link libs.
target_link_libraries(container_layout ${LINKING_LIBS})
