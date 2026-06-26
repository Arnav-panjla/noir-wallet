from binascii import hexlify, unhexlify

class TransactionParser:
    @staticmethod
    def parse(tx_json):
        tx = {
            'nonce': tx_json.get('nonce', '0x0'),
            'gasPrice': tx_json.get('gasPrice', '0x0'),
            'gas': tx_json.get('gas', '0x5208'),
            'to': tx_json.get('to', ''),
            'value': tx_json.get('value', '0x0'),
            'data': tx_json.get('data', '0x'),
            'chainId': tx_json.get('chainId', 1),
        }

        return TransactionParser._normalize_tx(tx)

    @staticmethod
    def _normalize_tx(tx):
        normalized = {}

        for key in ['nonce', 'gasPrice', 'gas', 'value', 'chainId']:
            val = tx.get(key, 0)
            if isinstance(val, str):
                if val.startswith('0x'):
                    normalized[key] = int(val, 16)
                else:
                    normalized[key] = int(val)
            else:
                normalized[key] = val

        normalized['to'] = TransactionParser._normalize_address(tx.get('to', ''))
        normalized['data'] = TransactionParser._normalize_hex(tx.get('data', '0x'))

        return normalized

    @staticmethod
    def _normalize_address(addr):
        if isinstance(addr, str):
            if addr.startswith('0x'):
                return addr.lower()
            return '0x' + addr.lower()
        return addr

    @staticmethod
    def _normalize_hex(data):
        if isinstance(data, str):
            if data.startswith('0x'):
                return data.lower()
            return '0x' + data.lower()
        return data

    @staticmethod
    def format_for_display(tx):
        to_addr = tx.get('to', '0x...')
        if len(to_addr) > 10:
            to_addr = to_addr[:6] + '...' + to_addr[-4:]

        value = tx.get('value', 0)
        if isinstance(value, int):
            eth_value = value / (10**18)
            value_str = f"{eth_value:.4f}"
        else:
            value_str = str(value)

        gas = tx.get('gas', 21000)
        gas_price = tx.get('gasPrice', 0)

        if isinstance(gas, int) and isinstance(gas_price, int):
            fee_wei = gas * gas_price
            fee_eth = fee_wei / (10**18)
            fee_str = f"{fee_eth:.6f}"
        else:
            fee_str = "0.0"

        return {
            'to': to_addr,
            'value': value_str,
            'gas': str(gas),
            'fee': fee_str,
        }

    @staticmethod
    def validate_tx(tx):
        errors = []

        if not tx.get('to'):
            errors.append("Missing recipient address")

        if 'nonce' not in tx:
            errors.append("Missing nonce")

        if 'gas' not in tx or tx['gas'] < 21000:
            errors.append("Invalid gas amount")

        if 'data' in tx and not isinstance(tx['data'], (str, bytes)):
            errors.append("Invalid data format")

        return len(errors) == 0, errors

class AddressValidator:
    @staticmethod
    def is_valid(address):
        if not isinstance(address, str):
            return False

        if not address.startswith('0x'):
            return False

        if len(address) != 42:
            return False

        try:
            int(address, 16)
            return True
        except ValueError:
            return False

    @staticmethod
    def checksum_address(address):
        if not AddressValidator.is_valid(address):
            return address

        addr = address[2:]
        hash_obj = __import__('hashlib').sha256(addr.lower().encode()).digest()
        checksummed = '0x'

        for i, char in enumerate(addr):
            if char in '0123456789':
                checksummed += char
            else:
                hash_val = int(hash_obj[i // 2].to_bytes(1, 'big'), 16)
                if i % 2 == 0:
                    hash_val >>= 4
                if (hash_val & 0xf) >= 8:
                    checksummed += char.upper()
                else:
                    checksummed += char.lower()

        return checksummed

class GasEstimator:
    BASE_GAS = 21000

    @staticmethod
    def estimate_gas(tx_data):
        gas = GasEstimator.BASE_GAS

        data = tx_data.get('data', '0x')
        if isinstance(data, str) and data.startswith('0x'):
            data = data[2:]
        else:
            data = str(data)

        for i in range(0, len(data), 2):
            byte_val = int(data[i:i+2], 16)
            if byte_val == 0:
                gas += 4
            else:
                gas += 16

        return gas

    @staticmethod
    def format_gas_fee(gas, gas_price):
        total_wei = gas * gas_price
        total_eth = total_wei / (10**18)
        return f"{total_eth:.6f} ETH"
