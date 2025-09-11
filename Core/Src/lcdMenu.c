/*
 * lcdMenu.c
 *
 *  Created on: Sep 11, 2025
 *      Author: Ziya
 */

#include <stdint.h>
#include "lcdMenu.h"
#include "lcd.h"
#include "adc.h"
#include "main.h"

uint8_t pageID;
uint8_t buttonState = 0;
uint8_t lcdLangId = 0; /* 0: EN, 1: TR */
static uint8_t prevPageID = 0xFF; /* force first render to clear */

/* Language tables using pointer-to-pointer (double pointer) style */
static const char * const strEN_LOADING_LINE0 = "********************";
static const char * const strEN_LOADING_LINE1 = "*      BIENSIS     *";
static const char * const strEN_LOADING_LINE2 = "* BATTERY CHARGER  *";
static const char * const strEN_LOADING_LINE3 = "********************";

static const char * const strTR_LOADING_LINE0 = "********************";
static const char * const strTR_LOADING_LINE1 = "*      BIENSIS     *";
static const char * const strTR_LOADING_LINE2 = "*   SARJ CIHAZI    *";
static const char * const strTR_LOADING_LINE3 = "********************";

static const char * const strEN_VBAT = "Vout:";  /* Output Voltage */
static const char * const strEN_VAC  = "Mains:"; /* AC Mains */
static const char * const strEN_IDC1 = "I1:";
static const char * const strEN_IDC2 = "Iout:";  /* Output Current */
static const char * const strEN_VDC1 = "VDC1:";
static const char * const strEN_VDC2 = "VDC2:";
static const char * const strEN_TEMP = "Temp:";

static const char * const strTR_VBAT = "CikisV:";  /* Cikis Gerilimi */
static const char * const strTR_VAC  = "Sebeke:";  /* Sebeke */
static const char * const strTR_IDC1 = "I1:";
static const char * const strTR_IDC2 = "CikisI:";  /* Cikis Akimi */
static const char * const strTR_VDC1 = "VDC1:";
static const char * const strTR_VDC2 = "VDC2:";
static const char * const strTR_TEMP = "Sic:";     /* Sicaklik */

/* Loading screen lines per language */
static const char * const LOADING_EN[4] = {
	strEN_LOADING_LINE0,
	strEN_LOADING_LINE1,
	strEN_LOADING_LINE2,
	strEN_LOADING_LINE3
};

static const char * const LOADING_TR[4] = {
	strTR_LOADING_LINE0,
	strTR_LOADING_LINE1,
	strTR_LOADING_LINE2,
	strTR_LOADING_LINE3
};

/* Labels per language */
static const char * const LABELS_EN[7] = {
	strEN_VBAT,strEN_VAC,strEN_IDC1,strEN_IDC2,strEN_VDC1,strEN_VDC2,strEN_TEMP
};
static const char * const LABELS_TR[7] = {
	strTR_VBAT,strTR_VAC,strTR_IDC1,strTR_IDC2,strTR_VDC1,strTR_VDC2,strTR_TEMP
};

/* Main page title per language (20 chars max) */
static const char * const MAIN_TITLE_EN = "BIENSIS BAT CHARGER";   /* 19 chars */
static const char * const MAIN_TITLE_TR = "BIENSIS SARJ CIHAZI";   /* 20 chars */

void lcd_menu_init(void)
{
	pageID = PAGE_LOADING;
	LCD_Clear(); /* ensure clear at startup */
}

void lcd_menu_set_page(uint8_t page)
{
	pageID = page;
	LCD_Clear();
}

void lcd_menu_set_language(uint8_t lang_id)
{
	lcdLangId = (lang_id != 0) ? 1u : 0u;
	LCD_Clear();
}

void lcd_handle(void)
{
	/* Clear once when page changes to avoid leftovers */
	if (pageID != prevPageID) {
		LCD_Clear();
		prevPageID = pageID;
	}

	switch(pageID)
	{
	case PAGE_LOADING:
	{
		const char * const * lines = (lcdLangId == 0) ? LOADING_EN : LOADING_TR; /* pointer-to-pointer selector */
		LCD_SetCursor(0, 0); LCD_Print(lines[0]);
		LCD_SetCursor(0, 1); LCD_Print(lines[1]);
		LCD_SetCursor(0, 2); LCD_Print(lines[2]);
		LCD_SetCursor(0, 3); LCD_Print(lines[3]);
	}
		break;

	case PAGE_MAIN:
	{
		const char * title = (lcdLangId == 0) ? MAIN_TITLE_EN : MAIN_TITLE_TR;
		const char * const * labels = (lcdLangId == 0) ? LABELS_EN : LABELS_TR;

		/* Title */
		LCD_SetCursor(0, 0);
		LCD_Print(title);

		/* Row 1: IDC2 and VBAT */
		LCD_SetCursor(0, 1);
		LCD_Print(labels[3]); /* IDC2 */
		LCD_PrintUInt16(adcIDC2);
		LCD_SetCursor(10, 1);
		LCD_Print(labels[0]); /* VBAT */
		LCD_PrintUInt16(adcVBAT1);

		/* Row 2: VAC */
		LCD_SetCursor(0, 2);
		LCD_Print(labels[1]); /* VAC */
		LCD_PrintUInt16(adcVAC);

		/* Row 3: TEMP */
		LCD_SetCursor(0, 3);
		LCD_Print(labels[6]); /* TEMP */
		LCD_PrintUInt16(adcTEMP);
	}
		break;

	default:
		break;
	}

}

void button_handle(void)
{
	if (buttonState == 0) {
		return;
	}

	/* Button order: Left, On, Up, Down, Right, Off */
	/* Left: previous page */
	if (buttonState & BUT_LEFT_M) {
		if (pageID == PAGE_MAIN) {
			lcd_menu_set_page(PAGE_LOADING);
		} else {
			lcd_menu_set_page(PAGE_MAIN);
		}
	}
	/* On: set SHUTDOWN2 = 1 */
	if (buttonState & BUT_ON_M) {
		HAL_GPIO_WritePin(SHUTDOWN2_GPIO_Port, SHUTDOWN2_Pin, GPIO_PIN_SET);
	}
	/* Up: toggle language EN/TR */
	if (buttonState & BUT_UP_M) {
		lcd_menu_set_language((uint8_t)(lcdLangId ^ 1u));
	}
	/* Down: no action */
	/* Right: next page */
	if (buttonState & BUT_RIGHT_M) {
		if (pageID == PAGE_LOADING) {
			lcd_menu_set_page(PAGE_MAIN);
		} else {
			lcd_menu_set_page(PAGE_LOADING);
		}
	}
	/* Off: set SHUTDOWN2 = 0 */
	if (buttonState & BUT_OFF_M) {
		HAL_GPIO_WritePin(SHUTDOWN2_GPIO_Port, SHUTDOWN2_Pin, GPIO_PIN_RESET);
	}

	buttonState = 0;
}
