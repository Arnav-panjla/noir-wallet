/* NOIR wallet — USB OTG FS bring-up for STM32F401 (DWC2 core).
 *
 * The 48 MHz USB clock comes from PLLQ=7 (VCO 336 MHz / 7) set in
 * SystemClock_Config(). This file only enables the peripheral clock, muxes
 * PA11 (DM) / PA12 (DP) to AF10, and starts TinyUSB. All F401-specific USB
 * code lives here so a board swap touches only this file.
 */

#include "stm32f4xx_hal.h"
#include "tusb.h"
#include "usb_setup.h"

/* Route the OTG FS interrupt to TinyUSB's DWC2 handler (rhport 0 = OTG_FS).
 * Also count interrupts so the splash can show whether the host is talking. */
volatile uint32_t usb_irq_count = 0;

void OTG_FS_IRQHandler(void) {
    usb_irq_count++;
    tud_int_handler(0);
}

void usb_init(void) {
    GPIO_InitTypeDef g = {0};

    __HAL_RCC_GPIOA_CLK_ENABLE();

    /* PA11 = USB_DM, PA12 = USB_DP */
    g.Pin       = GPIO_PIN_11 | GPIO_PIN_12;
    g.Mode      = GPIO_MODE_AF_PP;
    g.Pull      = GPIO_NOPULL;
    g.Speed     = GPIO_SPEED_FREQ_HIGH;
    g.Alternate = GPIO_AF10_OTG_FS;
    HAL_GPIO_Init(GPIOA, &g);

    __HAL_RCC_USB_OTG_FS_CLK_ENABLE();

    tusb_init();

    /* The BlackPill does not route VBUS to the OTG_FS VBUS pad (PA9), so the
     * VBUS-sense comparator reads "absent" and the device never pulls D+ up
     * even with soft-disconnect cleared. Override VBUS sensing so the device
     * assumes VBUS is always present (standard fix for boards with no VBUS
     * sense trace). Must run AFTER tusb_init(): TinyUSB's core soft-reset
     * re-clears the STM32 GCCFG extension bits during its FS-PHY init. */
    USB_OTG_FS->GCCFG |= USB_OTG_GCCFG_NOVBUSSENS;
}
