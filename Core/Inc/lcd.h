/*
 * lcd.h - HD44780 compatible driver (e.g., JHD204A 20x4) for STM32 HAL (4-bit)
 */

#ifndef __LCD_HD44780_H
#define __LCD_HD44780_H

#ifdef __cplusplus
extern "C" {
#endif

#include "main.h"
#include <stdint.h>

/* Public API */
void LCD_Init(void);
void LCD_Clear(void);
void LCD_Home(void);
void LCD_SetCursor(uint8_t col, uint8_t row);
void LCD_Print(const char *str);
void LCD_PrintN(const char *str, uint16_t max_len);
void LCD_WriteChar(char c);
void LCD_PrintUInt8(uint8_t value);
void LCD_PrintUInt16(uint16_t value);
void LCD_PrintInt16(int16_t value);

/* Optional helpers */
void LCD_DisplayOn(void);
void LCD_DisplayOff(void);
void LCD_Backlight(uint8_t on);

#ifdef __cplusplus
}
#endif

#endif /* __LCD_HD44780_H */


