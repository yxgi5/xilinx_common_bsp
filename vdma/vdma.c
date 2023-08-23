#include "../bsp.h"

#ifdef XPAR_XAXIVDMA_NUM_INSTANCES

#define DDR_BASEADDR XPAR_DDR_MEM_BASEADDR

#define FRAME_BUFFER_BASE_ADDR  	(DDR_BASEADDR + (0x10000000))

//#define FRAME_BUFFER_SIZE0      0x2000000    //0x2000000 for max 4KW RGB888 8bpc
#define FRAME_BUFFER_SIZE0      0x600000    //0x600000 for max 1080p RGB888 8bpc

#define FRAME_BUFFER_0          FRAME_BUFFER_BASE_ADDR
#define FRAME_BUFFER_1          FRAME_BUFFER_BASE_ADDR + FRAME_BUFFER_SIZE0
#define FRAME_BUFFER_2          FRAME_BUFFER_BASE_ADDR + (FRAME_BUFFER_SIZE0*2)
#define FRAME_BUFFER_3          FRAME_BUFFER_BASE_ADDR + (FRAME_BUFFER_SIZE0*3)
#define FRAME_BUFFER_4          FRAME_BUFFER_BASE_ADDR + (FRAME_BUFFER_SIZE0*4)
#define FRAME_BUFFER_5          FRAME_BUFFER_BASE_ADDR + (FRAME_BUFFER_SIZE0*5)
#define FRAME_BUFFER_6          FRAME_BUFFER_BASE_ADDR + (FRAME_BUFFER_SIZE0*6)
#define FRAME_BUFFER_7          FRAME_BUFFER_BASE_ADDR + (FRAME_BUFFER_SIZE0*7)
#define FRAME_BUFFER_8          FRAME_BUFFER_BASE_ADDR + (FRAME_BUFFER_SIZE0*8)
#define FRAME_BUFFER_9         FRAME_BUFFER_BASE_ADDR + (FRAME_BUFFER_SIZE0*9)
#define FRAME_BUFFER_10         FRAME_BUFFER_BASE_ADDR + (FRAME_BUFFER_SIZE0*10)
#define FRAME_BUFFER_11         FRAME_BUFFER_BASE_ADDR + (FRAME_BUFFER_SIZE0*11)

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


#endif // XPAR_XAXIVDMA_NUM_INSTANCES
