add_executable(i18n ${SOURCE_FILES} main.cpp)

target_include_directories(i18n PUBLIC "../../src")

target_link_libraries(i18n ${LINKING_LIBS})
