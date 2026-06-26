import machine
import time

class ButtonManager:
    def __init__(self):
        self.buttons = {
            'up': machine.Pin(16, machine.Pin.IN, machine.Pin.PULL_UP),
            'down': machine.Pin(17, machine.Pin.IN, machine.Pin.PULL_UP),
            'left': machine.Pin(18, machine.Pin.IN, machine.Pin.PULL_UP),
            'right': machine.Pin(19, machine.Pin.IN, machine.Pin.PULL_UP),
            'center': machine.Pin(20, machine.Pin.IN, machine.Pin.PULL_UP),
        }

        self.debounce_time = 50
        self.long_press_time = 1000
        self.press_start = {}
        self.last_action_time = {}

        for name in self.buttons:
            self.press_start[name] = 0
            self.last_action_time[name] = 0

    def get_action(self):
        current_time = time.ticks_ms()

        for name, btn in self.buttons.items():
            is_pressed = btn.value() == 0

            if is_pressed:
                if self.press_start[name] == 0:
                    self.press_start[name] = current_time

                hold_time = current_time - self.press_start[name]

                if hold_time > self.long_press_time:
                    if current_time - self.last_action_time[name] > self.long_press_time + 200:
                        self.last_action_time[name] = current_time
                        return {'type': 'long_press', 'button': name}

            else:
                if self.press_start[name] != 0:
                    hold_time = current_time - self.press_start[name]

                    if hold_time >= self.debounce_time and hold_time < self.long_press_time:
                        if current_time - self.last_action_time[name] > self.debounce_time:
                            self.last_action_time[name] = current_time
                            return {'type': 'press', 'button': name}

                    self.press_start[name] = 0

        return None

    def wait_for_button(self, timeout_ms=None):
        start_time = time.ticks_ms()
        while True:
            action = self.get_action()
            if action:
                return action

            if timeout_ms and time.ticks_diff(time.ticks_ms(), start_time) > timeout_ms:
                return None

            time.sleep(0.01)
