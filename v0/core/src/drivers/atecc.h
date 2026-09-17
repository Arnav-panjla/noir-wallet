#ifndef ATECC_H
#define ATECC_H

#include "stm32f4xx_hal.h"
#include <stdint.h>

/* ATECC608B over I2C, shared SDA/SCL with the OLED. */

/* CRC-16 (poly 0x8005, init 0x0000, no reflection) used on all packets. */
uint16_t atecc_crc16(uint16_t crc, const uint8_t *data, uint32_t len);

/* Wakes a sleeping ATECC608B: drives SDA low for >= 60us (tWHI). */
void atecc_wake(I2C_HandleTypeDef *hi2c);

/* Reads the 4-byte revision (Info mode 0x00). addr7 = 7-bit I2C address.
 * Returns 0 on success. */
int atecc_read_revision(I2C_HandleTypeDef *hi2c, uint8_t addr7, uint8_t rev[4]);

/* Reads the 9-byte serial number (Info mode 0x80). Returns 0 on success. */
int atecc_read_serial(I2C_HandleTypeDef *hi2c, uint8_t addr7, uint8_t serial[9]);

/* ATECC608B hardware TRNG: 32 certified-random bytes (Random opcode 0x1B).
 * Returns 0 on success. */
int atecc_random(I2C_HandleTypeDef *hi2c, uint8_t addr7, uint8_t out[32]);

/* SelfTest (0x77): conditions the DRBG/TRNG. Non-fatal; 0 = status OK. */
int atecc_selftest(I2C_HandleTypeDef *hi2c, uint8_t addr7);

/* Canonical wake: SDA pulse + 0x00 wake token + read 4-byte wake response.
 * Returns 0 if the device replies with 0x04 0x11 0x33 0x43. */
int atecc_wake_token(I2C_HandleTypeDef *hi2c, uint8_t addr7);

#endif
