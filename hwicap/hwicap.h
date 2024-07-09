#ifndef __HWICAP_H__

#if defined (XPAR_XHWICAP_NUM_INSTANCES)
#define __HWICAP_H__

#include <xhwicap.h>

extern XHwIcap HwIcap0;

int hwicap_init(void);
u32 hwicap_GetConfigReg(XHwIcap *InstancePtr, u32 ConfigReg, u32 *RegData);

// low_level ops dose not need call hwcap_init()
int WATCHDOG_TIMER_CFG(u32 timer_value);
int ISSUE_IPROG(u32 iprog_address);
int READ_IDCODE(u32 *IdCode);
int READ_BOOTSTS(u32 *StsCode);
int isFallbacked(void);
int READ_WDT(u32 *timer_value);

#endif // #if defined (XPAR_XHWICAP_NUM_INSTANCES)

#endif // #ifndef __HWICAP_H__

