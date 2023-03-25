# Include Flint headers.
include_directories(../../src)

# Build executable.
add_executable(i18n ${SOURCE_FILES} main.cpp)

# Link libs.
target_link_libraries(i18n ${LINKING_LIBS})
