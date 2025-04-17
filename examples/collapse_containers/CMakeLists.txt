add_executable(collapse_containers ${SOURCE_FILES} main.cpp)

target_include_directories(collapse_containers PUBLIC "../../src")

target_link_libraries(collapse_containers revector)
