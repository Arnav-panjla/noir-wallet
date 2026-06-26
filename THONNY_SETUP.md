# Thonny Setup Guide for Noir Wallet

This guide walks you through setting up the Noir Wallet firmware in Thonny IDE for the RP2040.

## Prerequisites

- Thonny IDE (latest version)
- RP2040 board (Shrike-lite or similar)
- USB cable (data cable, not power-only)
- MicroPython firmware for RP2040

## Step 1: Install Thonny

Download from [thonny.org](https://thonny.org)

## Step 2: Flash MicroPython

### Option A: Using Thonny
1. Connect RP2040 in bootloader mode (hold BOOT button while plugging in)
2. Go to **Tools → Options → Interpreter**
3. Select **MicroPython (RP2040)**
4. Thonny will prompt to install firmware automatically

### Option B: Manual Flash
```bash
# Download RP2040 firmware
wget https://micropython.org/resources/firmware/rp2-pico-latest.uf2

# Copy to USB mount (appears as RPI-RP2 when in bootloader)
cp rp2-pico-latest.uf2 /media/RPI-RP2/
```

## Step 3: Configure Thonny

1. Open Thonny
2. Go to **Tools → Options → Interpreter**
3. Select **MicroPython (RP2040)**
4. In **Port**, select your device (usually `/dev/ttyACM0` on Linux)
5. Click **OK**

## Step 4: Create Project Structure

In Thonny's file browser on the left, create this structure:

```
device/
├── boot.py
├── main.py
├── config.py
├── display.py
├── buttons.py
├── state_machine.py
├── crypto.py
├── hid_interface.py
├── atecc608a.py
├── tx_parser.py
├── tests.py
└── README.md
```

## Step 5: Upload Files

### Method A: Drag & Drop
1. Open **View → Files**
2. Left panel: your computer, Right panel: RP2040
3. Drag all `.py` files to the right panel

### Method B: Copy via Terminal
In Thonny's shell:
```python
import os
import shutil

files = [
    'boot.py', 'main.py', 'config.py', 'display.py',
    'buttons.py', 'state_machine.py', 'crypto.py',
    'hid_interface.py', 'atecc608a.py', 'tx_parser.py'
]

for f in files:
    with open(f, 'r') as src:
        content = src.read()
    with open(f'/device/{f}', 'w') as dst:
        dst.write(content)
```

## Step 6: Run the Firmware

### Option A: Auto-start
Files named `boot.py` run automatically on startup.

### Option B: Manual Start
In Thonny shell:
```python
import main
main.main()
```

### Option C: Test Suite
```python
import tests
tests.run_all_tests()
```

## Step 7: Monitor Serial Output

Open **View → Shell** to see debug output from the device.

### Common Output:
```
Noir Wallet - Initializing RP2040...
Board initialized
Filesystem ready
Starting Noir Wallet...
```

## Troubleshooting

### Device Not Found
```python
import machine
print(machine.Pin(25).value())  # Test onboard LED
```

### Display Not Working
```python
from display import OLEDDisplay
disp = OLEDDisplay()
disp.show_status("Test")  # Should show on OLED
```

### Button Not Responding
```python
from buttons import ButtonManager
buttons = ButtonManager()
while True:
    action = buttons.get_action()
    if action:
        print(action)
```

### Memory Issues
```python
import gc
gc.collect()
print(gc.mem_free())  # Check available memory
```

## Development Tips

### Fast Iteration
1. Edit code in left panel
2. Right-click file → **Send to device**
3. Run in shell: `exec(open('main.py').read())`

### Live Testing
```python
from display import OLEDDisplay
from buttons import ButtonManager

disp = OLEDDisplay()
buttons = ButtonManager()

# Test display
disp.show_status("Hello!")

# Wait for button
action = buttons.wait_for_button(timeout_ms=5000)
print(action)
```

### Debug Logging
Add to main.py for verbose output:
```python
import sys
sys.stdout = open('/dev/stdout', 'w')  # Enable serial output
```

## Flashing New Firmware

If you need to update MicroPython:
1. Enter bootloader mode (hold BOOT, press RESET)
2. Go to **Tools → Options → Interpreter**
3. Click **Install or update MicroPython**
4. Select latest version

## Next Steps

1. ✓ Firmware running
2. ✓ Display working
3. ✓ Buttons responding
4. → Test crypto signing
5. → Implement USB HID
6. → Integrate with MetaMask

## Testing Checklist

- [ ] Board detectable in Thonny
- [ ] OLED displays splash screen
- [ ] All 5 buttons respond
- [ ] Crypto tests pass
- [ ] No memory errors after 1 hour runtime
- [ ] USB HID enumeration works

## Resources

- [MicroPython Docs](https://docs.micropython.org)
- [RP2040 Datasheet](https://datasheets.raspberrypi.com/rp2040/rp2040-datasheet.pdf)
- [Thonny Docs](https://thonny.org)

## Tips for RP2040

### Limited Resources
- RAM: ~192 KB available for code
- Flash: ~2 MB available for code
- No floating-point hardware (software FP is slow)

### Optimization Tips
```python
# Pre-compile strings to save RAM
STATUS = "Ready"  # Not b"Ready" (bytes)

# Use generators for large loops
def large_loop():
    for i in range(10000):
        yield i  # Don't store all at once

# Reduce object creation
# Bad: [x*2 for x in range(100)]
# Good: for x in range(100): result = x*2
```

### Memory Profiling
```python
import gc
gc.collect()
print(f"Free: {gc.mem_free()} bytes")
print(f"Alloc: {gc.mem_alloc()} bytes")
```

Enjoy building with Noir Wallet! 🚀
