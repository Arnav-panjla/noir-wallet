#include "atecc.h"
#include "gpio_map.h"
#include <string.h>

/* Microchip CryptoAuthentication CRC-16 (poly 0x8005, init 0x0000).
 * This is NOT the standard CRC-16/IBM or CRC-16/UMTS: each byte is processed
 * LSB-first and compared against the MSB (bit 15) of the register. Verified
 * against the canonical wake response: crc([04 11]) == 0x4333 (bytes 33 43). */
uint16_t atecc_crc16(uint16_t crc, const uint8_t *data, uint32_t len) {
    for (uint32_t counter = 0; counter < len; counter++) {
        for (uint8_t shift = 0x01; shift > 0x00; shift <<= 1) {
            uint8_t data_bit = (data[counter] & shift) ? 1 : 0;
            uint8_t crc_bit  = (uint8_t)(crc >> 15);
            crc <<= 1;
            if (data_bit != crc_bit) crc ^= 0x8005;
        }
    }
    return crc;
}

void atecc_wake(I2C_HandleTypeDef *hi2c) {
    (void)hi2c;
    GPIO_InitTypeDef g = {0};

    /* Drive SDA low for >= 60us (tWHI) to wake a sleeping ATECC608B. */
    g.Pin = OLED_SDA_PIN;
    g.Mode = GPIO_MODE_OUTPUT_OD;
    g.Pull = GPIO_PULLUP;
    g.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(OLED_SDA_PORT, &g);
    HAL_GPIO_WritePin(OLED_SDA_PORT, OLED_SDA_PIN, GPIO_PIN_RESET);
    HAL_Delay(2);
    HAL_GPIO_WritePin(OLED_SDA_PORT, OLED_SDA_PIN, GPIO_PIN_SET);

    /* Put SDA back to its I2C alternate function (open-drain). */
    g.Pin = OLED_SDA_PIN;
    g.Mode = GPIO_MODE_AF_OD;
    g.Pull = GPIO_PULLUP;
    g.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    g.Alternate = OLED_I2C_AF;
    HAL_GPIO_Init(OLED_SDA_PORT, &g);
    HAL_Delay(2);
}

/* Sends a generic command with one-byte param1 and two-byte param2, returns
 * the response data payload.
 * Packet (after I2C word address 0x03):
 *   [count][opcode][param1][param2_lo][param2_hi][crc_lo][crc_hi]
 * Response: [count][data...][crc_lo][crc_hi]. */
static int atecc_cmd(I2C_HandleTypeDef *hi2c, uint8_t addr7, uint8_t opcode,
                     uint8_t p1, uint16_t p2, uint8_t *data, uint8_t data_len) {
    uint8_t cmd[7];
    /* count INCLUDES the count byte: count(1)+opcode(1)+param1(1)+param2(2)+crc(2)
     * = 7. (Same convention as the wake response 0x04 0x11 0x33 0x43, count=4.) */
    cmd[0] = 0x07;
    cmd[1] = opcode;
    cmd[2] = p1;
    cmd[3] = (uint8_t)(p2 & 0xFF);
    cmd[4] = (uint8_t)(p2 >> 8);
    uint16_t crc = atecc_crc16(0, cmd, 5);
    cmd[5] = (uint8_t)(crc & 0xFF);
    cmd[6] = (uint8_t)(crc >> 8);

    uint16_t dev = (uint16_t)(addr7 << 1);
    uint8_t tx[8], count, resp[40];
    tx[0] = 0x03;   /* I2C word address: command */
    memcpy(&tx[1], cmd, 7);

    /* Wake once, send once, then POLL for the response. The device NACKs its
     * I2C address while a command is still executing (e.g. the TRNG), so we
     * must retry the READ — never re-wake + re-send mid-command. */
    atecc_wake(hi2c);
    if (HAL_I2C_Master_Transmit(hi2c, dev, tx, 8, 100) != HAL_OK) return -1;

    for (int attempt = 0; attempt < 25; attempt++) {
        if (HAL_I2C_Master_Receive(hi2c, dev, &count, 1, 100) != HAL_OK) { HAL_Delay(1); continue; }
        if (count < 3 || count > 40) { HAL_Delay(1); continue; }
        if (HAL_I2C_Master_Receive(hi2c, dev, resp, count - 1, 100) != HAL_OK) return -1;

        /* resp = data(count-3 bytes) followed by crc(2). */
        uint8_t verify[40];
        verify[0] = count;
        memcpy(&verify[1], resp, (uint32_t)(count - 3));
        uint16_t rxcrc = (uint16_t)resp[count - 3] | ((uint16_t)resp[count - 2] << 8);
        if (atecc_crc16(0, verify, (uint32_t)1 + (count - 3)) != rxcrc) return -2;
        if ((count - 3) != data_len) return -3;

        memcpy(data, resp, data_len);
        return 0;
    }
    return -1;   /* no response within ~25ms */
}

int atecc_read_revision(I2C_HandleTypeDef *hi2c, uint8_t addr7, uint8_t rev[4]) {
    return atecc_cmd(hi2c, addr7, 0x30, 0x00, 0x0000, rev, 4);
}

int atecc_read_serial(I2C_HandleTypeDef *hi2c, uint8_t addr7, uint8_t serial[9]) {
    return atecc_cmd(hi2c, addr7, 0x30, 0x80, 0x0000, serial, 9);
}

/* ATECC608B hardware TRNG: 32 certified-random bytes (Random opcode 0x1B).
 * Uses mode 0x01 (SEED_UPDATE) which forces a reseed from the hardware
 * entropy source. Plain RANDOM mode (0x00) on a fresh/unconfigured device
 * returns the erased seed (0xFF...) deterministically, so we must seed-update. */
int atecc_random(I2C_HandleTypeDef *hi2c, uint8_t addr7, uint8_t out[32]) {
    return atecc_cmd(hi2c, addr7, 0x1B, 0x01, 0x0000, out, 32);
}

/* SelfTest (0x77): runs internal self-tests and reseeds the DRBG/TRNG. Best
 * effort — on a fresh/unconfigured device this conditions the RNG; a non-zero
 * return just means the self-test didn't fully pass (e.g. ECDSA test needs a
 * key slot) and is non-fatal. Returns 0 if status byte is 0x00. */
int atecc_selftest(I2C_HandleTypeDef *hi2c, uint8_t addr7) {
    uint8_t status = 0;
    if (atecc_cmd(hi2c, addr7, 0x77, 0x00, 0x0000, &status, 1) != 0) return -1;
    return (status == 0x00) ? 0 : 1;
}

int atecc_wake_token(I2C_HandleTypeDef *hi2c, uint8_t addr7) {
    uint8_t resp[4] = {0};
    uint16_t dev = (uint16_t)(addr7 << 1);
    uint8_t zero = 0x00;

    atecc_wake(hi2c);   /* SDA low pulse (tWHI >= 60us) */

    if (HAL_I2C_Master_Transmit(hi2c, dev, &zero, 1, 50) != HAL_OK) return -1;
    if (HAL_I2C_Master_Receive(hi2c, dev, resp, 4, 50) != HAL_OK) return -1;

    if (resp[0] == 0x04 && resp[1] == 0x11 && resp[2] == 0x33 && resp[3] == 0x43) {
        return 0;   /* canonical ATECC wake response */
    }
    return -2;      /* responded, but not the wake signature */
}
