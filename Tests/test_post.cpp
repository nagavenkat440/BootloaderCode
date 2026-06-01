#include <gtest/gtest.h>

extern "C"
{
#include "crc_utils.h"
}

/* Mock Serial Flash Read */
extern "C" uint8_t SerialFlash_ReadByte(uint32_t addr)
{
return (uint8_t)(addr & 0xFF);
}

/* Mock CRC */
extern "C" uint16_t g_CRC_calc_continue(uint8_t *buffer,
uint32_t size,
uint16_t crc)
{
uint16_t result = crc;


for (uint32_t i = 0; i < size; i++)
{
    result += buffer[i];
}

return result;


}

TEST(PostCRC, CalculateFlashCRC)
{
uint16_t crc =
calculate_flash_crc(0, 16);


EXPECT_GT(crc, 0);


}
