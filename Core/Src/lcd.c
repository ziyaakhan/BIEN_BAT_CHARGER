/*
 * lcd.c - HD44780 compatible driver (e.g., JHD204A 20x4) for STM32 HAL (4-bit)
 */

#include "lcd.h"

/* JHD204A is HD44780 compatible. 4-bit interface uses D4..D7, but in this
 * board the named pins are LCD_D0..LCD_D3 mapped to PA12, PA11, PA10, PA9.
 * We will treat LCD_D0 as D4, LCD_D1 as D5, LCD_D2 as D6, LCD_D3 as D7.
 */

/* Local helpers */
static void lcd_delay_us(uint16_t micros);
static void lcd_pulse_enable(void);
static void lcd_write4(uint8_t nibble);
static void lcd_send(uint8_t value, uint8_t is_data);

/* Minimal microsecond delay using busy loop scaled for 24 MHz SYSCLK.
 * This is approximate; HD44780 is tolerant. For safety we overshoot. */
static void lcd_delay_us(uint16_t micros)
{
    if (micros >= 1000u) {
        HAL_Delay(micros / 1000u);
        micros = (uint16_t)(micros % 1000u);
    }
    volatile uint32_t count = (uint32_t)micros * 40u; /* generous for 24 MHz */
    while (count--) {
        __asm volatile ("nop");
    }
}

static void lcd_pulse_enable(void)
{
    HAL_GPIO_WritePin(LCD_E_GPIO_Port, LCD_E_Pin, GPIO_PIN_SET);
    lcd_delay_us(1);
    HAL_GPIO_WritePin(LCD_E_GPIO_Port, LCD_E_Pin, GPIO_PIN_RESET);
    lcd_delay_us(50); /* enable cycle time */
}

static void lcd_write4(uint8_t nibble)
{
    /* Map nibble bit0..bit3 to LCD_D0..LCD_D3 pins */
    HAL_GPIO_WritePin(LCD_D0_GPIO_Port, LCD_D0_Pin, (nibble & 0x01) ? GPIO_PIN_SET : GPIO_PIN_RESET);
    HAL_GPIO_WritePin(LCD_D1_GPIO_Port, LCD_D1_Pin, (nibble & 0x02) ? GPIO_PIN_SET : GPIO_PIN_RESET);
    HAL_GPIO_WritePin(LCD_D2_GPIO_Port, LCD_D2_Pin, (nibble & 0x04) ? GPIO_PIN_SET : GPIO_PIN_RESET);
    HAL_GPIO_WritePin(LCD_D3_GPIO_Port, LCD_D3_Pin, (nibble & 0x08) ? GPIO_PIN_SET : GPIO_PIN_RESET);
    lcd_pulse_enable();
}

static void lcd_send(uint8_t value, uint8_t is_data)
{
    HAL_GPIO_WritePin(LCD_RS_GPIO_Port, LCD_RS_Pin, is_data ? GPIO_PIN_SET : GPIO_PIN_RESET);
    lcd_write4((uint8_t)(value >> 4));
    lcd_write4((uint8_t)(value & 0x0F));
}

/* Public API */
void LCD_Backlight(uint8_t on)
{
    HAL_GPIO_WritePin(LCD_BL_GPIO_Port, LCD_BL_Pin, on ? GPIO_PIN_SET : GPIO_PIN_RESET);
}

void LCD_Init(void)
{
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

void LCD_Clear(void)
{
    lcd_send(0x01, 0);
    HAL_Delay(2);
}

void LCD_Home(void)
{
    lcd_send(0x02, 0);
    HAL_Delay(2);
}

void LCD_SetCursor(uint8_t col, uint8_t row)
{
    /* JHD204A 20x4 DDRAM mapping */
    static const uint8_t row_offsets[4] = {0x00, 0x40, 0x14, 0x54};
    if (row > 3) row = 3;
    lcd_send((uint8_t)(0x80 | (row_offsets[row] + col)), 0);
}

void LCD_WriteChar(char c)
{
    lcd_send((uint8_t)c, 1);
}

void LCD_Print(const char *str)
{
    if (str == NULL) return;
    while (*str) {
        LCD_WriteChar(*str++);
    }
}

void LCD_PrintN(const char *str, uint16_t max_len)
{
    if (str == NULL) return;
    while (*str && max_len--) {
        LCD_WriteChar(*str++);
    }
}

void LCD_DisplayOn(void)
{
    lcd_send(0x0C, 0);
}

void LCD_DisplayOff(void)
{
    lcd_send(0x08, 0);
}

void LCD_PrintUInt8(uint8_t value)
{
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

void LCD_PrintUInt16(uint16_t value)
{
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

void LCD_PrintInt16(int16_t value)
{
    if (value < 0) {
        LCD_WriteChar('-');
        value = (int16_t)(-value);
    }
    LCD_PrintUInt16((uint16_t)value);
}