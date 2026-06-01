#include "crc_utils.h"

#define CHUNK_SIZE 256

/* External functions implemented elsewhere */
extern uint8_t SerialFlash_ReadByte(uint32_t addr);

extern uint16_t g_CRC_calc_continue(uint8_t *buffer,
uint32_t size,
uint16_t crc);

uint16_t calculate_flash_crc(uint32_t start_addr,
uint32_t total_size)
{
uint8_t buffer[CHUNK_SIZE];
uint16_t crc = 0xFFFF;

uint32_t remaining = total_size;
uint32_t addr = start_addr;

while (remaining > 0)
{
    uint32_t chunk =
        (remaining > CHUNK_SIZE) ?
        CHUNK_SIZE :
        remaining;

    for (uint32_t i = 0; i < chunk; i++)
    {
        buffer[i] =
            SerialFlash_ReadByte(addr + i);
    }

    crc = g_CRC_calc_continue(buffer,
                              chunk,
                              crc);

    addr += chunk;
    remaining -= chunk;
}

return crc;


}
