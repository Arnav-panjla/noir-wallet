# Noir Wallet - Complete Documentation Index

Your complete guide to the Noir Wallet hardware wallet project.

## 📚 Documentation Roadmap

Start here and follow the path that matches your goal.

### 🚀 I Want to Get Started Now (30 minutes)
1. Read: [QUICK_START.md](QUICK_START.md) - Hardware wiring + Installation
2. Read: [core/main/README.md](core/main/README.md) - What each module does
3. Upload: 11 Python files from `core/main/`
4. Test: Press buttons, run `tests.run_all_tests()`

### 🛠️ I Want to Set Up Thonny IDE (1 hour)
1. Read: [THONNY_SETUP.md](THONNY_SETUP.md) - Complete IDE setup
2. Flash MicroPython via Thonny
3. Configure interpreter
4. Drag-drop Python files to device
5. Run tests in shell

### 🔗 I Want to Integrate with MetaMask (2 hours)
1. Read: [INTEGRATION_GUIDE.md](INTEGRATION_GUIDE.md) - Full protocol
2. Review: [web/wallet-bridge.html](web/wallet-bridge.html) - Reference implementation
3. Understand: USB HID protocol specification
4. Implement: Custom provider class
5. Test: With wallet-bridge.html demo

### 🔒 I Want to Understand the Crypto (1-2 hours)
1. Read: [core/main/README.md](core/main/README.md) - Module overview
2. Review: [core/main/crypto.py](core/main/crypto.py) - secp256k1 implementation
3. Review: [core/main/tx_parser.py](core/main/tx_parser.py) - RLP encoding
4. Test: `import crypto; signer = EthereumSigner(); print(signer.get_address())`

### 🔧 I Want to Contribute (2-4 hours)
1. Read: [README.md](README.md) - Full architecture
2. Review: All Python files in `core/main/`
3. Run: [core/main/tests.py](core/main/tests.py) - Test suite
4. Extend: Add new features
5. Submit: PR with tests

### 📖 I Want Full Documentation (Entire project)
- Start with this INDEX
- Read all .md files
- Read all Python comments
- Review test cases

---

## 📄 Documentation Files (5 files, ~1,500 lines)

### Getting Started
| File | Lines | Topic | Who Should Read |
|------|-------|-------|-----------------|
| [QUICK_START.md](QUICK_START.md) | 200 | Hardware setup & first run | Everyone |
| [THONNY_SETUP.md](THONNY_SETUP.md) | 300 | IDE configuration & development | Developers |
| [HARDWARE_SPECS.md](HARDWARE_SPECS.md) | 300 | Complete hardware guide | Hardware builders |

### Integration & Advanced
| File | Lines | Topic | Who Should Read |
|------|-------|-------|-----------------|
| [INTEGRATION_GUIDE.md](INTEGRATION_GUIDE.md) | 250 | MetaMask integration | App developers |
| [BUILD_SUMMARY.md](BUILD_SUMMARY.md) | 250 | Project overview | Project managers |

### Reference
| File | Lines | Topic | Who Should Read |
|------|-------|-------|-----------------|
| [README.md](README.md) | 300 | Full project documentation | Everyone |
| [core/main/README.md](core/main/README.md) | 120 | Module-level docs | Developers |

---

## 💾 Firmware Files (11 Python files, ~1,700 lines)

### Core Application
| File | Lines | Purpose | Status |
|------|-------|---------|--------|
| [boot.py](core/main/boot.py) | 30 | Board initialization | ✅ Complete |
| [main.py](core/main/main.py) | 300 | Main event loop | ✅ Complete |
| [config.py](core/main/config.py) | 50 | Configuration | ✅ Complete |

### Hardware Drivers
| File | Lines | Purpose | Status |
|------|-------|---------|--------|
| [display.py](core/main/display.py) | 120 | OLED SSD1306 driver | ✅ Complete |
| [buttons.py](core/main/buttons.py) | 80 | 5-button input handler | ✅ Complete |
| [state_machine.py](core/main/state_machine.py) | 150 | State management | ✅ Complete |

### Cryptography
| File | Lines | Purpose | Status |
|------|-------|---------|--------|
| [crypto.py](core/main/crypto.py) | 300 | secp256k1, RLP, Keccak | ✅ Complete |
| [tx_parser.py](core/main/tx_parser.py) | 200 | Transaction parsing | ✅ Complete |

### Communication & Optional
| File | Lines | Purpose | Status |
|------|-------|---------|--------|
| [hid_interface.py](core/main/hid_interface.py) | 120 | USB HID protocol | ✅ Complete |
| [atecc608a.py](core/main/atecc608a.py) | 150 | Secure element interface | ✅ Complete (optional) |
| [tests.py](core/main/tests.py) | 200 | Test suite | ✅ Complete |

---

## 🌐 Web Files (1 HTML file, ~250 lines)

| File | Purpose |
|------|---------|
| [web/wallet-bridge.html](web/wallet-bridge.html) | WebHID browser interface for testing |

---

## 📊 Project Statistics

```
Total Code:         ~3,200 lines
  - Firmware:       ~1,700 lines (MicroPython)
  - Tests:          ~200 lines
  - Web:            ~250 lines
  - Documentation:  ~1,000+ lines

Files:              18 files
  - Python:         11 .py files
  - HTML/Web:       1 .html file
  - Markdown:       6 .md files

Time to Deploy:     ~30-60 minutes (with Thonny)
Time to Learn:      ~2-4 hours (full system)
```

---

## 🎯 Feature Checklist

### Core Features ✅
- [x] MicroPython on RP2040
- [x] OLED 128×64 display
- [x] 5 push buttons
- [x] secp256k1 signing
- [x] Keccak-256 hashing
- [x] RLP encoding
- [x] USB HID protocol
- [x] PIN protection
- [x] Transaction signing
- [x] Test suite

### Web Integration ✅
- [x] WebHID interface
- [x] Browser bridge
- [x] JSON protocol
- [x] Status display

### Documentation ✅
- [x] Quick start guide
- [x] Hardware specs
- [x] IDE setup guide
- [x] Integration guide
- [x] Module documentation
- [x] Security checklist

### Optional (Future) ⏳
- [ ] ATECC608A integration
- [ ] Firmware updates
- [ ] QR code display
- [ ] Multi-chain support
- [ ] Hardware wallet standard

---

## 🚦 Getting Help

### By Topic

#### "How do I..."

| Question | Answer |
|----------|--------|
| ...install the firmware? | [QUICK_START.md](QUICK_START.md) - Installation section |
| ...set up Thonny? | [THONNY_SETUP.md](THONNY_SETUP.md) - Step by step |
| ...integrate with MetaMask? | [INTEGRATION_GUIDE.md](INTEGRATION_GUIDE.md) - Full walkthrough |
| ...understand the code? | [core/main/README.md](core/main/README.md) - Module docs |
| ...wire the hardware? | [HARDWARE_SPECS.md](HARDWARE_SPECS.md) - Pin diagrams |
| ...test the crypto? | [core/main/tests.py](core/main/tests.py) - Run tests |
| ...debug an issue? | [THONNY_SETUP.md](THONNY_SETUP.md#troubleshooting) - Troubleshooting |

#### "What is..."

| Concept | Reference |
|---------|-----------|
| Architecture | [README.md](README.md#architecture) + [BUILD_SUMMARY.md](BUILD_SUMMARY.md) |
| USB HID Protocol | [INTEGRATION_GUIDE.md](INTEGRATION_GUIDE.md#usb-hid-protocol) |
| Crypto Implementation | [core/main/crypto.py](core/main/crypto.py) comments |
| Pin Configuration | [HARDWARE_SPECS.md](HARDWARE_SPECS.md#pin-configuration) |
| State Machine | [core/main/state_machine.py](core/main/state_machine.py) comments |

---

## 📋 Installation Checklist

### Before You Start
- [ ] Read [QUICK_START.md](QUICK_START.md)
- [ ] Gather hardware (RP2040, OLED, buttons)
- [ ] Install Thonny IDE

### Installation
- [ ] Flash MicroPython to RP2040
- [ ] Wire OLED (GP4=SDA, GP5=SCL)
- [ ] Wire 5 buttons (GP16-20 to GND)
- [ ] Upload all .py files from `core/main/`
- [ ] Test with `import tests; tests.run_all_tests()`

### Verification
- [ ] OLED shows splash screen
- [ ] Buttons respond
- [ ] All tests pass
- [ ] No memory errors

### Next Steps
- [ ] Test with web/wallet-bridge.html
- [ ] Try signing a transaction
- [ ] Read [INTEGRATION_GUIDE.md](INTEGRATION_GUIDE.md)

---

## 🔐 Security Checklist

Before production use:

- [ ] Understand the code (read crypto.py)
- [ ] Run all tests (tests.py)
- [ ] Test with test vectors
- [ ] Verify RLP encoding
- [ ] Check secp256k1 implementation
- [ ] Test PIN protection
- [ ] Review transaction display
- [ ] Security audit (professional)
- [ ] Hardware audit (professional)
- [ ] Only then: Use with real funds

⚠️ **DO NOT use with real funds until fully audited!**

---

## 📚 References

### External Documentation
- [MicroPython Docs](https://micropython.org)
- [RP2040 Datasheet](https://datasheets.raspberrypi.com)
- [SSD1306 Datasheet](https://cdn-shop.adafruit.com/datasheets/SSD1306.pdf)
- [ATECC608A Datasheet](https://www.microchip.com/en-us/product/ATECC608A)
- [Ethereum Docs](https://ethereum.org/en/developers/)
- [secp256k1 Reference](https://en.bitcoin.it/wiki/Secp256k1)

### In This Project
- **Architecture**: [README.md](README.md#architecture)
- **Crypto**: [core/main/crypto.py](core/main/crypto.py)
- **Protocol**: [INTEGRATION_GUIDE.md](INTEGRATION_GUIDE.md#communication-protocol)
- **Hardware**: [HARDWARE_SPECS.md](HARDWARE_SPECS.md)

---

## 🎓 Learning Paths

### Beginner (2 hours)
1. [QUICK_START.md](QUICK_START.md) - Get it running
2. [THONNY_SETUP.md](THONNY_SETUP.md) - Learn the IDE
3. [core/main/README.md](core/main/README.md) - Understand modules
4. Test with [web/wallet-bridge.html](web/wallet-bridge.html)

### Intermediate (4 hours)
1. Everything above
2. [core/main/crypto.py](core/main/crypto.py) - Understand signing
3. [core/main/tx_parser.py](core/main/tx_parser.py) - Learn RLP
4. [INTEGRATION_GUIDE.md](INTEGRATION_GUIDE.md) - Understand protocol
5. Modify code in Thonny

### Advanced (6-8 hours)
1. Everything above
2. Full code review of all files
3. [BUILD_SUMMARY.md](BUILD_SUMMARY.md) - Understand architecture
4. Read all test code
5. Security analysis
6. Plan extensions

---

## 🔄 Workflow

### Daily Development
```
1. Open Thonny
2. Edit Python file in left panel
3. Right-click → Send to device
4. Test in shell
5. Check OLED/buttons
6. Review serial output
```

### Adding Features
```
1. Read relevant module docs
2. Write tests first
3. Implement feature
4. Test locally
5. Test on device
6. Commit with message
```

### Debugging
```
1. Check Thonny shell output
2. Enable verbose logging
3. Add print() statements
4. Test isolated module
5. Check hardware connections
6. Review state machine
```

---

## 🎯 Next Milestones

### This Week
- [ ] Hardware assembled
- [ ] Firmware uploaded
- [ ] Tests passing
- [ ] OLED/buttons working

### This Month
- [ ] MetaMask integration tested
- [ ] Real transaction signing
- [ ] WebHID bridge stable
- [ ] Documentation complete

### This Quarter
- [ ] ATECC608A integrated
- [ ] Security audit passed
- [ ] Ready for limited beta

### This Year
- [ ] Production release
- [ ] Community contributions
- [ ] Hardware wallet standard
- [ ] FPGA acceleration

---

## 💬 Support

### For Questions About...

| Topic | See |
|-------|-----|
| Getting started | [QUICK_START.md](QUICK_START.md) |
| IDE setup | [THONNY_SETUP.md](THONNY_SETUP.md) |
| Hardware wiring | [HARDWARE_SPECS.md](HARDWARE_SPECS.md) |
| Code organization | [core/main/README.md](core/main/README.md) |
| Integration | [INTEGRATION_GUIDE.md](INTEGRATION_GUIDE.md) |
| Project overview | [BUILD_SUMMARY.md](BUILD_SUMMARY.md) |
| Everything | [README.md](README.md) |

---

## 📝 Quick Links

### Essential Files (Must Read)
1. [README.md](README.md) - Project overview
2. [QUICK_START.md](QUICK_START.md) - Get running
3. [HARDWARE_SPECS.md](HARDWARE_SPECS.md) - Wire it up

### For Developers
1. [THONNY_SETUP.md](THONNY_SETUP.md) - IDE setup
2. [core/main/README.md](core/main/README.md) - Module docs
3. [core/main/tests.py](core/main/tests.py) - Test suite

### For Integration
1. [INTEGRATION_GUIDE.md](INTEGRATION_GUIDE.md) - Full protocol
2. [web/wallet-bridge.html](web/wallet-bridge.html) - Reference impl.
3. [core/main/hid_interface.py](core/main/hid_interface.py) - HID code

---

## ✅ Progress Tracker

Track your journey:

```
[ ] Read README.md
[ ] Read QUICK_START.md
[ ] Gather hardware
[ ] Flash MicroPython
[ ] Wire OLED & buttons
[ ] Upload Python files
[ ] Run tests
[ ] Test OLED display
[ ] Test buttons
[ ] Read INTEGRATION_GUIDE.md
[ ] Test with web bridge
[ ] First transaction sign
[ ] Review all code
[ ] Security checklist
```

---

## 🚀 Ready to Start?

1. **Beginner?** → Start with [QUICK_START.md](QUICK_START.md)
2. **Developer?** → Start with [THONNY_SETUP.md](THONNY_SETUP.md)
3. **Integrating?** → Start with [INTEGRATION_GUIDE.md](INTEGRATION_GUIDE.md)
4. **Contributing?** → Read [README.md](README.md) + [BUILD_SUMMARY.md](BUILD_SUMMARY.md)
5. **Everything else?** → You're in the right place! 📖

---

**Last Updated:** 2026-06-26  
**Version:** 1.0  
**Status:** Complete & Ready to Use  
**License:** MIT

**Happy building! 🔐✨**
