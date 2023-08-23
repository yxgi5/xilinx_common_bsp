#ifndef __XSWITCH_H__
#define __XSWITCH_H__

#if defined (XPAR_XAXIS_SWITCH_NUM_INSTANCES)

#include "xaxis_switch.h"

int AxisSwitch(u16 DeviceId, XAxis_Switch * pAxisSwitch, u8 SiIndex, u8 MiIndex);
int axis_switch_cfg(void);

#endif // XPAR_XAXIS_SWITCH_NUM_INSTANCES

#endif // __XSWITCH_H__
