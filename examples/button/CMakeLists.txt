include_directories(../../src)

add_executable(button ${SOURCE_FILES} main.cpp)

target_link_libraries(button ${LINKING_LIBS})
