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

# use clang-format to enforce coding styles
find_program(CLANG_FORMAT_EXE NAMES 
    clang-format-10
    clang-format-9
    clang-format-8
    clang-format)

# gather all source code
file(GLOB_RECURSE ALL_SOURCE_FILES
    ${CMAKE_SOURCE_DIR}/*.c
    ${CMAKE_SOURCE_DIR}/*.cc
    ${CMAKE_SOURCE_DIR}/*.cpp
    ${CMAKE_SOURCE_DIR}/*.h
    ${CMAKE_SOURCE_DIR}/*.hpp
)
# exclude build folder and board CubeMX generated code
list(FILTER ALL_SOURCE_FILES EXCLUDE REGEX .*/.*build.*/.*)
list(FILTER ALL_SOURCE_FILES EXCLUDE REGEX .*/boards/.*)

# create formatting helper targets
if (CLANG_FORMAT_EXE)
    # download a third party clang format python wrapper
    set(RUN_CLANG_FORMAT ${CMAKE_BINARY_DIR}/run-clang-format.py)
    file(DOWNLOAD https://raw.githubusercontent.com/Sarcasm/run-clang-format/master/run-clang-format.py
         ${RUN_CLANG_FORMAT})

    # format code in place
    add_custom_target(format
        COMMAND python3 ${RUN_CLANG_FORMAT} --clang-format-executable ${CLANG_FORMAT_EXE} -i ${ALL_SOURCE_FILES}
        DEPENDS ${RUN_CLANG_FORMAT} 
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})

    # check for format violations
    add_custom_target(check-format
        COMMAND python3 ${RUN_CLANG_FORMAT} --clang-format-executable ${CLANG_FORMAT_EXE} ${ALL_SOURCE_FILES}
        DEPENDS ${RUN_CLANG_FORMAT} 
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})
endif ()
