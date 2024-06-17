
XIic_WriteReg(XPAR_IIC_0_BASEADDR, XIIC_GPO_REG_OFFSET, 0);


ret = xi2c_reg16_read(0x50>>1, 0x0000);
