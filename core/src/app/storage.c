#include "storage.h"
#include "stm32f4xx_hal.h"
#include <string.h>

#define STORAGE_SECTOR   FLASH_SECTOR_5   /* 0x08020000, 128 KB */
#define STORAGE_ADDR     0x08020000UL

uint32_t crc32(const uint8_t *data, uint32_t len) {
    uint32_t crc = 0xFFFFFFFFu;
    for (uint32_t i = 0; i < len; i++) {
        crc ^= data[i];
        for (int j = 0; j < 8; j++) {
            crc = (crc >> 1) ^ (0xEDB88320u & (0u - (crc & 1u)));
        }
    }
    return crc ^ 0xFFFFFFFFu;
}

int storage_load(storage_cfg_t *out) {
    const storage_cfg_t *p = (const storage_cfg_t *)STORAGE_ADDR;
    if (p->magic != STORAGE_MAGIC) return 0;
    uint32_t c = crc32((const uint8_t *)p + 8, sizeof(storage_cfg_t) - 8);
    if (c != p->crc) return 0;
    memcpy(out, p, sizeof(*out));
    return 1;
}

static int erase_sector(void) {
    FLASH_EraseInitTypeDef e = {0};
    uint32_t sector_error = 0;
    e.TypeErase = FLASH_TYPEERASE_SECTORS;
    e.Sector = STORAGE_SECTOR;
    e.NbSectors = 1;
    e.VoltageRange = FLASH_VOLTAGE_RANGE_3;
    HAL_FLASH_Unlock();
    HAL_StatusTypeDef s = HAL_FLASHEx_Erase(&e, &sector_error);
    HAL_FLASH_Lock();
    return (s == HAL_OK) ? 0 : -1;
}

int storage_save(const storage_cfg_t *cfg) {
    storage_cfg_t tmp = *cfg;
    tmp.magic = STORAGE_MAGIC;
    tmp.crc = crc32((const uint8_t *)&tmp + 8, sizeof(tmp) - 8);

    if (erase_sector() != 0) return -1;

    HAL_FLASH_Unlock();
    const uint32_t *src = (const uint32_t *)&tmp;
    uint32_t nwords = (sizeof(tmp) + 3) / 4;
    for (uint32_t i = 0; i < nwords; i++) {
        if (HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, STORAGE_ADDR + i * 4,
                              (uint64_t)src[i]) != HAL_OK) {
            HAL_FLASH_Lock();
            return -2;
        }
    }
    HAL_FLASH_Lock();
    return 0;
}

int storage_erase(void) {
    return erase_sector();
}
