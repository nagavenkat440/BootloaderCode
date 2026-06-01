#ifndef CRC_UTILS_H
#define CRC_UTILS_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

uint16_t calculate_flash_crc(uint32_t start_addr,
uint32_t total_size);

#ifdef __cplusplus
}
#endif

#endif
