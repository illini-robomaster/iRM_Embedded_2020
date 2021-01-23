# ---------------------------------------------------------------------- #
#                                                                        #
#  Copyright (C) 2021                                                    #
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

project(example_eigen ASM C CXX)

irm_add_arm_executable(${PROJECT_NAME}_f4
    TARGET DJI_Board_TypeA
    SOURCES main.cc
    DEPENDS eigen3)

irm_add_arm_executable(${PROJECT_NAME}_f1
    TARGET F103C8
    SOURCES f103.cc
    DEPENDS eigen3)
target_compile_definitions(${PROJECT_NAME}_f1.elf PRIVATE UART_HANDLE=huart1)
