# Hardware Specifications - Noir Wallet

Complete hardware requirements and wiring guide.

## Board Requirements

### Primary MCU
- **STM32F401CCU6** (WeAct BlackPill)
- Cortex-M4 @ 84 MHz
- 1× I2C interface (shared bus: OLED + ATECC608A)
- 5+ GPIO pins
- 64 KB SRAM, 256 KB Flash
- 25 MHz HSE crystal
- USB support for HID (via TinyUSB, DFU upload)

**Recommended Boards:**
- WeAct STM32F401CCU6 BlackPill
- Pinout reference: [STM32F401CCU6 pinout diagram](./STM32-STM32F4-STM32F401-STM32F401CCU6-pinout-low-resolution.jpg)

## Components List

### Required
| Part | Qty | Notes |
|------|-----|-------|
| STM32F401CCU6 BlackPill | 1 | WeAct board |
| SSD1306 OLED 128×64 | 1 | I2C interface, white/yellow/blue |
| ATECC608A | 1 | Secure element — entropy + serial |
| Momentary Push Button | 5 | 6×6mm or larger |
| USB Micro-B Cable | 1 | Data cable (not power-only) |
| PCB or Breadboard | 1 | For wiring |

### Optional (Future)
| Part | Qty | Purpose |
|------|-----|---------|
| LED 3mm | 1 | Status indicator |
| Resistor 220Ω | 1 | LED current limit |
| Buzzer 5V | 1 | Audio feedback |

## Wiring

### I2C1 (shared bus: OLED + ATECC608A)
| Signal | Pin |
|--------|-----|
| SCL | PB6 |
| SDA | PB7 |
| OLED address | 0x3C |
| ATECC608A address | 0x60 |

### Buttons (active-low, pressed = GND)
| Button | Pin |
|--------|-----|
| LEFT | PA7 |
| RIGHT | PB0 |
| UP | PA6 |
| DOWN | PB10 |
| CENTER | PB1 |

> `PB2` is `BOOT1` and reads LOW at boot — do not use for a button.

Source of truth for pin assignments: [`core/src/platform/gpio_map.h`](../core/src/platform/gpio_map.h).

### Case
3D-printed enclosure: [`CAD/case_v2.stl`](../CAD/case_v2.stl) + [`CAD/front_pannel_v3.stl`](../CAD/front_pannel_v3.stl).

### Tools
- USB to Micro-USB cable
- Wire (26 AWG recommended)
- Breadboard or PCB
- Soldering iron (for PCB version)
