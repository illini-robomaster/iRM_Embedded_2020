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

#include "main.h"
#include "tim.h"

#include "bsp_buzzer.h"

using Note = bsp::BuzzerNote;

static bsp::BuzzerNoteDelayed startup[] = {
  { Note::Mi3M,   80 },
  { Note::Silent, 80 }, 
  { Note::Mi3M,   80 },
  { Note::Silent, 240 },
  { Note::Mi3M,   80 },
  { Note::Silent, 240 },
  { Note::Do1M,   80 },
  { Note::Silent, 80 },
  { Note::Mi3M,   80 },
  { Note::Silent, 240 },
  { Note::So5M,   80 },
  { Note::Silent, 560 },
  { Note::So5L,   80  },
  { Note::Silent, 560 },
  { Note::Finish, 0 },
};

void RM_RTOS_Init(void) {
  bsp::Buzzer buzzer(&htim12, 1, 1000000);
  buzzer.SingSong(startup);
}
