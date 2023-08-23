#ifndef __VDMA_H__
#define __VDMA_H__

//#include "xparameters.h"
#if defined (XPAR_XAXIVDMA_NUM_INSTANCES)

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

#endif // XPAR_XAXIVDMA_NUM_INSTANCES

#endif // __VDMA_H__
