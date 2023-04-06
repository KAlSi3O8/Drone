#ifndef __OLED_H
#define __OLED_H

#include <stm32f4xx.h>
#define OLED_ADDR       0x78
#define OLED_INITCMD "\x00\xAE\
\xA8\x3F\xD3\x00\x40\x2E\
\xA1\xC8\xDA\x12\x81\xFF\
\xA4\xA6\xD5\x80\x20\x00\
\x8D\x14\xAF"
#define OLEC_CLRALLCMD "\x00\x21\x00\x7F\x22\x00\x07"

void OLED_All(uint8_t state);
void OLED_Init(void);
void OLED_WriteRAM(uint8_t x, uint8_t y, uint8_t w, uint8_t h);
void OLED_SS(uint8_t x, uint8_t y, uint8_t *str, uint32_t ziku, uint8_t n);
void OLED_SC(uint8_t x, uint8_t y, uint8_t h, uint8_t w, const uint8_t *zi, uint8_t n);
void OLED_Point(uint8_t x, uint8_t y, uint8_t state);
void OLED_ClnAllDRAM(void);
void OLED_DT(uint8_t x, uint8_t y, uint8_t x1, uint8_t y1, uint8_t bit);
void OLED_ShowNum(uint8_t x, uint8_t y, int32_t num, uint32_t ziku, uint8_t n, uint8_t len);
void OLED_println(uint8_t *str);
void OLED_ResetOffset(void);


#endif
