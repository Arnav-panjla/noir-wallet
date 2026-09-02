#ifndef USB_DESCRIPTORS_H
#define USB_DESCRIPTORS_H

#include "tusb.h"

/* Single HID interface (Ledger-style: bidirectional 64-byte reports). */
enum {
    ITF_NUM_HID = 0,
    ITF_NUM_TOTAL
};

#define EPNUM_HID     0x81   /* device -> host (IN)  */
#define EPNUM_HID_OUT 0x01   /* host  -> device (OUT) */

#define CONFIG_TOTAL_LEN (TUD_CONFIG_DESC_LEN + TUD_HID_INOUT_DESC_LEN)

#endif /* USB_DESCRIPTORS_H */
