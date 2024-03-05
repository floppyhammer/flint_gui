include_directories(../../src)

add_executable(container_layout ${SOURCE_FILES} main.cpp)

target_link_libraries(container_layout ${LINKING_LIBS})
