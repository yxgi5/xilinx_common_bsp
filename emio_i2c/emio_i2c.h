#ifndef _EMIO_I2C_

#if defined (XPAR_XGPIOPS_NUM_INSTANCES)
#define _EMIO_I2C_
#include "xgpiops.h"
#include "sleep.h"

typedef enum emio_i2c_no
{
    I2C_NO_0 = 0,
	I2C_NO_1,
	I2C_NO_2,
	I2C_NO_3,
	I2C_NO_4,
	I2C_NO_5,
	I2C_NO_6,
	I2C_NO_7,

	I2C_NO_BUTT,
} i2c_no;

typedef struct {
	i2c_no I2C_ID;
	u32 I2C_SCL;
	u32 I2C_SDA;
} XGpioPs_I2C_Cfg;

#define EMIO_SCL7_NUM 93
#define EMIO_SDA7_NUM 92
#define EMIO_SCL6_NUM 91
#define EMIO_SDA6_NUM 90
#define EMIO_SCL5_NUM 89
#define EMIO_SDA5_NUM 88
#define EMIO_SCL4_NUM 87
#define EMIO_SDA4_NUM 86

#define EMIO_SCL3_NUM 85
#define EMIO_SDA3_NUM 84
#define EMIO_SCL2_NUM 83
#define EMIO_SDA2_NUM 82
#define EMIO_SCL1_NUM 81
#define EMIO_SDA1_NUM 80
#define EMIO_SCL0_NUM 79
#define EMIO_SDA0_NUM 78

int emio_init(void);
void i2c_start(i2c_no i2c);
void i2c_stop(i2c_no i2c);
void i2c_ack(i2c_no i2c);
void i2c_nack(i2c_no i2c);
void i2c_send_byte(i2c_no i2c, u8 txd);
u8  i2c_recv_byte(i2c_no i2c);
u8  i2c_recv_ack(i2c_no i2c);
int emio_i2c_reg8_write(i2c_no i2c, char IIC_ADDR, char Addr, char Data);
int emio_i2c_reg8_read(i2c_no i2c, char IIC_ADDR, char Addr, u8 * ret);
int emio_i2c_reg16_write(i2c_no i2c, char IIC_ADDR, unsigned short Addr, char Data);
int emio_i2c_reg16_read(i2c_no i2c, char IIC_ADDR, unsigned short Addr, u8 * ret);

#endif /* _EMIO_I2C_ */

#endif // XPAR_XGPIOPS_NUM_INSTANCES
