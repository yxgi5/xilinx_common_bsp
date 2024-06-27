#include "../bsp.h"

#if 	defined (XPAR_MODBUS_RTU_0_AXI_GPIO_0_DEVICE_ID) && \
		defined (XPAR_MODBUS_RTU_0_AXI_UARTLITE_0_DEVICE_ID)

XGpio XGpioInstRs485Ctl;
XUartLite UartLiteRs485;

int rs485_heir_xgpio_setup()
{
	int Status ;

	Status = XGpio_Initialize(&XGpioInstRs485Ctl, XPAR_MODBUS_RTU_0_AXI_GPIO_0_DEVICE_ID) ;
	if (Status != XST_SUCCESS)
	{
		bsp_printf(TXT_RED "In %s: XGpio_Initialize failed...\r\n" TXT_RST, __func__);
		return XST_FAILURE;
	}

//	XGpio_SetDataDirection(&XGpioInstRs485Ctl, 1, 0x0); // output
//	XGpio_DiscreteWrite(&XGpioInstRs485Ctl, 1, 1); // 485 as output
//	XGpio_DiscreteWrite(&XGpioInstRs485Ctl, 1, 0); // 485 as input

	return XST_SUCCESS;
}

#if(STDOUT_BASEADDRESS != XPAR_MODBUS_RTU_0_AXI_UARTLITE_0_BASEADDR)
int rs485_heir_xuart_setup()
{
	int Status;

	Status = XUartLite_Initialize(&UartLiteRs485, XPAR_MODBUS_RTU_0_AXI_UARTLITE_0_DEVICE_ID);
	if (Status != XST_SUCCESS)
	{
		bsp_printf(TXT_RED "In %s: XUartLite_Initialize failed...\r\n" TXT_RST, __func__);
		return XST_FAILURE;
	}

	return XST_SUCCESS;
}
#else
#if defined (MODBUS_RTU_SLAVE)
#error "Cannot use uart as print io and modbus at the same time"
#endif
#endif // STDOUT_BASEADDRESS != XPAR_RS485_HEIR_0_AXI_UARTLITE_0_BASEADDR

#endif // XPAR_MODBUS_RTU_0_AXI_GPIO_0_DEVICE_ID && XPAR_MODBUS_RTU_0_AXI_UARTLITE_0_DEVICE_ID


/*
usage:

assume you have a RS485_heir, and using it as std print
call follows before main_loop

```
#if defined (XPAR_RS485_HEIR_0_AXI_GPIO_0_DEVICE_ID)
    Status = rs485_heir_xgpio_setup();
    if (Status != XST_SUCCESS)
	{
		Xil_Assert(__FILE__, __LINE__);
		return XST_FAILURE ;
	}
    UartLiteRs485_Output();
#endif // XPAR_RS485_HEIR_AXI_GPIO_0_DEVICE_ID

```
then you can control input/output when you need, by call UartLiteRs485_Output()/UartLiteRs485_Input()




assume you have a modbus_rtu_0 heir,
don't need call these code as uartlite_fifo will call them, and set input in initial


*/


