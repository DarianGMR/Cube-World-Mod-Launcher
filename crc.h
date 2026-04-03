#ifndef CRC_H
#define CRC_H

#include <cstdint>

uint32_t crc32_buf(const char* buf, unsigned long len);
uint32_t crc32_file(const char* fileName);

#endif // CRC_H