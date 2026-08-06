# Noir Wallet - Ethereum Hardware Wallet

<div align="center">
  <img src="https://img.shields.io/badge/Status-In%20Development-yellow" alt="Status: In Development">
  <img src="https://img.shields.io/badge/Language-MicroPython-blue" alt="Language: C">
  <img src="https://img.shields.io/badge/Hardware-RP2040-orange" alt="Hardware: RP2040">
  <br/>
  <strong>Secure, offline Ethereum transaction signing on RP2040</strong>
</div>

![](./assets/schematics.png)

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

---

## See Also

- [RP2040 Datasheet](https://datasheets.raspberrypi.com)
- [Ethereum Signing](https://ethereum.org/en/developers)
- [WebHID API](https://wicg.github.io/webhid)

**Built for BlocSoc @ University** 🚀

