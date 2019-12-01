function(irm_add_arm_executable name)
    cmake_parse_arguments(ARG "" "TARGET" "INCLUDES;DEPENDS;SOURCES" ${ARGN})
    set(HEX_FILE ${CMAKE_CURRENT_BINARY_DIR}/${name}.hex)
    set(BIN_FILE ${CMAKE_CURRENT_BINARY_DIR}/${name}.bin)
    set(MAP_FILE ${CMAKE_CURRENT_BINARY_DIR}/${name}.map)

    add_executable(${name}.elf ${ARG_SOURCES})
    target_link_libraries(${name}.elf PRIVATE ${ARG_DEPENDS} ${ARG_TARGET})
    target_include_directories(${name}.elf PRIVATE ${ARG_INCLUDES})
    target_link_options(${name}.elf PRIVATE -Wl,-Map=${MAP_FILE})

    add_custom_command(TARGET ${name}.elf POST_BUILD
        COMMAND arm-none-eabi-size ${name}.elf
        COMMAND arm-none-eabi-objcopy -Oihex $<TARGET_FILE:${name}.elf> ${HEX_FILE}
        COMMAND arm-none-eabi-objcopy -Obinary $<TARGET_FILE:${name}.elf> ${BIN_FILE}
        COMMENT "Building ${HEX_FILE}\nBuilding ${BIN_FILE}")

    add_custom_target(flash-${name}
        COMMAND st-flash write ${BIN_FILE} 0x8000000
        DEPENDS ${name}.elf)

    if (CMAKE_BUILD_TYPE STREQUAL "Debug")
        add_custom_target(debug-${name}
            COMMAND arm-none-eabi-gdb $<TARGET_FILE:${name}.elf>
            DEPENDS ${name}.elf)
    endif()
endfunction(irm_add_arm_executable)

