#include "../bsp.h"

#if defined (XPAR_XIIC_NUM_INSTANCES)

#define IIC_BASE_ADDRESS	XPAR_IIC_0_BASEADDR

int xi2c_reg8_write(char IIC_ADDR, char Addr, char Data)
{
	volatile unsigned SentByteCount;
	u32 CntlReg;
	u8 SendBuffer[2];
	SendBuffer[0] = Addr;
	SendBuffer[1] = Data;
	SentByteCount = XIic_Send(IIC_BASE_ADDRESS, IIC_ADDR, SendBuffer, 2, XIIC_STOP);

	if (SentByteCount != 2) {

		/* Send is aborted so reset Tx FIFO */
		CntlReg = XIic_ReadReg(IIC_BASE_ADDRESS,
					XIIC_CR_REG_OFFSET);
		XIic_WriteReg(IIC_BASE_ADDRESS, XIIC_CR_REG_OFFSET,
				CntlReg | XIIC_CR_TX_FIFO_RESET_MASK);
		XIic_WriteReg(IIC_BASE_ADDRESS, XIIC_CR_REG_OFFSET,
				XIIC_CR_ENABLE_DEVICE_MASK);
		return XST_FAILURE;
	}

	return XST_SUCCESS;
}

char xi2c_reg8_read(char IIC_ADDR, char Addr)
{
	u16 StatusReg;
	u8 SendBuffer;
	u8 ReceiveBuffer;
	SendBuffer=Addr;
	XIic_Send(IIC_BASE_ADDRESS, IIC_ADDR, &SendBuffer, 1, XIIC_REPEATED_START);
	XIic_Recv(IIC_BASE_ADDRESS, IIC_ADDR, &ReceiveBuffer, 1, XIIC_STOP);

//	do {
//		StatusReg = XIic_ReadReg(IIC_BASE_ADDRESS, XIIC_SR_REG_OFFSET);
//	}while((StatusReg & XIIC_SR_BUS_BUSY_MASK));

	return ReceiveBuffer;
}

int xi2c_reg16_write(char IIC_ADDR, unsigned short Addr, char Data)
{
	volatile unsigned SentByteCount;
	u32 CntlReg;
	u8 SendBuffer[3];
	SendBuffer[0] = Addr>>8;
	SendBuffer[1] = Addr;
	SendBuffer[2] = Data;
	SentByteCount = XIic_Send(IIC_BASE_ADDRESS, IIC_ADDR, SendBuffer, 3, XIIC_STOP);

	if (SentByteCount != 3) {

		/* Send is aborted so reset Tx FIFO */
		CntlReg = XIic_ReadReg(IIC_BASE_ADDRESS,
					XIIC_CR_REG_OFFSET);
		XIic_WriteReg(IIC_BASE_ADDRESS, XIIC_CR_REG_OFFSET,
				CntlReg | XIIC_CR_TX_FIFO_RESET_MASK);
		XIic_WriteReg(IIC_BASE_ADDRESS, XIIC_CR_REG_OFFSET,
				XIIC_CR_ENABLE_DEVICE_MASK);
		return XST_FAILURE;
	}

	return XST_SUCCESS;
}

char xi2c_reg16_read(char IIC_ADDR, unsigned short Addr)
{
	u16 StatusReg;
	u8 SendBuffer[2];
	SendBuffer[0] = Addr>>8;
	SendBuffer[1] = Addr;
	u8 ReceiveBuffer;
#if 1
	XIic_Send(IIC_BASE_ADDRESS, IIC_ADDR, SendBuffer, 2, XIIC_REPEATED_START);
	XIic_Recv(IIC_BASE_ADDRESS, IIC_ADDR, &ReceiveBuffer, 1, XIIC_STOP);
#else
	u8 i;

	*(u32 *)(XPAR_IIC_0_BASEADDR + 0x100) = 0x1 ;
	*(u32 *)(XPAR_IIC_0_BASEADDR + 0x120) = 0xF ;
	*(u32 *)(XPAR_IIC_0_BASEADDR + 0x108) = 0x100|(IIC_ADDR<<1)|0x00;
	*(u32 *)(XPAR_IIC_0_BASEADDR + 0x108) = 0xff&(Addr>>8);
	*(u32 *)(XPAR_IIC_0_BASEADDR + 0x108) = 0xff&Addr;
	*(u32 *)(XPAR_IIC_0_BASEADDR + 0x108) = 0x100|(IIC_ADDR<<1)|0x01;
	*(u32 *)(XPAR_IIC_0_BASEADDR + 0x108) = 0x200|0x01;

	for (i = 0 ; i< 1;i++)
	{
		while(((*(u32 *)(XPAR_IIC_0_BASEADDR + 0x104 ))&(0x40)));
		ReceiveBuffer=*(u32 *)(XPAR_IIC_0_BASEADDR + 0x10C);
	}
#endif
//	do {
//		StatusReg = XIic_ReadReg(IIC_BASE_ADDRESS, XIIC_SR_REG_OFFSET);
//	}while((StatusReg & XIIC_SR_BUS_BUSY_MASK));

	return ReceiveBuffer;
}


#endif // XPAR_XIIC_NUM_INSTANCES

/*
usage:


XIic_WriteReg(XPAR_IIC_0_BASEADDR, XIIC_GPO_REG_OFFSET, 0);


ret = xi2c_reg16_read(0x50>>1, 0x0000);

*/

