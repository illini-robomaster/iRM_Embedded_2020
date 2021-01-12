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

#include <stdint.h>

#include "crc.h"

namespace communication {

typedef struct {
  uint8_t   sof;
  uint16_t  data_length;
  uint8_t   sequence_id;
} __attribute__((packed)) header_t;

typedef uint8_t crc8_t;
typedef uint16_t crc16_t;

template <typename T>
class Message {
 public:
  Message(uint8_t sof, uint16_t command_id) : sof_(sof), command_id_(command_id) {}

  uint32_t GetPacketSize() const {
    return sizeof(header_t) + sizeof(crc8_t) + sizeof(command_id_) + 
           sizeof(T) + sizeof(crc16_t);
  }

  uint32_t Encode(uint8_t buffer[]) const {
    const uint32_t packet_size = GetPacketSize();
    // encode header
    header_t *header = GetHeader(buffer);
    header->sof = sof_;
    header->data_length = sizeof(T);
    header->sequence_id = 0; // TODO(alvin): implement this in the future
    // header crc8
    append_crc8(buffer, sizeof(header_t));
    // encode command id
    uint16_t *command_id = GetCommandID(buffer);
    *command_id = command_id_;
    // encode data
    uint8_t *data_start = GetDataStart();
    memcpy(data_start, this->data, sizeof(T));
    // whole frame crc16
    append_crc16(buffer, GetPacketSize() - sizeof(crc16_t));
    return packet_size;
  }

  T* Decode(uint8_t buffer[]) {
    header_t *header = reinterpret_cast<header_t*>(buffer);
    // validate SOF
    if (header->sof != sof_)
      return nullptr;
    // validate header integrity
    if (!check_crc8(header, sizeof(header_t) + sizeof(crc8_t)))
      return nullptr;
    // validate command id;
    if (*(GetCommandID(buffer)) != command_id_)
      return nullptr;
    // validate frame integrity
    if (!check_crc16(buffer, GetPacketSize()))
      return nullptr;
    // extract data frame
    memcpy(this->data, GetDataStart(buffer));

    return &data;
  }

  T data;

 private:
  inline header_t* GetHeader(uint8_t buffer[]) const {
    return reinterpret_cast<header_t*>(buffer);
  }

  inline uint16_t* GetCommandID(uint8_t buffer[]) const {
    return reinterpret_cast<uint16_t*>(buffer + sizeof(header_t) + sizeof(crc8_t));
  }

  inline uint8_t* GetDataStart(uint8_t buffer[]) const {
    return reinterpret_cast<uint8_t*>(
        buffer + sizeof(header_t) + sizeof(crc8_t) + sizeof(command_id_));
  }

  uint8_t sof_;
  uint16_t command_id_;
};

} /* namespace communication */
