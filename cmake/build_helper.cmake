# ---------------------------------------------------------------------- #
#                                                                        #
#  Copyright (C) 2020                                                    #
#  Illini RoboMaster @ University of Illinois at Urbana-Champaign.       #
#                                                                        #
#  This program is free software: you can redistribute it and/or modify  #
#  it under the terms of the GNU General Public License as published by  #
#  the Free Software Foundation, either version 3 of the License, or     #
#  (at your option) any later version.                                   #
#                                                                        #
#  This program is distributed in the hope that it will be useful,       #
#  but WITHOUT ANY WARRANTY; without even the implied warranty of        #
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         #
#  GNU General Public License for more details.                          #
#                                                                        #
#  You should have received a copy of the GNU General Public License     #
#  along with this program. If not, see <http://www.gnu.org/licenses/>.  #
#                                                                        #
# ---------------------------------------------------------------------- #

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

