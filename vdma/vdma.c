#include "../bsp.h"

#ifdef XPAR_XAXIVDMA_NUM_INSTANCES

#define DDR_BASEADDR XPAR_DDR_MEM_BASEADDR

#define FRAME_BUFFER_BASE_ADDR  	(DDR_BASEADDR + (0x10000000))

//#define FRAME_BUFFER_SIZE0      0x2000000    //0x2000000 for max 4KW RGB888 8bpc
#define FRAME_BUFFER_SIZE0      0x600000    //0x600000 for max 1080p RGB888 8bpc

#define FRAME_BUFFER_0          FRAME_BUFFER_BASE_ADDR + (FRAME_BUFFER_SIZE0*0)
#define FRAME_BUFFER_1          FRAME_BUFFER_BASE_ADDR + (FRAME_BUFFER_SIZE0*1)
#define FRAME_BUFFER_2          FRAME_BUFFER_BASE_ADDR + (FRAME_BUFFER_SIZE0*2)
#define FRAME_BUFFER_3          FRAME_BUFFER_BASE_ADDR + (FRAME_BUFFER_SIZE0*3)
#define FRAME_BUFFER_4          FRAME_BUFFER_BASE_ADDR + (FRAME_BUFFER_SIZE0*4)
#define FRAME_BUFFER_5          FRAME_BUFFER_BASE_ADDR + (FRAME_BUFFER_SIZE0*5)
#define FRAME_BUFFER_6          FRAME_BUFFER_BASE_ADDR + (FRAME_BUFFER_SIZE0*6)
#define FRAME_BUFFER_7          FRAME_BUFFER_BASE_ADDR + (FRAME_BUFFER_SIZE0*7)
#define FRAME_BUFFER_8          FRAME_BUFFER_BASE_ADDR + (FRAME_BUFFER_SIZE0*8)
#define FRAME_BUFFER_9          FRAME_BUFFER_BASE_ADDR + (FRAME_BUFFER_SIZE0*9)
#define FRAME_BUFFER_10         FRAME_BUFFER_BASE_ADDR + (FRAME_BUFFER_SIZE0*10)
#define FRAME_BUFFER_11         FRAME_BUFFER_BASE_ADDR + (FRAME_BUFFER_SIZE0*11)
#define FRAME_BUFFER_12         FRAME_BUFFER_BASE_ADDR + (FRAME_BUFFER_SIZE0*12)
#define FRAME_BUFFER_13         FRAME_BUFFER_BASE_ADDR + (FRAME_BUFFER_SIZE0*13)
#define FRAME_BUFFER_14         FRAME_BUFFER_BASE_ADDR + (FRAME_BUFFER_SIZE0*14)
#define FRAME_BUFFER_15         FRAME_BUFFER_BASE_ADDR + (FRAME_BUFFER_SIZE0*15)
#define FRAME_BUFFER_16         FRAME_BUFFER_BASE_ADDR + (FRAME_BUFFER_SIZE0*16)
#define FRAME_BUFFER_17         FRAME_BUFFER_BASE_ADDR + (FRAME_BUFFER_SIZE0*17)
#define FRAME_BUFFER_18         FRAME_BUFFER_BASE_ADDR + (FRAME_BUFFER_SIZE0*18)
#define FRAME_BUFFER_19         FRAME_BUFFER_BASE_ADDR + (FRAME_BUFFER_SIZE0*19)
#define FRAME_BUFFER_20         FRAME_BUFFER_BASE_ADDR + (FRAME_BUFFER_SIZE0*20)
#define FRAME_BUFFER_21         FRAME_BUFFER_BASE_ADDR + (FRAME_BUFFER_SIZE0*21)
#define FRAME_BUFFER_22         FRAME_BUFFER_BASE_ADDR + (FRAME_BUFFER_SIZE0*22)
#define FRAME_BUFFER_23         FRAME_BUFFER_BASE_ADDR + (FRAME_BUFFER_SIZE0*23)

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
		bsp_printf("XAxiVdma_LookupConfig failure\r\n");
		return XST_FAILURE;
	}

	Status = XAxiVdma_CfgInitialize(InstancePtr, Config, Config->BaseAddress);
	if (Status != XST_SUCCESS) {
		bsp_printf("XAxiVdma_CfgInitialize failure\r\n");
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
	   bsp_printf("Start read transfer failed %d\n\r", Status);
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
			bsp_printf("XAxiVdma_CfgInitialize failure\r\n");
			return XST_FAILURE;
		}
	}

	Status = XAxiVdma_SetFrmStore(InstancePtr, 3, XAXIVDMA_READ);
	if (Status != XST_SUCCESS)
	{
		bsp_printf(
			"Setting Frame Store Number Failed in Read Channel"
							" %d\r\n", Status);
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
		bsp_printf(
			"Set frame counter failed %d\r\n", Status);

		if(Status == XST_VDMA_MISMATCH_ERROR)
			bsp_printf("DMA Mismatch Error\r\n");

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
		bsp_printf("Read channel config failed %d\r\n", Status);
		return XST_FAILURE;
	}

	ReadCfg.FrameStoreStartAddr[0] = FrameStoreStartAddr0;
	ReadCfg.FrameStoreStartAddr[1] = FrameStoreStartAddr1;
	ReadCfg.FrameStoreStartAddr[2] = FrameStoreStartAddr2;

	Status = XAxiVdma_DmaSetBufferAddr(InstancePtr, XAXIVDMA_READ, ReadCfg.FrameStoreStartAddr);
	if (Status != XST_SUCCESS)
	{
		bsp_printf("Read channel set buffer address failed %d\r\n", Status);
		return XST_FAILURE;
	}

	Status = vdma_read_start(InstancePtr);
	if (Status != XST_SUCCESS)
	{
		bsp_printf("error starting VDMA..!");
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
		bsp_printf("Start write transfer failed %d\n\r", Status);
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
			bsp_printf("XAxiVdma_CfgInitialize failure\r\n");
			return XST_FAILURE;
		}
	}

	Status = XAxiVdma_SetFrmStore(InstancePtr, 3, XAXIVDMA_WRITE);
	if (Status != XST_SUCCESS)
	{
		bsp_printf(
		    "Setting Frame Store Number Failed in Write Channel"
							" %d\r\n", Status);
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
		bsp_printf(
			"Set frame counter failed %d\r\n", Status);
		if(Status == XST_VDMA_MISMATCH_ERROR)
			bsp_printf("DMA Mismatch Error\r\n");
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
		bsp_printf("Write channel config failed %d\r\n", Status);
		return XST_FAILURE;
	}


	WriteCfg.FrameStoreStartAddr[0] = FrameStoreStartAddr0;
	WriteCfg.FrameStoreStartAddr[1] = FrameStoreStartAddr1;
	WriteCfg.FrameStoreStartAddr[2] = FrameStoreStartAddr2;

	Status = XAxiVdma_DmaSetBufferAddr(InstancePtr, XAXIVDMA_WRITE, WriteCfg.FrameStoreStartAddr);
	if (Status != XST_SUCCESS)
	{
		bsp_printf("Write channel set buffer address failed %d\r\n", Status);
		return XST_FAILURE;
	}

	Status = vdma_write_start(InstancePtr);
	if (Status != XST_SUCCESS)
	{
		bsp_printf("error starting VDMA..!");
		return Status;
	}

	return XST_SUCCESS;
}

#if (XPAR_XAXIVDMA_NUM_INSTANCES >= 1U)
void vdma_config_m32_0(void)
{
    /* Start of VDMA Configuration */
    u32 bytePerPixels = 3;

    int offset0 = 0; // (y*w+x)*Bpp
    int offset1 = 0; // (y*w+x)*Bpp

    u32 stride0 = 1920;
    u32 width0 = 1920;
    u32 height0 = 1080;
    u32 stride1 = 1920;  // crop keeps write Stride
    u32 width1 = 1920;
    u32 height1 = 1080;

    /* Configure the Write interface (S2MM)*/
    // S2MM Control Register
    Xil_Out32(XPAR_AXI_VDMA_0_BASEADDR + 0x30, 0x8B);
    //S2MM Start Address 1
    Xil_Out32(XPAR_AXI_VDMA_0_BASEADDR + 0xAC, FRAME_BUFFER_0 + offset0);
    //S2MM Start Address 2
    Xil_Out32(XPAR_AXI_VDMA_0_BASEADDR + 0xB0, FRAME_BUFFER_1 + offset0);
    //S2MM Start Address 3
    Xil_Out32(XPAR_AXI_VDMA_0_BASEADDR + 0xB4, FRAME_BUFFER_2 + offset0);
    //S2MM Frame delay / Stride register
    Xil_Out32(XPAR_AXI_VDMA_0_BASEADDR + 0xA8, stride0*bytePerPixels);
    // S2MM HSIZE register
    Xil_Out32(XPAR_AXI_VDMA_0_BASEADDR + 0xA4, width0*bytePerPixels);
    // S2MM VSIZE register
    Xil_Out32(XPAR_AXI_VDMA_0_BASEADDR + 0xA0, height0);

    /* Configure the Read interface (MM2S)*/
    // MM2S Control Register
    Xil_Out32(XPAR_AXI_VDMA_0_BASEADDR + 0x00, 0x8B);
    // MM2S Start Address 1
    Xil_Out32(XPAR_AXI_VDMA_0_BASEADDR + 0x5C, FRAME_BUFFER_0 + offset1);
    // MM2S Start Address 2
    Xil_Out32(XPAR_AXI_VDMA_0_BASEADDR + 0x60, FRAME_BUFFER_1 + offset1);
    // MM2S Start Address 3
    Xil_Out32(XPAR_AXI_VDMA_0_BASEADDR + 0x64, FRAME_BUFFER_2 + offset1);
    // MM2S Frame delay / Stride register
    Xil_Out32(XPAR_AXI_VDMA_0_BASEADDR + 0x58, stride1*bytePerPixels);
    // MM2S HSIZE register
    Xil_Out32(XPAR_AXI_VDMA_0_BASEADDR + 0x54, width1*bytePerPixels);
    // MM2S VSIZE register
    Xil_Out32(XPAR_AXI_VDMA_0_BASEADDR + 0x50, height1);

//    bsp_printf("VDMA started!\r\n");
	/* End of VDMA Configuration */
}

void vdma_config_m64_0(void)
{
    /* Start of VDMA Configuration */
    u32 bytePerPixels = 3;

    int offset0 = 0; // (y*w+x)*Bpp
    int offset1 = 0; // (y*w+x)*Bpp

    u32 stride0 = 1920;
    u32 width0 = 1920;
    u32 height0 = 1080;
    u32 stride1 = 1920;  // crop keeps write Stride
    u32 width1 = 1920;
    u32 height1 = 1080;

    /* Configure the Write interface (S2MM)*/
    // S2MM Control Register
    Xil_Out32(XPAR_AXI_VDMA_0_BASEADDR + 0x30, 0x8B);
    //S2MM Start Address 1
    Xil_Out32(XPAR_AXI_VDMA_0_BASEADDR + 0xAC, FRAME_BUFFER_0 + offset0);
    Xil_Out32(XPAR_AXI_VDMA_0_BASEADDR + 0xB0, 0);
    //S2MM Start Address 2
    Xil_Out32(XPAR_AXI_VDMA_0_BASEADDR + 0xB4, FRAME_BUFFER_1 + offset0);
    Xil_Out32(XPAR_AXI_VDMA_0_BASEADDR + 0xB8, 0);
    //S2MM Start Address 3
    Xil_Out32(XPAR_AXI_VDMA_0_BASEADDR + 0xBC, FRAME_BUFFER_2 + offset0);
    Xil_Out32(XPAR_AXI_VDMA_0_BASEADDR + 0xC0, 0);
    //S2MM Frame delay / Stride register
    Xil_Out32(XPAR_AXI_VDMA_0_BASEADDR + 0xA8, stride0*bytePerPixels);
    // S2MM HSIZE register
    Xil_Out32(XPAR_AXI_VDMA_0_BASEADDR + 0xA4, width0*bytePerPixels);
    // S2MM VSIZE register
    Xil_Out32(XPAR_AXI_VDMA_0_BASEADDR + 0xA0, height0);

    /* Configure the Read interface (MM2S)*/
    // MM2S Control Register
    Xil_Out32(XPAR_AXI_VDMA_0_BASEADDR + 0x00, 0x8B);
    // MM2S Start Address 1
    Xil_Out32(XPAR_AXI_VDMA_0_BASEADDR + 0x5C, FRAME_BUFFER_0 + offset1);
    Xil_Out32(XPAR_AXI_VDMA_0_BASEADDR + 0x60, 0);
    // MM2S Start Address 2
    Xil_Out32(XPAR_AXI_VDMA_0_BASEADDR + 0x64, FRAME_BUFFER_1 + offset1);
    Xil_Out32(XPAR_AXI_VDMA_0_BASEADDR + 0x68, 0);
    // MM2S Start Address 3
    Xil_Out32(XPAR_AXI_VDMA_0_BASEADDR + 0x6C, FRAME_BUFFER_2 + offset1);
    Xil_Out32(XPAR_AXI_VDMA_0_BASEADDR + 0x70, 0);
    // MM2S Frame delay / Stride register
    Xil_Out32(XPAR_AXI_VDMA_0_BASEADDR + 0x58, stride1*bytePerPixels);
    // MM2S HSIZE register
    Xil_Out32(XPAR_AXI_VDMA_0_BASEADDR + 0x54, width1*bytePerPixels);
    // MM2S VSIZE register
    Xil_Out32(XPAR_AXI_VDMA_0_BASEADDR + 0x50, height1);

//    bsp_printf("VDMA started!\r\n");
    /* End of VDMA Configuration */
}
#endif
#if (XPAR_XAXIVDMA_NUM_INSTANCES >= 2U)
void vdma_config_m32_1(void)
{
    /* Start of VDMA Configuration */
    u32 bytePerPixels = 3;

    int offset0 = 0; // (y*w+x)*Bpp
    int offset1 = 0; // (y*w+x)*Bpp

    u32 stride0 = 1920;
    u32 width0 = 1920;
    u32 height0 = 1080;
    u32 stride1 = 1920;  // crop keeps write Stride
    u32 width1 = 1920;
    u32 height1 = 1080;

    /* Configure the Write interface (S2MM)*/
    // S2MM Control Register
    Xil_Out32(XPAR_AXI_VDMA_1_BASEADDR + 0x30, 0x8B);
    //S2MM Start Address 1
    Xil_Out32(XPAR_AXI_VDMA_1_BASEADDR + 0xAC, FRAME_BUFFER_3 + offset0);
    //S2MM Start Address 2
    Xil_Out32(XPAR_AXI_VDMA_1_BASEADDR + 0xB0, FRAME_BUFFER_4 + offset0);
    //S2MM Start Address 3
    Xil_Out32(XPAR_AXI_VDMA_1_BASEADDR + 0xB4, FRAME_BUFFER_5 + offset0);
    //S2MM Frame delay / Stride register
    Xil_Out32(XPAR_AXI_VDMA_1_BASEADDR + 0xA8, stride0*bytePerPixels);
    // S2MM HSIZE register
    Xil_Out32(XPAR_AXI_VDMA_1_BASEADDR + 0xA4, width0*bytePerPixels);
    // S2MM VSIZE register
    Xil_Out32(XPAR_AXI_VDMA_1_BASEADDR + 0xA0, height0);

    /* Configure the Read interface (MM2S)*/
    // MM2S Control Register
    Xil_Out32(XPAR_AXI_VDMA_1_BASEADDR + 0x00, 0x8B);
    // MM2S Start Address 1
    Xil_Out32(XPAR_AXI_VDMA_1_BASEADDR + 0x5C, FRAME_BUFFER_3 + offset1);
    // MM2S Start Address 2
    Xil_Out32(XPAR_AXI_VDMA_1_BASEADDR + 0x60, FRAME_BUFFER_4 + offset1);
    // MM2S Start Address 3
    Xil_Out32(XPAR_AXI_VDMA_1_BASEADDR + 0x64, FRAME_BUFFER_5 + offset1);
    // MM2S Frame delay / Stride register
    Xil_Out32(XPAR_AXI_VDMA_1_BASEADDR + 0x58, stride1*bytePerPixels);
    // MM2S HSIZE register
    Xil_Out32(XPAR_AXI_VDMA_1_BASEADDR + 0x54, width1*bytePerPixels);
    // MM2S VSIZE register
    Xil_Out32(XPAR_AXI_VDMA_1_BASEADDR + 0x50, height1);

//    bsp_printf("VDMA started!\r\n");
	/* End of VDMA Configuration */
}

void vdma_config_m64_1(void)
{
    /* Start of VDMA Configuration */
    u32 bytePerPixels = 3;

    int offset0 = 0; // (y*w+x)*Bpp
    int offset1 = 0; // (y*w+x)*Bpp

    u32 stride0 = 1920;
    u32 width0 = 1920;
    u32 height0 = 1080;
    u32 stride1 = 1920;  // crop keeps write Stride
    u32 width1 = 1920;
    u32 height1 = 1080;

    /* Configure the Write interface (S2MM)*/
    // S2MM Control Register
    Xil_Out32(XPAR_AXI_VDMA_1_BASEADDR + 0x30, 0x8B);
    //S2MM Start Address 1
    Xil_Out32(XPAR_AXI_VDMA_1_BASEADDR + 0xAC, FRAME_BUFFER_3 + offset0);
    Xil_Out32(XPAR_AXI_VDMA_1_BASEADDR + 0xB0, 0);
    //S2MM Start Address 2
    Xil_Out32(XPAR_AXI_VDMA_1_BASEADDR + 0xB4, FRAME_BUFFER_4 + offset0);
    Xil_Out32(XPAR_AXI_VDMA_1_BASEADDR + 0xB8, 0);
    //S2MM Start Address 3
    Xil_Out32(XPAR_AXI_VDMA_1_BASEADDR + 0xBC, FRAME_BUFFER_5 + offset0);
    Xil_Out32(XPAR_AXI_VDMA_1_BASEADDR + 0xC0, 0);
    //S2MM Frame delay / Stride register
    Xil_Out32(XPAR_AXI_VDMA_1_BASEADDR + 0xA8, stride0*bytePerPixels);
    // S2MM HSIZE register
    Xil_Out32(XPAR_AXI_VDMA_1_BASEADDR + 0xA4, width0*bytePerPixels);
    // S2MM VSIZE register
    Xil_Out32(XPAR_AXI_VDMA_1_BASEADDR + 0xA0, height0);

    /* Configure the Read interface (MM2S)*/
    // MM2S Control Register
    Xil_Out32(XPAR_AXI_VDMA_1_BASEADDR + 0x00, 0x8B);
    // MM2S Start Address 1
    Xil_Out32(XPAR_AXI_VDMA_1_BASEADDR + 0x5C, FRAME_BUFFER_3 + offset1);
    Xil_Out32(XPAR_AXI_VDMA_1_BASEADDR + 0x60, 0);
    // MM2S Start Address 2
    Xil_Out32(XPAR_AXI_VDMA_1_BASEADDR + 0x64, FRAME_BUFFER_4 + offset1);
    Xil_Out32(XPAR_AXI_VDMA_1_BASEADDR + 0x68, 0);
    // MM2S Start Address 3
    Xil_Out32(XPAR_AXI_VDMA_1_BASEADDR + 0x6C, FRAME_BUFFER_5 + offset1);
    Xil_Out32(XPAR_AXI_VDMA_1_BASEADDR + 0x70, 0);
    // MM2S Frame delay / Stride register
    Xil_Out32(XPAR_AXI_VDMA_1_BASEADDR + 0x58, stride1*bytePerPixels);
    // MM2S HSIZE register
    Xil_Out32(XPAR_AXI_VDMA_1_BASEADDR + 0x54, width1*bytePerPixels);
    // MM2S VSIZE register
    Xil_Out32(XPAR_AXI_VDMA_1_BASEADDR + 0x50, height1);

//    bsp_printf("VDMA started!\r\n");
    /* End of VDMA Configuration */
}
#endif
#if (XPAR_XAXIVDMA_NUM_INSTANCES >= 3U)
void vdma_config_m32_2(void)
{
    /* Start of VDMA Configuration */
    u32 bytePerPixels = 3;

    int offset0 = 0; // (y*w+x)*Bpp
    int offset1 = 0; // (y*w+x)*Bpp

    u32 stride0 = 1920;
    u32 width0 = 1920;
    u32 height0 = 1080;
    u32 stride1 = 1920;  // crop keeps write Stride
    u32 width1 = 1920;
    u32 height1 = 1080;

    /* Configure the Write interface (S2MM)*/
    // S2MM Control Register
    Xil_Out32(XPAR_AXI_VDMA_2_BASEADDR + 0x30, 0x8B);
    //S2MM Start Address 1
    Xil_Out32(XPAR_AXI_VDMA_2_BASEADDR + 0xAC, FRAME_BUFFER_6 + offset0);
    //S2MM Start Address 2
    Xil_Out32(XPAR_AXI_VDMA_2_BASEADDR + 0xB0, FRAME_BUFFER_7 + offset0);
    //S2MM Start Address 3
    Xil_Out32(XPAR_AXI_VDMA_2_BASEADDR + 0xB4, FRAME_BUFFER_8 + offset0);
    //S2MM Frame delay / Stride register
    Xil_Out32(XPAR_AXI_VDMA_2_BASEADDR + 0xA8, stride0*bytePerPixels);
    // S2MM HSIZE register
    Xil_Out32(XPAR_AXI_VDMA_2_BASEADDR + 0xA4, width0*bytePerPixels);
    // S2MM VSIZE register
    Xil_Out32(XPAR_AXI_VDMA_2_BASEADDR + 0xA0, height0);

    /* Configure the Read interface (MM2S)*/
    // MM2S Control Register
    Xil_Out32(XPAR_AXI_VDMA_2_BASEADDR + 0x00, 0x8B);
    // MM2S Start Address 1
    Xil_Out32(XPAR_AXI_VDMA_2_BASEADDR + 0x5C, FRAME_BUFFER_6 + offset1);
    // MM2S Start Address 2
    Xil_Out32(XPAR_AXI_VDMA_2_BASEADDR + 0x60, FRAME_BUFFER_7 + offset1);
    // MM2S Start Address 3
    Xil_Out32(XPAR_AXI_VDMA_2_BASEADDR + 0x64, FRAME_BUFFER_8 + offset1);
    // MM2S Frame delay / Stride register
    Xil_Out32(XPAR_AXI_VDMA_2_BASEADDR + 0x58, stride1*bytePerPixels);
    // MM2S HSIZE register
    Xil_Out32(XPAR_AXI_VDMA_2_BASEADDR + 0x54, width1*bytePerPixels);
    // MM2S VSIZE register
    Xil_Out32(XPAR_AXI_VDMA_2_BASEADDR + 0x50, height1);

//    bsp_printf("VDMA started!\r\n");
	/* End of VDMA Configuration */
}

void vdma_config_m64_2(void)
{
    /* Start of VDMA Configuration */
    u32 bytePerPixels = 3;

    int offset0 = 0; // (y*w+x)*Bpp
    int offset1 = 0; // (y*w+x)*Bpp

    u32 stride0 = 1920;
    u32 width0 = 1920;
    u32 height0 = 1080;
    u32 stride1 = 1920;  // crop keeps write Stride
    u32 width1 = 1920;
    u32 height1 = 1080;

    /* Configure the Write interface (S2MM)*/
    // S2MM Control Register
    Xil_Out32(XPAR_AXI_VDMA_2_BASEADDR + 0x30, 0x8B);
    //S2MM Start Address 1
    Xil_Out32(XPAR_AXI_VDMA_2_BASEADDR + 0xAC, FRAME_BUFFER_6 + offset0);
    Xil_Out32(XPAR_AXI_VDMA_2_BASEADDR + 0xB0, 0);
    //S2MM Start Address 2
    Xil_Out32(XPAR_AXI_VDMA_2_BASEADDR + 0xB4, FRAME_BUFFER_7 + offset0);
    Xil_Out32(XPAR_AXI_VDMA_2_BASEADDR + 0xB8, 0);
    //S2MM Start Address 3
    Xil_Out32(XPAR_AXI_VDMA_2_BASEADDR + 0xBC, FRAME_BUFFER_8 + offset0);
    Xil_Out32(XPAR_AXI_VDMA_2_BASEADDR + 0xC0, 0);
    //S2MM Frame delay / Stride register
    Xil_Out32(XPAR_AXI_VDMA_2_BASEADDR + 0xA8, stride0*bytePerPixels);
    // S2MM HSIZE register
    Xil_Out32(XPAR_AXI_VDMA_2_BASEADDR + 0xA4, width0*bytePerPixels);
    // S2MM VSIZE register
    Xil_Out32(XPAR_AXI_VDMA_2_BASEADDR + 0xA0, height0);

    /* Configure the Read interface (MM2S)*/
    // MM2S Control Register
    Xil_Out32(XPAR_AXI_VDMA_2_BASEADDR + 0x00, 0x8B);
    // MM2S Start Address 1
    Xil_Out32(XPAR_AXI_VDMA_2_BASEADDR + 0x5C, FRAME_BUFFER_6 + offset1);
    Xil_Out32(XPAR_AXI_VDMA_2_BASEADDR + 0x60, 0);
    // MM2S Start Address 2
    Xil_Out32(XPAR_AXI_VDMA_2_BASEADDR + 0x64, FRAME_BUFFER_7 + offset1);
    Xil_Out32(XPAR_AXI_VDMA_2_BASEADDR + 0x68, 0);
    // MM2S Start Address 3
    Xil_Out32(XPAR_AXI_VDMA_2_BASEADDR + 0x6C, FRAME_BUFFER_8 + offset1);
    Xil_Out32(XPAR_AXI_VDMA_2_BASEADDR + 0x70, 0);
    // MM2S Frame delay / Stride register
    Xil_Out32(XPAR_AXI_VDMA_2_BASEADDR + 0x58, stride1*bytePerPixels);
    // MM2S HSIZE register
    Xil_Out32(XPAR_AXI_VDMA_2_BASEADDR + 0x54, width1*bytePerPixels);
    // MM2S VSIZE register
    Xil_Out32(XPAR_AXI_VDMA_2_BASEADDR + 0x50, height1);

//    bsp_printf("VDMA started!\r\n");
    /* End of VDMA Configuration */
}
#endif
#if (XPAR_XAXIVDMA_NUM_INSTANCES >= 4U)
void vdma_config_m32_3(void)
{
    /* Start of VDMA Configuration */
    u32 bytePerPixels = 3;

    int offset0 = 0; // (y*w+x)*Bpp
    int offset1 = 0; // (y*w+x)*Bpp

    u32 stride0 = 1920;
    u32 width0 = 1920;
    u32 height0 = 1080;
    u32 stride1 = 1920;  // crop keeps write Stride
    u32 width1 = 1920;
    u32 height1 = 1080;

    /* Configure the Write interface (S2MM)*/
    // S2MM Control Register
    Xil_Out32(XPAR_AXI_VDMA_3_BASEADDR + 0x30, 0x8B);
    //S2MM Start Address 1
    Xil_Out32(XPAR_AXI_VDMA_3_BASEADDR + 0xAC, FRAME_BUFFER_9 + offset0);
    //S2MM Start Address 2
    Xil_Out32(XPAR_AXI_VDMA_3_BASEADDR + 0xB0, FRAME_BUFFER_10 + offset0);
    //S2MM Start Address 3
    Xil_Out32(XPAR_AXI_VDMA_3_BASEADDR + 0xB4, FRAME_BUFFER_11 + offset0);
    //S2MM Frame delay / Stride register
    Xil_Out32(XPAR_AXI_VDMA_3_BASEADDR + 0xA8, stride0*bytePerPixels);
    // S2MM HSIZE register
    Xil_Out32(XPAR_AXI_VDMA_3_BASEADDR + 0xA4, width0*bytePerPixels);
    // S2MM VSIZE register
    Xil_Out32(XPAR_AXI_VDMA_3_BASEADDR + 0xA0, height0);

    /* Configure the Read interface (MM2S)*/
    // MM2S Control Register
    Xil_Out32(XPAR_AXI_VDMA_3_BASEADDR + 0x00, 0x8B);
    // MM2S Start Address 1
    Xil_Out32(XPAR_AXI_VDMA_3_BASEADDR + 0x5C, FRAME_BUFFER_9 + offset1);
    // MM2S Start Address 2
    Xil_Out32(XPAR_AXI_VDMA_3_BASEADDR + 0x60, FRAME_BUFFER_10 + offset1);
    // MM2S Start Address 3
    Xil_Out32(XPAR_AXI_VDMA_3_BASEADDR + 0x64, FRAME_BUFFER_11 + offset1);
    // MM2S Frame delay / Stride register
    Xil_Out32(XPAR_AXI_VDMA_3_BASEADDR + 0x58, stride1*bytePerPixels);
    // MM2S HSIZE register
    Xil_Out32(XPAR_AXI_VDMA_3_BASEADDR + 0x54, width1*bytePerPixels);
    // MM2S VSIZE register
    Xil_Out32(XPAR_AXI_VDMA_3_BASEADDR + 0x50, height1);

//    bsp_printf("VDMA started!\r\n");
	/* End of VDMA Configuration */
}

void vdma_config_m64_3(void)
{
    /* Start of VDMA Configuration */
    u32 bytePerPixels = 3;

    int offset0 = 0; // (y*w+x)*Bpp
    int offset1 = 0; // (y*w+x)*Bpp

    u32 stride0 = 1920;
    u32 width0 = 1920;
    u32 height0 = 1080;
    u32 stride1 = 1920;  // crop keeps write Stride
    u32 width1 = 1920;
    u32 height1 = 1080;

    /* Configure the Write interface (S2MM)*/
    // S2MM Control Register
    Xil_Out32(XPAR_AXI_VDMA_3_BASEADDR + 0x30, 0x8B);
    //S2MM Start Address 1
    Xil_Out32(XPAR_AXI_VDMA_3_BASEADDR + 0xAC, FRAME_BUFFER_9 + offset0);
    Xil_Out32(XPAR_AXI_VDMA_3_BASEADDR + 0xB0, 0);
    //S2MM Start Address 2
    Xil_Out32(XPAR_AXI_VDMA_3_BASEADDR + 0xB4, FRAME_BUFFER_10 + offset0);
    Xil_Out32(XPAR_AXI_VDMA_3_BASEADDR + 0xB8, 0);
    //S2MM Start Address 3
    Xil_Out32(XPAR_AXI_VDMA_3_BASEADDR + 0xBC, FRAME_BUFFER_11 + offset0);
    Xil_Out32(XPAR_AXI_VDMA_3_BASEADDR + 0xC0, 0);
    //S2MM Frame delay / Stride register
    Xil_Out32(XPAR_AXI_VDMA_3_BASEADDR + 0xA8, stride0*bytePerPixels);
    // S2MM HSIZE register
    Xil_Out32(XPAR_AXI_VDMA_3_BASEADDR + 0xA4, width0*bytePerPixels);
    // S2MM VSIZE register
    Xil_Out32(XPAR_AXI_VDMA_3_BASEADDR + 0xA0, height0);

    /* Configure the Read interface (MM2S)*/
    // MM2S Control Register
    Xil_Out32(XPAR_AXI_VDMA_3_BASEADDR + 0x00, 0x8B);
    // MM2S Start Address 1
    Xil_Out32(XPAR_AXI_VDMA_3_BASEADDR + 0x5C, FRAME_BUFFER_9 + offset1);
    Xil_Out32(XPAR_AXI_VDMA_3_BASEADDR + 0x60, 0);
    // MM2S Start Address 2
    Xil_Out32(XPAR_AXI_VDMA_3_BASEADDR + 0x64, FRAME_BUFFER_10 + offset1);
    Xil_Out32(XPAR_AXI_VDMA_3_BASEADDR + 0x68, 0);
    // MM2S Start Address 3
    Xil_Out32(XPAR_AXI_VDMA_3_BASEADDR + 0x6C, FRAME_BUFFER_11 + offset1);
    Xil_Out32(XPAR_AXI_VDMA_3_BASEADDR + 0x70, 0);
    // MM2S Frame delay / Stride register
    Xil_Out32(XPAR_AXI_VDMA_3_BASEADDR + 0x58, stride1*bytePerPixels);
    // MM2S HSIZE register
    Xil_Out32(XPAR_AXI_VDMA_3_BASEADDR + 0x54, width1*bytePerPixels);
    // MM2S VSIZE register
    Xil_Out32(XPAR_AXI_VDMA_3_BASEADDR + 0x50, height1);

//    bsp_printf("VDMA started!\r\n");
    /* End of VDMA Configuration */
}
#endif
#if (XPAR_XAXIVDMA_NUM_INSTANCES >= 5U)
void vdma_config_m32_4(void)
{
    /* Start of VDMA Configuration */
    u32 bytePerPixels = 3;

    int offset0 = 0; // (y*w+x)*Bpp
    int offset1 = 0; // (y*w+x)*Bpp

    u32 stride0 = 1920;
    u32 width0 = 1920;
    u32 height0 = 1080;
    u32 stride1 = 1920;  // crop keeps write Stride
    u32 width1 = 1920;
    u32 height1 = 1080;

    /* Configure the Write interface (S2MM)*/
    // S2MM Control Register
    Xil_Out32(XPAR_AXI_VDMA_4_BASEADDR + 0x30, 0x8B);
    //S2MM Start Address 1
    Xil_Out32(XPAR_AXI_VDMA_4_BASEADDR + 0xAC, FRAME_BUFFER_12 + offset0);
    //S2MM Start Address 2
    Xil_Out32(XPAR_AXI_VDMA_4_BASEADDR + 0xB0, FRAME_BUFFER_13 + offset0);
    //S2MM Start Address 3
    Xil_Out32(XPAR_AXI_VDMA_4_BASEADDR + 0xB4, FRAME_BUFFER_14 + offset0);
    //S2MM Frame delay / Stride register
    Xil_Out32(XPAR_AXI_VDMA_4_BASEADDR + 0xA8, stride0*bytePerPixels);
    // S2MM HSIZE register
    Xil_Out32(XPAR_AXI_VDMA_4_BASEADDR + 0xA4, width0*bytePerPixels);
    // S2MM VSIZE register
    Xil_Out32(XPAR_AXI_VDMA_4_BASEADDR + 0xA0, height0);

    /* Configure the Read interface (MM2S)*/
    // MM2S Control Register
    Xil_Out32(XPAR_AXI_VDMA_4_BASEADDR + 0x00, 0x8B);
    // MM2S Start Address 1
    Xil_Out32(XPAR_AXI_VDMA_4_BASEADDR + 0x5C, FRAME_BUFFER_12 + offset1);
    // MM2S Start Address 2
    Xil_Out32(XPAR_AXI_VDMA_4_BASEADDR + 0x60, FRAME_BUFFER_13 + offset1);
    // MM2S Start Address 3
    Xil_Out32(XPAR_AXI_VDMA_4_BASEADDR + 0x64, FRAME_BUFFER_14 + offset1);
    // MM2S Frame delay / Stride register
    Xil_Out32(XPAR_AXI_VDMA_4_BASEADDR + 0x58, stride1*bytePerPixels);
    // MM2S HSIZE register
    Xil_Out32(XPAR_AXI_VDMA_4_BASEADDR + 0x54, width1*bytePerPixels);
    // MM2S VSIZE register
    Xil_Out32(XPAR_AXI_VDMA_4_BASEADDR + 0x50, height1);

//    bsp_printf("VDMA started!\r\n");
	/* End of VDMA Configuration */
}

void vdma_config_m64_4(void)
{
    /* Start of VDMA Configuration */
    u32 bytePerPixels = 3;

    int offset0 = 0; // (y*w+x)*Bpp
    int offset1 = 0; // (y*w+x)*Bpp

    u32 stride0 = 1920;
    u32 width0 = 1920;
    u32 height0 = 1080;
    u32 stride1 = 1920;  // crop keeps write Stride
    u32 width1 = 1920;
    u32 height1 = 1080;

    /* Configure the Write interface (S2MM)*/
    // S2MM Control Register
    Xil_Out32(XPAR_AXI_VDMA_4_BASEADDR + 0x30, 0x8B);
    //S2MM Start Address 1
    Xil_Out32(XPAR_AXI_VDMA_4_BASEADDR + 0xAC, FRAME_BUFFER_12 + offset0);
    Xil_Out32(XPAR_AXI_VDMA_4_BASEADDR + 0xB0, 0);
    //S2MM Start Address 2
    Xil_Out32(XPAR_AXI_VDMA_4_BASEADDR + 0xB4, FRAME_BUFFER_13 + offset0);
    Xil_Out32(XPAR_AXI_VDMA_4_BASEADDR + 0xB8, 0);
    //S2MM Start Address 3
    Xil_Out32(XPAR_AXI_VDMA_4_BASEADDR + 0xBC, FRAME_BUFFER_14 + offset0);
    Xil_Out32(XPAR_AXI_VDMA_4_BASEADDR + 0xC0, 0);
    //S2MM Frame delay / Stride register
    Xil_Out32(XPAR_AXI_VDMA_4_BASEADDR + 0xA8, stride0*bytePerPixels);
    // S2MM HSIZE register
    Xil_Out32(XPAR_AXI_VDMA_4_BASEADDR + 0xA4, width0*bytePerPixels);
    // S2MM VSIZE register
    Xil_Out32(XPAR_AXI_VDMA_4_BASEADDR + 0xA0, height0);

    /* Configure the Read interface (MM2S)*/
    // MM2S Control Register
    Xil_Out32(XPAR_AXI_VDMA_4_BASEADDR + 0x00, 0x8B);
    // MM2S Start Address 1
    Xil_Out32(XPAR_AXI_VDMA_4_BASEADDR + 0x5C, FRAME_BUFFER_12 + offset1);
    Xil_Out32(XPAR_AXI_VDMA_4_BASEADDR + 0x60, 0);
    // MM2S Start Address 2
    Xil_Out32(XPAR_AXI_VDMA_4_BASEADDR + 0x64, FRAME_BUFFER_13 + offset1);
    Xil_Out32(XPAR_AXI_VDMA_4_BASEADDR + 0x68, 0);
    // MM2S Start Address 3
    Xil_Out32(XPAR_AXI_VDMA_4_BASEADDR + 0x6C, FRAME_BUFFER_14 + offset1);
    Xil_Out32(XPAR_AXI_VDMA_4_BASEADDR + 0x70, 0);
    // MM2S Frame delay / Stride register
    Xil_Out32(XPAR_AXI_VDMA_4_BASEADDR + 0x58, stride1*bytePerPixels);
    // MM2S HSIZE register
    Xil_Out32(XPAR_AXI_VDMA_4_BASEADDR + 0x54, width1*bytePerPixels);
    // MM2S VSIZE register
    Xil_Out32(XPAR_AXI_VDMA_4_BASEADDR + 0x50, height1);

//    bsp_printf("VDMA started!\r\n");
    /* End of VDMA Configuration */
}
#endif
#if (XPAR_XAXIVDMA_NUM_INSTANCES >= 6U)
void vdma_config_m32_5(void)
{
    /* Start of VDMA Configuration */
    u32 bytePerPixels = 3;

    int offset0 = 0; // (y*w+x)*Bpp
    int offset1 = 0; // (y*w+x)*Bpp

    u32 stride0 = 1920;
    u32 width0 = 1920;
    u32 height0 = 1080;
    u32 stride1 = 1920;  // crop keeps write Stride
    u32 width1 = 1920;
    u32 height1 = 1080;

    /* Configure the Write interface (S2MM)*/
    // S2MM Control Register
    Xil_Out32(XPAR_AXI_VDMA_5_BASEADDR + 0x30, 0x8B);
    //S2MM Start Address 1
    Xil_Out32(XPAR_AXI_VDMA_5_BASEADDR + 0xAC, FRAME_BUFFER_15 + offset0);
    //S2MM Start Address 2
    Xil_Out32(XPAR_AXI_VDMA_5_BASEADDR + 0xB0, FRAME_BUFFER_16 + offset0);
    //S2MM Start Address 3
    Xil_Out32(XPAR_AXI_VDMA_5_BASEADDR + 0xB4, FRAME_BUFFER_17 + offset0);
    //S2MM Frame delay / Stride register
    Xil_Out32(XPAR_AXI_VDMA_5_BASEADDR + 0xA8, stride0*bytePerPixels);
    // S2MM HSIZE register
    Xil_Out32(XPAR_AXI_VDMA_5_BASEADDR + 0xA4, width0*bytePerPixels);
    // S2MM VSIZE register
    Xil_Out32(XPAR_AXI_VDMA_5_BASEADDR + 0xA0, height0);

    /* Configure the Read interface (MM2S)*/
    // MM2S Control Register
    Xil_Out32(XPAR_AXI_VDMA_5_BASEADDR + 0x00, 0x8B);
    // MM2S Start Address 1
    Xil_Out32(XPAR_AXI_VDMA_5_BASEADDR + 0x5C, FRAME_BUFFER_15 + offset1);
    // MM2S Start Address 2
    Xil_Out32(XPAR_AXI_VDMA_5_BASEADDR + 0x60, FRAME_BUFFER_16 + offset1);
    // MM2S Start Address 3
    Xil_Out32(XPAR_AXI_VDMA_5_BASEADDR + 0x64, FRAME_BUFFER_17 + offset1);
    // MM2S Frame delay / Stride register
    Xil_Out32(XPAR_AXI_VDMA_5_BASEADDR + 0x58, stride1*bytePerPixels);
    // MM2S HSIZE register
    Xil_Out32(XPAR_AXI_VDMA_5_BASEADDR + 0x54, width1*bytePerPixels);
    // MM2S VSIZE register
    Xil_Out32(XPAR_AXI_VDMA_5_BASEADDR + 0x50, height1);

//    bsp_printf("VDMA started!\r\n");
	/* End of VDMA Configuration */
}

void vdma_config_m64_5(void)
{
    /* Start of VDMA Configuration */
    u32 bytePerPixels = 3;

    int offset0 = 0; // (y*w+x)*Bpp
    int offset1 = 0; // (y*w+x)*Bpp

    u32 stride0 = 1920;
    u32 width0 = 1920;
    u32 height0 = 1080;
    u32 stride1 = 1920;  // crop keeps write Stride
    u32 width1 = 1920;
    u32 height1 = 1080;

    /* Configure the Write interface (S2MM)*/
    // S2MM Control Register
    Xil_Out32(XPAR_AXI_VDMA_5_BASEADDR + 0x30, 0x8B);
    //S2MM Start Address 1
    Xil_Out32(XPAR_AXI_VDMA_5_BASEADDR + 0xAC, FRAME_BUFFER_15 + offset0);
    Xil_Out32(XPAR_AXI_VDMA_5_BASEADDR + 0xB0, 0);
    //S2MM Start Address 2
    Xil_Out32(XPAR_AXI_VDMA_5_BASEADDR + 0xB4, FRAME_BUFFER_16 + offset0);
    Xil_Out32(XPAR_AXI_VDMA_5_BASEADDR + 0xB8, 0);
    //S2MM Start Address 3
    Xil_Out32(XPAR_AXI_VDMA_5_BASEADDR + 0xBC, FRAME_BUFFER_17 + offset0);
    Xil_Out32(XPAR_AXI_VDMA_5_BASEADDR + 0xC0, 0);
    //S2MM Frame delay / Stride register
    Xil_Out32(XPAR_AXI_VDMA_5_BASEADDR + 0xA8, stride0*bytePerPixels);
    // S2MM HSIZE register
    Xil_Out32(XPAR_AXI_VDMA_5_BASEADDR + 0xA4, width0*bytePerPixels);
    // S2MM VSIZE register
    Xil_Out32(XPAR_AXI_VDMA_5_BASEADDR + 0xA0, height0);

    /* Configure the Read interface (MM2S)*/
    // MM2S Control Register
    Xil_Out32(XPAR_AXI_VDMA_5_BASEADDR + 0x00, 0x8B);
    // MM2S Start Address 1
    Xil_Out32(XPAR_AXI_VDMA_5_BASEADDR + 0x5C, FRAME_BUFFER_15 + offset1);
    Xil_Out32(XPAR_AXI_VDMA_5_BASEADDR + 0x60, 0);
    // MM2S Start Address 2
    Xil_Out32(XPAR_AXI_VDMA_5_BASEADDR + 0x64, FRAME_BUFFER_16 + offset1);
    Xil_Out32(XPAR_AXI_VDMA_5_BASEADDR + 0x68, 0);
    // MM2S Start Address 3
    Xil_Out32(XPAR_AXI_VDMA_5_BASEADDR + 0x6C, FRAME_BUFFER_17 + offset1);
    Xil_Out32(XPAR_AXI_VDMA_5_BASEADDR + 0x70, 0);
    // MM2S Frame delay / Stride register
    Xil_Out32(XPAR_AXI_VDMA_5_BASEADDR + 0x58, stride1*bytePerPixels);
    // MM2S HSIZE register
    Xil_Out32(XPAR_AXI_VDMA_5_BASEADDR + 0x54, width1*bytePerPixels);
    // MM2S VSIZE register
    Xil_Out32(XPAR_AXI_VDMA_5_BASEADDR + 0x50, height1);

//    bsp_printf("VDMA started!\r\n");
    /* End of VDMA Configuration */
}
#endif
#if (XPAR_XAXIVDMA_NUM_INSTANCES >= 7U)
void vdma_config_m32_6(void)
{
    /* Start of VDMA Configuration */
    u32 bytePerPixels = 3;

    int offset0 = 0; // (y*w+x)*Bpp
    int offset1 = 0; // (y*w+x)*Bpp

    u32 stride0 = 1920;
    u32 width0 = 1920;
    u32 height0 = 1080;
    u32 stride1 = 1920;  // crop keeps write Stride
    u32 width1 = 1920;
    u32 height1 = 1080;

    /* Configure the Write interface (S2MM)*/
    // S2MM Control Register
    Xil_Out32(XPAR_AXI_VDMA_6_BASEADDR + 0x30, 0x8B);
    //S2MM Start Address 1
    Xil_Out32(XPAR_AXI_VDMA_6_BASEADDR + 0xAC, FRAME_BUFFER_18 + offset0);
    //S2MM Start Address 2
    Xil_Out32(XPAR_AXI_VDMA_6_BASEADDR + 0xB0, FRAME_BUFFER_19 + offset0);
    //S2MM Start Address 3
    Xil_Out32(XPAR_AXI_VDMA_6_BASEADDR + 0xB4, FRAME_BUFFER_20 + offset0);
    //S2MM Frame delay / Stride register
    Xil_Out32(XPAR_AXI_VDMA_6_BASEADDR + 0xA8, stride0*bytePerPixels);
    // S2MM HSIZE register
    Xil_Out32(XPAR_AXI_VDMA_6_BASEADDR + 0xA4, width0*bytePerPixels);
    // S2MM VSIZE register
    Xil_Out32(XPAR_AXI_VDMA_6_BASEADDR + 0xA0, height0);

    /* Configure the Read interface (MM2S)*/
    // MM2S Control Register
    Xil_Out32(XPAR_AXI_VDMA_6_BASEADDR + 0x00, 0x8B);
    // MM2S Start Address 1
    Xil_Out32(XPAR_AXI_VDMA_6_BASEADDR + 0x5C, FRAME_BUFFER_18 + offset1);
    // MM2S Start Address 2
    Xil_Out32(XPAR_AXI_VDMA_6_BASEADDR + 0x60, FRAME_BUFFER_19 + offset1);
    // MM2S Start Address 3
    Xil_Out32(XPAR_AXI_VDMA_6_BASEADDR + 0x64, FRAME_BUFFER_20 + offset1);
    // MM2S Frame delay / Stride register
    Xil_Out32(XPAR_AXI_VDMA_6_BASEADDR + 0x58, stride1*bytePerPixels);
    // MM2S HSIZE register
    Xil_Out32(XPAR_AXI_VDMA_6_BASEADDR + 0x54, width1*bytePerPixels);
    // MM2S VSIZE register
    Xil_Out32(XPAR_AXI_VDMA_6_BASEADDR + 0x50, height1);

//    bsp_printf("VDMA started!\r\n");
	/* End of VDMA Configuration */
}

void vdma_config_m64_6(void)
{
    /* Start of VDMA Configuration */
    u32 bytePerPixels = 3;

    int offset0 = 0; // (y*w+x)*Bpp
    int offset1 = 0; // (y*w+x)*Bpp

    u32 stride0 = 1920;
    u32 width0 = 1920;
    u32 height0 = 1080;
    u32 stride1 = 1920;  // crop keeps write Stride
    u32 width1 = 1920;
    u32 height1 = 1080;

    /* Configure the Write interface (S2MM)*/
    // S2MM Control Register
    Xil_Out32(XPAR_AXI_VDMA_6_BASEADDR + 0x30, 0x8B);
    //S2MM Start Address 1
    Xil_Out32(XPAR_AXI_VDMA_6_BASEADDR + 0xAC, FRAME_BUFFER_18 + offset0);
    Xil_Out32(XPAR_AXI_VDMA_6_BASEADDR + 0xB0, 0);
    //S2MM Start Address 2
    Xil_Out32(XPAR_AXI_VDMA_6_BASEADDR + 0xB4, FRAME_BUFFER_19 + offset0);
    Xil_Out32(XPAR_AXI_VDMA_6_BASEADDR + 0xB8, 0);
    //S2MM Start Address 3
    Xil_Out32(XPAR_AXI_VDMA_6_BASEADDR + 0xBC, FRAME_BUFFER_20 + offset0);
    Xil_Out32(XPAR_AXI_VDMA_6_BASEADDR + 0xC0, 0);
    //S2MM Frame delay / Stride register
    Xil_Out32(XPAR_AXI_VDMA_6_BASEADDR + 0xA8, stride0*bytePerPixels);
    // S2MM HSIZE register
    Xil_Out32(XPAR_AXI_VDMA_6_BASEADDR + 0xA4, width0*bytePerPixels);
    // S2MM VSIZE register
    Xil_Out32(XPAR_AXI_VDMA_6_BASEADDR + 0xA0, height0);

    /* Configure the Read interface (MM2S)*/
    // MM2S Control Register
    Xil_Out32(XPAR_AXI_VDMA_6_BASEADDR + 0x00, 0x8B);
    // MM2S Start Address 1
    Xil_Out32(XPAR_AXI_VDMA_6_BASEADDR + 0x5C, FRAME_BUFFER_18 + offset1);
    Xil_Out32(XPAR_AXI_VDMA_6_BASEADDR + 0x60, 0);
    // MM2S Start Address 2
    Xil_Out32(XPAR_AXI_VDMA_6_BASEADDR + 0x64, FRAME_BUFFER_19 + offset1);
    Xil_Out32(XPAR_AXI_VDMA_6_BASEADDR + 0x68, 0);
    // MM2S Start Address 3
    Xil_Out32(XPAR_AXI_VDMA_6_BASEADDR + 0x6C, FRAME_BUFFER_20 + offset1);
    Xil_Out32(XPAR_AXI_VDMA_6_BASEADDR + 0x70, 0);
    // MM2S Frame delay / Stride register
    Xil_Out32(XPAR_AXI_VDMA_6_BASEADDR + 0x58, stride1*bytePerPixels);
    // MM2S HSIZE register
    Xil_Out32(XPAR_AXI_VDMA_6_BASEADDR + 0x54, width1*bytePerPixels);
    // MM2S VSIZE register
    Xil_Out32(XPAR_AXI_VDMA_6_BASEADDR + 0x50, height1);

//    bsp_printf("VDMA started!\r\n");
    /* End of VDMA Configuration */
}
#endif
#if (XPAR_XAXIVDMA_NUM_INSTANCES >= 8U)
void vdma_config_m32_7(void)
{
    /* Start of VDMA Configuration */
    u32 bytePerPixels = 3;

    int offset0 = 0; // (y*w+x)*Bpp
    int offset1 = 0; // (y*w+x)*Bpp

    u32 stride0 = 1920;
    u32 width0 = 1920;
    u32 height0 = 1080;
    u32 stride1 = 1920;  // crop keeps write Stride
    u32 width1 = 1920;
    u32 height1 = 1080;

    /* Configure the Write interface (S2MM)*/
    // S2MM Control Register
    Xil_Out32(XPAR_AXI_VDMA_7_BASEADDR + 0x30, 0x8B);
    //S2MM Start Address 1
    Xil_Out32(XPAR_AXI_VDMA_7_BASEADDR + 0xAC, FRAME_BUFFER_21 + offset0);
    //S2MM Start Address 2
    Xil_Out32(XPAR_AXI_VDMA_7_BASEADDR + 0xB0, FRAME_BUFFER_22 + offset0);
    //S2MM Start Address 3
    Xil_Out32(XPAR_AXI_VDMA_7_BASEADDR + 0xB4, FRAME_BUFFER_23 + offset0);
    //S2MM Frame delay / Stride register
    Xil_Out32(XPAR_AXI_VDMA_7_BASEADDR + 0xA8, stride0*bytePerPixels);
    // S2MM HSIZE register
    Xil_Out32(XPAR_AXI_VDMA_7_BASEADDR + 0xA4, width0*bytePerPixels);
    // S2MM VSIZE register
    Xil_Out32(XPAR_AXI_VDMA_7_BASEADDR + 0xA0, height0);

    /* Configure the Read interface (MM2S)*/
    // MM2S Control Register
    Xil_Out32(XPAR_AXI_VDMA_7_BASEADDR + 0x00, 0x8B);
    // MM2S Start Address 1
    Xil_Out32(XPAR_AXI_VDMA_7_BASEADDR + 0x5C, FRAME_BUFFER_21 + offset1);
    // MM2S Start Address 2
    Xil_Out32(XPAR_AXI_VDMA_7_BASEADDR + 0x60, FRAME_BUFFER_22 + offset1);
    // MM2S Start Address 3
    Xil_Out32(XPAR_AXI_VDMA_7_BASEADDR + 0x64, FRAME_BUFFER_23 + offset1);
    // MM2S Frame delay / Stride register
    Xil_Out32(XPAR_AXI_VDMA_7_BASEADDR + 0x58, stride1*bytePerPixels);
    // MM2S HSIZE register
    Xil_Out32(XPAR_AXI_VDMA_7_BASEADDR + 0x54, width1*bytePerPixels);
    // MM2S VSIZE register
    Xil_Out32(XPAR_AXI_VDMA_7_BASEADDR + 0x50, height1);

//    bsp_printf("VDMA started!\r\n");
	/* End of VDMA Configuration */
}

void vdma_config_m64_7(void)
{
    /* Start of VDMA Configuration */
    u32 bytePerPixels = 3;

    int offset0 = 0; // (y*w+x)*Bpp
    int offset1 = 0; // (y*w+x)*Bpp

    u32 stride0 = 1920;
    u32 width0 = 1920;
    u32 height0 = 1080;
    u32 stride1 = 1920;  // crop keeps write Stride
    u32 width1 = 1920;
    u32 height1 = 1080;

    /* Configure the Write interface (S2MM)*/
    // S2MM Control Register
    Xil_Out32(XPAR_AXI_VDMA_7_BASEADDR + 0x30, 0x8B);
    //S2MM Start Address 1
    Xil_Out32(XPAR_AXI_VDMA_7_BASEADDR + 0xAC, FRAME_BUFFER_21 + offset0);
    Xil_Out32(XPAR_AXI_VDMA_7_BASEADDR + 0xB0, 0);
    //S2MM Start Address 2
    Xil_Out32(XPAR_AXI_VDMA_7_BASEADDR + 0xB4, FRAME_BUFFER_22 + offset0);
    Xil_Out32(XPAR_AXI_VDMA_7_BASEADDR + 0xB8, 0);
    //S2MM Start Address 3
    Xil_Out32(XPAR_AXI_VDMA_7_BASEADDR + 0xBC, FRAME_BUFFER_23 + offset0);
    Xil_Out32(XPAR_AXI_VDMA_7_BASEADDR + 0xC0, 0);
    //S2MM Frame delay / Stride register
    Xil_Out32(XPAR_AXI_VDMA_7_BASEADDR + 0xA8, stride0*bytePerPixels);
    // S2MM HSIZE register
    Xil_Out32(XPAR_AXI_VDMA_7_BASEADDR + 0xA4, width0*bytePerPixels);
    // S2MM VSIZE register
    Xil_Out32(XPAR_AXI_VDMA_7_BASEADDR + 0xA0, height0);

    /* Configure the Read interface (MM2S)*/
    // MM2S Control Register
    Xil_Out32(XPAR_AXI_VDMA_7_BASEADDR + 0x00, 0x8B);
    // MM2S Start Address 1
    Xil_Out32(XPAR_AXI_VDMA_7_BASEADDR + 0x5C, FRAME_BUFFER_21 + offset1);
    Xil_Out32(XPAR_AXI_VDMA_7_BASEADDR + 0x60, 0);
    // MM2S Start Address 2
    Xil_Out32(XPAR_AXI_VDMA_7_BASEADDR + 0x64, FRAME_BUFFER_22 + offset1);
    Xil_Out32(XPAR_AXI_VDMA_7_BASEADDR + 0x68, 0);
    // MM2S Start Address 3
    Xil_Out32(XPAR_AXI_VDMA_7_BASEADDR + 0x6C, FRAME_BUFFER_23 + offset1);
    Xil_Out32(XPAR_AXI_VDMA_7_BASEADDR + 0x70, 0);
    // MM2S Frame delay / Stride register
    Xil_Out32(XPAR_AXI_VDMA_7_BASEADDR + 0x58, stride1*bytePerPixels);
    // MM2S HSIZE register
    Xil_Out32(XPAR_AXI_VDMA_7_BASEADDR + 0x54, width1*bytePerPixels);
    // MM2S VSIZE register
    Xil_Out32(XPAR_AXI_VDMA_7_BASEADDR + 0x50, height1);

//    bsp_printf("VDMA started!\r\n");
    /* End of VDMA Configuration */
}
#endif


#endif // XPAR_XAXIVDMA_NUM_INSTANCES
