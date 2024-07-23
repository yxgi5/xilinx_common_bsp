// 几个概念
// 总线占用，总线忙，意思是有主机在总线发送了start
// 钳住I2C总线，意思是scl线被拉低
// 释放总线，有主机在总线发送了stop，且sda和scl被拉高

#include "../bsp.h"
#if defined (XPAR_XGPIOPS_NUM_INSTANCES) && !(EMIO_I2C_NO_BUTT == 0)

#define  GPIOPS_ID  XPAR_XGPIOPS_0_DEVICE_ID  //PS 端 GPIO 器件 ID
static  XGpioPs  gpiops_inst; //PS 端 GPIO 驱动实例

XGpioPs_I2C_Cfg XGpioPs_I2C_CfgTable[EMIO_I2C_NO_BUTT] =
{
#if (EMIO_I2C_NUM >= 1U)
	{EMIO_I2C_NO_0, EMIO_SCL0_NUM, EMIO_SDA0_NUM},
#endif
#if (EMIO_I2C_NUM >= 2U)
	{EMIO_I2C_NO_1, EMIO_SCL1_NUM, EMIO_SDA1_NUM},
#endif
#if (EMIO_I2C_NUM >= 3U)
	{EMIO_I2C_NO_2, EMIO_SCL2_NUM, EMIO_SDA2_NUM},
#endif
#if (EMIO_I2C_NUM >= 4U)
	{EMIO_I2C_NO_3, EMIO_SCL3_NUM, EMIO_SDA3_NUM},
#endif
#if (EMIO_I2C_NUM >= 5U)
	{EMIO_I2C_NO_4, EMIO_SCL4_NUM, EMIO_SDA4_NUM},
#endif
#if (EMIO_I2C_NUM >= 6U)
	{EMIO_I2C_NO_5, EMIO_SCL5_NUM, EMIO_SDA5_NUM},
#endif
#if (EMIO_I2C_NUM >= 7U)
	{EMIO_I2C_NO_6, EMIO_SCL6_NUM, EMIO_SDA6_NUM},
#endif
#if (EMIO_I2C_NUM >= 8U)
	{EMIO_I2C_NO_7, EMIO_SCL7_NUM, EMIO_SDA7_NUM}
#endif
};

//EMIO初始化
int emio_init(void)
{
	int Status ;
	XGpioPs_Config *gpiops_cfg_ptr; //PS 端 GPIO 配置信息

	//根据器件 ID 查找配置信息
	gpiops_cfg_ptr = XGpioPs_LookupConfig(GPIOPS_ID);
	//初始化器件驱动
	Status = XGpioPs_CfgInitialize(&gpiops_inst, gpiops_cfg_ptr, gpiops_cfg_ptr->BaseAddr);
	if(Status!= XST_SUCCESS)
	{
		return(XST_FAILURE);
	}

	for(int i=0; i<EMIO_I2C_NO_BUTT; i++)
	{
		//设置 gpio端口 为输入
		XGpioPs_SetDirectionPin(&gpiops_inst, XGpioPs_I2C_CfgTable[i].I2C_SCL, EMIO_INPUT);
		XGpioPs_SetDirectionPin(&gpiops_inst, XGpioPs_I2C_CfgTable[i].I2C_SDA, EMIO_INPUT);
		//使能 gpio端口 输入
		XGpioPs_SetOutputEnablePin(&gpiops_inst, XGpioPs_I2C_CfgTable[i].I2C_SCL, EMIO_INPUT);
		XGpioPs_SetOutputEnablePin(&gpiops_inst, XGpioPs_I2C_CfgTable[i].I2C_SDA, EMIO_INPUT);
		//将SCLK和SDA都拉高
		XGpioPs_WritePin(&gpiops_inst, XGpioPs_I2C_CfgTable[i].I2C_SCL, 1);
		XGpioPs_WritePin(&gpiops_inst, XGpioPs_I2C_CfgTable[i].I2C_SDA, 1);
	}

	return XST_SUCCESS ;
}

//产生起始信号
void emio_i2c_start(emio_i2c_no i2c)
{
	u32 sda;
	u32 scl;
	sda=XGpioPs_I2C_CfgTable[i2c].I2C_SDA;
	scl=XGpioPs_I2C_CfgTable[i2c].I2C_SCL;

	//设置 gpio端口 为输出
	XGpioPs_SetDirectionPin(&gpiops_inst, scl, EMIO_OUTPUT);
	XGpioPs_SetDirectionPin(&gpiops_inst, sda, EMIO_OUTPUT);
	//使能 gpio端口 输出
	XGpioPs_SetOutputEnablePin(&gpiops_inst, scl, EMIO_OUTPUT);
	XGpioPs_SetOutputEnablePin(&gpiops_inst, sda, EMIO_OUTPUT);
	// scl输出高， sda输出高
	XGpioPs_WritePin(&gpiops_inst, scl, 1);
	XGpioPs_WritePin(&gpiops_inst, sda, 1);

	usleep(4);

 	XGpioPs_WritePin(&gpiops_inst, sda, 0);  //START:when CLK is high,DATA change form high to low

 	usleep(4);

 	XGpioPs_WritePin(&gpiops_inst, scl, 0);  //钳住I2C总线，准备发送或接收数据
}

//产生停止信号
void emio_i2c_stop(emio_i2c_no i2c)
{
	u32 sda;
	u32 scl;
	sda=XGpioPs_I2C_CfgTable[i2c].I2C_SDA;
	scl=XGpioPs_I2C_CfgTable[i2c].I2C_SCL;

	//STOP:when CLK is high DATA change form low to high
	XGpioPs_WritePin(&gpiops_inst, scl, 0);
	XGpioPs_WritePin(&gpiops_inst, sda, 0);

	//设置 gpio端口 为输出
	XGpioPs_SetDirectionPin(&gpiops_inst, scl, EMIO_OUTPUT);
	XGpioPs_SetDirectionPin(&gpiops_inst, sda, EMIO_OUTPUT);
	//使能 gpio端口 输出
	XGpioPs_SetOutputEnablePin(&gpiops_inst, scl, EMIO_OUTPUT);
	XGpioPs_SetOutputEnablePin(&gpiops_inst, sda, EMIO_OUTPUT);

	XGpioPs_WritePin(&gpiops_inst, scl, 0);
	XGpioPs_WritePin(&gpiops_inst, sda, 0);

 	usleep(4);

	XGpioPs_WritePin(&gpiops_inst, scl, 1);

	usleep(4);

	XGpioPs_WritePin(&gpiops_inst, sda, 1);  //发送I2C总线结束信号

	usleep(4);
	XGpioPs_SetDirectionPin(&gpiops_inst, scl, EMIO_INPUT);
	XGpioPs_SetDirectionPin(&gpiops_inst, sda, EMIO_INPUT);
	XGpioPs_SetOutputEnablePin(&gpiops_inst, scl, EMIO_INPUT);
	XGpioPs_SetOutputEnablePin(&gpiops_inst, sda, EMIO_INPUT);
}

//产生ACK应答
void emio_i2c_ack(emio_i2c_no i2c)
{
	u32 sda;
	u32 scl;
	sda=XGpioPs_I2C_CfgTable[i2c].I2C_SDA;
	scl=XGpioPs_I2C_CfgTable[i2c].I2C_SCL;

	//设置 gpio端口 为输出
	XGpioPs_SetDirectionPin(&gpiops_inst, scl, EMIO_OUTPUT);
	XGpioPs_SetDirectionPin(&gpiops_inst, sda, EMIO_OUTPUT);
	//使能 gpio端口 输出
	XGpioPs_SetOutputEnablePin(&gpiops_inst, scl, EMIO_OUTPUT);
	XGpioPs_SetOutputEnablePin(&gpiops_inst, sda, EMIO_OUTPUT);

	XGpioPs_WritePin(&gpiops_inst, scl, 0);
	XGpioPs_WritePin(&gpiops_inst, sda, 0);

	usleep(4);

	XGpioPs_WritePin(&gpiops_inst, scl, 1);

	usleep(4);

	XGpioPs_WritePin(&gpiops_inst, scl, 0);

	usleep(4);

// sda 设置为输入
	XGpioPs_SetDirectionPin(&gpiops_inst, scl, EMIO_OUTPUT);
	XGpioPs_SetDirectionPin(&gpiops_inst, sda, EMIO_INPUT);
	XGpioPs_SetOutputEnablePin(&gpiops_inst, scl, EMIO_OUTPUT);
	XGpioPs_SetOutputEnablePin(&gpiops_inst, sda, EMIO_INPUT);
}

//产生NACK应答
void emio_i2c_nack(emio_i2c_no i2c)
{
	u32 sda;
	u32 scl;
	sda=XGpioPs_I2C_CfgTable[i2c].I2C_SDA;
	scl=XGpioPs_I2C_CfgTable[i2c].I2C_SCL;

	XGpioPs_SetDirectionPin(&gpiops_inst, sda, EMIO_OUTPUT);  	//SDA设置为输出
	XGpioPs_SetDirectionPin(&gpiops_inst, scl, EMIO_OUTPUT);	//SCL设置为输出
	XGpioPs_SetOutputEnablePin(&gpiops_inst, sda, EMIO_OUTPUT);	//使能SDA输出
	XGpioPs_SetOutputEnablePin(&gpiops_inst, scl, EMIO_OUTPUT);	//使能SCL输出

	XGpioPs_WritePin(&gpiops_inst, scl, 0);
	XGpioPs_WritePin(&gpiops_inst, sda, 1);

	usleep(4);

	XGpioPs_WritePin(&gpiops_inst, scl, 1);

	usleep(4);

	XGpioPs_WritePin(&gpiops_inst, scl, 0);

	usleep(4);

	// sda 设置为输入
	XGpioPs_SetDirectionPin(&gpiops_inst, scl, EMIO_OUTPUT);
	XGpioPs_SetDirectionPin(&gpiops_inst, sda, EMIO_INPUT);
	XGpioPs_SetOutputEnablePin(&gpiops_inst, scl, EMIO_OUTPUT);
	XGpioPs_SetOutputEnablePin(&gpiops_inst, sda, EMIO_INPUT);
}

//发送一个字节
void emio_i2c_send_byte(emio_i2c_no i2c, u8 txd)
{
    u8 t;
	u32 sda;
	u32 scl;
	sda=XGpioPs_I2C_CfgTable[i2c].I2C_SDA;
	scl=XGpioPs_I2C_CfgTable[i2c].I2C_SCL;

    XGpioPs_SetDirectionPin(&gpiops_inst, sda, EMIO_OUTPUT);//SDA设置为输出
    XGpioPs_SetDirectionPin(&gpiops_inst, scl, EMIO_OUTPUT);//SCL设置为输出
	XGpioPs_SetOutputEnablePin(&gpiops_inst, sda, EMIO_OUTPUT);//使能SDA输出
	XGpioPs_SetOutputEnablePin(&gpiops_inst, scl, EMIO_OUTPUT);//使能SCL输出

    XGpioPs_WritePin(&gpiops_inst, scl, 0);  //拉低时钟开始数据传输

    for(t=0; t<8; t++)
    {
        XGpioPs_WritePin(&gpiops_inst, sda, (txd&0x80)>>7);

        txd<<=1;

        usleep(2);

        XGpioPs_WritePin(&gpiops_inst, scl, 1);

        usleep(4);

        XGpioPs_WritePin(&gpiops_inst, scl, 0);

        usleep(2);
    }
	// sda 设置为输入
	XGpioPs_SetDirectionPin(&gpiops_inst, scl, EMIO_OUTPUT);
	XGpioPs_SetDirectionPin(&gpiops_inst, sda, EMIO_INPUT);
	XGpioPs_SetOutputEnablePin(&gpiops_inst, scl, EMIO_OUTPUT);
	XGpioPs_SetOutputEnablePin(&gpiops_inst, sda, EMIO_INPUT);
}

//接收一个字节
u8 emio_i2c_recv_byte(emio_i2c_no i2c)
{
	unsigned char i=0 , rxd=0;
	u32 sda;
	u32 scl;
	sda=XGpioPs_I2C_CfgTable[i2c].I2C_SDA;
	scl=XGpioPs_I2C_CfgTable[i2c].I2C_SCL;

	// sda 设置为输入
	XGpioPs_SetOutputEnablePin(&gpiops_inst, sda, EMIO_INPUT);
	XGpioPs_SetDirectionPin(&gpiops_inst, sda, EMIO_INPUT);
	XGpioPs_SetDirectionPin(&gpiops_inst, scl, EMIO_OUTPUT);
	XGpioPs_SetOutputEnablePin(&gpiops_inst, scl, EMIO_OUTPUT);

	XGpioPs_WritePin(&gpiops_inst, scl, 0);
	usleep(4);

	for(i=0;i<8;i++ )
	{
        XGpioPs_WritePin(&gpiops_inst, scl, 1);
        usleep(2);

        rxd <<= 1;
        if( XGpioPs_ReadPin(&gpiops_inst, sda) ) {
        	rxd = rxd | 0x01;
        }
		usleep(2);

		XGpioPs_WritePin(&gpiops_inst, scl, 0);
		usleep(4);
    }

    XGpioPs_SetDirectionPin(&gpiops_inst, sda, EMIO_OUTPUT);//SDA设置为输出
	XGpioPs_SetOutputEnablePin(&gpiops_inst, sda, EMIO_OUTPUT);//使能SDA输出

    return rxd;
}

u8 emio_i2c_recv_ack(emio_i2c_no i2c, emio_stretch_mode st_mode)
{
	u8 check;
	u32 ucErrTime=0;
	u32 sda;
	u32 scl;
	sda=XGpioPs_I2C_CfgTable[i2c].I2C_SDA;
	scl=XGpioPs_I2C_CfgTable[i2c].I2C_SCL;

	XGpioPs_SetOutputEnablePin(&gpiops_inst, sda, EMIO_INPUT);
	XGpioPs_SetDirectionPin(&gpiops_inst, sda, EMIO_INPUT);//SDA设置为输入
	XGpioPs_SetDirectionPin(&gpiops_inst, scl, EMIO_OUTPUT);//SCL设置为输出
	XGpioPs_SetOutputEnablePin(&gpiops_inst, scl, EMIO_OUTPUT);//使能SCL输出
	XGpioPs_WritePin(&gpiops_inst, scl, 0);

	if(st_mode)
	{
		usleep(10);
	}
	else
	{
		usleep(4);
	}

	if(st_mode)
	{
		XGpioPs_SetOutputEnablePin(&gpiops_inst, scl, EMIO_INPUT);
		XGpioPs_SetDirectionPin(&gpiops_inst, scl, EMIO_INPUT);//SCL设置为输入

		XGpioPs_WritePin(&gpiops_inst, scl, 0);
		while(XGpioPs_ReadPin(&gpiops_inst, scl) == 0)
		{
	      ucErrTime++;
	      usleep(1);
	      if(ucErrTime>100000) // 空设备一路就要0.5秒以上
	      {
	        ucErrTime = 0;
	        break;
	      }
	    }
	    usleep(10);
	}
	else
	{
//		XGpioPs_SetDirectionPin(&gpiops_inst, scl, EMIO_OUTPUT);//SCL设置为输出
//		XGpioPs_SetOutputEnablePin(&gpiops_inst, scl, EMIO_OUTPUT);//使能SCL输出
		XGpioPs_WritePin(&gpiops_inst, scl, 1);
		usleep(2);
	}

	check = 0;
	if(XGpioPs_ReadPin(&gpiops_inst, sda) == 1)
	{
		check = 1;
	}

	if(st_mode)
	{
		XGpioPs_SetDirectionPin(&gpiops_inst, scl, EMIO_OUTPUT);//SCL设置为输出
		XGpioPs_SetOutputEnablePin(&gpiops_inst, scl, EMIO_OUTPUT);//使能SCL输出
	}
	else
	{
		usleep(4);
	}
	XGpioPs_SetDirectionPin(&gpiops_inst, scl, 1);//SCL设置为输出
	XGpioPs_SetOutputEnablePin(&gpiops_inst, scl, 1);//使能SCL输出
	XGpioPs_WritePin(&gpiops_inst, scl, 0);//拉低SCL
	usleep(2);

	if(st_mode)
	{
		XGpioPs_WritePin(&gpiops_inst, sda, 1);
		XGpioPs_WritePin(&gpiops_inst, scl, 0);//拉低SCL
	}

	XGpioPs_SetDirectionPin(&gpiops_inst, sda, EMIO_OUTPUT);//SDA设置为输出
	XGpioPs_SetOutputEnablePin(&gpiops_inst, sda, EMIO_OUTPUT);//使能SDA输出

	XGpioPs_WritePin(&gpiops_inst, sda, 1);
	XGpioPs_WritePin(&gpiops_inst, scl, 0);//拉低SCL

	return check; 
}

int emio_i2c_reg8_write(emio_i2c_no i2c, char IIC_ADDR, char Addr, char Data, emio_stretch_mode st_mode)
{
	u8 ack=0;

	i2c_start(i2c);

	i2c_send_byte(i2c, IIC_ADDR<<1);
	ack=i2c_recv_ack(i2c, st_mode);
	if(ack)
	{
		i2c_stop(i2c);
		return XST_FAILURE;
	}

	i2c_send_byte(i2c, Addr);
	ack=i2c_recv_ack(i2c, st_mode);
	if(ack)
	{
		i2c_stop(i2c);
		return XST_FAILURE;
	}

	i2c_send_byte(i2c, Data);
	ack=i2c_recv_ack(i2c, st_mode);
	if(ack)
	{
		i2c_stop(i2c);
		return XST_FAILURE;
	}

	i2c_stop(i2c);

	return XST_SUCCESS;
}

// 7-bit addr
int emio_i2c_reg8_read(emio_i2c_no i2c, char IIC_ADDR, char Addr, u8 * ret, emio_stretch_mode st_mode)
{
	u8 rxd;
	u8 ack=0;

	i2c_start(i2c);

	i2c_send_byte(i2c, IIC_ADDR<<1);
	ack=i2c_recv_ack(i2c, st_mode);
	if(ack)
	{
		i2c_stop(i2c);
		return XST_FAILURE;
	}

	i2c_send_byte(i2c, Addr);
	ack=i2c_recv_ack(i2c, st_mode);
	if(ack)
	{
		i2c_stop(i2c);
		return XST_FAILURE;
	}

	//i2c_stop(i2c);

  	i2c_start(i2c);

  	i2c_send_byte(i2c, IIC_ADDR<<1 | 0x01);
  	ack=i2c_recv_ack(i2c, st_mode);
	if(ack)
	{
		i2c_stop(i2c);
		return XST_FAILURE;
	}

	rxd = i2c_recv_byte(i2c);
	i2c_nack(i2c);

  	i2c_stop(i2c);

  	*ret = rxd;

  	return XST_SUCCESS;
}

int emio_i2c_reg16_write(emio_i2c_no i2c, char IIC_ADDR, unsigned short Addr, char Data, emio_stretch_mode st_mode)
{
	u8 ack=0;

	i2c_start(i2c);

	i2c_send_byte(i2c, IIC_ADDR<<1);
	ack=i2c_recv_ack(i2c, st_mode);
	if(ack)
	{
		i2c_stop(i2c);
		return XST_FAILURE;
	}

	i2c_send_byte(i2c, Addr >> 8);
	ack=i2c_recv_ack(i2c, st_mode);
	if(ack)
	{
		i2c_stop(i2c);
		return XST_FAILURE;
	}

	i2c_send_byte(i2c, Addr & 0x00FF);
	ack=i2c_recv_ack(i2c, st_mode);
	if(ack)
	{
		i2c_stop(i2c);
		return XST_FAILURE;
	}

	i2c_send_byte(i2c, Data);
	ack=i2c_recv_ack(i2c, st_mode);
	if(ack)
	{
		i2c_stop(i2c);
		return XST_FAILURE;
	}

	i2c_stop(i2c);

	return XST_SUCCESS;
}

int emio_i2c_reg16_read(emio_i2c_no i2c, char IIC_ADDR, unsigned short Addr, u8 * ret, emio_stretch_mode st_mode)
{
	u8 rxd;
	u8 ack=0;

	i2c_start(i2c);

	i2c_send_byte(i2c, IIC_ADDR<<1);
//	i2c_ack(i2c);
	ack=i2c_recv_ack(i2c, st_mode);
	if(ack)
	{
		i2c_stop(i2c);
		return XST_FAILURE;
	}

	i2c_send_byte(i2c, Addr >> 8);
//	i2c_ack(i2c);
	ack=i2c_recv_ack(i2c, st_mode);
	if(ack)
	{
		i2c_stop(i2c);
		return XST_FAILURE;
	}

	i2c_send_byte(i2c, Addr & 0x00FF);
//	i2c_ack(i2c);
	ack=i2c_recv_ack(i2c, st_mode);
	if(ack)
	{
		i2c_stop(i2c);
		return XST_FAILURE;
	}

	//i2c_stop(i2c);

  	i2c_start(i2c);

  	i2c_send_byte(i2c, IIC_ADDR<<1 | 0x01);
//  	i2c_ack(i2c);
  	ack=i2c_recv_ack(i2c, st_mode);
	if(ack)
	{
		i2c_stop(i2c);
		return XST_FAILURE;
	}

	rxd = i2c_recv_byte(i2c);
	i2c_nack(i2c);
//	i2c_ack(i2c);

  	i2c_stop(i2c);

  	*ret = rxd;

  	return  XST_SUCCESS ;
}

#if 0
int emio_i2c_32b32_write(emio_i2c_no i2c, char IIC_ADDR, unsigned int Addr, unsigned int Data, stretch_mode st_mode)
{
	u8 ack=0;

	i2c_start(i2c);

	i2c_send_byte(i2c, IIC_ADDR<<1);
	ack=i2c_recv_ack(i2c, st_mode);
	if(ack)
	{
		i2c_stop(i2c);
		return XST_FAILURE;
	}

	i2c_send_byte(i2c, Addr >> 24);
	ack=i2c_recv_ack(i2c, st_mode);
	if(ack)
	{
		i2c_stop(i2c);
		return XST_FAILURE;
	}
	i2c_send_byte(i2c, Addr >> 16);
	ack=i2c_recv_ack(i2c, st_mode);
	if(ack)
	{
		i2c_stop(i2c);
		return XST_FAILURE;
	}
	i2c_send_byte(i2c, Addr >> 8);
	ack=i2c_recv_ack(i2c, st_mode);
	if(ack)
	{
		i2c_stop(i2c);
		return XST_FAILURE;
	}

	i2c_send_byte(i2c, Addr & 0x00FF);
	ack=i2c_recv_ack(i2c, st_mode);
	if(ack)
	{
		i2c_stop(i2c);
		return XST_FAILURE;
	}

	i2c_send_byte(i2c, Data>>24);
	ack=i2c_recv_ack(i2c, st_mode);
	if(ack)
	{
		i2c_stop(i2c);
		return XST_FAILURE;
	}

	i2c_send_byte(i2c, Data>>16);
	ack=i2c_recv_ack(i2c, st_mode);
	if(ack)
	{
		i2c_stop(i2c);
		return XST_FAILURE;
	}

	i2c_send_byte(i2c, Data>>8);
	ack=i2c_recv_ack(i2c, st_mode);
	if(ack)
	{
		i2c_stop(i2c);
		return XST_FAILURE;
	}

	i2c_send_byte(i2c, Data);
	ack=i2c_recv_ack(i2c, st_mode);
	if(ack)
	{
		i2c_stop(i2c);
		return XST_FAILURE;
	}

	i2c_stop(i2c);

	return XST_SUCCESS;
}

int emio_i2c_32b32_read(emio_i2c_no i2c, char IIC_ADDR, unsigned int Addr, unsigned int * ret, stretch_mode st_mode)
{
	u32 rxd;
	u8 ack=0;

	i2c_start(i2c);

	i2c_send_byte(i2c, IIC_ADDR<<1);
	ack=i2c_recv_ack(i2c, st_mode);
	if(ack)
	{
		i2c_stop(i2c);
		return XST_FAILURE;
	}

	i2c_send_byte(i2c, Addr >> 24);
	ack=i2c_recv_ack(i2c, st_mode);
	if(ack)
	{
		i2c_stop(i2c);
		return XST_FAILURE;
	}
	i2c_send_byte(i2c, Addr >> 16);
	ack=i2c_recv_ack(i2c, st_mode);
	if(ack)
	{
		i2c_stop(i2c);
		return XST_FAILURE;
	}
	i2c_send_byte(i2c, Addr >> 8);
	ack=i2c_recv_ack(i2c, st_mode);
	if(ack)
	{
		i2c_stop(i2c);
		return XST_FAILURE;
	}

	i2c_send_byte(i2c, Addr & 0x00FF);
	ack=i2c_recv_ack(i2c, st_mode);
	if(ack)
	{
		i2c_stop(i2c);
		return XST_FAILURE;
	}

	//i2c_stop(i2c);

  	i2c_start(i2c);

  	i2c_send_byte(i2c, IIC_ADDR<<1 | 0x01);
//  	i2c_ack(i2c);
  	ack=i2c_recv_ack(i2c, st_mode);
	if(ack)
	{
		i2c_stop(i2c);
		return XST_FAILURE;
	}

	rxd = i2c_recv_byte(i2c);
//	i2c_nack(i2c);
	i2c_ack(i2c);
	usleep(110);

	rxd += i2c_recv_byte(i2c)<<8;
//	i2c_nack(i2c);
	i2c_ack(i2c);
	usleep(110);

	rxd += i2c_recv_byte(i2c)<<16;
//	i2c_nack(i2c);
	i2c_ack(i2c);
	usleep(110);

	rxd += i2c_recv_byte(i2c)<<24;
	i2c_nack(i2c);
//	i2c_ack(i2c);

  	i2c_stop(i2c);

  	*ret = rxd;

  	return  XST_SUCCESS ;
}
#endif

#endif // #if defined (XPAR_XGPIOPS_NUM_INSTANCES) && !(EMIO_I2C_NO_BUTT == 0)


/*
usage:
assume you have a emio_i2c_heir
call follows before main_loop
```
#if defined (XPAR_XGPIOPS_NUM_INSTANCES)
    emio_init();
#endif // XPAR_XGPIOPS_NUM_INSTANCES
```
*/

