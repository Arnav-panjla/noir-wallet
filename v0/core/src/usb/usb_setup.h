#ifndef USB_SETUP_H
#define USB_SETUP_H

/* Bring up the USB OTG FS peripheral (48 MHz clock already configured by the
 * PLL in SystemClock_Config), configure PA11/PA12, and start TinyUSB. Call
 * once after clock + GPIO init. */
void usb_init(void);

/* Count of OTG_FS interrupts since boot — used on the splash as a "is the
 * host actually talking to us" diagnostic. 0 = D+ never connected. */
extern volatile uint32_t usb_irq_count;

#endif /* USB_SETUP_H */
