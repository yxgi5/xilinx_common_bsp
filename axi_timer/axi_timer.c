#include "../bsp.h"

#if defined (XPAR_XTMRCTR_NUM_INSTANCES)

////INTC InterruptController;  /* The instance of the Interrupt Controller */
//XTmrCtr TimerCounterInst;   /* The instance of the Timer Counter */
////extern volatile u16 adc;
////extern uint8_t g_adc_trigger;
//volatile uint8_t g_tmr0_trigger;
//extern uint8_t g_mods_timeout;



#if (XPAR_XTMRCTR_NUM_INSTANCES >= 1U)
XTmrCtr axi_timer0;
#endif // (XPAR_XTMRCTR_NUM_INSTANCES >= 1U)

#if (XPAR_XTMRCTR_NUM_INSTANCES >= 2U)
XTmrCtr axi_timer1;
#endif // (XPAR_XTMRCTR_NUM_INSTANCES >= 2U)

#if (XPAR_XTMRCTR_NUM_INSTANCES >= 3U)
XTmrCtr axi_timer2;
#endif // (XPAR_XTMRCTR_NUM_INSTANCES >= 3U)

#if (XPAR_XTMRCTR_NUM_INSTANCES >= 4U)
XTmrCtr axi_timer3;
#endif // (XPAR_XTMRCTR_NUM_INSTANCES >= 4U)

#if (XPAR_XTMRCTR_NUM_INSTANCES >= 5U)
XTmrCtr axi_timer4;
#endif // (XPAR_XTMRCTR_NUM_INSTANCES >= 5U)

#if (XPAR_XTMRCTR_NUM_INSTANCES >= 6U)
XTmrCtr axi_timer5;
#endif // (XPAR_XTMRCTR_NUM_INSTANCES >= 6U)

#if (XPAR_XTMRCTR_NUM_INSTANCES >= 7U)
XTmrCtr axi_timer6;
#endif // (XPAR_XTMRCTR_NUM_INSTANCES >= 7U)

#if (XPAR_XTMRCTR_NUM_INSTANCES >= 8U)
axi_timer7;
#endif // (XPAR_XTMRCTR_NUM_INSTANCES >= 8U)




/*****************************************************************************/
/**
* This function is the handler which performs processing for the timer counter.
* It is called from an interrupt context such that the amount of processing
* performed should be minimized.  It is called when the timer counter expires
* if interrupts are enabled.
*
* This handler provides an example of how to handle timer counter interrupts
* but is application specific.
*
* @param	CallBackRef is a pointer to the callback function
* @param	TmrCtrNumber is the number of the timer to which this
*		handler is associated with.
*
* @return	None.
*
* @note		None.
*
******************************************************************************/
//void TimerHandler(void *CallBackRef, u8 TmrCtrNumber)
//{
//	XTmrCtr *InstancePtr = (XTmrCtr *)CallBackRef;
//
//	/*
//	 * Check if the timer counter has expired, checking is not necessary
//	 * since that's the reason this function is executed, this just shows
//	 * how the callback reference can be used as a pointer to the instance
//	 * of the timer counter that expired, increment a shared variable so
//	 * the main thread of execution can see the timer expired
//	 */
//	if (XTmrCtr_IsExpired(InstancePtr, TmrCtrNumber)) {
//		if (TmrCtrNumber == TIMER_CNTR_0) {
////			Status = adc_read(&ret);
////			if(Status == XST_SUCCESS)
////			{
////				adc=(int)(((float)ret/4096)*3300);
////				//asm("nop");
////			}
//			g_tmr0_trigger = 1;
//		}
//		if (TmrCtrNumber == TIMER_CNTR_1) {
//			g_mods_timeout = 1;
////			XTmrCtr_SetOptions(InstancePtr, TmrCtrNumber, 0);
//			XTmrCtr_Stop(InstancePtr, TmrCtrNumber);
//		}
//	}
//}


void XTmrCtr_SetCallBack(XTmrCtr * InstancePtr, void * _pCallBack)
{
	Xil_AssertVoid(InstancePtr != NULL);
	Xil_AssertVoid(_pCallBack != NULL);
	InstancePtr->CallBackRef=_pCallBack;
}

#if defined (XPAR_ETHERNET_SUBSYSTEM_AXI_TIMER_0_DEVICE_ID)
#if defined (UDP_UPDATE) || defined (TCP_UPDATE) || defined (TCP_COMMAND_SRV) || defined (UDP_COMMAND_SRV)
int timer0_init()
{
	int Status = XST_SUCCESS;

	/*
	 * Initialize the timer counter so that it's ready to use,
	 * specify the device ID that is generated in xparameters.h
	 */
	Status = XTmrCtr_Initialize(&axi_timer0, XPAR_TMRCTR_0_DEVICE_ID);
	if (Status != XST_SUCCESS) {
		return XST_FAILURE;
	}

	XTmrCtr_SetOptions(&axi_timer0, TIMER_CNTR_0, XTC_INT_MODE_OPTION | XTC_AUTO_RELOAD_OPTION);
	XTmrCtr_SetResetValue(&axi_timer0, TIMER_CNTR_0, 0xffffffff - TIMER_TLR);
//	XTmrCtr_SetOptions(&axi_timer0, TIMER_CNTR_0, XTC_INT_MODE_OPTION | XTC_AUTO_RELOAD_OPTION | XTC_DOWN_COUNT_OPTION);
//	XTmrCtr_SetResetValue(&axi_timer0, TIMER_CNTR_0, TIMER_TLR);

//	XTmrCtr_SetOptions(&axi_timer0, TIMER_CNTR_1, XTC_INT_MODE_OPTION);

#if defined (INTC_DEVICE_ID) || defined (INTC_CONNECT)
	/*
	 * Connect a device driver handler that will be called when an interrupt
	 * for the device occurs, the device driver handler performs the
	 * specific interrupt processing for the device
	 */
	XTmrCtr_SetHandler(&axi_timer0, Timer0Handler, &axi_timer0);

	Status = INTC_CONNECT(&InterruptController, XPAR_PROCESSOR_SUBSYSTEM_MICROBLAZE_0_AXI_INTC_ETHERNET_SUBSYSTEM_AXI_TIMER_0_INTERRUPT_INTR,
				(XInterruptHandler)XTmrCtr_InterruptHandler,
				(void *)&axi_timer0);

	if (Status != XST_SUCCESS) {
		return XST_FAILURE;
	}

	// Don't forget Enable the interrupt for the specific interrupt source in proper place
	XIntc_Enable(&InterruptController, XPAR_PROCESSOR_SUBSYSTEM_MICROBLAZE_0_AXI_INTC_ETHERNET_SUBSYSTEM_AXI_TIMER_0_INTERRUPT_INTR);
#endif // #if defined (INTC_DEVICE_ID) || defined (INTC_CONNECT)

	// Don't forget start timer in proper place
	XTmrCtr_Start(&axi_timer0, TIMER_CNTR_0);
//	XTmrCtr_Start(&axi_timer0, TIMER_CNTR_1);

	return Status;
}
#endif //#if defined (UDP_UPDATE) || defined (TCP_UPDATE) || defined (TCP_COMMAND_SRV) || defined (UDP_COMMAND_SRV)
#endif //#if defined (XPAR_ETHERNET_SUBSYSTEM_AXI_TIMER_0_DEVICE_ID)

/*****************************************************************************/
/**
* This function launch a oneshot timer.
*
*
* @param	_uiTimeOut us
*
* @return	None.
*
* @note		None.
*
******************************************************************************/
#if defined (INTC_DEVICE_ID) || defined (INTC_CONNECT)
#if defined (XPAR_MODBUS_RTU_0_AXI_TIMER_0_DEVICE_ID) && defined (XPAR_ETHERNET_SUBSYSTEM_AXI_TIMER_0_DEVICE_ID)
int timer1_init()
{
	int Status = XST_SUCCESS;

	/*
	 * Initialize the timer counter so that it's ready to use,
	 * specify the device ID that is generated in xparameters.h
	 */
	Status = XTmrCtr_Initialize(&axi_timer1, XPAR_MODBUS_RTU_0_AXI_TIMER_0_DEVICE_ID);
	if (Status != XST_SUCCESS) {
		return XST_FAILURE;
	}

	XTmrCtr_SetOptions(&axi_timer1, TIMER_CNTR_0, XTC_INT_MODE_OPTION);
	XTmrCtr_SetResetValue(&axi_timer1, TIMER_CNTR_0, 0xffffffff - TIMER_TLR);

	/*
	 * Connect a device driver handler that will be called when an interrupt
	 * for the device occurs, the device driver handler performs the
	 * specific interrupt processing for the device
	 */
	XTmrCtr_SetHandler(&axi_timer1, Timer1Handler, &axi_timer1);

	Status = INTC_CONNECT(&InterruptController, XPAR_PROCESSOR_SUBSYSTEM_MICROBLAZE_0_AXI_INTC_MODBUS_RTU_0_AXI_TIMER_0_INTERRUPT_INTR,
				(XInterruptHandler)XTmrCtr_InterruptHandler,
				(void *)&axi_timer1);

	if (Status != XST_SUCCESS) {
		return XST_FAILURE;
	}

	// Don't forget Enable the interrupt for the specific interrupt source in proper place
	XIntc_Enable(&InterruptController, XPAR_PROCESSOR_SUBSYSTEM_MICROBLAZE_0_AXI_INTC_MODBUS_RTU_0_AXI_TIMER_0_INTERRUPT_INTR);

	// Don't forget start timer in proper place
//	XTmrCtr_Start(&axi_timer0, TIMER_CNTR_0);


	return Status;
}
void StartHardTimer1(uint32_t _uiTimeOut)
{

//	Xil_AssertVoid(_pCallBack != NULL);

//	TimerCounterInst.CallBackRef=_pCallBack;

//	XTmrCtr_Stop(&TimerCounterInst, TIMER_CNTR_1);

	XTmrCtr_SetResetValue(&axi_timer1, TIMER_CNTR_0, 0xffffffff-_uiTimeOut);

	XTmrCtr_Start(&axi_timer1, TIMER_CNTR_0);
}

#endif // #if defined (XPAR_MODBUS_RTU_0_AXI_TIMER_0_DEVICE_ID) && defined (XPAR_ETHERNET_SUBSYSTEM_AXI_TIMER_0_DEVICE_ID)

#if defined (XPAR_MODBUS_RTU_0_AXI_TIMER_0_DEVICE_ID) && !defined (XPAR_ETHERNET_SUBSYSTEM_AXI_TIMER_0_DEVICE_ID)
int timer0_init()
{
	int Status = XST_SUCCESS;

	/*
	 * Initialize the timer counter so that it's ready to use,
	 * specify the device ID that is generated in xparameters.h
	 */
	Status = XTmrCtr_Initialize(&axi_timer0, XPAR_MODBUS_RTU_0_AXI_TIMER_0_DEVICE_ID);
	if (Status != XST_SUCCESS) {
		return XST_FAILURE;
	}

	XTmrCtr_SetOptions(&axi_timer0, TIMER_CNTR_0, XTC_INT_MODE_OPTION);

	/*
	 * Connect a device driver handler that will be called when an interrupt
	 * for the device occurs, the device driver handler performs the
	 * specific interrupt processing for the device
	 */
	XTmrCtr_SetHandler(&axi_timer0, Timer0Handler, &axi_timer0);
#if defined (__MICROBLAZE__)
	Status = INTC_CONNECT(&InterruptController, XPAR_PROCESSOR_SUBSYSTEM_MICROBLAZE_0_AXI_INTC_MODBUS_RTU_0_AXI_TIMER_0_INTERRUPT_INTR,
				(XInterruptHandler)XTmrCtr_InterruptHandler,
				(void *)&axi_timer0);
#else
	Status = INTC_CONNECT(&InterruptController, XPAR_FABRIC_MODBUS_RTU_0_AXI_TIMER_0_INTERRUPT_INTR,
				(XInterruptHandler)XTmrCtr_InterruptHandler,
				(void *)&axi_timer0);
#endif // #if defined (__MICROBLAZE__)
	if (Status != XST_SUCCESS) {
		return XST_FAILURE;
	}

#if defined (__MICROBLAZE__)
	// Don't forget Enable the interrupt for the specific interrupt source in proper place
	XIntc_Enable(&InterruptController, XPAR_PROCESSOR_SUBSYSTEM_MICROBLAZE_0_AXI_INTC_MODBUS_RTU_0_AXI_TIMER_0_INTERRUPT_INTR);
#else
	XScuGic_Enable(&InterruptController, XPAR_FABRIC_MODBUS_RTU_0_AXI_TIMER_0_INTERRUPT_INTR);
#endif // #if defined (__MICROBLAZE__)

	// Don't forget start timer in proper place
//	XTmrCtr_Start(&axi_timer0, TIMER_CNTR_0);


	return Status;
}
void StartHardTimer0(uint32_t _uiTimeOut)
{

//	Xil_AssertVoid(_pCallBack != NULL);

//	TimerCounterInst.CallBackRef=_pCallBack;

//	XTmrCtr_Stop(&TimerCounterInst, TIMER_CNTR_1);

	XTmrCtr_SetResetValue(&axi_timer0, TIMER_CNTR_0, 0xffffffff-_uiTimeOut);

	XTmrCtr_Start(&axi_timer0, TIMER_CNTR_0);
}
#endif // #if defined (XPAR_MODBUS_RTU_0_AXI_TIMER_0_DEVICE_ID) && !defined (XPAR_ETHERNET_SUBSYSTEM_AXI_TIMER_0_DEVICE_ID)


#endif // #if defined (INTC_DEVICE_ID) || defined (INTC_CONNECT)

#endif //#if defined (XPAR_XTMRCTR_NUM_INSTANCES)


