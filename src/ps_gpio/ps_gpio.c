/*
 * ps_gpio.c
 *
 *  Created on: 2023年4月28日
 *      Author: fengke
 */

#include "../bsp.h"
#if defined (XPAR_XGPIOPS_NUM_INSTANCES)

XGpioPs Gpio;

int PsGpioSetup(XGpioPs *InstancePtr, u16 DeviceId)
{
	XGpioPs_Config *GPIO_CONFIG ;
	int Status ;

	GPIO_CONFIG = XGpioPs_LookupConfig(DeviceId) ;

	Status = XGpioPs_CfgInitialize(InstancePtr, GPIO_CONFIG, GPIO_CONFIG->BaseAddr) ;
	if (Status != XST_SUCCESS)
	{
		bsp_printf(TXT_RED "XGpioPs_CfgInitialize failed...\r\n" TXT_RST);
		return XST_FAILURE ;
	}
	/* set as output */

	XGpioPs_SetDirectionPin(InstancePtr, RELAY_CUTOFF, 1) ;
	XGpioPs_SetOutputEnablePin(InstancePtr, RELAY_CUTOFF, 1);
	XGpioPs_WritePin(&Gpio, RELAY_CUTOFF, 0) ; // RELAY_CUTOFF: 0-keep; 1-cutoff

	XGpioPs_SetDirectionPin(InstancePtr, PL_LOCK_LED, 1) ;
	XGpioPs_SetOutputEnablePin(InstancePtr, PL_LOCK_LED, 1);
	XGpioPs_WritePin(&Gpio, PL_LOCK_LED, 0) ;

	XGpioPs_SetDirectionPin(InstancePtr, EEPROM_WP, 1) ;
	XGpioPs_SetOutputEnablePin(InstancePtr, EEPROM_WP, 1);
	XGpioPs_WritePin(&Gpio, EEPROM_WP, 0) ; // EEPROM write protect off

	XGpioPs_SetDirectionPin(InstancePtr, SIL_RSTN, 1) ;
	XGpioPs_SetOutputEnablePin(InstancePtr, SIL_RSTN, 1);
	XGpioPs_WritePin(&Gpio, SIL_RSTN, 1) ;

	XGpioPs_SetDirectionPin(InstancePtr, CH0_LOCK, 0) ;
	XGpioPs_SetOutputEnablePin(InstancePtr, CH0_LOCK, 1);
	XGpioPs_WritePin(&Gpio, CH0_LOCK, 1) ;
	XGpioPs_SetDirectionPin(InstancePtr, CH0_ERRB, 0) ;
	XGpioPs_SetOutputEnablePin(InstancePtr, CH0_ERRB, 1);
	XGpioPs_WritePin(&Gpio, CH0_ERRB, 1) ;

	XGpioPs_SetDirectionPin(InstancePtr, CH1_LOCK, 0) ;
	XGpioPs_SetOutputEnablePin(InstancePtr, CH1_LOCK, 1);
	XGpioPs_WritePin(&Gpio, CH1_LOCK, 1) ;
	XGpioPs_SetDirectionPin(InstancePtr, CH1_ERRB, 0) ;
	XGpioPs_SetOutputEnablePin(InstancePtr, CH1_ERRB, 1);
	XGpioPs_WritePin(&Gpio, CH1_ERRB, 1) ;

	XGpioPs_SetDirectionPin(InstancePtr, CH2_LOCK, 0) ;
	XGpioPs_SetOutputEnablePin(InstancePtr, CH2_LOCK, 1);
	XGpioPs_WritePin(&Gpio, CH2_LOCK, 1) ;
	XGpioPs_SetDirectionPin(InstancePtr, CH2_ERRB, 0) ;
	XGpioPs_SetOutputEnablePin(InstancePtr, CH2_ERRB, 1);
	XGpioPs_WritePin(&Gpio, CH2_ERRB, 1) ;

	XGpioPs_SetDirectionPin(InstancePtr, CH3_LOCK, 0) ;
	XGpioPs_SetOutputEnablePin(InstancePtr, CH3_LOCK, 1);
	XGpioPs_WritePin(&Gpio, CH3_LOCK, 1) ;
	XGpioPs_SetDirectionPin(InstancePtr, CH3_ERRB, 0) ;
	XGpioPs_SetOutputEnablePin(InstancePtr, CH3_ERRB, 1);
	XGpioPs_WritePin(&Gpio, CH3_ERRB, 1) ;

	XGpioPs_SetDirectionPin(InstancePtr, DES_PWR_EN, 1) ;
	XGpioPs_SetOutputEnablePin(InstancePtr, DES_PWR_EN, 1);
	XGpioPs_WritePin(&Gpio, DES_PWR_EN, 1) ;

	return XST_SUCCESS ;
}

/****************************************************************************/
/**
*
* Write data to the specified pin.
*
* @param	InstancePtr is a pointer to the XGpioPs instance.
* @param	Pin is the pin number to which the Data is to be written.
*		Valid values are 0-117 in Zynq and 0-173 in Zynq Ultrascale+ MP.
*
* @return	None.
*
* @note		This function does a masked write to the specified pin of
*		the specified GPIO bank. The previous state of other pins
*		is maintained.
*
*****************************************************************************/
void XGpioPs_TogglePin(const XGpioPs *InstancePtr, u32 Pin)
{
	u32 RegOffset;
	u32 Value;
	u8 Bank;
	u8 PinNumber;
	u32 DataVar;

	Xil_AssertVoid(InstancePtr != NULL);
	Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);
	Xil_AssertVoid(Pin < InstancePtr->MaxPinNum);

	/* Get the Bank number and Pin number within the bank. */
#ifdef versal
	XGpioPs_GetBankPin(InstancePtr,(u8)Pin, &Bank, &PinNumber);
#else
	XGpioPs_GetBankPin((u8)Pin, &Bank, &PinNumber);
#endif

	if (PinNumber > 15U) {
		/* There are only 16 data bits in bit maskable register. */
		PinNumber -= (u8)16;
		RegOffset = XGPIOPS_DATA_MSW_OFFSET;
	} else {
		RegOffset = XGPIOPS_DATA_LSW_OFFSET;
	}


	DataVar = (XGpioPs_ReadReg(InstancePtr->GpioConfig.BaseAddr,
				 ((u32)(Bank) * XGPIOPS_DATA_BANK_OFFSET) +
				 XGPIOPS_DATA_RO_OFFSET) >> (u32)PinNumber) & (u32)1;
	/*
	 * Get the 32 bit value to be written to the Mask/Data register where
	 * the upper 16 bits is the mask and lower 16 bits is the data.
	 */
	FLPB(DataVar,BIT32(0));
	DataVar &= (u32)0x01;
	Value = ~((u32)1 << (PinNumber + 16U)) & ((DataVar << PinNumber) | 0xFFFF0000U);
	XGpioPs_WriteReg(InstancePtr->GpioConfig.BaseAddr,
			  ((u32)(Bank) * XGPIOPS_DATA_MASK_OFFSET) +
			  RegOffset, Value);

}

#endif // XPAR_XGPIOPS_NUM_INSTANCES

