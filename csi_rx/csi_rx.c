#include "../bsp.h"
#if defined (XPAR_XCSI_NUM_INSTANCES)

#if (XPAR_XCSI_NUM_INSTANCES >= 1U)
XCsiSs Csi2RxSsInst_0;
#endif
#if (XPAR_XCSI_NUM_INSTANCES >= 2U)
XCsiSs Csi2RxSsInst_1;
#endif
#if (XPAR_XCSI_NUM_INSTANCES >= 3U)
XCsiSs Csi2RxSsInst_2;
#endif
#if (XPAR_XCSI_NUM_INSTANCES >= 4U)
XCsiSs Csi2RxSsInst_3;
#endif

int Csi2RxSs_Init(XCsiSs *InstancePtr, u32 DeviceId)
{
	int Status;
	XCsiSs_Config *ConfigPtr;

	ConfigPtr = XCsiSs_LookupConfig(DeviceId);
	if (!ConfigPtr)
	{
		bsp_printf("CSI2RxSs LookupCfg failed\r\n");
		return XST_FAILURE;
	}

	Status = XCsiSs_CfgInitialize(InstancePtr, ConfigPtr,
			ConfigPtr->BaseAddr);
	if (Status != XST_SUCCESS)
	{
		bsp_printf("CsiRxSs Cfg init failed - %x\r\n", Status);
		return XST_FAILURE;
	}

	return XST_SUCCESS;
}

int csi_rx_config(void)
{
	int Status;
#if (XPAR_XCSI_NUM_INSTANCES >= 1U)
	Status = Csi2RxSs_Init(&Csi2RxSsInst_0, XPAR_MIPI_CSI2_RX_SUBSYST_0_DEVICE_ID);
	if (Status != XST_SUCCESS)
	{
		Xil_Assert(__FILE__, __LINE__);
		bsp_printf(TXT_RED "CSI Rx Ss 0 Init failed status = %x.\r\n"
						 TXT_RST, Status);
		return XST_FAILURE;
	}

//	XCsiSs_ReportCoreInfo(&Csi2RxSsInst_0);
	Status = XCsiSs_Reset(&Csi2RxSsInst_0);
	if (Status != XST_SUCCESS)
	{
		Xil_Assert(__FILE__, __LINE__);
		return XST_FAILURE;
	}

	Status = XCsiSs_Activate(&Csi2RxSsInst_0, XCSI_ENABLE);
	if (Status != XST_SUCCESS)
	{
		Xil_Assert(__FILE__, __LINE__);
		bsp_printf(TXT_RED "CSI Rx Ss 0 Activate failed status = %x.\r\n"
						 TXT_RST, Status);
		return XST_FAILURE;
	}
//	bsp_printf(TXT_RED "CSI Rx Ss 0 config done!\r\n" TXT_RST);
#endif

#if (XPAR_XCSI_NUM_INSTANCES >= 2U)
	Status = Csi2RxSs_Init(&Csi2RxSsInst_1, XPAR_CSI_1_DEVICE_ID);
	if (Status != XST_SUCCESS)
	{
		Xil_Assert(__FILE__, __LINE__);
		bsp_printf(TXT_RED "CSI Rx Ss 1 Init failed status = %x.\r\n"
						 TXT_RST, Status);
		return XST_FAILURE;
	}
//	XCsi_SetSoftReset(&Csi2RxSsInst_1);
//	XCsi_SetActiveLaneCount(&Csi2RxSsInst_1, 4);
//	XCsi_ClearSoftReset(&Csi2RxSsInst_1);
	Status = XCsi_Activate(&Csi2RxSsInst_1, XCSI_ENABLE);
	if (Status != XST_SUCCESS)
	{
		Xil_Assert(__FILE__, __LINE__);
		bsp_printf(TXT_RED "CSI Rx Ss 1 Activate failed status = %x.\r\n"
						 TXT_RST, Status);
		return XST_FAILURE;
	}
#endif

#if (XPAR_XCSI_NUM_INSTANCES >= 3U)
	Status = Csi2RxSs_Init(&Csi2RxSsInst_2, XPAR_CSI_2_DEVICE_ID);
	if (Status != XST_SUCCESS)
	{
		Xil_Assert(__FILE__, __LINE__);
		bsp_printf(TXT_RED "CSI Rx Ss 2 Init failed status = %x.\r\n"
						 TXT_RST, Status);
		return XST_FAILURE;
	}
//	XCsi_SetSoftReset(&Csi2RxSsInst_2);
//	XCsi_SetActiveLaneCount(&Csi2RxSsInst_2, 4);
//	XCsi_ClearSoftReset(&Csi2RxSsInst_2);
	Status = XCsi_Activate(&Csi2RxSsInst_2, XCSI_ENABLE);
	if (Status != XST_SUCCESS)
	{
		Xil_Assert(__FILE__, __LINE__);
		bsp_printf(TXT_RED "CSI Rx Ss 2 Activate failed status = %x.\r\n"
						 TXT_RST, Status);
		return XST_FAILURE;
	}
#endif

#if (XPAR_XCSI_NUM_INSTANCES >= 4U)
	Status = Csi2RxSs_Init(&Csi2RxSsInst_3, XPAR_CSI_3_DEVICE_ID);
	if (Status != XST_SUCCESS)
	{
		Xil_Assert(__FILE__, __LINE__);
		bsp_printf(TXT_RED "CSI Rx Ss 3 Init failed status = %x.\r\n"
						 TXT_RST, Status);
		return XST_FAILURE;
	}
//	XCsi_SetSoftReset(&Csi2RxSsInst_3);
//	XCsi_SetActiveLaneCount(&Csi2RxSsInst_3, 4);
//	XCsi_ClearSoftReset(&Csi2RxSsInst_3);
	Status = XCsi_Activate(&Csi2RxSsInst_3, XCSI_ENABLE);
	if (Status != XST_SUCCESS)
	{
		Xil_Assert(__FILE__, __LINE__);
		bsp_printf(TXT_RED "CSI Rx Ss 3 Activate failed status = %x.\r\n"
						 TXT_RST, Status);
		return XST_FAILURE;
	}
#endif

	return XST_SUCCESS;
}

void print_csi_rx_reg_info(XCsi *InstancePtr, u8 verbose)
{
	UINTPTR Addr;

	Xil_AssertNonvoid(InstancePtr != NULL);
	Addr = InstancePtr->Config.BaseAddr;

	if(verbose == 1)
	{
		xil_printf("(0x00)Core Configuration Register     = 0x%08x\r\n", Xil_In32(Addr+0x0));
		xil_printf("(0x04)Protocol Configuration Register = 0x%08x\r\n", Xil_In32(Addr+0x04));
		xil_printf("(0x10)Core Status Register            = 0x%08x\r\n", Xil_In32(Addr+0x10));
		xil_printf("(0x20)Global Interrupt Enable Register= 0x%08x\r\n", Xil_In32(Addr+0x20));
		xil_printf("(0x24)Interrupt Status Register       = 0x%08x\r\n", Xil_In32(Addr+0x24));
		xil_printf("(0x28)Interrupt Enable Register       = 0x%08x\r\n", Xil_In32(Addr+0x28));
		xil_printf("(0x30)Generic Short Packet Register   = 0x%08x\r\n", Xil_In32(Addr+0x30));
		xil_printf("(0x34)VCX Frame Error Register        = 0x%08x\r\n", Xil_In32(Addr+0x34));
		xil_printf("(0x3C)Clock Lane Information Register = 0x%08x\r\n", Xil_In32(Addr+0x3C));
		xil_printf("(0x40)Lane0 Information               = 0x%08x\r\n", Xil_In32(Addr+0x40));
		xil_printf("(0x44)Lane1 Information               = 0x%08x\r\n", Xil_In32(Addr+0x44));
		xil_printf("(0x48)Lane2 Information               = 0x%08x\r\n", Xil_In32(Addr+0x48));
		xil_printf("(0x4C)Lane3 Information               = 0x%08x\r\n", Xil_In32(Addr+0x4C));
		xil_printf("(0x60)Image Information 1 for VC0     = 0x%08x\r\n", Xil_In32(Addr+0x60));
		xil_printf("(0x64)Image Information 2 for VC0     = 0x%08x\r\n", Xil_In32(Addr+0x64));
		xil_printf("(0x68)Image Information 1 for VC1     = 0x%08x\r\n", Xil_In32(Addr+0x68));
		xil_printf("(0x6C)Image Information 2 for VC1     = 0x%08x\r\n", Xil_In32(Addr+0x6C));
		xil_printf("(0x70)Image Information 1 for VC2     = 0x%08x\r\n", Xil_In32(Addr+0x70));
		xil_printf("(0x74)Image Information 2 for VC2     = 0x%08x\r\n", Xil_In32(Addr+0x74));
		xil_printf("(0x78)Image Information 1 for VC3     = 0x%08x\r\n", Xil_In32(Addr+0x78));
		xil_printf("(0x7C)Image Information 2 for VC3     = 0x%08x\r\n", Xil_In32(Addr+0x7C));
		xil_printf("(0x80)Image Information 1 for VC4     = 0x%08x\r\n", Xil_In32(Addr+0x80));
		xil_printf("(0x84)Image Information 2 for VC4     = 0x%08x\r\n", Xil_In32(Addr+0x84));
		xil_printf("(0x88)Image Information 1 for VC5     = 0x%08x\r\n", Xil_In32(Addr+0x88));
		xil_printf("(0x8C)Image Information 2 for VC5     = 0x%08x\r\n", Xil_In32(Addr+0x8C));
		xil_printf("(0x90)Image Information 1 for VC6     = 0x%08x\r\n", Xil_In32(Addr+0x90));
		xil_printf("(0x94)Image Information 2 for VC6     = 0x%08x\r\n", Xil_In32(Addr+0x94));
		xil_printf("(0x98)Image Information 1 for VC7     = 0x%08x\r\n", Xil_In32(Addr+0x98));
		xil_printf("(0x9C)Image Information 2 for VC7     = 0x%08x\r\n", Xil_In32(Addr+0x9C));
		xil_printf("(0xA0)Image Information 1 for VC8     = 0x%08x\r\n", Xil_In32(Addr+0xA0));
		xil_printf("(0xA4)Image Information 2 for VC8     = 0x%08x\r\n", Xil_In32(Addr+0xA4));
		xil_printf("(0xA8)Image Information 1 for VC9     = 0x%08x\r\n", Xil_In32(Addr+0xA8));
		xil_printf("(0xAC)Image Information 2 for VC9     = 0x%08x\r\n", Xil_In32(Addr+0xAC));
		xil_printf("(0xB0)Image Information 1 for VC10    = 0x%08x\r\n", Xil_In32(Addr+0xB0));
		xil_printf("(0xB4)Image Information 2 for VC10    = 0x%08x\r\n", Xil_In32(Addr+0xB4));
		xil_printf("(0xB8)Image Information 1 for VC11    = 0x%08x\r\n", Xil_In32(Addr+0xB8));
		xil_printf("(0xBC)Image Information 2 for VC11    = 0x%08x\r\n", Xil_In32(Addr+0xBC));
		xil_printf("(0xC0)Image Information 1 for VC12    = 0x%08x\r\n", Xil_In32(Addr+0xC0));
		xil_printf("(0xC4)Image Information 2 for VC12    = 0x%08x\r\n", Xil_In32(Addr+0xC4));
		xil_printf("(0xC8)Image Information 1 for VC13    = 0x%08x\r\n", Xil_In32(Addr+0xC8));
		xil_printf("(0xCC)Image Information 2 for VC13    = 0x%08x\r\n", Xil_In32(Addr+0xCC));
		xil_printf("(0xD0)Image Information 1 for VC14    = 0x%08x\r\n", Xil_In32(Addr+0xD0));
		xil_printf("(0xD4)Image Information 2 for VC14    = 0x%08x\r\n", Xil_In32(Addr+0xD4));
		xil_printf("(0xD8)Image Information 1 for VC15    = 0x%08x\r\n", Xil_In32(Addr+0xD8));
		xil_printf("(0xDC)Image Information 2 for VC15    = 0x%08x\r\n", Xil_In32(Addr+0xDC));

		xil_printf("(0x00)D-PHY CONTROL                   = 0x%08x\r\n", Xil_In32(Addr+0x1000+0x00));
		xil_printf("(0x04)D-PHY IDELAY_TAP_VALUE          = 0x%08x\r\n", Xil_In32(Addr+0x1000+0x04));
		xil_printf("(0x08)D-PHY INIT                      = 0x%08x\r\n", Xil_In32(Addr+0x1000+0x08));
		xil_printf("(0x10)D-PHY HS_TIMEOUT                = 0x%08x\r\n", Xil_In32(Addr+0x1000+0x10));
		xil_printf("(0x14)D-PHY ESC_TIMEOUT               = 0x%08x\r\n", Xil_In32(Addr+0x1000+0x14));
		xil_printf("(0x18)D-PHY CL_STATUS                 = 0x%08x\r\n", Xil_In32(Addr+0x1000+0x18));
		xil_printf("(0x1C)D-PHY DL0_STATUS                = 0x%08x\r\n", Xil_In32(Addr+0x1000+0x1C));
		xil_printf("(0x20)D-PHY DL1_STATUS                = 0x%08x\r\n", Xil_In32(Addr+0x1000+0x20));
		xil_printf("(0x24)D-PHY DL2_STATUS                = 0x%08x\r\n", Xil_In32(Addr+0x1000+0x24));
		xil_printf("(0x28)D-PHY DL3_STATUS                = 0x%08x\r\n", Xil_In32(Addr+0x1000+0x28));
		xil_printf("(0x30)D-PHY HS_SETTLE L0              = 0x%08x\r\n", Xil_In32(Addr+0x1000+0x30));
		xil_printf("(0x48)D-PHY HS_SETTLE L1              = 0x%08x\r\n", Xil_In32(Addr+0x1000+0x48));
	}
}

#endif // XPAR_XCSI_NUM_INSTANCES
