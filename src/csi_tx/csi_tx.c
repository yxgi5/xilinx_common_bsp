#include "../bsp.h"
#if defined (XPAR_XCSI2TX_NUM_INSTANCES)

#if (XPAR_XCSI2TX_NUM_INSTANCES >= 1U)
XCsi2TxSs Csi2TxSsInst_0;
#endif
#if (XPAR_XCSI2TX_NUM_INSTANCES >= 2U)
XCsi2TxSs Csi2TxSsInst_1;
#endif
#if (XPAR_XCSI2TX_NUM_INSTANCES >= 3U)
XCsi2TxSs Csi2TxSsInst_2;
#endif
#if (XPAR_XCSI2TX_NUM_INSTANCES >= 4U)
XCsi2TxSs Csi2TxSsInst_3;
#endif

int Csi2TxSs_Init(XCsi2TxSs *InstancePtr, u32 DeviceId)
{
	int Status;
	XCsi2TxSs_Config *ConfigPtr;

	/* Obtain the device configuration for the MIPI CSI2 TX Subsystem */
	ConfigPtr = XCsi2TxSs_LookupConfig(DeviceId);
	if (!ConfigPtr) {
		return XST_FAILURE;
	}

//	ConfigPtr->DataType = 0x24; // RGB888
//	ConfigPtr->DataType = 0x2A; // RAW8
//	ConfigPtr->DataType = 0x2B; // RAW10
//	ConfigPtr->DataType = 0x2C;
//	ConfigPtr->DataType = 0x1E;
	/* Copy the device configuration into the Csi2TxSsInst's Config
	 * structure. */
	Status = XCsi2TxSs_CfgInitialize(InstancePtr, ConfigPtr,
					ConfigPtr->BaseAddr);
	if (Status != XST_SUCCESS) {
		bsp_printf(TXT_RED "MIPI CSI2 TX SS config initialization failed.\n\r" TXT_RST);
		return XST_FAILURE;
	}

	return XST_SUCCESS;
}

int csi_tx_config(void)
{
	int Status;
#if (XPAR_XCSI2TX_NUM_INSTANCES >= 1U)
	Status = Csi2TxSs_Init(&Csi2TxSsInst_0, XPAR_CSI2TXSS_0_DEVICE_ID);
	if (Status != XST_SUCCESS)
	{
		//Xil_Assert(__FILE__, __LINE__);
		return XST_FAILURE;
	}
    XCsi2TxSs_SetClkMode(&Csi2TxSsInst_0, 0);

    Status = XCsi2TxSs_SetLineCountForVC(&Csi2TxSsInst_0, 0, 1080);
    if (Status != XST_SUCCESS)
    {
    	//Xil_Assert(__FILE__, __LINE__);
    	bsp_printf(TXT_RED "In %s: XCsi2TxSs_SetLineCountForVC failed...\r\n" TXT_RST, __func__);
		return Status;
	}
//    Status = XCsi2TxSs_SetLineCountForVC(&Csi2TxSsInst_0, 1, 1080);
//    if (Status != XST_SUCCESS)
//    {
//    	bsp_printf(TXT_RED "In %s: XCsi2TxSs_SetLineCountForVC failed...\r\n" TXT_RST, __func__);
//		Xil_Assert(__FILE__, __LINE__);
//		return Status;
//	}

    Status = XCsi2TxSs_Activate(&Csi2TxSsInst_0, XCSI2TX_ENABLE);
	if (Status != XST_SUCCESS)
	{
		bsp_printf(TXT_RED "In %s: XCsi2TxSs_Activate failed...\r\n" TXT_RST, __func__);
		//Xil_Assert(__FILE__, __LINE__);
		return XST_FAILURE;
	}
#endif
#if (XPAR_XCSI2TX_NUM_INSTANCES >= 2U)
	Status = Csi2TxSs_Init(&Csi2TxSsInst_1, XPAR_CSI2TXSS_1_DEVICE_ID);
	if (Status != XST_SUCCESS)
	{
		//Xil_Assert(__FILE__, __LINE__);
		return XST_FAILURE;
	}
    XCsi2TxSs_SetClkMode(&Csi2TxSsInst_1, 0);

    Status = XCsi2TxSs_SetLineCountForVC(&Csi2TxSsInst_1, 0, 1080);
    if (Status != XST_SUCCESS)
    {
    	//Xil_Assert(__FILE__, __LINE__);
    	bsp_printf(TXT_RED "In %s: XCsi2TxSs_SetLineCountForVC failed...\r\n" TXT_RST, __func__);
		return Status;
	}
//    Status = XCsi2TxSs_SetLineCountForVC(&Csi2TxSsInst_1, 1, 1080);
//    if (Status != XST_SUCCESS)
//    {
//    	bsp_printf(TXT_RED "In %s: XCsi2TxSs_SetLineCountForVC failed...\r\n" TXT_RST, __func__);
//		Xil_Assert(__FILE__, __LINE__);
//		return Status;
//	}

    Status = XCsi2TxSs_Activate(&Csi2TxSsInst_1, XCSI2TX_ENABLE);
	if (Status != XST_SUCCESS)
	{
		bsp_printf(TXT_RED "In %s: XCsi2TxSs_Activate failed...\r\n" TXT_RST, __func__);
		//Xil_Assert(__FILE__, __LINE__);
		return XST_FAILURE;
	}
#endif
#if (XPAR_XCSI2TX_NUM_INSTANCES >= 3U)
	Status = Csi2TxSs_Init(&Csi2TxSsInst_2, XPAR_CSI2TXSS_2_DEVICE_ID);
	if (Status != XST_SUCCESS)
	{
		//Xil_Assert(__FILE__, __LINE__);
		return XST_FAILURE;
	}
    XCsi2TxSs_SetClkMode(&Csi2TxSsInst_2, 0);

    Status = XCsi2TxSs_SetLineCountForVC(&Csi2TxSsInst_2, 0, 1080);
    if (Status != XST_SUCCESS)
    {
    	//Xil_Assert(__FILE__, __LINE__);
    	bsp_printf(TXT_RED "In %s: XCsi2TxSs_SetLineCountForVC failed...\r\n" TXT_RST, __func__);
		return Status;
	}
//    Status = XCsi2TxSs_SetLineCountForVC(&Csi2TxSsInst_2, 1, 1080);
//    if (Status != XST_SUCCESS)
//    {
//    	bsp_printf(TXT_RED "In %s: XCsi2TxSs_SetLineCountForVC failed...\r\n" TXT_RST, __func__);
//		Xil_Assert(__FILE__, __LINE__);
//		return Status;
//	}

    Status = XCsi2TxSs_Activate(&Csi2TxSsInst_2, XCSI2TX_ENABLE);
	if (Status != XST_SUCCESS)
	{
		bsp_printf(TXT_RED "In %s: XCsi2TxSs_Activate failed...\r\n" TXT_RST, __func__);
		//Xil_Assert(__FILE__, __LINE__);
		return XST_FAILURE;
	}
#endif
#if (XPAR_XCSI2TX_NUM_INSTANCES >= 4U)
	Status = Csi2TxSs_Init(&Csi2TxSsInst_3, XPAR_CSI2TXSS_3_DEVICE_ID);
	if (Status != XST_SUCCESS)
	{
		//Xil_Assert(__FILE__, __LINE__);
		return XST_FAILURE;
	}
    XCsi2TxSs_SetClkMode(&Csi2TxSsInst_3, 0);

    Status = XCsi2TxSs_SetLineCountForVC(&Csi2TxSsInst_3, 0, 1080);
    if (Status != XST_SUCCESS)
    {
    	//Xil_Assert(__FILE__, __LINE__);
    	bsp_printf(TXT_RED "In %s: XCsi2TxSs_SetLineCountForVC failed...\r\n" TXT_RST, __func__);
		return Status;
	}
//    Status = XCsi2TxSs_SetLineCountForVC(&Csi2TxSsInst_3, 1, 1080);
//    if (Status != XST_SUCCESS)
//    {
//    	bsp_printf(TXT_RED "In %s: XCsi2TxSs_SetLineCountForVC failed...\r\n" TXT_RST, __func__);
//		Xil_Assert(__FILE__, __LINE__);
//		return Status;
//	}

    Status = XCsi2TxSs_Activate(&Csi2TxSsInst_3, XCSI2TX_ENABLE);
	if (Status != XST_SUCCESS)
	{
		bsp_printf(TXT_RED "In %s: XCsi2TxSs_Activate failed...\r\n" TXT_RST, __func__);
		//Xil_Assert(__FILE__, __LINE__);
		return XST_FAILURE;
	}
#endif
    return XST_SUCCESS;
}

#endif // XPAR_XCSI2TX_NUM_INSTANCES


/*
usage:
assume you have a xgpio to set 96 bit user data

declare a globle variable before main()
```
XGpio XGpioOutput;
```

call follows before main_loop, especially call csi_tx_config() after axis prepared(tpg,vdma,vpss)

    Status = xgpio_setup(&XGpioOutput, XPAR_GPIO_0_DEVICE_ID, 0, 0) ;
    if (Status != XST_SUCCESS)
	{
    	Xil_Assert(__FILE__, __LINE__);
		return XST_FAILURE ;
	}
	XGpio_DiscreteWrite(&XGpioOutput, 1, 0x24); // RGB888
//	XGpio_DiscreteWrite(&XGpioOutput, 1, 0x2A); // RAW8
//	XGpio_DiscreteWrite(&XGpioOutput, 1, 0x2B); // RAW10
//	XGpio_DiscreteWrite(&XGpioOutput, 1, 0x2C); // RAW12
//	XGpio_DiscreteWrite(&XGpioOutput, 1, 0x1E); // YUV422_8bit
//	XGpio_DiscreteWrite(&XGpioOutput, 2, VIDEO_COLUMNS*24/8<<16); // WC RGB888
//	XGpio_DiscreteWrite(&XGpioOutput, 2, 3840*24/8<<16); // WC RGB888
//	XGpio_DiscreteWrite(&XGpioOutput, 2, (1920*12/8)<<16); // WC RAW12
	XGpio_DiscreteWrite(&XGpioOutput, 2, (1920*24/8)<<16); // WC RGB888
//	XGpio_DiscreteWrite(&XGpioOutput, 2, (VIDEO_COLUMNS*10/8)<<16); // WC RAW10
//	XGpio_DiscreteWrite(&XGpioOutput, 2, (1920*16/8)<<16); // WC YUV422_8bit

	//after axis prepared
	Status = csi_tx_config();
	if (Status != XST_SUCCESS)
	{
		Xil_Assert(__FILE__, __LINE__);
		return XST_FAILURE ;
	}
*/
