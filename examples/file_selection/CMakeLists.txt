# Include Flint headers.
include_directories(../../src)

# Build executable.
add_executable(file_selection ${SOURCE_FILES} main.cpp)

# Link libs.
target_link_libraries(file_selection ${LINKING_LIBS})
