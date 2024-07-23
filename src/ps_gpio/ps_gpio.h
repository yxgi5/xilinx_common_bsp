#ifndef __PS_GPIO_H__

#if defined (XPAR_XGPIOPS_NUM_INSTANCES)
#define __PS_GPIO_H__
#include "xgpiops.h"

#define RELAY_CUTOFF        (78+0)
#define PL_LOCK_LED         (78+1)
#define EEPROM_WP			(78+2)
#define SIL_RSTN			(78+3)
#define CH0_LOCK			(78+4)
#define CH0_ERRB			(78+5)
#define CH1_LOCK			(78+6)
#define CH1_ERRB			(78+7)
#define CH2_LOCK			(78+8)
#define CH2_ERRB			(78+9)
#define CH3_LOCK			(78+10)
#define CH3_ERRB			(78+11)
#define DES_PWR_EN			(78+12)

extern XGpioPs Gpio;

int PsGpioSetup(XGpioPs *InstancePtr, u16 DeviceId);

#endif // XPAR_XGPIOPS_NUM_INSTANCES

#endif // __PS_GPIO_H__

