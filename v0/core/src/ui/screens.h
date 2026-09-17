#ifndef SCREENS_H
#define SCREENS_H

#include <stdint.h>

/* Minimal OLED UI framework: title bar, hint bar, centered text, menus. */

void ui_clear(void);
void ui_title(const char *title);
void ui_hint(const char *left, const char *right);
void ui_text(int16_t x, int16_t y, const char *s, uint8_t scale);
void ui_text_center(int16_t y, const char *s, uint8_t scale);
void ui_render(void);

typedef struct {
    const char **items;
    int n;
    int cursor;
} ui_menu_t;

void ui_menu_draw(const ui_menu_t *m, const char *title);

#endif
