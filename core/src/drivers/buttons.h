#ifndef BUTTONS_H
#define BUTTONS_H

#include <stdint.h>

typedef enum { BTN_PRESS, BTN_RELEASE, BTN_LONG_PRESS } btn_event_t;

typedef struct {
    int id;              /* btn_id_t from gpio_map.h: BTN_LEFT..BTN_CENTER */
    btn_event_t ev;
} btn_evt_t;

/* Reads initial pin state. */
void buttons_init(void);

/* Poll all buttons; call at a steady ~10-20ms. Returns 1 and fills *e if a
 * debounced press/release/long-press (500ms) happened this tick. */
int buttons_poll(btn_evt_t *e);

#endif
