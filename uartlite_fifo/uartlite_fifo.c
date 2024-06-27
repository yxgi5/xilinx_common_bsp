#include "../bsp.h"
#if defined (XPAR_XUARTLITE_NUM_INSTANCES) && defined (INTC_DEVICE_ID) && defined (INTC)

#if (UART0_FIFO_EN == 1) && defined (XPAR_UARTLITE_0_DEVICE_ID)

#if defined (MODBUS_RTU_SLAVE)
#if !defined (__RS485_H__)
#error "No rs485 heir in design"
#endif
#else
XUartLite UartLite0;            /* The instance of the UartLite Device */
#endif // MODBUS_RTU_SLAVE

UART_T g_tUart0;
static uint8_t g_TxBuf0[UART0_TX_BUF_SIZE];		/* 发送缓冲区 */
static uint8_t g_RxBuf0[UART0_RX_BUF_SIZE];		/* 接收缓冲区 */
#endif // #if (UART0_FIFO_EN == 1) && defined (XPAR_UARTLITE_0_DEVICE_ID)


#if (UART1_FIFO_EN == 1) && defined (XPAR_UARTLITE_1_DEVICE_ID)
XUartLite UartLite1;            /* The instance of the UartLite Device */
UART_T g_tUart1;
static uint8_t g_TxBuf1[UART1_TX_BUF_SIZE];		/* 发送缓冲区 */
static uint8_t g_RxBuf1[UART1_RX_BUF_SIZE];		/* 接收缓冲区 */
#endif // #if (UART1_FIFO_EN == 1) && defined (XPAR_UARTLITE_1_DEVICE_ID)


#if (UART2_FIFO_EN == 2) && defined (XPAR_UARTLITE_2_DEVICE_ID)
XUartLite UartLite2;            /* The instance of the UartLite Device */
UART_T g_tUart2;
static uint8_t g_TxBuf2[UART2_TX_BUF_SIZE];		/* 发送缓冲区 */
static uint8_t g_RxBuf2[UART2_RX_BUF_SIZE];		/* 接收缓冲区 */
#endif // #if (UART2_FIFO_EN == 2) && defined (XPAR_UARTLITE_2_DEVICE_ID)


#if (UART3_FIFO_EN == 3) && defined (XPAR_UARTLITE_3_DEVICE_ID)
XUartLite UartLite3;            /* The instance of the UartLite Device */
UART_T g_tUart3;
static uint8_t g_TxBuf3[UART3_TX_BUF_SIZE];		/* 发送缓冲区 */
static uint8_t g_RxBuf3[UART3_RX_BUF_SIZE];		/* 接收缓冲区 */
#endif // #if (UART3_FIFO_EN == 3) && defined (XPAR_UARTLITE_3_DEVICE_ID)


#if (UART4_FIFO_EN == 4) && defined (XPAR_UARTLITE_4_DEVICE_ID)
XUartLite UartLite4;            /* The instance of the UartLite Device */
UART_T g_tUart4;
static uint8_t g_TxBuf4[UART4_TX_BUF_SIZE];		/* 发送缓冲区 */
static uint8_t g_RxBuf4[UART4_RX_BUF_SIZE];		/* 接收缓冲区 */
#endif // #if (UART4_FIFO_EN == 4) && defined (XPAR_UARTLITE_4_DEVICE_ID)


#if (UART5_FIFO_EN == 5) && defined (XPAR_UARTLITE_5_DEVICE_ID)
XUartLite UartLite5;            /* The instance of the UartLite Device */
UART_T g_tUart5;
static uint8_t g_TxBuf5[UART5_TX_BUF_SIZE];		/* 发送缓冲区 */
static uint8_t g_RxBuf5[UART5_RX_BUF_SIZE];		/* 接收缓冲区 */
#endif // #if (UART5_FIFO_EN == 5) && defined (XPAR_UARTLITE_5_DEVICE_ID)


#if (UART6_FIFO_EN == 6) && defined (XPAR_UARTLITE_6_DEVICE_ID)
XUartLite UartLite6;            /* The instance of the UartLite Device */
UART_T g_tUart6;
static uint8_t g_TxBuf6[UART6_TX_BUF_SIZE];		/* 发送缓冲区 */
static uint8_t g_RxBuf6[UART6_RX_BUF_SIZE];		/* 接收缓冲区 */
#endif // #if (UART6_FIFO_EN == 6) && defined (XPAR_UARTLITE_6_DEVICE_ID)


#if (UART7_FIFO_EN == 7) && defined (XPAR_UARTLITE_7_DEVICE_ID)
XUartLite UartLite7;            /* The instance of the UartLite Device */
UART_T g_tUart7;
static uint8_t g_TxBuf7[UART7_TX_BUF_SIZE];		/* 发送缓冲区 */
static uint8_t g_RxBuf7[UART7_RX_BUF_SIZE];		/* 接收缓冲区 */
#endif // #if (UART7_FIFO_EN == 7) && defined (XPAR_UARTLITE_7_DEVICE_ID)

void Uart0RxCpltIRQ(UART_T *_pUart)
{
		/* 从串口接收数据寄存器读取数据存放到接收FIFO */
		uint8_t ch;
		ch = XUartLite_ReadReg(_pUart->uart->RegBaseAddress, XUL_RX_FIFO_OFFSET);
		//ch = XUartPs_ReadReg(_pUart->uart->Config.BaseAddress, XUARTPS_FIFO_OFFSET);
		_pUart->pRxBuf[_pUart->usRxWrite] = ch;
		if (++_pUart->usRxWrite >= _pUart->usRxBufSize)
		{
			_pUart->usRxWrite = 0;
		}
		if (_pUart->usRxCount < _pUart->usRxBufSize)
		{
			_pUart->usRxCount++;
		}

		/* 回调函数,通知应用程序收到新数据,一般是发送1个消息或者设置一个标记 */
		//if (_pUart->usRxWrite == _pUart->usRxRead)
		//if (_pUart->usRxCount == 1)
		{
			if (_pUart->ReciveNew)
			{
				_pUart->ReciveNew(ch); /* 比如，交给MODBUS解码程序处理字节流 */
			}
		}
}

void Uart0TxCpltIRQ(UART_T *_pUart)
{
	if(_pUart->usTxCount != 0)
	{
		_pUart->usTxCount--;
	}
	while (_pUart->usTxCount != 0)
	{
		/* 从发送FIFO取1个字节写入串口发送数据寄存器 */
		XUartLite_Send(_pUart->uart, &_pUart->pTxBuf[_pUart->usTxRead], 1);
		//USART_SendData(_pUart->uart, _pUart->pTxBuf[_pUart->usTxRead]);
		if (++_pUart->usTxRead >= _pUart->usTxBufSize)
		{
			_pUart->usTxRead = 0;
		}
		_pUart->usTxCount--;
	}
	//else
	if (_pUart->usTxCount == 0)
	{
		u8 StatusRegister;
		StatusRegister =
					XUartLite_GetStatusReg(_pUart->uart->RegBaseAddress);
		/* 回调函数, 一般用来处理RS485通信，将RS485芯片设置为接收模式，避免抢占总线 */
		while ((StatusRegister & XUL_SR_TX_FIFO_EMPTY) == 0)
		{
			StatusRegister =
								XUartLite_GetStatusReg(_pUart->uart->RegBaseAddress);
		}

		if (_pUart->SendOver)
		{
			_pUart->SendOver();
		}
	}
}

/*
 * uart 中断处理函数 XUartLite_InterruptHandler
 */
void Uart0IntrHandler(void *CallBackRef)
{
	XUartLite *InstancePtr = (XUartLite *) CallBackRef;

	u32 IsrStatus;
	u8 StatusRegisterVal;

	Xil_AssertVoid(InstancePtr != NULL);

	/*
	 * Read the status register to determine which, coulb be both
	 * interrupt is active
	 */
	IsrStatus = XUartLite_ReadReg(InstancePtr->RegBaseAddress,
					XUL_STATUS_REG_OFFSET);

	if ((IsrStatus & (XUL_SR_RX_FIFO_FULL |
		XUL_SR_RX_FIFO_VALID_DATA)) != 0) {
		//ReceiveDataHandler(InstancePtr);
		//XUartLite_ReceiveBuffer
		//XUartLite_ReceiveBuffer
		/*
		 * Enter a critical region by disabling all the UART interrupts to allow
		 * this call to stop a previous operation that may be interrupt driven
		 */
		StatusRegisterVal = XUartLite_GetStatusReg(InstancePtr->RegBaseAddress);
		XUartLite_WriteReg(InstancePtr->RegBaseAddress,
					XUL_CONTROL_REG_OFFSET, 0);

		u8 StatusRegister;
		/*
		 * Read the Status Register to determine if there is any data in
		 * the receiver/FIFO
		 */
		StatusRegister =
			XUartLite_GetStatusReg(InstancePtr->RegBaseAddress);
		while(StatusRegister & XUL_SR_RX_FIFO_VALID_DATA)
		{
			Uart0RxCpltIRQ(&g_tUart0);
			StatusRegister =
						XUartLite_GetStatusReg(InstancePtr->RegBaseAddress);
		}

		/*
		 * Restore the interrupt enable register to it's previous value such
		 * that the critical region is exited
		 */
		StatusRegisterVal &= XUL_CR_ENABLE_INTR;
		XUartLite_WriteReg(InstancePtr->RegBaseAddress,
					XUL_CONTROL_REG_OFFSET, StatusRegisterVal);

	}

	if ((IsrStatus & XUL_SR_TX_FIFO_EMPTY) != 0)
	{
		u8 StatusRegister;
		u8 IntrEnableStatus;
		//SendDataHandler(InstancePtr);
		//XUartLite_SendBuffer
		//XUartLite_SendBuffer
		/*
		 * Enter a critical region by disabling all the UART interrupts to allow
		 * this call to stop a previous operation that may be interrupt driven
		 */
		StatusRegister = XUartLite_GetStatusReg(InstancePtr->RegBaseAddress);
		XUartLite_WriteReg(InstancePtr->RegBaseAddress,
					XUL_CONTROL_REG_OFFSET, 0);

		/*
		 * Save the status register contents to restore the interrupt enable
		 * register to it's previous value when that the critical region is
		 * exited
		 */
		IntrEnableStatus = StatusRegister;
		if ((StatusRegister & XUL_SR_TX_FIFO_FULL) == 0)
		{
			Uart0TxCpltIRQ(&g_tUart0);
			StatusRegister =
						XUartLite_GetStatusReg(InstancePtr->RegBaseAddress);
		}

		/*
		 * Restore the interrupt enable register to it's previous value such
		 * that the critical region is exited
		 */
		IntrEnableStatus &= XUL_CR_ENABLE_INTR;
		XUartLite_WriteReg(InstancePtr->RegBaseAddress, XUL_CONTROL_REG_OFFSET,
					IntrEnableStatus);
	}
}

void Uart0VarInit(void)
{
	g_tUart0.Intc = &InterruptController;

#if defined (MODBUS_RTU_SLAVE)
#if !defined (__RS485_H__)
#error "No rs485 heir in design"
#endif
	g_tUart0.uart = &UartLiteRs485;						/* 串口设备 */
#else
	g_tUart0.uart = &UartLite0;						/* 串口设备 */
#endif

	g_tUart0.pTxBuf = g_TxBuf0;					/* 发送缓冲区指针 */
	g_tUart0.pRxBuf = g_RxBuf0;					/* 接收缓冲区指针 */
	g_tUart0.usTxBufSize = UART0_TX_BUF_SIZE;	/* 发送缓冲区大小 */
	g_tUart0.usRxBufSize = UART0_RX_BUF_SIZE;	/* 接收缓冲区大小 */
	g_tUart0.usTxWrite = 0;						/* 发送FIFO写索引 */
	g_tUart0.usTxRead = 0;						/* 发送FIFO读索引 */
	g_tUart0.usRxWrite = 0;						/* 接收FIFO写索引 */
	g_tUart0.usRxRead = 0;						/* 接收FIFO读索引 */
	g_tUart0.usRxCount = 0;						/* 接收到的新数据个数 */
	g_tUart0.usTxCount = 0;						/* 待发送的数据个数 */
#if defined (MODBUS_RTU_SLAVE)
#if !defined (__RS485_H__)
#error "No rs485 heir in design"
#endif
	g_tUart0.SendBefor = RS485_SendBefor;		/* 发送数据前的回调函数 */
	g_tUart0.SendOver = RS485_SendOver;			/* 发送完毕后的回调函数 */
	g_tUart0.ReciveNew = RS485_ReciveNew;		/* 接收到新数据后的回调函数 */
#else
	g_tUart0.SendBefor = 0;		/* 发送数据前的回调函数 */
	g_tUart0.SendOver = 0;			/* 发送完毕后的回调函数 */
	g_tUart0.ReciveNew = 0;		/* 接收到新数据后的回调函数 */
#endif
	g_tUart0.Sending = 0;						/* 正在发送中标志 */

	// g_tUart1....
}

int Uart0_Init(void)
{
	int Status;
	u32 StatusRegister;
	/*
	 * Initialize the UartLite driver so that it's ready to use.
	 */
#if defined (MODBUS_RTU_SLAVE)
	Status = rs485_heir_xuart_setup();
#else
	Status = XUartLite_Initialize(UartPtr, DeviceId);
#endif //#if defined (MODBUS_RTU_SLAVE)
	if (Status != XST_SUCCESS) {
		bsp_printf(TXT_RED "XUartLite_Initialize failed...\r\n" TXT_RST);
		return XST_FAILURE;
	}

#if defined (XPAR_XSCUGIC_NUM_INSTANCES)
	XScuGic_SetPriorityTriggerType(&InterruptController, XPAR_FABRIC_UARTLITE_0_VEC_ID, 0xA0, 0x3);
#endif // #if defined (XPAR_XSCUGIC_NUM_INSTANCES)

	/*
	 * Connect a device driver handler that will be called when an interrupt
	 * for the device occurs, the device driver handler performs the
	 * specific interrupt processing for the device.
	 */
#if defined (XPAR_XSCUGIC_NUM_INSTANCES)
	XUartLite_SetRecvHandler(&UartLiteRs485, Uart0RxCpltIRQ, &g_tUart0);
	XUartLite_SetSendHandler(&UartLiteRs485, Uart0TxCpltIRQ, &g_tUart0);
	Status = INTC_CONNECT(&InterruptController, XPAR_FABRIC_UARTLITE_0_VEC_ID,
			   (XInterruptHandler)XUartLite_InterruptHandler,
//			   (XInterruptHandler)Uart0IntrHandler,
			   (void *)&UartLiteRs485);
#else
	XUartLite_SetRecvHandler(&UartLiteRs485, Uart0RxCpltIRQ, &g_tUart0);
	XUartLite_SetSendHandler(&UartLiteRs485, Uart0TxCpltIRQ, &g_tUart0);
	Status = INTC_CONNECT(&InterruptController, XPAR_INTC_0_UARTLITE_0_VEC_ID,
			   (XInterruptHandler)XUartLite_InterruptHandler,
//			   (XInterruptHandler)Uart0IntrHandler,
			   (void *)&UartLiteRs485);
#endif
	if (Status != XST_SUCCESS) {
		bsp_printf(TXT_RED "UartLite Interrupt setup failed...\r\n" TXT_RST);
		return XST_FAILURE;
	}
	XUartLite_EnableInterrupt(&UartLiteRs485);

	/*
	 * Enter a critical region by disabling all the UART interrupts to allow
	 * this call to stop a previous operation that may be interrupt driven
	 */
	StatusRegister = XUartLite_GetStatusReg(UartLiteRs485.RegBaseAddress);
	XUartLite_WriteReg(UartLiteRs485.RegBaseAddress, XUL_CONTROL_REG_OFFSET, 0);
	/*
	 * Restore the interrupt enable register to it's previous value such
	 * that the critical region is exited
	 */
	StatusRegister &= XUL_CR_ENABLE_INTR;
	XUartLite_WriteReg(UartLiteRs485.RegBaseAddress,
				XUL_CONTROL_REG_OFFSET, StatusRegister);
#if defined (XPAR_XSCUGIC_NUM_INSTANCES)
	XScuGic_Enable(&InterruptController,XPAR_FABRIC_UARTLITE_0_VEC_ID);
#else
	XIntc_Enable(&InterruptController,XPAR_INTC_0_UARTLITE_0_VEC_ID);
#endif


#if defined (MODBUS_RTU_SLAVE)
#if !defined (__RS485_H__)
#error "No rs485 heir in design"
#endif
    Status = rs485_heir_xgpio_setup();
    if (Status != XST_SUCCESS)
	{
    	bsp_printf(TXT_RED "rs485_heir_xgpio_setup failed...\r\n" TXT_RST);
		//Xil_Assert(__FILE__, __LINE__);
		return XST_FAILURE ;
	}
    UartLiteRs485_Input();
#endif // MODBUS_RTU_SLAVE

	return XST_SUCCESS;
}

uint8_t UartGetChar(UART_T *_pUart, uint8_t *_pByte)
{
	uint16_t usCount;

	/* usRxWrite 变量在中断函数中被改写，主程序读取该变量时，必须进行临界区保护 */
	DISABLE_INT();
	usCount = _pUart->usRxCount;
	ENABLE_INT();

	/* 如果读和写索引相同，则返回0 */
	//if (_pUart->usRxRead == usRxWrite)
	if (usCount == 0)	/* 已经没有数据 */
	{
		return 1;
	}
	else
	{
		*_pByte = _pUart->pRxBuf[_pUart->usRxRead];		/* 从串口接收FIFO取1个数据 */

		/* 改写FIFO读索引 */
		DISABLE_INT();
		if (++_pUart->usRxRead >= _pUart->usRxBufSize)
		{
			_pUart->usRxRead = 0;
		}
		_pUart->usRxCount--;
		ENABLE_INT();
		return 0;
	}
}


/*
*********************************************************************************************************
*	函 数 名: UartSend
*	功能说明: 填写数据到UART发送缓冲区,并启动发送中断。中断处理函数发送完毕后，自动关闭发送中断
*	形    参:  无
*	返 回 值: 无
*********************************************************************************************************
*/
static void UartSend(UART_T *_pUart, uint8_t *_ucaBuf, uint16_t _usLen)
{
	uint16_t i;
//	u32 StatusRegister;

	for (i = 0; i < _usLen; i++)
	{
		/* 如果发送缓冲区已经满了，则等待缓冲区空 */
		while (1)
		{
			volatile uint16_t usCount;

			DISABLE_INT();
			usCount = _pUart->usTxCount;
			ENABLE_INT();
//			XUartLite_Send(_pUart->uart, &_pUart->pTxBuf[_pUart->usTxRead], 1);
			if (usCount < _pUart->usTxBufSize)
			{
				break;
			}
		}

		/* 将新数据填入发送缓冲区 */
		_pUart->pTxBuf[_pUart->usTxWrite] = _ucaBuf[i];

		DISABLE_INT();
		if (++_pUart->usTxWrite >= _pUart->usTxBufSize)
		{
			_pUart->usTxWrite = 0;
		}
		_pUart->usTxCount++;
		ENABLE_INT();
	}

	// 发起一次发送，中断里会继续发送剩下的内容
	XUartLite_Send(_pUart->uart, &_pUart->pTxBuf[_pUart->usTxRead], 1);
	if (++_pUart->usTxRead >= _pUart->usTxBufSize)
	{
		_pUart->usTxRead = 0;
	}
	//_pUart->usTxCount--; // 在中断里自动减
}

/*
*********************************************************************************************************
*	函 数 名: comSendBuf
*	功能说明: 向串口发送一组数据。数据放到发送缓冲区后立即返回，由中断服务程序在后台完成发送
*	形    参: _ucPort: 端口号(COM1 - COM6)
*			  _ucaBuf: 待发送的数据缓冲区
*			  _usLen : 数据长度
*	返 回 值: 无
*********************************************************************************************************
*/
void comSendBuf(uint8_t *_ucaBuf, uint16_t _usLen)
{
	UART_T *pUart;

	//pUart = ComToUart(_ucPort);
	pUart = &g_tUart0;
	if (pUart == 0)
	{
		return;
	}

	if (pUart->SendBefor != 0)
	{
		pUart->SendBefor();		/* 如果是RS485通信，可以在这个函数中将RS485设置为发送模式 */
	}

	UartSend(pUart, _ucaBuf, _usLen);
	//XUartLite_Send(pUart->uart, _ucaBuf, _usLen);
}


#if defined (MODBUS_RTU_SLAVE)
#if !defined (__RS485_H__)
#error "No rs485 heir in design"
#endif
/*
*********************************************************************************************************
*	函 数 名: RS485_ReciveNew
*	功能说明: 接收到新的数据
*	形    参: _byte 接收到的新数据
*	返 回 值: 无
*********************************************************************************************************
*/
extern void MODS_ReciveNew(uint8_t _byte);
void RS485_ReciveNew(uint8_t _byte)
{
	MODS_ReciveNew(_byte);
}

/*
*********************************************************************************************************
*	函 数 名: RS485_SendBefor
*	功能说明: 发送数据前的准备工作。对于RS485通信，请设置RS485芯片为发送状态，
*			  并修改 UartVarInit()中的函数指针等于本函数名，比如 g_tUart2.SendBefor = RS485_SendBefor
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************
*/
void RS485_SendBefor(void)
{
//	u32 ret;
//	RS485_TX_EN();	/* 切换RS485收发芯片为发送模式 */
	UartLiteRs485_Output();
}

/*
*********************************************************************************************************
*	函 数 名: RS485_SendOver
*	功能说明: 发送一串数据结束后的善后处理。对于RS485通信，请设置RS485芯片为接收状态，
*			  并修改 UartVarInit()中的函数指针等于本函数名，比如 g_tUart2.SendOver = RS485_SendOver
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************
*/
void RS485_SendOver(void)
{
//	u32 ret;
//	RS485_RX_EN();	/* 切换RS485收发芯片为接收模式 */
	UartLiteRs485_Input();
}

/*
*********************************************************************************************************
*	函 数 名: RS485_SendBuf
*	功能说明: 通过RS485芯片发送一串数据。注意，本函数不等待发送完毕。
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************
*/
void RS485_SendBuf(uint8_t *_ucaBuf, uint16_t _usLen)
{
	comSendBuf(_ucaBuf, _usLen);
}
#endif // #if defined (MODBUS_RTU_SLAVE)

#endif // #if defined (XPAR_XUARTLITE_NUM_INSTANCES) && defined (INTC_DEVICE_ID) && defined (INTC)


/*
usage:
assume you have one or more axi_uartlite in design, call comSendBuf() to send, call UartGetChar() to get a char from fifo
call follows before main_loop, if you're using rs232
```
#if defined (__UARTLITE_FIFO_H__)
    Uart0VarInit();
    Uart0_Init();
#endif // __UARTLITE_FIFO_H__
```
if you are using rs485 but not modbus, follow callbacks in UartXVarInit() must be set
```
	g_tUart0.SendBefor = RS485_SendBefor;
	g_tUart0.SendOver = RS485_SendOver;
```




call follows before main_loop, if you are using modbus
```
#if defined (__UARTLITE_FIFO_H__)
    Uart0VarInit();
    Uart0_Init();
#if defined (MODBUS_RTU_SLAVE)
    MODS_VarInit();
#endif // MODBUS_RTU_SLAVE
#endif // __UARTLITE_FIFO_H__
```
follow callbacks in UartXVarInit() must be set
```
	g_tUart0.SendBefor = RS485_SendBefor;
	g_tUart0.SendOver = RS485_SendOver;
	g_tUart0.ReciveNew = RS485_ReciveNew;
```


*/
