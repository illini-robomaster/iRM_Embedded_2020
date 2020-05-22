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

project(Walle ASM C CXX)

irm_add_board_specific_library(Walle-utils
    TARGET DJI_Board_TypeA
    SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/walle_utils.cc
    INCLUDES ${CMAKE_CURRENT_SOURCE_DIR}
    DEPENDS DJI_Board_TypeA_irm)

irm_add_arm_executable(test_servo
    TARGET DJI_Board_TypeA
    SOURCES test_servo.cc
    DEPENDS Walle-utils)

irm_add_arm_executable(test_eye
    TARGET DJI_Board_TypeA
    SOURCES test_eye.cc
    DEPENDS Walle-utils)

irm_add_arm_executable(walle
    TARGET DJI_Board_TypeA
    SOURCES walle.cc
    DEPENDS Walle-utils)
