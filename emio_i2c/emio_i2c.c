#include "../bsp.h"
#if defined (XPAR_XGPIOPS_NUM_INSTANCES)

#define  GPIOPS_ID  XPAR_XGPIOPS_0_DEVICE_ID  //PS 端 GPIO 器件 ID
static  XGpioPs  gpiops_inst; //PS 端 GPIO 驱动实例

XGpioPs_I2C_Cfg XGpioPs_I2C_CfgTable[I2C_NO_BUTT] =
{
	{I2C_NO_0, EMIO_SCL0_NUM, EMIO_SDA0_NUM},
	{I2C_NO_1, EMIO_SCL1_NUM, EMIO_SDA1_NUM},
	{I2C_NO_2, EMIO_SCL2_NUM, EMIO_SDA2_NUM},
	{I2C_NO_3, EMIO_SCL3_NUM, EMIO_SDA3_NUM},
	{I2C_NO_4, EMIO_SCL4_NUM, EMIO_SDA4_NUM},
	{I2C_NO_5, EMIO_SCL5_NUM, EMIO_SDA5_NUM},
	{I2C_NO_6, EMIO_SCL6_NUM, EMIO_SDA6_NUM},
	{I2C_NO_7, EMIO_SCL7_NUM, EMIO_SDA7_NUM}
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

	for(int i=0; i<I2C_NO_BUTT; i++)
	{
		//设置 gpio端口 为输出
		XGpioPs_SetDirectionPin(&gpiops_inst, XGpioPs_I2C_CfgTable[i].I2C_SCL, 1);
		XGpioPs_SetDirectionPin(&gpiops_inst, XGpioPs_I2C_CfgTable[i].I2C_SDA, 1);
		//使能 gpio端口 输出
		XGpioPs_SetOutputEnablePin(&gpiops_inst, XGpioPs_I2C_CfgTable[i].I2C_SCL, 1);
		XGpioPs_SetOutputEnablePin(&gpiops_inst, XGpioPs_I2C_CfgTable[i].I2C_SDA, 1);
		//将SCLK和SDA都拉高
		XGpioPs_WritePin(&gpiops_inst, XGpioPs_I2C_CfgTable[i].I2C_SCL, 1);
		XGpioPs_WritePin(&gpiops_inst, XGpioPs_I2C_CfgTable[i].I2C_SDA, 1);
	}

	return XST_SUCCESS ;
}

//产生起始信号
void i2c_start(i2c_no i2c)
{
	u32 sda;
	u32 scl;
	sda=XGpioPs_I2C_CfgTable[i2c].I2C_SDA;
	scl=XGpioPs_I2C_CfgTable[i2c].I2C_SCL;

	//设置 gpio端口 为输出
	XGpioPs_SetDirectionPin(&gpiops_inst, scl, 1);
	XGpioPs_SetDirectionPin(&gpiops_inst, sda, 1);
	//使能 gpio端口 输出
	XGpioPs_SetOutputEnablePin(&gpiops_inst, scl, 1);
	XGpioPs_SetOutputEnablePin(&gpiops_inst, sda, 1);

	XGpioPs_WritePin(&gpiops_inst, scl, 1);
	XGpioPs_WritePin(&gpiops_inst, sda, 1);

	usleep(4);

 	XGpioPs_WritePin(&gpiops_inst, sda, 0);  //START:when CLK is high,DATA change form high to low

 	usleep(4);

 	XGpioPs_WritePin(&gpiops_inst, scl, 0);  //钳住I2C总线，准备发送或接收数据
}

//产生停止信号
void i2c_stop(i2c_no i2c)
{
	u32 sda;
	u32 scl;
	sda=XGpioPs_I2C_CfgTable[i2c].I2C_SDA;
	scl=XGpioPs_I2C_CfgTable[i2c].I2C_SCL;

	XGpioPs_WritePin(&gpiops_inst, scl, 0);

	XGpioPs_WritePin(&gpiops_inst, sda, 0);  //STOP:when CLK is high DATA change form low to high

 	usleep(4);

	XGpioPs_WritePin(&gpiops_inst, scl, 1);

	usleep(4);

	XGpioPs_WritePin(&gpiops_inst, sda, 1);  //发送I2C总线结束信号

}

//产生ACK应答
void i2c_ack(i2c_no i2c)
{
	u32 sda;
	u32 scl;
	sda=XGpioPs_I2C_CfgTable[i2c].I2C_SDA;
	scl=XGpioPs_I2C_CfgTable[i2c].I2C_SCL;

	XGpioPs_SetDirectionPin(&gpiops_inst, sda, 1);  //SDA设置为输出
	XGpioPs_SetOutputEnablePin(&gpiops_inst, sda, 1);//使能SDA输出
	XGpioPs_SetDirectionPin(&gpiops_inst, scl, 1);//SCL设置为输出
	XGpioPs_SetOutputEnablePin(&gpiops_inst, scl, 1);//使能SCL输出

	XGpioPs_WritePin(&gpiops_inst, scl, 0);
	XGpioPs_WritePin(&gpiops_inst, sda, 0);

	usleep(4);

	XGpioPs_WritePin(&gpiops_inst, scl, 1);

	usleep(4);

	XGpioPs_WritePin(&gpiops_inst, scl, 0);

	usleep(4);
}

//产生NACK应答
void i2c_nack(i2c_no i2c)
{
	u32 sda;
	u32 scl;
	sda=XGpioPs_I2C_CfgTable[i2c].I2C_SDA;
	scl=XGpioPs_I2C_CfgTable[i2c].I2C_SCL;

	XGpioPs_SetDirectionPin(&gpiops_inst, sda, 1);  //SDA设置为输出
	XGpioPs_SetOutputEnablePin(&gpiops_inst, sda, 1);//使能SDA输出
	XGpioPs_SetDirectionPin(&gpiops_inst, scl, 1);//SCL设置为输出
	XGpioPs_SetOutputEnablePin(&gpiops_inst, scl, 1);//使能SCL输出

	XGpioPs_WritePin(&gpiops_inst, scl, 0);
	XGpioPs_WritePin(&gpiops_inst, sda, 1);

	usleep(4);

	XGpioPs_WritePin(&gpiops_inst, scl, 1);

	usleep(4);

	XGpioPs_WritePin(&gpiops_inst, scl, 0);

	usleep(4);
}

//发送一个字节
void i2c_send_byte(i2c_no i2c, u8 txd)
{
    u8 t;
	u32 sda;
	u32 scl;
	sda=XGpioPs_I2C_CfgTable[i2c].I2C_SDA;
	scl=XGpioPs_I2C_CfgTable[i2c].I2C_SCL;

    XGpioPs_SetDirectionPin(&gpiops_inst, sda, 1);//SDA设置为输出
	XGpioPs_SetOutputEnablePin(&gpiops_inst, sda, 1);//使能SDA输出
    XGpioPs_SetDirectionPin(&gpiops_inst, scl, 1);//SCL设置为输出
	XGpioPs_SetOutputEnablePin(&gpiops_inst, scl, 1);//使能SCL输出

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
	XGpioPs_SetOutputEnablePin(&gpiops_inst, sda, 0);
    XGpioPs_SetDirectionPin(&gpiops_inst, sda, 0);  //SDA设置为输入
}

//接收一个字节
u8  i2c_recv_byte(i2c_no i2c)
{
	unsigned char i=0 , rxd=0;
	u32 sda;
	u32 scl;
	sda=XGpioPs_I2C_CfgTable[i2c].I2C_SDA;
	scl=XGpioPs_I2C_CfgTable[i2c].I2C_SCL;

	XGpioPs_SetOutputEnablePin(&gpiops_inst, sda, 0);
	XGpioPs_SetDirectionPin(&gpiops_inst, sda, 0);//SDA设置为输入
	XGpioPs_SetDirectionPin(&gpiops_inst, scl, 1);//SCL设置为输出
	XGpioPs_SetOutputEnablePin(&gpiops_inst, scl, 1);//使能SCL输出
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

	XGpioPs_SetDirectionPin(&gpiops_inst, sda, 1);  //SDA设置为输出
	XGpioPs_SetOutputEnablePin(&gpiops_inst, sda, 1);//使能SDA输出

    return rxd;

}

u8  i2c_recv_ack(i2c_no i2c)
{
	u8 check;
	u32 ucErrTime=0;
	u32 sda;
	u32 scl;
	sda=XGpioPs_I2C_CfgTable[i2c].I2C_SDA;
	scl=XGpioPs_I2C_CfgTable[i2c].I2C_SCL;

	XGpioPs_SetOutputEnablePin(&gpiops_inst, sda, 0);
	XGpioPs_SetDirectionPin(&gpiops_inst, sda, 0);//SDA设置为输入
	XGpioPs_SetDirectionPin(&gpiops_inst, scl, 1);//SCL设置为输出
	XGpioPs_SetOutputEnablePin(&gpiops_inst, scl, 1);//使能SCL输出
	XGpioPs_WritePin(&gpiops_inst, scl, 0);

	usleep(10);

	XGpioPs_SetOutputEnablePin(&gpiops_inst, scl, 0);
	XGpioPs_SetDirectionPin(&gpiops_inst, scl, 0);//SCL设置为输入

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
	
	check = 0;
	if(XGpioPs_ReadPin(&gpiops_inst, sda) == 1)
	{
		check = 1;
	}

	XGpioPs_SetDirectionPin(&gpiops_inst, scl, 1);//SCL设置为输出
	XGpioPs_SetOutputEnablePin(&gpiops_inst, scl, 1);//使能SCL输出
	XGpioPs_WritePin(&gpiops_inst, scl, 0);//拉低SCL
	usleep(2);

	XGpioPs_SetDirectionPin(&gpiops_inst, sda, 1);//SDA设置为输出
	XGpioPs_SetOutputEnablePin(&gpiops_inst, sda, 1);//使能SDA输出
	
	return check; 
}

int emio_i2c_reg8_write(i2c_no i2c, char IIC_ADDR, char Addr, char Data)
{
	u8 ack=0;

	i2c_start(i2c);

	i2c_send_byte(i2c, IIC_ADDR<<1);
	ack=i2c_recv_ack(i2c);
	if(ack)
	{
		return XST_FAILURE;
	}

	i2c_send_byte(i2c, Addr);
	ack=i2c_recv_ack(i2c);
	if(ack)
	{
		return XST_FAILURE;
	}

	i2c_send_byte(i2c, Data);
	ack=i2c_recv_ack(i2c);
	if(ack)
	{
		return XST_FAILURE;
	}

	i2c_stop(i2c);

	return XST_SUCCESS;
}

// 7-bit addr
int emio_i2c_reg8_read(i2c_no i2c, char IIC_ADDR, char Addr, u8 * ret)
{
	u8 rxd;
	u8 ack=0;

	i2c_start(i2c);

	i2c_send_byte(i2c, IIC_ADDR<<1);
	ack=i2c_recv_ack(i2c);
	if(ack)
	{
		return XST_FAILURE;
	}

	i2c_send_byte(i2c, Addr);
	ack=i2c_recv_ack(i2c);
	if(ack)
	{
		return XST_FAILURE;
	}

	//i2c_stop(i2c);

  	i2c_start(i2c);

  	i2c_send_byte(i2c, IIC_ADDR<<1 | 0x01);
  	ack=i2c_recv_ack(i2c);
	if(ack)
	{
		return XST_FAILURE;
	}

	rxd = i2c_recv_byte(i2c);
	i2c_nack(i2c);

  	i2c_stop(i2c);

  	*ret = rxd;

  	return XST_SUCCESS;
}

int emio_i2c_reg16_write(i2c_no i2c, char IIC_ADDR, unsigned short Addr, char Data)
{
	u8 ack=0;

	i2c_start(i2c);

	i2c_send_byte(i2c, IIC_ADDR<<1);
	ack=i2c_recv_ack(i2c);
	if(ack)
	{
		i2c_stop(i2c);
		return XST_FAILURE;
	}

	i2c_send_byte(i2c, Addr >> 8);
	ack=i2c_recv_ack(i2c);
	if(ack)
	{
		i2c_stop(i2c);
		return XST_FAILURE;
	}

	i2c_send_byte(i2c, Addr & 0x00FF);
	ack=i2c_recv_ack(i2c);
	if(ack)
	{
		i2c_stop(i2c);
		return XST_FAILURE;
	}

	i2c_send_byte(i2c, Data);
	ack=i2c_recv_ack(i2c);
	if(ack)
	{
		i2c_stop(i2c);
		return XST_FAILURE;
	}

	i2c_stop(i2c);

	return XST_SUCCESS;
}

int emio_i2c_reg16_read(i2c_no i2c, char IIC_ADDR, unsigned short Addr, u8 * ret)
{
	u8 rxd;
	u8 ack=0;

	i2c_start(i2c);

	i2c_send_byte(i2c, IIC_ADDR<<1);
//	i2c_ack(i2c);
	ack=i2c_recv_ack(i2c);
	if(ack)
	{
		i2c_stop(i2c);
		return XST_FAILURE;
	}

	i2c_send_byte(i2c, Addr >> 8);
//	i2c_ack(i2c);
	ack=i2c_recv_ack(i2c);
	if(ack)
	{
		i2c_stop(i2c);
		return XST_FAILURE;
	}

	i2c_send_byte(i2c, Addr & 0x00FF);
//	i2c_ack(i2c);
	ack=i2c_recv_ack(i2c);
	if(ack)
	{
		i2c_stop(i2c);
		return XST_FAILURE;
	}

	//i2c_stop(i2c);

  	i2c_start(i2c);

  	i2c_send_byte(i2c, IIC_ADDR<<1 | 0x01);
//  	i2c_ack(i2c);
  	ack=i2c_recv_ack(i2c);
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

#endif // XPAR_XGPIOPS_NUM_INSTANCES

