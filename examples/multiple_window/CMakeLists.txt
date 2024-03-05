include_directories(../../src)

add_executable(multiple_window ${SOURCE_FILES} main.cpp)

target_link_libraries(multiple_window ${LINKING_LIBS})
