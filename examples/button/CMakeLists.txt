add_executable(button ${SOURCE_FILES} main.cpp)

target_include_directories(button PUBLIC "../../src")

target_link_libraries(button flint_gui)
