/**
 * @file lcdMenu.c
 * @brief LCD Menu System Implementation for Battery Charger
 * @details Provides menu navigation, language support, and button handling
 * @author Ziya
 * @date 2025
 */

#include <stdint.h>
#include "lcdMenu.h"
#include "lcd.h"
#include "adc.h"
#include "main.h"
#include "out_control.h"

/** @name Global State Variables */
/**@{*/
uint8_t pageID;        /**< Current page ID */
uint8_t buttonState = 0;   /**< Button state bitfield */
uint8_t lcdLangId = 1;     /**< Language ID (0: EN, 1: TR) */
static uint8_t prevPageID = 0xFF; /**< Previous page ID for change detection */
uint8_t uiNeedsClear = 0;  /**< UI refresh flag */
/* operatingMode now defined in out_control.c */
const char * companyName = "BIENSIS"; /**< Company name editable at runtime */
uint8_t menuIndex = 0;  /**< Current menu selection index [0..3] */
uint8_t subIndex = 0;   /**< Current subpage selection index */
static uint8_t isEditing = 0;       /**< 0: navigating, 1: editing current field */
static uint32_t rightPressStartMs = 0; /**< Right button hold start (ms), 0 if not holding */
uint8_t mfgPinError = 0;     /**< 1 if last PIN attempt was wrong */
static uint32_t mfgPinErrorUntilMs = 0; /**< millis until which error is shown */
static uint16_t editBackupValue = 0;    /**< backup for numeric edits */

/* UI-only parameters and PIN state */
uint8_t brightness = 50;         /**< 0..100 */
uint16_t mfgPinCode = 0;         /**< Default 0000 */
uint8_t mfgPinInput[4] = {0,0,0,0};
uint8_t mfgPinPos = 0;           /**< PIN cursor 0..3 */
/**@}*/

/* English strings (kept minimal; unused full labels removed) */
static const char * const strEN_LOADING_LINE0 = "********************";
static const char * const strEN_LOADING_LINE3 = "********************";

/* Turkish strings (kept minimal; unused full labels removed) */
static const char * const strTR_LOADING_LINE0 = "********************";
static const char * const strTR_LOADING_LINE3 = "********************";

/* Loading arrays removed; dynamic composition is used */

/* Full label arrays removed; compact labels used below */

/** @name Device/Title Names via pointer-to-pointer */
/**@{*/
const char * const DEVICE_NAMES_EN[2] = { "BATTERY CHARGER", "POWER SUPPLY" };
const char * const DEVICE_NAMES_TR[2] = { "SARJ CIHAZI", "GUC KAYNAGI" };
const char * const * DEVICE_NAMES_LANG[2] = { DEVICE_NAMES_EN, DEVICE_NAMES_TR };

const char * const TITLE_NAMES_EN[2] = { "BAT CHARGER", "POWER SUPPLY" };
const char * const TITLE_NAMES_TR[2] = { "SARJ CIHAZI", "GUC KAYNAGI" };
const char * const * TITLE_NAMES_LANG[2] = { TITLE_NAMES_EN, TITLE_NAMES_TR };
/**@}*/

/** @name Compact Labels for Tight Layout */
/**@{*/
static const char * const LABELS_EN_SHORT[4] = { "I Out:", "V Out:", "Line:", "Temp:" };
static const char * const LABELS_TR_SHORT[4] = { "Cikis I:", "Cikis V:", "Sebeke:", "Sic:" };
/**@}*/

/** @name Short labels for charge stages (fit at right side of LCD) */
/**@{*/
static const char * const STAGE_EN_SHORT[] = {
	/* STATE_BULK */		  "BULK",
	/* STATE_BATTERY_SAFE */  "SAFE",
	/* STATE_ABSORPTION */	  "ABS",
	/* STATE_EQUALIZATION */  "EQL",
	/* STATE_FLOAT */		  "FLOAT",
	/* STATE_STORAGE */	  "STORE",
	/* STATE_REFRESH */	  "RFRSH"
};

static const char * const STAGE_TR_SHORT[] = {
	/* STATE_BULK */		  "BULK",
	/* STATE_BATTERY_SAFE */  "GUVEN",
	/* STATE_ABSORPTION */	  "ABSOR",
	/* STATE_EQUALIZATION */  "ESIT",
	/* STATE_FLOAT */		  "FLOAT",
	/* STATE_STORAGE */	  "DEPO",
	/* STATE_REFRESH */	  "YENIL"
};

static const char * const * STAGE_NAMES_LANG[2] = { STAGE_EN_SHORT, STAGE_TR_SHORT };
/**@}*/

/* String table keys for i18n */
typedef enum {
    UI_STR_MENU_TITLE = 0,
    UI_STR_ENTER_DATA,
    UI_STR_OUTPUT_CONTROL,
    UI_STR_OPERATING_MODE,
    UI_STR_SETTINGS,
    UI_STR_TEST_V,
    UI_STR_TEST_I,
    UI_STR_SHORT_TEST,
    UI_STR_LANG,
    UI_STR_BRIGHT,
    UI_STR_MFG_MENU,
    UI_STR_MANUFACTURER,
    UI_STR_ENTER_PIN,
    UI_STR_WRONG_PIN,
    UI_LBL_BATV,
    UI_LBL_CAPACITY,
    UI_LBL_COUNT,
    UI_STR_OPEN,
    UI_STR_CLOSE,
    UI_STR_CHARGER_NAME,
    UI_STR_SUPPLY_NAME,
    UI_STR_FACTORY_PAGE,
    UI_STR_LEFT_EXIT,
    UI_STR_SAFE_CHARGE,
    UI_STR_SOFT_CHARGE,
    UI_STR_EQUALIZE
} UiStrId;

static const char * const UI_STR_EN[] = {
    [UI_STR_MENU_TITLE]    = "Menu",
    [UI_STR_ENTER_DATA]    = "Enter Data",
    [UI_STR_OUTPUT_CONTROL]= "Output Control",
    [UI_STR_OPERATING_MODE]= "Operating Mode",
    [UI_STR_SETTINGS]      = "Settings",
    [UI_STR_TEST_V]        = "Test V:",
    [UI_STR_TEST_I]        = "Test I:",
    [UI_STR_SHORT_TEST]    = "Short test:",
    [UI_STR_LANG]          = "Lang:",
    [UI_STR_BRIGHT]        = "Bright:",
    [UI_STR_MFG_MENU]      = "Mfg menu:",
    [UI_STR_MANUFACTURER]  = "MANUFACTURER",
    [UI_STR_ENTER_PIN]     = "ENTER PIN",
    [UI_STR_WRONG_PIN]     = "WRONG PIN",
    [UI_LBL_BATV]          = "Bat V:",
    [UI_LBL_CAPACITY]      = "Capacity:",
    [UI_LBL_COUNT]         = "Count:",
    [UI_STR_OPEN]          = "Open",
    [UI_STR_CLOSE]         = "Close",
    [UI_STR_CHARGER_NAME]  = "Charger",
    [UI_STR_SUPPLY_NAME]   = "Supply",
    [UI_STR_FACTORY_PAGE]  = "Factory page",
    [UI_STR_LEFT_EXIT]     = "Left to exit",
    [UI_STR_SAFE_CHARGE]   = "Safe:",
    [UI_STR_SOFT_CHARGE]   = "Soft:",
    [UI_STR_EQUALIZE]      = "Equalize:"
};

static const char * const UI_STR_TR[] = {
    [UI_STR_MENU_TITLE]    = "Menu",
    [UI_STR_ENTER_DATA]    = "Verileri Gir",
    [UI_STR_OUTPUT_CONTROL]= "Cikis Kontrol",
    [UI_STR_OPERATING_MODE]= "Calisma Modu",
    [UI_STR_SETTINGS]      = "Ayarlar",
    [UI_STR_TEST_V]        = "Test V:",
    [UI_STR_TEST_I]        = "Test I:",
    [UI_STR_SHORT_TEST]    = "Kisa test:",
    [UI_STR_LANG]          = "Dil:",
    [UI_STR_BRIGHT]        = "Parlak:",
    [UI_STR_MFG_MENU]      = "Uretici Menu",
    [UI_STR_MANUFACTURER]  = "URETICI MENU",
    [UI_STR_ENTER_PIN]     = "PIN GIR",
    [UI_STR_WRONG_PIN]     = "YANLIS PIN",
    [UI_LBL_BATV]          = "Aku V:",
    [UI_LBL_CAPACITY]      = "Kapasite:",
    [UI_LBL_COUNT]         = "Sayi:",
    [UI_STR_OPEN]          = "Acik",
    [UI_STR_CLOSE]         = "Kapali",
    [UI_STR_CHARGER_NAME]  = "Sarj Cihazi",
    [UI_STR_SUPPLY_NAME]   = "Guc Kaynagi",
    [UI_STR_FACTORY_PAGE]  = "Fabrika sayfasi",
    [UI_STR_LEFT_EXIT]     = "Sol cikis",
    [UI_STR_SAFE_CHARGE]   = "Guvenli Sarj:",
    [UI_STR_SOFT_CHARGE]   = "Soft Sarj:",
    [UI_STR_EQUALIZE]      = "V esitleme:"
};

static const char * const * UI_STR_TABLE[2] = { UI_STR_EN, UI_STR_TR };

/* Language-resolved pointers (assigned once per language) */
static const char * STR_BATV = 0;
static const char * STR_CAPACITY = 0;
static const char * STR_COUNT = 0;
static const char * STR_LOAD_BORDER_TOP = 0;
static const char * STR_LOAD_BORDER_BOTTOM = 0;
static const char * const * STR_LABELS_SHORT = 0;
static const char * const * STR_MENU_ITEMS = 0;
static const char * STR_LANG_VAL = 0;
static char CH_CURR = 'I';
static uint8_t STATUS_COL = 15u;
static uint8_t TEMP_COL = 11u;

/* Menu title and items (defined here so ui_assign_language can reference them) */
static const char * const MENU_ITEMS_EN[4] = { "Enter Data", "Output Control", "Operating Mode", "Settings" };
static const char * const MENU_ITEMS_TR[4] = { "Verileri Gir", "Cikis Kontrol", "Calisma Modu", "Ayarlar" };

static inline const char * ui_get(UiStrId id)
{
    return UI_STR_TABLE[lcdLangId][id];
}

static void ui_assign_language(void)
{
    const char * const * T = UI_STR_TABLE[lcdLangId];
    STR_BATV     = T[UI_LBL_BATV];
    STR_CAPACITY = T[UI_LBL_CAPACITY];
    STR_COUNT    = T[UI_LBL_COUNT];
    if (lcdLangId == 0) {
        STR_LOAD_BORDER_TOP    = strEN_LOADING_LINE0;
        STR_LOAD_BORDER_BOTTOM = strEN_LOADING_LINE3;
        STR_LABELS_SHORT       = LABELS_EN_SHORT;
        STR_MENU_ITEMS         = MENU_ITEMS_EN;
        STR_LANG_VAL           = "EN";
        CH_CURR                = 'I';
        STATUS_COL             = 15u;
        TEMP_COL               = 11u;
    } else {
        STR_LOAD_BORDER_TOP    = strTR_LOADING_LINE0;
        STR_LOAD_BORDER_BOTTOM = strTR_LOADING_LINE3;
        STR_LABELS_SHORT       = LABELS_TR_SHORT;
        STR_MENU_ITEMS         = MENU_ITEMS_TR;
        STR_LANG_VAL           = "TR";
        CH_CURR                = 'A';
        STATUS_COL             = 14u;
        TEMP_COL               = 12u;
    }
}

/** @name Menu Page Strings */
/**@{*/
/* Definitions moved below to avoid forward-reference issues */
/**@}*/

/**
 * @brief Initialize the LCD menu system
 * @details Sets up initial page and clears display
 */
void lcd_menu_init(void) {
    pageID = PAGE_LOADING;
    ui_assign_language();
    LCD_Clear(); /* ensure clear at startup */
}

/**
 * @brief Set the current menu page
 * @param page Page ID (PAGE_LOADING or PAGE_MAIN)
 */
void lcd_menu_set_page(uint8_t page) {
    pageID = page;
    LCD_Clear();
}

/**
 * @brief Set the display language
 * @param lang_id Language ID (0: English, 1: Turkish)
 */
void lcd_menu_set_language(uint8_t lang_id) {
    if (lang_id != 0u)
    {
        lcdLangId = 1u;
    }
    else
    {
        lcdLangId = 0u;
    }
    ui_assign_language();
    LCD_Clear();
}

/**
 * @brief Handle LCD display rendering
 * @details Renders the current page based on pageID and language
 */
void lcd_handle(void)
{
    /* Clear once when page changes or explicitly requested */
    if (pageID != prevPageID || uiNeedsClear)
    {
        LCD_Clear();
        prevPageID = pageID;
        uiNeedsClear = 0;
    }

    /* Ensure language strings are assigned even if init wasn't called */
    if (STR_BATV == 0 || STR_CAPACITY == 0 || STR_COUNT == 0)
    {
        ui_assign_language();
    }

    /* Edit mode is entered with Right press in button_handle; no long-press */

    switch(pageID)
    {
    case PAGE_LOADING:
    {
        /* Dynamic line 2 content per operating mode */
        LCD_SetCursor(0, 0); 
		LCD_Print(STR_LOAD_BORDER_TOP);
        /* Center company name */
        LCD_SetCursor(0, 1);
        {
            char line[21];
            const char *name = companyName;
            int len = 0; while (name[len] && len < 20) len++;
            int pad = (20 - len) / 2;
            for (int i=0;i<pad;i++) line[i] = ' ';
            for (int i=0;i<len;i++) line[pad+i] = name[i];
            for (int i=pad+len;i<20;i++) line[i] = ' ';
            line[20] = '\0';
            LCD_Print(line);
        }
        LCD_SetCursor(0, 2);
        {
            const char *dev = DEVICE_NAMES_LANG[lcdLangId][operatingMode];
            char buf[21];
            int len=0; while (dev[len] && len<20) len++;
            int pad = (20 - len) / 2;
            for (int i=0;i<pad;i++) buf[i]=' ';
            for (int i=0;i<len;i++) buf[pad+i]=dev[i];
            for (int i=pad+len;i<20;i++) buf[i]=' ';
            buf[20]='\0';
            LCD_Print(buf);
        }
        LCD_SetCursor(0, 3); 
		LCD_Print(STR_LOAD_BORDER_BOTTOM);
    }
        break;

    case PAGE_MAIN: {
        const char * titleShort = TITLE_NAMES_LANG[lcdLangId][operatingMode];
        const char * const * labelsShort = STR_LABELS_SHORT;

        /* Title (row 0) */
		LCD_SetCursor(0, 0);
        /* Compose: COMPANY + space + titleShort (ensure <=20) */
        {
            char line[21];
            int idx = 0;
            const char *a = companyName;
            while (*a && idx < 20) line[idx++] = *a++;
            if (idx < 20) line[idx++] = ' ';
            const char *b = titleShort;
            while (*b && idx < 20) line[idx++] = *b++;
            while (idx < 20) line[idx++] = ' ';
            line[20] = '\0';
            LCD_Print(line);
        }

        /* Row 1 (index 1): Iout / Cikis I */
		LCD_SetCursor(0, 1);
        LCD_Print(labelsShort[0]);
        LCD_PrintUInt16_1dp(adcIDC2);
        LCD_WriteChar(CH_CURR);

        /* Row 2 (index 2): Vout / Cikis V */
		LCD_SetCursor(0, 2);
        LCD_Print(labelsShort[1]);
        LCD_PrintUInt16_1dp(adcVBAT1);
        LCD_WriteChar('V');

        /* Status moved one row down: right side of row 1 */
        {
            LCD_SetCursor(STATUS_COL, 1);
            if (HAL_GPIO_ReadPin(SHUTDOWN2_GPIO_Port, SHUTDOWN2_Pin) == GPIO_PIN_SET)
            {
                LCD_Print(ui_get(UI_STR_OPEN));
            }
            else
            {
                LCD_Print(ui_get(UI_STR_CLOSE));
            }
        }

        /* If charger and output is on, show charge state under the status */
        {
            LCD_SetCursor(STATUS_COL, 2);
            if (operatingMode == MODE_CHARGER && HAL_GPIO_ReadPin(SHUTDOWN2_GPIO_Port, SHUTDOWN2_Pin) == GPIO_PIN_SET)
            {
                /* clear previous content and print stage */
                LCD_Print("       ");
                LCD_SetCursor(STATUS_COL, 2);
                LCD_Print(STAGE_NAMES_LANG[lcdLangId][batInfo.chargeState]);
            }
            else
            {
                /* clear area under status when not applicable */
                LCD_Print("       ");
            }
        }

        /* Row 3: Mains/Sebeke and Temp/Sic split across the line */
		LCD_SetCursor(0, 3);
        LCD_Print(labelsShort[2]); /* Sebeke/Mains */
        LCD_PrintUInt16(adcVAC);
        LCD_WriteChar('V');
        LCD_SetCursor(TEMP_COL, 3);
        LCD_Print(labelsShort[3]); /* Temp/Sic */
        LCD_PrintUInt16(adcTEMP);
        LCD_WriteChar('C');
    }
        break;

    case PAGE_MENU: {
        const char * title = ui_get(UI_STR_MENU_TITLE);
        const char * const * items = STR_MENU_ITEMS;

        /* Title (uppercase) */
        LCD_SetCursor(1, 0);
        {
            const char *t = title;
            while (*t)
            {
                char c = *t++;
                if (c >= 'a' && c <= 'z')
                {
                    c = (char)(c - 'a' + 'A');
                }
                LCD_WriteChar(c);
            }
        }

        /* Circular menu with centered '>' at row 2 (index 2).
           We show 3 items around the selected index: prev, current, next. */
        uint8_t total = 4u;
        uint8_t sel = (uint8_t)(menuIndex % total);
        uint8_t prev = (uint8_t)((sel + total - 1u) % total);
        uint8_t next = (uint8_t)((sel + 1u) % total);

        /* Row 1: previous item (leave col 0 empty) */
        LCD_SetCursor(1, 1);
        LCD_WriteChar((char)('1' + prev));
        LCD_WriteChar('.');
        LCD_Print(items[prev]);

        /* Row 2: current item with '>' at column 0 */
        LCD_SetCursor(0, 2);
        LCD_WriteChar('>');
        LCD_SetCursor(1, 2);
        LCD_WriteChar((char)('1' + sel));
        LCD_WriteChar('.');
        LCD_Print(items[sel]);

        /* Row 3: next item (leave col 0 empty) */
        LCD_SetCursor(1, 3);
        LCD_WriteChar((char)('1' + next));
        LCD_WriteChar('.');
        LCD_Print(items[next]);
    }
        break;

    case PAGE_ENTER_DATA: {
        /* Title */
        LCD_SetCursor(1, 0);
        {
            const char *t = ui_get(UI_STR_ENTER_DATA);
            while (*t) 
            { 
                char c = *t++;
                if (c >= 'a' && c <= 'z') 
                {
                    c = (char)(c - 'a' + 'A');
                }
                LCD_WriteChar(c);
            }
        }
        /* Scrolling list with centered '>' at row 2 */
        if (operatingMode == MODE_CHARGER) {
            uint8_t total = 6u; /* BatV, Capacity, Count, Safe, Soft, Equalize */
            uint8_t sel = (uint8_t)(subIndex % total);
            uint8_t prev = (uint8_t)((sel + total - 1u) % total);
            uint8_t next = (uint8_t)((sel + 1u) % total);
            /* row1 prev */
            LCD_SetCursor(1,1);
            if (prev == 0)
            {
                LCD_Print(STR_BATV);
                {
                    uint16_t batv;
                if (batInfo.batteryVoltage >= 24u)
                {
                    batv = 24u;
                }
                else
                {
                    batv = 12u;
                }
                    LCD_PrintUInt16(batv);
                    LCD_WriteChar('V');
                }
            }
            else if (prev == 1) 
			{ 
				LCD_Print(STR_CAPACITY);
				LCD_PrintUInt16_1dp(batInfo.batteryCap);
				LCD_Print("Ah"); 
			}
            else if (prev == 2)
            {
                LCD_Print(STR_COUNT);
                LCD_PrintUInt16(batInfo.numberOfBattery);
            }
            else if (prev == 3)
            {
                LCD_Print(ui_get(UI_STR_SAFE_CHARGE));
                LCD_Print(ui_get(batInfo.safeChargeEnabled ? UI_STR_OPEN : UI_STR_CLOSE));
            }
            else if (prev == 4)
            {
                LCD_Print(ui_get(UI_STR_SOFT_CHARGE));
                LCD_Print(ui_get(batInfo.softChargeEnabled ? UI_STR_OPEN : UI_STR_CLOSE));
            }
            else /* prev == 5 */
            {
                LCD_Print(ui_get(UI_STR_EQUALIZE));
                LCD_Print(ui_get(batInfo.equalizationEnabled ? UI_STR_OPEN : UI_STR_CLOSE));
            }
            /* row2 sel */
            LCD_SetCursor(0,2);
            LCD_WriteChar('>');
            LCD_SetCursor(1,2);
            if (sel == 0) {
                LCD_Print(STR_BATV);
                {
                    uint16_t batv;
                    if (batInfo.batteryVoltage >= 24u) { batv = 24u; } else { batv = 12u; }
                    LCD_PrintUInt16(batv);
                    LCD_WriteChar('V');
                }
            }
            else if (sel == 1)
            {
                LCD_Print(STR_CAPACITY);
                if (isEditing) LCD_WriteChar('[');
                LCD_PrintUInt16_1dp(batInfo.batteryCap);
                if (isEditing) LCD_WriteChar(']');
                LCD_Print("Ah");
            }
            else if (sel == 2)
            {
                LCD_Print(STR_COUNT);
                if (isEditing) LCD_WriteChar('[');
                LCD_PrintUInt16(batInfo.numberOfBattery);
                if (isEditing) LCD_WriteChar(']');
            }
            else if (sel == 3)
            {
                LCD_Print(ui_get(UI_STR_SAFE_CHARGE));
                LCD_Print(ui_get(batInfo.safeChargeEnabled ? UI_STR_OPEN : UI_STR_CLOSE));
            }
            else if (sel == 4)
            {
                LCD_Print(ui_get(UI_STR_SOFT_CHARGE));
                LCD_Print(ui_get(batInfo.softChargeEnabled ? UI_STR_OPEN : UI_STR_CLOSE));
            }
            else /* sel == 5 */
            {
                LCD_Print(ui_get(UI_STR_EQUALIZE));
                LCD_Print(ui_get(batInfo.equalizationEnabled ? UI_STR_OPEN : UI_STR_CLOSE));
            }
            /* row3 next */
            LCD_SetCursor(1,3);
            if (next == 0)
            {
                LCD_Print(STR_BATV);
                { 
                    uint16_t batv; 
                    if (batInfo.batteryVoltage >= 24u) { batv = 24u; } else { batv = 12u; }
                    LCD_PrintUInt16(batv);
                    LCD_WriteChar('V');
                } 
            }
            else if (next == 1)
            {
                LCD_Print(STR_CAPACITY);
                LCD_PrintUInt16_1dp(batInfo.batteryCap);
                LCD_Print("Ah"); 
            }
            else if (next == 2)
            {
                LCD_Print(STR_COUNT);
                LCD_PrintUInt16(batInfo.numberOfBattery);
            }
            else if (next == 3)
            {
                LCD_Print(ui_get(UI_STR_SAFE_CHARGE));
                LCD_Print(ui_get(batInfo.safeChargeEnabled ? UI_STR_OPEN : UI_STR_CLOSE));
            }
            else if (next == 4)
            {
                LCD_Print(ui_get(UI_STR_SOFT_CHARGE));
                LCD_Print(ui_get(batInfo.softChargeEnabled ? UI_STR_OPEN : UI_STR_CLOSE));
            }
            else /* next == 5 */
            {
                LCD_Print(ui_get(UI_STR_EQUALIZE));
                LCD_Print(ui_get(batInfo.equalizationEnabled ? UI_STR_OPEN : UI_STR_CLOSE));
            }
        }
        else
        {
            uint8_t total = 2u; /* V set, I max */
            uint8_t sel = (uint8_t)(subIndex % total);
            /* row1: blank if at top, else previous item */
            LCD_SetCursor(0,1);
            if (sel == 0) {
                LCD_Print("                    ");
            }
            else
            {
                LCD_SetCursor(1,1);
                LCD_Print("V set:");
                LCD_PrintUInt16_1dp(outputVSet_dV);
                LCD_WriteChar('V');
            }
            /* row2: selected */
            LCD_SetCursor(0,2);
            LCD_WriteChar('>');
            LCD_SetCursor(1,2);
            if (sel == 0)
            {
                LCD_Print("V set:");
                if (isEditing) LCD_WriteChar('[');
                LCD_PrintUInt16_1dp(outputVSet_dV);
                if (isEditing) LCD_WriteChar(']');
                LCD_WriteChar('V');
            }
            else
            {
                LCD_Print("I max:");
                if (isEditing) LCD_WriteChar('[');
                LCD_PrintUInt16_1dp(outputIMax_dA);
                if (isEditing) LCD_WriteChar(']');
                LCD_WriteChar(CH_CURR);
            }
            /* row3: blank if at bottom, else next item */
            LCD_SetCursor(0,3);
            if (sel == (uint8_t)(total-1u)) {
                LCD_Print("                    ");
            }
            else
            {
                LCD_SetCursor(1,3);
                LCD_Print("I max:");
                LCD_PrintUInt16_1dp(outputIMax_dA);
                LCD_WriteChar(CH_CURR);
            }
        }
    }
		break;

    case PAGE_OUTPUT_CONTROL: {
        LCD_SetCursor(1,0);
        {
            const char *t = ui_get(UI_STR_OUTPUT_CONTROL);
            while (*t)
            {
                char c = *t++;
                if (c >= 'a' && c <= 'z')
                {
                    c = (char)(c - 'a' + 'A');
                }
                LCD_WriteChar(c);
            }
        }
        if (operatingMode == MODE_CHARGER) {
            uint8_t total = 2u; /* Test V, Test I */
            uint8_t sel = (uint8_t)(subIndex % total);
            /* Row1: previous or blank if at top */
            LCD_SetCursor(0,1);
            if (sel == 0) {
                LCD_Print("                    ");
            }
            else
            {
                LCD_SetCursor(1,1);
                LCD_Print("Test V:");
                LCD_PrintUInt16_1dp(testVoltage_dV);
                LCD_WriteChar('V');
            }
            /* Row2: selected with edit highlight */
            LCD_SetCursor(0,2);
            LCD_WriteChar('>');
            LCD_SetCursor(1,2);
            if (sel == 0) {
                LCD_Print("Test V:");
                if (isEditing)
                {
                    LCD_WriteChar('[');
                }
                LCD_PrintUInt16_1dp(testVoltage_dV);
                if (isEditing)
                {
                    LCD_WriteChar(']');
                }
                LCD_WriteChar('V');
            }
            else
            {
                LCD_Print("Test I:");
                if (isEditing)
                {
                    LCD_WriteChar('[');
                }
                LCD_PrintUInt16_1dp(testCurrent_dA);
                if (isEditing)
                {
                    LCD_WriteChar(']');
                }
                LCD_WriteChar(CH_CURR);
            }
            /* Row3: next or blank if at bottom */
            LCD_SetCursor(0,3);
            if (sel == (uint8_t)(total-1u))
            {
                LCD_Print("                    ");
            }
            else
            {
                LCD_SetCursor(1,3);
                LCD_Print("Test I:");
                LCD_PrintUInt16_1dp(testCurrent_dA);
                LCD_WriteChar(CH_CURR);
            }
        }
        else
        {
            /* Only one item, keep it on row2 with marker; clear rows 1 and 3 */
            LCD_SetCursor(0,1);
            LCD_Print("                    ");
            LCD_SetCursor(0,2);
            LCD_WriteChar('>');
            LCD_SetCursor(1,2);
            LCD_Print(ui_get(UI_STR_SHORT_TEST)); 
			if (shortCircuitTest)
			{
				LCD_Print(ui_get(UI_STR_OPEN));
			} else
			{
				LCD_Print(ui_get(UI_STR_CLOSE));
			}
            LCD_SetCursor(0,3);
            LCD_Print("                    ");
        }
    }
        break;

    case PAGE_OPERATING_MODE: {
        LCD_SetCursor(1,0);
        {
            const char *t = ui_get(UI_STR_OPERATING_MODE);
            while (*t)
            {
                char c = *t++;
                if (c >= 'a' && c <= 'z')
                {
                    c = (char)(c - 'a' + 'A');
                }
                LCD_WriteChar(c);
            }
        }
        {
            uint8_t total = 2u; /* Charger, Supply */
            uint8_t sel = (uint8_t)(subIndex % total);
            /* row1: if sel is first, blank; else show previous */
            LCD_SetCursor(0,1);
            if (sel == 0) {
                LCD_Print("                    ");
            } else {
                LCD_SetCursor(1,1); 
				LCD_WriteChar('1'); 
				LCD_WriteChar('.'); 
				LCD_Print(ui_get(UI_STR_CHARGER_NAME));
            }
            /* row2: selected */
            LCD_SetCursor(0,2);
            LCD_WriteChar('>');
            LCD_SetCursor(1,2);
            if (sel==0) 
			{ 
				LCD_WriteChar('1'); 
				LCD_WriteChar('.'); 
				LCD_Print(ui_get(UI_STR_CHARGER_NAME)); 
			}
            else 
			{ 
				LCD_WriteChar('2'); 
				LCD_WriteChar('.'); 
				LCD_Print(ui_get(UI_STR_SUPPLY_NAME)); 
			}
            /* row3: if sel is last, blank; else show next */
            LCD_SetCursor(0,3);
            if (sel == (uint8_t)(total-1u)) {
                LCD_Print("                    ");
            } else {
                LCD_SetCursor(1,3);
                LCD_WriteChar('2');
                LCD_WriteChar('.');
                LCD_Print(ui_get(UI_STR_SUPPLY_NAME));
            }
        }
    }
		break;

    case PAGE_SETTINGS: {
        LCD_SetCursor(1,0);
        {
            const char *t = ui_get(UI_STR_SETTINGS);
            while (*t) { char c=*t++; if(c>='a'&&c<='z') c=(char)(c-'a'+'A'); LCD_WriteChar(c);}    
        }
        {
            uint8_t total = 3u; /* Lang, Bright, Mfg */
            uint8_t sel = (uint8_t)(subIndex % total);
            uint8_t prev = (uint8_t)((sel + total - 1u) % total);
            uint8_t next = (uint8_t)((sel + 1u) % total);
            /* row1 prev */
            LCD_SetCursor(1,1);
            if (prev == 0) {
                LCD_Print(ui_get(UI_STR_LANG));
                LCD_Print(STR_LANG_VAL);
            }
            else if (prev == 1) {
                LCD_Print(ui_get(UI_STR_BRIGHT));
                LCD_PrintUInt16(brightness);
                LCD_WriteChar('%');
            }
            else {
                LCD_Print(ui_get(UI_STR_MFG_MENU));
            }
            /* row2 sel */
            LCD_SetCursor(0,2);
            LCD_WriteChar('>');
            LCD_SetCursor(1,2);
            if (sel == 0) {
                LCD_Print(ui_get(UI_STR_LANG));
                LCD_Print(STR_LANG_VAL);
            } else if (sel == 1) {
                LCD_Print(ui_get(UI_STR_BRIGHT));
                if (isEditing)
                {
                    LCD_WriteChar('[');
                }
                LCD_PrintUInt16(brightness);
                if (isEditing)
                {
                    LCD_WriteChar(']');
                }
                LCD_WriteChar('%');
            } else {
                LCD_Print(ui_get(UI_STR_MFG_MENU));
            }
            /* row3 next */
            LCD_SetCursor(1,3);
            if (next == 0) {
                LCD_Print(ui_get(UI_STR_LANG));
                LCD_Print(STR_LANG_VAL);
            }
            else if (next == 1) {
                LCD_Print(ui_get(UI_STR_BRIGHT));
                LCD_PrintUInt16(brightness);
                LCD_WriteChar('%');
            }
            else {
                LCD_Print(ui_get(UI_STR_MFG_MENU));
            }
        }
    }
        break;

    case PAGE_MFG_PIN: {
        /* Row 0: centered title in uppercase */
        {
            const char *src = ui_get(UI_STR_MANUFACTURER);
            char up[21]; 
            uint8_t n = 0; 
            while (src[n] && n < 20) 
            { 
                char c = src[n]; 
                if (c >= 'a' && c <= 'z') 
                {
                    c = (char)(c - 'a' + 'A');
                }
                up[n] = c; 
                n++; 
            }
            up[n]='\0';
            uint8_t col = (uint8_t)((20u - n) / 2u);
            LCD_SetCursor(col, 0);
            LCD_Print(up);
        }
        /* Row 1: centered prompt or error */
        {
            const char *msg;
            if (mfgPinError && HAL_GetTick() < mfgPinErrorUntilMs) {
                msg = ui_get(UI_STR_WRONG_PIN);
            } else {
                mfgPinError = 0; msg = ui_get(UI_STR_ENTER_PIN);
            }
            uint8_t len=0; while (msg[len] && len<20) len++;
            uint8_t col = (uint8_t)((20u - len) / 2u);
            LCD_SetCursor(0, 1); LCD_Print("                    "); /* clear line */
            LCD_SetCursor(col, 1); LCD_Print(msg);
        }
        /* Row 2: centered 4 digits */
        {
            uint8_t start = 8u; /* (20-4)/2 */
            LCD_SetCursor(0, 2);
            LCD_Print("                    ");
            LCD_SetCursor(start, 2);
            for (uint8_t i = 0; i < 4; i++)
            {
                LCD_WriteChar((char)('0' + mfgPinInput[i]));
            }
        }
        /* Row 3: caret under current digit */
        {
            uint8_t start = 8u;
            LCD_SetCursor(0, 3);
            LCD_Print("                    ");
            LCD_SetCursor((uint8_t)(start + mfgPinPos), 3);
            LCD_WriteChar('^');
        }
    }
        break;

    case PAGE_MFG_MENU: {
        LCD_SetCursor(1,0);
        { 
            const char *t = ui_get(UI_STR_MANUFACTURER); 
            while(*t)
            {
                char c = *t++; 
                if (c >= 'a' && c <= 'z') 
                {
                    c = (char)(c - 'a' + 'A');
                }
                LCD_WriteChar(c);
            } 
        }
        LCD_SetCursor(0,2);
        LCD_Print(ui_get(UI_STR_FACTORY_PAGE));
        LCD_SetCursor(0,3);
        LCD_Print(ui_get(UI_STR_LEFT_EXIT));
    }
        break;

    /* duplicate blocks removed */

    default:
        break;
    }
}

/**
 * @brief Handle button press events
 * @details Processes buttonState and performs corresponding actions
 * 
 * Button mapping:
 * - Left: Previous page
 * - On: Set SHUTDOWN2 = 1
 * - Up: Toggle language EN/TR
 * - Down: No action
 * - Right: Next page
 * - Off: Set SHUTDOWN2 = 0
 */
void button_handle(void) {
    if (buttonState == 0) 
    {
        return;
    }

    /* Button order: Left, On, Up, Down, Right, Off */
    /* Structure by page for clarity (switch-case style) */
    if (buttonState & BUT_LEFT_M) 
    {
        if (isEditing) 
        {
            /* cancel: restore from backup depending on page */
            if (pageID == PAGE_ENTER_DATA) {
                if (operatingMode == MODE_CHARGER) {
                    if (subIndex==1) batInfo.batteryCap = (uint16_t)editBackupValue;
                    else if (subIndex==2) batInfo.numberOfBattery = (uint8_t)editBackupValue;
                } else {
                    if (subIndex==0) outputVSet_dV = editBackupValue;
                    else if (subIndex==1) outputIMax_dA = editBackupValue;
                }
            } else if (pageID == PAGE_OUTPUT_CONTROL) {
                if (operatingMode == MODE_CHARGER) {
                    if (subIndex==0) testVoltage_dV = editBackupValue;
                    else if (subIndex==1) testCurrent_dA = editBackupValue;
                }
            } else if (pageID == PAGE_SETTINGS) {
                if (subIndex==1) brightness = (uint8_t)editBackupValue;
            }
            isEditing = 0u; 
            uiNeedsClear = 1u; 
            buttonState = 0; 
            return;
        }
        if (pageID == PAGE_MFG_PIN) 
        {
            lcd_menu_set_page(PAGE_SETTINGS);
        } 
        else if (pageID == PAGE_MENU) 
        {
            lcd_menu_set_page(PAGE_MAIN);
        } 
        else if (pageID == PAGE_MAIN) 
        {
            /* no action on main page */
        } 
        else if (pageID == PAGE_ENTER_DATA || pageID == PAGE_OUTPUT_CONTROL || pageID == PAGE_OPERATING_MODE || pageID == PAGE_SETTINGS) 
        {
            lcd_menu_set_page(PAGE_MENU);
        } 
        else 
        {
            /* other pages: no action unless specified later */
        }
    }
    /* On: set SHUTDOWN2 = 1 (same on all pages) */
    if (buttonState & BUT_ON_M) {
        HAL_GPIO_WritePin(SHUTDOWN2_GPIO_Port, SHUTDOWN2_Pin, GPIO_PIN_SET);
        deviceOn = 1;
        batInfo.chargeState = STATE_BULK;
    }
    /* Off: set SHUTDOWN2 = 0 (same on all pages) 
	*/
    if (buttonState & BUT_OFF_M) {
        HAL_GPIO_WritePin(SHUTDOWN2_GPIO_Port, SHUTDOWN2_Pin, GPIO_PIN_RESET);
        deviceOn = 0;
        dacValueI = 0;
        dacValueV = 0;
    }

    /* Up/Down behavior depends on page */
    switch (pageID) {
    case PAGE_MENU:
        if (buttonState & BUT_UP_M) {
            menuIndex = (uint8_t)((menuIndex + 4u - 1u) % 4u);
        }
        if (buttonState & BUT_DOWN_M) {
            menuIndex = (uint8_t)((menuIndex + 1u) % 4u);
        }
        break;
    case PAGE_ENTER_DATA:
        if (isEditing) {
            /* Up/Down adjust values */
            if (operatingMode == MODE_CHARGER) {
                if (buttonState & BUT_UP_M) {
                    if (subIndex == 0) { batInfo.batteryVoltage = (batInfo.batteryVoltage >= 24u) ? 12u : 24u; }
                    else if (subIndex == 1 && batInfo.batteryCap < 990) { batInfo.batteryCap += 10; }
                    else if (subIndex == 2 && batInfo.numberOfBattery < 24) { batInfo.numberOfBattery++; }
                }
                if (buttonState & BUT_DOWN_M) {
                    if (subIndex == 0) { batInfo.batteryVoltage = (batInfo.batteryVoltage >= 24u) ? 12u : 24u; }
                    else if (subIndex == 1 && batInfo.batteryCap > 9) { batInfo.batteryCap -= 10; }
                    else if (subIndex == 2 && batInfo.numberOfBattery > 1) { batInfo.numberOfBattery--; }
                }
            } else { /* MODE_SUPPLY */
                if (buttonState & BUT_UP_M) {
                    if (subIndex == 0 && outputVSet_dV < 240) { outputVSet_dV++; }
                    else if (subIndex == 1 && outputIMax_dA < 400) { outputIMax_dA++; }
                }
                if (buttonState & BUT_DOWN_M) {
                    if (subIndex == 0 && outputVSet_dV > 0) { outputVSet_dV--; }
                    else if (subIndex == 1 && outputIMax_dA > 0) { outputIMax_dA--; }
                }
            }
        } else {
            /* Navigate fields with Up/Down */
            uint8_t total;
            if (operatingMode == MODE_CHARGER) { total = 6u; } else { total = 2u; }
            if (buttonState & BUT_UP_M) 
            { 
                subIndex = (uint8_t)((subIndex + total - 1u) % total); 
            }
            if (buttonState & BUT_DOWN_M) 
            { 
                subIndex = (uint8_t)((subIndex + 1u) % total); 
            }
        }
        break;
    case PAGE_OUTPUT_CONTROL:
        if (isEditing) {
            if (operatingMode == MODE_CHARGER) {
                if (buttonState & BUT_UP_M) {
                    if (subIndex == 0) { testVoltage_dV++; }
                    else if (subIndex == 1) { testCurrent_dA++; }
                }
                if (buttonState & BUT_DOWN_M) {
                    if (subIndex == 0 && testVoltage_dV > 0) { testVoltage_dV--; }
                    else if (subIndex == 1 && testCurrent_dA > 0) { testCurrent_dA--; }
                }
            } else {
                if (buttonState & BUT_UP_M) { shortCircuitTest ^= 1u; }
                if (buttonState & BUT_DOWN_M) { shortCircuitTest ^= 1u; }
            }
        } else {
            if (operatingMode == MODE_CHARGER) {
                uint8_t total = 2u;
                if (buttonState & BUT_UP_M) 
                { 
                    subIndex = (uint8_t)((subIndex + total - 1u) % total); 
                }
                if (buttonState & BUT_DOWN_M) 
                { 
                    subIndex = (uint8_t)((subIndex + 1u) % total); 
                }
            } else {
                /* only one item: no navigation */
            }
        }
        break;
    case PAGE_OPERATING_MODE:
        if (isEditing) {
            if (buttonState & BUT_UP_M) { operatingMode = MODE_CHARGER; }
            if (buttonState & BUT_DOWN_M) { operatingMode = MODE_SUPPLY; }
        } else {
            if (buttonState & BUT_UP_M) 
            { 
                subIndex = (uint8_t)((subIndex + 2u - 1u) % 2u); 
            }
            if (buttonState & BUT_DOWN_M) 
            { 
                subIndex = (uint8_t)((subIndex + 1u) % 2u); 
            }
        }
        break;
    case PAGE_SETTINGS:
        if (isEditing) {
            if (subIndex == 1) {
                if (buttonState & BUT_UP_M) 
                { 
                    if (brightness < 100) 
                    {
                        brightness++; 
                    }
                }
                if (buttonState & BUT_DOWN_M) 
                { 
                    if (brightness > 0) 
                    {
                        brightness--; 
                    }
                }
            }
        } else {
            if (buttonState & BUT_UP_M) 
            { 
                subIndex = (uint8_t)((subIndex + 3u - 1u) % 3u); 
            }
            if (buttonState & BUT_DOWN_M) 
            { 
                subIndex = (uint8_t)((subIndex + 1u) % 3u); 
            }
        }
        break;
    case PAGE_MFG_PIN:
        if (buttonState & BUT_UP_M) 
        {
            if (mfgPinInput[mfgPinPos] < 9) 
            {
                mfgPinInput[mfgPinPos]++;
            }
        }
        if (buttonState & BUT_DOWN_M) 
        {
            if (mfgPinInput[mfgPinPos] > 0) 
            {
                mfgPinInput[mfgPinPos]--;
            }
        }
        if (buttonState & BUT_LEFT_M) 
        {
            lcd_menu_set_page(PAGE_SETTINGS);
        }
        if (buttonState & BUT_RIGHT_M) {
            /* 2s hold to validate */
            GPIO_PinState rightNow = HAL_GPIO_ReadPin(B5_GPIO_Port, B5_Pin);
            if (rightNow == GPIO_PIN_SET) {
                if (rightPressStartMs == 0u) rightPressStartMs = HAL_GetTick();
                if ((HAL_GetTick() - rightPressStartMs) >= 1000u) {
                    uint16_t entered = (uint16_t)(mfgPinInput[0]*1000 + mfgPinInput[1]*100 + mfgPinInput[2]*10 + mfgPinInput[3]);
                    if (entered == mfgPinCode) {
                        mfgPinError = 0; 
                        rightPressStartMs = 0u; 
                        lcd_menu_set_page(PAGE_MFG_MENU);
                    } else {
                        mfgPinError = 1; 
                        mfgPinErrorUntilMs = HAL_GetTick() + 2000u; 
                        rightPressStartMs = 0u; /* stay on PIN, show error 2s */
                    }
                }
            } else {
                rightPressStartMs = 0u;
            }
        }
        break;
    case PAGE_MFG_MENU:
        if (buttonState & BUT_LEFT_M) 
        {
            lcd_menu_set_page(PAGE_SETTINGS);
        }
        break;
    default:
        break;
    }

    /* Right: navigate/enter/edit */
    if (buttonState & BUT_RIGHT_M) {
        if (pageID == PAGE_MAIN) {
            lcd_menu_set_page(PAGE_MENU);
        } else if (pageID == PAGE_LOADING) {
            lcd_menu_set_page(PAGE_MAIN);
        } else if (pageID == PAGE_MENU) {
            /* Enter selected subpage */
            subIndex = 0;
            switch (menuIndex) {
            case 0:
                lcd_menu_set_page(PAGE_ENTER_DATA);
                break;
            case 1:
                lcd_menu_set_page(PAGE_OUTPUT_CONTROL);
                break;
            case 2:
                lcd_menu_set_page(PAGE_OPERATING_MODE);
                break;
            case 3:
                lcd_menu_set_page(PAGE_SETTINGS);
                break;
            default:
                break;
            }
        } else if (pageID == PAGE_ENTER_DATA) {
            /* Immediate toggle for selection fields; edit for numeric */
            if (operatingMode == MODE_CHARGER && subIndex == 0u) {
                /* Bat V immediate toggle */
                batInfo.batteryVoltage = (batInfo.batteryVoltage >= 24u) ? 12u : 24u;
            } else if (operatingMode == MODE_CHARGER && (subIndex == 3u || subIndex == 4u || subIndex == 5u)) {
                /* Safe/Soft/Equalize toggle directly */
                if (subIndex == 3u)
                {
                    batInfo.safeChargeEnabled ^= 1u;
                }
                else if (subIndex == 4u)
                {
                    batInfo.softChargeEnabled ^= 1u;
                }
                else
                {
                    batInfo.equalizationEnabled ^= 1u;
                }
            } else {
                if (!isEditing) {
                    /* enter edit mode and backup current value */
                    if (operatingMode == MODE_CHARGER) {
                        if (subIndex == 1u) 
                        { 
                            editBackupValue = (uint16_t)batInfo.batteryCap; 
                        }
                        else 
                        { 
                            editBackupValue = (uint16_t)batInfo.numberOfBattery; 
                        }
                    } else {
                        if (subIndex == 0u) 
                        { 
                            editBackupValue = outputVSet_dV; 
                        }
                        else 
                        { 
                            editBackupValue = outputIMax_dA; 
                        }
                    }
                    isEditing = 1u;
                } else {
                    /* second Right: save and exit edit mode */
                    isEditing = 0u;
                }
            }
        } else if (pageID == PAGE_OUTPUT_CONTROL) {
            if (operatingMode == MODE_CHARGER) {
                if (!isEditing) {
                    if (subIndex == 0u) 
                    { 
                        editBackupValue = testVoltage_dV; 
                    }
                    else 
                    { 
                        editBackupValue = testCurrent_dA; 
                    }
                    isEditing = 1u;
                } else {
                    isEditing = 0u; /* save current values */
                }
            } else {
                /* Supply: Short test immediate toggle */
                shortCircuitTest ^= 1u;
            }
        } else if (pageID == PAGE_SETTINGS) {
            /* Language: toggle, Manufacturer: go to PIN page, Brightness: edit */
            if (subIndex == 0u) {
                lcd_menu_set_language((uint8_t)(lcdLangId ^ 1u));
            } else if (subIndex == 1u) {
                if (!isEditing) 
                { 
                    editBackupValue = brightness; 
                    isEditing = 1u; 
                }
                else 
                { 
                    isEditing = 0u; 
                }
            } else {
                mfgPinPos = 0; 
                mfgPinInput[0] = mfgPinInput[1] = mfgPinInput[2] = mfgPinInput[3] = 0;
                mfgPinError = 0;
                lcd_menu_set_page(PAGE_MFG_PIN);
            }
        } else if (pageID == PAGE_OPERATING_MODE) {
            /* Sağ: seçili öğeye göre modu ATA ve ana sayfaya dön */
            if (subIndex == 0u) 
            { 
                operatingMode = MODE_CHARGER; 
            }
            else 
            { 
                operatingMode = MODE_SUPPLY; 
            }
            lcd_menu_set_page(PAGE_MAIN);
        } else if (pageID == PAGE_MFG_PIN) {
            /* Sağ kısa: bir sonraki haneye geçsin */
            mfgPinPos = (uint8_t)((mfgPinPos + 1u) % 4u);
        }
    }

    uiNeedsClear = 1; /* clear-once after any button handling */
    buttonState = 0;
}
