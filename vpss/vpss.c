/*
 * vpss.c
 *
 *  Created on: 2023年5月4日
 *      Author: fengke
 */

#include "../bsp.h"
#if defined (XPAR_XVPROCSS_NUM_INSTANCES)

XVprocSs VprocInst0;

XVidC_VideoMode resId;
XVidC_VideoStream StreamIn, StreamOut;
XVidC_VideoTiming const *TimingPtr;

XVidC_VideoMode resId;
XVidC_VideoStream StreamIn, StreamOut;
XVidC_VideoTiming const *TimingPtr;
XVidC_FrameRate fpsIn = XVIDC_FR_30HZ;
XVidC_FrameRate fpsOut = XVIDC_FR_30HZ;
XVidC_ColorFormat colorFmtIn0 = XVIDC_CSF_RGB;
//XVidC_ColorFormat colorFmtIn0 = XVIDC_CSF_YCRCB_422;
XVidC_ColorFormat colorFmtOut0 = XVIDC_CSF_RGB;

/* Assign Mode ID Enumeration. First entry Must be > XVIDC_VM_CUSTOM */
typedef enum {
	XVIDC_VM_1280x3840_30_P = (XVIDC_VM_CUSTOM + 1),
	XVIDC_VM_320x960_30_P = (XVIDC_VM_1280x3840_30_P + 1),
	XVIDC_VM_1920x2160_30_P = (XVIDC_VM_320x960_30_P + 1),
	XVIDC_VM_960x1080_30_P = (XVIDC_VM_1920x2160_30_P + 1),
    XVIDC_CM_NUM_SUPPORTED
} XVIDC_CUSTOM_MODES;

/* Create entry for each mode in the custom table */
const XVidC_VideoTimingMode XVidC_MyVideoTimingMode[(XVIDC_CM_NUM_SUPPORTED - (XVIDC_VM_CUSTOM + 1))] =
{
    { XVIDC_VM_1280x3840_30_P, "1280x3840@30Hz", XVIDC_FR_30HZ,
        {1280, 110, 40, 220, 1650, 1,
        		3840, 5, 5, 20, 3870, 0, 0, 0, 0, 1} },
    { XVIDC_VM_320x960_30_P, "320x960@30Hz", XVIDC_FR_30HZ,
        {320, 10, 10, 10, 350, 1,
        		960, 5, 5, 10, 980, 0, 0, 0, 0, 1} },
    { XVIDC_VM_1920x2160_30_P, "1920x2160@30Hz", XVIDC_FR_30HZ,
		{1920, 10, 10, 10, 1950, 1,
				2160, 5, 5, 10, 2180, 0, 0, 0, 0, 1} },
	{ XVIDC_VM_960x1080_30_P, "960x1080@30Hz", XVIDC_FR_30HZ,
		{960, 10, 10, 10, 990, 1,
				1080, 5, 5, 10, 1100, 0, 0, 0, 0, 1} }
};


int vpss_config(void)
{
	int Status;
	XVprocSs_Config *VprocCfgPtr;

    /* User registers custom timing table */
	//bsp_printf("INFO> Registering Custom Timing Table with %d entries \r\n", (XVIDC_CM_NUM_SUPPORTED - (XVIDC_VM_CUSTOM + 1)));
	Status = XVidC_RegisterCustomTimingModes(XVidC_MyVideoTimingMode, (XVIDC_CM_NUM_SUPPORTED - (XVIDC_VM_CUSTOM + 1)));
	if (Status != XST_SUCCESS)
	{
		bsp_printf(TXT_RED "In %s: register custom timing table failed...\r\n" TXT_RST, __func__);
		Xil_Assert(__FILE__, __LINE__);
		return(XST_FAILURE);
	}

    /* VPSS Configuration*/
	memset(&VprocInst0, 0, sizeof(XVprocSs));
//	memset(VprocCfgPtr, 0, sizeof(XVprocSs_Config));
    VprocCfgPtr = XVprocSs_LookupConfig(XPAR_XVPROCSS_0_DEVICE_ID);

//    switch (VprocCfgPtr->Topology)
//    {
//        case XVPROCSS_TOPOLOGY_FULL_FLEDGED:
//        case XVPROCSS_TOPOLOGY_DEINTERLACE_ONLY:
//			XVprocSs_SetFrameBufBaseaddr(&VprocInst0, USR_FRAME_BUF_BASEADDR);
//			break;
//
//        default:
//            break;
//    }

    //XVprocSs_LogReset(&VprocInst0);

    Status = XVprocSs_CfgInitialize(&VprocInst0, VprocCfgPtr, VprocCfgPtr->BaseAddress);
    if(Status != XST_SUCCESS)
	{
//		while(1)
//			NOP();
    	bsp_printf(TXT_RED "In %s: XVprocSs_CfgInitialize failed...\r\n" TXT_RST, __func__);
    	// Xil_Assert(__FILE__, __LINE__);
		return(XST_FAILURE);
	}

    //Get the resolution details
//    resId = XVidC_GetVideoModeId(1920, 2160, fpsIn, 0);
    resId = XVidC_GetVideoModeId(1920, 1080, fpsIn, 0);

    TimingPtr = XVidC_GetTimingInfo(resId);

//    TimingPtr->HActive = 1280;
//    TimingPtr->HFrontPorch = 110;
//	TimingPtr->HSyncWidth = 40;
//	TimingPtr->HBackPorch = 220;
//	TimingPtr->HTotal = 1650;
//	TimingPtr->HSyncPolarity = 1;
//	TimingPtr->VActive = 3840;
//	TimingPtr->F0PVFrontPorch = 5;
//	TimingPtr->F0PVSyncWidth = 5;
//	TimingPtr->F0PVBackPorch = 20;
//	TimingPtr->F0PVTotal = 3870;
	//	TimingPtr->F1VFrontPorch = 0;
//	TimingPtr->F1VSyncWidth = 0;
//	TimingPtr->F1VBackPorch = 0;
//	TimingPtr->F1VTotal = 0;
//	TimingPtr->VSyncPolarity = 1;

	//Set the input stream
	StreamIn.VmId           = resId;
	StreamIn.Timing         = *TimingPtr;
	StreamIn.ColorFormatId  = colorFmtIn0;
	StreamIn.ColorDepth     = VprocCfgPtr->ColorDepth;
	StreamIn.PixPerClk      = VprocCfgPtr->PixPerClock;
	StreamIn.FrameRate      = fpsIn;
	StreamIn.IsInterlaced   = 0;
	Status = XVprocSs_SetVidStreamIn(&VprocInst0, &StreamIn);
	if (Status != XST_SUCCESS)
	{
		bsp_printf(TXT_RED "In %s: XVprocSs_SetVidStreamIn failed...\r\n" TXT_RST, __func__);
		// Xil_Assert(__FILE__, __LINE__);
		return XST_FAILURE ;
	}

//    resId = XVidC_GetVideoModeId(960, 1080, fpsOut, 0);
    resId = XVidC_GetVideoModeId(1920, 1080, fpsOut, 0);

    TimingPtr = XVidC_GetTimingInfo(resId);

//    TimingPtr->HActive = 1280;
//    TimingPtr->HFrontPorch = 110;
//	TimingPtr->HSyncWidth = 40;
//	TimingPtr->HBackPorch = 220;
//	TimingPtr->HTotal = 1650;
//	TimingPtr->HSyncPolarity = 1;
//	TimingPtr->VActive = 3840;
//	TimingPtr->F0PVFrontPorch = 5;
//	TimingPtr->F0PVSyncWidth = 5;
//	TimingPtr->F0PVBackPorch = 20;
//	TimingPtr->F0PVTotal = 3870;
//	TimingPtr->F1VFrontPorch = 0;
//	TimingPtr->F1VSyncWidth = 0;
//	TimingPtr->F1VBackPorch = 0;
//	TimingPtr->F1VTotal = 0;
//	TimingPtr->VSyncPolarity = 1;

    //Set the output stream
    StreamOut.VmId           = resId;
    StreamOut.Timing         = *TimingPtr;
    StreamOut.ColorFormatId  = colorFmtOut0;
    StreamOut.ColorDepth     = VprocCfgPtr->ColorDepth;
    StreamOut.PixPerClk      = VprocCfgPtr->PixPerClock;
    StreamOut.FrameRate      = fpsOut;
    StreamOut.IsInterlaced   = 0;
    Status = XVprocSs_SetVidStreamOut(&VprocInst0, &StreamOut);
	if (Status != XST_SUCCESS)
	{
		bsp_printf(TXT_RED "In %s: XVprocSs_SetVidStreamOut failed...\r\n" TXT_RST, __func__);
		//Xil_Assert(__FILE__, __LINE__);
		return XST_FAILURE ;
	}

    Status = XVprocSs_SetSubsystemConfig(&VprocInst0);
    if(Status!= XST_SUCCESS)
    {
    	bsp_printf(TXT_RED "In %s: XVprocSs_SetSubsystemConfig failed...\r\n" TXT_RST, __func__);
    	//Xil_Assert(__FILE__, __LINE__);
    	return(XST_FAILURE);
    }
    //bsp_printf("VPSS0 Started\r\n");
}

#endif // XPAR_XVPROCSS_NUM_INSTANCES
