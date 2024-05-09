add_executable(scrolling ${SOURCE_FILES} main.cpp)

target_include_directories(scrolling PUBLIC "../../src")

target_link_libraries(scrolling ${LINKING_LIBS})
