/**
 * @file lcd.h
 * @brief HD44780 compatible LCD driver for STM32 HAL (4-bit interface)
 * @details This driver supports HD44780 compatible displays like JHD204A 20x4 LCD
 * @author Ziya
 * @date 2025
 */

#ifndef __LCD_HD44780_H
#define __LCD_HD44780_H

#ifdef __cplusplus
extern "C" {
#endif

#include "main.h"
#include <stdint.h>

/**
 * @brief Initialize the LCD display
 * @details Performs 4-bit initialization sequence for HD44780 compatible displays
 */
void LCD_Init(void);

/**
 * @brief Clear the entire LCD display
 */
void LCD_Clear(void);

/**
 * @brief Move cursor to home position (0,0)
 */
void LCD_Home(void);

/**
 * @brief Set cursor position
 * @param col Column position (0-19 for 20x4 display)
 * @param row Row position (0-3 for 20x4 display)
 */
void LCD_SetCursor(uint8_t col, uint8_t row);

/**
 * @brief Print a null-terminated string
 * @param str String to print (NULL-safe)
 */
void LCD_Print(const char *str);

/**
 * @brief Print a string with maximum length limit
 * @param str String to print
 * @param max_len Maximum number of characters to print
 */
void LCD_PrintN(const char *str, uint16_t max_len);

/**
 * @brief Write a single character
 * @param c Character to write
 */
void LCD_WriteChar(char c);

/**
 * @brief Print an 8-bit unsigned integer
 * @param value Value to print (0-255)
 */
void LCD_PrintUInt8(uint8_t value);

/**
 * @brief Print a 16-bit unsigned integer
 * @param value Value to print (0-65535)
 */
void LCD_PrintUInt16(uint16_t value);

/**
 * @brief Print a 16-bit signed integer
 * @param value Value to print (-32768 to 32767)
 */
void LCD_PrintInt16(int16_t value);

/**
 * @brief Print a 16-bit value with 1 decimal place
 * @details Input value is multiplied by 10, output shows 1 decimal place
 * @param value_times_10 Value multiplied by 10 (e.g., 194 for 19.4)
 */
void LCD_PrintUInt16_1dp(uint16_t value_times_10);

/**
 * @brief Print an 8-bit value with 2-digit formatting
 * @param value Value to print (0-99, values >=100 show as 99)
 */
void LCD_PrintUInt8_2d(uint8_t value);

/**
 * @brief Turn display on
 */
void LCD_DisplayOn(void);

/**
 * @brief Turn display off
 */
void LCD_DisplayOff(void);

/**
 * @brief Control LCD backlight
 * @param on 1 to turn on, 0 to turn off
 */
void LCD_Backlight(uint8_t on);

#ifdef __cplusplus
}
#endif

#endif /* __LCD_HD44780_H */


