#ifndef WALLET_H
#define WALLET_H

#include <stdint.h>
#include <stddef.h>

/* BIP-39 mnemonic (12 words for 16-byte entropy) from raw entropy.
 * Writes the space-separated words into `out` (>= 256 bytes). 0 = ok. */
int wallet_mnemonic_from_entropy(const uint8_t *entropy, size_t entropy_len, char *out);

/* BIP-39 seed (64 bytes) from mnemonic + passphrase. */
void wallet_seed_from_mnemonic(const char *mnemonic, const char *passphrase, uint8_t seed[64]);

/* Derive m/44'/60'/0'/0/index. Fills priv[32], uncompressed pub65[65], addr[20].
 * 0 = ok. */
int wallet_derive_eth(const uint8_t seed[64], uint32_t index,
                      uint8_t priv[32], uint8_t pub65[65], uint8_t addr[20]);

/* keccak256(uncompressed_pub[1:65])[12:] -> addr[20]. */
void wallet_address_from_pub65(const uint8_t pub65[65], uint8_t addr[20]);

/* RFC6979 deterministic secp256k1 signature over hash32.
 * sig[64] = r(32)||s(32); *recid = recovery id (0/1). 0 = ok. */
int wallet_sign(const uint8_t priv[32], const uint8_t hash32[32],
                uint8_t sig[64], uint8_t *recid);

#endif
