# Include Flint headers.
include_directories(../../src)

# Build executable.
add_executable(tabs ${SOURCE_FILES} main.cpp)

# Link libs.
target_link_libraries(tabs ${LINKING_LIBS})
