#pragma once

#include <inttypes.h>

namespace communication {

#define SIZEOF_CRC8   1
#define SIZEOF_CRC16  2

/**
 * @brief calculate crc8 given data stream
 *
 * @param ptr     start address of the data stream
 * @param length  length of data [in bytes]
 *
 * @return calculated crc8 value
 */
uint8_t get_crc8(void *ptr, uint16_t length);

/**
 * @brief calculate crc16 given data stream
 *
 * @param ptr     start address of the data stream
 * @param length  length of data [in bytes]
 *
 * @return calculated crc16 value
 */
uint16_t get_crc16(void *ptr, uint16_t length);

/**
 * @brief validate data stream by checking the 1-byte crc8 value at the end
 *
 * @param ptr     start address of the data stream
 * @param length  length of data [in bytes] (including the last crc8 byte)
 *
 * @return true if valid, otherwise false
 */
bool check_crc8(void *ptr, uint16_t length);

/**
 * @brief validate data stream by checking the 2-byte crc16 value at the end
 *
 * @param ptr     start address of the data stream
 * @param length  length of data [in bytes] (including the last-2 crc16 bytes)
 *
 * @return true if valid, otherwise false
 */
bool check_crc16(void *ptr, uint16_t length);

/**
 * @brief append a crc8 checksum to the end of a data stream
 *
 * @param ptr     start address of the data stream
 * @param length  length of data [in bytes] (does not include checksum bytes)
 */
void append_crc8(void *ptr, uint16_t length);

/**
 * @brief append a crc16 (2-byte) checksum to the end of a data stream
 *
 * @param ptr     start address of the data stream
 * @param length  length of data [in bytes] (does not include checksum bytes)
 */
void append_crc16(void *ptr, uint16_t length);

} /* namespace communication */
