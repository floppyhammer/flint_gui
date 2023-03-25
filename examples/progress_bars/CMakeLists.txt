# Include Flint headers.
include_directories(../../src)

# Build executable.
add_executable(progress_bars ${SOURCE_FILES} main.cpp)

# Link libs.
target_link_libraries(progress_bars ${LINKING_LIBS})
