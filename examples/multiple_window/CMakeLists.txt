add_executable(multiple_window ${SOURCE_FILES} main.cpp)

target_include_directories(multiple_window PUBLIC "../../src")

target_link_libraries(multiple_window flint_gui)
