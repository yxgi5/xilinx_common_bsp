#ifndef __PS_GPIO_H__

#if defined (XPAR_XGPIOPS_NUM_INSTANCES)
#define __PS_GPIO_H__
#include "xgpiops.h"


/*
 *	Max pins in the ZynqMP GPIO device	ZU+
 *	0 - 25,  Bank 0		MIO
 *	26 - 51, Bank 1		MIO
 *	52 - 77, Bank 2		MIO
 *	78 - 109, Bank 3	EMIO
 *	110 - 141, Bank 4	EMIO
 *	142 - 173, Bank 5	EMIO
 */

/*
 *	Max pins in the GPIO device	ZYNQ
 *	0 - 31,  Bank 0		MIO
 *	32 - 53, Bank 1		MIO
 *	54 - 85, Bank 2		EMIO
 *	86 - 117, Bank 3	EMIO
 */


#define RELAY_CUTOFF        (78+0)
#define SIL_RSTN			(78+1)
#define EEPROM_WP			(78+2)
#define PL_LOCK_LED         (78+3)
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
void XGpioPs_TogglePin(const XGpioPs *InstancePtr, u32 Pin);

#endif // XPAR_XGPIOPS_NUM_INSTANCES

#endif // __PS_GPIO_H__

