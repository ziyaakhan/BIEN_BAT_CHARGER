/*
 * button.h
 *
 *  Created on: Sep 8, 2025
 *      Author: Ziya
 */

#ifndef INC_BUTTON_H_
#define INC_BUTTON_H_


#define BUT1 0
#define BUT2 1
#define BUT3 2
#define BUT4 3
#define BUT5 4
#define BUT6 5

#define BUT1_M (1U << BUT1)
#define BUT2_M (1U << BUT2)
#define BUT3_M (1U << BUT3)
#define BUT4_M (1U << BUT4)
#define BUT5_M (1U << BUT5)
#define BUT6_M (1U << BUT6)

extern uint8_t buttonState;

#endif /* INC_BUTTON_H_ */
