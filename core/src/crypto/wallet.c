#include "wallet.h"
#include "bip39.h"
#include "bip32.h"
#include "ecdsa.h"
#include "secp256k1.h"
#include "curves.h"
#include "sha3.h"
#include "memzero.h"
#include <string.h>

int wallet_mnemonic_from_entropy(const uint8_t *entropy, size_t entropy_len, char *out) {
    const char *m = mnemonic_from_data(entropy, entropy_len);
    if (m == 0 || m[0] == 0) return -1;
    strcpy(out, m);   /* trezor returns a static buffer — copy it */
    return 0;
}

void wallet_seed_from_mnemonic(const char *mnemonic, const char *passphrase, uint8_t seed[64]) {
    mnemonic_to_seed(mnemonic, passphrase ? passphrase : "", seed, 0);
}

int wallet_derive_eth(const uint8_t seed[64], uint32_t index,
                      uint8_t priv[32], uint8_t pub65[65], uint8_t addr[20]) {
    HDNode node;
    if (!hdnode_from_seed(seed, 64, SECP256K1_NAME, &node)) return -1;
    hdnode_private_ckd(&node, 0x8000002C);   /* 44' */
    hdnode_private_ckd(&node, 0x8000003C);   /* 60' */
    hdnode_private_ckd(&node, 0x80000000);   /* 0'  */
    hdnode_private_ckd(&node, 0x00000000);   /* 0   */
    hdnode_private_ckd(&node, index);        /* N   */
    hdnode_fill_public_key(&node);
    memcpy(priv, node.private_key, 32);
    if (ecdsa_get_public_key65(node.curve->params, node.private_key, pub65) != 0) {
        memzero(&node, sizeof(node));
        return -1;
    }
    wallet_address_from_pub65(pub65, addr);
    memzero(&node, sizeof(node));
    return 0;
}

void wallet_address_from_pub65(const uint8_t pub65[65], uint8_t addr[20]) {
    uint8_t hash[32];
    keccak_256(pub65 + 1, 64, hash);
    memcpy(addr, hash + 12, 20);
    memzero(hash, sizeof(hash));
}

int wallet_sign(const uint8_t priv[32], const uint8_t hash32[32],
                uint8_t sig[64], uint8_t *recid) {
    uint8_t v = 0;
    if (ecdsa_sign_digest(&secp256k1, priv, hash32, sig, &v, 0) != 0) return -1;
    if (recid) *recid = v;
    return 0;
}
