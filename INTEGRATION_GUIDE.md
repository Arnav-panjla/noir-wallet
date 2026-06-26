# MetaMask Integration Guide

This guide explains how to integrate Noir Wallet with MetaMask for transaction signing.

## Architecture Overview

```
┌─────────────────┐
│   MetaMask      │
│   (Browser)     │
└────────┬────────┘
         │ WebHID API
         │
┌────────▼────────┐
│  WebHID Bridge  │  (web/wallet-bridge.html)
│  (JavaScript)   │
└────────┬────────┘
         │ USB HID
         │
┌────────▼────────┐
│  RP2040 Firmware│  (core/main/*)
│  Noir Wallet    │
└─────────────────┘
```

## USB HID Protocol

### Device Identification
- **Vendor ID**: `0x2e8a` (Raspberry Pi Foundation)
- **Product ID**: Depends on firmware (check device descriptor)
- **Interface**: HID

### Command Format
```
[COMMAND_BYTE][PAYLOAD_LENGTH][PAYLOAD...]

COMMAND_BYTE:
  0x01 = Get Public Key
  0x02 = Sign Transaction
  0x03 = Verify PIN
  0x04 = Get Status
```

### Responses
```
[0xFF][LENGTH][JSON_RESPONSE...]   - Success
[0xFE][LENGTH][ERROR_MESSAGE...]   - Error
```

## Implementation Steps

### Step 1: Enable WebHID Permissions

In your web application, request HID access:

```javascript
const requestHID = async () => {
  try {
    const devices = await navigator.hid.requestDevice({
      filters: [{vendorId: 0x2e8a}]
    });
    if (devices.length > 0) {
      const device = devices[0];
      await device.open();
      return device;
    }
  } catch (error) {
    console.error('HID request failed:', error);
  }
};
```

### Step 2: Send Get Public Key Command

```javascript
const getPublicKey = async (device) => {
  const cmd = new Uint8Array([0x01, 0x00]);
  await device.sendReport(0, cmd);
  
  // Listen for response
  device.addEventListener('inputreport', (event) => {
    const response = JSON.parse(new TextDecoder().decode(event.data.buffer.slice(2)));
    console.log('Public Key:', response.pubkey);
    console.log('Address:', response.address);
  });
};
```

### Step 3: Create Transaction

```javascript
const createTransaction = async (device, to, value, gasPrice, gas, data = '0x') => {
  const tx = {
    nonce: 0,  // Get from MetaMask
    gasPrice: String(parseInt(gasPrice) * 1e9),
    gas: parseInt(gas),
    to: to.toLowerCase(),
    value: String(parseFloat(value) * 1e18),
    data: data,
    chainId: 1
  };
  
  return tx;
};
```

### Step 4: Sign Transaction

```javascript
const signTransaction = async (device, txData) => {
  const payload = JSON.stringify(txData);
  const cmd = new Uint8Array([
    0x02,  // Sign TX command
    payload.length,
    ...new TextEncoder().encode(payload)
  ]);
  
  await device.sendReport(0, cmd);
  
  return new Promise((resolve) => {
    device.addEventListener('inputreport', (event) => {
      const response = JSON.parse(
        new TextDecoder().decode(event.data.buffer.slice(2))
      );
      resolve(response);  // {r, s, v}
    });
  });
};
```

## MetaMask Custom Provider Integration

### Using MetaMask's SignTypedData

```javascript
class NoirWalletProvider {
  constructor(device) {
    this.device = device;
    this.account = null;
  }

  async getAccounts() {
    const pubkey = await this.getPublicKey();
    this.account = pubkey.address;
    return [this.account];
  }

  async signTransaction(tx) {
    const cleanTx = {
      nonce: tx.nonce,
      gasPrice: tx.gasPrice,
      gas: tx.gas,
      to: tx.to,
      value: tx.value,
      data: tx.data,
      chainId: 1
    };

    const signature = await this.sendSignCommand(cleanTx);
    
    // Combine signature
    const signedTx = {
      ...tx,
      r: '0x' + signature.r,
      s: '0x' + signature.s,
      v: signature.v
    };

    return signedTx;
  }

  async sendSignCommand(tx) {
    const payload = JSON.stringify(tx);
    const cmd = new Uint8Array([0x02, payload.length, ...new TextEncoder().encode(payload)]);
    await this.device.sendReport(0, cmd);

    return new Promise((resolve) => {
      const handler = (event) => {
        const response = JSON.parse(
          new TextDecoder().decode(event.data.buffer.slice(2))
        );
        this.device.removeEventListener('inputreport', handler);
        resolve(response);
      };
      this.device.addEventListener('inputreport', handler);
    });
  }
}
```

## Testing Checklist

### Basic HID Communication
- [ ] Device appears in `navigator.hid.getDevices()`
- [ ] Can open device
- [ ] Can send 0x04 (Get Status) command
- [ ] Receives valid response

### Public Key Retrieval
- [ ] Send 0x01 command
- [ ] Device shows splash screen
- [ ] Receive valid public key
- [ ] Address is checksummed

### Transaction Signing
- [ ] Send 0x02 with transaction
- [ ] Device shows transaction details on OLED
- [ ] OLED asks for PIN
- [ ] User enters PIN
- [ ] OLED asks for confirmation
- [ ] User confirms (UP) or rejects (DOWN)
- [ ] Device returns signature (r, s, v)
- [ ] Signature is valid for the transaction

## Security Considerations

### Before Production Use

1. **Audit Crypto Code**
   - Have secp256k1 implementation reviewed
   - Verify RLP encoding correctness
   - Test against known test vectors

2. **Test with ATECC608A**
   - Ensure secure key storage works
   - Verify signing through secure element
   - Test key generation

3. **Pin Verification**
   - Implement secure PIN storage (hashed, not plaintext)
   - Add attempt limits and timeouts
   - Consider rate limiting

4. **Transport Security**
   - Validate all input from host
   - Implement message authentication (MAC)
   - Add replay protection

5. **User Interface**
   - Clearly show transaction details
   - Warn about unusual values
   - Implement timeout for user action

## Troubleshooting

### Device Not Found
```javascript
const devices = await navigator.hid.getDevices();
console.log('Devices:', devices);
```

### No Response from Device
1. Check OLED is showing something
2. Verify USB cable is plugged in
3. Restart browser tab
4. Check device permissions in Thonny

### Wrong Signature
1. Verify transaction fields match
2. Check nonce is correct
3. Verify private key hasn't changed
4. Test with small transaction first

### PIN Entry Not Working
1. Press buttons physically on device
2. Check button GPIO pins
3. Verify debounce timing

## Performance

**Typical Timing:**
- Connect: ~500ms
- Get Public Key: ~1000ms
- Get Status: ~200ms
- Sign Transaction: ~2000ms (including user input)

**Bandwidth:**
- Average command: ~100 bytes
- Average response: ~200 bytes
- No streaming required

## Future Enhancements

### Short Term
- [ ] Multiple account support
- [ ] Message signing (EIP-191)
- [ ] Transaction previews with graphics
- [ ] QR code display of address

### Medium Term
- [ ] Multi-signature support
- [ ] EIP-712 typed data signing
- [ ] Hardware wallet standard (SLIP-0039)
- [ ] Firmware updates over USB

### Long Term
- [ ] ATECC608A integration
- [ ] Custom networks support
- [ ] DeFi transaction templates
- [ ] Cold storage/airgap operation

## Example Web App

A complete example is provided in `web/wallet-bridge.html`:

```bash
# Open in browser (requires HTTPS or localhost for WebHID)
python3 -m http.server 8000
# Then visit: https://localhost:8000/web/wallet-bridge.html
```

## References

- [WebHID API](https://wicg.github.io/webhid/)
- [Ethereum JSON RPC](https://ethereum.org/en/developers/docs/apis/json-rpc/)
- [MetaMask Docs](https://docs.metamask.io/)
- [secp256k1](https://en.bitcoin.it/wiki/Secp256k1)
- [EIP-191: Signed Data](https://eips.ethereum.org/EIPS/eip-191)

## Support

For integration issues:
1. Enable debug logging in device
2. Check browser console for errors
3. Verify HID protocol compliance
4. Test with wallet-bridge.html first
5. Enable verbose logging in firmware

Good luck integrating! 🚀
