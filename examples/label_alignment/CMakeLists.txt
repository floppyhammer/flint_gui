# Include Flint headers.
include_directories(../../src)

# Build executable.
add_executable(label_alignment ${SOURCE_FILES} main.cpp)

# Link libs.
target_link_libraries(label_alignment ${LINKING_LIBS})
