#include "../bsp.h"

#ifdef XPAR_XAXIVDMA_NUM_INSTANCES

#if (XPAR_XAXIVDMA_NUM_INSTANCES >= 1U)
XAxiVdma Vdma0;
#endif
#if (XPAR_XAXIVDMA_NUM_INSTANCES >= 2U)
XAxiVdma Vdma1;
#endif
#if (XPAR_XAXIVDMA_NUM_INSTANCES >= 3U)
XAxiVdma Vdma2;
#endif
#if (XPAR_XAXIVDMA_NUM_INSTANCES >= 4U)
XAxiVdma Vdma3;
#endif
#if (XPAR_XAXIVDMA_NUM_INSTANCES >= 5U)
XAxiVdma Vdma4;
#endif
#if (XPAR_XAXIVDMA_NUM_INSTANCES >= 6U)
XAxiVdma Vdma5;
#endif
#if (XPAR_XAXIVDMA_NUM_INSTANCES >= 7U)
XAxiVdma Vdma6;
#endif
#if (XPAR_XAXIVDMA_NUM_INSTANCES >= 8U)
XAxiVdma Vdma7;
#endif

int vdma_init(XAxiVdma *InstancePtr, u16 DeviceID)
{
	int Status;
	XAxiVdma_Config *Config;

	Config = XAxiVdma_LookupConfig(DeviceID);
	if (NULL == Config) {
		bsp_printf(TXT_RED "In %s: XAxiVdma_LookupConfig failed...\r\n" TXT_RST, __func__);
		return XST_FAILURE;
	}

	Status = XAxiVdma_CfgInitialize(InstancePtr, Config, Config->BaseAddress);
	if (Status != XST_SUCCESS) {
		bsp_printf(TXT_RED "In %s: XAxiVdma_CfgInitialize failed...\r\n" TXT_RST, __func__);
		return XST_FAILURE;
	}

	return XST_SUCCESS;
}


int vdma_read_start(XAxiVdma *InstancePtr)
{
	int Status;

	Xil_AssertNonvoid(InstancePtr != NULL);

	// MM2S Startup
	Status = XAxiVdma_DmaStart(InstancePtr, XAXIVDMA_READ);
	if (Status != XST_SUCCESS)
	{
	   bsp_printf(TXT_RED "Start read transfer failed %d\r\n" TXT_RST, Status);
	   return XST_FAILURE;
	}

	return XST_SUCCESS;
}


void vdma_read_stop(XAxiVdma *InstancePtr)
{
	Xil_AssertNonvoid(InstancePtr != NULL);

	XAxiVdma_DmaStop(InstancePtr, XAXIVDMA_READ);
}

int vdma_read_init
(
	XAxiVdma *InstancePtr,
	u16 DeviceID,
	u16 HoriSizeInput,
	u16 VertSizeInput,
	u16 Stride,
	UINTPTR FrameStoreStartAddr0,
	UINTPTR FrameStoreStartAddr1,
	UINTPTR FrameStoreStartAddr2
)
{
	int Status;
	XAxiVdma_FrameCounter FrameCfg;
	XAxiVdma_DmaSetup ReadCfg;

	if (InstancePtr->IsReady != XAXIVDMA_DEVICE_READY)
	{
		Status = vdma_init(InstancePtr, DeviceID);
		if (Status != XST_SUCCESS)
		{
			bsp_printf(TXT_RED "In %s: XAxiVdma_CfgInitialize failed...\r\n" TXT_RST, __func__);
			return XST_FAILURE;
		}
	}

	Status = XAxiVdma_SetFrmStore(InstancePtr, 3, XAXIVDMA_READ);
	if (Status != XST_SUCCESS)
	{
		bsp_printf(TXT_RED
			"Setting Frame Store Number Failed in Read Channel"
							" %d\r\n" TXT_RST, Status);
		return XST_FAILURE;
	}

	XAxiVdma_GetFrameCounter(InstancePtr, &FrameCfg) ;

	if (FrameCfg.ReadFrameCount == 0)
		FrameCfg.ReadFrameCount = 1 ;

	FrameCfg.WriteFrameCount = 1;
	FrameCfg.WriteDelayTimerCount = 10;

	Status = XAxiVdma_SetFrameCounter(InstancePtr, &FrameCfg);
	if (Status != XST_SUCCESS)
	{
		bsp_printf(TXT_RED
			"Set frame counter failed %d\r\n" TXT_RST, Status);

		if(Status == XST_VDMA_MISMATCH_ERROR)
			bsp_printf("DMA Mismatch Error\r\n");
			bsp_printf(TXT_RED "DMA Mismatch Error\r\n" TXT_RST);

		return XST_FAILURE;
	}

	ReadCfg.EnableCircularBuf = 1;
	ReadCfg.EnableFrameCounter = 0;
	ReadCfg.FixedFrameStoreAddr = 0;

	ReadCfg.EnableSync = 1;
	ReadCfg.PointNum = 1;

	ReadCfg.FrameDelay = 0;

	ReadCfg.VertSizeInput = VertSizeInput;
	ReadCfg.HoriSizeInput = HoriSizeInput;
	ReadCfg.Stride = Stride;

	Status = XAxiVdma_DmaConfig(InstancePtr, XAXIVDMA_READ, &ReadCfg);
	if (Status != XST_SUCCESS)
	{
		bsp_printf(TXT_RED "In %s: Read channel config failed %d\r\n" TXT_RST, __func__, , Status);
		return XST_FAILURE;
	}

	ReadCfg.FrameStoreStartAddr[0] = FrameStoreStartAddr0;
	ReadCfg.FrameStoreStartAddr[1] = FrameStoreStartAddr1;
	ReadCfg.FrameStoreStartAddr[2] = FrameStoreStartAddr2;

	Status = XAxiVdma_DmaSetBufferAddr(InstancePtr, XAXIVDMA_READ, ReadCfg.FrameStoreStartAddr);
	if (Status != XST_SUCCESS)
	{
		bsp_printf(TXT_RED "In %s: Read channel set buffer address failed %d\r\n" TXT_RST, __func__, , Status);
		return XST_FAILURE;
	}

	Status = vdma_read_start(InstancePtr);
	if (Status != XST_SUCCESS)
	{
		bsp_printf(TXT_RED "In %s: Starting VDMA failed...\r\n" TXT_RST, __func__);
		return Status;
	}

	return XST_SUCCESS;
}

int vdma_write_start(XAxiVdma *InstancePtr)
{
	Xil_AssertNonvoid(InstancePtr != NULL);
	int Status;

	// S2MM Startup
	Status = XAxiVdma_DmaStart(InstancePtr, XAXIVDMA_WRITE);
	if (Status != XST_SUCCESS)
	{
		bsp_printf(TXT_RED "In %s: Start write transfer failed...\r\n" TXT_RST, __func__);
		return XST_FAILURE;
	}
	return XST_SUCCESS;
}

void vdma_write_stop(XAxiVdma *InstancePtr)
{
	Xil_AssertNonvoid(InstancePtr != NULL);

	XAxiVdma_DmaStop(InstancePtr, XAXIVDMA_WRITE);
}

int vdma_write_init
(
	XAxiVdma *InstancePtr,
	u16 DeviceID,
	u16 HoriSizeInput,
	u16 VertSizeInput,
	u16 Stride,
	UINTPTR FrameStoreStartAddr0,
	UINTPTR FrameStoreStartAddr1,
	UINTPTR FrameStoreStartAddr2
)
{
	int Status;
	XAxiVdma_FrameCounter FrameCfg;
	XAxiVdma_DmaSetup WriteCfg;

	if (InstancePtr->IsReady != XAXIVDMA_DEVICE_READY)
	{
		Status = vdma_init(InstancePtr, DeviceID);
		if (Status != XST_SUCCESS)
		{
			bsp_printf(TXT_RED "In %s: XAxiVdma_CfgInitialize failed...\r\n" TXT_RST, __func__);
			return XST_FAILURE;
		}
	}

	Status = XAxiVdma_SetFrmStore(InstancePtr, 3, XAXIVDMA_WRITE);
	if (Status != XST_SUCCESS)
	{
		bsp_printf(TXT_RED
		    "Setting Frame Store Number Failed in Write Channel"
							" %d\r\n" TXT_RST, Status);
		return XST_FAILURE;
	}

	XAxiVdma_GetFrameCounter(InstancePtr, &FrameCfg) ;

	if (FrameCfg.ReadFrameCount == 0)
		FrameCfg.ReadFrameCount = 1 ;

	FrameCfg.WriteFrameCount = 1;
	FrameCfg.WriteDelayTimerCount = 10;

	Status = XAxiVdma_SetFrameCounter(InstancePtr, &FrameCfg);
	if (Status != XST_SUCCESS)
	{
		bsp_printf(TXT_RED
			"Set frame counter failed %d\r\n" TXT_RST, Status);
		if(Status == XST_VDMA_MISMATCH_ERROR)
			bsp_printf(TXT_RED "DMA Mismatch Error\r\n" TXT_RST);
		return XST_FAILURE;
	}

	WriteCfg.EnableCircularBuf = 0;
	WriteCfg.EnableFrameCounter = 0;
	WriteCfg.FixedFrameStoreAddr = 0;

	WriteCfg.EnableSync = 1;
	WriteCfg.PointNum = 1;

	WriteCfg.FrameDelay = 0;

	WriteCfg.VertSizeInput = VertSizeInput;
	WriteCfg.HoriSizeInput = HoriSizeInput;
	WriteCfg.Stride = Stride;

	Status = XAxiVdma_DmaConfig(InstancePtr, XAXIVDMA_WRITE, &WriteCfg);
	if (Status != XST_SUCCESS)
	{
		bsp_printf(TXT_RED "In %s: Write channel config failed %d\r\n" TXT_RST, __func__, Status);
		return XST_FAILURE;
	}


	WriteCfg.FrameStoreStartAddr[0] = FrameStoreStartAddr0;
	WriteCfg.FrameStoreStartAddr[1] = FrameStoreStartAddr1;
	WriteCfg.FrameStoreStartAddr[2] = FrameStoreStartAddr2;

	Status = XAxiVdma_DmaSetBufferAddr(InstancePtr, XAXIVDMA_WRITE, WriteCfg.FrameStoreStartAddr);
	if (Status != XST_SUCCESS)
	{
		bsp_printf(TXT_RED "In %s: Write channel set buffer address failed %d\r\n" TXT_RST, __func__, Status);
		return XST_FAILURE;
	}

	Status = vdma_write_start(InstancePtr);
	if (Status != XST_SUCCESS)
	{
		bsp_printf(TXT_RED "In %s: Start VDMA failed...\r\n" TXT_RST, __func__);
		return Status;
	}

	return XST_SUCCESS;
}

//void vdma_config_m32_0(void)
//{
//    /* Start of VDMA Configuration */
//    u32 bytePerPixels = 3;
//
//    int offset0 = 0; // (y*w+x)*Bpp
//    int offset1 = 0; // (y*w+x)*Bpp
//
//    u32 stride0 = 1920;
//    u32 width0 = 1920;
//    u32 height0 = 1080;
//    u32 stride1 = 1920;  // crop keeps write Stride
//    u32 width1 = 1920;
//    u32 height1 = 1080;
//
//    /* Configure the Write interface (S2MM)*/
//    // S2MM Control Register
//    Xil_Out32(XPAR_AXI_VDMA_0_BASEADDR + 0x30, 0x8B);
//    //S2MM Start Address 1
//    Xil_Out32(XPAR_AXI_VDMA_0_BASEADDR + 0xAC, FRAME_BUFFER_0 + offset0);
//    //S2MM Start Address 2
//    Xil_Out32(XPAR_AXI_VDMA_0_BASEADDR + 0xB0, FRAME_BUFFER_1 + offset0);
//    //S2MM Start Address 3
//    Xil_Out32(XPAR_AXI_VDMA_0_BASEADDR + 0xB4, FRAME_BUFFER_2 + offset0);
//    //S2MM Frame delay / Stride register
//    Xil_Out32(XPAR_AXI_VDMA_0_BASEADDR + 0xA8, stride0*bytePerPixels);
//    // S2MM HSIZE register
//    Xil_Out32(XPAR_AXI_VDMA_0_BASEADDR + 0xA4, width0*bytePerPixels);
//    // S2MM VSIZE register
//    Xil_Out32(XPAR_AXI_VDMA_0_BASEADDR + 0xA0, height0);
//
//    /* Configure the Read interface (MM2S)*/
//    // MM2S Control Register
//    Xil_Out32(XPAR_AXI_VDMA_0_BASEADDR + 0x00, 0x8B);
//    // MM2S Start Address 1
//    Xil_Out32(XPAR_AXI_VDMA_0_BASEADDR + 0x5C, FRAME_BUFFER_0 + offset1);
//    // MM2S Start Address 2
//    Xil_Out32(XPAR_AXI_VDMA_0_BASEADDR + 0x60, FRAME_BUFFER_1 + offset1);
//    // MM2S Start Address 3
//    Xil_Out32(XPAR_AXI_VDMA_0_BASEADDR + 0x64, FRAME_BUFFER_2 + offset1);
//    // MM2S Frame delay / Stride register
//    Xil_Out32(XPAR_AXI_VDMA_0_BASEADDR + 0x58, stride1*bytePerPixels);
//    // MM2S HSIZE register
//    Xil_Out32(XPAR_AXI_VDMA_0_BASEADDR + 0x54, width1*bytePerPixels);
//    // MM2S VSIZE register
//    Xil_Out32(XPAR_AXI_VDMA_0_BASEADDR + 0x50, height1);
//
////    bsp_printf("VDMA started!\r\n");
//	/* End of VDMA Configuration */
//}
//
//void vdma_config_m64_0(void)
//{
//    /* Start of VDMA Configuration */
//    u32 bytePerPixels = 3;
//
//    int offset0 = 0; // (y*w+x)*Bpp
//    int offset1 = 0; // (y*w+x)*Bpp
//
//    u32 stride0 = 1920;
//    u32 width0 = 1920;
//    u32 height0 = 1080;
//    u32 stride1 = 1920;  // crop keeps write Stride
//    u32 width1 = 1920;
//    u32 height1 = 1080;
//
//    /* Configure the Write interface (S2MM)*/
//    // S2MM Control Register
//    Xil_Out32(XPAR_AXI_VDMA_0_BASEADDR + 0x30, 0x8B);
//    //S2MM Start Address 1
//    Xil_Out32(XPAR_AXI_VDMA_0_BASEADDR + 0xAC, FRAME_BUFFER_0 + offset0);
//    Xil_Out32(XPAR_AXI_VDMA_0_BASEADDR + 0xB0, 0);
//    //S2MM Start Address 2
//    Xil_Out32(XPAR_AXI_VDMA_0_BASEADDR + 0xB4, FRAME_BUFFER_1 + offset0);
//    Xil_Out32(XPAR_AXI_VDMA_0_BASEADDR + 0xB8, 0);
//    //S2MM Start Address 3
//    Xil_Out32(XPAR_AXI_VDMA_0_BASEADDR + 0xBC, FRAME_BUFFER_2 + offset0);
//    Xil_Out32(XPAR_AXI_VDMA_0_BASEADDR + 0xC0, 0);
//    //S2MM Frame delay / Stride register
//    Xil_Out32(XPAR_AXI_VDMA_0_BASEADDR + 0xA8, stride0*bytePerPixels);
//    // S2MM HSIZE register
//    Xil_Out32(XPAR_AXI_VDMA_0_BASEADDR + 0xA4, width0*bytePerPixels);
//    // S2MM VSIZE register
//    Xil_Out32(XPAR_AXI_VDMA_0_BASEADDR + 0xA0, height0);
//
//    /* Configure the Read interface (MM2S)*/
//    // MM2S Control Register
//    Xil_Out32(XPAR_AXI_VDMA_0_BASEADDR + 0x00, 0x8B);
//    // MM2S Start Address 1
//    Xil_Out32(XPAR_AXI_VDMA_0_BASEADDR + 0x5C, FRAME_BUFFER_0 + offset1);
//    Xil_Out32(XPAR_AXI_VDMA_0_BASEADDR + 0x60, 0);
//    // MM2S Start Address 2
//    Xil_Out32(XPAR_AXI_VDMA_0_BASEADDR + 0x64, FRAME_BUFFER_1 + offset1);
//    Xil_Out32(XPAR_AXI_VDMA_0_BASEADDR + 0x68, 0);
//    // MM2S Start Address 3
//    Xil_Out32(XPAR_AXI_VDMA_0_BASEADDR + 0x6C, FRAME_BUFFER_2 + offset1);
//    Xil_Out32(XPAR_AXI_VDMA_0_BASEADDR + 0x70, 0);
//    // MM2S Frame delay / Stride register
//    Xil_Out32(XPAR_AXI_VDMA_0_BASEADDR + 0x58, stride1*bytePerPixels);
//    // MM2S HSIZE register
//    Xil_Out32(XPAR_AXI_VDMA_0_BASEADDR + 0x54, width1*bytePerPixels);
//    // MM2S VSIZE register
//    Xil_Out32(XPAR_AXI_VDMA_0_BASEADDR + 0x50, height1);
//
////    bsp_printf("VDMA started!\r\n");
//    /* End of VDMA Configuration */
//}

#if (XPAR_XAXIVDMA_NUM_INSTANCES >= 1U)
#if (XPAR_AXI_VDMA_0_ADDR_WIDTH == 32U)
void vdma_config_m32_0(void)
{
    /* Start of VDMA Configuration */
    u32 bytePerPixels = VDMA_0_BPP;

    int offset0 = VDMA_0_W_OFFSET; // (y*w+x)*Bpp
    int offset1 = VDMA_0_R_OFFSET; // (y*w+x)*Bpp

    u32 stride0 = VDMA_0_W_STRIDE;
    u32 width0 = VDMA_0_W_WIDTH;
    u32 height0 = VDMA_0_W_HEIGHTH;
    u32 stride1 = VDMA_0_R_STRIDE;  // crop keeps write Stride
    u32 width1 = VDMA_0_R_WIDTH;
    u32 height1 = VDMA_0_R_HEIGHTH;

    /* Configure the Write interface (S2MM)*/
    // S2MM Control Register
    Xil_Out32(XPAR_AXI_VDMA_0_BASEADDR + 0x30, 0x8B);
#if (XPAR_AXI_VDMA_0_NUM_FSTORES >= 1U)
    //S2MM Start Address 1
    Xil_Out32(XPAR_AXI_VDMA_0_BASEADDR + 0xAC, FRAME_BUFFER_0_0 + offset0);
#endif // XPAR_AXI_VDMA_0_NUM_FSTORES == 1U
#if (XPAR_AXI_VDMA_0_NUM_FSTORES >= 2U)
    //S2MM Start Address 2
    Xil_Out32(XPAR_AXI_VDMA_0_BASEADDR + 0xB0, FRAME_BUFFER_0_1 + offset0);
#endif // XPAR_AXI_VDMA_0_NUM_FSTORES == 2U
#if (XPAR_AXI_VDMA_0_NUM_FSTORES >= 3U)
    //S2MM Start Address 3
    Xil_Out32(XPAR_AXI_VDMA_0_BASEADDR + 0xB4, FRAME_BUFFER_0_2 + offset0);
#endif // XPAR_AXI_VDMA_0_NUM_FSTORES == 3U
    //S2MM Frame delay / Stride register
    Xil_Out32(XPAR_AXI_VDMA_0_BASEADDR + 0xA8, stride0*bytePerPixels);
    // S2MM HSIZE register
    Xil_Out32(XPAR_AXI_VDMA_0_BASEADDR + 0xA4, width0*bytePerPixels);
    // S2MM VSIZE register
    Xil_Out32(XPAR_AXI_VDMA_0_BASEADDR + 0xA0, height0);

    /* Configure the Read interface (MM2S)*/
    // MM2S Control Register
    Xil_Out32(XPAR_AXI_VDMA_0_BASEADDR + 0x00, 0x8B);
#if (XPAR_AXI_VDMA_0_NUM_FSTORES >= 1U)
    // MM2S Start Address 1
    Xil_Out32(XPAR_AXI_VDMA_0_BASEADDR + 0x5C, FRAME_BUFFER_0_0 + offset1);
#endif // XPAR_AXI_VDMA_0_NUM_FSTORES == 1U
#if (XPAR_AXI_VDMA_0_NUM_FSTORES >= 2U)
    // MM2S Start Address 2
    Xil_Out32(XPAR_AXI_VDMA_0_BASEADDR + 0x60, FRAME_BUFFER_0_1 + offset1);
#endif // XPAR_AXI_VDMA_0_NUM_FSTORES == 2U
#if (XPAR_AXI_VDMA_0_NUM_FSTORES >= 3U)
    // MM2S Start Address 3
    Xil_Out32(XPAR_AXI_VDMA_0_BASEADDR + 0x64, FRAME_BUFFER_0_2 + offset1);
#endif // XPAR_AXI_VDMA_0_NUM_FSTORES == 3U
    // MM2S Frame delay / Stride register
    Xil_Out32(XPAR_AXI_VDMA_0_BASEADDR + 0x58, stride1*bytePerPixels);
    // MM2S HSIZE register
    Xil_Out32(XPAR_AXI_VDMA_0_BASEADDR + 0x54, width1*bytePerPixels);
    // MM2S VSIZE register
    Xil_Out32(XPAR_AXI_VDMA_0_BASEADDR + 0x50, height1);

//    bsp_printf("VDMA started!\r\n");
	/* End of VDMA Configuration */
}
#endif // XPAR_AXI_VDMA_0_ADDR_WIDTH == 32U
#if (XPAR_AXI_VDMA_0_ADDR_WIDTH == 64U)
void vdma_config_m64_0(void)
{
    /* Start of VDMA Configuration */
    u32 bytePerPixels = VDMA_0_BPP;

    int offset0 = VDMA_0_W_OFFSET; // (y*w+x)*Bpp
    int offset1 = VDMA_0_R_OFFSET; // (y*w+x)*Bpp

    u32 stride0 = VDMA_0_W_STRIDE;
    u32 width0 = VDMA_0_W_WIDTH;
    u32 height0 = VDMA_0_W_HEIGHTH;
    u32 stride1 = VDMA_0_R_STRIDE;  // crop keeps write Stride
    u32 width1 = VDMA_0_R_WIDTH;
    u32 height1 = VDMA_0_R_HEIGHTH;

    /* Configure the Write interface (S2MM)*/
    // S2MM Control Register
    Xil_Out32(XPAR_AXI_VDMA_0_BASEADDR + 0x30, 0x8B);
#if (XPAR_AXI_VDMA_0_NUM_FSTORES >= 1U)
    //S2MM Start Address 1
    Xil_Out32(XPAR_AXI_VDMA_0_BASEADDR + 0xAC, FRAME_BUFFER_0_0 + offset0);
    Xil_Out32(XPAR_AXI_VDMA_0_BASEADDR + 0xB0, 0);
#endif // XPAR_AXI_VDMA_0_NUM_FSTORES == 1U
#if (XPAR_AXI_VDMA_0_NUM_FSTORES >= 2U)
    //S2MM Start Address 2
    Xil_Out32(XPAR_AXI_VDMA_0_BASEADDR + 0xB4, FRAME_BUFFER_0_1 + offset0);
    Xil_Out32(XPAR_AXI_VDMA_0_BASEADDR + 0xB8, 0);
#endif // XPAR_AXI_VDMA_0_NUM_FSTORES == 2U
#if (XPAR_AXI_VDMA_0_NUM_FSTORES >= 3U)
    //S2MM Start Address 3
    Xil_Out32(XPAR_AXI_VDMA_0_BASEADDR + 0xBC, FRAME_BUFFER_0_2 + offset0);
    Xil_Out32(XPAR_AXI_VDMA_0_BASEADDR + 0xC0, 0);
#endif // XPAR_AXI_VDMA_0_NUM_FSTORES == 3U
    //S2MM Frame delay / Stride register
    Xil_Out32(XPAR_AXI_VDMA_0_BASEADDR + 0xA8, stride0*bytePerPixels);
    // S2MM HSIZE register
    Xil_Out32(XPAR_AXI_VDMA_0_BASEADDR + 0xA4, width0*bytePerPixels);
    // S2MM VSIZE register
    Xil_Out32(XPAR_AXI_VDMA_0_BASEADDR + 0xA0, height0);

    /* Configure the Read interface (MM2S)*/
    // MM2S Control Register
    Xil_Out32(XPAR_AXI_VDMA_0_BASEADDR + 0x00, 0x8B);
#if (XPAR_AXI_VDMA_0_NUM_FSTORES >= 1U)
    // MM2S Start Address 1
    Xil_Out32(XPAR_AXI_VDMA_0_BASEADDR + 0x5C, FRAME_BUFFER_0_0 + offset1);
    Xil_Out32(XPAR_AXI_VDMA_0_BASEADDR + 0x60, 0);
#endif // XPAR_AXI_VDMA_0_NUM_FSTORES == 1U
#if (XPAR_AXI_VDMA_0_NUM_FSTORES >= 2U)
    // MM2S Start Address 2
    Xil_Out32(XPAR_AXI_VDMA_0_BASEADDR + 0x64, FRAME_BUFFER_0_1 + offset1);
    Xil_Out32(XPAR_AXI_VDMA_0_BASEADDR + 0x68, 0);
#endif // XPAR_AXI_VDMA_0_NUM_FSTORES == 2U
#if (XPAR_AXI_VDMA_0_NUM_FSTORES >= 3U)
    // MM2S Start Address 3
    Xil_Out32(XPAR_AXI_VDMA_0_BASEADDR + 0x6C, FRAME_BUFFER_0_2 + offset1);
    Xil_Out32(XPAR_AXI_VDMA_0_BASEADDR + 0x70, 0);
#endif // XPAR_AXI_VDMA_0_NUM_FSTORES == 3U
    // MM2S Frame delay / Stride register
    Xil_Out32(XPAR_AXI_VDMA_0_BASEADDR + 0x58, stride1*bytePerPixels);
    // MM2S HSIZE register
    Xil_Out32(XPAR_AXI_VDMA_0_BASEADDR + 0x54, width1*bytePerPixels);
    // MM2S VSIZE register
    Xil_Out32(XPAR_AXI_VDMA_0_BASEADDR + 0x50, height1);

//    bsp_printf("VDMA 0 started!\r\n");
    /* End of VDMA Configuration */
}
#endif // XPAR_AXI_VDMA_0_ADDR_WIDTH == 64U
#endif // XPAR_XAXIVDMA_NUM_INSTANCES == 1U

#if (XPAR_XAXIVDMA_NUM_INSTANCES >= 2U)
#if (XPAR_AXI_VDMA_1_ADDR_WIDTH == 32U)
void vdma_config_m32_1(void)
{
    /* Start of VDMA Configuration */
    u32 bytePerPixels = VDMA_1_BPP;

    int offset0 = VDMA_1_W_OFFSET; // (y*w+x)*Bpp
    int offset1 = VDMA_1_R_OFFSET; // (y*w+x)*Bpp

    u32 stride0 = VDMA_1_W_STRIDE;
    u32 width0 = VDMA_1_W_WIDTH;
    u32 height0 = VDMA_1_W_HEIGHTH;
    u32 stride1 = VDMA_1_R_STRIDE;  // crop keeps write Stride
    u32 width1 = VDMA_1_R_WIDTH;
    u32 height1 = VDMA_1_R_HEIGHTH;

    /* Configure the Write interface (S2MM)*/
    // S2MM Control Register
    Xil_Out32(XPAR_AXI_VDMA_1_BASEADDR + 0x30, 0x8B);
#if (XPAR_AXI_VDMA_1_NUM_FSTORES >= 1U)
    //S2MM Start Address 1
    Xil_Out32(XPAR_AXI_VDMA_1_BASEADDR + 0xAC, FRAME_BUFFER_1_0 + offset0);
#endif // XPAR_AXI_VDMA_1_NUM_FSTORES == 1U
#if (XPAR_AXI_VDMA_1_NUM_FSTORES >= 2U)
    //S2MM Start Address 2
    Xil_Out32(XPAR_AXI_VDMA_1_BASEADDR + 0xB0, FRAME_BUFFER_1_1 + offset0);
#endif // XPAR_AXI_VDMA_1_NUM_FSTORES == 2U
#if (XPAR_AXI_VDMA_1_NUM_FSTORES >= 3U)
    //S2MM Start Address 3
    Xil_Out32(XPAR_AXI_VDMA_1_BASEADDR + 0xB4, FRAME_BUFFER_1_2 + offset0);
#endif // XPAR_AXI_VDMA_1_NUM_FSTORES == 3U
    //S2MM Frame delay / Stride register
    Xil_Out32(XPAR_AXI_VDMA_1_BASEADDR + 0xA8, stride0*bytePerPixels);
    // S2MM HSIZE register
    Xil_Out32(XPAR_AXI_VDMA_1_BASEADDR + 0xA4, width0*bytePerPixels);
    // S2MM VSIZE register
    Xil_Out32(XPAR_AXI_VDMA_1_BASEADDR + 0xA0, height0);

    /* Configure the Read interface (MM2S)*/
    // MM2S Control Register
    Xil_Out32(XPAR_AXI_VDMA_1_BASEADDR + 0x00, 0x8B);
#if (XPAR_AXI_VDMA_1_NUM_FSTORES >= 1U)
    // MM2S Start Address 1
    Xil_Out32(XPAR_AXI_VDMA_1_BASEADDR + 0x5C, FRAME_BUFFER_1_0 + offset1);
#endif // XPAR_AXI_VDMA_1_NUM_FSTORES == 1U
#if (XPAR_AXI_VDMA_1_NUM_FSTORES >= 2U)
    // MM2S Start Address 2
    Xil_Out32(XPAR_AXI_VDMA_1_BASEADDR + 0x60, FRAME_BUFFER_1_1 + offset1);
#endif // XPAR_AXI_VDMA_1_NUM_FSTORES == 2U
#if (XPAR_AXI_VDMA_1_NUM_FSTORES >= 3U)
    // MM2S Start Address 3
    Xil_Out32(XPAR_AXI_VDMA_1_BASEADDR + 0x64, FRAME_BUFFER_1_2 + offset1);
#endif // XPAR_AXI_VDMA_1_NUM_FSTORES == 3U
    // MM2S Frame delay / Stride register
    Xil_Out32(XPAR_AXI_VDMA_1_BASEADDR + 0x58, stride1*bytePerPixels);
    // MM2S HSIZE register
    Xil_Out32(XPAR_AXI_VDMA_1_BASEADDR + 0x54, width1*bytePerPixels);
    // MM2S VSIZE register
    Xil_Out32(XPAR_AXI_VDMA_1_BASEADDR + 0x50, height1);

//    bsp_printf("VDMA started!\r\n");
	/* End of VDMA Configuration */
}
#endif // XPAR_AXI_VDMA_1_ADDR_WIDTH == 32U
#if (XPAR_AXI_VDMA_1_ADDR_WIDTH == 64U)
void vdma_config_m64_1(void)
{
    /* Start of VDMA Configuration */
    u32 bytePerPixels = VDMA_1_BPP;

    int offset0 = VDMA_1_W_OFFSET; // (y*w+x)*Bpp
    int offset1 = VDMA_1_R_OFFSET; // (y*w+x)*Bpp

    u32 stride0 = VDMA_1_W_STRIDE;
    u32 width0 = VDMA_1_W_WIDTH;
    u32 height0 = VDMA_1_W_HEIGHTH;
    u32 stride1 = VDMA_1_R_STRIDE;  // crop keeps write Stride
    u32 width1 = VDMA_1_R_WIDTH;
    u32 height1 = VDMA_1_R_HEIGHTH;

    /* Configure the Write interface (S2MM)*/
    // S2MM Control Register
    Xil_Out32(XPAR_AXI_VDMA_1_BASEADDR + 0x30, 0x8B);
#if (XPAR_AXI_VDMA_1_NUM_FSTORES >= 1U)
    //S2MM Start Address 1
    Xil_Out32(XPAR_AXI_VDMA_1_BASEADDR + 0xAC, FRAME_BUFFER_1_0 + offset0);
    Xil_Out32(XPAR_AXI_VDMA_1_BASEADDR + 0xB0, 0);
#endif // XPAR_AXI_VDMA_1_NUM_FSTORES == 1U
#if (XPAR_AXI_VDMA_1_NUM_FSTORES >= 2U)
    //S2MM Start Address 2
    Xil_Out32(XPAR_AXI_VDMA_1_BASEADDR + 0xB4, FRAME_BUFFER_1_1 + offset0);
    Xil_Out32(XPAR_AXI_VDMA_1_BASEADDR + 0xB8, 0);
#endif // XPAR_AXI_VDMA_1_NUM_FSTORES == 2U
#if (XPAR_AXI_VDMA_1_NUM_FSTORES >= 3U)
    //S2MM Start Address 3
    Xil_Out32(XPAR_AXI_VDMA_1_BASEADDR + 0xBC, FRAME_BUFFER_1_2 + offset0);
    Xil_Out32(XPAR_AXI_VDMA_1_BASEADDR + 0xC0, 0);
#endif // XPAR_AXI_VDMA_1_NUM_FSTORES == 3U
    //S2MM Frame delay / Stride register
    Xil_Out32(XPAR_AXI_VDMA_1_BASEADDR + 0xA8, stride0*bytePerPixels);
    // S2MM HSIZE register
    Xil_Out32(XPAR_AXI_VDMA_1_BASEADDR + 0xA4, width0*bytePerPixels);
    // S2MM VSIZE register
    Xil_Out32(XPAR_AXI_VDMA_1_BASEADDR + 0xA0, height0);

    /* Configure the Read interface (MM2S)*/
    // MM2S Control Register
    Xil_Out32(XPAR_AXI_VDMA_1_BASEADDR + 0x00, 0x8B);
#if (XPAR_AXI_VDMA_1_NUM_FSTORES >= 1U)
    // MM2S Start Address 1
    Xil_Out32(XPAR_AXI_VDMA_1_BASEADDR + 0x5C, FRAME_BUFFER_1_0 + offset1);
    Xil_Out32(XPAR_AXI_VDMA_1_BASEADDR + 0x60, 0);
#endif // XPAR_AXI_VDMA_1_NUM_FSTORES == 1U
#if (XPAR_AXI_VDMA_1_NUM_FSTORES >= 2U)
    // MM2S Start Address 2
    Xil_Out32(XPAR_AXI_VDMA_1_BASEADDR + 0x64, FRAME_BUFFER_1_1 + offset1);
    Xil_Out32(XPAR_AXI_VDMA_1_BASEADDR + 0x68, 0);
#endif // XPAR_AXI_VDMA_1_NUM_FSTORES == 2U
#if (XPAR_AXI_VDMA_1_NUM_FSTORES >= 3U)
    // MM2S Start Address 3
    Xil_Out32(XPAR_AXI_VDMA_1_BASEADDR + 0x6C, FRAME_BUFFER_1_2 + offset1);
    Xil_Out32(XPAR_AXI_VDMA_1_BASEADDR + 0x70, 0);
#endif // XPAR_AXI_VDMA_1_NUM_FSTORES == 3U
    // MM2S Frame delay / Stride register
    Xil_Out32(XPAR_AXI_VDMA_1_BASEADDR + 0x58, stride1*bytePerPixels);
    // MM2S HSIZE register
    Xil_Out32(XPAR_AXI_VDMA_1_BASEADDR + 0x54, width1*bytePerPixels);
    // MM2S VSIZE register
    Xil_Out32(XPAR_AXI_VDMA_1_BASEADDR + 0x50, height1);

//    bsp_printf("VDMA 1 started!\r\n");
    /* End of VDMA Configuration */
}
#endif // XPAR_AXI_VDMA_1_ADDR_WIDTH == 64U
#endif // XPAR_XAXIVDMA_NUM_INSTANCES == 2U

#if (XPAR_XAXIVDMA_NUM_INSTANCES >= 3U)
#if (XPAR_AXI_VDMA_2_ADDR_WIDTH == 32U)
void vdma_config_m32_2(void)
{
    /* Start of VDMA Configuration */
    u32 bytePerPixels = VDMA_2_BPP;

    int offset0 = VDMA_2_W_OFFSET; // (y*w+x)*Bpp
    int offset1 = VDMA_2_R_OFFSET; // (y*w+x)*Bpp

    u32 stride0 = VDMA_2_W_STRIDE;
    u32 width0 = VDMA_2_W_WIDTH;
    u32 height0 = VDMA_2_W_HEIGHTH;
    u32 stride1 = VDMA_2_R_STRIDE;  // crop keeps write Stride
    u32 width1 = VDMA_2_R_WIDTH;
    u32 height1 = VDMA_2_R_HEIGHTH;

    /* Configure the Write interface (S2MM)*/
    // S2MM Control Register
    Xil_Out32(XPAR_AXI_VDMA_2_BASEADDR + 0x30, 0x8B);
#if (XPAR_AXI_VDMA_2_NUM_FSTORES >= 1U)
    //S2MM Start Address 1
    Xil_Out32(XPAR_AXI_VDMA_2_BASEADDR + 0xAC, FRAME_BUFFER_2_0 + offset0);
#endif // XPAR_AXI_VDMA_2_NUM_FSTORES == 1U
#if (XPAR_AXI_VDMA_2_NUM_FSTORES >= 2U)
    //S2MM Start Address 2
    Xil_Out32(XPAR_AXI_VDMA_2_BASEADDR + 0xB0, FRAME_BUFFER_2_1 + offset0);
#endif // XPAR_AXI_VDMA_2_NUM_FSTORES == 2U
#if (XPAR_AXI_VDMA_2_NUM_FSTORES >= 3U)
    //S2MM Start Address 3
    Xil_Out32(XPAR_AXI_VDMA_2_BASEADDR + 0xB4, FRAME_BUFFER_2_2 + offset0);
#endif // XPAR_AXI_VDMA_2_NUM_FSTORES == 3U
    //S2MM Frame delay / Stride register
    Xil_Out32(XPAR_AXI_VDMA_2_BASEADDR + 0xA8, stride0*bytePerPixels);
    // S2MM HSIZE register
    Xil_Out32(XPAR_AXI_VDMA_2_BASEADDR + 0xA4, width0*bytePerPixels);
    // S2MM VSIZE register
    Xil_Out32(XPAR_AXI_VDMA_2_BASEADDR + 0xA0, height0);

    /* Configure the Read interface (MM2S)*/
    // MM2S Control Register
    Xil_Out32(XPAR_AXI_VDMA_2_BASEADDR + 0x00, 0x8B);
#if (XPAR_AXI_VDMA_2_NUM_FSTORES >= 1U)
    // MM2S Start Address 1
    Xil_Out32(XPAR_AXI_VDMA_2_BASEADDR + 0x5C, FRAME_BUFFER_2_0 + offset1);
#endif // XPAR_AXI_VDMA_2_NUM_FSTORES == 1U
#if (XPAR_AXI_VDMA_2_NUM_FSTORES >= 2U)
    // MM2S Start Address 2
    Xil_Out32(XPAR_AXI_VDMA_2_BASEADDR + 0x60, FRAME_BUFFER_2_1 + offset1);
#endif // XPAR_AXI_VDMA_2_NUM_FSTORES == 2U
#if (XPAR_AXI_VDMA_2_NUM_FSTORES >= 3U)
    // MM2S Start Address 3
    Xil_Out32(XPAR_AXI_VDMA_2_BASEADDR + 0x64, FRAME_BUFFER_2_2 + offset1);
#endif // XPAR_AXI_VDMA_2_NUM_FSTORES == 3U
    // MM2S Frame delay / Stride register
    Xil_Out32(XPAR_AXI_VDMA_2_BASEADDR + 0x58, stride1*bytePerPixels);
    // MM2S HSIZE register
    Xil_Out32(XPAR_AXI_VDMA_2_BASEADDR + 0x54, width1*bytePerPixels);
    // MM2S VSIZE register
    Xil_Out32(XPAR_AXI_VDMA_2_BASEADDR + 0x50, height1);

//    bsp_printf("VDMA started!\r\n");
	/* End of VDMA Configuration */
}
#endif // XPAR_AXI_VDMA_2_ADDR_WIDTH == 32U
#if (XPAR_AXI_VDMA_2_ADDR_WIDTH == 64U)
void vdma_config_m64_2(void)
{
    /* Start of VDMA Configuration */
    u32 bytePerPixels = VDMA_2_BPP;

    int offset0 = VDMA_2_W_OFFSET; // (y*w+x)*Bpp
    int offset1 = VDMA_2_R_OFFSET; // (y*w+x)*Bpp

    u32 stride0 = VDMA_2_W_STRIDE;
    u32 width0 = VDMA_2_W_WIDTH;
    u32 height0 = VDMA_2_W_HEIGHTH;
    u32 stride1 = VDMA_2_R_STRIDE;  // crop keeps write Stride
    u32 width1 = VDMA_2_R_WIDTH;
    u32 height1 = VDMA_2_R_HEIGHTH;

    /* Configure the Write interface (S2MM)*/
    // S2MM Control Register
    Xil_Out32(XPAR_AXI_VDMA_2_BASEADDR + 0x30, 0x8B);
#if (XPAR_AXI_VDMA_2_NUM_FSTORES >= 1U)
    //S2MM Start Address 1
    Xil_Out32(XPAR_AXI_VDMA_2_BASEADDR + 0xAC, FRAME_BUFFER_2_0 + offset0);
    Xil_Out32(XPAR_AXI_VDMA_2_BASEADDR + 0xB0, 0);
#endif // XPAR_AXI_VDMA_2_NUM_FSTORES == 1U
#if (XPAR_AXI_VDMA_2_NUM_FSTORES >= 2U)
    //S2MM Start Address 2
    Xil_Out32(XPAR_AXI_VDMA_2_BASEADDR + 0xB4, FRAME_BUFFER_2_1 + offset0);
    Xil_Out32(XPAR_AXI_VDMA_2_BASEADDR + 0xB8, 0);
#endif // XPAR_AXI_VDMA_2_NUM_FSTORES == 2U
#if (XPAR_AXI_VDMA_2_NUM_FSTORES >= 3U)
    //S2MM Start Address 3
    Xil_Out32(XPAR_AXI_VDMA_2_BASEADDR + 0xBC, FRAME_BUFFER_2_2 + offset0);
    Xil_Out32(XPAR_AXI_VDMA_2_BASEADDR + 0xC0, 0);
#endif // XPAR_AXI_VDMA_2_NUM_FSTORES == 3U
    //S2MM Frame delay / Stride register
    Xil_Out32(XPAR_AXI_VDMA_2_BASEADDR + 0xA8, stride0*bytePerPixels);
    // S2MM HSIZE register
    Xil_Out32(XPAR_AXI_VDMA_2_BASEADDR + 0xA4, width0*bytePerPixels);
    // S2MM VSIZE register
    Xil_Out32(XPAR_AXI_VDMA_2_BASEADDR + 0xA0, height0);

    /* Configure the Read interface (MM2S)*/
    // MM2S Control Register
    Xil_Out32(XPAR_AXI_VDMA_2_BASEADDR + 0x00, 0x8B);
#if (XPAR_AXI_VDMA_2_NUM_FSTORES >= 1U)
    // MM2S Start Address 1
    Xil_Out32(XPAR_AXI_VDMA_2_BASEADDR + 0x5C, FRAME_BUFFER_2_0 + offset1);
    Xil_Out32(XPAR_AXI_VDMA_2_BASEADDR + 0x60, 0);
#endif // XPAR_AXI_VDMA_2_NUM_FSTORES == 1U
#if (XPAR_AXI_VDMA_2_NUM_FSTORES >= 2U)
    // MM2S Start Address 2
    Xil_Out32(XPAR_AXI_VDMA_2_BASEADDR + 0x64, FRAME_BUFFER_2_1 + offset1);
    Xil_Out32(XPAR_AXI_VDMA_2_BASEADDR + 0x68, 0);
#endif // XPAR_AXI_VDMA_2_NUM_FSTORES == 2U
#if (XPAR_AXI_VDMA_2_NUM_FSTORES >= 3U)
    // MM2S Start Address 3
    Xil_Out32(XPAR_AXI_VDMA_2_BASEADDR + 0x6C, FRAME_BUFFER_2_2 + offset1);
    Xil_Out32(XPAR_AXI_VDMA_2_BASEADDR + 0x70, 0);
#endif // XPAR_AXI_VDMA_2_NUM_FSTORES == 3U
    // MM2S Frame delay / Stride register
    Xil_Out32(XPAR_AXI_VDMA_2_BASEADDR + 0x58, stride1*bytePerPixels);
    // MM2S HSIZE register
    Xil_Out32(XPAR_AXI_VDMA_2_BASEADDR + 0x54, width1*bytePerPixels);
    // MM2S VSIZE register
    Xil_Out32(XPAR_AXI_VDMA_2_BASEADDR + 0x50, height1);

//    bsp_printf("VDMA started!\r\n");
    /* End of VDMA Configuration */
}
#endif // XPAR_AXI_VDMA_2_ADDR_WIDTH == 64U
#endif // XPAR_XAXIVDMA_NUM_INSTANCES == 3U

#if (XPAR_XAXIVDMA_NUM_INSTANCES >= 4U)
#if (XPAR_AXI_VDMA_3_ADDR_WIDTH == 32U)
void vdma_config_m32_3(void)
{
    /* Start of VDMA Configuration */
    u32 bytePerPixels = VDMA_3_BPP;

    int offset0 = VDMA_3_W_OFFSET; // (y*w+x)*Bpp
    int offset1 = VDMA_3_R_OFFSET; // (y*w+x)*Bpp

    u32 stride0 = VDMA_3_W_STRIDE;
    u32 width0 = VDMA_3_W_WIDTH;
    u32 height0 = VDMA_3_W_HEIGHTH;
    u32 stride1 = VDMA_3_R_STRIDE;  // crop keeps write Stride
    u32 width1 = VDMA_3_R_WIDTH;
    u32 height1 = VDMA_3_R_HEIGHTH;

    /* Configure the Write interface (S2MM)*/
    // S2MM Control Register
    Xil_Out32(XPAR_AXI_VDMA_3_BASEADDR + 0x30, 0x8B);
#if (XPAR_AXI_VDMA_3_NUM_FSTORES >= 1U)
    //S2MM Start Address 1
    Xil_Out32(XPAR_AXI_VDMA_3_BASEADDR + 0xAC, FRAME_BUFFER_3_0 + offset0);
#endif // XPAR_AXI_VDMA_3_NUM_FSTORES == 1U
#if (XPAR_AXI_VDMA_3_NUM_FSTORES >= 2U)
    //S2MM Start Address 2
    Xil_Out32(XPAR_AXI_VDMA_3_BASEADDR + 0xB0, FRAME_BUFFER_3_1 + offset0);
#endif // XPAR_AXI_VDMA_3_NUM_FSTORES == 2U
#if (XPAR_AXI_VDMA_3_NUM_FSTORES >= 3U)
    //S2MM Start Address 3
    Xil_Out32(XPAR_AXI_VDMA_3_BASEADDR + 0xB4, FRAME_BUFFER_3_2 + offset0);
#endif // XPAR_AXI_VDMA_3_NUM_FSTORES == 3U
    //S2MM Frame delay / Stride register
    Xil_Out32(XPAR_AXI_VDMA_3_BASEADDR + 0xA8, stride0*bytePerPixels);
    // S2MM HSIZE register
    Xil_Out32(XPAR_AXI_VDMA_3_BASEADDR + 0xA4, width0*bytePerPixels);
    // S2MM VSIZE register
    Xil_Out32(XPAR_AXI_VDMA_3_BASEADDR + 0xA0, height0);

    /* Configure the Read interface (MM2S)*/
    // MM2S Control Register
    Xil_Out32(XPAR_AXI_VDMA_3_BASEADDR + 0x00, 0x8B);
#if (XPAR_AXI_VDMA_3_NUM_FSTORES >= 1U)
    // MM2S Start Address 1
    Xil_Out32(XPAR_AXI_VDMA_3_BASEADDR + 0x5C, FRAME_BUFFER_3_0 + offset1);
#endif // XPAR_AXI_VDMA_3_NUM_FSTORES == 1U
#if (XPAR_AXI_VDMA_3_NUM_FSTORES >= 2U)
    // MM2S Start Address 2
    Xil_Out32(XPAR_AXI_VDMA_3_BASEADDR + 0x60, FRAME_BUFFER_3_1 + offset1);
#endif // XPAR_AXI_VDMA_3_NUM_FSTORES == 2U
#if (XPAR_AXI_VDMA_3_NUM_FSTORES >= 3U)
    // MM2S Start Address 3
    Xil_Out32(XPAR_AXI_VDMA_3_BASEADDR + 0x64, FRAME_BUFFER_3_2 + offset1);
#endif // XPAR_AXI_VDMA_3_NUM_FSTORES == 3U
    // MM2S Frame delay / Stride register
    Xil_Out32(XPAR_AXI_VDMA_3_BASEADDR + 0x58, stride1*bytePerPixels);
    // MM2S HSIZE register
    Xil_Out32(XPAR_AXI_VDMA_3_BASEADDR + 0x54, width1*bytePerPixels);
    // MM2S VSIZE register
    Xil_Out32(XPAR_AXI_VDMA_3_BASEADDR + 0x50, height1);

//    bsp_printf("VDMA started!\r\n");
	/* End of VDMA Configuration */
}
#endif // XPAR_AXI_VDMA_3_ADDR_WIDTH == 32U
#if (XPAR_AXI_VDMA_3_ADDR_WIDTH == 64U)
void vdma_config_m64_3(void)
{
    /* Start of VDMA Configuration */
    u32 bytePerPixels = VDMA_3_BPP;

    int offset0 = VDMA_3_W_OFFSET; // (y*w+x)*Bpp
    int offset1 = VDMA_3_R_OFFSET; // (y*w+x)*Bpp

    u32 stride0 = VDMA_3_W_STRIDE;
    u32 width0 = VDMA_3_W_WIDTH;
    u32 height0 = VDMA_3_W_HEIGHTH;
    u32 stride1 = VDMA_3_R_STRIDE;  // crop keeps write Stride
    u32 width1 = VDMA_3_R_WIDTH;
    u32 height1 = VDMA_3_R_HEIGHTH;

    /* Configure the Write interface (S2MM)*/
    // S2MM Control Register
    Xil_Out32(XPAR_AXI_VDMA_3_BASEADDR + 0x30, 0x8B);
#if (XPAR_AXI_VDMA_3_NUM_FSTORES >= 1U)
    //S2MM Start Address 1
    Xil_Out32(XPAR_AXI_VDMA_3_BASEADDR + 0xAC, FRAME_BUFFER_3_0 + offset0);
    Xil_Out32(XPAR_AXI_VDMA_3_BASEADDR + 0xB0, 0);
#endif // XPAR_AXI_VDMA_3_NUM_FSTORES == 1U
#if (XPAR_AXI_VDMA_3_NUM_FSTORES >= 2U)
    //S2MM Start Address 2
    Xil_Out32(XPAR_AXI_VDMA_3_BASEADDR + 0xB4, FRAME_BUFFER_3_1 + offset0);
    Xil_Out32(XPAR_AXI_VDMA_3_BASEADDR + 0xB8, 0);
#endif // XPAR_AXI_VDMA_3_NUM_FSTORES == 2U
#if (XPAR_AXI_VDMA_3_NUM_FSTORES >= 3U)
    //S2MM Start Address 3
    Xil_Out32(XPAR_AXI_VDMA_3_BASEADDR + 0xBC, FRAME_BUFFER_3_2 + offset0);
    Xil_Out32(XPAR_AXI_VDMA_3_BASEADDR + 0xC0, 0);
#endif // XPAR_AXI_VDMA_3_NUM_FSTORES == 3U
    //S2MM Frame delay / Stride register
    Xil_Out32(XPAR_AXI_VDMA_3_BASEADDR + 0xA8, stride0*bytePerPixels);
    // S2MM HSIZE register
    Xil_Out32(XPAR_AXI_VDMA_3_BASEADDR + 0xA4, width0*bytePerPixels);
    // S2MM VSIZE register
    Xil_Out32(XPAR_AXI_VDMA_3_BASEADDR + 0xA0, height0);

    /* Configure the Read interface (MM2S)*/
    // MM2S Control Register
    Xil_Out32(XPAR_AXI_VDMA_3_BASEADDR + 0x00, 0x8B);
#if (XPAR_AXI_VDMA_3_NUM_FSTORES >= 1U)
    // MM2S Start Address 1
    Xil_Out32(XPAR_AXI_VDMA_3_BASEADDR + 0x5C, FRAME_BUFFER_3_0 + offset1);
    Xil_Out32(XPAR_AXI_VDMA_3_BASEADDR + 0x60, 0);
#endif // XPAR_AXI_VDMA_3_NUM_FSTORES == 1U
#if (XPAR_AXI_VDMA_3_NUM_FSTORES >= 2U)
    // MM2S Start Address 2
    Xil_Out32(XPAR_AXI_VDMA_3_BASEADDR + 0x64, FRAME_BUFFER_3_1 + offset1);
    Xil_Out32(XPAR_AXI_VDMA_3_BASEADDR + 0x68, 0);
#endif // XPAR_AXI_VDMA_3_NUM_FSTORES == 2U
#if (XPAR_AXI_VDMA_3_NUM_FSTORES >= 3U)
    // MM2S Start Address 3
    Xil_Out32(XPAR_AXI_VDMA_3_BASEADDR + 0x6C, FRAME_BUFFER_3_2 + offset1);
    Xil_Out32(XPAR_AXI_VDMA_3_BASEADDR + 0x70, 0);
#endif // XPAR_AXI_VDMA_3_NUM_FSTORES == 3U
    // MM2S Frame delay / Stride register
    Xil_Out32(XPAR_AXI_VDMA_3_BASEADDR + 0x58, stride1*bytePerPixels);
    // MM2S HSIZE register
    Xil_Out32(XPAR_AXI_VDMA_3_BASEADDR + 0x54, width1*bytePerPixels);
    // MM2S VSIZE register
    Xil_Out32(XPAR_AXI_VDMA_3_BASEADDR + 0x50, height1);

//    bsp_printf("VDMA started!\r\n");
    /* End of VDMA Configuration */
}
#endif // XPAR_AXI_VDMA_3_ADDR_WIDTH == 64U
#endif // XPAR_XAXIVDMA_NUM_INSTANCES == 4U

#if (XPAR_XAXIVDMA_NUM_INSTANCES >= 5U)
#if (XPAR_AXI_VDMA_4_ADDR_WIDTH == 32U)
void vdma_config_m32_4(void)
{
    /* Start of VDMA Configuration */
    u32 bytePerPixels = VDMA_4_BPP;

    int offset0 = VDMA_4_W_OFFSET; // (y*w+x)*Bpp
    int offset1 = VDMA_4_R_OFFSET; // (y*w+x)*Bpp

    u32 stride0 = VDMA_4_W_STRIDE;
    u32 width0 = VDMA_4_W_WIDTH;
    u32 height0 = VDMA_4_W_HEIGHTH;
    u32 stride1 = VDMA_4_R_STRIDE;  // crop keeps write Stride
    u32 width1 = VDMA_4_R_WIDTH;
    u32 height1 = VDMA_4_R_HEIGHTH;

    /* Configure the Write interface (S2MM)*/
    // S2MM Control Register
    Xil_Out32(XPAR_AXI_VDMA_4_BASEADDR + 0x30, 0x8B);
#if (XPAR_AXI_VDMA_4_NUM_FSTORES >= 1U)
    //S2MM Start Address 1
    Xil_Out32(XPAR_AXI_VDMA_4_BASEADDR + 0xAC, FRAME_BUFFER_4_0 + offset0);
#endif // XPAR_AXI_VDMA_4_NUM_FSTORES == 1U
#if (XPAR_AXI_VDMA_4_NUM_FSTORES >= 2U)
    //S2MM Start Address 2
    Xil_Out32(XPAR_AXI_VDMA_4_BASEADDR + 0xB0, FRAME_BUFFER_4_1 + offset0);
#endif // XPAR_AXI_VDMA_4_NUM_FSTORES == 2U
#if (XPAR_AXI_VDMA_4_NUM_FSTORES >= 3U)
    //S2MM Start Address 3
    Xil_Out32(XPAR_AXI_VDMA_4_BASEADDR + 0xB4, FRAME_BUFFER_4_2 + offset0);
#endif // XPAR_AXI_VDMA_4_NUM_FSTORES == 3U
    //S2MM Frame delay / Stride register
    Xil_Out32(XPAR_AXI_VDMA_4_BASEADDR + 0xA8, stride0*bytePerPixels);
    // S2MM HSIZE register
    Xil_Out32(XPAR_AXI_VDMA_4_BASEADDR + 0xA4, width0*bytePerPixels);
    // S2MM VSIZE register
    Xil_Out32(XPAR_AXI_VDMA_4_BASEADDR + 0xA0, height0);

    /* Configure the Read interface (MM2S)*/
    // MM2S Control Register
    Xil_Out32(XPAR_AXI_VDMA_4_BASEADDR + 0x00, 0x8B);
#if (XPAR_AXI_VDMA_4_NUM_FSTORES >= 1U)
    // MM2S Start Address 1
    Xil_Out32(XPAR_AXI_VDMA_4_BASEADDR + 0x5C, FRAME_BUFFER_4_0 + offset1);
#endif // XPAR_AXI_VDMA_4_NUM_FSTORES == 1U
#if (XPAR_AXI_VDMA_4_NUM_FSTORES >= 2U)
    // MM2S Start Address 2
    Xil_Out32(XPAR_AXI_VDMA_4_BASEADDR + 0x60, FRAME_BUFFER_4_1 + offset1);
#endif // XPAR_AXI_VDMA_4_NUM_FSTORES == 2U
#if (XPAR_AXI_VDMA_4_NUM_FSTORES >= 3U)
    // MM2S Start Address 3
    Xil_Out32(XPAR_AXI_VDMA_4_BASEADDR + 0x64, FRAME_BUFFER_4_2 + offset1);
#endif // XPAR_AXI_VDMA_4_NUM_FSTORES == 3U
    // MM2S Frame delay / Stride register
    Xil_Out32(XPAR_AXI_VDMA_4_BASEADDR + 0x58, stride1*bytePerPixels);
    // MM2S HSIZE register
    Xil_Out32(XPAR_AXI_VDMA_4_BASEADDR + 0x54, width1*bytePerPixels);
    // MM2S VSIZE register
    Xil_Out32(XPAR_AXI_VDMA_4_BASEADDR + 0x50, height1);

//    bsp_printf("VDMA started!\r\n");
	/* End of VDMA Configuration */
}
#endif // XPAR_AXI_VDMA_4_ADDR_WIDTH == 32U
#if (XPAR_AXI_VDMA_4_ADDR_WIDTH == 64U)
void vdma_config_m64_4(void)
{
    /* Start of VDMA Configuration */
    u32 bytePerPixels = VDMA_4_BPP;

    int offset0 = VDMA_4_W_OFFSET; // (y*w+x)*Bpp
    int offset1 = VDMA_4_R_OFFSET; // (y*w+x)*Bpp

    u32 stride0 = VDMA_4_W_STRIDE;
    u32 width0 = VDMA_4_W_WIDTH;
    u32 height0 = VDMA_4_W_HEIGHTH;
    u32 stride1 = VDMA_4_R_STRIDE;  // crop keeps write Stride
    u32 width1 = VDMA_4_R_WIDTH;
    u32 height1 = VDMA_4_R_HEIGHTH;

    /* Configure the Write interface (S2MM)*/
    // S2MM Control Register
    Xil_Out32(XPAR_AXI_VDMA_4_BASEADDR + 0x30, 0x8B);
#if (XPAR_AXI_VDMA_4_NUM_FSTORES >= 1U)
    //S2MM Start Address 1
    Xil_Out32(XPAR_AXI_VDMA_4_BASEADDR + 0xAC, FRAME_BUFFER_4_0 + offset0);
    Xil_Out32(XPAR_AXI_VDMA_4_BASEADDR + 0xB0, 0);
#endif // XPAR_AXI_VDMA_4_NUM_FSTORES == 1U
#if (XPAR_AXI_VDMA_4_NUM_FSTORES >= 2U)
    //S2MM Start Address 2
    Xil_Out32(XPAR_AXI_VDMA_4_BASEADDR + 0xB4, FRAME_BUFFER_4_1 + offset0);
    Xil_Out32(XPAR_AXI_VDMA_4_BASEADDR + 0xB8, 0);
#endif // XPAR_AXI_VDMA_4_NUM_FSTORES == 2U
#if (XPAR_AXI_VDMA_4_NUM_FSTORES >= 3U)
    //S2MM Start Address 3
    Xil_Out32(XPAR_AXI_VDMA_4_BASEADDR + 0xBC, FRAME_BUFFER_4_2 + offset0);
    Xil_Out32(XPAR_AXI_VDMA_4_BASEADDR + 0xC0, 0);
#endif // XPAR_AXI_VDMA_4_NUM_FSTORES == 3U
    //S2MM Frame delay / Stride register
    Xil_Out32(XPAR_AXI_VDMA_4_BASEADDR + 0xA8, stride0*bytePerPixels);
    // S2MM HSIZE register
    Xil_Out32(XPAR_AXI_VDMA_4_BASEADDR + 0xA4, width0*bytePerPixels);
    // S2MM VSIZE register
    Xil_Out32(XPAR_AXI_VDMA_4_BASEADDR + 0xA0, height0);

    /* Configure the Read interface (MM2S)*/
    // MM2S Control Register
    Xil_Out32(XPAR_AXI_VDMA_4_BASEADDR + 0x00, 0x8B);
#if (XPAR_AXI_VDMA_4_NUM_FSTORES >= 1U)
    // MM2S Start Address 1
    Xil_Out32(XPAR_AXI_VDMA_4_BASEADDR + 0x5C, FRAME_BUFFER_4_0 + offset1);
    Xil_Out32(XPAR_AXI_VDMA_4_BASEADDR + 0x60, 0);
#endif // XPAR_AXI_VDMA_4_NUM_FSTORES == 1U
#if (XPAR_AXI_VDMA_4_NUM_FSTORES >= 2U)
    // MM2S Start Address 2
    Xil_Out32(XPAR_AXI_VDMA_4_BASEADDR + 0x64, FRAME_BUFFER_4_1 + offset1);
    Xil_Out32(XPAR_AXI_VDMA_4_BASEADDR + 0x68, 0);
#endif // XPAR_AXI_VDMA_4_NUM_FSTORES == 2U
#if (XPAR_AXI_VDMA_4_NUM_FSTORES >= 3U)
    // MM2S Start Address 3
    Xil_Out32(XPAR_AXI_VDMA_4_BASEADDR + 0x6C, FRAME_BUFFER_4_2 + offset1);
    Xil_Out32(XPAR_AXI_VDMA_4_BASEADDR + 0x70, 0);
#endif // XPAR_AXI_VDMA_4_NUM_FSTORES == 3U
    // MM2S Frame delay / Stride register
    Xil_Out32(XPAR_AXI_VDMA_4_BASEADDR + 0x58, stride1*bytePerPixels);
    // MM2S HSIZE register
    Xil_Out32(XPAR_AXI_VDMA_4_BASEADDR + 0x54, width1*bytePerPixels);
    // MM2S VSIZE register
    Xil_Out32(XPAR_AXI_VDMA_4_BASEADDR + 0x50, height1);

//    bsp_printf("VDMA started!\r\n");
    /* End of VDMA Configuration */
}
#endif // XPAR_AXI_VDMA_4_ADDR_WIDTH == 64U
#endif // XPAR_XAXIVDMA_NUM_INSTANCES == 5U

#if (XPAR_XAXIVDMA_NUM_INSTANCES >= 6U)
#if (XPAR_AXI_VDMA_5_ADDR_WIDTH == 32U)
void vdma_config_m32_5(void)
{
    /* Start of VDMA Configuration */
    u32 bytePerPixels = VDMA_5_BPP;

    int offset0 = VDMA_5_W_OFFSET; // (y*w+x)*Bpp
    int offset1 = VDMA_5_R_OFFSET; // (y*w+x)*Bpp

    u32 stride0 = VDMA_5_W_STRIDE;
    u32 width0 = VDMA_5_W_WIDTH;
    u32 height0 = VDMA_5_W_HEIGHTH;
    u32 stride1 = VDMA_5_R_STRIDE;  // crop keeps write Stride
    u32 width1 = VDMA_5_R_WIDTH;
    u32 height1 = VDMA_5_R_HEIGHTH;

    /* Configure the Write interface (S2MM)*/
    // S2MM Control Register
    Xil_Out32(XPAR_AXI_VDMA_5_BASEADDR + 0x30, 0x8B);
#if (XPAR_AXI_VDMA_5_NUM_FSTORES >= 1U)
    //S2MM Start Address 1
    Xil_Out32(XPAR_AXI_VDMA_5_BASEADDR + 0xAC, FRAME_BUFFER_5_0 + offset0);
#endif // XPAR_AXI_VDMA_5_NUM_FSTORES == 1U
#if (XPAR_AXI_VDMA_5_NUM_FSTORES >= 2U)
    //S2MM Start Address 2
    Xil_Out32(XPAR_AXI_VDMA_5_BASEADDR + 0xB0, FRAME_BUFFER_5_1 + offset0);
#endif // XPAR_AXI_VDMA_5_NUM_FSTORES == 2U
#if (XPAR_AXI_VDMA_5_NUM_FSTORES >= 3U)
    //S2MM Start Address 3
    Xil_Out32(XPAR_AXI_VDMA_5_BASEADDR + 0xB4, FRAME_BUFFER_5_2 + offset0);
#endif // XPAR_AXI_VDMA_5_NUM_FSTORES == 3U
    //S2MM Frame delay / Stride register
    Xil_Out32(XPAR_AXI_VDMA_5_BASEADDR + 0xA8, stride0*bytePerPixels);
    // S2MM HSIZE register
    Xil_Out32(XPAR_AXI_VDMA_5_BASEADDR + 0xA4, width0*bytePerPixels);
    // S2MM VSIZE register
    Xil_Out32(XPAR_AXI_VDMA_5_BASEADDR + 0xA0, height0);

    /* Configure the Read interface (MM2S)*/
    // MM2S Control Register
    Xil_Out32(XPAR_AXI_VDMA_5_BASEADDR + 0x00, 0x8B);
#if (XPAR_AXI_VDMA_5_NUM_FSTORES >= 1U)
    // MM2S Start Address 1
    Xil_Out32(XPAR_AXI_VDMA_5_BASEADDR + 0x5C, FRAME_BUFFER_5_0 + offset1);
#endif // XPAR_AXI_VDMA_5_NUM_FSTORES == 1U
#if (XPAR_AXI_VDMA_5_NUM_FSTORES >= 2U)
    // MM2S Start Address 2
    Xil_Out32(XPAR_AXI_VDMA_5_BASEADDR + 0x60, FRAME_BUFFER_5_1 + offset1);
#endif // XPAR_AXI_VDMA_5_NUM_FSTORES == 2U
#if (XPAR_AXI_VDMA_5_NUM_FSTORES >= 3U)
    // MM2S Start Address 3
    Xil_Out32(XPAR_AXI_VDMA_5_BASEADDR + 0x64, FRAME_BUFFER_5_2 + offset1);
#endif // XPAR_AXI_VDMA_5_NUM_FSTORES == 3U
    // MM2S Frame delay / Stride register
    Xil_Out32(XPAR_AXI_VDMA_5_BASEADDR + 0x58, stride1*bytePerPixels);
    // MM2S HSIZE register
    Xil_Out32(XPAR_AXI_VDMA_5_BASEADDR + 0x54, width1*bytePerPixels);
    // MM2S VSIZE register
    Xil_Out32(XPAR_AXI_VDMA_5_BASEADDR + 0x50, height1);

//    bsp_printf("VDMA started!\r\n");
	/* End of VDMA Configuration */
}
#endif // XPAR_AXI_VDMA_5_ADDR_WIDTH == 32U
#if (XPAR_AXI_VDMA_5_ADDR_WIDTH == 64U)
void vdma_config_m64_5(void)
{
    /* Start of VDMA Configuration */
    u32 bytePerPixels = VDMA_5_BPP;

    int offset0 = VDMA_5_W_OFFSET; // (y*w+x)*Bpp
    int offset1 = VDMA_5_R_OFFSET; // (y*w+x)*Bpp

    u32 stride0 = VDMA_5_W_STRIDE;
    u32 width0 = VDMA_5_W_WIDTH;
    u32 height0 = VDMA_5_W_HEIGHTH;
    u32 stride1 = VDMA_5_R_STRIDE;  // crop keeps write Stride
    u32 width1 = VDMA_5_R_WIDTH;
    u32 height1 = VDMA_5_R_HEIGHTH;

    /* Configure the Write interface (S2MM)*/
    // S2MM Control Register
    Xil_Out32(XPAR_AXI_VDMA_5_BASEADDR + 0x30, 0x8B);
#if (XPAR_AXI_VDMA_5_NUM_FSTORES >= 1U)
    //S2MM Start Address 1
    Xil_Out32(XPAR_AXI_VDMA_5_BASEADDR + 0xAC, FRAME_BUFFER_5_0 + offset0);
    Xil_Out32(XPAR_AXI_VDMA_5_BASEADDR + 0xB0, 0);
#endif // XPAR_AXI_VDMA_5_NUM_FSTORES == 1U
#if (XPAR_AXI_VDMA_5_NUM_FSTORES >= 2U)
    //S2MM Start Address 2
    Xil_Out32(XPAR_AXI_VDMA_5_BASEADDR + 0xB4, FRAME_BUFFER_5_1 + offset0);
    Xil_Out32(XPAR_AXI_VDMA_5_BASEADDR + 0xB8, 0);
#endif // XPAR_AXI_VDMA_5_NUM_FSTORES == 2U
#if (XPAR_AXI_VDMA_5_NUM_FSTORES >= 3U)
    //S2MM Start Address 3
    Xil_Out32(XPAR_AXI_VDMA_5_BASEADDR + 0xBC, FRAME_BUFFER_5_2 + offset0);
    Xil_Out32(XPAR_AXI_VDMA_5_BASEADDR + 0xC0, 0);
#endif // XPAR_AXI_VDMA_5_NUM_FSTORES == 3U
    //S2MM Frame delay / Stride register
    Xil_Out32(XPAR_AXI_VDMA_5_BASEADDR + 0xA8, stride0*bytePerPixels);
    // S2MM HSIZE register
    Xil_Out32(XPAR_AXI_VDMA_5_BASEADDR + 0xA4, width0*bytePerPixels);
    // S2MM VSIZE register
    Xil_Out32(XPAR_AXI_VDMA_5_BASEADDR + 0xA0, height0);

    /* Configure the Read interface (MM2S)*/
    // MM2S Control Register
    Xil_Out32(XPAR_AXI_VDMA_5_BASEADDR + 0x00, 0x8B);
#if (XPAR_AXI_VDMA_5_NUM_FSTORES >= 1U)
    // MM2S Start Address 1
    Xil_Out32(XPAR_AXI_VDMA_5_BASEADDR + 0x5C, FRAME_BUFFER_5_0 + offset1);
    Xil_Out32(XPAR_AXI_VDMA_5_BASEADDR + 0x60, 0);
#endif // XPAR_AXI_VDMA_5_NUM_FSTORES == 1U
#if (XPAR_AXI_VDMA_5_NUM_FSTORES >= 2U)
    // MM2S Start Address 2
    Xil_Out32(XPAR_AXI_VDMA_5_BASEADDR + 0x64, FRAME_BUFFER_5_1 + offset1);
    Xil_Out32(XPAR_AXI_VDMA_5_BASEADDR + 0x68, 0);
#endif // XPAR_AXI_VDMA_5_NUM_FSTORES == 2U
#if (XPAR_AXI_VDMA_5_NUM_FSTORES >= 3U)
    // MM2S Start Address 3
    Xil_Out32(XPAR_AXI_VDMA_5_BASEADDR + 0x6C, FRAME_BUFFER_5_2 + offset1);
    Xil_Out32(XPAR_AXI_VDMA_5_BASEADDR + 0x70, 0);
#endif // XPAR_AXI_VDMA_5_NUM_FSTORES == 3U
    // MM2S Frame delay / Stride register
    Xil_Out32(XPAR_AXI_VDMA_5_BASEADDR + 0x58, stride1*bytePerPixels);
    // MM2S HSIZE register
    Xil_Out32(XPAR_AXI_VDMA_5_BASEADDR + 0x54, width1*bytePerPixels);
    // MM2S VSIZE register
    Xil_Out32(XPAR_AXI_VDMA_5_BASEADDR + 0x50, height1);

//    bsp_printf("VDMA started!\r\n");
    /* End of VDMA Configuration */
}
#endif // XPAR_AXI_VDMA_5_ADDR_WIDTH == 64U
#endif // XPAR_XAXIVDMA_NUM_INSTANCES == 6U

#if (XPAR_XAXIVDMA_NUM_INSTANCES >= 7U)
#if (XPAR_AXI_VDMA_6_ADDR_WIDTH == 32U)
void vdma_config_m32_6(void)
{
    /* Start of VDMA Configuration */
    u32 bytePerPixels = VDMA_6_BPP;

    int offset0 = VDMA_6_W_OFFSET; // (y*w+x)*Bpp
    int offset1 = VDMA_6_R_OFFSET; // (y*w+x)*Bpp

    u32 stride0 = VDMA_6_W_STRIDE;
    u32 width0 = VDMA_6_W_WIDTH;
    u32 height0 = VDMA_6_W_HEIGHTH;
    u32 stride1 = VDMA_6_R_STRIDE;  // crop keeps write Stride
    u32 width1 = VDMA_6_R_WIDTH;
    u32 height1 = VDMA_6_R_HEIGHTH;

    /* Configure the Write interface (S2MM)*/
    // S2MM Control Register
    Xil_Out32(XPAR_AXI_VDMA_6_BASEADDR + 0x30, 0x8B);
#if (XPAR_AXI_VDMA_6_NUM_FSTORES >= 1U)
    //S2MM Start Address 1
    Xil_Out32(XPAR_AXI_VDMA_6_BASEADDR + 0xAC, FRAME_BUFFER_6_0 + offset0);
#endif // XPAR_AXI_VDMA_6_NUM_FSTORES == 1U
#if (XPAR_AXI_VDMA_6_NUM_FSTORES >= 2U)
    //S2MM Start Address 2
    Xil_Out32(XPAR_AXI_VDMA_6_BASEADDR + 0xB0, FRAME_BUFFER_6_1 + offset0);
#endif // XPAR_AXI_VDMA_6_NUM_FSTORES == 2U
#if (XPAR_AXI_VDMA_6_NUM_FSTORES >= 3U)
    //S2MM Start Address 3
    Xil_Out32(XPAR_AXI_VDMA_6_BASEADDR + 0xB4, FRAME_BUFFER_6_2 + offset0);
#endif // XPAR_AXI_VDMA_6_NUM_FSTORES == 3U
    //S2MM Frame delay / Stride register
    Xil_Out32(XPAR_AXI_VDMA_6_BASEADDR + 0xA8, stride0*bytePerPixels);
    // S2MM HSIZE register
    Xil_Out32(XPAR_AXI_VDMA_6_BASEADDR + 0xA4, width0*bytePerPixels);
    // S2MM VSIZE register
    Xil_Out32(XPAR_AXI_VDMA_6_BASEADDR + 0xA0, height0);

    /* Configure the Read interface (MM2S)*/
    // MM2S Control Register
    Xil_Out32(XPAR_AXI_VDMA_6_BASEADDR + 0x00, 0x8B);
#if (XPAR_AXI_VDMA_6_NUM_FSTORES >= 1U)
    // MM2S Start Address 1
    Xil_Out32(XPAR_AXI_VDMA_6_BASEADDR + 0x5C, FRAME_BUFFER_6_0 + offset1);
#endif // XPAR_AXI_VDMA_6_NUM_FSTORES == 1U
#if (XPAR_AXI_VDMA_6_NUM_FSTORES >= 2U)
    // MM2S Start Address 2
    Xil_Out32(XPAR_AXI_VDMA_6_BASEADDR + 0x60, FRAME_BUFFER_6_1 + offset1);
#endif // XPAR_AXI_VDMA_6_NUM_FSTORES == 2U
#if (XPAR_AXI_VDMA_6_NUM_FSTORES >= 3U)
    // MM2S Start Address 3
    Xil_Out32(XPAR_AXI_VDMA_6_BASEADDR + 0x64, FRAME_BUFFER_6_2 + offset1);
#endif // XPAR_AXI_VDMA_6_NUM_FSTORES == 3U
    // MM2S Frame delay / Stride register
    Xil_Out32(XPAR_AXI_VDMA_6_BASEADDR + 0x58, stride1*bytePerPixels);
    // MM2S HSIZE register
    Xil_Out32(XPAR_AXI_VDMA_6_BASEADDR + 0x54, width1*bytePerPixels);
    // MM2S VSIZE register
    Xil_Out32(XPAR_AXI_VDMA_6_BASEADDR + 0x50, height1);

//    bsp_printf("VDMA started!\r\n");
	/* End of VDMA Configuration */
}
#endif // XPAR_AXI_VDMA_6_ADDR_WIDTH == 32U
#if (XPAR_AXI_VDMA_6_ADDR_WIDTH == 64U)
void vdma_config_m64_6(void)
{
    /* Start of VDMA Configuration */
    u32 bytePerPixels = VDMA_6_BPP;

    int offset0 = VDMA_6_W_OFFSET; // (y*w+x)*Bpp
    int offset1 = VDMA_6_R_OFFSET; // (y*w+x)*Bpp

    u32 stride0 = VDMA_6_W_STRIDE;
    u32 width0 = VDMA_6_W_WIDTH;
    u32 height0 = VDMA_6_W_HEIGHTH;
    u32 stride1 = VDMA_6_R_STRIDE;  // crop keeps write Stride
    u32 width1 = VDMA_6_R_WIDTH;
    u32 height1 = VDMA_6_R_HEIGHTH;

    /* Configure the Write interface (S2MM)*/
    // S2MM Control Register
    Xil_Out32(XPAR_AXI_VDMA_6_BASEADDR + 0x30, 0x8B);
#if (XPAR_AXI_VDMA_6_NUM_FSTORES >= 1U)
    //S2MM Start Address 1
    Xil_Out32(XPAR_AXI_VDMA_6_BASEADDR + 0xAC, FRAME_BUFFER_6_0 + offset0);
    Xil_Out32(XPAR_AXI_VDMA_6_BASEADDR + 0xB0, 0);
#endif // XPAR_AXI_VDMA_6_NUM_FSTORES == 1U
#if (XPAR_AXI_VDMA_6_NUM_FSTORES >= 2U)
    //S2MM Start Address 2
    Xil_Out32(XPAR_AXI_VDMA_6_BASEADDR + 0xB4, FRAME_BUFFER_6_1 + offset0);
    Xil_Out32(XPAR_AXI_VDMA_6_BASEADDR + 0xB8, 0);
#endif // XPAR_AXI_VDMA_6_NUM_FSTORES == 2U
#if (XPAR_AXI_VDMA_6_NUM_FSTORES >= 3U)
    //S2MM Start Address 3
    Xil_Out32(XPAR_AXI_VDMA_6_BASEADDR + 0xBC, FRAME_BUFFER_6_2 + offset0);
    Xil_Out32(XPAR_AXI_VDMA_6_BASEADDR + 0xC0, 0);
#endif // XPAR_AXI_VDMA_6_NUM_FSTORES == 3U
    //S2MM Frame delay / Stride register
    Xil_Out32(XPAR_AXI_VDMA_6_BASEADDR + 0xA8, stride0*bytePerPixels);
    // S2MM HSIZE register
    Xil_Out32(XPAR_AXI_VDMA_6_BASEADDR + 0xA4, width0*bytePerPixels);
    // S2MM VSIZE register
    Xil_Out32(XPAR_AXI_VDMA_6_BASEADDR + 0xA0, height0);

    /* Configure the Read interface (MM2S)*/
    // MM2S Control Register
    Xil_Out32(XPAR_AXI_VDMA_6_BASEADDR + 0x00, 0x8B);
#if (XPAR_AXI_VDMA_6_NUM_FSTORES >= 1U)
    // MM2S Start Address 1
    Xil_Out32(XPAR_AXI_VDMA_6_BASEADDR + 0x5C, FRAME_BUFFER_6_0 + offset1);
    Xil_Out32(XPAR_AXI_VDMA_6_BASEADDR + 0x60, 0);
#endif // XPAR_AXI_VDMA_6_NUM_FSTORES == 1U
#if (XPAR_AXI_VDMA_6_NUM_FSTORES >= 2U)
    // MM2S Start Address 2
    Xil_Out32(XPAR_AXI_VDMA_6_BASEADDR + 0x64, FRAME_BUFFER_6_1 + offset1);
    Xil_Out32(XPAR_AXI_VDMA_6_BASEADDR + 0x68, 0);
#endif // XPAR_AXI_VDMA_6_NUM_FSTORES == 2U
#if (XPAR_AXI_VDMA_6_NUM_FSTORES >= 3U)
    // MM2S Start Address 3
    Xil_Out32(XPAR_AXI_VDMA_6_BASEADDR + 0x6C, FRAME_BUFFER_6_2 + offset1);
    Xil_Out32(XPAR_AXI_VDMA_6_BASEADDR + 0x70, 0);
#endif // XPAR_AXI_VDMA_6_NUM_FSTORES == 3U
    // MM2S Frame delay / Stride register
    Xil_Out32(XPAR_AXI_VDMA_6_BASEADDR + 0x58, stride1*bytePerPixels);
    // MM2S HSIZE register
    Xil_Out32(XPAR_AXI_VDMA_6_BASEADDR + 0x54, width1*bytePerPixels);
    // MM2S VSIZE register
    Xil_Out32(XPAR_AXI_VDMA_6_BASEADDR + 0x50, height1);

//    bsp_printf("VDMA started!\r\n");
    /* End of VDMA Configuration */
}
#endif // XPAR_AXI_VDMA_6_ADDR_WIDTH == 64U
#endif // XPAR_XAXIVDMA_NUM_INSTANCES == 7U

#if (XPAR_XAXIVDMA_NUM_INSTANCES >= 8U)
#if (XPAR_AXI_VDMA_7_ADDR_WIDTH == 32U)
void vdma_config_m32_7(void)
{
    /* Start of VDMA Configuration */
    u32 bytePerPixels = VDMA_7_BPP;

    int offset0 = VDMA_7_W_OFFSET; // (y*w+x)*Bpp
    int offset1 = VDMA_7_R_OFFSET; // (y*w+x)*Bpp

    u32 stride0 = VDMA_7_W_STRIDE;
    u32 width0 = VDMA_7_W_WIDTH;
    u32 height0 = VDMA_7_W_HEIGHTH;
    u32 stride1 = VDMA_7_R_STRIDE;  // crop keeps write Stride
    u32 width1 = VDMA_7_R_WIDTH;
    u32 height1 = VDMA_7_R_HEIGHTH;

    /* Configure the Write interface (S2MM)*/
    // S2MM Control Register
    Xil_Out32(XPAR_AXI_VDMA_7_BASEADDR + 0x30, 0x8B);
#if (XPAR_AXI_VDMA_7_NUM_FSTORES >= 1U)
    //S2MM Start Address 1
    Xil_Out32(XPAR_AXI_VDMA_7_BASEADDR + 0xAC, FRAME_BUFFER_7_0 + offset0);
#endif // XPAR_AXI_VDMA_7_NUM_FSTORES == 1U
#if (XPAR_AXI_VDMA_7_NUM_FSTORES >= 2U)
    //S2MM Start Address 2
    Xil_Out32(XPAR_AXI_VDMA_7_BASEADDR + 0xB0, FRAME_BUFFER_7_1 + offset0);
#endif // XPAR_AXI_VDMA_7_NUM_FSTORES == 2U
#if (XPAR_AXI_VDMA_7_NUM_FSTORES >= 3U)
    //S2MM Start Address 3
    Xil_Out32(XPAR_AXI_VDMA_7_BASEADDR + 0xB4, FRAME_BUFFER_7_2 + offset0);
#endif // XPAR_AXI_VDMA_7_NUM_FSTORES == 3U
    //S2MM Frame delay / Stride register
    Xil_Out32(XPAR_AXI_VDMA_7_BASEADDR + 0xA8, stride0*bytePerPixels);
    // S2MM HSIZE register
    Xil_Out32(XPAR_AXI_VDMA_7_BASEADDR + 0xA4, width0*bytePerPixels);
    // S2MM VSIZE register
    Xil_Out32(XPAR_AXI_VDMA_7_BASEADDR + 0xA0, height0);

    /* Configure the Read interface (MM2S)*/
    // MM2S Control Register
    Xil_Out32(XPAR_AXI_VDMA_7_BASEADDR + 0x00, 0x8B);
#if (XPAR_AXI_VDMA_7_NUM_FSTORES >= 1U)
    // MM2S Start Address 1
    Xil_Out32(XPAR_AXI_VDMA_7_BASEADDR + 0x5C, FRAME_BUFFER_7_0 + offset1);
#endif // XPAR_AXI_VDMA_7_NUM_FSTORES == 1U
#if (XPAR_AXI_VDMA_7_NUM_FSTORES >= 2U)
    // MM2S Start Address 2
    Xil_Out32(XPAR_AXI_VDMA_7_BASEADDR + 0x60, FRAME_BUFFER_7_1 + offset1);
#endif // XPAR_AXI_VDMA_7_NUM_FSTORES == 2U
#if (XPAR_AXI_VDMA_7_NUM_FSTORES >= 3U)
    // MM2S Start Address 3
    Xil_Out32(XPAR_AXI_VDMA_7_BASEADDR + 0x64, FRAME_BUFFER_7_2 + offset1);
#endif // XPAR_AXI_VDMA_7_NUM_FSTORES == 3U
    // MM2S Frame delay / Stride register
    Xil_Out32(XPAR_AXI_VDMA_7_BASEADDR + 0x58, stride1*bytePerPixels);
    // MM2S HSIZE register
    Xil_Out32(XPAR_AXI_VDMA_7_BASEADDR + 0x54, width1*bytePerPixels);
    // MM2S VSIZE register
    Xil_Out32(XPAR_AXI_VDMA_7_BASEADDR + 0x50, height1);

//    bsp_printf("VDMA started!\r\n");
	/* End of VDMA Configuration */
}
#endif // XPAR_AXI_VDMA_7_ADDR_WIDTH == 32U
#if (XPAR_AXI_VDMA_7_ADDR_WIDTH == 64U)
void vdma_config_m64_7(void)
{
    /* Start of VDMA Configuration */
    u32 bytePerPixels = VDMA_7_BPP;

    int offset0 = VDMA_7_W_OFFSET; // (y*w+x)*Bpp
    int offset1 = VDMA_7_R_OFFSET; // (y*w+x)*Bpp

    u32 stride0 = VDMA_7_W_STRIDE;
    u32 width0 = VDMA_7_W_WIDTH;
    u32 height0 = VDMA_7_W_HEIGHTH;
    u32 stride1 = VDMA_7_R_STRIDE;  // crop keeps write Stride
    u32 width1 = VDMA_7_R_WIDTH;
    u32 height1 = VDMA_7_R_HEIGHTH;

    /* Configure the Write interface (S2MM)*/
    // S2MM Control Register
    Xil_Out32(XPAR_AXI_VDMA_7_BASEADDR + 0x30, 0x8B);
#if (XPAR_AXI_VDMA_7_NUM_FSTORES >= 1U)
    //S2MM Start Address 1
    Xil_Out32(XPAR_AXI_VDMA_7_BASEADDR + 0xAC, FRAME_BUFFER_7_0 + offset0);
    Xil_Out32(XPAR_AXI_VDMA_7_BASEADDR + 0xB0, 0);
#endif // XPAR_AXI_VDMA_7_NUM_FSTORES == 1U
#if (XPAR_AXI_VDMA_7_NUM_FSTORES >= 2U)
    //S2MM Start Address 2
    Xil_Out32(XPAR_AXI_VDMA_7_BASEADDR + 0xB4, FRAME_BUFFER_7_1 + offset0);
    Xil_Out32(XPAR_AXI_VDMA_7_BASEADDR + 0xB8, 0);
#endif // XPAR_AXI_VDMA_7_NUM_FSTORES == 2U
#if (XPAR_AXI_VDMA_7_NUM_FSTORES >= 3U)
    //S2MM Start Address 3
    Xil_Out32(XPAR_AXI_VDMA_7_BASEADDR + 0xBC, FRAME_BUFFER_7_2 + offset0);
    Xil_Out32(XPAR_AXI_VDMA_7_BASEADDR + 0xC0, 0);
#endif // XPAR_AXI_VDMA_7_NUM_FSTORES == 3U
    //S2MM Frame delay / Stride register
    Xil_Out32(XPAR_AXI_VDMA_7_BASEADDR + 0xA8, stride0*bytePerPixels);
    // S2MM HSIZE register
    Xil_Out32(XPAR_AXI_VDMA_7_BASEADDR + 0xA4, width0*bytePerPixels);
    // S2MM VSIZE register
    Xil_Out32(XPAR_AXI_VDMA_7_BASEADDR + 0xA0, height0);

    /* Configure the Read interface (MM2S)*/
    // MM2S Control Register
    Xil_Out32(XPAR_AXI_VDMA_7_BASEADDR + 0x00, 0x8B);
#if (XPAR_AXI_VDMA_7_NUM_FSTORES >= 1U)
    // MM2S Start Address 1
    Xil_Out32(XPAR_AXI_VDMA_7_BASEADDR + 0x5C, FRAME_BUFFER_7_0 + offset1);
    Xil_Out32(XPAR_AXI_VDMA_7_BASEADDR + 0x60, 0);
#endif // XPAR_AXI_VDMA_7_NUM_FSTORES == 1U
#if (XPAR_AXI_VDMA_7_NUM_FSTORES >= 2U)
    // MM2S Start Address 2
    Xil_Out32(XPAR_AXI_VDMA_7_BASEADDR + 0x64, FRAME_BUFFER_7_1 + offset1);
    Xil_Out32(XPAR_AXI_VDMA_7_BASEADDR + 0x68, 0);
#endif // XPAR_AXI_VDMA_7_NUM_FSTORES == 2U
#if (XPAR_AXI_VDMA_7_NUM_FSTORES >= 3U)
    // MM2S Start Address 3
    Xil_Out32(XPAR_AXI_VDMA_7_BASEADDR + 0x6C, FRAME_BUFFER_7_2 + offset1);
    Xil_Out32(XPAR_AXI_VDMA_7_BASEADDR + 0x70, 0);
#endif // XPAR_AXI_VDMA_7_NUM_FSTORES == 3U
    // MM2S Frame delay / Stride register
    Xil_Out32(XPAR_AXI_VDMA_7_BASEADDR + 0x58, stride1*bytePerPixels);
    // MM2S HSIZE register
    Xil_Out32(XPAR_AXI_VDMA_7_BASEADDR + 0x54, width1*bytePerPixels);
    // MM2S VSIZE register
    Xil_Out32(XPAR_AXI_VDMA_7_BASEADDR + 0x50, height1);

//    bsp_printf("VDMA started!\r\n");
    /* End of VDMA Configuration */
}
#endif // XPAR_AXI_VDMA_7_ADDR_WIDTH == 64U
#endif // XPAR_XAXIVDMA_NUM_INSTANCES == 8U

void vdma_reg_cfg
(
	XAxiVdma *InstancePtr,
	u32 bytePerPixels,
	s32 w_offset,	// offset (y*w+x)*Bpp
	s32 r_offset,
	u32 w_stride,
	u32 w_width,
	u32 w_height,
	u32 r_stride,
	u32 r_width,
	u32 r_height,
	UINTPTR *BufferAddr
)
{
	Xil_AssertNonvoid(InstancePtr != NULL);

	if(InstancePtr->HasS2Mm)
	{
		if(InstancePtr->AddrWidth == 32U)
		{
			/* Configure the Write interface (S2MM)*/
			// S2MM Control Register
			Xil_Out32(InstancePtr->BaseAddr + 0x30, 0x8B);
			//S2MM Start Address
			for(u16 i=0; i< InstancePtr->MaxNumFrames; i++)
			{
				Xil_Out32(InstancePtr->BaseAddr + 0xAC + 4*i, BufferAddr[i]);
			}
			//S2MM Frame delay / Stride register
			Xil_Out32(InstancePtr->BaseAddr + 0xA8, w_stride*bytePerPixels);
			// S2MM HSIZE register
			Xil_Out32(InstancePtr->BaseAddr + 0xA4, w_width*bytePerPixels);
			// S2MM VSIZE register
			Xil_Out32(InstancePtr->BaseAddr + 0xA0, w_height);
		}
		else // 64U
		{
			/* Configure the Write interface (S2MM)*/
			// S2MM Control Register
			Xil_Out32(InstancePtr->BaseAddr + 0x30, 0x8B);
			//S2MM Start Address
			for(u16 i=0; i< InstancePtr->MaxNumFrames; i++)
			{
				Xil_Out32(InstancePtr->BaseAddr + 0xAC + 8*i, BufferAddr[i]);
				Xil_Out32(InstancePtr->BaseAddr + 0xB0 + 8*i, 0);
			}
			//S2MM Frame delay / Stride register
			Xil_Out32(InstancePtr->BaseAddr + 0xA8, w_stride*bytePerPixels);
			// S2MM HSIZE register
			Xil_Out32(InstancePtr->BaseAddr + 0xA4, w_width*bytePerPixels);
			// S2MM VSIZE register
			Xil_Out32(InstancePtr->BaseAddr + 0xA0, w_height);
		}
	}

	if(InstancePtr->HasMm2S)
	{
		if(InstancePtr->AddrWidth == 32U)
		{
		    /* Configure the Read interface (MM2S)*/
		    // MM2S Control Register
		    Xil_Out32(InstancePtr->BaseAddr + 0x00, 0x8B);
		    // MM2S Start Address
		    for(u16 i=0; i< InstancePtr->MaxNumFrames; i++)
			{
				Xil_Out32(InstancePtr->BaseAddr + 0x5C + 4*i, BufferAddr[i]);
			}
		    // MM2S Frame delay / Stride register
		    Xil_Out32(InstancePtr->BaseAddr + 0x58, r_stride*bytePerPixels);
		    // MM2S HSIZE register
		    Xil_Out32(InstancePtr->BaseAddr + 0x54, r_width*bytePerPixels);
		    // MM2S VSIZE register
		    Xil_Out32(InstancePtr->BaseAddr + 0x50, r_height);
		}
		else // 64U
		{
		    /* Configure the Read interface (MM2S)*/
		    // MM2S Control Register
		    Xil_Out32(InstancePtr->BaseAddr + 0x00, 0x8B);
		    // MM2S Start Address
		    for(u16 i=0; i< InstancePtr->MaxNumFrames; i++)
			{
				Xil_Out32(InstancePtr->BaseAddr + 0x5C + 8*i, BufferAddr[i]);
				Xil_Out32(InstancePtr->BaseAddr + 0x60 + 8*i, 0);
			}
		    // MM2S Frame delay / Stride register
		    Xil_Out32(InstancePtr->BaseAddr + 0x58, r_stride*bytePerPixels);
		    // MM2S HSIZE register
		    Xil_Out32(InstancePtr->BaseAddr + 0x54, r_width*bytePerPixels);
		    // MM2S VSIZE register
		    Xil_Out32(InstancePtr->BaseAddr + 0x50, r_height);
		}
	}
}

int vdma_config(void)
{
	int Status;

#if (XPAR_XAXIVDMA_NUM_INSTANCES >= 1U)
//	UINTPTR vmda_0_frame_addrs[XAXIVDMA_MAX_FRAMESTORE] = {
	UINTPTR vmda_0_frame_addrs[] = {
#if (XPAR_AXI_VDMA_0_NUM_FSTORES >= 1U)
		FRAME_BUFFER_0_0,
#endif // XPAR_AXI_VDMA_0_NUM_FSTORES == 1U
#if (XPAR_AXI_VDMA_0_NUM_FSTORES >= 2U)
		FRAME_BUFFER_0_1,
#endif // XPAR_AXI_VDMA_0_NUM_FSTORES == 2U
#if (XPAR_AXI_VDMA_0_NUM_FSTORES >= 3U)
		FRAME_BUFFER_0_2,
#endif // XPAR_AXI_VDMA_0_NUM_FSTORES == 3U
	};
	if (Vdma0.IsReady != XAXIVDMA_DEVICE_READY)
	{
		Status = vdma_init(&Vdma0, XPAR_AXI_VDMA_0_DEVICE_ID);
		if (Status != XST_SUCCESS)
		{
			bsp_printf(TXT_RED "In %s: XAxiVdma_CfgInitialize failed...\r\n" TXT_RST, __func__);
			return XST_FAILURE;
		}
	}
	vdma_reg_cfg
	(
		&Vdma0,
		VDMA_0_BPP,
		VDMA_0_W_OFFSET,	// offset (y*w+x)*Bpp
		VDMA_0_R_OFFSET,
		VDMA_0_W_STRIDE,
		VDMA_0_W_WIDTH,
		VDMA_0_W_HEIGHTH,
		VDMA_0_R_STRIDE,
		VDMA_0_R_WIDTH,
		VDMA_0_R_HEIGHTH,
		vmda_0_frame_addrs
	);
//	bsp_printf("vdma 0 config done!\r\n");
#endif // XPAR_XAXIVDMA_NUM_INSTANCES == 1U

#if (XPAR_XAXIVDMA_NUM_INSTANCES >= 2U)
	UINTPTR vmda_1_frame_addrs[] = {
#if (XPAR_AXI_VDMA_1_NUM_FSTORES >= 1U)
		FRAME_BUFFER_1_0,
#endif // XPAR_AXI_VDMA_1_NUM_FSTORES == 1U
#if (XPAR_AXI_VDMA_1_NUM_FSTORES >= 2U)
		FRAME_BUFFER_1_1,
#endif // XPAR_AXI_VDMA_1_NUM_FSTORES == 2U
#if (XPAR_AXI_VDMA_1_NUM_FSTORES >= 3U)
		FRAME_BUFFER_1_2,
#endif // XPAR_AXI_VDMA_1_NUM_FSTORES == 3U
	};
	if (Vdma1.IsReady != XAXIVDMA_DEVICE_READY)
	{
		Status = vdma_init(&Vdma1, XPAR_AXI_VDMA_1_DEVICE_ID);
		if (Status != XST_SUCCESS)
		{
			bsp_printf(TXT_RED "In %s: XAxiVdma_CfgInitialize failed...\r\n" TXT_RST, __func__);
			return XST_FAILURE;
		}
	}
	vdma_reg_cfg
	(
		&Vdma1,
		3,
		0,	// offset (y*w+x)*Bpp
		0,
		VDMA_1_W_STRIDE,
		VDMA_1_W_WIDTH,
		VDMA_1_W_HEIGHTH,
		VDMA_1_R_STRIDE,
		VDMA_1_R_WIDTH,
		VDMA_1_R_HEIGHTH,
		vmda_1_frame_addrs
	);
#endif // XPAR_XAXIVDMA_NUM_INSTANCES == 2U

#if (XPAR_XAXIVDMA_NUM_INSTANCES >= 3U)
	UINTPTR vmda_2_frame_addrs[] = {
#if (XPAR_AXI_VDMA_2_NUM_FSTORES >= 1U)
		FRAME_BUFFER_2_0,
#endif // XPAR_AXI_VDMA_2_NUM_FSTORES == 1U
#if (XPAR_AXI_VDMA_2_NUM_FSTORES >= 2U)
		FRAME_BUFFER_2_1,
#endif // XPAR_AXI_VDMA_2_NUM_FSTORES == 2U
#if (XPAR_AXI_VDMA_2_NUM_FSTORES >= 3U)
		FRAME_BUFFER_2_2,
#endif // XPAR_AXI_VDMA_2_NUM_FSTORES == 3U
	};
	if (Vdma2.IsReady != XAXIVDMA_DEVICE_READY)
	{
		Status = vdma_init(&Vdma2, XPAR_AXI_VDMA_2_DEVICE_ID);
		if (Status != XST_SUCCESS)
		{
			bsp_printf(TXT_RED "In %s: XAxiVdma_CfgInitialize failed...\r\n" TXT_RST, __func__);
			return XST_FAILURE;
		}
	}
	vdma_reg_cfg
	(
		&Vdma2,
		3,
		0,	// offset (y*w+x)*Bpp
		0,
		VDMA_2_W_STRIDE,
		VDMA_2_W_WIDTH,
		VDMA_2_W_HEIGHTH,
		VDMA_2_R_STRIDE,
		VDMA_2_R_WIDTH,
		VDMA_2_R_HEIGHTH,
		vmda_2_frame_addrs
	);
#endif // XPAR_XAXIVDMA_NUM_INSTANCES == 3U

#if (XPAR_XAXIVDMA_NUM_INSTANCES >= 4U)
	UINTPTR vmda_3_frame_addrs[] = {
#if (XPAR_AXI_VDMA_3_NUM_FSTORES >= 1U)
		FRAME_BUFFER_3_0,
#endif // XPAR_AXI_VDMA_3_NUM_FSTORES == 1U
#if (XPAR_AXI_VDMA_3_NUM_FSTORES >= 2U)
		FRAME_BUFFER_3_1,
#endif // XPAR_AXI_VDMA_3_NUM_FSTORES == 2U
#if (XPAR_AXI_VDMA_3_NUM_FSTORES >= 3U)
		FRAME_BUFFER_3_2,
#endif // XPAR_AXI_VDMA_3_NUM_FSTORES == 3U
	};
	if (Vdma3.IsReady != XAXIVDMA_DEVICE_READY)
	{
		Status = vdma_init(&Vdma3, XPAR_AXI_VDMA_3_DEVICE_ID);
		if (Status != XST_SUCCESS)
		{
			bsp_printf(TXT_RED "In %s: XAxiVdma_CfgInitialize failed...\r\n" TXT_RST, __func__);
			return XST_FAILURE;
		}
	}
	vdma_reg_cfg
	(
		&Vdma3,
		3,
		0,	// offset (y*w+x)*Bpp
		0,
		VDMA_3_W_STRIDE,
		VDMA_3_W_WIDTH,
		VDMA_3_W_HEIGHTH,
		VDMA_3_R_STRIDE,
		VDMA_3_R_WIDTH,
		VDMA_3_R_HEIGHTH,
		vmda_3_frame_addrs
	);
#endif // XPAR_XAXIVDMA_NUM_INSTANCES == 4U

#if (XPAR_XAXIVDMA_NUM_INSTANCES >= 5U)
	UINTPTR vmda_4_frame_addrs[] = {
#if (XPAR_AXI_VDMA_4_NUM_FSTORES >= 1U)
		FRAME_BUFFER_4_0,
#endif // XPAR_AXI_VDMA_4_NUM_FSTORES == 1U
#if (XPAR_AXI_VDMA_4_NUM_FSTORES >= 2U)
		FRAME_BUFFER_4_1,
#endif // XPAR_AXI_VDMA_4_NUM_FSTORES == 2U
#if (XPAR_AXI_VDMA_4_NUM_FSTORES >= 3U)
		FRAME_BUFFER_4_2,
#endif // XPAR_AXI_VDMA_4_NUM_FSTORES == 3U
	};
	if (Vdma4.IsReady != XAXIVDMA_DEVICE_READY)
	{
		Status = vdma_init(&Vdma4, XPAR_AXI_VDMA_4_DEVICE_ID);
		if (Status != XST_SUCCESS)
		{
			bsp_printf(TXT_RED "In %s: XAxiVdma_CfgInitialize failed...\r\n" TXT_RST, __func__);
			return XST_FAILURE;
		}
	}
	vdma_reg_cfg
	(
		&Vdma4,
		3,
		0,	// offset (y*w+x)*Bpp
		0,
		VDMA_4_W_STRIDE,
		VDMA_4_W_WIDTH,
		VDMA_4_W_HEIGHTH,
		VDMA_4_R_STRIDE,
		VDMA_4_R_WIDTH,
		VDMA_4_R_HEIGHTH,
		vmda_4_frame_addrs
	);
#endif // XPAR_XAXIVDMA_NUM_INSTANCES == 5U

#if (XPAR_XAXIVDMA_NUM_INSTANCES >= 6U)
	UINTPTR vmda_5_frame_addrs[] = {
#if (XPAR_AXI_VDMA_5_NUM_FSTORES >= 1U)
		FRAME_BUFFER_5_0,
#endif // XPAR_AXI_VDMA_5_NUM_FSTORES == 1U
#if (XPAR_AXI_VDMA_5_NUM_FSTORES >= 2U)
		FRAME_BUFFER_5_1,
#endif // XPAR_AXI_VDMA_5_NUM_FSTORES == 2U
#if (XPAR_AXI_VDMA_5_NUM_FSTORES >= 3U)
		FRAME_BUFFER_5_2,
#endif // XPAR_AXI_VDMA_5_NUM_FSTORES == 3U
	};
	if (Vdma5.IsReady != XAXIVDMA_DEVICE_READY)
	{
		Status = vdma_init(&Vdma5, XPAR_AXI_VDMA_5_DEVICE_ID);
		if (Status != XST_SUCCESS)
		{
			bsp_printf(TXT_RED "In %s: XAxiVdma_CfgInitialize failed...\r\n" TXT_RST, __func__);
			return XST_FAILURE;
		}
	}
	vdma_reg_cfg
	(
		&Vdma5,
		3,
		0,	// offset (y*w+x)*Bpp
		0,
		VDMA_5_W_STRIDE,
		VDMA_5_W_WIDTH,
		VDMA_5_W_HEIGHTH,
		VDMA_5_R_STRIDE,
		VDMA_5_R_WIDTH,
		VDMA_5_R_HEIGHTH,
		vmda_5_frame_addrs
	);
#endif // XPAR_XAXIVDMA_NUM_INSTANCES == 6U

#if (XPAR_XAXIVDMA_NUM_INSTANCES >= 7U)
	UINTPTR vmda_6_frame_addrs[] = {
#if (XPAR_AXI_VDMA_6_NUM_FSTORES >= 1U)
		FRAME_BUFFER_6_0,
#endif // XPAR_AXI_VDMA_6_NUM_FSTORES == 1U
#if (XPAR_AXI_VDMA_6_NUM_FSTORES >= 2U)
		FRAME_BUFFER_6_1,
#endif // XPAR_AXI_VDMA_6_NUM_FSTORES == 2U
#if (XPAR_AXI_VDMA_6_NUM_FSTORES >= 3U)
		FRAME_BUFFER_6_2,
#endif // XPAR_AXI_VDMA_6_NUM_FSTORES == 3U
	};
	if (Vdma6.IsReady != XAXIVDMA_DEVICE_READY)
	{
		Status = vdma_init(&Vdma6, XPAR_AXI_VDMA_6_DEVICE_ID);
		if (Status != XST_SUCCESS)
		{
			bsp_printf(TXT_RED "In %s: XAxiVdma_CfgInitialize failed...\r\n" TXT_RST, __func__);
			return XST_FAILURE;
		}
	}
	vdma_reg_cfg
	(
		&Vdma6,
		3,
		0,	// offset (y*w+x)*Bpp
		0,
		VDMA_6_W_STRIDE,
		VDMA_6_W_WIDTH,
		VDMA_6_W_HEIGHTH,
		VDMA_6_R_STRIDE,
		VDMA_6_R_WIDTH,
		VDMA_6_R_HEIGHTH,
		vmda_6_frame_addrs
	);
#endif // XPAR_XAXIVDMA_NUM_INSTANCES == 7U
#if (XPAR_XAXIVDMA_NUM_INSTANCES >= 8U)
	UINTPTR vmda_7_frame_addrs[] = {
#if (XPAR_AXI_VDMA_7_NUM_FSTORES >= 1U)
		FRAME_BUFFER_7_0,
#endif // XPAR_AXI_VDMA_7_NUM_FSTORES == 1U
#if (XPAR_AXI_VDMA_7_NUM_FSTORES >= 2U)
		FRAME_BUFFER_7_1,
#endif // XPAR_AXI_VDMA_7_NUM_FSTORES == 2U
#if (XPAR_AXI_VDMA_7_NUM_FSTORES >= 3U)
		FRAME_BUFFER_7_2,
#endif // XPAR_AXI_VDMA_7_NUM_FSTORES == 3U
	};
	if (Vdma7.IsReady != XAXIVDMA_DEVICE_READY)
	{
		Status = vdma_init(&Vdma7, XPAR_AXI_VDMA_7_DEVICE_ID);
		if (Status != XST_SUCCESS)
		{
			bsp_printf(TXT_RED "In %s: XAxiVdma_CfgInitialize failed...\r\n" TXT_RST, __func__);
			return XST_FAILURE;
		}
	}
	vdma_reg_cfg
	(
		&Vdma7,
		3,
		0,	// offset (y*w+x)*Bpp
		0,
		VDMA_7_W_STRIDE,
		VDMA_7_W_WIDTH,
		VDMA_7_W_HEIGHTH,
		VDMA_7_R_STRIDE,
		VDMA_7_R_WIDTH,
		VDMA_7_R_HEIGHTH,
		vmda_7_frame_addrs
	);
#endif // XPAR_XAXIVDMA_NUM_INSTANCES == 8U

	return XST_SUCCESS;
}
# if 0
void vdma_config_m32(void)
{
//	Xil_DCacheDisable();
//	memset(FRAME_BUFFER_7,0,2880*2*4);
//	memset(FRAME_BUFFER_8,0,2880*2*4);
//	memset(FRAME_BUFFER_9,0,2880*2*4);
//	memset(FRAME_BUFFER_7+2880*2*1084,0,2880*2*4);
//	memset(FRAME_BUFFER_8+2880*2*1084,0,2880*2*4);
//	memset(FRAME_BUFFER_9+2880*2*1084,0,2880*2*4);
//	memcpy(FRAME_BUFFER_7+2880*2*0,ar0233_embd_head_line[0], 2880*2);
//	memcpy(FRAME_BUFFER_7+2880*2*1,ar0233_embd_head_line[1], 2880*2);
//	memcpy(FRAME_BUFFER_7+2880*2*2,ar0233_embd_head_line[2], 2880*2);
//	memcpy(FRAME_BUFFER_7+2880*2*3,ar0233_embd_head_line[3], 2880*2);
//	memcpy(FRAME_BUFFER_8+2880*2*0,ar0233_embd_head_line[0], 2880*2);
//	memcpy(FRAME_BUFFER_8+2880*2*1,ar0233_embd_head_line[1], 2880*2);
//	memcpy(FRAME_BUFFER_8+2880*2*2,ar0233_embd_head_line[2], 2880*2);
//	memcpy(FRAME_BUFFER_8+2880*2*3,ar0233_embd_head_line[3], 2880*2);
//	memcpy(FRAME_BUFFER_9+2880*2*0,ar0233_embd_head_line[0], 2880*2);
//	memcpy(FRAME_BUFFER_9+2880*2*1,ar0233_embd_head_line[1], 2880*2);
//	memcpy(FRAME_BUFFER_9+2880*2*2,ar0233_embd_head_line[2], 2880*2);
//	memcpy(FRAME_BUFFER_9+2880*2*3,ar0233_embd_head_line[3], 2880*2);
//	memcpy(FRAME_BUFFER_7+2880*2*1084, ar0233_embd_tail_line[0], 2880*2);
//	memcpy(FRAME_BUFFER_7+2880*2*1085, ar0233_embd_tail_line[1], 2880*2);
//	memcpy(FRAME_BUFFER_7+2880*2*1086, ar0233_embd_tail_line[2], 2880*2);
//	memcpy(FRAME_BUFFER_7+2880*2*1087, ar0233_embd_tail_line[3], 2880*2);
//	memcpy(FRAME_BUFFER_8+2880*2*1084, ar0233_embd_tail_line[0], 2880*2);
//	memcpy(FRAME_BUFFER_8+2880*2*1085, ar0233_embd_tail_line[1], 2880*2);
//	memcpy(FRAME_BUFFER_8+2880*2*1086, ar0233_embd_tail_line[2], 2880*2);
//	memcpy(FRAME_BUFFER_8+2880*2*1087, ar0233_embd_tail_line[3], 2880*2);
//	memcpy(FRAME_BUFFER_9+2880*2*1084, ar0233_embd_tail_line[0], 2880*2);
//	memcpy(FRAME_BUFFER_9+2880*2*1085, ar0233_embd_tail_line[1], 2880*2);
//	memcpy(FRAME_BUFFER_9+2880*2*1086, ar0233_embd_tail_line[2], 2880*2);
//	memcpy(FRAME_BUFFER_9+2880*2*1087, ar0233_embd_tail_line[3], 2880*2);
//	Xil_DCacheEnable();

#if (XPAR_XAXIVDMA_NUM_INSTANCES >= 1U) && (XPAR_AXI_VDMA_0_ADDR_WIDTH == 32U)
	vdma_config_m32_0();
#endif
#if (XPAR_XAXIVDMA_NUM_INSTANCES >= 2U) && (XPAR_AXI_VDMA_1_ADDR_WIDTH == 32U)
	vdma_config_m32_1();
#endif
#if (XPAR_XAXIVDMA_NUM_INSTANCES >= 3U) && (XPAR_AXI_VDMA_2_ADDR_WIDTH == 32U)
	vdma_config_m32_2();
#endif
#if (XPAR_XAXIVDMA_NUM_INSTANCES >= 4U) && (XPAR_AXI_VDMA_3_ADDR_WIDTH == 32U)
	vdma_config_m32_3();
#endif
#if (XPAR_XAXIVDMA_NUM_INSTANCES >= 5U) && (XPAR_AXI_VDMA_4_ADDR_WIDTH == 32U)
	vdma_config_m32_4();
#endif
#if (XPAR_XAXIVDMA_NUM_INSTANCES >= 6U) && (XPAR_AXI_VDMA_5_ADDR_WIDTH == 32U)
	vdma_config_m32_5();
#endif
#if (XPAR_XAXIVDMA_NUM_INSTANCES >= 7U) && (XPAR_AXI_VDMA_6_ADDR_WIDTH == 32U)
	vdma_config_m32_6();
#endif
#if (XPAR_XAXIVDMA_NUM_INSTANCES >= 8U) && (XPAR_AXI_VDMA_7_ADDR_WIDTH == 32U)
	vdma_config_m32_7();
#endif
}

void vdma_config_m64(void)
{
	//	Xil_DCacheDisable();
	//	memset(FRAME_BUFFER_7,0,2880*2*4);
	//	memset(FRAME_BUFFER_8,0,2880*2*4);
	//	memset(FRAME_BUFFER_9,0,2880*2*4);
	//	memset(FRAME_BUFFER_7+2880*2*1084,0,2880*2*4);
	//	memset(FRAME_BUFFER_8+2880*2*1084,0,2880*2*4);
	//	memset(FRAME_BUFFER_9+2880*2*1084,0,2880*2*4);
	//	memcpy(FRAME_BUFFER_7+2880*2*0,ar0233_embd_head_line[0], 2880*2);
	//	memcpy(FRAME_BUFFER_7+2880*2*1,ar0233_embd_head_line[1], 2880*2);
	//	memcpy(FRAME_BUFFER_7+2880*2*2,ar0233_embd_head_line[2], 2880*2);
	//	memcpy(FRAME_BUFFER_7+2880*2*3,ar0233_embd_head_line[3], 2880*2);
	//	memcpy(FRAME_BUFFER_8+2880*2*0,ar0233_embd_head_line[0], 2880*2);
	//	memcpy(FRAME_BUFFER_8+2880*2*1,ar0233_embd_head_line[1], 2880*2);
	//	memcpy(FRAME_BUFFER_8+2880*2*2,ar0233_embd_head_line[2], 2880*2);
	//	memcpy(FRAME_BUFFER_8+2880*2*3,ar0233_embd_head_line[3], 2880*2);
	//	memcpy(FRAME_BUFFER_9+2880*2*0,ar0233_embd_head_line[0], 2880*2);
	//	memcpy(FRAME_BUFFER_9+2880*2*1,ar0233_embd_head_line[1], 2880*2);
	//	memcpy(FRAME_BUFFER_9+2880*2*2,ar0233_embd_head_line[2], 2880*2);
	//	memcpy(FRAME_BUFFER_9+2880*2*3,ar0233_embd_head_line[3], 2880*2);
	//	memcpy(FRAME_BUFFER_7+2880*2*1084, ar0233_embd_tail_line[0], 2880*2);
	//	memcpy(FRAME_BUFFER_7+2880*2*1085, ar0233_embd_tail_line[1], 2880*2);
	//	memcpy(FRAME_BUFFER_7+2880*2*1086, ar0233_embd_tail_line[2], 2880*2);
	//	memcpy(FRAME_BUFFER_7+2880*2*1087, ar0233_embd_tail_line[3], 2880*2);
	//	memcpy(FRAME_BUFFER_8+2880*2*1084, ar0233_embd_tail_line[0], 2880*2);
	//	memcpy(FRAME_BUFFER_8+2880*2*1085, ar0233_embd_tail_line[1], 2880*2);
	//	memcpy(FRAME_BUFFER_8+2880*2*1086, ar0233_embd_tail_line[2], 2880*2);
	//	memcpy(FRAME_BUFFER_8+2880*2*1087, ar0233_embd_tail_line[3], 2880*2);
	//	memcpy(FRAME_BUFFER_9+2880*2*1084, ar0233_embd_tail_line[0], 2880*2);
	//	memcpy(FRAME_BUFFER_9+2880*2*1085, ar0233_embd_tail_line[1], 2880*2);
	//	memcpy(FRAME_BUFFER_9+2880*2*1086, ar0233_embd_tail_line[2], 2880*2);
	//	memcpy(FRAME_BUFFER_9+2880*2*1087, ar0233_embd_tail_line[3], 2880*2);
	//	Xil_DCacheEnable();

#if (XPAR_XAXIVDMA_NUM_INSTANCES >= 1U) && (XPAR_AXI_VDMA_0_ADDR_WIDTH == 64U)
	vdma_config_m64_0();
#endif
#if (XPAR_XAXIVDMA_NUM_INSTANCES >= 2U) && (XPAR_AXI_VDMA_1_ADDR_WIDTH == 64U)
	vdma_config_m64_1();
#endif
#if (XPAR_XAXIVDMA_NUM_INSTANCES >= 3U) && (XPAR_AXI_VDMA_2_ADDR_WIDTH == 64U)
	vdma_config_m64_2();
#endif
#if (XPAR_XAXIVDMA_NUM_INSTANCES >= 4U) && (XPAR_AXI_VDMA_3_ADDR_WIDTH == 64U)
	vdma_config_m64_3();
#endif
#if (XPAR_XAXIVDMA_NUM_INSTANCES >= 5U) && (XPAR_AXI_VDMA_4_ADDR_WIDTH == 64U)
	vdma_config_m64_4();
#endif
#if (XPAR_XAXIVDMA_NUM_INSTANCES >= 6U) && (XPAR_AXI_VDMA_5_ADDR_WIDTH == 64U)
	vdma_config_m64_5();
#endif
#if (XPAR_XAXIVDMA_NUM_INSTANCES >= 7U) && (XPAR_AXI_VDMA_6_ADDR_WIDTH == 64U)
	vdma_config_m64_6();
#endif
#if (XPAR_XAXIVDMA_NUM_INSTANCES >= 8U) && (XPAR_AXI_VDMA_7_ADDR_WIDTH == 64U)
	vdma_config_m64_7();
#endif
}
#endif


void vdma_config_direct(void)
{
#if (XPAR_XAXIVDMA_NUM_INSTANCES >= 1U)
#if (XPAR_AXI_VDMA_0_ADDR_WIDTH == 32U)
	vdma_config_m32_0();
#endif // XPAR_AXI_VDMA_0_ADDR_WIDTH == 32U
#if (XPAR_AXI_VDMA_0_ADDR_WIDTH == 64U)
	vdma_config_m64_0();
#endif // XPAR_AXI_VDMA_0_ADDR_WIDTH == 64U
#endif
#if (XPAR_XAXIVDMA_NUM_INSTANCES >= 2U)
#if (XPAR_AXI_VDMA_1_ADDR_WIDTH == 32U)
	vdma_config_m32_1();
#endif // XPAR_AXI_VDMA_1_ADDR_WIDTH == 32U
#if (XPAR_AXI_VDMA_1_ADDR_WIDTH == 64U)
	vdma_config_m64_1();
#endif // XPAR_AXI_VDMA_1_ADDR_WIDTH == 64U
#endif
#if (XPAR_XAXIVDMA_NUM_INSTANCES >= 3U)
#if (XPAR_AXI_VDMA_2_ADDR_WIDTH == 32U)
	vdma_config_m32_2();
#endif // XPAR_AXI_VDMA_2_ADDR_WIDTH == 32U
#if (XPAR_AXI_VDMA_2_ADDR_WIDTH == 64U)
	vdma_config_m64_2();
#endif // XPAR_AXI_VDMA_2_ADDR_WIDTH == 64U
#endif
#if (XPAR_XAXIVDMA_NUM_INSTANCES >= 4U)
#if (XPAR_AXI_VDMA_3_ADDR_WIDTH == 32U)
	vdma_config_m32_3();
#endif // XPAR_AXI_VDMA_3_ADDR_WIDTH == 32U
#if (XPAR_AXI_VDMA_3_ADDR_WIDTH == 64U)
	vdma_config_m64_3();
#endif // XPAR_AXI_VDMA_3_ADDR_WIDTH == 64U
#endif
#if (XPAR_XAXIVDMA_NUM_INSTANCES >= 5U)
#if (XPAR_AXI_VDMA_4_ADDR_WIDTH == 32U)
	vdma_config_m32_4();
#endif // XPAR_AXI_VDMA_4_ADDR_WIDTH == 32U
#if (XPAR_AXI_VDMA_4_ADDR_WIDTH == 64U)
	vdma_config_m64_4();
#endif // XPAR_AXI_VDMA_4_ADDR_WIDTH == 64U
#endif
#if (XPAR_XAXIVDMA_NUM_INSTANCES >= 6U)
#if (XPAR_AXI_VDMA_5_ADDR_WIDTH == 32U)
	vdma_config_m32_5();
#endif // XPAR_AXI_VDMA_5_ADDR_WIDTH == 32U
#if (XPAR_AXI_VDMA_5_ADDR_WIDTH == 64U)
	vdma_config_m64_5();
#endif // XPAR_AXI_VDMA_5_ADDR_WIDTH == 64U
#endif
#if (XPAR_XAXIVDMA_NUM_INSTANCES >= 7U)
#if (XPAR_AXI_VDMA_6_ADDR_WIDTH == 32U)
	vdma_config_m32_6();
#endif // XPAR_AXI_VDMA_6_ADDR_WIDTH == 32U
#if (XPAR_AXI_VDMA_6_ADDR_WIDTH == 64U)
	vdma_config_m64_6();
#endif // XPAR_AXI_VDMA_6_ADDR_WIDTH == 64U
#endif
#if (XPAR_XAXIVDMA_NUM_INSTANCES >= 8U)
#if (XPAR_AXI_VDMA_7_ADDR_WIDTH == 32U)
	vdma_config_m32_7();
#endif // XPAR_AXI_VDMA_7_ADDR_WIDTH == 32U
#if (XPAR_AXI_VDMA_7_ADDR_WIDTH == 64U)
	vdma_config_m64_7();
#endif // XPAR_AXI_VDMA_7_ADDR_WIDTH == 64U
#endif
}

#if (XPAR_XAXIVDMA_NUM_INSTANCES >= 1U)
void clear_display_0(void)
{
	u32 bytePerPixels = VDMA_0_BPP;
	u32 line = 0;
	u32 column = 0;

	line = VDMA_0_R_HEIGHTH;
	column = VDMA_0_R_STRIDE;

#if (XPAR_AXIVDMA_0_INCLUDE_S2MM == 1U)
    Xil_Out32(XPAR_AXIVDMA_0_BASEADDR + 0x00, 0x8A);//stop mm2s
#endif
#if (XPAR_AXIVDMA_0_INCLUDE_MM2S == 1U)
	Xil_Out32(XPAR_AXIVDMA_0_BASEADDR + 0x30, 0x8A);//stop s2mm
#endif

	Xil_DCacheDisable();
#if (XPAR_AXI_VDMA_0_NUM_FSTORES >= 1U)
    memset((void *)FRAME_BUFFER_0_0,0xff,line*column*bytePerPixels);//background
#endif // XPAR_AXI_VDMA_0_NUM_FSTORES == 1U
#if (XPAR_AXI_VDMA_0_NUM_FSTORES >= 2U)
    memset((void *)FRAME_BUFFER_0_1,0xff,line*column*bytePerPixels);//background
#endif // XPAR_AXI_VDMA_0_NUM_FSTORES == 2U
#if (XPAR_AXI_VDMA_0_NUM_FSTORES >= 3U)
    memset((void *)FRAME_BUFFER_0_2,0xff,line*column*bytePerPixels);//background
#endif // XPAR_AXI_VDMA_0_NUM_FSTORES == 3U
	Xil_DCacheEnable();

//	bsp_printf("clear vdma 0 disp done!\n\r");

#if defined (CFG_VDMA_0_AFTER_CLEAR)

#if (XPAR_AXI_VDMA_0_ADDR_WIDTH == 32U)
	vdma_config_m32_0();
#endif // XPAR_AXI_VDMA_0_ADDR_WIDTH == 32U
#if (XPAR_AXI_VDMA_0_ADDR_WIDTH == 64U)
	vdma_config_m64_0();
#endif // XPAR_AXI_VDMA_0_ADDR_WIDTH == 64U

#endif // CFG_VDMA_0_AFTER_CLEAR
}
#endif // XPAR_XAXIVDMA_NUM_INSTANCES == 1U

#if (XPAR_XAXIVDMA_NUM_INSTANCES >= 2U)
void clear_display_1(void)
{
	u32 bytePerPixels = VDMA_1_BPP;
	u32 line = 0;
	u32 column = 0;

	line = VDMA_1_R_HEIGHTH;
	column = VDMA_1_R_STRIDE;

#if (XPAR_AXIVDMA_1_INCLUDE_S2MM == 1U)
    Xil_Out32(XPAR_AXIVDMA_1_BASEADDR + 0x00, 0x8A);//stop mm2s
#endif
#if (XPAR_AXIVDMA_1_INCLUDE_MM2S == 1U)
	Xil_Out32(XPAR_AXIVDMA_1_BASEADDR + 0x30, 0x8A);//stop s2mm
#endif

	Xil_DCacheDisable();
#if (XPAR_AXI_VDMA_1_NUM_FSTORES >= 1U)
    memset((void *)FRAME_BUFFER_1_0,0xff,line*column*bytePerPixels);//background
#endif // XPAR_AXI_VDMA_1_NUM_FSTORES == 1U
#if (XPAR_AXI_VDMA_1_NUM_FSTORES >= 2U)
    memset((void *)FRAME_BUFFER_1_1,0xff,line*column*bytePerPixels);//background
#endif // XPAR_AXI_VDMA_1_NUM_FSTORES == 2U
#if (XPAR_AXI_VDMA_1_NUM_FSTORES >= 3U)
    memset((void *)FRAME_BUFFER_1_2,0xff,line*column*bytePerPixels);//background
#endif // XPAR_AXI_VDMA_1_NUM_FSTORES == 3U
	Xil_DCacheEnable();

//	bsp_printf("clear vdma 1 disp done!\n\r");

#if defined (CFG_VDMA_1_AFTER_CLEAR)

#if (XPAR_AXI_VDMA_1_ADDR_WIDTH == 32U)
	vdma_config_m32_1();
#endif // XPAR_AXI_VDMA_1_ADDR_WIDTH == 32U
#if (XPAR_AXI_VDMA_1_ADDR_WIDTH == 64U)
	vdma_config_m64_1();
#endif // XPAR_AXI_VDMA_1_ADDR_WIDTH == 64U

#endif // CFG_VDMA_1_AFTER_CLEAR
}
#endif // XPAR_XAXIVDMA_NUM_INSTANCES == 2U

#if (XPAR_XAXIVDMA_NUM_INSTANCES >= 3U)
void clear_display_2(void)
{
	u32 bytePerPixels = VDMA_2_BPP;
	u32 line = 0;
	u32 column = 0;

	line = VDMA_2_R_HEIGHTH;
	column = VDMA_2_R_STRIDE;

#if (XPAR_AXIVDMA_2_INCLUDE_S2MM == 1U)
    Xil_Out32(XPAR_AXIVDMA_2_BASEADDR + 0x00, 0x8A);//stop mm2s
#endif
#if (XPAR_AXIVDMA_2_INCLUDE_MM2S == 1U)
	Xil_Out32(XPAR_AXIVDMA_2_BASEADDR + 0x30, 0x8A);//stop s2mm
#endif

	Xil_DCacheDisable();
#if (XPAR_AXI_VDMA_2_NUM_FSTORES >= 1U)
    memset((void *)FRAME_BUFFER_2_0,0xff,line*column*bytePerPixels);//background
#endif // XPAR_AXI_VDMA_2_NUM_FSTORES == 1U
#if (XPAR_AXI_VDMA_2_NUM_FSTORES >= 2U)
    memset((void *)FRAME_BUFFER_2_1,0xff,line*column*bytePerPixels);//background
#endif // XPAR_AXI_VDMA_2_NUM_FSTORES == 2U
#if (XPAR_AXI_VDMA_2_NUM_FSTORES >= 3U)
    memset((void *)FRAME_BUFFER_2_2,0xff,line*column*bytePerPixels);//background
#endif // XPAR_AXI_VDMA_2_NUM_FSTORES == 3U
	Xil_DCacheEnable();

//	bsp_printf("clear vdma 2 disp done!\n\r");

#if defined (CFG_VDMA_2_AFTER_CLEAR)

#if (XPAR_AXI_VDMA_2_ADDR_WIDTH == 32U)
	vdma_config_m32_2();
#endif // XPAR_AXI_VDMA_2_ADDR_WIDTH == 32U
#if (XPAR_AXI_VDMA_2_ADDR_WIDTH == 64U)
	vdma_config_m64_2();
#endif // XPAR_AXI_VDMA_2_ADDR_WIDTH == 64U

#endif // CFG_VDMA_2_AFTER_CLEAR
}
#endif // XPAR_XAXIVDMA_NUM_INSTANCES == 3U

#if (XPAR_XAXIVDMA_NUM_INSTANCES >= 4U)
void clear_display_3(void)
{
	u32 bytePerPixels = VDMA_3_BPP;
	u32 line = 0;
	u32 column = 0;

	line = VDMA_3_R_HEIGHTH;
	column = VDMA_3_R_STRIDE;

#if (XPAR_AXIVDMA_3_INCLUDE_S2MM == 1U)
    Xil_Out32(XPAR_AXIVDMA_3_BASEADDR + 0x00, 0x8A);//stop mm2s
#endif
#if (XPAR_AXIVDMA_3_INCLUDE_MM2S == 1U)
	Xil_Out32(XPAR_AXIVDMA_3_BASEADDR + 0x30, 0x8A);//stop s2mm
#endif

	Xil_DCacheDisable();
#if (XPAR_AXI_VDMA_3_NUM_FSTORES >= 1U)
    memset((void *)FRAME_BUFFER_3_0,0xff,line*column*bytePerPixels);//background
#endif // XPAR_AXI_VDMA_3_NUM_FSTORES == 1U
#if (XPAR_AXI_VDMA_3_NUM_FSTORES >= 2U)
    memset((void *)FRAME_BUFFER_3_1,0xff,line*column*bytePerPixels);//background
#endif // XPAR_AXI_VDMA_3_NUM_FSTORES == 2U
#if (XPAR_AXI_VDMA_3_NUM_FSTORES >= 3U)
    memset((void *)FRAME_BUFFER_3_2,0xff,line*column*bytePerPixels);//background
#endif // XPAR_AXI_VDMA_3_NUM_FSTORES == 3U
	Xil_DCacheEnable();

//	bsp_printf("clear vdma 3 disp done!\n\r");

#if defined (CFG_VDMA_3_AFTER_CLEAR)

#if (XPAR_AXI_VDMA_3_ADDR_WIDTH == 32U)
	vdma_config_m32_3();
#endif // XPAR_AXI_VDMA_3_ADDR_WIDTH == 32U
#if (XPAR_AXI_VDMA_3_ADDR_WIDTH == 64U)
	vdma_config_m64_3();
#endif // XPAR_AXI_VDMA_3_ADDR_WIDTH == 64U

#endif // CFG_VDMA_3_AFTER_CLEAR
}
#endif // XPAR_XAXIVDMA_NUM_INSTANCES == 4U

#if (XPAR_XAXIVDMA_NUM_INSTANCES >= 5U)
void clear_display_4(void)
{
	u32 bytePerPixels = VDMA_4_BPP;
	u32 line = 0;
	u32 column = 0;

	line = VDMA_4_R_HEIGHTH;
	column = VDMA_4_R_STRIDE;

#if (XPAR_AXIVDMA_4_INCLUDE_S2MM == 1U)
    Xil_Out32(XPAR_AXIVDMA_4_BASEADDR + 0x00, 0x8A);//stop mm2s
#endif
#if (XPAR_AXIVDMA_4_INCLUDE_MM2S == 1U)
	Xil_Out32(XPAR_AXIVDMA_4_BASEADDR + 0x30, 0x8A);//stop s2mm
#endif

	Xil_DCacheDisable();
#if (XPAR_AXI_VDMA_4_NUM_FSTORES >= 1U)
    memset((void *)FRAME_BUFFER_4_0,0xff,line*column*bytePerPixels);//background
#endif // XPAR_AXI_VDMA_4_NUM_FSTORES == 1U
#if (XPAR_AXI_VDMA_4_NUM_FSTORES >= 2U)
    memset((void *)FRAME_BUFFER_4_1,0xff,line*column*bytePerPixels);//background
#endif // XPAR_AXI_VDMA_4_NUM_FSTORES == 2U
#if (XPAR_AXI_VDMA_4_NUM_FSTORES >= 3U)
    memset((void *)FRAME_BUFFER_4_2,0xff,line*column*bytePerPixels);//background
#endif // XPAR_AXI_VDMA_4_NUM_FSTORES == 3U
	Xil_DCacheEnable();

//	bsp_printf("clear vdma 4 disp done!\n\r");

#if defined (CFG_VDMA_4_AFTER_CLEAR)

#if (XPAR_AXI_VDMA_4_ADDR_WIDTH == 32U)
	vdma_config_m32_4();
#endif // XPAR_AXI_VDMA_4_ADDR_WIDTH == 32U
#if (XPAR_AXI_VDMA_4_ADDR_WIDTH == 64U)
	vdma_config_m64_4();
#endif // XPAR_AXI_VDMA_4_ADDR_WIDTH == 64U

#endif // CFG_VDMA_4_AFTER_CLEAR
}
#endif // XPAR_XAXIVDMA_NUM_INSTANCES == 5U


#if (XPAR_XAXIVDMA_NUM_INSTANCES >= 6U)
void clear_display_5(void)
{
	u32 bytePerPixels = VDMA_5_BPP;
	u32 line = 0;
	u32 column = 0;

	line = VDMA_5_R_HEIGHTH;
	column = VDMA_5_R_STRIDE;

#if (XPAR_AXIVDMA_5_INCLUDE_S2MM == 1U)
    Xil_Out32(XPAR_AXIVDMA_5_BASEADDR + 0x00, 0x8A);//stop mm2s
#endif
#if (XPAR_AXIVDMA_5_INCLUDE_MM2S == 1U)
	Xil_Out32(XPAR_AXIVDMA_5_BASEADDR + 0x30, 0x8A);//stop s2mm
#endif

	Xil_DCacheDisable();
#if (XPAR_AXI_VDMA_5_NUM_FSTORES >= 1U)
    memset((void *)FRAME_BUFFER_5_0,0xff,line*column*bytePerPixels);//background
#endif // XPAR_AXI_VDMA_5_NUM_FSTORES == 1U
#if (XPAR_AXI_VDMA_5_NUM_FSTORES >= 2U)
    memset((void *)FRAME_BUFFER_5_1,0xff,line*column*bytePerPixels);//background
#endif // XPAR_AXI_VDMA_5_NUM_FSTORES == 2U
#if (XPAR_AXI_VDMA_5_NUM_FSTORES >= 3U)
    memset((void *)FRAME_BUFFER_5_2,0xff,line*column*bytePerPixels);//background
#endif // XPAR_AXI_VDMA_5_NUM_FSTORES == 3U
	Xil_DCacheEnable();

//	bsp_printf("clear vdma 5 disp done!\n\r");

#if defined (CFG_VDMA_5_AFTER_CLEAR)

#if (XPAR_AXI_VDMA_5_ADDR_WIDTH == 32U)
	vdma_config_m32_5();
#endif // XPAR_AXI_VDMA_5_ADDR_WIDTH == 32U
#if (XPAR_AXI_VDMA_5_ADDR_WIDTH == 64U)
	vdma_config_m64_5();
#endif // XPAR_AXI_VDMA_5_ADDR_WIDTH == 64U

#endif // CFG_VDMA_5_AFTER_CLEAR
}
#endif // XPAR_XAXIVDMA_NUM_INSTANCES == 6U

#if (XPAR_XAXIVDMA_NUM_INSTANCES >= 7U)
void clear_display_6(void)
{
	u32 bytePerPixels = VDMA_6_BPP;
	u32 line = 0;
	u32 column = 0;

	line = VDMA_6_R_HEIGHTH;
	column = VDMA_6_R_STRIDE;

#if (XPAR_AXIVDMA_6_INCLUDE_S2MM == 1U)
    Xil_Out32(XPAR_AXIVDMA_6_BASEADDR + 0x00, 0x8A);//stop mm2s
#endif
#if (XPAR_AXIVDMA_6_INCLUDE_MM2S == 1U)
	Xil_Out32(XPAR_AXIVDMA_6_BASEADDR + 0x30, 0x8A);//stop s2mm
#endif

	Xil_DCacheDisable();
#if (XPAR_AXI_VDMA_6_NUM_FSTORES >= 1U)
    memset((void *)FRAME_BUFFER_6_0,0xff,line*column*bytePerPixels);//background
#endif // XPAR_AXI_VDMA_6_NUM_FSTORES == 1U
#if (XPAR_AXI_VDMA_6_NUM_FSTORES >= 2U)
    memset((void *)FRAME_BUFFER_6_1,0xff,line*column*bytePerPixels);//background
#endif // XPAR_AXI_VDMA_6_NUM_FSTORES == 2U
#if (XPAR_AXI_VDMA_6_NUM_FSTORES >= 3U)
    memset((void *)FRAME_BUFFER_6_2,0xff,line*column*bytePerPixels);//background
#endif // XPAR_AXI_VDMA_6_NUM_FSTORES == 3U
	Xil_DCacheEnable();

//	bsp_printf("clear vdma 6 disp done!\n\r");

#if defined (CFG_VDMA_6_AFTER_CLEAR)

#if (XPAR_AXI_VDMA_6_ADDR_WIDTH == 32U)
	vdma_config_m32_6();
#endif // XPAR_AXI_VDMA_6_ADDR_WIDTH == 32U
#if (XPAR_AXI_VDMA_6_ADDR_WIDTH == 64U)
	vdma_config_m64_6();
#endif // XPAR_AXI_VDMA_6_ADDR_WIDTH == 64U

#endif // CFG_VDMA_6_AFTER_CLEAR
}
#endif // XPAR_XAXIVDMA_NUM_INSTANCES == 7U

#if (XPAR_XAXIVDMA_NUM_INSTANCES >= 8U)
void clear_display_7(void)
{
	u32 bytePerPixels = VDMA_7_BPP;
	u32 line = 0;
	u32 column = 0;

	line = VDMA_7_R_HEIGHTH;
	column = VDMA_7_R_STRIDE;

#if (XPAR_AXIVDMA_7_INCLUDE_S2MM == 1U)
    Xil_Out32(XPAR_AXIVDMA_7_BASEADDR + 0x00, 0x8A);//stop mm2s
#endif
#if (XPAR_AXIVDMA_7_INCLUDE_MM2S == 1U)
	Xil_Out32(XPAR_AXIVDMA_7_BASEADDR + 0x30, 0x8A);//stop s2mm
#endif

	Xil_DCacheDisable();
#if (XPAR_AXI_VDMA_7_NUM_FSTORES >= 1U)
    memset((void *)FRAME_BUFFER_7_0,0xff,line*column*bytePerPixels);//background
#endif // XPAR_AXI_VDMA_7_NUM_FSTORES == 1U
#if (XPAR_AXI_VDMA_7_NUM_FSTORES >= 2U)
    memset((void *)FRAME_BUFFER_7_1,0xff,line*column*bytePerPixels);//background
#endif // XPAR_AXI_VDMA_7_NUM_FSTORES == 2U
#if (XPAR_AXI_VDMA_7_NUM_FSTORES >= 3U)
    memset((void *)FRAME_BUFFER_7_2,0xff,line*column*bytePerPixels);//background
#endif // XPAR_AXI_VDMA_7_NUM_FSTORES == 3U
	Xil_DCacheEnable();

//	bsp_printf("clear vdma 7 disp done!\n\r");

#if defined (CFG_VDMA_7_AFTER_CLEAR)

#if (XPAR_AXI_VDMA_7_ADDR_WIDTH == 32U)
	vdma_config_m32_7();
#endif // XPAR_AXI_VDMA_7_ADDR_WIDTH == 32U
#if (XPAR_AXI_VDMA_7_ADDR_WIDTH == 64U)
	vdma_config_m64_7();
#endif // XPAR_AXI_VDMA_7_ADDR_WIDTH == 64U

#endif // CFG_VDMA_7_AFTER_CLEAR
}
#endif // XPAR_XAXIVDMA_NUM_INSTANCES == 8U

void clear_display(void)
{
#if (XPAR_XAXIVDMA_NUM_INSTANCES >= 1U)
	clear_display_0();
#endif
#if (XPAR_XAXIVDMA_NUM_INSTANCES >= 2U)
	clear_display_1();
#endif
#if (XPAR_XAXIVDMA_NUM_INSTANCES >= 3U)
	clear_display_2();
#endif
#if (XPAR_XAXIVDMA_NUM_INSTANCES >= 4U)
	clear_display_3();
#endif
#if (XPAR_XAXIVDMA_NUM_INSTANCES >= 5U)
	clear_display_4();
#endif
#if (XPAR_XAXIVDMA_NUM_INSTANCES >= 6U)
	clear_display_5();
#endif
#if (XPAR_XAXIVDMA_NUM_INSTANCES >= 7U)
	clear_display_6();
#endif
#if (XPAR_XAXIVDMA_NUM_INSTANCES >= 8U)
	clear_display_7();
#endif
}

#endif // XPAR_XAXIVDMA_NUM_INSTANCES
