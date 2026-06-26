import machine
import time

def find_i2c_pins():
    print("="*50)
    print("FINDING I2C PINS FOR OLED")
    print("="*50)

    pin_pairs = [
        (4, 5),    # Default (GP4=SDA, GP5=SCL)
        (0, 1),    # Alternative 1
        (6, 7),    # Alternative 2
        (8, 9),    # Alternative 3
        (10, 11),  # Alternative 4
        (12, 13),  # Alternative 5
        (14, 15),  # Alternative 6
        (16, 17),  # Alternative 7
        (18, 19),  # Alternative 8
        (20, 21),  # Alternative 9
        (26, 27),  # Alternative 10
    ]

    for i2c_id in [0, 1]:
        print(f"\nTesting I2C{i2c_id}...")

        for sda, scl in pin_pairs:
            try:
                i2c = machine.I2C(i2c_id, scl=machine.Pin(scl), sda=machine.Pin(sda), freq=100000)
                devices = i2c.scan()

                if devices:
                    print(f"  ✓ I2C{i2c_id} with SDA=GP{sda}, SCL=GP{scl} → Devices: {[hex(d) for d in devices]}")

                    for addr in devices:
                        if addr in [0x3c, 0x3d]:
                            print(f"    → FOUND OLED at 0x{addr:02x}!")
                            print(f"\nUse these settings in config.py:")
                            print(f'I2C_DISPLAY = {{')
                            print(f'    "id": {i2c_id},')
                            print(f'    "sda": {sda},')
                            print(f'    "scl": {scl},')
                            print(f'    "freq": 100000,')
                            print(f'    "addr": 0x{addr:02x},')
                            print(f'}}')
                            return i2c_id, sda, scl, addr

            except Exception as e:
                pass

    print("\n✗ No OLED found on any I2C pins!")
    print("\nTroubleshooting:")
    print("1. Check OLED is powered (LED should be on)")
    print("2. Check wiring:")
    print("   - GND to GND")
    print("   - VCC to 3V3 (or 5V if board supports it)")
    print("   - SDA to any GPIO pin")
    print("   - SCL to any GPIO pin")
    print("3. Try different pin pairs")
    print("4. Check pull-up resistors (4.7k on SDA and SCL)")
    print("5. Test with a multimeter that pins have 3.3V")

    return None, None, None, None

def main():
    result = find_i2c_pins()
    if result[0] is not None:
        print("\n✓ OLED configuration found!")
    else:
        print("\n✗ Could not find OLED")

if __name__ == "__main__":
    main()
