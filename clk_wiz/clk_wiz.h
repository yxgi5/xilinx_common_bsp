#ifndef __CLK_WIZ_H__

//#include "xparameters.h"
#if defined (XPAR_XCLK_WIZ_NUM_INSTANCES)
#define __CLK_WIZ_H__
#include "xil_types.h"
#include "xclk_wiz.h"

#define CLK_LOCK 	(1)

#if (XPAR_XCLK_WIZ_NUM_INSTANCES == 1U)
extern XClk_Wiz ClkWiz_Dynamic0;
#elif (XPAR_XCLK_WIZ_NUM_INSTANCES == 2U)
extern XClk_Wiz ClkWiz_Dynamic0;
extern XClk_Wiz ClkWiz_Dynamic1;
#elif (XPAR_XCLK_WIZ_NUM_INSTANCES == 3U)
extern XClk_Wiz ClkWiz_Dynamic0;
extern XClk_Wiz ClkWiz_Dynamic1;
extern XClk_Wiz ClkWiz_Dynamic3;
#endif

int Wait_For_Lock(XClk_Wiz_Config *CfgPtr_Dynamic);
int XClk_Wiz_dynamic_reconfig(XClk_Wiz * ClkWizInstance, u32 DeviceId);
int clkwiz_config(void);

#endif // XPAR_XCLK_WIZ_NUM_INSTANCES

#endif // __CLK_WIZ_H__

