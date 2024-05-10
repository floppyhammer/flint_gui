add_executable(popup_menu ${SOURCE_FILES} main.cpp)

target_include_directories(popup_menu PUBLIC "../../src")

target_link_libraries(popup_menu flint_gui)
