#ifndef __VDMA_H__

//#include "xparameters.h"
#if defined (XPAR_XAXIVDMA_NUM_INSTANCES)
#define __VDMA_H__
#include "xaxivdma.h"

#if (XPAR_XAXIVDMA_NUM_INSTANCES >= 1U)
extern XAxiVdma Vdma0;
#endif
#if (XPAR_XAXIVDMA_NUM_INSTANCES >= 2U)
extern XAxiVdma Vdma1;
#endif
#if (XPAR_XAXIVDMA_NUM_INSTANCES >= 3U)
extern XAxiVdma Vdma2;
#endif
#if (XPAR_XAXIVDMA_NUM_INSTANCES >= 4U)
extern XAxiVdma Vdma3;
#endif
#if (XPAR_XAXIVDMA_NUM_INSTANCES >= 5U)
extern XAxiVdma Vdma4;
#endif
#if (XPAR_XAXIVDMA_NUM_INSTANCES >= 6U)
extern XAxiVdma Vdma5;
#endif
#if (XPAR_XAXIVDMA_NUM_INSTANCES >= 7U)
extern XAxiVdma Vdma6;
#endif
#if (XPAR_XAXIVDMA_NUM_INSTANCES >= 8U)
extern XAxiVdma Vdma7;
#endif

int vdma_init(XAxiVdma *InstancePtr, u16 DeviceID);
int vdma_read_start(XAxiVdma *InstancePtr);
void vdma_read_stop(XAxiVdma *InstancePtr);
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
);
int vdma_write_start(XAxiVdma *InstancePtr);
void vdma_write_stop(XAxiVdma *InstancePtr);
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
);

#if (XPAR_XAXIVDMA_NUM_INSTANCES >= 1U)
void vdma_config_m32_0(void);
void vdma_config_m64_0(void);
#endif
#if (XPAR_XAXIVDMA_NUM_INSTANCES >= 2U)
void vdma_config_m32_1(void);
void vdma_config_m64_1(void);
#endif
#if (XPAR_XAXIVDMA_NUM_INSTANCES >= 3U)
void vdma_config_m32_2(void);
void vdma_config_m64_2(void);
#endif
#if (XPAR_XAXIVDMA_NUM_INSTANCES >= 4U)
void vdma_config_m32_3(void);
void vdma_config_m64_3(void);
#endif
#if (XPAR_XAXIVDMA_NUM_INSTANCES >= 5U)
void vdma_config_m32_4(void);
void vdma_config_m64_4(void);
#endif
#if (XPAR_XAXIVDMA_NUM_INSTANCES >= 6U)
void vdma_config_m32_5(void);
void vdma_config_m64_5(void);
#endif
#if (XPAR_XAXIVDMA_NUM_INSTANCES >= 7U)
void vdma_config_m32_6(void);
void vdma_config_m64_6(void);
#endif
#if (XPAR_XAXIVDMA_NUM_INSTANCES >= 8U)
void vdma_config_m32_7(void);
void vdma_config_m64_7(void);
#endif

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
);
int vdma_config(void);

void vdma_config_m32(void);
void vdma_config_m64(void);

#endif // __VDMA_H__

#endif // XPAR_XAXIVDMA_NUM_INSTANCES

