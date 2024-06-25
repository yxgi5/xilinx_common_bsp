/*
 * serdes.c
 *
 *  Created on: 2023年4月26日
 *      Author: fengke
 */
#include "../bsp.h"

#if defined (SER_CFG) || defined (DES_CFG)

#if defined (DES_CFG)
struct reginfo max9296_rgb888_gmsl2[] =
{
	{0x90, 0x0313, 0x00},// CSI output disabled
//	{0x90, 0x0001, 0x12},//11=Remote control channel disabled\3Gbs,default 02=Remote control channel Enabled\6Gbs,
//	{0x90, 0x0010, 0x21},// Link A is selected, reset link
//	{0x90, 0x0010, 0xA1},// Link A is selected, reset link
	{0x90, SEQUENCE_WAIT_MS, 0x80},//delay for a while

#if 1
//	{0x90, 0x0001, 0x01},// 3Gbps mode
//	{0x90, 0x0010, 0x21},// Link A is selected, reset link
//	{0x90, SEQUENCE_WAIT_MS, 0x80},//delay for a while

	{ 0x90, 0x0002, 0x13 }, // use pipeline X
	{ 0x90, 0x0050, 0x02 }, // pipeline X stream ID
	{ 0x90, 0x0051, 0x00 }, // pipeline Y stream ID
	{ 0x90, 0x0052, 0x01 }, // pipeline Z stream ID
	{ 0x90, 0x0053, 0x03 }, // pipeline U stream ID

	{0x90, 0x0330, 0x04},
	{0x90, 0x0333, 0x4e},
	{0x90, 0x0334, 0xe4},
	{0x90, 0x040a, 0x00},
	{0x90, 0x044a, 0xd0},
	{0x90, 0x048a, 0xd0},
	{0x90, 0x04ca, 0x00},
//	{0x90, 0x031d, 0x2a},
//	{0x90, 0x0320, 0x2a},
//	{0x90, 0x0323, 0x2a},
//	{0x90, 0x0326, 0x2a},
//	{0x90, 0x031d, 0x25},
//	{0x90, 0x0320, 0x25},
//	{0x90, 0x0323, 0x25},
//	{0x90, 0x0326, 0x25},
	{0x90, 0x031d, 0x0a},
	{0x90, 0x0320, 0x0a},
	{0x90, 0x0323, 0x0a},
	{0x90, 0x0326, 0x0a},

	{0x90, 0x040b, 0x07},
	{0x90, 0x040c, 0x00},
	{0x90, 0x042d, 0x15},
	{0x90, 0x040d, 0x24},
	{0x90, 0x040e, 0x24},
	{0x90, 0x040f, 0x00},
	{0x90, 0x0410, 0x00},
	{0x90, 0x0411, 0x01},
	{0x90, 0x0412, 0x01},

//	{0x90, 0x044b, 0x07},
//	{0x90, 0x044c, 0x00},
//	{0x90, 0x046d, 0x15},
//	{0x90, 0x044d, 0x24},
//	{0x90, 0x044e, 0x24},
//	{0x90, 0x044f, 0x00},
//	{0x90, 0x0490, 0x00},
//	{0x90, 0x0451, 0x01},
//	{0x90, 0x0452, 0x01},
//
//	{0x90, 0x048b, 0x07},
//	{0x90, 0x048c, 0x00},
//	{0x90, 0x04ad, 0x15},
//	{0x90, 0x048d, 0x24},
//	{0x90, 0x048e, 0x24},
//	{0x90, 0x048f, 0x00},
//	{0x90, 0x0490, 0x00},
//	{0x90, 0x0491, 0x01},
//	{0x90, 0x0492, 0x01},
//
//	{0x90, 0x04cb, 0x07},
//	{0x90, 0x04cc, 0x00},
//	{0x90, 0x04ed, 0x15},
//	{0x90, 0x04cd, 0x24},
//	{0x90, 0x04ce, 0x24},
//	{0x90, 0x04cf, 0x00},
//	{0x90, 0x04D0, 0x00},
//	{0x90, 0x04D1, 0x01},
//	{0x90, 0x04D2, 0x01},

	{0x90, 0x0332, 0xF0},
	{0x90, 0x0313, 0x02},
#endif

	{0x90, SEQUENCE_END, 0x00}
};
#endif // DES_CFG

#if defined (SER_CFG)
struct reginfo max96717_rgb888_gmsl2[] =
{
//	{0x80, 0x0001, 0x08},// default 0x08, 6Gbps mode
//	{0x80, 0x0001, 0x14},// 3Gbps mode
//	{0x80, 0x0010, 0x21},// reset link and registers

	{0x80, SEQUENCE_WAIT_MS, 0x80},
	{0x80, 0x0331, 0x30},//default 0x30, 4lane
//	{0x80, 0x0318, 0x5E},//mem_dt1_selz
	{0x80, 0x0318, 0x64},//mem_dt1_selz

	{0x80, SEQUENCE_END, 0x00}
};
#endif // SER_CFG

int max929x_write(i2c_no i2c, u8 addr, u16 reg, u8 data)
{
	int ret;
	ret = xgpio_i2c_reg16_write(i2c, addr>>1, reg, data, STRETCH_ON);
	return ret;
}

void max929x_write_array(i2c_no i2c, struct reginfo *regarray)
{
	int i = 0;

	while (regarray[i].reg != SEQUENCE_END)
	{
		if(regarray[i].reg == SEQUENCE_WAIT_MS)
		{
		      usleep((regarray[i].val)*1000);
		}
		else
		{
			max929x_write(i2c, regarray[i].addr, regarray[i].reg,regarray[i].val);
		}
		i++;
	}
}

#endif // SER_CFG || DES_CFG


/*
usage:
assume you have a xgpio_i2c heir, or you can modify to use xiic or emio_i2c

ref to follows
```
#if defined (SER_CFG) || defined (DES_CFG)
    // MAX9296 config
    u8 ret8=0;
#if defined (DES_CFG)
    Status = xgpio_i2c_reg16_read(I2C_NO_3, 0x90>>1, 0x0000, &ret8, STRETCH_ON);
    Status = xgpio_i2c_reg16_read(I2C_NO_3, 0x90>>1, 0x0001, &ret8, STRETCH_ON);
#if defined (SERDES_3G)
    Status = xgpio_i2c_reg16_write(I2C_NO_3, 0x90>>1, 0x0001, 0x01, STRETCH_ON); // 3Gbps
    Status = xgpio_i2c_reg16_write(I2C_NO_3, 0x90>>1, 0x0010, 0x21, STRETCH_ON); // reset link
#else
    Status = xgpio_i2c_reg16_write(I2C_NO_3, 0x90>>1, 0x0001, 0x02, STRETCH_ON); // 6Gbps
	Status = xgpio_i2c_reg16_write(I2C_NO_3, 0x90>>1, 0x0010, 0x21, STRETCH_ON); // reset link
#endif // SERDES_3G
    max929x_write_array(I2C_NO_3, max9296_rgb888_gmsl2);
#endif // DES_CFG
#if defined (SER_CFG)
    Status = xgpio_i2c_reg16_read(I2C_NO_3, 0x80>>1, 0x0000, &ret8, STRETCH_ON);
    Status = xgpio_i2c_reg16_read(I2C_NO_3, 0x80>>1, 0x0001, &ret8, STRETCH_ON);
#if defined (SERDES_3G)
    Status = xgpio_i2c_reg16_write(I2C_NO_3, 0x80>>1, 0x0001, 0x04, STRETCH_ON); // 3Gbps
    Status = xgpio_i2c_reg16_write(I2C_NO_3, 0x80>>1, 0x0010, 0x21, STRETCH_ON); // reset link
#else
    Status = xgpio_i2c_reg16_write(I2C_NO_3, 0x80>>1, 0x0001, 0x08, STRETCH_ON); // 6Gbps
    Status = xgpio_i2c_reg16_write(I2C_NO_3, 0x80>>1, 0x0010, 0x21, STRETCH_ON); // reset link
#endif // SERDES_3G
//    max929x_write_array(I2C_NO_3, max9295_rgb888_gmsl2);
    max929x_write_array(I2C_NO_3, max96717_rgb888_gmsl2);
#endif // SER_CFG
#if defined (DES_CFG)
    Status = xgpio_i2c_reg16_read(I2C_NO_3, 0x90>>1, 0x0000, &ret8, STRETCH_ON);
	Status = xgpio_i2c_reg16_read(I2C_NO_3, 0x90>>1, 0x0001, &ret8, STRETCH_ON);
#endif // DES_CFG
#if defined (SER_CFG)
    Status = xgpio_i2c_reg16_read(I2C_NO_3, 0x80>>1, 0x0000, &ret8, STRETCH_ON);
    Status = xgpio_i2c_reg16_read(I2C_NO_3, 0x80>>1, 0x0001, &ret8, STRETCH_ON);
#endif // SER_CFG
#endif // SER_CFG || DES_CFG
```
*/
