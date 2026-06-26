BOARD = "rp2040"

I2C_DISPLAY = {
    "id": 0,
    "sda": 4,
    "scl": 5,
    "freq": 400000,
    "addr": 0x3c,
}

I2C_SECURE_ELEMENT = {
    "id": 1,
    "sda": 6,
    "scl": 7,
    "freq": 100000,
    "addr": 0x60,
}

PINS = {
    "button_up": 16,
    "button_down": 17,
    "button_left": 18,
    "button_right": 19,
    "button_center": 20,
    "led_status": 25,
}

WALLET = {
    "pin_code": "1234",
    "chain_id": 1,
    "max_pin_attempts": 3,
    "pin_timeout": 30,
    "tx_timeout": 60,
}

DISPLAY = {
    "width": 128,
    "height": 64,
    "timeout": 30,
}

SECURITY = {
    "use_secure_element": False,
    "pin_required": True,
    "confirm_tx": True,
}
