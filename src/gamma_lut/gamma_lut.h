#ifndef __GAMMA_LUT_H__

//#include "xparameters.h"
#if defined (XPAR_XV_GAMMA_LUT_NUM_INSTANCES)
#define __GAMMA_LUT_H__
//#include "xil_types.h"
#include "xv_gamma_lut.h"

//int gamma_lut_init(u16 deviceid,u32 hsize,u32 vsize);
int gamma_lut_init(u16 deviceid,u32 hsize,u32 vsize,u8 format);

#endif // XPAR_XV_GAMMA_LUT_NUM_INSTANCES

#endif // __GAMMA_LUT_H__
