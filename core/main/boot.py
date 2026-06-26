import machine
import sys

def init_board():
    print("Noir Wallet - Initializing RP2040...")

    machine.freq(125_000_000)

    try:
        import micropython
        micropython.alloc_emergency_exception_buf(100)
    except:
        pass

    print("Board initialized")

def mount_filesystem():
    try:
        import os
        import json

        if not "config.json" in os.listdir():
            default_config = {
                "pin": "1234",
                "chain_id": 1,
                "max_attempts": 3,
                "display_timeout": 30
            }
            with open("config.json", "w") as f:
                json.dump(default_config, f)
        print("Filesystem ready")
    except Exception as e:
        print(f"Filesystem error: {e}")

def main():
    init_board()
    mount_filesystem()

    try:
        from main import main as wallet_main
        wallet_main()
    except Exception as e:
        print(f"Error starting wallet: {e}")
        import traceback
        traceback.print_exc()

if __name__ == "__main__":
    main()
