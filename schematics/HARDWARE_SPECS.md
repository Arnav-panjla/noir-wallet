# Hardware Specifications - Noir Wallet

Complete hardware requirements and wiring guide.

## Board Requirements

### Primary MCU
- **RP2040** (Raspberry Pi Pico or clone)
- 2× I2C interfaces
- 5+ GPIO pins
- 264 KB SRAM, 2 MB Flash
- USB support for HID

**Recommended Boards:**
- Raspberry Pi Pico ($4)
- Shrike-lite (custom RP2040 board)
- Various clones available on Aliexpress

## Components List

### Required
| Part | Qty | Notes |
|------|-----|-------|
| RP2040 Board | 1 | Pico or clone |
| SSD1306 OLED 128×64 | 1 | I2C interface, white/yellow/blue |
| Momentary Push Button | 5 | 6×6mm or larger |
| USB Micro-B Cable | 1 | Data cable (not power-only) |
| PCB or Breadboard | 1 | For wiring |

### Optional (Future)
| Part | Qty | Purpose |
|------|-----|---------|
| ATECC608A | 1 | Secure key storage |
| LED 3mm | 1 | Status indicator |
| Resistor 220Ω | 1 | LED current limit |
| Buzzer 5V | 1 | Audio feedback |

### Tools
- USB to Micro-USB cable
- Wire (26 AWG recommended)
- Breadboard or PCB
- Soldering iron (for PCB version)
