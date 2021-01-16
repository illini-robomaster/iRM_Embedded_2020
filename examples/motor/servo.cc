/****************************************************************************
 *                                                                          *
 *  Copyright (C) 2020 RoboMaster.                                          *
 *  Illini RoboMaster @ University of Illinois at Urbana-Champaign          *
 *                                                                          *
 *  This program is free software: you can redistribute it and/or modify    *
 *  it under the terms of the GNU General Public License as published by    *
 *  the Free Software Foundation, either version 3 of the License, or       *
 *  (at your option) any later version.                                     *
 *                                                                          *
 *  This program is distributed in the hope that it will be useful,         *
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of          *
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the           *
 *  GNU General Public License for more details.                            *
 *                                                                          *
 *  You should have received a copy of the GNU General Public License       *
 *  along with this program. If not, see <http://www.gnu.org/licenses/>.    *
 *                                                                          *
 ****************************************************************************/

#include "cmsis_os.h"
#include "motor.h"

control::MotorPWMBase *motor1 = nullptr;

void RM_RTOS_Init() {
    motor1 = new control::MotorPWMBase(&htim4, 1, 1000000, 50, 0);
}

void RM_RTOS_Default_Task(const void *args) {
    UNUSED(args);
    while(1){
        motor1->SetOutput(1500);
        osDelay(1000);
        motor1->SetOutput(1000);
        osDelay(1000);
        //there're generally 3 positions: left,mid and right, which corresponds to 1000,1500 and 2000;
    }
}
