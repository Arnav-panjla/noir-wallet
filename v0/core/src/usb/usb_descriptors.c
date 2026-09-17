/* NOIR wallet — USB descriptors.
 *
 * Emulates a Ledger Nano S Plus (VID 0x2C97, PID 0x5011) so stock MetaMask
 * auto-detects the device. Compile-time flag NOIR_HONEST_VID switches to a
 * pidcodes VID (0x1209:0x4E4B) for anything beyond personal use — see the
 * hardware-wallet-dev skill for the USB-IF collision rationale.
 */

#include "tusb.h"
#include "usb_descriptors.h"
#include <string.h>

/* ---- Device descriptor ----------------------------------------- */
tusb_desc_device_t const desc_device = {
    .bLength            = sizeof(tusb_desc_device_t),
    .bDescriptorType    = TUSB_DESC_DEVICE,
    .bcdUSB             = 0x0200,
    .bDeviceClass       = 0x00,
    .bDeviceSubClass    = 0x00,
    .bDeviceProtocol    = 0x00,
    .bMaxPacketSize0    = CFG_TUD_ENDPOINT0_SIZE,
#ifdef NOIR_HONEST_VID
    .idVendor           = 0x1209,   /* pidcodes.net */
    .idProduct          = 0x4E4B,
#else
    .idVendor           = 0x2C97,   /* Ledger */
    .idProduct          = 0x5011,   /* Nano S Plus */
#endif
    .bcdDevice          = 0x0100,
    .iManufacturer      = 0x01,
    .iProduct           = 0x02,
    .iSerialNumber      = 0x03,
    .bNumConfigurations = 0x01,
};

/* ---- HID report descriptor ------------------------------------- */
/* 64-byte vendor report, report ID 0, both input and output — matches the
 * Ledger HID transport's fixed 64-byte frames. */
uint8_t const desc_hid_report[] = {
    TUD_HID_REPORT_DESC_GENERIC_INOUT(CFG_TUD_HID_EP_BUFSIZE)
};

/* ---- Configuration descriptor ---------------------------------- */
uint8_t const desc_configuration[] = {
    TUD_CONFIG_DESCRIPTOR(1, ITF_NUM_TOTAL, 0, CONFIG_TOTAL_LEN, 0x00, 100),
    TUD_HID_INOUT_DESCRIPTOR(ITF_NUM_HID, 0, HID_ITF_PROTOCOL_NONE,
                             sizeof(desc_hid_report),
                             EPNUM_HID_OUT, EPNUM_HID,
                             CFG_TUD_HID_EP_BUFSIZE, 1),
};

/* ---- String descriptors ---------------------------------------- */
static const char *const string_desc_arr[] = {
    /* 0 = langid, handled specially */
    (const char[]){0x09, 0x04},
    "Ledger",          /* 1: iManufacturer */
    "Nano S Plus",     /* 2: iProduct */
    "NOIR0001",        /* 3: iSerialNumber */
};

enum {
    STRID_LANGID       = 0,
    STRID_MANUFACTURER = 1,
    STRID_PRODUCT      = 2,
    STRID_SERIAL       = 3,
};

static uint16_t _desc_str[32];

uint8_t const *tud_descriptor_device_cb(void) {
    return (uint8_t const *)&desc_device;
}

uint8_t const *tud_descriptor_configuration_cb(uint8_t index) {
    (void)index;
    return desc_configuration;
}

uint8_t const *tud_hid_descriptor_report_cb(uint8_t instance) {
    (void)instance;
    return desc_hid_report;
}

uint16_t const *tud_descriptor_string_cb(uint8_t index, uint16_t langid) {
    (void)langid;
    size_t chr_count;

    if (index == STRID_LANGID) {
        _desc_str[1] = 0x0409;              /* English (US) */
        chr_count = 1;
    } else {
        if (index >= sizeof(string_desc_arr) / sizeof(string_desc_arr[0])) {
            return NULL;
        }
        const char *str = string_desc_arr[index];
        chr_count = strlen(str);
        if (chr_count > 31) chr_count = 31; /* fit _desc_str buffer */
        for (size_t i = 0; i < chr_count; i++) {
            _desc_str[1 + i] = (uint16_t)(uint8_t)str[i];
        }
    }

    _desc_str[0] = (uint16_t)((TUSB_DESC_STRING << 8) | (2 * chr_count));
    return _desc_str;
}
