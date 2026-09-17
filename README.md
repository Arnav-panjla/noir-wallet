# Noir Wallet

<div align="center">
  <img src="https://img.shields.io/badge/status-research%20prototype-yellow" alt="Status: research prototype">
  <img src="https://img.shields.io/badge/firmware-C-blue" alt="Firmware: C">
  <img src="https://img.shields.io/badge/V0-STM32F401%20%2B%20ATECC608A-orange" alt="V0 hardware">
  <br/>
  <strong>Open hardware-wallet research from Blockchain Society, IIT Delhi</strong>
  <br/>
  <a href="https://noirwallet.blocsoc.in">Website</a> ·
  <a href="https://noirwallet.blocsoc.in/paper">Paper</a>
  <br/>
  <br/>
</div>

![Noir Wallet V0 architecture](./v0/assets/Schematics.png)

---

## Repository layout

```text
.
├── v0/
│   ├── core/          # STM32F401 firmware and host tests
│   ├── CAD/           # V0 enclosure source and STL files
│   ├── schematics/    # V0 wiring, pinout, and ATECC notes
│   ├── assets/        # V0 photographs and diagrams
│   └── publication/   # Public V0 research paper and images
├── v1/
│   └── publication/   # Public V1 architecture/security research
├── Proposal/          # Local competition material; ignored by Git
├── LICENSE
└── README.md
```

Root `README.md` is project index. README files under vendored libraries remain
upstream package documentation, not separate Noir Wallet project guides.

## V0: working research prototype

V0 uses STM32F401CCU6, ATECC608A, SSD1306 128×64 OLED, five active-low
buttons, and USB HID enumeration.

Current firmware demonstrates hardware bring-up, BIP-39 mnemonic generation,
Ethereum address derivation, UI, buttons, and USB enumeration. Transaction
frame handling and production signing are not complete: current HID callback
discards incoming frames. ATECC608A is not secp256k1 signing-key store. Treat
V0 as research prototype, not production wallet.

- [Firmware](./v0/core/)
- [Hardware specifications](./v0/schematics/HARDWARE_SPECS.md)
- [CAD](./v0/CAD/)
- [Ledger N3XT paper source](./v0/publication/paper.tex)
- [Ledger N3XT paper PDF](./v0/publication/paper.pdf)

## V1: hardened architecture research

V1 targets non-exportable secp256k1 signing, protected
MCU-to-secure-element communication, secure boot, explicit transaction
confirmation, and smaller custom SMD PCB.

Current recommendation: evaluate NXP EdgeLock SE051C2 first, Microchip TA101
as fallback. Both support secp256k1 for Ethereum and Monad; configured
variants also provide Ed25519 paths for possible Solana support. Secure-element
choice does not independently solve compromised MCU or OLED substitution.

- [Secure-element report source](./v1/publication/secure-element-report.tex)
- [Secure-element report PDF](./v1/publication/secure-element-report.pdf)

## Build V0 firmware

Host crypto tests pass from moved layout. Embedded PlatformIO snapshot still
expects a TinyUSB checkout under `v0/core/lib/tinyusb` and valid source links
under `v0/core/src/trezor` and `v0/core/src/usb/tinyusb`; those dependencies
were already incomplete before repository restructuring.

After restoring those dependencies:

```bash
cd v0/core
pio run
```

Host crypto tests:

```bash
cd v0/core
make
./host_test
```

## Build publications

V0 paper:

```bash
cd v0/publication
pdflatex -interaction=nonstopmode -halt-on-error paper.tex
pdflatex -interaction=nonstopmode -halt-on-error paper.tex
```

V1 secure-element report:

```bash
cd v1/publication
pdflatex -interaction=nonstopmode -halt-on-error secure-element-report.tex
pdflatex -interaction=nonstopmode -halt-on-error secure-element-report.tex
```

## Links

- [Noir Wallet](https://noirwallet.blocsoc.in)
- [Published paper](https://noirwallet.blocsoc.in/paper)
- [GitHub](https://github.com/Arnav-panjla/noir-wallet)
- [STM32F401CC data sheet](https://www.st.com/resource/en/datasheet/stm32f401cc.pdf)
- [NXP EdgeLock SE051 data sheet](https://www.nxp.com/docs/en/data-sheet/SE051.pdf)

## License

See [LICENSE](./LICENSE). Vendored dependencies retain their own notices and
licenses.


