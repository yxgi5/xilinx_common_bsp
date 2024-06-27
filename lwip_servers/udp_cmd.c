#include "../bsp.h"

#if defined (XPAR_XEMACPS_NUM_INSTANCES) || defined (XPAR_XAXIETHERNET_NUM_INSTANCES)
#if defined (UDP_COMMAND_SRV)

//static struct udp_pcb *client_pcb = NULL;

void print_udp_cmd_header(void)
{
    bsp_printf("%20s %6d\r\n", "UDP CMD", UDP_CMD_SVR_PORT);
}

static uint8_t checksum(uint8_t * ptr, int16_t cnt)
{
	int16_t i = 0;
	uint8_t calc=0;
	for(i=0;i<cnt;i++)
	{
		calc=calc + ptr[i];
	}

	return calc=~calc&0xff;
}


uint8_t receivebuf[1500] = {0};
uint8_t send_buf[1500] = {0};
int receivelen = 0;
int sendlen = 0;
u8 cerrent_ch;
u8 reset_pl;

void udp_cmd_recv_callback(void *arg, struct udp_pcb *upcb, struct pbuf *p,
		const ip_addr_t *addr, u16_t port)
{
    struct pbuf *pq;
	receivelen = p->len;
	memset(receivebuf, 0, sizeof(receivebuf));
	memset(send_buf, 0, sizeof(send_buf));
	memcpy(receivebuf,p->payload,p->len);
    pbuf_free(p);
    int32_t Status=0;
    uint16_t msg_len=0;
    uint16_t msg_cmd=0;
    uint32_t msg_addr=0;
    uint32_t msg_value=0;
    uint8_t msg_ack=0;
    uint16_t mem_len=0;

    memcpy(&msg_len,receivebuf,2);
    memcpy(&msg_cmd,receivebuf+4,1);

    if(msg_cmd==0x10)	// read mem addr
    {
    	memcpy(&msg_addr,receivebuf+5,4);
    	msg_value = Xil_In32(msg_addr); // don't use signed int, otherwise hardfail
    	//mdata=htonl(mdata);
    	memcpy(send_buf,receivebuf,receivelen);
    	sendlen=receivelen;
    	//memcpy(send_buf,&sendlen,2);
    	memcpy(send_buf+9,&msg_value,4);
    	send_buf[sendlen-1] = checksum(send_buf,sendlen-1); // �����checksum���ȫ��
    	memcpy(send_buf,&sendlen,2);
    }
    if(msg_cmd==0x11)	// write mem addr
	{
		memcpy(&msg_addr,receivebuf+5,4);
		memcpy(&msg_value,receivebuf+9,4);
		//msg_value=htonl(msg_value);
		//*(volatile unsigned int *)(msg_addr) = msg_value;
		Xil_Out32(msg_addr, msg_value);
		msg_value = Xil_In32(msg_addr); // don't use signed int, otherwise hardfail

		memcpy(send_buf,receivebuf,receivelen);
		sendlen=receivelen;
		//memcpy(send_buf,&sendlen,2);
		memcpy(send_buf+9,&msg_value,4);
		send_buf[sendlen-1] = checksum(send_buf,sendlen-1); // �����checksum���ȫ��
		memcpy(send_buf,&sendlen,2);
	}
    if(msg_cmd==0x12)	// read mem bulk
	{
		memcpy(&msg_addr,receivebuf+5,4);
		memcpy(&mem_len,receivebuf+9,4);
		memcpy(send_buf,receivebuf,receivelen);
		memcpy(send_buf+receivelen-1,(void*)msg_addr,mem_len*4);
		sendlen=receivelen+mem_len*4;
		memcpy(send_buf,&sendlen,2);
		send_buf[sendlen-1] = checksum(send_buf,sendlen-1); // �����checksum���ȫ��
	}
    if(msg_cmd==0x13)	// write mem bulk
	{
		memcpy(&msg_addr,receivebuf+5,4);
		memcpy(&mem_len,receivebuf+9,4);
		memcpy((void*)msg_addr,receivebuf+13,mem_len*4);
		sendlen=receivelen-mem_len*4;
		memcpy(send_buf,receivebuf,sendlen);
//		memcpy(send_buf,&sendlen,2);
		send_buf[sendlen-1] = checksum(send_buf,sendlen-1); // �����checksum���ȫ��
		memcpy(send_buf,&sendlen,2);
	}
    if(msg_cmd == 0x20)
	{
		uint8_t addr;
		uint8_t ch;
		uint8_t len;
		uint8_t flag;
		uint16_t reg_addr;
		uint8_t data[256];
		ch =receivebuf[5];
		addr = receivebuf[6];
		len = receivebuf[7];
		reg_addr = (uint16_t)receivebuf[8] << 8 | receivebuf[9];
		flag = receivebuf[10];
//		if(flag == 1)
//		{
//			Status = xgpio_i2c_reg8_Continuous_read(ch,addr>>1,reg_addr,data,len,STRETCH_OFF);
//		}
//		else
//		{
//			Status = xgpio_i2c_reg16_Continuous_read(ch,addr>>1,reg_addr,data,len,STRETCH_OFF);
//		}

		memcpy(send_buf,receivebuf,5);
		memcpy(send_buf+5,&Status,1);
		memcpy(send_buf+6,data,len);
		sendlen = 6 + len+1;
		send_buf[sendlen-1] = checksum(send_buf,sendlen-1); //
		memcpy(send_buf,&sendlen,2);
	}
   if(msg_cmd == 0x21)
	{
		printf("recv ch\r\n");
		uint8_t addr;
		uint8_t ch;
		uint8_t value;
		uint8_t flag;
		uint16_t reg_addr;

		addr = receivebuf[6];
		ch =receivebuf[5];
		reg_addr = (uint16_t)receivebuf[7] << 8 | receivebuf[8];
		flag = receivebuf[9];
		value = receivebuf[11];
		if(flag == 1)
		{
//			Status = xgpio_i2c_reg8_write(ch,addr>>1,reg_addr,value,STRETCH_OFF);
		}
		else
		{
//			Status = xgpio_i2c_reg16_write(ch,addr>>1,reg_addr,value,STRETCH_OFF);
		}

		memcpy(send_buf,receivebuf,5);
		memcpy(send_buf+5,&Status,1);
		memcpy(send_buf+6,receivebuf+5,msg_len-5);
		sendlen = msg_len + 1;
		send_buf[sendlen-1] = checksum(send_buf,sendlen-1); //
		memcpy(send_buf,&sendlen,2);
	}

    if(msg_cmd==0x40) // variables get and set
    {
    	uint16_t cmd_index;
    	uint32_t msg_send;
		memcpy(&cmd_index,receivebuf+5,2);

		if(cmd_index==0)
		{
//			uint16_t major = __SW_VER_MAJOR__;
//			uint16_t minor = __SW_VER_MINOR__;
			uint32_t ver = __HW_VER__;
//			msg_send = (uint32_t)(((uint32_t)major)<<16) + (uint32_t)minor;
			msg_send = ver;

			memcpy(send_buf,receivebuf,7);
			memcpy(send_buf+7,&msg_send,4);
			sendlen = 12;
			send_buf[sendlen-1] = checksum(send_buf,sendlen-1); // �����checksum���ȫ��
			memcpy(send_buf,&sendlen,2);
		}
		if(cmd_index==1)
		{
//			uint16_t major = __SW_VER_MAJOR__;
//			uint16_t minor = __SW_VER_MINOR__;
			uint32_t ver = __SW_VER__;
//			msg_send = (uint32_t)(((uint32_t)major)<<16) + (uint32_t)minor;
			msg_send = ver;

			memcpy(send_buf,receivebuf,7);
			memcpy(send_buf+7,&msg_send,4);
			sendlen = 12;
			send_buf[sendlen-1] = checksum(send_buf,sendlen-1); // �����checksum���ȫ��
			memcpy(send_buf,&sendlen,2);
		}
		if(cmd_index==2)
		{
//			uint16_t major = __SW_VER_MAJOR__;
//			uint16_t minor = __SW_VER_MINOR__;
			uint32_t ver = AXI_LITE_REG_mReadReg(XPAR_PROCESSOR_SUBSYSTEM_AXI_LITE_REG_0_S00_AXI_BASEADDR, AXI_LITE_REG_S00_AXI_SLV_REG0_OFFSET);
//			msg_send = (uint32_t)(((uint32_t)major)<<16) + (uint32_t)minor;
			msg_send = ver;

			memcpy(send_buf,receivebuf,7);
			memcpy(send_buf+7,&msg_send,4);
			sendlen = 12;
			send_buf[sendlen-1] = checksum(send_buf,sendlen-1); // �����checksum���ȫ��
			memcpy(send_buf,&sendlen,2);
		}
		if(cmd_index==3)
		{
//			uint16_t major = __SW_VER_MAJOR__;
//			uint16_t minor = __SW_VER_MINOR__;
			uint32_t ver = __SW_VER__;
//			msg_send = (uint32_t)(((uint32_t)major)<<16) + (uint32_t)minor;
			msg_send = ver;

			memcpy(send_buf,receivebuf,7);
			memcpy(send_buf+7,&msg_send,4);
			sendlen = 12;
			send_buf[sendlen-1] = checksum(send_buf,sendlen-1); // �����checksum���ȫ��
			memcpy(send_buf,&sendlen,2);
		}
		if(cmd_index==4)
		{
//			uint16_t major = __SW_VER_MAJOR__;
//			uint16_t minor = __SW_VER_MINOR__;
			uint32_t ver = __SW_VER__;
//			msg_send = (uint32_t)(((uint32_t)major)<<16) + (uint32_t)minor;
			msg_send = ver;

			memcpy(send_buf,receivebuf,7);
			memcpy(send_buf+7,&msg_send,4);
			sendlen = 12;
			send_buf[sendlen-1] = checksum(send_buf,sendlen-1); // �����checksum���ȫ��
			memcpy(send_buf,&sendlen,2);
		}
		if(cmd_index==5)// cur_ch_get
		{
			memcpy(send_buf,receivebuf,7);
			memcpy(send_buf+7,&cerrent_ch,1);
			sendlen = 9;
			send_buf[sendlen-1] = checksum(send_buf,sendlen-1); // �����checksum���ȫ��
			memcpy(send_buf,&sendlen,2);
		}
		if(cmd_index==6) //cur_ch_set
		{
			uint8_t var1;
			memcpy(&var1,receivebuf+7,1);
			cerrent_ch = var1;
			memcpy(send_buf,receivebuf,7);
			memcpy(send_buf+7,&msg_send,1);
			sendlen = 7 + 1 + 1;
			send_buf[sendlen-1] = checksum(send_buf,sendlen-1); // �����checksum���ȫ��
			memcpy(send_buf,&sendlen,2);
		}
		if(cmd_index == 7)
		{
			uint8_t var1;
			memcpy(&var1,receivebuf+7,1);
		//	reset_pl = var1;
			memcpy(send_buf,receivebuf,7);
			memcpy(send_buf+7,&msg_send,1);
			sendlen = 7 + 1 + 1;
			send_buf[sendlen-1] = checksum(send_buf,sendlen-1); // �����checksum���ȫ��
			memcpy(send_buf,&sendlen,2);
		}
		if(cmd_index==10) // ��ѯglobal_config
		{
			memcpy(send_buf,receivebuf,7);
		//	memcpy(send_buf+7, &global_config, sizeof(config_Settings_t));
		//	sendlen = 7 + sizeof(config_Settings_t) + 1;
			send_buf[sendlen-1] = checksum(send_buf,sendlen-1); // �����checksum���ȫ��
			memcpy(send_buf,&sendlen,2);
		}
		if(cmd_index==11) // д��global_config
		{
			int32_t Status;
			uint8_t sendmsg;
		//	memcpy(&global_config,receivebuf+7,sizeof(config_Settings_t));
//			memcpy(&(global_config.frequency_Hz),receivebuf+29,sizeof(int64_t));
//			memcpy(&(global_config.agc_mode),receivebuf+37,1);
			// U34 choose port 0
//			I2cMux_Eeprom();
	//		Status = saveconfig(&global_config);

			if(Status!=XST_SUCCESS)
			{
				sendmsg = 1;
			}
			else
			{
				sendmsg = 0;
			}

			memcpy(send_buf,receivebuf,7);
			memcpy(send_buf+7,&msg_send,1);
			sendlen = 7 + 1 + 1;
			send_buf[sendlen-1] = checksum(send_buf,sendlen-1); // �����checksum���ȫ��
			memcpy(send_buf,&sendlen,2);
		}
		if(cmd_index==12) // ��ѯdefault_config
		{
			memcpy(send_buf,receivebuf,7);
		//	memcpy(send_buf+7, &default_config, sizeof(config_Settings_t));
			//sendlen = 7 + sizeof(config_Settings_t) + 1;
			send_buf[sendlen-1] = checksum(send_buf,sendlen-1); // �����checksum���ȫ��
			memcpy(send_buf,&sendlen,2);
		}
    }

    pq = pbuf_alloc(PBUF_TRANSPORT,sendlen,PBUF_POOL);
	pbuf_take(pq,(char*)send_buf,sendlen);

    udp_connect(upcb, addr, port);
#ifdef PRINT_REMOTE
	  bsp_printf("local %s port %d connected with ",
	  			inet_ntoa(echo_netif->ip_addr),
				echo_server_port);
	  bsp_printf("%s port %d\r\n", inet_ntoa(*addr),
			  port);
#endif
	udp_send(upcb,pq);
	udp_disconnect(upcb);
	pbuf_free(pq);

}
/*
 * Create new pcb, bind pcb and port, set call back function
 */
int start_udp_cmd_application(void)
{
	struct udp_pcb *pcb;
	err_t err;

	/* Create a new UDP PCB structure  */
	pcb = udp_new();
	if (!pcb) {
		bsp_printf("Error creating PCB. Out of Memory\n\r");
		return -1;
	}

	/* Bind the upcb to the UDP_PORT port */
	/* Using IP_ANY_TYPE==IP_ADDR_ANY allow the upcb to be used by any local interface */
	err = udp_bind(pcb, IP_ADDR_ANY, UDP_CMD_SVR_PORT);
	if (err != ERR_OK) {
		bsp_printf("Unable to bind to port %d: err = %d\n\r", UDP_CMD_SVR_PORT, err);
		udp_remove(pcb);
		return -2;
	}

    /* Set a receive callback for the upcb */
	udp_recv(pcb, udp_cmd_recv_callback, NULL);
//	bsp_printf("UDP server started @ port %d\n\r", UDP_CMD_SVR_PORT);

	return 0;
}

void transfer_udp_cmd_data(void)
{
	return;
}


#endif // UDP_COMMAND_SRV

#endif // XPAR_XEMACPS_NUM_INSTANCES || XPAR_XAXIETHERNET_NUM_INSTANCES

/*
usage:

call udp_server_setup() and platform_enable_interrupts() before the main_loop
call tcp_transfer_data() in the main_loop
run
```
    if (TcpFastTmrFlag) {
        tcp_fasttmr();
        TcpFastTmrFlag = 0;
    }
    if (TcpSlowTmrFlag) {
        tcp_slowtmr();
        TcpSlowTmrFlag = 0;
    }
```
in the main_loop, if tcp_transfer_data() not content it
keep a single `server_netif` definition, if you have more than one lwip process.

*/
