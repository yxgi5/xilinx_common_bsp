/******************************************************************************
*
* Copyright (C) 2009 - 2014 Xilinx, Inc.  All rights reserved.
*
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files (the "Software"), to deal
* in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
* copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included in
* all copies or substantial portions of the Software.
*
* Use of the Software is limited solely to applications:
* (a) running on a Xilinx device, or
* (b) that interact with a Xilinx device through a bus or interconnect.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
* XILINX  BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
* WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF
* OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
* SOFTWARE.
*
* Except as contained in this notice, the name of the Xilinx shall not be used
* in advertising or otherwise to promote the sale, use or other dealings in
* this Software without prior written authorization from Xilinx.
*
******************************************************************************/

/*
 * helloworld.c: simple test application
 *
 * This application configures UART 16550 to baud rate 9600.
 * PS7 UART (Zynq) is not initialized by this application, since
 * bootrom/bsp configures it to baud rate 115200
 *
 * ------------------------------------------------
 * | UART TYPE   BAUD RATE                        |
 * ------------------------------------------------
 *   uartns550   9600
 *   uartlite    Configurable only in HW design
 *   ps7_uart    115200 (configured by bootrom/bsp)
 */

#include "bsp.h"

#if defined (XPAR_AXI_LITE_REG_NUM_INSTANCES) && (XPAR_AXI_LITE_REG_0_DEVICE_ID == 0)
volatile u32 __HW_VER__;
#endif



int main()
{
	int Status ;

    init_platform(); // include interrupts setup

#if defined(XPAR_XGPIO_I2C_0_AXI_GPIO_0_DEVICE_ID)
    Status = xgpio_i2c_init();
    if (Status != XST_SUCCESS)
	{
		Xil_Assert(__FILE__, __LINE__);
		return XST_FAILURE ;
	}
//#elif defined (XPAR_XGPIO_NUM_INSTANCES)
//    Status = xgpio_setup(&XGpioInst, XPAR_GPIO_0_DEVICE_ID, 0, 0);
//    if (Status != XST_SUCCESS)
//	{
//		Xil_Assert(__FILE__, __LINE__);
//		return XST_FAILURE ;
//	}
#endif // XPAR_XGPIO_I2C_0_AXI_GPIO_0_DEVICE_ID

#if defined(XPAR_AXI_IIC_0_DEVICE_ID)
    XIic_WriteReg(XPAR_IIC_0_BASEADDR, XIIC_GPO_REG_OFFSET, 0);
#endif // XPAR_AXI_IIC_0_DEVICE_ID


    bsp_printf("\r\n\r\n***************************\n\r");
    bsp_printf("Test common API.\n\r");
    bsp_printf("\r\n%s, UTC %s\r\n",__DATE__,__TIME__);
#if defined (XPAR_AXI_LITE_REG_NUM_INSTANCES) && (XPAR_AXI_LITE_REG_0_DEVICE_ID == 0)
	__HW_VER__ = AXI_LITE_REG_mReadReg(XPAR_PROCESSOR_SUBSYSTEM_AXI_LITE_REG_0_S00_AXI_BASEADDR, AXI_LITE_REG_S00_AXI_SLV_REG0_OFFSET);
//	__HW_VER__ = AXI_LITE_REG_mReadReg(XPAR_AXI_LITE_REG_0_S00_AXI_BASEADDR, AXI_LITE_REG_S00_AXI_SLV_REG0_OFFSET);
	bsp_printf(TXT_GREEN "hardware ver = 0x%08x\n\r" TXT_RST, __HW_VER__);
#endif // XPAR_AXI_LITE_REG_NUM_INSTANCES
#ifdef SW_VER_BY_COMPILE_TIME
    __SW_VER__ = GetSoftWareVersion();
    bsp_printf("software ver = 0x%08x\n\r", __SW_VER__);
    bsp_printf("***************************\n\r");
#elif defined (__SW_VER__)
//    bsp_printf("software ver = 0x%08x\n\r", __SW_VER__);
    bsp_printf(TXT_GREEN "software ver = 0x%08x\n\r" TXT_RST, __SW_VER__);
    bsp_printf("***************************\n\r");
#endif // __SW_VER__ || SW_VER_BY_COMPILE_TIME

#if defined (__UARTLITE_FIFO_H__)
    Uart0VarInit();
    Uart0_Init();
#if defined (MODBUS_RTU_SLAVE)
    MODS_VarInit();
#endif // MODBUS_RTU_SLAVE
#endif // __UARTLITE_FIFO_H__


#if defined(__SIL9136_H__)
    sil9136_config();
#endif

#if defined (SER_CFG) || defined (DES_CFG)
    // MAX9296 config
    u8 ret8=0;
#if defined (DES_CFG)
    Status = xgpio_i2c_reg16_read(I2C_NO_3, 0x90>>1, 0x0000, &ret8, STRETCH_ON);
    Status = xgpio_i2c_reg16_read(I2C_NO_3, 0x90>>1, 0x0001, &ret8, STRETCH_ON);
#if defined (SERDES_3G)
    Status = xgpio_i2c_reg16_write(I2C_NO_3, 0x90>>1, 0x0001, 0x01, STRETCH_ON); // 3Gbps
    Status = xgpio_i2c_reg16_write(I2C_NO_3, 0x90>>1, 0x0010, 0x21, STRETCH_ON); // reset link
#else
    Status = xgpio_i2c_reg16_write(I2C_NO_3, 0x90>>1, 0x0001, 0x02, STRETCH_ON); // 6Gbps
	Status = xgpio_i2c_reg16_write(I2C_NO_3, 0x90>>1, 0x0010, 0x21, STRETCH_ON); // reset link
#endif // SERDES_3G
    max929x_write_array(I2C_NO_3, max9296_rgb888_gmsl2);
#endif // DES_CFG
#if defined (SER_CFG)
    Status = xgpio_i2c_reg16_read(I2C_NO_3, 0x80>>1, 0x0000, &ret8, STRETCH_ON);
    Status = xgpio_i2c_reg16_read(I2C_NO_3, 0x80>>1, 0x0001, &ret8, STRETCH_ON);
#if defined (SERDES_3G)
    Status = xgpio_i2c_reg16_write(I2C_NO_3, 0x80>>1, 0x0001, 0x04, STRETCH_ON); // 3Gbps
    Status = xgpio_i2c_reg16_write(I2C_NO_3, 0x80>>1, 0x0010, 0x21, STRETCH_ON); // reset link
#else
    Status = xgpio_i2c_reg16_write(I2C_NO_3, 0x80>>1, 0x0001, 0x08, STRETCH_ON); // 6Gbps
    Status = xgpio_i2c_reg16_write(I2C_NO_3, 0x80>>1, 0x0010, 0x21, STRETCH_ON); // reset link
#endif // SERDES_3G
//    max929x_write_array(I2C_NO_3, max9295_rgb888_gmsl2);
    max929x_write_array(I2C_NO_3, max96717_rgb888_gmsl2);
#endif // SER_CFG
#if defined (DES_CFG)
    Status = xgpio_i2c_reg16_read(I2C_NO_3, 0x90>>1, 0x0000, &ret8, STRETCH_ON);
	Status = xgpio_i2c_reg16_read(I2C_NO_3, 0x90>>1, 0x0001, &ret8, STRETCH_ON);
#endif // DES_CFG
#if defined (SER_CFG)
    Status = xgpio_i2c_reg16_read(I2C_NO_3, 0x80>>1, 0x0000, &ret8, STRETCH_ON);
    Status = xgpio_i2c_reg16_read(I2C_NO_3, 0x80>>1, 0x0001, &ret8, STRETCH_ON);
#endif // SER_CFG
#endif // SER_CFG || DES_CFG

#if defined (XPAR_XAXIS_SWITCH_NUM_INSTANCES)
    Status = axis_switch_cfg(0);
    if (Status != XST_SUCCESS)
	{
		Xil_Assert(__FILE__, __LINE__);
		return XST_FAILURE ;
	}
#endif // XPAR_XAXIS_SWITCH_NUM_INSTANCES

#if defined (XPAR_XCLK_WIZ_NUM_INSTANCES)
    Status = clkwiz_config();
    if (Status != XST_SUCCESS)
	{
		Xil_Assert(__FILE__, __LINE__);
		return XST_FAILURE ;
	}
#endif // XPAR_XCLK_WIZ_NUM_INSTANCES

#if defined (XPAR_XVTC_NUM_INSTANCES)
    Status = vtc_config();
    if (Status != XST_SUCCESS)
	{
		Xil_Assert(__FILE__, __LINE__);
		return XST_FAILURE ;
	}
#endif // XPAR_XVTC_NUM_INSTANCES

#if defined (XPAR_XV_TPG_NUM_INSTANCES)
    Status = tpg_config();
    if (Status != XST_SUCCESS)
	{
		Xil_Assert(__FILE__, __LINE__);
		return XST_FAILURE ;
	}
#endif // XPAR_XV_TPG_NUM_INSTANCES


#if defined (XPAR_XAXIVDMA_NUM_INSTANCES)
    clear_display();
//    vdma_config_direct();
	Status = vdma_config();
	if (Status != XST_SUCCESS)
	{
		Xil_Assert(__FILE__, __LINE__);
		return XST_FAILURE ;
	}
#endif // XPAR_XAXIVDMA_NUM_INSTANCES

#if defined (XPAR_XVPROCSS_NUM_INSTANCES)
	Status = vpss_config();
	if (Status != XST_SUCCESS)
	{
		Xil_Assert(__FILE__, __LINE__);
		return XST_FAILURE ;
	}
#endif // XPAR_XVPROCSS_NUM_INSTANCES

#if defined (UDP_UPDATE) || defined (TCP_UPDATE) || defined (TCP_COMMAND_SRV) || defined (UDP_COMMAND_SRV)
	lwip_common_init(&server_netif);
#endif // #if defined (UDP_UPDATE) || defined (TCP_UPDATE) || defined (TCP_COMMAND_SRV) || defined (UDP_COMMAND_SRV)

#if defined (INTC_DEVICE_ID) || defined (INTC)
	platform_enable_interrupts();
#endif //#if defined (INTC_DEVICE_ID) || defined (INTC)
    while(1)
    {
#if defined (UDP_UPDATE) || defined (TCP_UPDATE) || defined (TCP_COMMAND_SRV) || defined (UDP_COMMAND_SRV)
    	transfer_data(&server_netif);
#endif // #if defined (UDP_UPDATE) || defined (TCP_UPDATE) || defined (TCP_COMMAND_SRV) || defined (UDP_COMMAND_SRV)

#if	defined (MODBUS_RTU_SLAVE)
    	MODS_Poll();
#endif

//    	bsp_printf("pi=%f\n\r",3.1415);
//    	ret32 = xgpio_i2c_reg16_read(I2C_NO_0, 0x80>>1, 0x0000, &ret8, STRETCH_ON);
    }
	// never reached
    cleanup_platform();
    return 0;
}
