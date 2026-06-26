# Noir Wallet - Ethereum Hardware Wallet

<div align="center">
  <img src="https://img.shields.io/badge/Status-In%20Development-yellow" alt="Status: In Development">
  <img src="https://img.shields.io/badge/Language-MicroPython-blue" alt="Language: MicroPython">
  <img src="https://img.shields.io/badge/Hardware-RP2040-orange" alt="Hardware: RP2040">
  <br/>
  <strong>Secure, offline Ethereum transaction signing on RP2040</strong>
</div>

---

## Overview

Noir Wallet is a **hardware wallet** for Ethereum running on the **RP2040 microcontroller**. It provides offline transaction signing with PIN protection, human-readable transaction verification on an OLED display, and USB HID communication with MetaMask and other Ethereum dApps.

### Key Features

- ✅ **Offline Signing**: Private keys stay on device
- ✅ **PIN Protection**: 4-digit PIN before any signing
- ✅ **OLED Display**: 128×64 shows transaction details
- ✅ **5 Push Buttons**: Full user interaction
- ✅ **WebHID Compatible**: Works with browsers via USB
- ✅ **MetaMask Ready**: Direct integration with MetaMask
- ✅ **Secure Element Ready**: ATECC608A slot for future use
- ✅ **Modular MicroPython**: Easy to modify and extend

---

## Hardware

### Required
- **MCU**: RP2040 (Raspberry Pi Pico or clone)
- **Display**: SSD1306 128×64 OLED (I2C)
- **Input**: 5× momentary push buttons (GPIO)
- **USB**: USB-A to micro-USB cable

### Optional (Future)
- **Secure Element**: ATECC608A (I2C) for key storage
- **FPGA**: For acceleration

### Pin Configuration

| Component | Pin | Note |
|-----------|-----|------|
| OLED SDA | GPIO 4 | I2C ID 0 |
| OLED SCL | GPIO 5 | I2C ID 0 |
| Button UP | GPIO 16 | Active low |
| Button DOWN | GPIO 17 | Active low |
| Button LEFT | GPIO 18 | Active low |
| Button RIGHT | GPIO 19 | Active low |
| Button CENTER | GPIO 20 | Active low |
| SE SDA | GPIO 6 | I2C ID 1 (optional) |
| SE SCL | GPIO 7 | I2C ID 1 (optional) |

---

## Software Architecture

```
core/main/
├── boot.py              # Board initialization
├── main.py              # Main application loop (500 lines)
├── config.py            # Configuration constants
├── display.py           # SSD1306 OLED driver (120 lines)
├── buttons.py           # 5-button input handler (80 lines)
├── state_machine.py     # Wallet state management (150 lines)
├── crypto.py            # Ethereum signing + RLP (300 lines)
├── hid_interface.py     # USB HID communication (120 lines)
├── atecc608a.py         # ATECC608A secure element (150 lines)
├── tx_parser.py         # Transaction parsing (200 lines)
├── tests.py             # Test suite (200 lines)
└── README.md            # Module documentation
```

### Core Modules

| Module | Purpose | LOC |
|--------|---------|-----|
| **display.py** | OLED UI, menus, transaction display | 120 |
| **buttons.py** | Debounced button input, press detection | 80 |
| **state_machine.py** | PIN entry, confirmation, signing states | 150 |
| **crypto.py** | secp256k1 signing, RLP encoding, Keccak | 300 |
| **hid_interface.py** | USB HID protocol, MetaMask commands | 120 |
| **tx_parser.py** | Transaction validation and formatting | 200 |
| **atecc608a.py** | Microchip secure element interface | 150 |

---

## Getting Started

### Quick Start (5 minutes)

1. **Flash MicroPython** to RP2040
2. **Copy firmware files** from `core/main/`
3. **Wire hardware** (OLED + 5 buttons)
4. **Test** with Thonny IDE

See **[QUICK_START.md](QUICK_START.md)** for step-by-step instructions.

### Detailed Setup

See **[THONNY_SETUP.md](THONNY_SETUP.md)** for:
- Installing Thonny IDE
- Flashing MicroPython
- Uploading firmware
- Running tests
- Debugging

---

## API & Protocol

### USB HID Commands

```
Host → Device
[CMD_BYTE][PAYLOAD_LENGTH][PAYLOAD...]

Device → Host
[0xFF][LENGTH][JSON_RESPONSE...]  (success)
[0xFE][LENGTH][ERROR_MESSAGE...]  (error)
```

### Commands

| CMD | Type | Payload | Response |
|-----|------|---------|----------|
| 0x01 | Get Public Key | - | `{pubkey, address}` |
| 0x02 | Sign Tx | TX JSON | `{r, s, v}` |
| 0x03 | Verify PIN | PIN | `{valid: bool}` |
| 0x04 | Get Status | - | `{state, address}` |

### Example: Sign Transaction

```javascript
// JavaScript
const tx = {
  nonce: 5,
  gasPrice: '20000000000',
  gas: '21000',
  to: '0x742d35Cc6634C0532925a3b844Bc9e7595f42e55',
  value: '1000000000000000000',
  data: '0x',
  chainId: 1
};

const cmd = new Uint8Array([0x02, JSON.stringify(tx).length, ...new TextEncoder().encode(JSON.stringify(tx))]);
await device.sendReport(0, cmd);
```

---

## User Interface

### Boot Sequence
1. Splash screen (2 sec)
2. Wallet address display
3. Ready for input

### PIN Entry
- **UP/DOWN**: Cycle digit 0-9
- **LEFT**: Delete digit
- **CENTER**: Confirm (after 4 digits)
- **Long CENTER**: Cancel

Default PIN: `1234`

### Transaction Signing
1. Receive TX from host
2. Show recipient, value, gas fee
3. Request PIN
4. Show "Confirm?" screen
5. **UP** = Sign, **DOWN** = Reject
6. Return signature (r, s, v)

---

## Security Model

### On-Device Protection
- ✅ Private key never leaves device
- ✅ PIN required before signing
- ✅ User confirms transaction on display
- ✅ No plaintext key storage
- ✅ Hardware entropy (RP2040 RNG)

### Limitations
- ⚠️ No key backup (single instance)
- ⚠️ No factory reset protection
- ⚠️ OLED can be spoofed by host (use your eyes!)
- ⚠️ No rate limiting (yet)

### Future (with ATECC608A)
- Key generation in secure element
- Signing in secure element
- Tamper detection
- Secure storage

---

## Roadmap

### Phase 1 (Current) ✅
- [x] MicroPython firmware on RP2040
- [x] OLED + 5-button UI
- [x] secp256k1 signing
- [x] Transaction RLP parsing
- [x] USB HID protocol
- [x] WebHID bridge demo
- [x] Test suite

### Phase 2 (Next)
- [ ] MetaMask integration tested
- [ ] Real Ethereum network signing
- [ ] QR code for address
- [ ] Message signing (EIP-191)
- [ ] Multiple accounts

### Phase 3 (Future)
- [ ] ATECC608A integration
- [ ] EIP-712 typed data
- [ ] Hardware wallet standard
- [ ] FPGA signing acceleration
- [ ] Firmware updates

---

## Cryptography

| Component | Implementation | Notes |
|-----------|----------------|-------|
| **Curve** | secp256k1 | Pure Python (RP2040 has no HW crypto) |
| **Signing** | ECDSA | Software, ~2KB private key |
| **Hashing** | Keccak-256 | Falls back to SHA256 if pycryptodome unavailable |
| **Encoding** | RLP | Custom implementation |
| **Random** | `os.urandom()` | RP2040 hardware RNG |

### Test Coverage
```
✓ Crypto module     (secp256k1, RLP, Keccak)
✓ State machine     (PIN entry, confirmation)
✓ Button input      (debounce, press detection)
✓ HID protocol      (encode/decode)
✓ Transaction parser (validation, formatting)
```

Run: `import tests; tests.run_all_tests()`

---

## Files & Documentation

### Firmware
- `core/main/main.py` - Main event loop
- `core/main/display.py` - OLED driver
- `core/main/buttons.py` - Button handler
- `core/main/state_machine.py` - State machine
- `core/main/crypto.py` - Ethereum crypto
- `core/main/hid_interface.py` - USB HID
- `core/main/atecc608a.py` - Secure element
- `core/main/tx_parser.py` - TX parsing

### Web Integration
- `web/wallet-bridge.html` - WebHID bridge + UI
- `INTEGRATION_GUIDE.md` - MetaMask integration

### Documentation
- `QUICK_START.md` - 5-minute setup
- `THONNY_SETUP.md` - Detailed IDE setup
- `core/main/README.md` - Module docs
- `INTEGRATION_GUIDE.md` - MetaMask integration

---

## Testing

### Run All Tests
```python
import tests
tests.run_all_tests()
```

### Test Individual Modules
```python
from crypto import EthereumSigner
signer = EthereumSigner()
print(signer.get_address())

from buttons import ButtonManager
buttons = ButtonManager()
action = buttons.wait_for_button()

from display import OLEDDisplay
disp = OLEDDisplay()
disp.show_status("Hello!")
```

---

## Development

### Quick Iteration in Thonny
1. Edit Python file in left panel
2. Right-click → Send to device
3. In shell: `exec(open('module.py').read())`
4. Test live in shell

### Common Issues

| Issue | Solution |
|-------|----------|
| OLED not showing | Check I2C address with `i2c.scan()` |
| Buttons not working | Verify GPIO pins with `Pin(16).value()` |
| Out of memory | Run `import gc; gc.collect()` |
| USB not detected | Try different port, restart Thonny |

See [THONNY_SETUP.md](THONNY_SETUP.md) for more.

---

## Performance

| Operation | Time | Notes |
|-----------|------|-------|
| Boot | ~2 sec | Splash screen |
| Get Public Key | ~1 sec | Device initialization |
| Sign Tx | ~2-5 sec | Including user PIN/confirm |
| OLED update | ~50 ms | Full screen redraw |

**Memory**: ~140 KB free after boot

---

## Security Audit Checklist

Before using with real funds:
- [ ] Code audit by cryptography expert
- [ ] Test secp256k1 against known vectors
- [ ] Verify RLP encoding correctness
- [ ] Test with ATECC608A
- [ ] Penetration testing
- [ ] Hardware tamper testing

---

## Legal

⚠️ **THIS IS BETA SOFTWARE**

For educational purposes. Do not use with real funds until audited.

---

## License

MIT

---

## Contributing

Contributions welcome! Please:
1. Test thoroughly
2. Add unit tests
3. Document changes
4. Submit PR with clear description

---

## Support

- 📖 [Quick Start](QUICK_START.md)
- 🛠️ [Thonny Setup](THONNY_SETUP.md)
- 🔗 [MetaMask Integration](INTEGRATION_GUIDE.md)
- 🐛 Issues/bugs: GitHub issues

---

## See Also

- [MicroPython Docs](https://micropython.org)
- [RP2040 Datasheet](https://datasheets.raspberrypi.com)
- [Ethereum Signing](https://ethereum.org/en/developers)
- [WebHID API](https://wicg.github.io/webhid)

**Built for BlocSoc @ University** 🚀

