add_subdirectory(third_party)
add_subdirectory(bsp)

add_library(rm_shared INTERFACE)
target_link_libraries(rm_shared INTERFACE third_party bsp)

