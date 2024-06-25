#ifndef __MODBUS_SLAVE_H_
#if defined (MODBUS_RTU_SLAVE)
#if !defined (__RS485_H__)
#error "No rs485 heir in design"
#endif

#define __MODBUS_SLAVE_H_
#include "util_lib.h"
#define SADDR485	1
#define SBAUD485	XPAR_UARTLITE_0_BAUDRATE

/* 01H 读强制单线圈 */
/* 05H 写强制单线圈 */
#define REG_D01		0x0101
#define REG_D02		0x0102
#define REG_D03		0x0103
#define REG_D04		0x0104
#define REG_DXX 	REG_D04

/* 02H 读取输入状态 */
#define REG_T01		0x0201
#define REG_T02		0x0202
#define REG_T03		0x0203
#define REG_TXX		REG_T03

/* 03H 读保持寄存器 */
/* 06H 写保持寄存器 */
/* 10H 写多个保存寄存器 */
//#define SLAVE_REG_P01		0x0301
//#define SLAVE_REG_P02		0x0302
#define SLAVE_REG_P00		0x0000	// 预留用于从机地址读写
#define SLAVE_REG_P01		0x0001	// 这里实现一个保持寄存器作为示例
//#define SLAVE_REG_P02		0x0002	// 自行扩展

/* 04H 读取输入寄存器(模拟信号) */
//#define REG_A01		0x0401
#define REG_A01		0x0001
#define REG_AXX		REG_A01


/* RTU 应答代码 */
#define RSP_OK				0		/* 成功 */
#define RSP_ERR_CMD			0x01	/* 不支持的功能码 */
#define RSP_ERR_REG_ADDR	0x02	/* 寄存器地址错误 */
#define RSP_ERR_VALUE		0x03	/* 数据值域错误 */
#define RSP_ERR_WRITE		0x04	/* 写入失败 */

#define S_RX_BUF_SIZE		128
#define S_TX_BUF_SIZE		128

typedef struct
{
	volatile uint8_t RxBuf[S_RX_BUF_SIZE];
	volatile uint8_t RxCount;
	volatile uint8_t RxStatus;
	volatile uint8_t RxNewFlag;

	volatile uint8_t RspCode;

	volatile uint8_t TxBuf[S_TX_BUF_SIZE];
	volatile uint8_t TxCount;
}MODS_T;

typedef struct
{
	/* 03H 06H 读写保持寄存器 */
	uint16_t P01;
	//uint16_t P02;

	/* 04H 读取模拟量寄存器 */
	uint16_t A01;

	/* 01H 05H 读写单个强制线圈 */
//	uint16_t D01;
//	uint16_t D02;
//	uint16_t D03;
//	uint16_t D04;

}VAR_T;

extern volatile uint8_t g_mods_timeout;
void MODS_Poll(void);
void MODS_VarInit(void);
extern MODS_T g_tModS;
extern VAR_T g_tVar;

#endif // MODBUS_RTU_SLAVE
#endif // __MODBUS_SLAVE_H_
