include_directories(../../src)

add_executable(file_selection ${SOURCE_FILES} main.cpp)

target_link_libraries(file_selection ${LINKING_LIBS})
