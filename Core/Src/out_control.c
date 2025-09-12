/*
 * out_control.c
 *
 *  Created on: Sep 12, 2025
 *      Author: Ziya
 */

#include "out_control.h"
#include "adc.h"

/* Own the control variables here */
OperatingMode operatingMode = MODE_CHARGER;
uint8_t  batteryVoltageSel = 0;
uint16_t batteryCapacityAh = 60;
uint8_t  batteryCount = 1;
uint8_t  safeChargeOn = 0;
uint8_t  softChargeOn = 0;
uint8_t  voltageEqualOn = 0;
uint16_t testVoltage_dV = 120;
uint16_t testCurrent_dA = 50;
uint16_t outputVSet_dV = 120;
uint16_t outputIMax_dA = 100;
uint8_t  shortCircuitTest = 0;
uint8_t  deviceOn = 0;

PIDController pidVout =
{
  .Kp = PID_VOUT_KP,
  .Ki = PID_VOUT_KI,
  .Kd = PID_VOUT_KD,

  .setpoint = 0,
  .measured = 0,

  .error = 0,
  .prevError = 0,
  .integral = 0,
  .derivative = 0,

  .output = 0,

  .outputMin = PID_VOUT_OUT_MIN,
  .outputMax = PID_VOUT_OUT_MAX,
};

PIDController pidIout =
{
  .Kp = PID_IOUT_KP,
  .Ki = PID_IOUT_KI,
  .Kd = PID_IOUT_KD,

  .setpoint = 0,
  .measured = 0,

  .error = 0,
  .prevError = 0,
  .integral = 0,
  .derivative = 0,

  .output = 0,

  .outputMin = PID_IOUT_OUT_MIN,
  .outputMax = PID_IOUT_OUT_MAX,
};

BATTERY_INFO batInfo = {
    .batteryVoltage             = 120,
    .batteryCap                 = 90,
    .numberOfBattery            = 1,
    .bulkCurrent                = 9,
    .floatVoltage               = 135,
    .absorptionVoltage          = 144,
    .absorptionFinishCurrent    = 1,
    .storageVoltage             = 132,
    .safeVoltage                = 144,
    .safeStepMV                 = 7,

    .softChargeEnabled          = 0,
    .safeChargeEnabled          = 0,
    .equalizationEnabled        = 0,

    .chargeState                = STATE_BULK,
    .chargeMinute               = 0,
    .chargeHour                 = 0,
    .chargeDay                  = 0,
    .chargeWeek                 = 0,
};

int PID_Compute(PIDController *pid, unsigned long setpoint, unsigned long measured)
{
    pid->setpoint = setpoint;
    pid->measured = measured;

    pid->error = pid->setpoint - pid->measured ;

    pid->integral += pid->error;
    pid->derivative = pid->error - pid->prevError;
    pid->prevError = pid->error;


    if (pid->integral > PID_VOUT_INT_MAX)
        pid->integral = PID_VOUT_INT_MAX;
    else if (pid->integral < PID_VOUT_INT_MIN)
        pid->integral = PID_VOUT_INT_MIN;


    pid->output = pid->Kp * pid->error +
                  pid->Ki * pid->integral +
                  pid->Kd * pid->derivative;

    pid->output = pid->output >> 12;

    if (pid->output > pid->outputMax)
        pid->output = pid->outputMax;
    else if (pid->output < pid->outputMin)
        pid->output = pid->outputMin;

    return pid->output;
}

/* Place for future control loop using adc measurements + above parameters */
