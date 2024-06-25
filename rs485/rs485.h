#ifndef __RS485_H__

#if 	defined (XPAR_MODBUS_RTU_0_AXI_GPIO_0_DEVICE_ID) && \
		defined (XPAR_MODBUS_RTU_0_AXI_UARTLITE_0_DEVICE_ID)
#define __RS485_H__
#include "xil_types.h"
#include "xgpio.h"
#include "xuartlite.h"


#define RS485CTL 	XPAR_MODBUS_RTU_0_AXI_GPIO_0_BASEADDR
#define UartLiteRs485_Input()  do{ Xil_Out32(RS485CTL, 0); } while(0)
#define UartLiteRs485_Output() do{ Xil_Out32(RS485CTL, 1); } while(0)

extern XGpio XGpioInstRs485Ctl;
extern XUartLite UartLiteRs485;

int rs485_heir_xgpio_setup();

#if(STDOUT_BASEADDRESS != XPAR_PROCESSOR_SUBSYSTEM_RS485_HEIR_0_AXI_UARTLITE_0_BASEADDR)
int rs485_heir_xuart_setup();
#endif // STDOUT_BASEADDRESS != XPAR_RS485_HEIR_0_AXI_UARTLITE_0_BASEADDR

#endif // __RS485_H__

#endif // XPAR_MODBUS_RTU_0_AXI_GPIO_0_DEVICE_ID && XPAR_MODBUS_RTU_0_AXI_UARTLITE_0_DEVICE_ID


