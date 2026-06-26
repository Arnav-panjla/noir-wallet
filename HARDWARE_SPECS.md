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
| Resistor 10kΩ | 5 | Button pull-ups |
| Resistor 4.7kΩ | 2 | I2C pull-ups (optional if on OLED) |
| Capacitor 0.1µF | 2 | OLED decoupling |
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

## Pin Configuration

### RP2040 Pin Layout

```
        ┌─────────────────────────────────────┐
        │         RASPBERRY PI PICO           │
        │                                     │
   1    │  GP0       ← UART TX           VBUS │  40
   2    │  GP1       ← UART RX          VSYS  │  39
   3    │  GND       ← GND               GND   │  38
   4    │  GP2                           3V3   │  37
   5    │  GP3                           3V3   │  36
   6    │  GP4       → OLED SDA          ADC   │  35
   7    │  GP5       → OLED SCL         (GND)  │  34
   8    │  GND       ← GND              GP28   │  33
   9    │  GP6       → SE SDA (opt)     GP27   │  32
  10    │  GP7       → SE SCL (opt)     GP26   │  31
  11    │  GP8                          GP22   │  30
  12    │  GP9                           GND   │  29
  13    │  GND       ← GND              GP21   │  28
  14    │  GP10                         GP20   │  27  ← CENTER BUTTON
  15    │  GP11                         GP19   │  26  ← RIGHT BUTTON
  16    │  GP12                         GP18   │  25  ← LEFT BUTTON
  17    │  GP13                         GP17   │  24  ← DOWN BUTTON
  18    │  GND       ← GND              GP16   │  23  ← UP BUTTON
        │                                     │
        │  Note: Pins 1, 3, 8, 13, 18, 23,  │
        │        29, 34, 38, 40 are GND       │
        └─────────────────────────────────────┘
```

### Detailed Pin Mapping

#### I2C Interface 0 (Display)
```
GP4  → SDA (OLED)
GP5  → SCL (OLED)
GND  → GND (OLED)
3V3  → VCC (OLED)
```

#### I2C Interface 1 (Secure Element - Optional)
```
GP6  → SDA (ATECC608A)
GP7  → SCL (ATECC608A)
GND  → GND (ATECC608A)
3V3  → VCC (ATECC608A)
```

#### GPIO (Buttons)
```
GP16 → UP Button
GP17 → DOWN Button
GP18 → LEFT Button
GP19 → RIGHT Button
GP20 → CENTER Button
GND  → Button Commons
```

#### Optional
```
GP25 → On-board LED (diagnostic)
```

## Wiring Diagrams

### OLED Display (SSD1306)

```
    RP2040              SSD1306 OLED
    
    3V3  ─────────────► VCC
    GP4  ─────────────► SDA
    GP5  ─────────────► SCL
    GND  ─────────────► GND
```

**I2C Pull-ups:**
- If OLED doesn't have built-in pull-ups, add 4.7kΩ from:
  - SDA to 3V3
  - SCL to 3V3

### Push Buttons

```
    RP2040              Button
    
    GP16 ─────[10kΩ]──► 3V3
             │
             └─ Button ─┬─ GND
    
    (Repeat for GP17, GP18, GP19, GP20)
```

**Button Configuration:**
- One side: GPIO pin
- Other side: GND (via button press)
- Pull-up to 3V3 (10kΩ)
- Active LOW (pressed = 0V)

### Complete Breadboard Layout

```
        ┌──────────────────────┐
        │     RP2040 Pico      │
        │                      │
    3V3 │●─────────┬───────────│ ← 4.7kΩ pull-ups to:
        │          │           │    - SDA (GP4)
   GP4  │●─────────┤─[SDA]─────│    - SCL (GP5)
   GP5  │●─────────┤─[SCL]─────│
    GND │●─────────┼─[GND]─────│
        │          │           │
   GP16 │●─[10kΩ]──┼─[UP]──────│ ← 5× Buttons to GND
   GP17 │●─[10kΩ]──┼─[DOWN]────│
   GP18 │●─[10kΩ]──┼─[LEFT]────│
   GP19 │●─[10kΩ]──┼─[RIGHT]───│
   GP20 │●─[10kΩ]──┼─[CENTER]──│
   GND  │●─────────┴───[GND]────│
        │                      │
        │    +─────────────+   │
        │    │ SSD1306     │   │
        │    │ OLED 128×64 │   │
        │    │ GND  VCC    │   │
        │    │ SDA  SCL    │   │
        │    +─────────────+   │
        │          ││││        │
        └──────────┼│││────────┘
                   ││││
                   │││└─ GND
                   ││└── 3V3
                   │└─── SDA (GP4)
                   └──── SCL (GP5)
```

## OLED Connector Options

### Type A: 4-Pin Header
```
Pin 1: GND
Pin 2: VCC (3V3)
Pin 3: SCL (GP5)
Pin 4: SDA (GP4)
```

### Type B: 7-Pin Header (with extras)
```
Pin 1: GND
Pin 2: VCC
Pin 3: SCL
Pin 4: SDA
Pin 5: NC
Pin 6: NC
Pin 7: NC
```

**Check your specific module** - pinout varies by manufacturer.

## Secure Element (ATECC608A) - Optional

### ATECC608A Pinout
```
Pin 1: GND
Pin 2: SDA (GP6)
Pin 3: SCL (GP7)
Pin 4: VCC (3V3)
```

**Note:** Optional module, not required for basic operation.

## Button PCB Layout

For custom PCB, button matrix:

```
         ┌─────────────────────┐
         │   5 Push Buttons    │
         │                     │
    UP   │ ●   ●   ●   ●   ●  │
  DOWN   │ ●   ●   ●   ●   ●  │
  LEFT   │ ●   ●   ●   ●   ●  │
 RIGHT   │ ●   ●   ●   ●   ●  │
 CENTER  │ ●   ●   ●   ●   ●  │
         │                     │
         └─────────────────────┘

Center button at middle position
4 directional buttons around it
```

## Assembly

### Step 1: Prepare Board
1. Insert RP2040 into breadboard (center)
2. Insert OLED module
3. Strip and prepare wires

### Step 2: Power
1. Connect GND from RP2040 to OLED GND
2. Connect 3V3 from RP2040 to OLED VCC
3. Connect GND to button common line

### Step 3: I2C
1. Connect GP4 (SDA) to OLED SDA
2. Connect GP5 (SCL) to OLED SCL
3. Add 4.7kΩ pull-ups if needed

### Step 4: Buttons
1. Connect 10kΩ resistors from each GPIO to 3V3
2. Connect buttons from GPIO to GND (common line)
3. Test each button with `machine.Pin(n).value()`

### Step 5: Verify
```python
# In Thonny shell
import machine

# Check I2C
i2c = machine.I2C(0, scl=machine.Pin(5), sda=machine.Pin(4))
print(i2c.scan())  # Should show [60] for OLED

# Check buttons
for pin in [16, 17, 18, 19, 20]:
    print(f"GPIO {pin}: {machine.Pin(pin, machine.Pin.IN, machine.Pin.PULL_UP).value()}")
```

## Power Budget

### Current Consumption
| Component | Current | Notes |
|-----------|---------|-------|
| RP2040 | 30-80 mA | Depends on CPU load |
| OLED | 10-20 mA | Backlight always on |
| Buttons | <1 mA | No power |
| Total | ~50-100 mA | Typical operation |

### Power Supply
- **USB Power**: Sufficient for development/testing
- **Battery**: 5V USB power bank (~2A)
- **Supply Voltage**: 5V USB or 3.3V regulated

### RP2040 Voltage
- USB: 5V (auto-regulated to 3.3V)
- GPIO: 3.3V (not 5V tolerant)
- OLED: 3.3V-5V (check module specs)

## Troubleshooting Hardware

### OLED Not Showing
1. Check voltage: GND=0V, VCC=3.3V
2. Check I2C address: `i2c.scan()` → should show [60] or [61]
3. Verify SDA/SCL connections
4. Check pull-up resistors

### Buttons Not Working
1. Press button, check voltage drops to 0V
2. Verify pull-up resistors present
3. Test GPIO: `machine.Pin(16, machine.Pin.IN, machine.Pin.PULL_UP).value()`
   - Should be 1 (unpressed), 0 (pressed)

### I2C Communication Issues
1. Add capacitors (0.1µF) near VCC on OLED
2. Check cable length (short = better)
3. Try different I2C address
4. Reduce clock frequency

## Safety Precautions

⚠️ **Important:**
1. **Never** connect 5V to GPIO pins (except through pull-ups)
2. **Always** use pull-up resistors on buttons
3. **Always** use correct orientation on buttons (short/long leg)
4. **Check** polarity before powering on
5. **Disconnect** USB before rewiring

## Cost Breakdown

### Minimum Setup (~$10)
- RP2040: $4
- OLED 128×64: $3
- 5× Buttons: $1
- Wires, resistors: $1
- Breadboard: $1

### Quality Setup (~$20)
- RP2040: $5
- OLED I2C module: $5
- 5× Cherry MX buttons: $3
- PCB (custom): $5
- Enclosure: $2

### With Secure Element (~$30)
- Everything above: $20
- ATECC608A breakout: $8
- Add 10 minutes wiring

## Alternative Hardware

### RP2040-Based Boards
- Raspberry Pi Pico: Official, well-documented
- Adafruit ItsyBitsy RP2040: Smaller form factor
- Makerdiary nRF5340 DK: More powerful (different MCU though)

### OLED Modules
- 128×64 I2C (white): Most common
- 128×64 SPI: Faster (needs GPIO)
- 128×32 I2C: Smaller (not recommended)
- 0.96" vs 1.3": Size difference

### Buttons
- 6×6mm momentary: Standard
- 12×12mm: Larger, easier to press
- Cherry MX: Mechanical, durable
- Capacitive: Touch-based (different circuit)

## Resources

- [RP2040 Datasheet](https://datasheets.raspberrypi.com/rp2040/rp2040-datasheet.pdf)
- [SSD1306 Datasheet](https://cdn-shop.adafruit.com/datasheets/SSD1306.pdf)
- [ATECC608A Datasheet](https://www.microchip.com/en-us/product/ATECC608A)
- [MicroPython Pin Reference](https://docs.micropython.org/en/latest/rp2/quickref.html)

---

**Hardware assembly: ~30 minutes**

**Ready? Start with QUICK_START.md!** 🔧
