add_executable(tab ${SOURCE_FILES} main.cpp)

target_include_directories(tab PUBLIC "../../src")

target_link_libraries(tab ${LINKING_LIBS})
