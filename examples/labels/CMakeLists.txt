# Include Flint headers.
include_directories(../../src)

# Build executable.
add_executable(labels ${SOURCE_FILES} main.cpp)

# Link libs.
target_link_libraries(labels ${LINKING_LIBS})
