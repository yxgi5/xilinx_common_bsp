/**
 *******************************************************************************
 *
 * Copyright (C) 2022 - 2027  XXX Company. All rights reserved.
 *
 * @file xgpio_i2c.h
 * @brief
 *
 * \n
 * @details
 *
 * \n
 * @version 
 * @author Andreas Zhang
 * @date 2022-01-19
 *
 * @verbatim
 * \n
 * @history
 * Ver   Who Date     Changes
 * ----- --- -------- ---------------------------------------------
 * 1.3.0 who 02/07/11 First release
 * 
 *
 * @endverbatim
 *******************************************************************************
 */
#ifndef __XGPIO_I2C_H__
#define __XGPIO_I2C_H__

#if defined(XPAR_XGPIO_I2C_0_AXI_GPIO_0_DEVICE_ID)

#include "xgpio.h"
#include "sleep.h"

#define INPUT 1
#define OUTPUT 0

typedef enum xgpio_i2c_no
{
    I2C_NO_0 = 0,
	I2C_NO_1,
	I2C_NO_2,
	I2C_NO_3,
//	I2C_NO_4,
//	I2C_NO_5,
//	I2C_NO_6,
//	I2C_NO_7,

	I2C_NO_BUTT,
} i2c_no;

typedef enum
{
    STRETCH_OFF = 0,
	STRETCH_ON,
} stretch_mode;

typedef enum xgpio_i2c_bank_no
{
    I2C_BANK_1 = 1,
	I2C_BANK_2,

	I2C_BANK_BUTT,
} i2c_bank_no;

typedef struct {
	i2c_no I2C_ID;
	u16 DeviceId;
	i2c_bank_no I2C_BANK;
	u32 I2C_SCL;
	u32 I2C_SDA;
} XGpio_I2C_Cfg;

//#define XGPIO_SCL7_NUM 15
//#define XGPIO_SDA7_NUM 14
//#define XGPIO_SCL6_NUM 13
//#define XGPIO_SDA6_NUM 12
//#define XGPIO_SCL5_NUM 11
//#define XGPIO_SDA5_NUM 10
//#define XGPIO_SCL4_NUM 9
//#define XGPIO_SDA4_NUM 8
#define XGPIO_SCL3_NUM 7
#define XGPIO_SDA3_NUM 6
#define XGPIO_SCL2_NUM 5
#define XGPIO_SDA2_NUM 4
#define XGPIO_SCL1_NUM 3
#define XGPIO_SDA1_NUM 2
#define XGPIO_SCL0_NUM 1
#define XGPIO_SDA0_NUM 0

int xgpio_init(void);
//void i2c_scl_pos(i2c_no i2c);
void i2c_start(i2c_no i2c);
void i2c_stop(i2c_no i2c);
void i2c_ack(i2c_no i2c);
void i2c_nack(i2c_no i2c);
void i2c_send_byte(i2c_no i2c, u8 txd);
u8  i2c_recv_byte(i2c_no i2c);
u8  i2c_recv_ack(i2c_no i2c, stretch_mode st_mode);
int xgpio_i2c_reg8_write(i2c_no i2c, char IIC_ADDR, char Addr, char Data, stretch_mode st_mode);
int xgpio_i2c_reg8_read(i2c_no i2c, char IIC_ADDR, char Addr, u8 * ret, stretch_mode st_mode);
int xgpio_i2c_reg16_write(i2c_no i2c, char IIC_ADDR, unsigned short Addr, char Data, stretch_mode st_mode);
int xgpio_i2c_reg16_read(i2c_no i2c, char IIC_ADDR, unsigned short Addr, u8 * ret, stretch_mode st_mode);

//int xgpio_i2c_32b32_write(i2c_no i2c, char IIC_ADDR, unsigned int Addr, unsigned int Data, stretch_mode st_mode);
//int xgpio_i2c_32b32_read(i2c_no i2c, char IIC_ADDR, unsigned int Addr, unsigned int * ret, stretch_mode st_mode);


#endif // XPAR_XGPIO_I2C_0_AXI_GPIO_0_DEVICE_ID

#endif // __XGPIO_I2C_H__
