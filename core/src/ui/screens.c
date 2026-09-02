#include "screens.h"
#include "ssd1306_min.h"
#include "gpio_map.h"
#include <string.h>

void ui_clear(void) { SSD1306_Clear(); }

void ui_text(int16_t x, int16_t y, const char *s, uint8_t scale) {
    SSD1306_DrawString(x, y, s, scale);
}

void ui_text_center(int16_t y, const char *s, uint8_t scale) {
    int w = (int)strlen(s) * 6 * scale;
    SSD1306_DrawString((SSD1306_WIDTH - w) / 2, y, s, scale);
}

void ui_title(const char *title) {
    SSD1306_DrawString(2, 1, title, 1);
    SSD1306_DrawFastHLine(0, 10, SSD1306_WIDTH);
}

void ui_hint(const char *left, const char *right) {
    SSD1306_DrawFastHLine(0, 54, SSD1306_WIDTH);
    SSD1306_DrawString(2, 56, left, 1);
    if (right) {
        int w = (int)strlen(right) * 6;
        SSD1306_DrawString(SSD1306_WIDTH - 2 - w, 56, right, 1);
    }
}

void ui_render(void) { SSD1306_UpdateScreen(&hi2c1); }

void ui_menu_draw(const ui_menu_t *m, const char *title) {
    ui_clear();
    ui_title(title);
    for (int i = 0; i < m->n; i++) {
        int y = 14 + i * 12;
        char line[24];
        int p = 0;
        line[p++] = (i == m->cursor) ? '>' : ' ';
        for (const char *s = m->items[i]; *s; s++) line[p++] = *s;
        line[p] = '\0';
        SSD1306_DrawString(6, y, line, 1);
    }
    ui_hint("UP DN", "OK SEL");
    ui_render();
}
