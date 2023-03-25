# Include Flint headers.
include_directories(../../src)

# Build executable.
add_executable(multiple_windows ${SOURCE_FILES} main.cpp)

# Link libs.
target_link_libraries(multiple_windows ${LINKING_LIBS})
