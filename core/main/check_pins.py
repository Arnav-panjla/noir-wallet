import machine

def check_all_pins():
    print("="*50)
    print("RP2040 PIN AVAILABILITY CHECK")
    print("="*50)

    reserved_pins = {
        25: "LED_BUILTIN",
        23: "GND",
        24: "GND",
        29: "GND",
        34: "GND",
        38: "GND",
        40: "GND",
        39: "VSYS",
        36: "3V3",
        37: "3V3",
    }

    print("\nTesting GPIO pins 0-29...\n")

    available = []
    bad = []

    for pin_num in range(30):
        if pin_num in reserved_pins:
            print(f"GP{pin_num:2d}: RESERVED ({reserved_pins[pin_num]})")
            continue

        try:
            pin = machine.Pin(pin_num, machine.Pin.IN, machine.Pin.PULL_UP)
            val = pin.value()
            print(f"GP{pin_num:2d}: ✓ OK (value={val})")
            available.append(pin_num)
        except Exception as e:
            print(f"GP{pin_num:2d}: ✗ ERROR - {e}")
            bad.append((pin_num, str(e)))

    print(f"\n{'='*50}")
    print(f"Summary:")
    print(f"  Available: {len(available)} pins")
    print(f"  Bad/Reserved: {30 - len(available)} pins")

    if bad:
        print(f"\nBad pins:")
        for pin, err in bad:
            print(f"  GP{pin}: {err}")

    print(f"\n{'='*50}")
    print("Recommended I2C pins (need 2 pins with pullups):")
    print(f"\nDefault (if available): GP4 (SDA), GP5 (SCL)")

    if 4 in available and 5 in available:
        print("  ✓ Default pins are available")
    else:
        if 4 not in available:
            print("  ✗ GP4 not available")
        if 5 not in available:
            print("  ✗ GP5 not available")
        print(f"\nAvailable pairs:")
        pairs = [
            (0, 1), (6, 7), (8, 9), (10, 11), (12, 13),
            (14, 15), (16, 17), (18, 19), (20, 21)
        ]
        for sda, scl in pairs:
            if sda in available and scl in available:
                print(f"  • GP{sda} (SDA), GP{scl} (SCL)")

    print("\nButton pins (need 5 pins for buttons):")
    button_pins = [16, 17, 18, 19, 20]
    button_status = []
    for pin in button_pins:
        status = "✓" if pin in available else "✗"
        button_status.append(f"{status} GP{pin}")
    for s in button_status:
        print(f"  {s}")

    return available

def test_i2c(sda_pin, scl_pin):
    print(f"\n{'='*50}")
    print(f"Testing I2C with SDA=GP{sda_pin}, SCL=GP{scl_pin}")
    print(f"{'='*50}\n")

    try:
        print(f"Creating I2C0...")
        i2c = machine.I2C(0, scl=machine.Pin(scl_pin), sda=machine.Pin(sda_pin), freq=100000)
        print(f"✓ I2C initialized")

        print(f"Scanning for devices...")
        devices = i2c.scan()

        if devices:
            print(f"✓ Found {len(devices)} device(s):")
            for addr in devices:
                print(f"  • 0x{addr:02x}")
                if addr in [0x3c, 0x3d]:
                    print(f"    ↳ This looks like an OLED!")
        else:
            print(f"✗ No devices found on I2C bus")
            print(f"  Check:")
            print(f"  1. Is OLED powered? (LED should be on)")
            print(f"  2. Wiring correct?")
            print(f"  3. Pull-up resistors present? (4.7k on SDA/SCL)")

    except Exception as e:
        print(f"✗ I2C Error: {e}")
        print(f"  This usually means:")
        print(f"  - Pin {sda_pin} or {scl_pin} is not available")
        print(f"  - Pin is already in use")
        print(f"  - Bad pin configuration")

def main():
    print("Checking all pins...\n")
    available = check_all_pins()

    if 4 in available and 5 in available:
        test_i2c(4, 5)
    elif available:
        # Try first available pair
        for i in range(0, len(available)-1, 2):
            test_i2c(available[i], available[i+1])
            break

if __name__ == "__main__":
    main()
