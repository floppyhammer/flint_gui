include_directories(../../src)

add_executable(texture_rect ${SOURCE_FILES} main.cpp)

target_link_libraries(texture_rect ${LINKING_LIBS})
