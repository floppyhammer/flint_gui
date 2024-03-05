include_directories(../../src)

add_executable(tree ${SOURCE_FILES} main.cpp)

target_link_libraries(tree ${LINKING_LIBS})
