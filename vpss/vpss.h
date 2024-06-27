#ifndef __VPSS_H__

//#include "xparameters.h"
#if defined (XPAR_XVPROCSS_NUM_INSTANCES)
#define __VPSS_H__
#include "xvprocss.h"
#include "xvidc.h"

//#define USR_FRAME_BUF_BASEADDR     	(DDR_BASEADDR + (0x20000000))

extern XVprocSs VprocInst0;

int vpss_config(void);

#endif // XPAR_XVPROCSS_NUM_INSTANCES

#endif // __VPSS_H__

