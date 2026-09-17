#ifndef STORAGE_H
#define STORAGE_H

#include <stdint.h>

/* Flash config storage for NOIR wallet.
 *
 * Lives in the last 128 KB flash sector (sector 5 @ 0x08020000 on the
 * F401CC). Erased flash reads 0xFF, so a never-provisioned board fails the
 * magic check and reports "empty".
 */

#define STORAGE_MAGIC  0x4E4F4952UL   /* "NOIR" */

typedef struct {
    uint32_t magic;                /* 0x4E4F4952 when valid */
    uint32_t crc;                  /* CRC-32 over bytes [8..sizeof) */
    uint8_t  provisioned;          /* 1 = wallet exists */
    uint8_t  _pad1[3];
    /* Fields populated by later steps: */
    uint8_t  passcode_hash[32];    /* PBKDF2/SHA-256 of the button sequence */
    uint8_t  passcode_salt[16];
    uint8_t  seed_entropy[32];     /* demo: entropy in flash (not ATECC-wrapped) */
    uint8_t  seed_len;             /* 16 = 12 words, 32 = 24 words */
    uint8_t  _pad2[3];
} storage_cfg_t;

/* CRC-32 (zlib polynomial, reflected, init/final 0xFFFFFFFF). */
uint32_t crc32(const uint8_t *data, uint32_t len);

/* Returns 1 and fills *out if a valid (magic + CRC) config is present. */
int storage_load(storage_cfg_t *out);

/* Erases the sector and writes cfg (sets magic + CRC). Returns 0 on success. */
int storage_save(const storage_cfg_t *cfg);

/* Factory reset: erases the config sector. Returns 0 on success. */
int storage_erase(void);

#endif
