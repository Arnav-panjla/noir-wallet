/* NOIR wallet — TinyUSB configuration.
 *
 * Single HID interface (bidirectional 64-byte reports) emulating a Ledger
 * Nano S Plus. Device-only, no RTOS, no debug. Keep this minimal: the STM32F4
 * DWC2 device port is the only TinyUSB source compiled (see
 * lib/tinyusb/library.json srcFilter).
 */

#ifndef _TUSB_CONFIG_H_
#define _TUSB_CONFIG_H_

#define BOARD_TUD_RHPORT      0
#define BOARD_TUD_MAX_SPEED   OPT_MODE_DEFAULT_SPEED

/* ---- MCU / OS --------------------------------------------------- */
#define CFG_TUSB_MCU          OPT_MCU_STM32F4
#define CFG_TUSB_OS           OPT_OS_NONE
#define CFG_TUSB_DEBUG        0

/* ---- Device stack ----------------------------------------------- */
#define CFG_TUD_ENABLED       1
#define CFG_TUD_MAX_SPEED     BOARD_TUD_MAX_SPEED

#define CFG_TUSB_MEM_SECTION
#define CFG_TUSB_MEM_ALIGN    __attribute__ ((aligned(4)))

#define CFG_TUD_ENDPOINT0_SIZE 64

/* ---- Classes ---------------------------------------------------- */
#define CFG_TUD_CDC           0
#define CFG_TUD_MSC           0
#define CFG_TUD_HID           1
#define CFG_TUD_MIDI          0
#define CFG_TUD_VENDOR        0

/* HID buffer size must hold one full 64-byte Ledger frame. */
#define CFG_TUD_HID_EP_BUFSIZE 64

#endif /* _TUSB_CONFIG_H_ */
