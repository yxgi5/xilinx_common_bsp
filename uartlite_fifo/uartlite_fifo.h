#ifndef __UARTLITE_FIFO_H__

#if defined (XPAR_XUARTLITE_NUM_INSTANCES) && defined (INTC_DEVICE_ID) && defined (INTC)
#define __UARTLITE_FIFO_H__

#include "xuartlite.h"

#define	UART0_FIFO_EN	1
#define	UART1_FIFO_EN	0
#define	UART2_FIFO_EN	0
#define	UART3_FIFO_EN	0
#define	UART4_FIFO_EN	0
#define	UART5_FIFO_EN	0
#define	UART6_FIFO_EN	0
#define	UART7_FIFO_EN	0


//#define UART_DEVICE_ID      XPAR_UARTLITE_1_DEVICE_ID
//#define INTC_DEVICE_ID      XPAR_INTC_0_DEVICE_ID
//#define UART_IRPT_INTR      XPAR_INTC_0_UARTLITE_1_VEC_ID   // 中断ID

#if (UART0_FIFO_EN == 1) && defined (XPAR_UARTLITE_0_DEVICE_ID)
#define UART0_BAUD			XPAR_UARTLITE_0_BAUDRATE
#define UART0_TX_BUF_SIZE	128
#define UART0_RX_BUF_SIZE	128
#endif // #if (UART0_FIFO_EN == 1)
#if (UART1_FIFO_EN == 1) && defined (XPAR_UARTLITE_1_DEVICE_ID)
#define UART1_BAUD			XPAR_UARTLITE_1_BAUDRATE
#define UART1_TX_BUF_SIZE	128
#define UART1_RX_BUF_SIZE	128
#endif // #if (UART1_FIFO_EN == 1)
#if (UART2_FIFO_EN == 1) && defined (XPAR_UARTLITE_2_DEVICE_ID)
#define UART2_BAUD			XPAR_UARTLITE_2_BAUDRATE
#define UART2_TX_BUF_SIZE	128
#define UART2_RX_BUF_SIZE	128
#endif // #if (UART2_FIFO_EN == 1)
#if (UART3_FIFO_EN == 1) && defined (XPAR_UARTLITE_3_DEVICE_ID)
#define UART3_BAUD			XPAR_UARTLITE_3_BAUDRATE
#define UART3_TX_BUF_SIZE	128
#define UART3_RX_BUF_SIZE	128
#endif // #if (UART3_FIFO_EN == 1)
#if (UART4_FIFO_EN == 1) && defined (XPAR_UARTLITE_4_DEVICE_ID)
#define UART4_BAUD			XPAR_UARTLITE_4_BAUDRATE
#define UART4_TX_BUF_SIZE	128
#define UART4_RX_BUF_SIZE	128
#endif // #if (UART4_FIFO_EN == 1)
#if (UART5_FIFO_EN == 1) && defined (XPAR_UARTLITE_5_DEVICE_ID)
#define UART5_BAUD			XPAR_UARTLITE_5_BAUDRATE
#define UART5_TX_BUF_SIZE	128
#define UART5_RX_BUF_SIZE	128
#endif // #if (UART5_FIFO_EN == 1)
#if (UART6_FIFO_EN == 1) && defined (XPAR_UARTLITE_6_DEVICE_ID)
#define UART6_BAUD			XPAR_UARTLITE_6_BAUDRATE
#define UART6_TX_BUF_SIZE	128
#define UART6_RX_BUF_SIZE	128
#endif // #if (UART6_FIFO_EN == 1)
#if (UART7_FIFO_EN == 1) && defined (XPAR_UARTLITE_7_DEVICE_ID)
#define UART7_BAUD			XPAR_UARTLITE_7_BAUDRATE
#define UART7_TX_BUF_SIZE	128
#define UART7_RX_BUF_SIZE	128
#endif // #if (UART7_FIFO_EN == 1)

/* 开关全局中断的宏 */
//#define ENABLE_INT()	asm(" msr  DAIFCLR, #2")	/* 使能全局中断 */
//#define DISABLE_INT()	asm(" msr  DAIFSET, #2")	/* 禁止全局中断 */
//#define ENABLE_INT() microblaze_enable_interrupts()
//#define DISABLE_INT() microblaze_disable_interrupts()
#define ENABLE_INT()   Xil_ExceptionEnable()
#define DISABLE_INT()  Xil_ExceptionDisable()

typedef struct
{
	INTC *Intc;
	XUartLite *uart;				/* 串口设备指针 */
	uint8_t *pTxBuf;			/* 发送缓冲区 */
	uint8_t *pRxBuf;			/* 接收缓冲区 */
	uint16_t usTxBufSize;		/* 发送缓冲区大小 */
	uint16_t usRxBufSize;		/* 接收缓冲区大小 */
	volatile uint16_t usTxWrite;	/* 发送缓冲区写指针 */
	volatile uint16_t usTxRead;		/* 发送缓冲区读指针 */
	volatile uint16_t usTxCount;	/* 等待发送的数据个数 */

	volatile uint16_t usRxWrite;	/* 接收缓冲区写指针 */
	volatile uint16_t usRxRead;		/* 接收缓冲区读指针 */
	volatile uint16_t usRxCount;	/* 还未读取的新数据个数 */

	void (*SendBefor)(void); 	/* 开始发送之前的回调函数指针（主要用于RS485切换到发送模式） */
	void (*SendOver)(void); 	/* 发送完毕的回调函数指针（主要用于RS485将发送模式切换为接收模式） */
	void (*ReciveNew)(uint8_t _byte);	/* 串口收到数据的回调函数指针 */
	uint8_t Sending;			/* 正在发送中 */
}UART_T;

uint8_t UartGetChar(UART_T *_pUart, uint8_t *_pByte);
int Uart0_Init(void);
//void SetupInterruptSystem(XIntc *GicInstancePtr,
//		XUartLite *UartInstancePtr, u16 UartIntrId);
void Uart0VarInit(void);
void RS485_SendBuf(uint8_t *_ucaBuf, uint16_t _usLen);
void RS485_SendBefor(void);
void RS485_SendOver(void);
void RS485_ReciveNew(uint8_t _byte);

#endif //#if defined (XPAR_XUARTLITE_NUM_INSTANCES) && defined (INTC_DEVICE_ID) && defined (INTC)

#endif //#ifndef __UARTLITE_FIFO_H__
