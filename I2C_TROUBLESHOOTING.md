# I2C/OLED Troubleshooting Guide

Your RP2040 is having trouble communicating with the OLED display. Let's fix it!

## Quick Diagnosis

### Step 1: Check If Pins Are Available

Run this in Thonny shell:
```python
exec(open('check_pins.py').read())
```

This will:
- Test all GPIO pins
- Find which pins are available
- Suggest I2C pin pairs
- Test actual I2C communication

### Step 2: Find OLED I2C Address & Pins

Run this to scan all possible I2C configurations:
```python
exec(open('find_i2c.py').read())
```

This will:
- Try all I2C pin combinations
- Find your OLED's address
- Tell you exactly what to use in `config.py`

## Common Issues & Fixes

### Issue 1: "bad SCL pin"

**Cause**: GP5 (or your SCL pin) is not available

**Fix**:
1. Run `check_pins.py` to find available pins
2. Use a different pin pair for I2C
3. Update `config.py`:
   ```python
   I2C_DISPLAY = {
       "id": 0,
       "sda": 6,      # Change from 4
       "scl": 7,      # Change from 5
       "freq": 100000,
       "addr": 0x3c,
   }
   ```
4. Update `display.py` to use new pins:
   ```python
   def __init__(self, i2c_id=0, sda_pin=6, scl_pin=7, addr=0x3c):
   ```

### Issue 2: "no module named 'hmac'"

**Cause**: Removed (hmac not in MicroPython)

**Fix**: Already fixed in crypto.py ✓
- Just re-upload the latest `crypto.py`

### Issue 3: OLED Not Responding

**Symptoms**: 
- Scan finds no devices
- I2C initialization fails
- OLED LED is off or dim

**Fixes**:
1. **Check power**
   - Is OLED LED on?
   - Test with multimeter: should be 3.3V
   - Try different USB port

2. **Check wiring**
   - GND → GND (MUST be connected)
   - VCC → 3V3 (3.3V only, or 5V if module supports it)
   - SDA → GPIO pin (any pin from `check_pins.py`)
   - SCL → GPIO pin (any pin from `check_pins.py`)

3. **Add pull-up resistors**
   - If OLED doesn't have internal pullups, add:
   - 4.7kΩ from SDA to 3V3
   - 4.7kΩ from SCL to 3V3

4. **Try different I2C address**
   - Default: 0x3C
   - Alternative: 0x3D
   - Check with `find_i2c.py`

## Step-by-Step Recovery

### Phase 1: Diagnose

1. **Check USB connection**
   ```python
   import machine
   print("RP2040 is connected")
   ```

2. **Check pin availability**
   ```python
   exec(open('check_pins.py').read())
   ```
   - Look for available GPIO pins
   - Note which pins work

3. **Find OLED on I2C**
   ```python
   exec(open('find_i2c.py').read())
   ```
   - Shows exact pins and address to use

### Phase 2: Configure

If `find_i2c.py` found your OLED:

1. **Edit config.py**
   ```python
   I2C_DISPLAY = {
       "id": 0,           # From find_i2c.py output
       "sda": 6,          # From find_i2c.py output
       "scl": 7,          # From find_i2c.py output
       "freq": 100000,
       "addr": 0x3c,      # From find_i2c.py output
   }
   ```

2. **Edit display.py** - Update `__init__` method
   ```python
   def __init__(self, i2c_id=0, sda_pin=6, scl_pin=7, addr=0x3c):
   ```

3. **Upload both files to device**

### Phase 3: Test

1. **Test I2C directly**
   ```python
   from config import I2C_DISPLAY as cfg
   import machine
   
   i2c = machine.I2C(cfg["id"], 
                     scl=machine.Pin(cfg["scl"]),
                     sda=machine.Pin(cfg["sda"]),
                     freq=cfg["freq"])
   
   devices = i2c.scan()
   print(f"Devices found: {[hex(x) for x in devices]}")
   ```

2. **Test OLED driver**
   ```python
   from config import I2C_DISPLAY as cfg
   from ssd1306 import SSD1306_I2C
   import machine
   
   i2c = machine.I2C(cfg["id"],
                     scl=machine.Pin(cfg["scl"]),
                     sda=machine.Pin(cfg["sda"]),
                     freq=cfg["freq"])
   
   oled = SSD1306_I2C(128, 64, i2c, addr=cfg["addr"])
   oled.fill(0)
   oled.text("Success!", 40, 30)
   oled.show()
   ```

3. **Test display wrapper**
   ```python
   from display import OLEDDisplay
   
   display = OLEDDisplay()
   display.show_splash()
   ```

4. **Test full wallet**
   ```python
   import main
   main.main()
   ```

## Pin Reference

### RP2040 Safe Pins for I2C

**NOT SAFE** (reserved or problematic):
- GP23-29: GND
- GP30-39: Power/special
- GP40: GND

**SAFE for I2C** (try these):
```
I2C Option 1: SDA=GP0,  SCL=GP1
I2C Option 2: SDA=GP4,  SCL=GP5   (default, often not available)
I2C Option 3: SDA=GP6,  SCL=GP7   (usually safe)
I2C Option 4: SDA=GP8,  SCL=GP9
I2C Option 5: SDA=GP10, SCL=GP11
I2C Option 6: SDA=GP12, SCL=GP13
I2C Option 7: SDA=GP14, SCL=GP15
I2C Option 8: SDA=GP16, SCL=GP17
I2C Option 9: SDA=GP18, SCL=GP19
I2C Option 10: SDA=GP20, SCL=GP21
```

**SAFE for Buttons**:
- GP16, GP17, GP18, GP19, GP20 (default)
- Or any from "SAFE" list above

## Testing Checklist

- [ ] USB cable is plugged in (data cable, not power-only)
- [ ] OLED has power (LED is on)
- [ ] All wires connected (GND, VCC, SDA, SCL)
- [ ] Pull-up resistors in place (if needed)
- [ ] `check_pins.py` runs without errors
- [ ] `find_i2c.py` finds OLED device
- [ ] I2C test in Phase 3 Step 1 works
- [ ] OLED test in Phase 3 Step 2 shows text
- [ ] Display wrapper test works
- [ ] Full wallet starts successfully

## If Still Not Working

### Debug Script

Create a new file `debug_i2c.py`:
```python
import machine
import time

print("="*50)
print("I2C DEBUG")
print("="*50)

# Test different configurations
configs = [
    (4, 5), (0, 1), (6, 7), (8, 9), (10, 11),
    (12, 13), (14, 15), (16, 17), (18, 19), (20, 21)
]

for sda, scl in configs:
    try:
        print(f"\nTrying SDA=GP{sda}, SCL=GP{scl}")
        i2c = machine.I2C(0, scl=machine.Pin(scl), sda=machine.Pin(sda), freq=100000)
        devices = i2c.scan()
        if devices:
            print(f"  ✓ Found: {[hex(x) for x in devices]}")
            if 0x3c in devices or 0x3d in devices:
                print(f"  ✓✓ OLED FOUND! Use these pins!")
    except Exception as e:
        print(f"  ✗ {e}")

print("\n" + "="*50)
```

Run it:
```python
exec(open('debug_i2c.py').read())
```

### Hardware Verification

**With multimeter** (if available):
- GND: 0V
- 3V3: 3.3V
- SDA/SCL when idle: 3.3V (pulled high)
- SDA/SCL when in use: fluctuates between 0-3.3V

## Config.py Template

Once you find working pins, update `config.py`:

```python
I2C_DISPLAY = {
    "id": 0,                # Which I2C (0 or 1)
    "sda": 6,              # SDA pin (from find_i2c.py)
    "scl": 7,              # SCL pin (from find_i2c.py)
    "freq": 100000,        # I2C frequency
    "addr": 0x3c,          # OLED address (0x3c or 0x3d)
}

I2C_SECURE_ELEMENT = {
    "id": 1,               # Different I2C for SE (optional)
    "sda": 6,
    "scl": 7,
    "freq": 100000,
    "addr": 0x60,
}

PINS = {
    "button_up": 16,
    "button_down": 17,
    "button_left": 18,
    "button_right": 19,
    "button_center": 20,
    "led_status": 25,
}
```

## Getting Help

If you're still stuck:

1. **Share output of:**
   - `check_pins.py`
   - `find_i2c.py`
   - `debug_i2c.py`

2. **Check your OLED module:**
   - Model: (search for chip name + "datasheet")
   - Voltage: 3.3V, 5V, or both?
   - I2C address: 0x3C or 0x3D?
   - Pull-ups: Built-in or need external?

3. **Verify hardware:**
   - Is OLED plugged into breadboard correctly?
   - Are all wires secure?
   - Do other devices work on same I2C pins?

---

**Remember**: Most I2C issues are wiring, power, or pin conflicts. The diagnostic scripts will find the problem! 🔍
