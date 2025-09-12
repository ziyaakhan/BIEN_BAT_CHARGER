/**
 * @file lcd.c
 * @brief HD44780 compatible LCD driver implementation for STM32 HAL (4-bit interface)
 * @details This driver supports HD44780 compatible displays like JHD204A 20x4 LCD
 * @author Ziya
 * @date 2025
 */

#include "lcd.h"

/* JHD204A is HD44780 compatible. 4-bit interface uses D4..D7, but in this
 * board the named pins are LCD_D0..LCD_D3 mapped to PA12, PA11, PA10, PA9.
 * We will treat LCD_D0 as D4, LCD_D1 as D5, LCD_D2 as D6, LCD_D3 as D7.
 */

/**
 * @brief Local helper function prototypes
 */
static void lcd_delay_us(uint16_t micros);
static void lcd_pulse_enable(void);
static void lcd_write4(uint8_t nibble);
static void lcd_send(uint8_t value, uint8_t is_data);

/**
 * @brief Minimal microsecond delay using busy loop scaled for 24 MHz SYSCLK
 * @details This is approximate; HD44780 is tolerant. For safety we overshoot.
 * @param micros Number of microseconds to delay
 */
static void lcd_delay_us(uint16_t micros) {
    if (micros >= 1000u) {
        HAL_Delay(micros / 1000u);
        micros = (uint16_t)(micros % 1000u);
    }
    volatile uint32_t count = (uint32_t)micros * 40u; /* generous for 24 MHz */
    while (count--) {
        __asm volatile ("nop");
    }
}

/**
 * @brief Pulse the LCD enable pin
 * @details Generates proper enable pulse timing for HD44780
 */
static void lcd_pulse_enable(void) {
    HAL_GPIO_WritePin(LCD_E_GPIO_Port, LCD_E_Pin, GPIO_PIN_SET);
    lcd_delay_us(1);
    HAL_GPIO_WritePin(LCD_E_GPIO_Port, LCD_E_Pin, GPIO_PIN_RESET);
    lcd_delay_us(50); /* enable cycle time */
}

/**
 * @brief Write 4-bit nibble to LCD data pins
 * @param nibble 4-bit value to write (lower 4 bits used)
 */
static void lcd_write4(uint8_t nibble) {
    /* Map nibble bit0..bit3 to LCD_D0..LCD_D3 pins */
    HAL_GPIO_WritePin(LCD_D0_GPIO_Port, LCD_D0_Pin, (nibble & 0x01) ? GPIO_PIN_SET : GPIO_PIN_RESET);
    HAL_GPIO_WritePin(LCD_D1_GPIO_Port, LCD_D1_Pin, (nibble & 0x02) ? GPIO_PIN_SET : GPIO_PIN_RESET);
    HAL_GPIO_WritePin(LCD_D2_GPIO_Port, LCD_D2_Pin, (nibble & 0x04) ? GPIO_PIN_SET : GPIO_PIN_RESET);
    HAL_GPIO_WritePin(LCD_D3_GPIO_Port, LCD_D3_Pin, (nibble & 0x08) ? GPIO_PIN_SET : GPIO_PIN_RESET);
    lcd_pulse_enable();
}

/**
 * @brief Send 8-bit value to LCD (command or data)
 * @param value 8-bit value to send
 * @param is_data 1 for data, 0 for command
 */
static void lcd_send(uint8_t value, uint8_t is_data) {
    HAL_GPIO_WritePin(LCD_RS_GPIO_Port, LCD_RS_Pin, is_data ? GPIO_PIN_SET : GPIO_PIN_RESET);
    lcd_write4((uint8_t)(value >> 4));
    lcd_write4((uint8_t)(value & 0x0F));
}

/**
 * @brief Control LCD backlight
 * @param on 1 to turn on, 0 to turn off
 */
void LCD_Backlight(uint8_t on) {
    HAL_GPIO_WritePin(LCD_BL_GPIO_Port, LCD_BL_Pin, on ? GPIO_PIN_SET : GPIO_PIN_RESET);
}

/**
 * @brief Initialize the LCD display
 * @details Performs 4-bit initialization sequence per HD44780 datasheet
 */
void LCD_Init(void) {
    /* Ensure control lines default */
    HAL_GPIO_WritePin(LCD_E_GPIO_Port, LCD_E_Pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(LCD_RS_GPIO_Port, LCD_RS_Pin, GPIO_PIN_RESET);

    /* Wait for LCD power up */
    HAL_Delay(40);

    /* 4-bit init sequence per HD44780 datasheet */
    lcd_write4(0x03);
    HAL_Delay(5);
    lcd_write4(0x03);
    lcd_delay_us(150);
    lcd_write4(0x03);
    lcd_delay_us(150);
    lcd_write4(0x02); /* set 4-bit mode */

    /* Function set: 4-bit, 2-line (20x4 uses 2-line controller), 5x8 dots */
    lcd_send(0x28, 0);
    /* Display OFF */
    lcd_send(0x08, 0);
    /* Clear display */
    lcd_send(0x01, 0);
    HAL_Delay(2);
    /* Entry mode: increment, no shift */
    lcd_send(0x06, 0);
    /* Display ON, cursor off, blink off */
    lcd_send(0x0C, 0);
}

/**
 * @brief Clear the entire LCD display
 */
void LCD_Clear(void) {
    lcd_send(0x01, 0);
    HAL_Delay(2);
}

/**
 * @brief Move cursor to home position (0,0)
 */
void LCD_Home(void) {
    lcd_send(0x02, 0);
    HAL_Delay(2);
}

/**
 * @brief Set cursor position
 * @param col Column position (0-19 for 20x4 display)
 * @param row Row position (0-3 for 20x4 display)
 */
void LCD_SetCursor(uint8_t col, uint8_t row) {
    /* JHD204A 20x4 DDRAM mapping */
    static const uint8_t row_offsets[4] = {0x00, 0x40, 0x14, 0x54};
    if (row > 3) row = 3;
    lcd_send((uint8_t)(0x80 | (row_offsets[row] + col)), 0);
}

/**
 * @brief Write a single character
 * @param c Character to write
 */
void LCD_WriteChar(char c) {
    lcd_send((uint8_t)c, 1);
}

/**
 * @brief Print a null-terminated string
 * @param str String to print (NULL-safe)
 */
void LCD_Print(const char *str) {
    if (str == NULL) return;
    while (*str) {
        LCD_WriteChar(*str++);
    }
}

/**
 * @brief Print a string with maximum length limit
 * @param str String to print
 * @param max_len Maximum number of characters to print
 */
void LCD_PrintN(const char *str, uint16_t max_len) {
    if (str == NULL) return;
    while (*str && max_len--) {
        LCD_WriteChar(*str++);
    }
}

/**
 * @brief Turn display on
 */
void LCD_DisplayOn(void) {
    lcd_send(0x0C, 0);
}

/**
 * @brief Turn display off
 */
void LCD_DisplayOff(void) {
    lcd_send(0x08, 0);
}

/**
 * @brief Print an 8-bit unsigned integer
 * @param value Value to print (0-255)
 */
void LCD_PrintUInt8(uint8_t value) {
    char buf[4]; /* max 255 */
    int idx = 0;
    if (value >= 200) {
        buf[idx++] = '2';
        value -= 200;
    } else if (value >= 100) {
        buf[idx++] = '1';
        value -= 100;
    }
    if (idx > 0 || value >= 10) {
        buf[idx++] = (char)('0' + (value / 10));
        value = (uint8_t)(value % 10);
    }
    buf[idx++] = (char)('0' + value);
    buf[idx] = '\0';
    LCD_Print(buf);
}

/**
 * @brief Print a 16-bit unsigned integer
 * @param value Value to print (0-65535)
 */
void LCD_PrintUInt16(uint16_t value) {
    char buf[6]; /* max 65535 */
    int idx = 0;
    
    /* Handle thousands */
    if (value >= 10000) {
        buf[idx++] = (char)('0' + (value / 10000));
        value = (uint16_t)(value % 10000);
    }
    if (value >= 1000) {
        buf[idx++] = (char)('0' + (value / 1000));
        value = (uint16_t)(value % 1000);
    }
    if (value >= 100) {
        buf[idx++] = (char)('0' + (value / 100));
        value = (uint16_t)(value % 100);
    }
    if (value >= 10) {
        buf[idx++] = (char)('0' + (value / 10));
        value = (uint16_t)(value % 10);
    }
    buf[idx++] = (char)('0' + value);
    buf[idx] = '\0';
    LCD_Print(buf);
}

/**
 * @brief Print a 16-bit signed integer
 * @param value Value to print (-32768 to 32767)
 */
void LCD_PrintInt16(int16_t value) {
    if (value < 0) {
        LCD_WriteChar('-');
        value = (int16_t)(-value);
    }
    LCD_PrintUInt16((uint16_t)value);
}

/**
 * @brief Print a 16-bit value with 1 decimal place
 * @details Input value is multiplied by 10, output shows 1 decimal place
 * @param value_times_10 Value multiplied by 10 (e.g., 194 for 19.4)
 */
void LCD_PrintUInt16_1dp(uint16_t value_times_10) {
    uint16_t integer = value_times_10 / 10u;
    uint16_t frac = value_times_10 % 10u;
    /* Pad to fixed width 4: " 9.0" or "19.4" */
    if (integer < 10u) {
        LCD_WriteChar(' ');
    }
    LCD_PrintUInt16(integer);
    LCD_WriteChar('.');
    LCD_WriteChar((char)('0' + frac));
}

/**
 * @brief Print an 8-bit value with 2-digit formatting
 * @param value Value to print (0-99, values >=100 show as 99)
 */
void LCD_PrintUInt8_2d(uint8_t value) {
    if (value < 10u) {
        LCD_WriteChar('0');
        LCD_WriteChar((char)('0' + value));
    } else if (value < 100u) {
        char buf[3];
        int idx = 0;
        if (value >= 10u) {
            buf[idx++] = (char)('0' + (value / 10u));
            value = (uint8_t)(value % 10u);
        }
        buf[idx++] = (char)('0' + value);
        buf[idx] = '\0';
        LCD_Print(buf);
    } else {
        LCD_Print("99");
    }
}
