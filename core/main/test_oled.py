import machine
import time

def test_i2c():
    print("Testing I2C connection...")
    try:
        i2c = machine.I2C(0, scl=machine.Pin(5), sda=machine.Pin(4), freq=400000)
        devices = i2c.scan()
        print(f"Found I2C devices at addresses: {[hex(x) for x in devices]}")

        if 0x3c in devices:
            print("✓ SSD1306 found at 0x3C")
            return i2c, 0x3c
        elif 0x3d in devices:
            print("✓ SSD1306 found at 0x3D")
            return i2c, 0x3d
        else:
            print("✗ SSD1306 not found")
            print("  Expected 0x3C or 0x3D")
            print("  Check I2C wiring!")
            return None, None
    except Exception as e:
        print(f"✗ I2C error: {e}")
        return None, None

def test_ssd1306(i2c, addr):
    print("\nTesting SSD1306 driver...")
    try:
        from ssd1306 import SSD1306_I2C
        oled = SSD1306_I2C(128, 64, i2c, addr=addr)
        print("✓ SSD1306 driver initialized")
        return oled
    except Exception as e:
        print(f"✗ SSD1306 error: {e}")
        return None

def test_display(oled):
    print("\nTesting display output...")
    try:
        oled.fill(0)
        oled.text("NOIR WALLET", 20, 10)
        oled.text("OLED TEST", 30, 25)
        oled.text("Success!", 40, 40)
        oled.hline(0, 50, 128, 1)
        oled.show()
        print("✓ Display test complete")
        print("  You should see text on OLED")
        return True
    except Exception as e:
        print(f"✗ Display error: {e}")
        return False

def test_display_module():
    print("\nTesting OLEDDisplay wrapper...")
    try:
        from display import OLEDDisplay
        display = OLEDDisplay()
        print("✓ OLEDDisplay initialized")

        display.show_splash()
        time.sleep(1)
        display.show_status("Test Complete")
        print("✓ Display wrapper works!")
        return True
    except Exception as e:
        print(f"✗ OLEDDisplay error: {e}")
        return False

def main():
    print("="*50)
    print("NOIR WALLET - OLED TEST")
    print("="*50)

    i2c, addr = test_i2c()

    if not i2c:
        print("\n✗ FAILED: Cannot communicate with OLED")
        print("\nTroubleshooting:")
        print("1. Check USB cable is plugged in")
        print("2. Verify OLED wiring:")
        print("   - GND to GND")
        print("   - VCC to 3V3")
        print("   - SDA to GPIO 4")
        print("   - SCL to GPIO 5")
        print("3. Check for 4.7k pullup resistors")
        print("4. Try different USB port")
        return False

    oled = test_ssd1306(i2c, addr)

    if not oled:
        print("\n✗ FAILED: Cannot initialize SSD1306")
        print("  Make sure ssd1306.py is uploaded to device")
        return False

    if not test_display(oled):
        print("\n✗ FAILED: Display output error")
        return False

    if not test_display_module():
        print("\n✗ FAILED: Display wrapper error")
        print("  Make sure display.py is uploaded to device")
        return False

    print("\n" + "="*50)
    print("✓ ALL OLED TESTS PASSED!")
    print("="*50)
    print("\nYour OLED is working correctly.")
    print("You can now run: import main; main.main()")
    return True

if __name__ == "__main__":
    main()
