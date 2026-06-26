import json
import time
from collections import deque

class HIDInterface:
    CMD_GET_PUBKEY = 0x01
    CMD_SIGN_TX = 0x02
    CMD_VERIFY_PIN = 0x03
    CMD_GET_STATUS = 0x04

    def __init__(self):
        self.input_buffer = deque(maxlen=10)
        self.output_buffer = deque(maxlen=10)
        self._init_usb()

    def _init_usb(self):
        try:
            import usb_device
            from usb_device import get_serial_instance
            self.usb = get_serial_instance()
        except ImportError:
            self.usb = None

    def get_command(self):
        if not self.usb:
            return None

        try:
            data = self.usb.read(64)
            if data:
                return self._parse_command(data)
        except:
            pass

        return None

    def _parse_command(self, data):
        if len(data) < 2:
            return None

        cmd_type = data[0]
        payload_len = data[1]
        payload = data[2:2 + payload_len]

        if cmd_type == self.CMD_GET_PUBKEY:
            return {'type': 'get_pubkey'}

        elif cmd_type == self.CMD_SIGN_TX:
            try:
                tx_data = json.loads(payload.decode())
                return {'type': 'sign_tx', 'tx_data': tx_data}
            except:
                return None

        elif cmd_type == self.CMD_VERIFY_PIN:
            pin = payload.decode()
            return {'type': 'verify_pin', 'pin': pin}

        elif cmd_type == self.CMD_GET_STATUS:
            return {'type': 'get_status'}

        return None

    def send_response(self, data):
        response = self._encode_response(data)
        if self.usb:
            try:
                self.usb.write(response)
            except:
                pass

    def send_error(self, error_msg):
        response = self._encode_error(error_msg)
        if self.usb:
            try:
                self.usb.write(response)
            except:
                pass

    def _encode_response(self, data):
        payload = json.dumps(data).encode()
        length = len(payload)
        return bytes([0xFF, length]) + payload

    def _encode_error(self, error_msg):
        payload = json.dumps({'error': error_msg}).encode()
        length = len(payload)
        return bytes([0xFE, length]) + payload

class SimpleUSBSerial:
    def __init__(self):
        self.rx_buffer = b''
        self.tx_buffer = b''

    def read(self, size):
        if len(self.rx_buffer) > 0:
            data = self.rx_buffer[:size]
            self.rx_buffer = self.rx_buffer[size:]
            return data
        return b''

    def write(self, data):
        self.tx_buffer += data
        return len(data)

    def any(self):
        return len(self.rx_buffer) > 0
