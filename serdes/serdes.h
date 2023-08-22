#ifndef __SERDES_H__
#define __SERDES_H__

//#include "xil_types.h"

struct reginfo
{
	u8	addr;
    u16 reg;
    u8 	val;
};

#define SEQUENCE_PROPERTY    0xFFFD
#define SEQUENCE_WAIT_MS     0xFFFE
#define SEQUENCE_END	     0xFFFF

extern struct reginfo max9296_rgb888_gmsl2[];
extern struct reginfo max96717_rgb888_gmsl2[];

int max929x_write(i2c_no i2c, u8 addr, u16 reg, u8 data);
void max929x_write_array(i2c_no i2c, struct reginfo *regarray);

#endif // __SERDES_H__
