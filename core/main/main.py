import machine
import time
import gc

try:
    import ssd1306
    from display import OLEDDisplay
    from buttons import ButtonManager
    from state_machine import WalletState
    from crypto import EthereumSigner
    from hid_interface import HIDInterface
    from tx_parser import TransactionParser
except ImportError as e:
    print(f"Import error: {e}")
    print("Missing modules:")
    print("  - ssd1306.py")
    print("  - display.py")
    print("  - buttons.py")
    print("  - state_machine.py")
    print("  - crypto.py")
    print("  - hid_interface.py")
    print("  - tx_parser.py")

class NoirWallet:
    def __init__(self):
        self.display = None
        self.buttons = None
        self.state = None
        self.signer = None
        self.hid = None
        self.ready = False

        try:
            print("Initializing OLED display...")
            self.display = OLEDDisplay()
            print("✓ Display initialized")
        except Exception as e:
            print(f"✗ Display error: {e}")
            return

        try:
            print("Initializing buttons...")
            self.buttons = ButtonManager()
            print("✓ Buttons initialized")
        except Exception as e:
            print(f"✗ Button error: {e}")
            return

        try:
            print("Initializing crypto...")
            self.signer = EthereumSigner()
            print("✓ Crypto initialized")
        except Exception as e:
            print(f"✗ Crypto error: {e}")
            return

        try:
            print("Initializing state machine...")
            self.state = WalletState()
            print("✓ State machine initialized")
        except Exception as e:
            print(f"✗ State error: {e}")
            return

        try:
            print("Initializing HID...")
            self.hid = HIDInterface()
            print("✓ HID initialized")
        except Exception as e:
            print(f"✗ HID error: {e}")

        self.ready = True
        print("✓ All systems initialized")

    def init(self):
        if not self.display:
            print("Display not initialized, skipping UI")
            return

        try:
            self.display.show_splash()
            time.sleep(2)

            if self.signer:
                pubkey_info = self.signer.get_public_key()
                address = pubkey_info.get('address', 'unknown')

                self.display.clear()
                self.display.oled.text("Address:", 5, 10)

                addr_display = address[2:10] if len(address) > 2 else address
                self.display.oled.text(addr_display, 5, 25)

                self.display.oled.text("Press CENTER", 15, 50)
                self.display.oled.text("to continue", 15, 60)
                self.display.oled.show()
            else:
                self.display.show_status("Crypto failed")

        except Exception as e:
            print(f"Init error: {e}")
            try:
                self.display.show_error(f"Init failed")
            except:
                print(f"Display error: {e}")

    def run(self):
        if not self.ready:
            return

        idle_counter = 0
        menu_visible = False

        while True:
            try:
                self.handle_hid_input()
                action = self.handle_button_input()

                if action and action['button'] == 'center' and action['type'] == 'press':
                    if not menu_visible:
                        self.show_main_menu()
                        menu_visible = True
                    else:
                        menu_visible = False
                        self.display.show_status("Ready")

                idle_counter += 1
                if idle_counter > 1000:
                    gc.collect()
                    idle_counter = 0

                time.sleep(0.05)

            except Exception as e:
                print(f"Runtime error: {e}")
                self.display.show_error(f"Error: {str(e)[:20]}")
                time.sleep(2)

    def show_main_menu(self):
        options = [
            "Status",
            "Settings",
            "About",
            "Back"
        ]
        self.display.show_menu(options)

    def handle_button_input(self):
        action = self.buttons.get_action()
        if action and self.state:
            try:
                self.state.handle_action(action, self.display)
            except Exception as e:
                print(f"Button handler error: {e}")
        return action

    def handle_hid_input(self):
        try:
            cmd = self.hid.get_command()
            if cmd:
                self.process_command(cmd)
        except Exception as e:
            print(f"HID input error: {e}")

    def process_command(self, cmd):
        try:
            if cmd['type'] == 'get_pubkey':
                pubkey = self.signer.get_public_key()
                self.hid.send_response(pubkey)
                self.display.show_success("PubKey sent")

            elif cmd['type'] == 'sign_tx':
                tx_data = cmd.get('tx_data', {})
                parsed_tx = TransactionParser.parse(tx_data)

                valid, errors = TransactionParser.validate_tx(parsed_tx)
                if not valid:
                    self.hid.send_error(f"Invalid TX: {errors[0]}")
                    return

                self.state.set_pending_tx(parsed_tx)
                display_data = TransactionParser.format_for_display(parsed_tx)
                self.display.show_transaction(display_data)

                if self.state.wait_for_confirmation(timeout=30):
                    if self.state.verify_pin():
                        signature = self.signer.sign_transaction(parsed_tx)
                        self.hid.send_response(signature)
                        self.display.show_success("Signed!")
                    else:
                        self.hid.send_error("PIN verification failed")
                        self.display.show_error("Wrong PIN")
                else:
                    self.hid.send_error("Transaction rejected by user")
                    self.display.show_status("Rejected")

        except Exception as e:
            print(f"Command processing error: {e}")
            self.hid.send_error(f"Processing error: {str(e)}")

def main():
    try:
        print("Starting Noir Wallet...")
        wallet = NoirWallet()

        if wallet.ready:
            wallet.init()
            wallet.run()
        else:
            print("Wallet initialization failed")

    except KeyboardInterrupt:
        print("Wallet stopped by user")
    except Exception as e:
        print(f"Fatal error: {e}")
        import traceback
        traceback.print_exc()

if __name__ == "__main__":
    main()
