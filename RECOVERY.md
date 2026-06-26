# Quick Recovery Guide

Your wallet had issues with OLED communication. Here's the fastest way to fix it.

## What We Fixed

1. ✅ **Removed `hmac` import** - Not in MicroPython
2. ✅ **Improved I2C detection** - Auto-finds your OLED
3. ✅ **Better error messages** - Shows exactly what's wrong
4. ✅ **Diagnostic tools** - Scripts to find your exact pins

## Files to Upload NOW

These are the **critical** files:

### Core Firmware (Upload ALL 13)
```
core/main/ssd1306.py         ← NEW (OLED driver)
core/main/check_pins.py      ← NEW (diagnostics)
core/main/find_i2c.py        ← NEW (find OLED)
core/main/boot.py
core/main/config.py
core/main/main.py            ← UPDATED
core/main/display.py         ← UPDATED (better I2C handling)
core/main/crypto.py          ← FIXED (removed hmac)
core/main/buttons.py
core/main/state_machine.py
core/main/hid_interface.py
core/main/tx_parser.py
core/main/atecc608a.py
```

Optional:
- `test_oled.py` - OLED testing
- `tests.py` - Unit tests

## Quick Fix Steps

### Step 1: Upload Files (5 min)
Use Thonny drag-drop:
1. View → Files (if not showing)
2. Select all `.py` files from `core/main/`
3. Drag to device (right panel)

### Step 2: Diagnose (2 min)
In Thonny shell:
```python
exec(open('check_pins.py').read())
```

Read the output carefully. It will show:
- Which pins work
- Which pins fail
- What to do next

### Step 3: Find OLED (2 min)
```python
exec(open('find_i2c.py').read())
```

It will output something like:
```
✓ I2C0 with SDA=GP6, SCL=GP7 → Devices: ['0x3c']
→ FOUND OLED at 0x3c!

Use these settings in config.py:
I2C_DISPLAY = {
    "id": 0,
    "sda": 6,
    "scl": 7,
    "freq": 100000,
    "addr": 0x3c,
}
```

**Copy these exact values!**

### Step 4: Update Config (2 min)
Edit `config.py` and replace I2C_DISPLAY:

```python
I2C_DISPLAY = {
    "id": 0,      # From find_i2c.py
    "sda": 6,     # From find_i2c.py
    "scl": 7,     # From find_i2c.py
    "freq": 100000,
    "addr": 0x3c, # From find_i2c.py
}
```

Upload to device.

### Step 5: Update Display.py (1 min)
Edit `display.py` line 7, change:
```python
def __init__(self, i2c_id=0, sda_pin=6, scl_pin=7, addr=0x3c):
```

Use the same values from Step 3!

Upload to device.

### Step 6: Test (1 min)
In Thonny shell:
```python
from display import OLEDDisplay
display = OLEDDisplay()
display.show_splash()
```

You should see text on OLED!

### Step 7: Run Wallet (1 min)
```python
import main
main.main()
```

Done! 🎉

## If Something's Still Wrong

Run this to debug:
```python
exec(open('check_pins.py').read())
```

Then:
```python
exec(open('find_i2c.py').read())
```

**Share the output** if you need help.

Common issues:
- **"bad SCL pin"** → GP5 not available, use find_i2c.py output
- **"no module hmac"** → Already fixed in new crypto.py
- **OLED not responding** → Check wiring, try find_i2c.py
- **Device not found** → OLED not powered or wired

## File Summary

### NEW Files (Do Upload)
- `ssd1306.py` - Custom OLED driver (280 lines)
- `check_pins.py` - Pin tester (100 lines)
- `find_i2c.py` - OLED finder (90 lines)
- `I2C_TROUBLESHOOTING.md` - Full guide

### UPDATED Files (Re-upload)
- `display.py` - Better I2C handling
- `crypto.py` - Removed hmac import
- `main.py` - Better error messages

### UNCHANGED (Still work as-is)
- `buttons.py`
- `state_machine.py`
- `tx_parser.py`
- `hid_interface.py`
- `atecc608a.py`
- `config.py` (but you'll edit it!)
- `boot.py`
- `tests.py`

## Testing Checklist

After each step, check:

- [ ] All 13 .py files uploaded
- [ ] `check_pins.py` runs successfully
- [ ] `find_i2c.py` finds your OLED
- [ ] `config.py` has correct I2C pins
- [ ] `display.py` has matching SDA/SCL pins
- [ ] OLED shows splash screen
- [ ] Buttons respond (try pressing them)
- [ ] Wallet starts: `import main; main.main()`

## Command Reference

```python
# Diagnose pins
exec(open('check_pins.py').read())

# Find OLED
exec(open('find_i2c.py').read())

# Test OLED directly
exec(open('test_oled.py').read())

# Run unit tests
import tests
tests.run_all_tests()

# Start wallet
import main
main.main()
```

## What's Different Now

**Before**: Hard-coded pins, no error messages, import error
```
✗ bad SCL pin
✗ no module named 'hmac'
✗ OLEDDisplay isn't defined
```

**After**: Auto-detect pins, helpful messages, all deps included
```
✓ I2C initialized
✓ Found OLED at 0x3C
✓ SSD1306 working
✓ Display initialized
✓ All systems ready
```

## Pin Examples

### Common Working Configurations

**Option 1** (if find_i2c.py finds this):
```python
I2C_DISPLAY = {
    "id": 0,
    "sda": 0,
    "scl": 1,
    "freq": 100000,
    "addr": 0x3c,
}
```

**Option 2**:
```python
I2C_DISPLAY = {
    "id": 0,
    "sda": 6,
    "scl": 7,
    "freq": 100000,
    "addr": 0x3c,
}
```

**Always use the output from `find_i2c.py`!**

## Next Steps After Recovery

1. ✓ OLED working
2. → Test buttons: Press all 5
3. → Test crypto: `from crypto import EthereumSigner`
4. → Test signing: `signer.sign_transaction({...})`
5. → Test MetaMask: Open `web/wallet-bridge.html`

## Emergency Recovery

If everything breaks:

1. **Delete all Python files from device**
2. **Reboot device** (soft reboot in Thonny)
3. **Upload fresh files** from `core/main/`
4. **Run `check_pins.py` and `find_i2c.py` again**
5. **Update config as shown**

## Still Stuck?

Follow **[I2C_TROUBLESHOOTING.md](I2C_TROUBLESHOOTING.md)** for detailed debugging.

Most issues are solved by:
1. Running `find_i2c.py`
2. Using its output in config.py
3. Re-uploading config.py + display.py

You've got this! 💪
