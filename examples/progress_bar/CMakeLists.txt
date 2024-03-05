include_directories(../../src)

add_executable(progress_bar ${SOURCE_FILES} main.cpp)

target_link_libraries(progress_bar ${LINKING_LIBS})
