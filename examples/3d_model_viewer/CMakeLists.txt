# Include Flint headers.
include_directories(../../src)

# Build executable.
add_executable(3d_model_viewer ${SOURCE_FILES} main.cpp)

# Link libs.
target_link_libraries(3d_model_viewer ${LINKING_LIBS})
