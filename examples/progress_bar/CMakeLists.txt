add_executable(progress_bar ${SOURCE_FILES} main.cpp)

target_include_directories(progress_bar PUBLIC "../../src")

target_link_libraries(progress_bar revector)
