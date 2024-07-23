#include "../bsp.h"

#if defined (XPAR_XAXIS_SWITCH_NUM_INSTANCES)

#if (XPAR_XAXIS_SWITCH_NUM_INSTANCES >= 1U)
XAxis_Switch AxisSwitch0;
#endif
#if (XPAR_XAXIS_SWITCH_NUM_INSTANCES >= 2U)
XAxis_Switch AxisSwitch1;
#endif
#if (XPAR_XAXIS_SWITCH_NUM_INSTANCES >= 3U)
XAxis_Switch AxisSwitch2;
#endif
#if (XPAR_XAXIS_SWITCH_NUM_INSTANCES >= 4U)
XAxis_Switch AxisSwitch3;
#endif
#if (XPAR_XAXIS_SWITCH_NUM_INSTANCES >= 5U)
XAxis_Switch AxisSwitch4;
#endif
#if (XPAR_XAXIS_SWITCH_NUM_INSTANCES >= 6U)
XAxis_Switch AxisSwitch5;
#endif
#if (XPAR_XAXIS_SWITCH_NUM_INSTANCES >= 7U)
XAxis_Switch AxisSwitch6;
#endif
#if (XPAR_XAXIS_SWITCH_NUM_INSTANCES >= 8U)
XAxis_Switch AxisSwitch7;
#endif

int AxisSwitch(u16 DeviceId, XAxis_Switch * pAxisSwitch, u8 SiIndex, u8 MiIndex)
{
    XAxis_Switch_Config *Config;
    int Status;

    u8 num = XPAR_AXIS_SWITCH_0_DEVICE_ID;

    /* Initialize the AXI4-Stream Switch driver so that it's ready to
     * use look up configuration in the config table, then
     * initialize it.
     */
    Config = XAxisScr_LookupConfig(DeviceId);
    if (NULL == Config) {
        return XST_FAILURE;
    }

    Status = XAxisScr_CfgInitialize(pAxisSwitch, Config,
                        Config->BaseAddress);
    if (Status != XST_SUCCESS) {
		bsp_printf(TXT_RED "In %s: AXI4-Stream initialization failed...\r\n" TXT_RST, __func__);
        return XST_FAILURE;
    }

    /* Disable register update */
    XAxisScr_RegUpdateDisable(pAxisSwitch);

    /* Disable all MI ports */
    XAxisScr_MiPortDisableAll(pAxisSwitch);

    /* Source SI[1] to MI[0] */
    XAxisScr_MiPortEnable(pAxisSwitch, MiIndex, SiIndex);

    /* Enable register update */
    XAxisScr_RegUpdateEnable(pAxisSwitch);

    /* Check for MI port enable */
    Status = XAxisScr_IsMiPortEnabled(pAxisSwitch, MiIndex, SiIndex);
    if (Status) {
    	//bsp_printf("Switch %d: MI[%d] is sourced from SI[%d].\r\n", num, MiIndex, SiIndex);
    }
    else
    {
    	return XST_FAILURE;
    }

    return XST_SUCCESS;
}

int axis_switch_cfg(u8 s_num)
{
    int Status;

    Status = AxisSwitch(XPAR_AXIS_SWITCH_0_DEVICE_ID, &AxisSwitch0, s_num, 0);
    if (Status != XST_SUCCESS)
	{
//		Xil_Assert(__FILE__, __LINE__);
		bsp_printf(TXT_RED "In %s: AxisSwitch failed...\r\n" TXT_RST, __func__);
		return XST_FAILURE ;
	}

//    Status = AxisSwitch(XPAR_AXIS_SWITCH_1_DEVICE_ID, &AxisSwitch1, 1, 0);
//	{
////		Xil_Assert(__FILE__, __LINE__);
//		return XST_FAILURE ;
//	}
}

#endif // XPAR_XAXIS_SWITCH_NUM_INSTANCES


/*
you need modify the function implement

usage: like follows
```
    Status = axis_switch_cfg(0);
    if (Status != XST_SUCCESS)
	{
		Xil_Assert(__FILE__, __LINE__);
		return XST_FAILURE ;
	}
```
*/
