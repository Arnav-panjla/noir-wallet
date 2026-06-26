# Noir Wallet - RP2040 Firmware

MicroPython firmware for the Noir hardware wallet running on RP2040.

## Architecture

```
boot.py              - Initialization & board setup
main.py              - Main application loop
├── display.py       - SSD1306 OLED driver & UI
├── buttons.py       - 5-button input handler
├── state_machine.py - Wallet state management
├── crypto.py        - Ethereum signing & RLP
├── hid_interface.py - USB communication
├── atecc608a.py     - Secure element interface (ATECC608A)
└── config.py        - Configuration constants
```

## Hardware Connections

### Display (SSD1306 128x64 OLED)
- I2C ID: 0
- SDA: GPIO 4
- SCL: GPIO 5
- Address: 0x3C

### Buttons (5x Momentary Push)
- UP: GPIO 16
- DOWN: GPIO 17
- LEFT: GPIO 18
- RIGHT: GPIO 19
- CENTER: GPIO 20

### Secure Element (ATECC608A) - Optional
- I2C ID: 1
- SDA: GPIO 6
- SCL: GPIO 7
- Address: 0x60

## Installation

### 1. Flash MicroPython
```bash
esptool.py --chip rp2040 -p /dev/ttyUSB0 write_flash -z 0 firmware.uf2
```

Or drag `rp2040-uf2` file to USB bootloader.

### 2. Copy Files to Device
Use Thonny IDE or `ampy`:
```bash
# Via ampy
ampy --port /dev/ttyUSB0 put boot.py
ampy --port /dev/ttyUSB0 put main.py
ampy --port /dev/ttyUSB0 put display.py
# ... copy all other modules
```

## Usage

### PIN Entry
- **UP/DOWN**: Increment/decrement digit
- **LEFT**: Delete last digit
- **CENTER**: Confirm PIN
- **Long CENTER**: Cancel

### Transaction Confirmation
- **UP**: Approve transaction
- **DOWN**: Reject transaction

### Status Display
- Shows wallet address on boot
- Displays transaction details
- Shows signing progress

## Development

### Testing Display
```python
from display import OLEDDisplay
display = OLEDDisplay()
display.show_status("Test Message")
```

### Testing Buttons
```python
from buttons import ButtonManager
buttons = ButtonManager()
action = buttons.get_action()
print(action)  # {'type': 'press', 'button': 'up'}
```

### Testing Crypto
```python
from crypto import EthereumSigner
signer = EthereumSigner()
address = signer.get_address()
print(address)
```

## Security Features

1. **PIN Protection**: 4-digit PIN before signing
2. **Confirmation Display**: Shows transaction details before signing
3. **Hardware Random**: Uses RP2040 internal RNG
4. **Secure Element Ready**: ATECC608A slot for future key storage

## Integration with MetaMask

The wallet communicates via USB HID protocol:

```
Host → Device
[0x02] [LEN] [JSON_TX_DATA]

Device → Host
[0xFF] [LEN] [JSON_SIGNATURE]
```

## Future Enhancements

- [ ] ATECC608A key generation and signing
- [ ] Transaction RLP parsing improvements
- [ ] QR code display for addresses
- [ ] Firmware update over USB
- [ ] Multi-signature support
- [ ] Custom token display

## Troubleshooting

### Device not recognized in Thonny
- Check USB cable (data cable, not power-only)
- Try different USB port
- Restart Thonny

### Display not working
- Verify I2C address (0x3C or 0x3D)
- Check SDA/SCL pullups (4.7kΩ typical)
- Use `i2c.scan()` to detect address

### Button not responding
- Test with `buttons.wait_for_button()`
- Check GPIO pin configuration
- Verify button is pulled high

## License

MIT
