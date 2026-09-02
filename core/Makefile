CC      = gcc
CFLAGS  = -O2 -std=gnu99 -Wall -Wno-unused-function \
          -Ilib/trezor-crypto -Isrc/crypto \
          -DUSE_BIP32_25519_CURVES=0 -DUSE_NEM=0 -DUSE_CARDANO=0 \
          -DUSE_INSECURE_PRNG=1 \
          -ffunction-sections -fdata-sections
LDFLAGS = -Wl,--gc-sections

TC = lib/trezor-crypto
# aes/ and ed25519-donna/ are NOT compiled: they're only used by NEM and
# 25519-curve code paths, both compiled out. Their headers are still needed
# (bip32.h includes them unconditionally), which is why the files exist.
TC_SRCS = $(TC)/bignum.c $(TC)/ecdsa.c $(TC)/curves.c $(TC)/secp256k1.c \
          $(TC)/nist256p1.c $(TC)/hmac.c $(TC)/bip32.c $(TC)/bip39.c \
          $(TC)/bip39_english.c $(TC)/pbkdf2.c $(TC)/base58.c $(TC)/sha2.c \
          $(TC)/sha3.c $(TC)/hasher.c $(TC)/memzero.c $(TC)/rfc6979.c \
          $(TC)/hmac_drbg.c \
          $(TC)/der.c $(TC)/consteq.c $(TC)/buffer.c $(TC)/rand.c $(TC)/rand_insecure.c

host_test: test/test_wallet.c src/crypto/wallet.c $(TC_SRCS)
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

.PHONY: clean
clean:
	rm -f host_test
