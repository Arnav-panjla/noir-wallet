import machine
import time
from ssd1306 import SSD1306_I2C

class OLEDDisplay:
    def __init__(self, i2c_id=0, sda_pin=4, scl_pin=5, addr=0x3c):
        self.i2c = None
        self.oled = None
        self.width = 128
        self.height = 64

        self._init_i2c(i2c_id, sda_pin, scl_pin, addr)

    def _init_i2c(self, i2c_id, sda_pin, scl_pin, addr):
        try:
            print(f"Initializing I2C{i2c_id} with SDA=GP{sda_pin}, SCL=GP{scl_pin}")
            self.i2c = machine.I2C(i2c_id, scl=machine.Pin(scl_pin), sda=machine.Pin(sda_pin), freq=100000)
            print(f"I2C initialized, scanning for OLED at 0x{addr:02x}...")

            devices = self.i2c.scan()
            print(f"Found devices: {[hex(d) for d in devices]}")

            if addr not in devices:
                if devices:
                    print(f"OLED not at 0x{addr:02x}, trying {hex(devices[0])}")
                    addr = devices[0]
                else:
                    raise Exception("No I2C devices found! Check wiring.")

            print(f"Initializing SSD1306 at address 0x{addr:02x}")
            self.oled = SSD1306_I2C(128, 64, self.i2c, addr=addr)
            print("✓ OLED initialized successfully")

        except Exception as e:
            print(f"✗ OLED init error: {e}")
            self.oled = None
            raise

    def clear(self):
        self.oled.fill(0)
        self.show()

    def show(self):
        self.oled.show()

    def show_splash(self):
        self.clear()
        self.oled.text("NOIR WALLET", 20, 10)
        self.oled.text("v1.0", 45, 25)
        self.oled.text("RP2040", 40, 40)
        self.oled.text("Initializing...", 15, 55)
        self.show()

    def show_status(self, text):
        self.clear()
        self.oled.text(text, 10, 28)
        self.show()

    def show_menu(self, options, selected_idx=0):
        self.clear()
        self.oled.text("MENU", 50, 5)
        self.oled.hline(0, 15, 128, 1)

        for i, option in enumerate(options[:4]):
            y = 20 + (i * 10)
            prefix = ">" if i == selected_idx else " "
            text = f"{prefix} {option}"[:16]
            self.oled.text(text, 5, y)

        self.show()

    def show_pin_entry(self, digits_entered, max_digits=4):
        self.clear()
        self.oled.text("PIN ENTRY", 35, 5)
        self.oled.hline(0, 15, 128, 1)

        mask = "*" * len(digits_entered) + "_" * (max_digits - len(digits_entered))
        self.oled.text(mask, 40, 30)

        self.oled.text("Press to confirm", 10, 55)
        self.show()

    def show_transaction(self, tx_data):
        self.clear()
        self.oled.text("REVIEW TX", 30, 5)
        self.oled.hline(0, 15, 128, 1)

        to_addr = tx_data.get('to', '0x...')[:10] + "..."
        value = str(tx_data.get('value', '0'))[:10]
        gas = str(tx_data.get('gas', '21000'))[:8]

        self.oled.text("To: " + to_addr, 5, 20)
        self.oled.text("Val: " + value, 5, 35)
        self.oled.text("Gas: " + gas, 5, 50)

        self.show()

    def show_confirmation_prompt(self, text="Confirm?"):
        self.clear()
        self.oled.text(text, 25, 25)
        self.oled.text("UP=Yes  DOWN=No", 10, 50)
        self.show()

    def show_error(self, error_msg):
        self.clear()
        self.oled.text("ERROR", 45, 10)
        self.oled.hline(0, 20, 128, 1)
        lines = self._wrap_text(error_msg, 20)
        for i, line in enumerate(lines[:3]):
            text = line[:20]
            self.oled.text(text, 5, 25 + (i * 12))
        self.show()

    def show_success(self, msg="Success!"):
        self.clear()
        msg = msg[:20]
        self.oled.text(msg, 20, 28)
        self.show()

    def _wrap_text(self, text, width):
        words = text.split(' ')
        lines = []
        current_line = ""
        for word in words:
            if len(current_line) + len(word) <= width:
                current_line += word + " "
            else:
                if current_line:
                    lines.append(current_line.strip())
                current_line = word + " "
        if current_line:
            lines.append(current_line.strip())
        return lines

    def show_waiting(self, msg="Waiting..."):
        self.clear()
        msg = msg[:20]
        self.oled.text(msg, 25, 28)
        self.show()

    def scroll_text(self, text):
        self.clear()
        for x in range(128, -len(text) * 6, -1):
            self.oled.fill(0)
            self.oled.text(text, x, 28)
            self.show()
            time.sleep(0.05)
