add_executable(tree ${SOURCE_FILES} main.cpp)

target_include_directories(tree PUBLIC "../../src")

target_link_libraries(tree revector)
