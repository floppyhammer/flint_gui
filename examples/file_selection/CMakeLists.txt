add_executable(file_selection ${SOURCE_FILES} main.cpp)

target_include_directories(file_selection PUBLIC "../../src")

target_link_libraries(file_selection ${LINKING_LIBS})
