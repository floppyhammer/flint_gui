include_directories(../../src)

add_executable(i18n ${SOURCE_FILES} main.cpp)

target_link_libraries(i18n ${LINKING_LIBS})
