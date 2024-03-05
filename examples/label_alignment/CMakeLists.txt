include_directories(../../src)

add_executable(label_alignment ${SOURCE_FILES} main.cpp)

target_link_libraries(label_alignment ${LINKING_LIBS})
