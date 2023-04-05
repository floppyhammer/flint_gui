# Include Flint headers.
include_directories(../../src)

# Build executable.
add_executable(ecs ${SOURCE_FILES} main.cpp)

# Link libs.
target_link_libraries(ecs ${LINKING_LIBS})
