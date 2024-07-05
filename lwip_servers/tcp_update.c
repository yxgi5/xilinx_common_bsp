#include "../bsp.h"

#if defined (XPAR_XEMACPS_NUM_INSTANCES) || defined (XPAR_XAXIETHERNET_NUM_INSTANCES)
#if defined (TCP_UPDATE)

static struct tcp_pcb *client_pcb = NULL;

//static u8 rxbuffer[MAX_FLASH_LEN];
//static u32 total_bytes = 0;
//static int start_update_flag = 0 ;


void print_tcp_update_header(void)
{
    bsp_printf("%20s %6d\r\n", "TCP Update", TCP_UPDATE_SVR_PORT);
}


void tcp_update_svr_send_msg(const char *msg)
{
#if (SEND_MSG == 1U)
	err_t err;
    tcp_nagle_disable(client_pcb);
    u32 tmp = tcp_sndbuf(client_pcb);
//    if (tcp_sndbuf(client_pcb) > strlen(msg))
    if(tmp > strlen(msg))
    {
        err = tcp_write(client_pcb, msg, strlen(msg), TCP_WRITE_FLAG_COPY);
        if (err != ERR_OK)
            bsp_printf("tcp_server: Error on tcp_write: %d\r\n", err);
        err = tcp_output(client_pcb);
        if (err != ERR_OK)
            bsp_printf("tcp_server: Error on tcp_output: %d\r\n", err);
    } else
    {
        bsp_printf("no space in tcp_sndbuf\r\n");
    }
#endif
}

void tcp_update_process_print(u8 percent)
{
    switch (percent) {
    case 0:
    	tcp_update_svr_send_msg("0%..");
        bsp_printf("0%%..");
        break;
    case 1:
    	tcp_update_svr_send_msg("10%..");
        bsp_printf("10%%..");
        break;
    case 2:
    	tcp_update_svr_send_msg("20%..");
        bsp_printf("20%%..");
        break;
    case 3:
    	tcp_update_svr_send_msg("30%..");
        bsp_printf("30%%..");
        break;
    case 4:
    	tcp_update_svr_send_msg("40%..");
        bsp_printf("40%%..");
        break;
    case 5:
    	tcp_update_svr_send_msg("50%..");
        bsp_printf("50%%..");
        break;
    case 6:
    	tcp_update_svr_send_msg("60%..");
        bsp_printf("60%%..");
        break;
    case 7:
    	tcp_update_svr_send_msg("70%..");
        bsp_printf("70%%..");
        break;
    case 8:
    	tcp_update_svr_send_msg("80%..");
        bsp_printf("80%%..");
        break;
    case 9:
    	tcp_update_svr_send_msg("90%..");
        bsp_printf("90%%..");
        break;
    case 10:
    	tcp_update_svr_send_msg("100%\r\n");
        bsp_printf("100%%\r\n");
    default:
        break;
    }
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

static err_t tcp_update_recv_callback(void *arg, struct tcp_pcb *tpcb, struct pbuf *p, err_t err)
{
#if 1
    struct pbuf *q;

    if (!p) {
//        tcp_close(tpcb);
//        tcp_recv(tpcb, NULL);
    	tcp_server_close(tpcb);
        bsp_printf("tcp connection closed\r\n");
        return ERR_OK;
    }
    q = p;

    if (q->tot_len == 6 && !(memcmp("update", p->payload, 6))) {
        start_update_flag = 1;
        tcp_update_svr_send_msg("\r\nStart QSPI Update\r\n");
    } else if (q->tot_len == 5 && !(memcmp("clear", p->payload, 5))) {
        start_update_flag = 0;
        total_bytes = 0;
        tcp_update_svr_send_msg("Clear received data\r\n");
        bsp_printf("Clear received data\r\n");
    } else {
        while (q->tot_len != q->len) {
            memcpy(&rxbuffer[total_bytes], q->payload, q->len);
            total_bytes += q->len;
            q = q->next;
        }
        memcpy(&rxbuffer[total_bytes], q->payload, q->len);
        total_bytes += q->len;
        // TODO: checksum
//        if(total_bytes == 15043010)
//        {
			if(!(memcmp("update", &rxbuffer[total_bytes-6], 6)))
			{
//				NOP();
				total_bytes -= 6;
				start_update_flag = 1;
				tcp_update_svr_send_msg("\r\nStart QSPI Update\r\n");
			}
//			else
//			{
//				NOP();
//			}
//        }
    }

    tcp_recved(tpcb, p->tot_len);
    pbuf_free(p);

    return ERR_OK;
#else
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
	bsp_printf("tcp_update_ser: Connection aborted\n\r");
}

static err_t accept_callback(void *arg, struct tcp_pcb *newpcb, err_t err)
{
    bsp_printf("tcp_update_ser: Connection Accepted\r\n");
    client_pcb = newpcb;

    tcp_recv(client_pcb, tcp_update_recv_callback);
    tcp_arg(client_pcb, NULL);
    tcp_err(client_pcb, tcp_server_err);

    return ERR_OK;
}

int start_tcp_update_application(void)
{
    struct tcp_pcb *pcb;
    err_t err;

//    pcb = tcp_new_ip_type(IPADDR_TYPE_ANY);
    pcb = tcp_new();
    if (!pcb) {
        bsp_printf("Error creating PCB. Out of Memory\n\r");
        return -1;
    }

    err = tcp_bind(pcb, IP_ANY_TYPE, TCP_UPDATE_SVR_PORT);
    if (err != ERR_OK) {
        bsp_printf("Unable to bind to port %d: err = %d\n\r", TCP_UPDATE_SVR_PORT, err);
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
//    bsp_printf("TCP server started @ port %d\n\r", TCP_UPDATE_SVR_PORT);

    return 0;
}

///** Close a tcp session */
//void tcp_server_close(struct tcp_pcb *pcb)
//{
//	err_t err;
//
//	if (pcb != NULL) {
//		tcp_recv(pcb, NULL);
//		tcp_err(pcb, NULL);
//		err = tcp_close(pcb);
//		if (err != ERR_OK) {
//			/* Free memory with abort */
//			tcp_abort(pcb);
//		}
//	}
//}
///** Error callback, tcp session aborted */
//static void tcp_server_err(void *arg, err_t err)
//{
//	LWIP_UNUSED_ARG(err);
//	u64_t now = get_time_ms();
//	u64_t diff_ms = now - server.start_time;
//	tcp_server_close(client_pcb);
//	c_pcb = NULL;
//	tcp_conn_report(diff_ms, TCP_ABORTED_REMOTE);
//	bsp_printf("TCP connection aborted\n\r");
//}

void transfer_tcp_update_data(void)
{
	char msg[60];
	if (start_update_flag)
	{
//		Status = update_qspi(&QspiInstance, QSPIPSU_DEVICE_ID, ReceivedCount, FlashRxBuffer) ;
//		if (Status != XST_SUCCESS)
//			bsp_printf(TXT_RED "In %s: Update flash failed...\r\n" TXT_RST, __func__);
//		StartUpdate = 0 ;
//		ReceivedCount = 0;
        bsp_printf("Start QSPI Update!\r\n");
        bsp_printf("file size of BOOT.bin is %lu Bytes\r\n", total_bytes);
        sprintf(msg, "file size of BOOT.bin is %lu Bytes\r\n", total_bytes);
        tcp_update_svr_send_msg(msg);
        if (qspi_update(total_bytes, rxbuffer) != XST_SUCCESS)
        {
        	tcp_update_svr_send_msg("Update Qspi Error!\r\n");
            bsp_printf("Update Qspi Error!\r\n");
        }
        else
        {
            total_bytes = 0;
        }
	}
    start_update_flag = 0;
}

#endif // TCP_UPDATE

#endif // XPAR_XEMACPS_NUM_INSTANCES || XPAR_XAXIETHERNET_NUM_INSTANCES


/*
usage:
call tcp_server_setup() and platform_enable_interrupts() before the main_loop
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
