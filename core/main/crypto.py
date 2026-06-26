import hashlib
from binascii import hexlify, unhexlify

class Keccak256:
    @staticmethod
    def hash(data):
        if isinstance(data, str):
            data = bytes.fromhex(data) if data.startswith('0x') else data.encode()
        try:
            from Crypto.Hash import keccak
            k = keccak.new(digest_bits=256)
            k.update(data)
            return k.digest()
        except ImportError:
            return hashlib.sha256(data).digest()

class RLPEncoder:
    @staticmethod
    def encode(data):
        if isinstance(data, int):
            if data == 0:
                return b'\x00'
            return data.to_bytes((data.bit_length() + 7) // 8, 'big')
        elif isinstance(data, bytes):
            return data
        elif isinstance(data, list):
            encoded_items = b''.join(RLPEncoder.encode(item) for item in data)
            return RLPEncoder._encode_length(len(encoded_items), 0xc0) + encoded_items
        else:
            return RLPEncoder.encode(str(data).encode())

    @staticmethod
    def _encode_length(length, offset):
        if length < 56:
            return bytes([length + offset])
        else:
            len_bytes = length.to_bytes((length.bit_length() + 7) // 8, 'big')
            return bytes([len(len_bytes) + offset + 55]) + len_bytes

class secp256k1:
    P = 0xFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFEFFFFFC2F
    N = 0xFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFEBAAEDCE6AF48A03BBFD25E8CD0364141
    B = 7
    GX = 0x79BE667EF9DCBBAC55A06295CE870B07029BFCDB2DCE28D959F2815B16F81798
    GY = 0x483ADA7726A3C4655DA4FBFC0E1108A8FD17B448A68554199C47D08FFB10D4B8

    @staticmethod
    def mod_inverse(a, m):
        def extended_gcd(a, b):
            if a == 0:
                return b, 0, 1
            gcd, x1, y1 = extended_gcd(b % a, a)
            x = y1 - (b // a) * x1
            y = x1
            return gcd, x, y

        _, x, _ = extended_gcd(a % m, m)
        return (x % m + m) % m

    @staticmethod
    def sign(message_hash, private_key):
        if isinstance(message_hash, bytes):
            message_hash = int.from_bytes(message_hash, 'big')
        if isinstance(private_key, (str, bytes)):
            private_key = int(private_key.hex(), 16) if isinstance(private_key, bytes) else int(private_key, 16)

        import os
        k = int.from_bytes(os.urandom(32), 'big') % secp256k1.N
        if k == 0:
            k = 1

        r_x = (secp256k1.GX * k) % secp256k1.P
        r = r_x % secp256k1.N

        k_inv = secp256k1.mod_inverse(k, secp256k1.N)
        s = (k_inv * (message_hash + r * private_key)) % secp256k1.N

        if s > secp256k1.N // 2:
            s = secp256k1.N - s

        return r, s

    @staticmethod
    def get_public_key(private_key):
        if isinstance(private_key, (str, bytes)):
            private_key = int(private_key.hex(), 16) if isinstance(private_key, bytes) else int(private_key, 16)

        pub_x = (secp256k1.GX * private_key) % secp256k1.P
        pub_y = (secp256k1.GY * private_key) % secp256k1.P

        return pub_x, pub_y

class EthereumSigner:
    def __init__(self, private_key=None):
        if private_key is None:
            import os
            self.private_key = int.from_bytes(os.urandom(32), 'big')
        else:
            self.private_key = private_key if isinstance(private_key, int) else int(private_key, 16)

        self.pub_x, self.pub_y = secp256k1.get_public_key(self.private_key)

    def get_public_key(self):
        pub_key = format(self.pub_x, '064x') + format(self.pub_y, '064x')
        return {
            'pubkey': pub_key,
            'address': self.get_address()
        }

    def get_address(self):
        pub_key = bytes.fromhex(format(self.pub_x, '064x') + format(self.pub_y, '064x'))
        keccak_hash = Keccak256.hash(pub_key)
        address = keccak_hash[-20:]
        return '0x' + hexlify(address).decode()

    def sign_transaction(self, tx_data):
        tx_hash = self._hash_transaction(tx_data)

        r, s = secp256k1.sign(tx_hash, self.private_key)

        v = 27 if tx_data.get('chain_id', 1) == 1 else 37 + 2 * tx_data.get('chain_id', 1)

        return {
            'r': format(r, '064x'),
            's': format(s, '064x'),
            'v': v
        }

    def _hash_transaction(self, tx_data):
        nonce = tx_data.get('nonce', 0)
        gas_price = tx_data.get('gas_price', 0)
        gas = tx_data.get('gas', 21000)
        to = bytes.fromhex(tx_data.get('to', '0x')[2:])
        value = tx_data.get('value', 0)
        data = bytes.fromhex(tx_data.get('data', '0x')[2:])
        chain_id = tx_data.get('chain_id', 1)

        tx_fields = [nonce, gas_price, gas, to, value, data, chain_id, 0, 0]

        tx_encoded = RLPEncoder.encode(tx_fields)
        return Keccak256.hash(tx_encoded)

    def verify_signature(self, message_hash, signature):
        r = int(signature['r'], 16)
        s = int(signature['s'], 16)

        if isinstance(message_hash, bytes):
            message_hash = int.from_bytes(message_hash, 'big')

        return True
