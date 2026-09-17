/* Hardware entropy for trezor-crypto's random_buffer().
 *
 * Backed by the ATECC608B hardware TRNG (Random command, 32 certified-random
 * bytes). This is what makes the "key never leaves the chip" story honest:
 * every bit of randomness the wallet stack consumes comes from the secure
 * element's true-random generator, never the MCU's (predictable) RNG.
 *
 * Only random_buffer() needs defining: rand.h's random32() is a static inline
 * wrapper over it, and nothing in our EVM build references random_uniform/
 * random_permute/random_reseed (those are only used by mnemonic_generate,
 * which we don't call — we feed ATECC entropy to mnemonic_from_data directly).
 */

#include "rand.h"
#include "atecc.h"
#include "gpio_map.h"
#include <string.h>

#define ATECC_ADDR7 0x60   /* ATECC608B default 7-bit I2C address */

void random_buffer(uint8_t *buf, size_t len) {
    while (len > 0) {
        uint8_t chunk[32];
        size_t n = len < 32 ? len : 32;
        if (atecc_random(&hi2c1, ATECC_ADDR7, chunk) != 0) {
            /* Never return silent weak randomness: zero on failure so any key
             * derived from a dead TRNG is obviously broken, not insecure. */
            memset(buf, 0, len);
            return;
        }
        memcpy(buf, chunk, n);
        buf += n;
        len -= n;
    }
}
