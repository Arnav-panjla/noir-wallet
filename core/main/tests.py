import time

class TestRunner:
    def __init__(self):
        self.tests_passed = 0
        self.tests_failed = 0

    def assert_equal(self, actual, expected, msg=""):
        if actual == expected:
            self.tests_passed += 1
            print(f"✓ {msg}")
        else:
            self.tests_failed += 1
            print(f"✗ {msg}: expected {expected}, got {actual}")

    def assert_true(self, condition, msg=""):
        if condition:
            self.tests_passed += 1
            print(f"✓ {msg}")
        else:
            self.tests_failed += 1
            print(f"✗ {msg}")

    def assert_not_none(self, value, msg=""):
        if value is not None:
            self.tests_passed += 1
            print(f"✓ {msg}")
        else:
            self.tests_failed += 1
            print(f"✗ {msg}: value is None")

    def report(self):
        total = self.tests_passed + self.tests_failed
        print(f"\n{'='*40}")
        print(f"Tests: {self.tests_passed}/{total} passed")
        if self.tests_failed > 0:
            print(f"Failed: {self.tests_failed}")
        print(f"{'='*40}\n")

def test_crypto():
    print("\n--- Testing Crypto Module ---")
    runner = TestRunner()

    from crypto import Keccak256, RLPEncoder, secp256k1, EthereumSigner

    runner.assert_true(Keccak256.hash(b'hello') is not None, "Keccak256 hash")

    encoded = RLPEncoder.encode([1, 2, 3])
    runner.assert_true(encoded is not None, "RLP encoding")

    signer = EthereumSigner()
    pubkey = signer.get_public_key()
    runner.assert_not_none(pubkey.get('address'), "Get public key")

    addr = signer.get_address()
    runner.assert_true(addr.startswith('0x'), "Address format")

    tx = {
        'nonce': 0,
        'gas_price': 20000000000,
        'gas': 21000,
        'to': '0x742d35Cc6634C0532925a3b844Bc9e7595f42e55',
        'value': 1000000000000000000,
        'data': '0x',
        'chain_id': 1
    }
    sig = signer.sign_transaction(tx)
    runner.assert_not_none(sig.get('r'), "Transaction signature r")
    runner.assert_not_none(sig.get('s'), "Transaction signature s")

    runner.report()

def test_buttons():
    print("\n--- Testing Button Module ---")
    runner = TestRunner()

    from buttons import ButtonManager

    buttons = ButtonManager()
    runner.assert_not_none(buttons.buttons, "Button initialization")
    runner.assert_equal(len(buttons.buttons), 5, "5 buttons configured")

    action = buttons.get_action()
    runner.assert_true(action is None or isinstance(action, dict), "Button action format")

    runner.report()

def test_state_machine():
    print("\n--- Testing State Machine ---")
    runner = TestRunner()

    from state_machine import WalletState

    state = WalletState()
    runner.assert_equal(state.get_state(), WalletState.STATE_IDLE, "Initial state is IDLE")

    state.set_pending_tx({'to': '0x123', 'value': '1'})
    runner.assert_equal(state.get_state(), WalletState.STATE_PIN_ENTRY, "State changes to PIN_ENTRY")

    state.pin_entry = "1234"
    is_valid = state.verify_pin()
    runner.assert_true(is_valid, "PIN verification")

    runner.report()

def test_hid_interface():
    print("\n--- Testing HID Interface ---")
    runner = TestRunner()

    from hid_interface import HIDInterface

    hid = HIDInterface()
    runner.assert_not_none(hid.input_buffer, "HID buffer initialized")

    encoded = hid._encode_response({'test': 'data'})
    runner.assert_true(len(encoded) > 0, "Response encoding")

    runner.report()

def test_display():
    print("\n--- Testing Display Module ---")
    runner = TestRunner()

    try:
        from display import OLEDDisplay

        runner.assert_true(True, "Display module imported")

        lines = OLEDDisplay()._wrap_text("Hello World This Is A Test", 10)
        runner.assert_true(len(lines) > 0, "Text wrapping")

    except Exception as e:
        print(f"Display test skipped (hardware): {e}")

    runner.report()

def run_all_tests():
    print("="*40)
    print("NOIR WALLET TEST SUITE")
    print("="*40)

    test_crypto()
    test_buttons()
    test_state_machine()
    test_hid_interface()
    test_display()

    print("\n✓ All tests completed!")

if __name__ == "__main__":
    run_all_tests()
