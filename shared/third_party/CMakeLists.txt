project(third_party C)

file(GLOB SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/*.c)

add_library(${PROJECT_NAME} INTERFACE)
target_sources(${PROJECT_NAME} INTERFACE ${SOURCES})
target_include_directories(${PROJECT_NAME} INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})
