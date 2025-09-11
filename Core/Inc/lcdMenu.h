/*
 * lcdMenu.h
 *
 *  Created on: Sep 11, 2025
 *      Author: Ziya
 */

#ifndef INC_LCDMENU_H_
#define INC_LCDMENU_H_

#include <stdint.h>

/* Button bit positions (readability) */
#define BUT_LEFT_POS   0
#define BUT_ON_POS     1
#define BUT_UP_POS     2
#define BUT_DOWN_POS   3
#define BUT_RIGHT_POS  4
#define BUT_OFF_POS    5

/* Button masks */
#define BUT_LEFT_M   (1U << BUT_LEFT_POS)
#define BUT_ON_M     (1U << BUT_ON_POS)
#define BUT_UP_M     (1U << BUT_UP_POS)
#define BUT_DOWN_M   (1U << BUT_DOWN_POS)
#define BUT_RIGHT_M  (1U << BUT_RIGHT_POS)
#define BUT_OFF_M    (1U << BUT_OFF_POS)

/* Optional compatibility aliases (remove if not needed elsewhere) */
#define BUT1_M BUT_LEFT_M
#define BUT2_M BUT_ON_M
#define BUT3_M BUT_UP_M
#define BUT4_M BUT_DOWN_M
#define BUT5_M BUT_RIGHT_M
#define BUT6_M BUT_OFF_M

/* Pages */
#define PAGE_LOADING 0
#define PAGE_MAIN    1

/* Public API */
void lcd_menu_init(void);
void lcd_menu_set_page(uint8_t page);
void lcd_menu_set_language(uint8_t lang_id); /* 0: EN, 1: TR */
void lcd_handle(void);
void button_handle(void);

/* Shared state */
extern uint8_t pageID;
extern uint8_t buttonState;
extern uint8_t lcdLangId; /* 0: EN, 1: TR */
extern uint8_t uiNeedsClear;

#endif /* INC_LCDMENU_H_ */