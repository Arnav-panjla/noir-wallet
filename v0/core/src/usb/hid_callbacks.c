/* NOIR wallet — TinyUSB HID class callbacks.
 *
 * The Ledger transport is raw 64-byte reports on the interrupt IN/OUT
 * endpoints. TinyUSB 0.17 delivers host->device OUT reports through
 * tud_hid_set_report_cb(HID_REPORT_TYPE_OUTPUT); device->host sends go out via
 * tud_hid_n_report() when tud_hid_n_ready().
 */

#include "tusb.h"
#include "usb_descriptors.h"

/* Host requested a report via control transfer — not used by Ledger's transport. */
uint16_t tud_hid_get_report_cb(uint8_t itf, uint8_t report_id,
                               hid_report_type_t report_type,
                               uint8_t *buffer, uint16_t reqlen) {
    (void)itf; (void)report_id; (void)report_type; (void)buffer; (void)reqlen;
    return 0;
}

/* Host -> device: a report arrived on the interrupt OUT endpoint (0x01). This
 * is the Ledger transport receive path — one 64-byte frame per call. Will feed
 * the hid_frame reassembler in M7c. */
void tud_hid_set_report_cb(uint8_t itf, uint8_t report_id,
                           hid_report_type_t report_type,
                           uint8_t const *buffer, uint16_t bufsize) {
    (void)itf; (void)report_id;
    if (report_type != HID_REPORT_TYPE_OUTPUT) return;

    /* TODO(M7c): feed buffer[0..bufsize) to hid_frame assembler. */
    (void)buffer;
    (void)bufsize;
}
