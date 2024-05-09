add_executable(container_layout ${SOURCE_FILES} main.cpp)

target_include_directories(container_layout PUBLIC "../../src")

target_link_libraries(container_layout ${LINKING_LIBS})
