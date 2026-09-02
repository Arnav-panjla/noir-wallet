#include <stdio.h>
#include <string.h>
#include "wallet.h"
#include "ecdsa.h"
#include "secp256k1.h"

static int failures = 0;
#define CHECK(cond, name) do { \
    if (cond) printf("PASS  %s\n", name); \
    else { printf("FAIL  %s\n", name); failures++; } \
} while (0)

static void hexdump(const uint8_t *d, size_t n) {
    for (size_t i = 0; i < n; i++) printf("%02x", d[i]);
}

static int from_hex(const char *s, uint8_t *out) {
    size_t len = strlen(s) / 2;
    for (size_t i = 0; i < len; i++) {
        unsigned v;
        if (sscanf(s + 2 * i, "%2x", &v) != 1) return -1;
        out[i] = (uint8_t)v;
    }
    return (int)len;
}

int main(void) {
    char mnemonic[256];
    uint8_t entropy[16] = {0};
    uint8_t seed[64];

    /* 1. BIP-39 entropy -> words (16 zero bytes = "abandon x11 about") */
    CHECK(wallet_mnemonic_from_entropy(entropy, 16, mnemonic) == 0, "bip39 entropy->words");
    CHECK(strcmp(mnemonic, "abandon abandon abandon abandon abandon abandon abandon abandon abandon abandon abandon about") == 0,
          "bip39 words match (abandon x11 about)");
    printf("  mnemonic: %s\n", mnemonic);

    /* 2. BIP-39 seed, official vector (passphrase "TREZOR") */
    wallet_seed_from_mnemonic(mnemonic, "TREZOR", seed);
    {
        const char *expect =
            "c55257c360c07c72029aebc1b53c05ed0362ada38ead3e3e9efa3708e5349553"
            "1f09a6987599d18264c1e1c92f2cf141630c7a3c4ab7c81b2f001698e7463b04";
        uint8_t exp_seed[64];
        from_hex(expect, exp_seed);
        CHECK(memcmp(seed, exp_seed, 64) == 0, "bip39 seed vector (TREZOR)");
    }

    /* 3. Full pipeline: Hardhat's default mnemonic -> account 0 address */
    {
        const char *mn = "test test test test test test test test test test test junk";
        uint8_t seed2[64], priv[32], pub65[65], addr[20];
        uint8_t expect_addr[20];
        from_hex("f39fd6e51aad88f6f4ce6ab8827279cfffb92266", expect_addr);
        wallet_seed_from_mnemonic(mn, "", seed2);
        CHECK(wallet_derive_eth(seed2, 0, priv, pub65, addr) == 0, "derive m/44'/60'/0'/0/0");
        CHECK(memcmp(addr, expect_addr, 20) == 0, "address == 0xf39F... (Hardhat account 0)");
        if (memcmp(addr, expect_addr, 20) != 0) {
            printf("  got addr: "); hexdump(addr, 20); printf("\n");
        }

        /* 4. sign + verify + RFC6979 determinism */
        uint8_t hash[32];
        for (int i = 0; i < 32; i++) hash[i] = 0xAB;
        uint8_t sig[64], sig2[64], v, v2;
        CHECK(wallet_sign(priv, hash, sig, &v) == 0, "sign hash");
        CHECK(ecdsa_verify_digest(&secp256k1, pub65, sig, hash) == 0, "verify signature");
        CHECK(wallet_sign(priv, hash, sig2, &v2) == 0 && memcmp(sig, sig2, 64) == 0 && v == v2,
              "RFC6979 deterministic (same sig twice)");
    }

    printf("\n%s (%d failures)\n", failures ? "FAILED" : "ALL PASS", failures);
    return failures ? 1 : 0;
}
