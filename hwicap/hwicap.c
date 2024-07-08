#include "../bsp.h"

#if defined (XPAR_XHWICAP_NUM_INSTANCES)

/*
 * Initialize the HwIcap instance.
 */


XHwIcap HwIcap0;


int hwcap_init(void)
{
	int Status;
	XHwIcap_Config *CfgPtr;

	CfgPtr = XHwIcap_LookupConfig(XPAR_HWICAP_0_DEVICE_ID);
	if (CfgPtr == NULL) {
		bsp_printf(TXT_RED "XHwIcap_LookupConfig failed...\r\n" TXT_RST);
		return XST_FAILURE;
	}

	Status = XHwIcap_CfgInitialize(&HwIcap0, CfgPtr, CfgPtr->BaseAddress);
	if (Status != XST_SUCCESS) {
		bsp_printf(TXT_RED "hwicap initialization failed...\r\n" TXT_RST);
		return XST_FAILURE;
	}

	/*
	 * Run the Self test.
	 */
	Status = XHwIcap_SelfTest(&HwIcap0);
	if (Status != XST_SUCCESS) {
		bsp_printf(TXT_RED "hwicap selftest failed...\r\n" TXT_RST);
		return XST_FAILURE;
	}

	return XST_SUCCESS;
}


// ConfigReg defined in xhwicap_i.h ref to ug470
u32 hwicap_GetConfigReg(XHwIcap *InstancePtr, u32 ConfigReg, u32 *RegData)
{
	return XHwIcap_GetConfigReg(InstancePtr, ConfigReg, RegData);
}

/*****************************************************************************/
/**
*
* This function issues IPROG to the target device.
*
* @param	iprog_address is the Jump address for the ICAP
* @param	BaseAddress is the base address of the HwIcap instance.
* @return	XST_SUCCESS if successful, otherwise XST_FAILURE
*
* @note		None
*
******************************************************************************/
int ISSUE_IPROG(u32 iprog_address)
{
//	int Status;
	u32 Index;
	u32 Retries;

	u32 SendtoFIFO[8] =
	{
		0xFFFFFFFF, /* Dummy Word */
		0xAA995566, /* Sync Word*/
		0x20000000, /* Type 1 NO OP */
		0x30020001, /* Write WBSTAR cmd */
		0x00800000, /* Warm boot start address (Load the desired address) */
		0x30008001, /* Write CMD */
		0x0000000F, /* Write IPROG */
		0x20000000, /* Type 1 NO OP  */
	};
	SendtoFIFO[4] = iprog_address; //Load the Warm Boot Address


	/*
	 * Write command sequence to the FIFO
	 */
	for (Index = 0; Index < 8; Index++) {
		XHwIcap_WriteReg(XPAR_HWICAP_0_BASEADDR, XHI_WF_OFFSET, SendtoFIFO[Index]);
	}

	/*
	 * Start the transfer of the data from the FIFO to the ICAP device.
	 */
	XHwIcap_WriteReg(XPAR_HWICAP_0_BASEADDR, XHI_CR_OFFSET, XHI_CR_WRITE_MASK);

	/*
	 * Poll for done, which indicates end of transfer
	 */
	Retries = 0;
	while ((XHwIcap_ReadReg(XPAR_HWICAP_0_BASEADDR, XHI_SR_OFFSET) &
			XHI_SR_DONE_MASK) != XHI_SR_DONE_MASK) {
		Retries++;
		if (Retries > XHI_MAX_RETRIES) {

			/*
			 * Waited to long. Exit with error.
			 */
			printf("\r\nHwIcapLowLevelExample failed- retries  \
			failure. \r\n\r\n");

			return XST_FAILURE;
		}
	}

	/*
	 * Wait till the Write bit is cleared in the CR register.
	 */
	while ((XHwIcap_ReadReg(XPAR_HWICAP_0_BASEADDR, XHI_CR_OFFSET)) &
					XHI_CR_WRITE_MASK);
	/*
	 * Write to the SIZE register. We want to readback one word.
	 */
	XHwIcap_WriteReg(XPAR_HWICAP_0_BASEADDR, XHI_SZ_OFFSET, 1);


	/*
	 * Start the transfer of the data from ICAP to the FIFO.
	 */
	XHwIcap_WriteReg(XPAR_HWICAP_0_BASEADDR, XHI_CR_OFFSET, XHI_CR_READ_MASK);

	/*
	 * Poll for done, which indicates end of transfer
	 */
	Retries = 0;
	while ((XHwIcap_ReadReg(XPAR_HWICAP_0_BASEADDR, XHI_SR_OFFSET) &
			XHI_SR_DONE_MASK) != XHI_SR_DONE_MASK) {
		Retries++;
		if (Retries > XHI_MAX_RETRIES) {

			/*
			 * Waited too long. Exit with error.
			 */

			return XST_FAILURE;
		}
	}


	return XST_SUCCESS;
}


/*****************************************************************************/
/**
*
* This function returns the IDCODE of the target device.
*
* @param	BaseAddress is the base address of the HwIcap instance.
* @param	IdCode is the IDCODE of the part this code is running on.
*
* @return	XST_SUCCESS if successful, otherwise XST_FAILURE
*
* @note		None
*
******************************************************************************/
int READ_IDCODE(u32 *IdCode)
{
//	int Status;
	u32 Index;
	u32 Retries;

	u32 ReadId[6] =
	{
		XHI_DUMMY_PACKET,   /* Dummy Word */
		XHI_SYNC_PACKET,    /* Sync Word*/
		XHI_NOOP_PACKET,    /* Type 1 NO OP */
		XHI_NOOP_PACKET,    /* Type 1 NO OP */
		XHI_DEVICE_ID_READ, /* Read Product ID Code Register */
		XHI_NOOP_PACKET,    /* Type 1 NO OP */
	};


	/*
	 * Write command sequence to the FIFO
	 */
	for (Index = 0; Index < 6; Index++) {
		XHwIcap_WriteReg(XPAR_HWICAP_0_BASEADDR, XHI_WF_OFFSET, ReadId[Index]);
	}


	/*
	 * Start the transfer of the data from the FIFO to the ICAP device.
	 */
	XHwIcap_WriteReg(XPAR_HWICAP_0_BASEADDR, XHI_CR_OFFSET, XHI_CR_WRITE_MASK);

	/*
	 * Poll for done, which indicates end of transfer
	 */
	Retries = 0;
	while ((XHwIcap_ReadReg(XPAR_HWICAP_0_BASEADDR, XHI_SR_OFFSET) &
			XHI_SR_DONE_MASK) != XHI_SR_DONE_MASK) {
		Retries++;
		if (Retries > XHI_MAX_RETRIES) {

			/*
			 * Waited to long. Exit with error.
			 */
			printf("\r\nHwIcapLowLevelExample failed- retries  \
			failure. \r\n\r\n");

			return XST_FAILURE;
		}
	}

	/*
	 * Wait till the Write bit is cleared in the CR register.
	 */
	while ((XHwIcap_ReadReg(XPAR_HWICAP_0_BASEADDR, XHI_CR_OFFSET)) &
					XHI_CR_WRITE_MASK);
	/*
	 * Write to the SIZE register. We want to readback one word.
	 */
	XHwIcap_WriteReg(XPAR_HWICAP_0_BASEADDR, XHI_SZ_OFFSET, 1);


	/*
	 * Start the transfer of the data from ICAP to the FIFO.
	 */
	XHwIcap_WriteReg(XPAR_HWICAP_0_BASEADDR, XHI_CR_OFFSET, XHI_CR_READ_MASK);

	/*
	 * Poll for done, which indicates end of transfer
	 */
	Retries = 0;
	while ((XHwIcap_ReadReg(XPAR_HWICAP_0_BASEADDR, XHI_SR_OFFSET) &
			XHI_SR_DONE_MASK) != XHI_SR_DONE_MASK) {
		Retries++;
		if (Retries > XHI_MAX_RETRIES) {

			/*
			 * Waited to long. Exit with error.
			 */

			return XST_FAILURE;
		}
	}

	/*
	 * Wait till the Read bit is cleared in the CR register.
	 */
	while ((XHwIcap_ReadReg(XPAR_HWICAP_0_BASEADDR, XHI_CR_OFFSET)) &
					XHI_CR_READ_MASK);
	/*
	 * Return the IDCODE value
	 */
	*IdCode =   XHwIcap_ReadReg(XPAR_HWICAP_0_BASEADDR, XHI_RF_OFFSET);

	return XST_SUCCESS;
}


/*****************************************************************************/
/**
*
* This function returns the BOOTSTS of the target device.
*
* @param	BaseAddress is the base address of the HwIcap instance.
* @param	StsCode is the BOOTSTS reg value of the part this code is running on.
*
* @return	XST_SUCCESS if successful, otherwise XST_FAILURE
*
* @note		None
*
******************************************************************************/
#define XHI_BOOTSTS_READ		0x2802C001	// see ug470: 0b10110 << 13 = 0x2c000
int READ_BOOTSTS(u32 *StsCode)
{
//	int Status;
	u32 Index;
	u32 Retries;

	u32 ReadId[6] =
	{
		XHI_DUMMY_PACKET,   /* Dummy Word */
		XHI_SYNC_PACKET,    /* Sync Word*/
		XHI_NOOP_PACKET,    /* Type 1 NO OP */
		XHI_NOOP_PACKET,    /* Type 1 NO OP */
		XHI_BOOTSTS_READ, /* Read Product ID Code Register */
		XHI_NOOP_PACKET,    /* Type 1 NO OP */
	};


	/*
	 * Write command sequence to the FIFO
	 */
	for (Index = 0; Index < 6; Index++) {
		XHwIcap_WriteReg(XPAR_HWICAP_0_BASEADDR, XHI_WF_OFFSET, ReadId[Index]);
	}


	/*
	 * Start the transfer of the data from the FIFO to the ICAP device.
	 */
	XHwIcap_WriteReg(XPAR_HWICAP_0_BASEADDR, XHI_CR_OFFSET, XHI_CR_WRITE_MASK);

	/*
	 * Poll for done, which indicates end of transfer
	 */
	Retries = 0;
	while ((XHwIcap_ReadReg(XPAR_HWICAP_0_BASEADDR, XHI_SR_OFFSET) &
			XHI_SR_DONE_MASK) != XHI_SR_DONE_MASK) {
		Retries++;
		if (Retries > XHI_MAX_RETRIES) {

			/*
			 * Waited to long. Exit with error.
			 */
			printf("\r\nHwIcapLowLevelExample failed- retries  \
			failure. \r\n\r\n");

			return XST_FAILURE;
		}
	}

	/*
	 * Wait till the Write bit is cleared in the CR register.
	 */
	while ((XHwIcap_ReadReg(XPAR_HWICAP_0_BASEADDR, XHI_CR_OFFSET)) &
					XHI_CR_WRITE_MASK);
	/*
	 * Write to the SIZE register. We want to readback one word.
	 */
	XHwIcap_WriteReg(XPAR_HWICAP_0_BASEADDR, XHI_SZ_OFFSET, 1);


	/*
	 * Start the transfer of the data from ICAP to the FIFO.
	 */
	XHwIcap_WriteReg(XPAR_HWICAP_0_BASEADDR, XHI_CR_OFFSET, XHI_CR_READ_MASK);

	/*
	 * Poll for done, which indicates end of transfer
	 */
	Retries = 0;
	while ((XHwIcap_ReadReg(XPAR_HWICAP_0_BASEADDR, XHI_SR_OFFSET) &
			XHI_SR_DONE_MASK) != XHI_SR_DONE_MASK) {
		Retries++;
		if (Retries > XHI_MAX_RETRIES) {

			/*
			 * Waited to long. Exit with error.
			 */

			return XST_FAILURE;
		}
	}

	/*
	 * Wait till the Read bit is cleared in the CR register.
	 */
	while ((XHwIcap_ReadReg(XPAR_HWICAP_0_BASEADDR, XHI_CR_OFFSET)) &
					XHI_CR_READ_MASK);
	/*
	 * Return the IDCODE value
	 */
	*StsCode =   XHwIcap_ReadReg(XPAR_HWICAP_0_BASEADDR, XHI_RF_OFFSET);

	return XST_SUCCESS;
}

int isFallbacked(void)
{
	u32 StsCode;

	READ_BOOTSTS(&StsCode);
	bsp_printf("StsCode = 0x%08x\n\r", StsCode);
	if(CHB(StsCode, BIT32(1)))
	{
		return 1;
	}
	else
	{
		return 0;
	}
}


#endif // #if defined (XPAR_XHWICAP_NUM_INSTANCES)
