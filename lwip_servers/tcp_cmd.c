#include "../bsp.h"

#if defined (XPAR_XEMACPS_NUM_INSTANCES) || defined (XPAR_XAXIETHERNET_NUM_INSTANCES)
#if defined (TCP_COMMAND_SRV)

static struct tcp_pcb *client_pcb = NULL;

void print_tcp_cmd_header(void)
{
    bsp_printf("%20s %6d\r\n", "TCP CMD", TCP_CMD_SVR_PORT);
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


/** Close a tcp session */
static void tcp_server_close(struct tcp_pcb *pcb)
{
	err_t err;

	if (pcb != NULL) {
		tcp_recv(pcb, NULL);
		tcp_err(pcb, NULL);
		err = tcp_close(pcb);
		if (err != ERR_OK) {
			/* Free memory with abort */
			tcp_abort(pcb);
		}
	}
}

uint8_t receivebuf1[1500] = {0};
uint8_t send_buf1[1500] = {0};
int receivelen1 = 0;
int sendlen1 = 0;
u8 cerrent_ch;
u8 reset_pl;

static err_t tcp_cmd_recv_callback(void *arg, struct tcp_pcb *tpcb, struct pbuf *p, err_t err)
{
#if 0
	/* do not read the packet if we are not in ESTABLISHED state */
	if (!p) {
		tcp_close(tpcb);
		tcp_recv(tpcb, NULL);
		return ERR_OK;
	}

	/* indicate that the packet has been received */
	tcp_recved(tpcb, p->len);

	/* echo back the payload */
	/* in this case, we assume that the payload is < TCP_SND_BUF */
	if (tcp_sndbuf(tpcb) > p->len) {
		err = tcp_write(tpcb, p->payload, p->len, 1);
	} else
		bsp_printf("no space in tcp_sndbuf\n\r");

	/* free the received pbuf */
	pbuf_free(p);

	return ERR_OK;
#else

	/* do not read the packet if we are not in ESTABLISHED state */
	if (!p) {
		tcp_server_close(tpcb);
		bsp_printf("tcp connection closed\r\n");
		return ERR_OK;
	}


	receivelen1 = p->len;
	memset(receivebuf1, 0, sizeof(receivebuf1));
	memset(send_buf1, 0, sizeof(send_buf1));
	memcpy(receivebuf1,p->payload,p->len);

	/* indicate that the packet has been received */
	tcp_recved(tpcb, p->len);
//	tcp_recved(tpcb, p->tot_len);

	/* free the received pbuf */
	pbuf_free(p);

    int32_t Status=0;
    uint16_t msg_len=0;
    uint16_t msg_cmd=0;
    uint32_t msg_addr=0;
    uint32_t msg_value=0;
    uint8_t msg_ack=0;
    uint16_t mem_len=0;

    memcpy(&msg_len,receivebuf1,2);
    memcpy(&msg_cmd,receivebuf1+4,1);

    if(msg_cmd==0x10)	// read mem addr
    {
    	memcpy(&msg_addr,receivebuf1+5,4);
    	msg_value = Xil_In32(msg_addr); // don't use signed int, otherwise hardfail
    	//mdata=htonl(mdata);
    	memcpy(send_buf1,receivebuf1,receivelen1);
    	sendlen1=receivelen1;
    	//memcpy(send_buf,&sendlen,2);
    	memcpy(send_buf1+9,&msg_value,4);
    	send_buf1[sendlen1-1] = checksum(send_buf1,sendlen1-1); // �����checksum���ȫ��
    	memcpy(send_buf1,&sendlen1,2);
    }
    if(msg_cmd==0x11)	// write mem addr
	{
		memcpy(&msg_addr,receivebuf1+5,4);
		memcpy(&msg_value,receivebuf1+9,4);
		//msg_value=htonl(msg_value);
		//*(volatile unsigned int *)(msg_addr) = msg_value;
		Xil_Out32(msg_addr, msg_value);
		msg_value = Xil_In32(msg_addr); // don't use signed int, otherwise hardfail

		memcpy(send_buf1,receivebuf1,receivelen1);
		sendlen1=receivelen1;
		//memcpy(send_buf,&sendlen,2);
		memcpy(send_buf1+9,&msg_value,4);
		send_buf1[sendlen1-1] = checksum(send_buf1,sendlen1-1); // �����checksum���ȫ��
		memcpy(send_buf1,&sendlen1,2);
	}
    if(msg_cmd==0x12)	// read mem bulk
	{
		memcpy(&msg_addr,receivebuf1+5,4);
		memcpy(&mem_len,receivebuf1+9,4);
		memcpy(send_buf1,receivebuf1,receivelen1);
		memcpy(send_buf1+receivelen1-1,(void*)msg_addr,mem_len*4);
		sendlen1=receivelen1+mem_len*4;
		memcpy(send_buf1,&sendlen1,2);
		send_buf1[sendlen1-1] = checksum(send_buf1,sendlen1-1); // �����checksum���ȫ��
	}
    if(msg_cmd==0x13)	// write mem bulk
	{
		memcpy(&msg_addr,receivebuf1+5,4);
		memcpy(&mem_len,receivebuf1+9,4);
		memcpy((void*)msg_addr,receivebuf1+13,mem_len*4);
		sendlen1=receivelen1-mem_len*4;
		memcpy(send_buf1,receivebuf1,sendlen1);
//		memcpy(send_buf,&sendlen,2);
		send_buf1[sendlen1-1] = checksum(send_buf1,sendlen1-1); // �����checksum���ȫ��
		memcpy(send_buf1,&sendlen1,2);
	}
    if(msg_cmd == 0x20)
	{
		uint8_t addr;
		uint8_t ch;
		uint8_t len;
		uint8_t flag;
		uint16_t reg_addr;
		uint8_t data[256];
		ch =receivebuf1[5];
		addr = receivebuf1[6];
		len = receivebuf1[7];
		reg_addr = (uint16_t)receivebuf1[8] << 8 | receivebuf1[9];
		flag = receivebuf1[10];
//		if(flag == 1)
//		{
//			Status = xgpio_i2c_reg8_Continuous_read(ch,addr>>1,reg_addr,data,len,STRETCH_OFF);
//		}
//		else
//		{
//			Status = xgpio_i2c_reg16_Continuous_read(ch,addr>>1,reg_addr,data,len,STRETCH_OFF);
//		}

		memcpy(send_buf1,receivebuf1,5);
		memcpy(send_buf1+5,&Status,1);
		memcpy(send_buf1+6,data,len);
		sendlen1 = 6 + len+1;
		send_buf1[sendlen1-1] = checksum(send_buf1,sendlen1-1); //
		memcpy(send_buf1,&sendlen1,2);
	}
   if(msg_cmd == 0x21)
	{
		printf("recv ch\r\n");
		uint8_t addr;
		uint8_t ch;
		uint8_t value;
		uint8_t flag;
		uint16_t reg_addr;

		addr = receivebuf1[6];
		ch =receivebuf1[5];
		reg_addr = (uint16_t)receivebuf1[7] << 8 | receivebuf1[8];
		flag = receivebuf1[9];
		value = receivebuf1[11];
		if(flag == 1)
		{
//			Status = xgpio_i2c_reg8_write(ch,addr>>1,reg_addr,value,STRETCH_OFF);
		}
		else
		{
//			Status = xgpio_i2c_reg16_write(ch,addr>>1,reg_addr,value,STRETCH_OFF);
		}

		memcpy(send_buf1,receivebuf1,5);
		memcpy(send_buf1+5,&Status,1);
		memcpy(send_buf1+6,receivebuf1+5,msg_len-5);
		sendlen1 = msg_len + 1;
		send_buf1[sendlen1-1] = checksum(send_buf1,sendlen1-1); //
		memcpy(send_buf1,&sendlen1,2);
	}

   if(msg_cmd==0x40) // variables get and set
   {
   	uint16_t cmd_index;
   	uint32_t msg_send;
		memcpy(&cmd_index,receivebuf1+5,2);

		if(cmd_index==0)
		{
//			uint16_t major = __SW_VER_MAJOR__;
//			uint16_t minor = __SW_VER_MINOR__;
			uint32_t ver = __HW_VER__;
//			msg_send = (uint32_t)(((uint32_t)major)<<16) + (uint32_t)minor;
			msg_send = ver;

			memcpy(send_buf1,receivebuf1,7);
			memcpy(send_buf1+7,&msg_send,4);
			sendlen1 = 12;
			send_buf1[sendlen1-1] = checksum(send_buf1,sendlen1-1); // �����checksum���ȫ��
			memcpy(send_buf1,&sendlen1,2);
		}
		if(cmd_index==1)
		{
//			uint16_t major = __SW_VER_MAJOR__;
//			uint16_t minor = __SW_VER_MINOR__;
			uint32_t ver = __SW_VER__;
//			msg_send = (uint32_t)(((uint32_t)major)<<16) + (uint32_t)minor;
			msg_send = ver;

			memcpy(send_buf1,receivebuf1,7);
			memcpy(send_buf1+7,&msg_send,4);
			sendlen1 = 12;
			send_buf1[sendlen1-1] = checksum(send_buf1,sendlen1-1); // �����checksum���ȫ��
			memcpy(send_buf1,&sendlen1,2);
		}
		if(cmd_index==2)
		{
//			uint16_t major = __SW_VER_MAJOR__;
//			uint16_t minor = __SW_VER_MINOR__;
			uint32_t ver = AXI_LITE_REG_mReadReg(XPAR_PROCESSOR_SUBSYSTEM_AXI_LITE_REG_0_S00_AXI_BASEADDR, AXI_LITE_REG_S00_AXI_SLV_REG0_OFFSET);
//			msg_send = (uint32_t)(((uint32_t)major)<<16) + (uint32_t)minor;
			msg_send = ver;

			memcpy(send_buf1,receivebuf1,7);
			memcpy(send_buf1+7,&msg_send,4);
			sendlen1 = 12;
			send_buf1[sendlen1-1] = checksum(send_buf1,sendlen1-1); // �����checksum���ȫ��
			memcpy(send_buf1,&sendlen1,2);
		}
		if(cmd_index==3)
		{
//			uint16_t major = __SW_VER_MAJOR__;
//			uint16_t minor = __SW_VER_MINOR__;
			uint32_t ver = __SW_VER__;
//			msg_send = (uint32_t)(((uint32_t)major)<<16) + (uint32_t)minor;
			msg_send = ver;

			memcpy(send_buf1,receivebuf1,7);
			memcpy(send_buf1+7,&msg_send,4);
			sendlen1 = 12;
			send_buf1[sendlen1-1] = checksum(send_buf1,sendlen1-1); // �����checksum���ȫ��
			memcpy(send_buf1,&sendlen1,2);
		}
		if(cmd_index==4)
		{
//			uint16_t major = __SW_VER_MAJOR__;
//			uint16_t minor = __SW_VER_MINOR__;
			uint32_t ver = __SW_VER__;
//			msg_send = (uint32_t)(((uint32_t)major)<<16) + (uint32_t)minor;
			msg_send = ver;

			memcpy(send_buf1,receivebuf1,7);
			memcpy(send_buf1+7,&msg_send,4);
			sendlen1 = 12;
			send_buf1[sendlen1-1] = checksum(send_buf1,sendlen1-1); // �����checksum���ȫ��
			memcpy(send_buf1,&sendlen1,2);
		}
		if(cmd_index==5)// cur_ch_get
		{
			memcpy(send_buf1,receivebuf1,7);
			memcpy(send_buf1+7,&cerrent_ch,1);
			sendlen1 = 9;
			send_buf1[sendlen1-1] = checksum(send_buf1,sendlen1-1); // �����checksum���ȫ��
			memcpy(send_buf1,&sendlen1,2);
		}
		if(cmd_index==6) //cur_ch_set
		{
			uint8_t var1;
			memcpy(&var1,receivebuf1+7,1);
			cerrent_ch = var1;
			memcpy(send_buf1,receivebuf1,7);
			memcpy(send_buf1+7,&msg_send,1);
			sendlen1 = 7 + 1 + 1;
			send_buf1[sendlen1-1] = checksum(send_buf1,sendlen1-1); // �����checksum���ȫ��
			memcpy(send_buf1,&sendlen1,2);
		}
		if(cmd_index == 7)
		{
			uint8_t var1;
			memcpy(&var1,receivebuf1+7,1);
		//	reset_pl = var1;
			memcpy(send_buf1,receivebuf1,7);
			memcpy(send_buf1+7,&msg_send,1);
			sendlen1 = 7 + 1 + 1;
			send_buf1[sendlen1-1] = checksum(send_buf1,sendlen1-1); // �����checksum���ȫ��
			memcpy(send_buf1,&sendlen1,2);
		}
		if(cmd_index==10) // ��ѯglobal_config
		{
			memcpy(send_buf1,receivebuf1,7);
		//	memcpy(send_buf1+7, &global_config, sizeof(config_Settings_t));
		//	sendlen1 = 7 + sizeof(config_Settings_t) + 1;
			send_buf1[sendlen1-1] = checksum(send_buf1,sendlen1-1); // �����checksum���ȫ��
			memcpy(send_buf1,&sendlen1,2);
		}
		if(cmd_index==11) // д��global_config
		{
			int32_t Status;
			uint8_t sendmsg;
		//	memcpy(&global_config,receivebuf1+7,sizeof(config_Settings_t));
//			memcpy(&(global_config.frequency_Hz),receivebuf1+29,sizeof(int64_t));
//			memcpy(&(global_config.agc_mode),receivebuf1+37,1);
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

			memcpy(send_buf1,receivebuf1,7);
			memcpy(send_buf1+7,&msg_send,1);
			sendlen1 = 7 + 1 + 1;
			send_buf1[sendlen1-1] = checksum(send_buf1,sendlen1-1); // �����checksum���ȫ��
			memcpy(send_buf1,&sendlen1,2);
		}
		if(cmd_index==12) // ��ѯdefault_config
		{
			memcpy(send_buf1,receivebuf1,7);
		//	memcpy(send_buf1+7, &default_config, sizeof(config_Settings_t));
			//sendlen1 = 7 + sizeof(config_Settings_t) + 1;
			send_buf1[sendlen1-1] = checksum(send_buf1,sendlen1-1); // �����checksum���ȫ��
			memcpy(send_buf1,&sendlen1,2);
		}
    }

	/* echo back the payload */
	/* in this case, we assume that the payload is < TCP_SND_BUF */
	if (tcp_sndbuf(tpcb) > p->len) {
		err = tcp_write(tpcb, send_buf1, sendlen1, TCP_WRITE_FLAG_COPY);
//		err = tcp_write(tpcb, msg, strlen(msg), TCP_WRITE_FLAG_COPY);
	} else
		bsp_printf("no space in tcp_sndbuf\n\r");

	return ERR_OK;
#endif
}


/** Error callback, tcp session aborted */
static void tcp_server_err(void *arg, err_t err)
{
	LWIP_UNUSED_ARG(err);
//	u64_t now = get_time_ms();
//	u64_t diff_ms = now - server.start_time;
	tcp_server_close(client_pcb);
	client_pcb = NULL;
//	tcp_conn_report(diff_ms, TCP_ABORTED_REMOTE);
	bsp_printf("tcp_cmd_ser: Connection aborted\n\r");
}

static err_t accept_callback(void *arg, struct tcp_pcb *newpcb, err_t err)
{
    bsp_printf("tcp_cmd_ser: Connection Accepted\r\n");
    client_pcb = newpcb;

    tcp_recv(client_pcb, tcp_cmd_recv_callback);
    tcp_arg(client_pcb, NULL);
    tcp_err(client_pcb, tcp_server_err);

    return ERR_OK;
}

/*
 * Create new pcb, bind pcb and port, set call back function
 */
int start_tcp_cmd_application(void)
{
	struct tcp_pcb *pcb;
	err_t err;

	/* Create a new TCP PCB structure  */
    pcb = tcp_new();
    if (!pcb) {
        bsp_printf("Error creating PCB. Out of Memory\n\r");
        return -1;
    }

	/* Bind the pcb to the TCP_PORT port */
	/* Using IP_ANY_TYPE==IP_ADDR_ANY allow the pcb to be used by any local interface */
    err = tcp_bind(pcb, IP_ANY_TYPE, TCP_CMD_SVR_PORT);
    if (err != ERR_OK) {
        bsp_printf("Unable to bind to port %d: err = %d\n\r", TCP_CMD_SVR_PORT, err);
        tcp_close(pcb);
        return -2;
    }

    tcp_arg(pcb, NULL);

    pcb = tcp_listen(pcb);
    if (!pcb) {
        bsp_printf("Out of memory while tcp_listen\n\r");
        return -3;
    }

    tcp_accept(pcb, accept_callback);
//    bsp_printf("TCP server started @ port %d\n\r", TCP_CMD_SVR_PORT);

    return 0;
}

void transfer_tcp_cmd_data(void)
{
	return;
}


#endif // TCP_COMMAND_SRV

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
