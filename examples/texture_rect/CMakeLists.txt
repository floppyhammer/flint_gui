add_executable(texture_rect ${SOURCE_FILES} main.cpp)

target_include_directories(texture_rect PUBLIC "../../src")

target_link_libraries(texture_rect revector)
