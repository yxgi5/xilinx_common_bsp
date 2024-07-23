#ifndef __AXI_TIMER_H_

#if defined (XPAR_XTMRCTR_NUM_INSTANCES)
#define __AXI_TIMER_H_
//#include "xparameters.h"
#include "xtmrctr.h"
//#include "xil_exception.h"
//#include "xintc.h"
//#include <stdio.h>

//#define INTC		XIntc
//#define INTC_DEVICE_ID		XPAR_INTC_0_DEVICE_ID
//#define INTC_HANDLER	XIntc_InterruptHandler

#if (XPAR_XTMRCTR_NUM_INSTANCES >= 1U)
extern XTmrCtr axi_timer0;
#endif // (XPAR_XTMRCTR_NUM_INSTANCES >= 1U)

#if (XPAR_XTMRCTR_NUM_INSTANCES >= 2U)
extern XTmrCtr axi_timer1;
#endif // (XPAR_XTMRCTR_NUM_INSTANCES >= 2U)

#if (XPAR_XTMRCTR_NUM_INSTANCES >= 3U)
extern XTmrCtr axi_timer2;
#endif // (XPAR_XTMRCTR_NUM_INSTANCES >= 3U)

#if (XPAR_XTMRCTR_NUM_INSTANCES >= 4U)
extern XTmrCtr axi_timer3;
#endif // (XPAR_XTMRCTR_NUM_INSTANCES >= 4U)

#if (XPAR_XTMRCTR_NUM_INSTANCES >= 5U)
extern XTmrCtr axi_timer4;
#endif // (XPAR_XTMRCTR_NUM_INSTANCES >= 5U)

#if (XPAR_XTMRCTR_NUM_INSTANCES >= 6U)
extern XTmrCtr axi_timer5;
#endif // (XPAR_XTMRCTR_NUM_INSTANCES >= 6U)

#if (XPAR_XTMRCTR_NUM_INSTANCES >= 7U)
extern XTmrCtr axi_timer6;
#endif // (XPAR_XTMRCTR_NUM_INSTANCES >= 7U)

#if (XPAR_XTMRCTR_NUM_INSTANCES >= 8U)
XTmrCtr axi_timer7;
#endif // (XPAR_XTMRCTR_NUM_INSTANCES >= 8U)


void XTmrCtr_SetCallBack(XTmrCtr * InstancePtr, void * _pCallBack);

#define TIMER_CNTR_0	 0
#define TIMER_CNTR_1	 1

//#define RESET_VALUE_0	 0xFF67697F
//#define RESET_VALUE_0	 0xFD050F7F
//#define RESET_VALUE_0	 0xFA0A1EFF

#if defined (XPAR_ETHERNET_SUBSYSTEM_AXI_TIMER_0_DEVICE_ID)
// 250ms for tcp
//#define TIMER_TLR (25000000*((float)MHZ/100))
#define TIMER_TLR 25000000*(((float)XPAR_ETHERNET_SUBSYSTEM_AXI_TIMER_0_CLOCK_FREQ_HZ/1000000)/100)
int timer0_init();
#endif //#if defined (XPAR_ETHERNET_SUBSYSTEM_AXI_TIMER_0_DEVICE_ID)

#if defined (INTC_DEVICE_ID) || defined (INTC_CONNECT)
#if defined (XPAR_MODBUS_RTU_0_AXI_TIMER_0_DEVICE_ID)
#if defined (XPAR_ETHERNET_SUBSYSTEM_AXI_TIMER_0_DEVICE_ID)
int timer1_init();
void StartHardTimer1(uint32_t _uiTimeOut);
#else
int timer0_init();
void StartHardTimer0(uint32_t _uiTimeOut);
#endif // #if define (XPAR_ETHERNET_SUBSYSTEM_AXI_TIMER_0_DEVICE_ID)
#endif // #if defined (XPAR_MODBUS_RTU_0_AXI_TIMER_0_DEVICE_ID)
#endif // #if defined (INTC_DEVICE_ID) || defined (INTC_CONNECT)



#endif // XPAR_XTMRCTR_NUM_INSTANCES

#endif // __AXI_TIMER_H_
