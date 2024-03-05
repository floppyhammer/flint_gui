include_directories(../../src)

add_executable(tab ${SOURCE_FILES} main.cpp)

target_link_libraries(tab ${LINKING_LIBS})
