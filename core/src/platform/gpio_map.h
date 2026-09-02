#ifndef GPIO_MAP_H
#define GPIO_MAP_H

/* Single source of truth for NOIR wallet pin assignments.
 * Change wiring here, nowhere else. */

#include "stm32f4xx_hal.h"

extern I2C_HandleTypeDef hi2c1;   /* owned by drivers/ssd1306.c */

/* ---- I2C1: SSD1306 OLED ---------------------------------- */
#define OLED_I2C             I2C1
#define OLED_SCL_PIN         GPIO_PIN_6
#define OLED_SCL_PORT        GPIOB
#define OLED_SDA_PIN         GPIO_PIN_7
#define OLED_SDA_PORT        GPIOB
#define OLED_I2C_ADDR        (0x3C << 1)   /* HAL wants 8-bit format */
#define OLED_I2C_AF          GPIO_AF4_I2C1

/* ---- Buttons (all active-low, pressed == GPIO_PIN_RESET) -- */
/* Verified wiring:
 *     PA7  = LEFT   ("A7")
 *     PB0  = RIGHT  ("B0")
 *     PA6  = UP     ("A6")    (PB2 is BOOT1 and reads LOW — unusable)
 *     PB10 = DOWN   ("B10")
 *     PB1  = CENTER ("B1")
 */
typedef struct {
    GPIO_TypeDef *port;
    uint32_t      pin;
} btn_map_t;

typedef enum {
    BTN_LEFT = 0,
    BTN_RIGHT,
    BTN_UP,
    BTN_DOWN,
    BTN_CENTER,
    BTN_COUNT
} btn_id_t;

static const btn_map_t BTN_MAP[BTN_COUNT] = {
    [BTN_LEFT]   = { GPIOA, GPIO_PIN_7  },  /* PA7  = LEFT            */
    [BTN_RIGHT]  = { GPIOB, GPIO_PIN_0  },  /* PB0  = RIGHT           */
    [BTN_UP]     = { GPIOA, GPIO_PIN_6  },  /* PA6  = UP              */
    [BTN_DOWN]   = { GPIOB, GPIO_PIN_10 },  /* PB10 = DOWN            */
    [BTN_CENTER] = { GPIOB, GPIO_PIN_1  },  /* PB1  = CENTER          */
};

/* Display letter for each button, in the same order as BTN_MAP. */
static const char BTN_LABELS[BTN_COUNT] = {
    [BTN_LEFT]   = 'L',
    [BTN_RIGHT]  = 'R',
    [BTN_UP]     = 'U',
    [BTN_DOWN]   = 'D',
    [BTN_CENTER] = 'C',
};

/* Physical pin name for the on-screen diagnostic. */
static const char * const BTN_NAMES[BTN_COUNT] = {
    [BTN_LEFT]   = "A7",
    [BTN_RIGHT]  = "B0",
    [BTN_UP]     = "A6",
    [BTN_DOWN]   = "B10",
    [BTN_CENTER] = "B1",
};

#endif /* GPIO_MAP_H */
