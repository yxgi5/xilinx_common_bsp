#ifndef __CSI_RX_H__

//#include "xparameters.h"
#if defined (XPAR_XCSI_NUM_INSTANCES)
#define __CSI_RX_H__
#include "xcsi.h"

#if (XPAR_XCSI_NUM_INSTANCES >= 1U)
extern XCsi Csi2RxSsInst_0;
#endif
#if (XPAR_XCSI_NUM_INSTANCES >= 2U)
extern XCsi Csi2RxSsInst_1;
#endif
#if (XPAR_XCSI_NUM_INSTANCES >= 3U)
extern XCsi Csi2RxSsInst_2;
#endif
#if (XPAR_XCSI_NUM_INSTANCES >= 4U)
extern XCsi Csi2RxSsInst_3;
#endif

int Csi2RxSs_Init(XCsi *InstancePtr, u32 DeviceId);
int csi_tx_config(void);
void print_csi_rx_reg_info(XCsi *InstancePtr, u8 verbose);

#endif // __CSI_RX_H__

#endif // XPAR_XCSI_NUM_INSTANCES

