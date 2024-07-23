#ifndef __X_I2C_H__

#if defined (XPAR_XIIC_NUM_INSTANCES)
#define __X_I2C_H__
//#include "xil_types.h"
#include "xiic.h"

int xi2c_reg8_write(char IIC_ADDR, char Addr, char Data);
char xi2c_reg8_read(char IIC_ADDR, char Addr);
int xi2c_reg16_write(char IIC_ADDR, unsigned short Addr, char Data);
char xi2c_reg16_read(char IIC_ADDR, unsigned short Addr);

#endif // XPAR_XIIC_NUM_INSTANCES

#endif // __X_I2C_H__
