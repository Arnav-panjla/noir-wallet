#include "buttons.h"
#include "gpio_map.h"

#define DEBOUNCE_SAMPLES 2     /* N consistent samples (at ~10ms poll = ~20ms) */
#define LONG_PRESS_MS    500

static uint8_t  stable[BTN_COUNT];
static uint8_t  count[BTN_COUNT];
static uint32_t press_tick[BTN_COUNT];
static uint8_t  long_fired[BTN_COUNT];

void buttons_init(void) {
    for (int i = 0; i < BTN_COUNT; i++) {
        stable[i] = (HAL_GPIO_ReadPin(BTN_MAP[i].port, BTN_MAP[i].pin) == GPIO_PIN_RESET);
        count[i] = 0;
        press_tick[i] = 0;
        long_fired[i] = 0;
    }
}

int buttons_poll(btn_evt_t *e) {
    uint32_t now = HAL_GetTick();
    for (int i = 0; i < BTN_COUNT; i++) {
        uint8_t raw = (HAL_GPIO_ReadPin(BTN_MAP[i].port, BTN_MAP[i].pin) == GPIO_PIN_RESET);

        if (raw != stable[i]) {
            if (++count[i] >= DEBOUNCE_SAMPLES) {
                stable[i] = raw;
                count[i] = 0;
                if (raw) {
                    press_tick[i] = now;
                    long_fired[i] = 0;
                    e->id = i; e->ev = BTN_PRESS; return 1;
                } else {
                    e->id = i; e->ev = BTN_RELEASE; return 1;
                }
            }
        } else {
            count[i] = 0;
        }

        /* long-press fires once, while held */
        if (stable[i] && !long_fired[i] && (now - press_tick[i]) >= LONG_PRESS_MS) {
            long_fired[i] = 1;
            e->id = i; e->ev = BTN_LONG_PRESS; return 1;
        }
    }
    return 0;
}
