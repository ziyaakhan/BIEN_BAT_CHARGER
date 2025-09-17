/**
 * @file lcdMenu.h
 * @brief LCD Menu System for Battery Charger
 * @details Provides menu navigation, language support, and button handling
 * @author Ziya
 * @date 2025
 */

#ifndef INC_LCDMENU_H_
#define INC_LCDMENU_H_

#include <stdint.h>

/** @name Button Bit Positions
 * @brief Button positions in buttonState bitfield for readability
 */
/**@{*/
#define BUT_LEFT_POS   0  /**< Left button bit position */
#define BUT_ON_POS     1  /**< On button bit position */
#define BUT_UP_POS     2  /**< Up button bit position */
#define BUT_DOWN_POS   3  /**< Down button bit position */
#define BUT_RIGHT_POS  4  /**< Right button bit position */
#define BUT_OFF_POS    5  /**< Off button bit position */
/**@}*/

/* Manufacturer menu PIN selection via compile-time define */
#define BIENDEBUG
#ifdef BIENDEBUG
#define MFG_MENU_PIN 0000
#else
#define MFG_MENU_PIN 1234
#endif

/** @name Button Masks
 * @brief Button masks for checking buttonState
 */
/**@{*/
#define BUT_LEFT_M   (1U << BUT_LEFT_POS)   /**< Left button mask */
#define BUT_ON_M     (1U << BUT_ON_POS)     /**< On button mask */
#define BUT_UP_M     (1U << BUT_UP_POS)     /**< Up button mask */
#define BUT_DOWN_M   (1U << BUT_DOWN_POS)   /**< Down button mask */
#define BUT_RIGHT_M  (1U << BUT_RIGHT_POS)  /**< Right button mask */
#define BUT_OFF_M    (1U << BUT_OFF_POS)    /**< Off button mask */
/**@}*/

/** @name Compatibility Aliases
 * @brief Legacy button masks for backward compatibility
 */
/**@{*/
#define BUT1_M BUT_LEFT_M   /**< Legacy button 1 mask */
#define BUT2_M BUT_ON_M     /**< Legacy button 2 mask */
#define BUT3_M BUT_UP_M     /**< Legacy button 3 mask */
#define BUT4_M BUT_DOWN_M   /**< Legacy button 4 mask */
#define BUT5_M BUT_RIGHT_M  /**< Legacy button 5 mask */
#define BUT6_M BUT_OFF_M    /**< Legacy button 6 mask */
/**@}*/

/** @name Page Definitions
 * @brief Available menu pages
 */
/**@{*/
#define PAGE_LOADING 0  /**< Loading/splash screen page */
#define PAGE_MAIN    1  /**< Main display page with measurements */
#define PAGE_MENU    2  /**< Menu page with selectable items */
#define PAGE_ENTER_DATA     3  /**< Enter Data page */
#define PAGE_OUTPUT_CONTROL 4  /**< Output Control page */
#define PAGE_OPERATING_MODE 5  /**< Operating Mode selection page */
#define PAGE_SETTINGS       6  /**< Settings page */
#define PAGE_MFG_PIN        7  /**< Manufacturer PIN entry page */
#define PAGE_MFG_MENU       8  /**< Manufacturer menu page */
#define PAGE_MFG_GAIN       9  /**< Manufacturer Gain settings page */
/**@}*/

/**
 * @brief Operating modes
 */
typedef enum {
    MODE_CHARGER = 0,  /**< Battery Charger mode */
    MODE_SUPPLY  = 1   /**< Power Supply mode */
} OperatingMode;

/**
 * @brief Initialize the LCD menu system
 * @details Sets up initial page and clears display
 */
void lcd_menu_init(void);

/**
 * @brief Set the current menu page
 * @param page Page ID (PAGE_LOADING or PAGE_MAIN)
 */
void lcd_menu_set_page(uint8_t page);

/**
 * @brief Set the display language
 * @param lang_id Language ID (0: English, 1: Turkish)
 */
void lcd_menu_set_language(uint8_t lang_id);

/**
 * @brief Handle LCD display rendering
 * @details Renders the current page based on pageID and language
 */
void lcd_handle(void);

/**
 * @brief Handle button press events
 * @details Processes buttonState and performs corresponding actions
 */
void button_handle(void);

/** @name Shared State Variables
 * @brief Global state variables for menu system
 */
/**@{*/
extern uint8_t pageID;        /**< Current page ID */
extern uint8_t buttonState;   /**< Button state bitfield */
extern uint8_t lcdLangId;     /**< Language ID (0: EN, 1: TR) */
extern uint8_t uiNeedsClear;  /**< UI refresh flag */
extern OperatingMode operatingMode; /**< Current operating mode */
extern uint8_t menuIndex;           /**< Current menu selection index */
extern uint8_t subIndex;            /**< Current subpage selection index */
extern const char * companyName;    /**< Company name shown on loading/title */

/**
 * @brief Device names per language and mode (pointer-to-pointer)
 * @details Access as DEVICE_NAMES_LANG[lcdLangId][operatingMode]
 */
extern const char * const * DEVICE_NAMES_LANG[2];
extern const char * const DEVICE_NAMES_EN[2];
extern const char * const DEVICE_NAMES_TR[2];

/**
 * @brief Short title names per language and mode for main title (<= 20 chars with company)
 * @details Access as TITLE_NAMES_LANG[lcdLangId][operatingMode]
 */
extern const char * const * TITLE_NAMES_LANG[2];
extern const char * const TITLE_NAMES_EN[2];
extern const char * const TITLE_NAMES_TR[2];

/* UI parameters and PIN state exposed if needed by other modules */
extern uint8_t brightness;           /**< 0..100 */
extern uint16_t mfgPinCode;          /**< Manufacturer PIN (default 0000) */
extern uint8_t mfgPinInput[4];       /**< Current PIN entry digits */
extern uint8_t mfgPinPos;            /**< Current cursor pos 0..3 for PIN */
extern uint8_t mfgPinError;          /**< Last PIN state: 1 wrong */
/**@}*/

#endif /* INC_LCDMENU_H_ */
