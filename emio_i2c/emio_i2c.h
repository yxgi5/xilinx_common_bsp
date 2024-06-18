#ifndef __EMIO_I2C_H__

#if defined (XPAR_XGPIOPS_NUM_INSTANCES)
#define __EMIO_I2C_H__
#include "xgpiops.h"
#include "sleep.h"

#if defined(XPAR_XGPIOPS_0_DEVICE_ID)

#define EMIO_I2C_NUM 	(4U)	// 1~48(emio 78~173 for zynqmp), 1~32(emio 54~117 for zynq)

#define EMIO_INPUT  0
#define EMIO_OUTPUT 1

typedef enum emio_i2c_no
{
#if (EMIO_I2C_NUM >= 1U)
    I2C_NO_0 = 0,
#endif
#if (EMIO_I2C_NUM >= 2U)
	I2C_NO_1,
#endif
#if (EMIO_I2C_NUM >= 3U)
	I2C_NO_2,
#endif
#if (EMIO_I2C_NUM >= 4U)
	I2C_NO_3,
#endif
#if (EMIO_I2C_NUM >= 5U)
	I2C_NO_4,
#endif
#if (EMIO_I2C_NUM >= 6U)
	I2C_NO_5,
#endif
#if (EMIO_I2C_NUM >= 7U)
	I2C_NO_6,
#endif
#if (EMIO_I2C_NUM >= 8U)
	I2C_NO_7,
#endif

	I2C_NO_BUTT,
} i2c_no;

typedef enum
{
    STRETCH_OFF = 0,
	STRETCH_ON,
} stretch_mode;

typedef struct {
	i2c_no I2C_ID;
	u32 I2C_SCL;
	u32 I2C_SDA;
} XGpioPs_I2C_Cfg;

#if defined (PLATFORM_ZYNQMP)
#if (EMIO_I2C_NUM >= 1U)
#define EMIO_SDA0_NUM 78
#define EMIO_SCL0_NUM 79
#endif
#if (EMIO_I2C_NUM >= 2U)
#define EMIO_SDA1_NUM 80
#define EMIO_SCL1_NUM 81
#endif
#if (EMIO_I2C_NUM >= 3U)
#define EMIO_SDA2_NUM 82
#define EMIO_SCL2_NUM 83
#endif
#if (EMIO_I2C_NUM >= 4U)
#define EMIO_SDA3_NUM 84
#define EMIO_SCL3_NUM 85
#endif
#if (EMIO_I2C_NUM >= 5U)
#define EMIO_SDA4_NUM 86
#define EMIO_SCL4_NUM 87
#endif
#if (EMIO_I2C_NUM >= 6U)
#define EMIO_SDA5_NUM 88
#define EMIO_SCL5_NUM 89
#endif
#if (EMIO_I2C_NUM >= 7U)
#define EMIO_SDA6_NUM 90
#define EMIO_SCL6_NUM 91
#endif
#if (EMIO_I2C_NUM >= 8U)
#define EMIO_SDA7_NUM 92
#define EMIO_SCL7_NUM 93
#endif
#endif // PLATFORM_ZYNQMP

#if defined (PLATFORM_ZYNQ)
#if (EMIO_I2C_NUM >= 1U)
#define EMIO_SDA0_NUM 54
#define EMIO_SCL0_NUM 55
#endif
#if (EMIO_I2C_NUM >= 2U)
#define EMIO_SDA1_NUM 56
#define EMIO_SCL1_NUM 57
#endif
#if (EMIO_I2C_NUM >= 3U)
#define EMIO_SDA2_NUM 58
#define EMIO_SCL2_NUM 59
#endif
#if (EMIO_I2C_NUM >= 4U)
#define EMIO_SDA3_NUM 60
#define EMIO_SCL3_NUM 61
#endif
#if (EMIO_I2C_NUM >= 5U)
#define EMIO_SDA4_NUM 62
#define EMIO_SCL4_NUM 63
#endif
#if (EMIO_I2C_NUM >= 6U)
#define EMIO_SDA5_NUM 64
#define EMIO_SCL5_NUM 65
#endif
#if (EMIO_I2C_NUM >= 7U)
#define EMIO_SDA6_NUM 66
#define EMIO_SCL6_NUM 67
#endif
#if (EMIO_I2C_NUM >= 8U)
#define EMIO_SDA7_NUM 68
#define EMIO_SCL7_NUM 69
#endif
#endif // PLATFORM_ZYNQ

int emio_init(void);
void i2c_start(i2c_no i2c);
void i2c_stop(i2c_no i2c);
void i2c_ack(i2c_no i2c);
void i2c_nack(i2c_no i2c);
void i2c_send_byte(i2c_no i2c, u8 txd);
u8  i2c_recv_byte(i2c_no i2c);
u8  i2c_recv_ack(i2c_no i2c, stretch_mode st_mode);
int emio_i2c_reg8_write(i2c_no i2c, char IIC_ADDR, char Addr, char Data, stretch_mode st_mode);
int emio_i2c_reg8_read(i2c_no i2c, char IIC_ADDR, char Addr, u8 * ret, stretch_mode st_mode);
int emio_i2c_reg16_write(i2c_no i2c, char IIC_ADDR, unsigned short Addr, char Data, stretch_mode st_mode);
int emio_i2c_reg16_read(i2c_no i2c, char IIC_ADDR, unsigned short Addr, u8 * ret, stretch_mode st_mode);

//int emio_i2c_32b32_write(i2c_no i2c, char IIC_ADDR, unsigned int Addr, unsigned int Data, stretch_mode st_mode);
//int emio_i2c_32b32_read(i2c_no i2c, char IIC_ADDR, unsigned int Addr, unsigned int * ret, stretch_mode st_mode);

#endif // XPAR_XGPIOPS_0_DEVICE_ID

#endif // __EMIO_I2C_H__

#endif // XPAR_XGPIOPS_NUM_INSTANCES
