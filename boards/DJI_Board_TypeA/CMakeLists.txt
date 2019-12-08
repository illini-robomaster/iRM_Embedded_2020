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

get_filename_component(CURRENT_DIR_NAME ${CMAKE_CURRENT_SOURCE_DIR} NAME)
project(${CURRENT_DIR_NAME} C ASM)

set(MCU_LINE STM32F427xx)
set(MCU_ARCH cortex-m4)
set(MCU_FLOAT_ABI hard)
set(MCU_FPU fpv4-sp-d16)
set(MCU_LINKER_SCRIPT STM32F427IIHx_FLASH.ld)

######################################################
# ----- create board specific interface library -----#
######################################################
add_library(${PROJECT_NAME}_interface INTERFACE)
target_compile_definitions(${PROJECT_NAME}_interface INTERFACE
    ${MCU_LINE} ARM_MATH_CM4
    USE_HAL_DRIVER
    USE_HAL_TIM_REGISTER_CALLBACKS
    USE_HAL_UART_REGISTER_CALLBACKS)
target_compile_options(${PROJECT_NAME}_interface INTERFACE
    $<$<COMPILE_LANGUAGE:ASM>:-x assembler-with-cpp>
    -mcpu=${MCU_ARCH} -mthumb -mthumb-interwork -mfpu=${MCU_FPU} -mfloat-abi=${MCU_FLOAT_ABI}
    -ffunction-sections -fdata-sections -fmessage-length=0 -specs=nano.specs -specs=nosys.specs)
target_link_options(${PROJECT_NAME}_interface INTERFACE
    -mcpu=${MCU_ARCH} -mthumb -mfpu=${MCU_FPU} -mfloat-abi=${MCU_FLOAT_ABI}
    -ffunction-sections -fdata-sections -fmessage-length=0 -specs=nano.specs -specs=nosys.specs
    -T${CMAKE_CURRENT_SOURCE_DIR}/${MCU_LINKER_SCRIPT} -Wl,--gc-sections)
target_include_directories(${PROJECT_NAME}_interface INTERFACE
    Core/Inc
    FATFS/Target
    FATFS/App
    USB_DEVICE/App
    USB_DEVICE/Target
    Drivers/STM32F4xx_HAL_Driver/Inc
    Drivers/STM32F4xx_HAL_Driver/Inc/Legacy
    Drivers/CMSIS/Include
    Drivers/CMSIS/Device/ST/STM32F4xx/Include
    DSP/Include
    Middlewares/ST/STM32_USB_Device_Library/Class/CDC/Inc
    Middlewares/ST/STM32_USB_Device_Library/Core/Inc
    Middlewares/Third_Party/FatFs/src
    Middlewares/Third_Party/FreeRTOS/Source/include
    Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS
    Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM4F)

#####################################################
# ----- create board specific arm dsp library ----- #
#####################################################

file(GLOB_RECURSE DSP_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/DSP/Source/*.c)
add_library(${PROJECT_NAME}_libDSP ${DSP_SOURCES})
target_compile_options(${PROJECT_NAME}_libDSP PRIVATE
    -D__FPU_PRESENT=1 -Wno-strict-aliasing)
target_link_libraries(${PROJECT_NAME}_libDSP PRIVATE ${PROJECT_NAME}_interface)

###################################################
# ----- create board specific driver library -----#
###################################################
file(GLOB_RECURSE CORE_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/Core/Src/*.c)
file(GLOB_RECURSE FATFS_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/FATFS/*.c)
file(GLOB_RECURSE USB_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/USB_DEVICE/*.c)
file(GLOB_RECURSE DRIVER_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/Drivers/*.c)
file(GLOB_RECURSE MIDDLEWARE_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/Middlewares/*.c)
file(GLOB_RECURSE STARTUP_ASM ${CMAKE_CURRENT_SOURCE_DIR}/startup_stm32f427xx.s)

add_library(${PROJECT_NAME} OBJECT
    ${CORE_SOURCES} ${FATFS_SOURCES} ${USB_SOURCES}
    ${DRIVER_SOURCES} ${MIDDLEWARE_SOURCES} ${STARTUP_ASM})
# ST libraries throws those warnings
target_compile_options(${PROJECT_NAME} PRIVATE
    -Wno-maybe-uninitialized -Wno-unused-parameter -Wno-sign-compare)
# link against board specific arm dsp library and interface
target_link_libraries(${PROJECT_NAME}
    PRIVATE ${PROJECT_NAME}_libDSP
    PUBLIC ${PROJECT_NAME}_interface)
