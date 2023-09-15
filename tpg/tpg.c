#include "../bsp.h"
#if defined (XPAR_XV_TPG_NUM_INSTANCES)

//#if (XPAR_XV_TPG_NUM_INSTANCES == 1U)
//XV_tpg tpg_inst0;
//XVidC_ColorFormat colorFmtIn0 = XVIDC_CSF_RGB;
//u32 bckgndId0=XTPG_BKGND_COLOR_BARS;
//
//#elif (XPAR_XV_TPG_NUM_INSTANCES == 2U)
//XV_tpg tpg_inst0;
//XVidC_ColorFormat colorFmtIn0 = XVIDC_CSF_RGB;
//u32 bckgndId0=XTPG_BKGND_COLOR_BARS;
//XV_tpg tpg_inst1;
//XVidC_ColorFormat colorFmtIn1 = XVIDC_CSF_RGB;
//u32 bckgndId1=XTPG_BKGND_COLOR_BARS;
//
//#elif (XPAR_XV_TPG_NUM_INSTANCES == 3U)
//XV_tpg tpg_inst0;
//XVidC_ColorFormat colorFmtIn0 = XVIDC_CSF_RGB;
//u32 bckgndId0=XTPG_BKGND_COLOR_BARS;
//XV_tpg tpg_inst1;
//XVidC_ColorFormat colorFmtIn1 = XVIDC_CSF_RGB;
//u32 bckgndId1=XTPG_BKGND_COLOR_BARS;
//XV_tpg tpg_inst2;
//XVidC_ColorFormat colorFmtIn2 = XVIDC_CSF_RGB;
//u32 bckgndId2=XTPG_BKGND_COLOR_BARS;
//
//#elif (XPAR_XV_TPG_NUM_INSTANCES == 4U)
//XV_tpg tpg_inst0;
//XVidC_ColorFormat colorFmtIn0 = XVIDC_CSF_RGB;
//u32 bckgndId0=XTPG_BKGND_COLOR_BARS;
//XV_tpg tpg_inst1;
//XVidC_ColorFormat colorFmtIn1 = XVIDC_CSF_RGB;
//u32 bckgndId1=XTPG_BKGND_COLOR_BARS;
//XV_tpg tpg_inst2;
//XVidC_ColorFormat colorFmtIn2 = XVIDC_CSF_RGB;
//u32 bckgndId2=XTPG_BKGND_COLOR_BARS;
//XV_tpg tpg_inst3;
//XVidC_ColorFormat colorFmtIn3 = XVIDC_CSF_RGB;
//u32 bckgndId3=XTPG_BKGND_COLOR_BARS;
//
//#endif

#if (XPAR_XV_TPG_NUM_INSTANCES >= 1U)
XV_tpg tpg_inst0;
XVidC_ColorFormat colorFmtIn0 = XVIDC_CSF_RGB;
u32 bckgndId0=XTPG_BKGND_COLOR_BARS;
#endif // XPAR_XV_TPG_NUM_INSTANCES == 1U
#if (XPAR_XV_TPG_NUM_INSTANCES >= 2U)
XV_tpg tpg_inst1;
XVidC_ColorFormat colorFmtIn1 = XVIDC_CSF_RGB;
u32 bckgndId1=XTPG_BKGND_COLOR_BARS;
#endif // XPAR_XV_TPG_NUM_INSTANCES == 2U
#if (XPAR_XV_TPG_NUM_INSTANCES >= 3U)
XV_tpg tpg_inst2;
XVidC_ColorFormat colorFmtIn2 = XVIDC_CSF_RGB;
u32 bckgndId2=XTPG_BKGND_COLOR_BARS;
#endif // XPAR_XV_TPG_NUM_INSTANCES == 3U
#if (XPAR_XV_TPG_NUM_INSTANCES >= 4U)
XV_tpg tpg_inst3;
XVidC_ColorFormat colorFmtIn3 = XVIDC_CSF_RGB;
u32 bckgndId3=XTPG_BKGND_COLOR_BARS;
#endif // XPAR_XV_TPG_NUM_INSTANCES == 4U

/************************** Function Definitions *****************************/

/*****************************************************************************/
/**
*
* This function configures the TPG core.
* @param	InstancePtr is a pointer to the TPG core instance to be
*		worked on.
* @param	height is the output video height
* @param	width is the output video width
* @param	colorFormat is the output colorFormat
* @param	bckgndId is the ID for the background
*
*
******************************************************************************/
void tpg_cfg(XV_tpg *InstancePtr, u32 height, u32 width, u32 colorFormat, u32 bckgndId)
{
	// Stop TPG
	XV_tpg_DisableAutoRestart(InstancePtr);

	// Set Resolution
    XV_tpg_Set_height(InstancePtr, height);
    XV_tpg_Set_width(InstancePtr, width);

    // Set Color Space
    XV_tpg_Set_colorFormat(InstancePtr, colorFormat);

    // Change the pattern to color bar
    XV_tpg_Set_bckgndId(InstancePtr, bckgndId);

    // Enable overlay
    XV_tpg_Set_ovrlayId(InstancePtr, 0);
}

/*****************************************************************************/
/**
*
* This function sets up the moving box of the TPG core.
* @param	InstancePtr is a pointer to the TPG core instance to be
*		worked on.
* @param	boxSize is the size of the moving box
* @param	motionSpeed is the speed of the moving box
*
*
******************************************************************************/
void tpg_box(XV_tpg *InstancePtr, u32 boxSize, u32 motionSpeed)
{
    // Set Overlay to moving box
    // Set the size of the box
    XV_tpg_Set_boxSize(InstancePtr, boxSize);
    // Set the speed of the box
    XV_tpg_Set_motionSpeed(InstancePtr, motionSpeed);
    // Enable the moving box
    XV_tpg_Set_ovrlayId(InstancePtr, 1);

}


int tpg_config(void)
{
    int Status;

    Status = XV_tpg_Initialize(&tpg_inst0, XPAR_V_TPG_0_DEVICE_ID);
    if(Status!= XST_SUCCESS)
    {
        bsp_printf("TPG0 configuration failed\r\n");
        return(XST_FAILURE);
    }

    tpg_cfg(&tpg_inst0, 1080, 1920, colorFmtIn0, bckgndId0);

    //Configure the moving box of the TPG0
    tpg_box(&tpg_inst0, 50, 1);

    //Start the TPG0
    XV_tpg_EnableAutoRestart(&tpg_inst0);
    XV_tpg_Start(&tpg_inst0);
    //bsp_printf("TPG0 started!\r\n");

    return XST_SUCCESS;
}

#endif // XPAR_XV_TPG_NUM_INSTANCES
