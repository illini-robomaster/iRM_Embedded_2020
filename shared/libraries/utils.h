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

#pragma once

/**
 * @brief clip a value to fall into a given range
 *
 * @tparam T    type of the value
 * @param value value to the clipped
 * @param min   range min
 * @param max   range max
 *
 * @return clipped value that falls in the range [min, max]
 *
 * @note undefined behavior if min > max
 */
template <typename T>
T clip(T value, T min, T max) {
  return value < min ? min : (value > max ? max : value);
}

/**
 * @brief wrap around a value to fall into a given range
 *
 * @tparam T    type of the value
 * @param value value to be wrapped around
 * @param min   range min
 * @param max   range max
 *
 * @return wrapped around value that falls in the range [min, max]
 *
 * @note undefined behavior if value is more than one cycle away from min or max
 */
template <typename T>
T wrap(T value, T min, T max) {
  const T range = max - min;
  return value < min ? value + range : (value > max ? value - range : value);
}
