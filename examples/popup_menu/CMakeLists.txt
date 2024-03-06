include_directories(../../src)

add_executable(popup_menu ${SOURCE_FILES} main.cpp)

target_link_libraries(popup_menu ${LINKING_LIBS})
