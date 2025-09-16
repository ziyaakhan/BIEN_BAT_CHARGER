/*
 * out_control.h
 *
 *  Created on: Sep 12, 2025
 *      Author: Ziya
 */

#ifndef INC_OUT_CONTROL_H_
#define INC_OUT_CONTROL_H_

#include "main.h"
#include <stdint.h>
#include "lcdMenu.h"  /* for OperatingMode enum */

#define PID_VOUT_KP 15000
#define PID_VOUT_KI 0
#define PID_VOUT_KD 0

#define PID_VOUT_OUT_MAX  200 //2000
#define PID_VOUT_OUT_MIN -200 //0

#define PID_VOUT_INT_MAX  2000 //2000
#define PID_VOUT_INT_MIN -2000 //0
//-------------------------//
#define PID_IOUT_KP 10000
#define PID_IOUT_KI 0

#define PID_IOUT_KD 0

#define PID_IOUT_OUT_MAX  200 //2000
#define PID_IOUT_OUT_MIN -200 //0

#define PID_IOUT_INT_MAX  1 //2000
#define PID_IOUT_INT_MIN -1 //0

/* Control parameters exposed to other modules */
extern OperatingMode operatingMode;     /* MODE_CHARGER / MODE_SUPPLY */
extern uint8_t  batteryVoltageSel;      /* 0:12V, 1:24V */
extern uint16_t batteryCapacityAh;      /* 1..999 Ah */
extern uint8_t  batteryCount;           /* 1..24 */
extern uint8_t  safeChargeOn;           /* 0/1 */
extern uint8_t  softChargeOn;           /* 0/1 */
extern uint8_t  voltageEqualOn;         /* 0/1 */
extern uint16_t testVoltage_dV;         /* 0.1V units */
extern uint16_t testCurrent_dA;         /* 0.1A units */
extern uint16_t outputVSet_dV;          /* 0.1V units */
extern uint16_t outputIMax_dA;          /* 0.1A units */
extern uint8_t  shortCircuitTest;       /* 0/1 */
extern uint8_t deviceOn;

typedef struct
{
    const  int Kp;
    const  int Ki;
    const  int Kd;

    int setpoint;
    int measured;

    int error;
    int prevError;

    int integral;
    int derivative;

    int output;

    int outputMin;
    int outputMax;
}PIDController;

typedef enum {
    STATE_BULK,
    STATE_BATTERY_SAFE,
    STATE_ABSORPTION,
    STATE_EQUALIZATION,
    STATE_FLOAT,
    STATE_STORAGE,
    STATE_REFRESH
} ChargeState_t;

typedef struct
{
    unsigned int batteryVoltage;
    unsigned int batteryCap;
    unsigned int numberOfBattery;
    unsigned int bulkCurrent;
    unsigned int floatVoltage;
    unsigned int absorptionVoltage;
    unsigned int absorptionFinishCurrent;
    unsigned int storageVoltage;
    unsigned int safeVoltage;
    unsigned int safeStepMV;
    unsigned char softChargeEnabled;
    unsigned char safeChargeEnabled;
    unsigned char equalizationEnabled;
    ChargeState_t chargeState;
    unsigned char chargeMinute;
    unsigned char chargeHour;
    unsigned char chargeDay;
    unsigned char chargeWeek;
}BATTERY_INFO;

extern BATTERY_INFO batInfo;
extern PIDController pidVout;
extern PIDController pidIout;

extern int PID_Compute(PIDController *pid, unsigned long setpoint, unsigned long measured);
extern void outCalculation();

#endif /* INC_OUT_CONTROL_H_ */
