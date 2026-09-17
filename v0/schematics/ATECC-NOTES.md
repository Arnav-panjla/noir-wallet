# ATECC secure element — findings (parked, revisit later)

## Chip identity
- **ATECC608A**, NOT 608B. Revision bytes `00006002` (a 608B would read `00006003`).
- On the same I2C bus as the OLED: SDA=PB7, SCL=PB6. OLED `0x3C`, ATECC `0x60`.
- 8-bit I2C address byte in config = `0xC0` → 7-bit `0x60` (so locking config will NOT move the address).

## Serial number — SOLVED (just switch the read path)
- The 9-byte serial is `01237674504b151aee`, sitting in the **config zone**: bytes `[0:4]` + `[8:13]`.
- The `Info` command `0x30` / param1 `0x80` (the "official" serial read) returns `ret=-3` on this part — broken path, ignore it.
- Fix: read it via the **Read** command (`0x02`, config zone, block 0, 32 bytes) and splice:
  `serial[0:4] = block[0:4]`, `serial[4:9] = block[8:13]`.

## TRNG determinism — root cause confirmed
- On an **unconfigured / unlocked** part, `Random` (`0x1B`) returns a fixed diagnostic pattern `FF FF 00 00 ...`, NOT entropy. Microchip (GitHub issue #121 + their forum) confirms:
  "Prior to the Configuration zone being locked, the RNG produces 0xFF,0xFF,0x00,0x00… to facilitate testing. Configure (and lock) the config zone before the RNG outputs random numbers."
- That pattern is why we saw `zoo` (BIP-39 word #2047 = all-ones) and a fixed 12-word mnemonic.
- **Fix = lock the config zone.** One-way door. This is the correct end-state for a hardware wallet.

## Lock command (when we do it)
- Opcode `0x17`, **mode `0x80`** = lock Config zone with **CRC check skipped** (no summary CRC needed).
- `atecc_cmd(hi2c, 0x60, 0x17, 0x80, 0x0000, &status, 1)`; `status == 0x00` = locked OK.
- Config lock byte = config offset **87** (`LockConfig`); data+OTP lock = offset **86** (`LockValue`). `0x55` = unlocked, anything else = locked-ish. Current part reads `cfglock=20 vallock=8f` — provisioned but RNG still deterministic, so it needs an explicit Lock.

## Other commands that work
- `Wake`: SDA low pulse + `0x00` token + read 4-byte response → `04113343`.
- `Info` revision (`0x30`, p1 `0x00`): returns `00006002`.
- `SelfTest` (`0x77`): OK.
- `Read` config zone: works for all 4 blocks.

## CRC ground truth (do NOT regress)
- Microchip CRC-16, poly `0x8005`, init `0x0000`, **LSB-first** processing (compare bit 15).
- `atcrc([04,11]) == 0x4333` (matches wake response `33 43`); CRC of `"123456789"` = `0xFEE8` (not `0xBB3D`).
- A standard MSB-first CRC is WRONG for this chip — this bug previously masked every error response.

## Current demo-grade entropy (do not claim NIST-certified)
- Working firmware uses `Random` mode `0x01` (SEED_UPDATE) + `SelfTest` + an MCU DWT cycle-counter XOR backstop in `hw_rng.c`. Unique per generation, not crypto-grade. Upgrading to the true TRNG is the "lock config" step above.
