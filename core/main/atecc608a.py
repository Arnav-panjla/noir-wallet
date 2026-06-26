import machine
import time

class ATECC608A:
    SLOT_PRIVATE_KEY = 0
    SLOT_PUBLIC_KEY = 1
    SLOT_CONFIG = 5

    CMD_INFO = 0x00
    CMD_SIGN = 0x64
    CMD_GENKEY = 0x40
    CMD_READ = 0x02
    CMD_WRITE = 0x12
    CMD_LOCK = 0x17

    def __init__(self, i2c_id=1, sda_pin=6, scl_pin=7, addr=0x60):
        self.i2c = machine.I2C(i2c_id, scl=machine.Pin(scl_pin), sda=machine.Pin(sda_pin), freq=100000)
        self.addr = addr
        self.is_configured = False

    def read_public_key(self, slot=SLOT_PUBLIC_KEY):
        try:
            data = self._send_command(self.CMD_READ, [slot])
            return data
        except Exception as e:
            return None

    def generate_key_pair(self, slot=SLOT_PRIVATE_KEY):
        try:
            response = self._send_command(self.CMD_GENKEY, [0x04, slot])
            return response
        except Exception as e:
            return None

    def sign_digest(self, digest, slot=SLOT_PRIVATE_KEY):
        try:
            payload = [slot] + list(digest)
            response = self._send_command(self.CMD_SIGN, payload)
            return response
        except Exception as e:
            return None

    def get_info(self):
        try:
            response = self._send_command(self.CMD_INFO, [])
            return response
        except Exception as e:
            return None

    def lock_zone(self, zone):
        try:
            response = self._send_command(self.CMD_LOCK, [zone])
            return response
        except Exception as e:
            return None

    def _send_command(self, cmd_type, payload):
        packet = self._build_packet(cmd_type, payload)
        self.i2c.writeto(self.addr, packet)
        time.sleep(0.1)
        response = self.i2c.readfrom(self.addr, 64)
        return self._parse_response(response)

    def _build_packet(self, cmd_type, payload):
        cmd_data = bytes([cmd_type] + payload)
        length = len(cmd_data) + 1
        crc = self._crc16(bytes([length]) + cmd_data)
        return bytes([0x03, length]) + cmd_data + crc.to_bytes(2, 'little')

    def _parse_response(self, data):
        if len(data) < 4:
            raise Exception("Invalid response")

        length = data[0]
        status = data[1]
        payload = data[2:2 + length - 3]
        crc = int.from_bytes(data[2 + length - 3:2 + length - 1], 'little')

        if status != 0x00:
            raise Exception(f"ATECC608A error: {status}")

        return payload

    def _crc16(self, data):
        crc = 0
        for byte in data:
            crc ^= byte
            for _ in range(8):
                if crc & 0x0001:
                    crc = (crc >> 1) ^ 0x8408
                else:
                    crc >>= 1
        return crc

class SecureKeyManager:
    def __init__(self, secure_element=None):
        self.se = secure_element or ATECC608A()
        self.key_slot = ATECC608A.SLOT_PRIVATE_KEY

    def initialize(self):
        info = self.se.get_info()
        if info:
            self.is_configured = True
            return True
        return False

    def generate_wallet_key(self):
        try:
            pubkey = self.se.generate_key_pair(self.key_slot)
            return pubkey
        except:
            return None

    def sign_with_se(self, message_hash):
        try:
            signature = self.se.sign_digest(message_hash, self.key_slot)
            return signature
        except:
            return None

    def is_initialized(self):
        return self.is_configured
