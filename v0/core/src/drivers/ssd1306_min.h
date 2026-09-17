#ifndef SSD1306_MIN_H
#define SSD1306_MIN_H

#include "stm32f4xx_hal.h"
#include <stdint.h>

#define SSD1306_ADDR        (0x3C << 1)
#define SSD1306_WIDTH       128
#define SSD1306_HEIGHT      64

void SSD1306_Init(I2C_HandleTypeDef *hi2c);
void SSD1306_Clear(void);
void SSD1306_UpdateScreen(I2C_HandleTypeDef *hi2c);
void SSD1306_DrawPixel(int16_t x, int16_t y, uint8_t color);
void SSD1306_DrawBitmap(int16_t x, int16_t y, const uint8_t *bitmap, int16_t w, int16_t h);
void SSD1306_DrawFastHLine(int16_t x, int16_t y, int16_t w);
void SSD1306_DrawChar(int16_t x, int16_t y, char c, uint8_t scale);
void SSD1306_DrawString(int16_t x, int16_t y, const char *str, uint8_t scale);

#endif