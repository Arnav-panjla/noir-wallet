# Quick Start Guide - Noir Wallet

Get your hardware wallet running in 5 minutes.

## What You Need

- RP2040 board (Shrike-lite, Raspberry Pi Pico, or clone)
- OLED display (SSD1306, 128x64)
- 5 push buttons
- USB cable

## Hardware Wiring

### OLED Display (I2C)
```
OLED GND   → GND
OLED VCC   → 3V3
OLED SDA   → GPIO 4
OLED SCL   → GPIO 5
```

### Push Buttons (GPIO, active low)
```
UP     → GPIO 16
DOWN   → GPIO 17
LEFT   → GPIO 18
RIGHT  → GPIO 19
CENTER → GPIO 20

All buttons: other pin → GND (with 10kΩ pull-up to 3V3)
```

## Installation

### 1. Flash MicroPython
```bash
# Connect RP2040 in bootloader mode (hold BOOT, plug in)
# Download firmware
wget https://micropython.org/resources/firmware/rp2-pico-latest.uf2
# Copy to device (appears as USB drive)
cp rp2-pico-latest.uf2 /mnt/RPI-RP2/
```

### 2. Copy Wallet Code

**Via Thonny IDE (Easiest):**
1. Open Thonny
2. Select RP2040 as interpreter (Tools → Options)
3. Open file browser (View → Files)
4. Drag all `.py` files from this project to the device

**Via Command Line:**
```bash
# Install ampy
pip install adafruit-ampy

# Upload all files
for f in *.py; do ampy --port /dev/ttyACM0 put $f; done
```

### 3. Test

Connect USB and press CENTER button. You should see:
- OLED splash screen
- Wallet address displayed
- Ready for signing

## First Transaction

### MetaMask Test Flow

1. **Web Page** (runs JavaScript):
```javascript
// Request public key
const response = await navigator.hid.requestDevice({filters: [{vendorId: 0x2e8a}]});
const device = response[0];
await device.open();

// Send get_pubkey command
await device.sendReport(0, new Uint8Array([0x01, 0x00]));
const data = await device.receiveReport();
console.log('Public Key:', data);
```

2. **RP2040** receives command:
   - Displays wallet info
   - Returns public key over USB HID

3. **Web Page** sends transaction:
```javascript
const tx = {
  nonce: 5,
  gasPrice: '20000000000',
  gas: '21000',
  to: '0x742d35Cc6634C0532925a3b844Bc9e7595f42e55',
  value: '1000000000000000000',
  data: '0x',
  chainId: 1
};

// Send sign_tx command
const payload = JSON.stringify(tx);
const cmd = new Uint8Array([0x02, payload.length, ...new TextEncoder().encode(payload)]);
await device.sendReport(0, cmd);
```

4. **RP2040** shows transaction:
   - Displays recipient address
   - Shows transaction value
   - Asks for PIN
   - Asks for confirmation

5. **User**:
   - Enters 4-digit PIN with UP/DOWN/LEFT/CENTER buttons
   - Presses UP to confirm or DOWN to reject

6. **RP2040** returns signature:
```javascript
const sig = await device.receiveReport();
// sig contains {r, s, v} in JSON format
```

## PIN Entry

### Default PIN: `1234`

**Change PIN** (in `state_machine.py`):
```python
self.pin_code = "9876"  # Change this
```

**PIN Entry Controls:**
- **UP Button**: Next digit (0-9)
- **DOWN Button**: Previous digit (9-0)
- **LEFT Button**: Delete last digit
- **CENTER Button**: Confirm (after 4 digits)
- **Long CENTER**: Cancel

## Testing Without MetaMask

### Test Display
```python
from display import OLEDDisplay

disp = OLEDDisplay()
disp.show_status("Hello World!")
```

### Test Buttons
```python
from buttons import ButtonManager

buttons = ButtonManager()
while True:
    action = buttons.wait_for_button()
    print(f"Button: {action['button']}, Type: {action['type']}")
```

### Test Crypto
```python
from crypto import EthereumSigner

signer = EthereumSigner()
print(f"Address: {signer.get_address()}")

tx = {
    'nonce': 0, 'gasPrice': 20000000000, 'gas': 21000,
    'to': '0x742d35Cc6634C0532925a3b844Bc9e7595f42e55',
    'value': 1000000000000000000, 'data': '0x', 'chainId': 1
}
sig = signer.sign_transaction(tx)
print(f"Signature: {sig}")
```

## File Structure

```
noir-wallet/
├── core/main/
│   ├── boot.py           # Runs at startup
│   ├── main.py           # Main application
│   ├── config.py         # Configuration
│   ├── display.py        # OLED driver
│   ├── buttons.py        # Button handler
│   ├── state_machine.py  # State management
│   ├── crypto.py         # Ethereum signing
│   ├── hid_interface.py  # USB communication
│   ├── atecc608a.py      # Secure element (optional)
│   ├── tx_parser.py      # Transaction parsing
│   ├── tests.py          # Test suite
│   └── README.md         # Module documentation
├── THONNY_SETUP.md       # Thonny IDE setup
└── QUICK_START.md        # This file
```

## Common Issues

### OLED Not Showing
- Check I2C address: `i2c.scan()` should show `[0x3c]` or `[0x3d]`
- Check wiring (SDA/SCL, GND, VCC)
- Add pull-up resistors (4.7kΩ) to SDA and SCL

### Buttons Not Working
- Test with: `machine.Pin(16, machine.Pin.IN, machine.Pin.PULL_UP).value()`
- Should be `1` when not pressed, `0` when pressed
- Check button wiring

### Out of Memory
- Run: `import gc; gc.collect()`
- Delete unused files from device
- Reduce buffer sizes in `config.py`

### USB Not Detected
- Use data cable (not power-only)
- Try different USB port
- Restart Thonny

## Next Steps

1. ✓ Install & test basic functionality
2. → Test crypto signing
3. → Set up WebHID bridge
4. → Integrate with MetaMask
5. → Add ATECC608A support
6. → Implement firmware updates

## Security Notes

⚠️ **This is a demo. DO NOT use with real funds yet.**

Before production:
- [ ] Audit crypto code
- [ ] Test with ATECC608A
- [ ] Implement secure PIN storage
- [ ] Add transaction verification
- [ ] Security audit by expert

## Resources

- **MicroPython**: https://micropython.org
- **RP2040 Docs**: https://datasheets.raspberrypi.com
- **Thonny IDE**: https://thonny.org
- **Ethereum Signing**: https://ethereum.org/en/developers/

## Support

For issues:
1. Check Thonny shell output (View → Shell)
2. Run `tests.py` to verify modules
3. Enable debug logging in `main.py`
4. Check hardware connections

Good luck! 🚀
