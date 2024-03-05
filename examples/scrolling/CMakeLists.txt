include_directories(../../src)

add_executable(scrolling ${SOURCE_FILES} main.cpp)

target_link_libraries(scrolling ${LINKING_LIBS})
