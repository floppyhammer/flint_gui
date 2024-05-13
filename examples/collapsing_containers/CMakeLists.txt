add_executable(collapsing_containers ${SOURCE_FILES} main.cpp)

target_include_directories(collapsing_containers PUBLIC "../../src")

target_link_libraries(collapsing_containers flint_gui)
