import time

class WalletState:
    STATE_IDLE = "idle"
    STATE_PIN_ENTRY = "pin_entry"
    STATE_CONFIRM_TX = "confirm_tx"
    STATE_SIGNING = "signing"

    def __init__(self):
        self.state = self.STATE_IDLE
        self.pending_tx = None
        self.pin_entry = ""
        self.pin_code = "1234"
        self.pin_attempts = 0
        self.max_pin_attempts = 3
        self.confirmation_result = None
        self.selected_idx = 0

    def handle_action(self, action, display):
        button = action['button']
        action_type = action['type']

        if self.state == self.STATE_IDLE:
            self._handle_idle(button, action_type, display)

        elif self.state == self.STATE_PIN_ENTRY:
            self._handle_pin_entry(button, action_type, display)

        elif self.state == self.STATE_CONFIRM_TX:
            self._handle_confirm_tx(button, action_type, display)

    def _handle_idle(self, button, action_type, display):
        if action_type == 'press' and button == 'center':
            display.show_menu(["Sign TX", "Settings", "Info", "Shutdown"])

    def _handle_pin_entry(self, button, action_type, display):
        if action_type == 'press':
            if button == 'up':
                self.pin_entry = self._increment_digit(self.pin_entry)
                display.show_pin_entry(self.pin_entry)

            elif button == 'down':
                self.pin_entry = self._decrement_digit(self.pin_entry)
                display.show_pin_entry(self.pin_entry)

            elif button == 'left':
                if len(self.pin_entry) > 0:
                    self.pin_entry = self.pin_entry[:-1]
                display.show_pin_entry(self.pin_entry)

            elif button == 'center':
                if len(self.pin_entry) == 4:
                    self.confirmation_result = (self.pin_entry == self.pin_code)
                    self.state = self.STATE_CONFIRM_TX
                else:
                    display.show_error("PIN must be 4 digits")
                    time.sleep(1)
                    display.show_pin_entry(self.pin_entry)

        elif action_type == 'long_press' and button == 'center':
            self.state = self.STATE_IDLE
            self.pin_entry = ""
            display.show_status("Cancelled")
            time.sleep(1)

    def _handle_confirm_tx(self, button, action_type, display):
        if action_type == 'press':
            if button == 'up':
                self.confirmation_result = True
                self.state = self.STATE_SIGNING

            elif button == 'down':
                self.confirmation_result = False
                self.state = self.STATE_IDLE
                self.pending_tx = None

    def _increment_digit(self, current):
        if len(current) < 4:
            next_val = int(current[-1]) + 1 if current else 0
            if next_val > 9:
                next_val = 0
            return current[:-1] + str(next_val) if current else str(next_val)
        return current

    def _decrement_digit(self, current):
        if current:
            next_val = int(current[-1]) - 1
            if next_val < 0:
                next_val = 9
            return current[:-1] + str(next_val)
        return current

    def set_pending_tx(self, tx_data):
        self.pending_tx = tx_data
        self.state = self.STATE_PIN_ENTRY
        self.pin_entry = ""
        self.pin_attempts = 0

    def wait_for_confirmation(self, timeout=30):
        start_time = time.time()
        while time.time() - start_time < timeout:
            if self.state == self.STATE_SIGNING:
                return self.confirmation_result is True
            if self.state == self.STATE_IDLE:
                return False
            time.sleep(0.1)
        return False

    def verify_pin(self):
        return self.pin_entry == self.pin_code

    def get_state(self):
        return self.state
