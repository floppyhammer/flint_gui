add_executable(label_alignment ${SOURCE_FILES} main.cpp)

target_include_directories(label_alignment PUBLIC "../../src")

target_link_libraries(label_alignment revector)
