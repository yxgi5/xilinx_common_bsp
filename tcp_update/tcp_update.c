#include "../bsp.h"

#if defined (XPAR_XEMACPS_NUM_INSTANCES) || defined (XPAR_XAXIETHERNET_NUM_INSTANCES)
#if defined (TCP_UPDATE)

static struct tcp_pcb *client_pcb = NULL;

u8 rxbuffer[MAX_FLASH_LEN];
u32 total_bytes = 0;

//#if (XPAR_XQSPIPSU_NUM_INSTANCES == 1U)
//extern XQspiPsu QspiInstance;
//#endif // XPAR_XQSPIPSU_NUM_INSTANCES == 1U
//#if (XPAR_XQSPIPS_NUM_INSTANCES==1)
//extern XQspiPs QspiInstance;
//#endif // XPAR_XQSPIPS_NUM_INSTANCES==1

u8 start_update_flag = 0;

/* missing declaration in lwIP */
void lwip_init();

#if LWIP_IPV6==0
#if LWIP_DHCP==1
extern volatile int dhcp_timoutcntr;
err_t dhcp_start(struct netif *netif);
#endif
#endif

struct netif server_netif;


#if LWIP_IPV6==1
	void print_ip6(char *msg, ip_addr_t *ip)
	{
		print(msg);
	//	xil_printf(" %s\n\r", inet6_ntoa(*ip));
		xil_printf(" %x:%x:%x:%x:%x:%x:%x:%x\n\r",
				IP6_ADDR_BLOCK1(&ip->u_addr.ip6),
				IP6_ADDR_BLOCK2(&ip->u_addr.ip6),
				IP6_ADDR_BLOCK3(&ip->u_addr.ip6),
				IP6_ADDR_BLOCK4(&ip->u_addr.ip6),
				IP6_ADDR_BLOCK5(&ip->u_addr.ip6),
				IP6_ADDR_BLOCK6(&ip->u_addr.ip6),
				IP6_ADDR_BLOCK7(&ip->u_addr.ip6),
				IP6_ADDR_BLOCK8(&ip->u_addr.ip6));

	}
#else
	void print_ip(char *msg, ip_addr_t *ip)
	{
		print(msg);
		xil_printf("%d.%d.%d.%d\n\r", ip4_addr1(ip), ip4_addr2(ip),
				ip4_addr3(ip), ip4_addr4(ip));
	}

	void print_ip_settings(ip_addr_t *ip, ip_addr_t *mask, ip_addr_t *gw)
	{

		print_ip("Board IP: ", ip);
		print_ip("Netmask : ", mask);
		print_ip("Gateway : ", gw);
	}

	void assign_default_ip(ip_addr_t *ip, ip_addr_t *mask, ip_addr_t *gw)
	{
	    int err;

	    xil_printf("Configuring default IP %s \r\n", DEFAULT_IP_ADDRESS);

	    err = inet_aton(DEFAULT_IP_ADDRESS, ip);
	    if (!err)
	        xil_printf("Invalid default IP address: %d\r\n", err);

	    err = inet_aton(DEFAULT_IP_MASK, mask);
	    if (!err)
	        xil_printf("Invalid default IP MASK: %d\r\n", err);

	    err = inet_aton(DEFAULT_GW_ADDRESS, gw);
	    if (!err)
	        xil_printf("Invalid default gateway address: %d\r\n", err);
	}
#endif // LWIP_IPV6

void print_app_header(void)
{
	xil_printf("\n\r\n\r-----LwIP TCP Remote Update------\n\r");
	xil_printf("TCP packets sent to port %d\n\r", TCP_SER_PORT);
}

void process_print(u8 percent)
{
    switch (percent) {
    case 0:
        sent_msg("0%..");
        xil_printf("0%%..");
        break;
    case 1:
        sent_msg("10%..");
        xil_printf("10%%..");
        break;
    case 2:
        sent_msg("20%..");
        xil_printf("20%%..");
        break;
    case 3:
        sent_msg("30%..");
        xil_printf("30%%..");
        break;
    case 4:
        sent_msg("40%..");
        xil_printf("40%%..");
        break;
    case 5:
        sent_msg("50%..");
        xil_printf("50%%..");
        break;
    case 6:
        sent_msg("60%..");
        xil_printf("60%%..");
        break;
    case 7:
        sent_msg("70%..");
        xil_printf("70%%..");
        break;
    case 8:
        sent_msg("80%..");
        xil_printf("80%%..");
        break;
    case 9:
        sent_msg("90%..");
        xil_printf("90%%..");
        break;
    case 10:
        sent_msg("100%\r\n");
        xil_printf("100%%\r\n");
    default:
        break;
    }
}

void sent_msg(const char *msg)
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
            xil_printf("tcp_server: Error on tcp_write: %d\r\n", err);
        err = tcp_output(client_pcb);
        if (err != ERR_OK)
            xil_printf("tcp_server: Error on tcp_output: %d\r\n", err);
    } else
    {
        xil_printf("no space in tcp_sndbuf\r\n");
    }
#endif
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

static err_t recv_callback(void *arg, struct tcp_pcb *tpcb, struct pbuf *p, err_t err)
{
#if 1
    struct pbuf *q;

    if (!p) {
//        tcp_close(tpcb);
//        tcp_recv(tpcb, NULL);
    	tcp_server_close(tpcb);
        xil_printf("tcp connection closed\r\n");
        return ERR_OK;
    }
    q = p;

    if (q->tot_len == 6 && !(memcmp("update", p->payload, 6))) {
        start_update_flag = 1;
        sent_msg("\r\nStart QSPI Update\r\n");
    } else if (q->tot_len == 5 && !(memcmp("clear", p->payload, 5))) {
        start_update_flag = 0;
        total_bytes = 0;
        sent_msg("Clear received data\r\n");
        xil_printf("Clear received data\r\n");
    } else {
        while (q->tot_len != q->len) {
            memcpy(&rxbuffer[total_bytes], q->payload, q->len);
            total_bytes += q->len;
            q = q->next;
        }
        memcpy(&rxbuffer[total_bytes], q->payload, q->len);
        total_bytes += q->len;
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
		xil_printf("no space in tcp_sndbuf\n\r");

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
	xil_printf("TCP connection aborted\n\r");
}

err_t accept_callback(void *arg, struct tcp_pcb *newpcb, err_t err)
{
    xil_printf("tcp_server: Connection Accepted\r\n");
    client_pcb = newpcb;

    tcp_recv(client_pcb, recv_callback);
    tcp_arg(client_pcb, NULL);
    tcp_err(client_pcb, tcp_server_err);

    return ERR_OK;
}

int start_tcp(void)
{
    struct tcp_pcb *pcb;
    err_t err;

    err = qspi_init();
    if (err != XST_SUCCESS) {
        xil_printf("QSPI init Failed\r\n");
        return XST_FAILURE;
    }
    xil_printf("Successfully init QSPI\r\n");

//    pcb = tcp_new_ip_type(IPADDR_TYPE_ANY);
    pcb = tcp_new();
    if (!pcb) {
        xil_printf("Error creating PCB. Out of Memory\n\r");
        return -1;
    }

    err = tcp_bind(pcb, IP_ANY_TYPE, TCP_SER_PORT);
    if (err != ERR_OK) {
        xil_printf("Unable to bind to port %d: err = %d\n\r", TCP_SER_PORT, err);
        tcp_close(pcb);
        return -2;
    }

    tcp_arg(pcb, NULL);

    pcb = tcp_listen(pcb);
    if (!pcb) {
        xil_printf("Out of memory while tcp_listen\n\r");
        return -3;
    }

    tcp_accept(pcb, accept_callback);

    xil_printf("TCP server started @ port %d\n\r", TCP_SER_PORT);

    return 0;
}

int tcp_server_setup(void)
{
	int Status;
	struct netif *netif;

	/* the mac address of the board. this should be unique per board */
	unsigned char mac_ethernet_address[] = { 0x10, 0x0a, 0x35, 0x00, 0x01, 0x02 };

	netif = &server_netif;

#if defined (__arm__) && !defined (ARMR5)
#if XPAR_GIGE_PCS_PMA_SGMII_CORE_PRESENT == 1 || XPAR_GIGE_PCS_PMA_1000BASEX_CORE_PRESENT == 1
	ProgramSi5324();
	ProgramSfpPhy();
#endif
#endif

		/* Define this board specific macro in order perform PHY reset on ZCU102 */
#ifdef XPS_BOARD_ZCU102
	if(IicPhyReset()) {
		xil_printf("Error performing PHY reset \n\r");
		return -1;
	}
#endif

//	init_platform();

	print_app_header();

	lwip_init();

	/* Add network interface to the netif_list, and set it as default */
	if (!xemac_add(netif, NULL, NULL, NULL, mac_ethernet_address,
				PLATFORM_EMAC_BASEADDR)) {
		xil_printf("Error adding N/W interface\r\n");
		return -1;
	}
#if (LWIP_IPV6 == 1)
	netif->ip6_autoconfig_enabled = 1;

	netif_create_ip6_linklocal_address(netif, 1);
	netif_ip6_addr_set_state(netif, 0, IP6_ADDR_VALID);

	print_ip6("\n\rBoard IPv6 address ", &netif->ip6_addr[0].u_addr.ip6);
#endif

	netif_set_default(netif);

	/* now enable interrupts */
//	platform_enable_interrupts();

	/* specify that the network if is up */
	netif_set_up(netif);

#if (LWIP_IPV6==0)
#if (LWIP_DHCP==1)
	/* Create a new DHCP client for this interface.
	 * Note: you must call dhcp_fine_tmr() and dhcp_coarse_tmr() at
	 * the predefined regular intervals after starting the client.
	 */
    dhcp_start(netif);
    dhcp_timoutcntr = 24;

    while (((netif->ip_addr.addr) == 0) && (dhcp_timoutcntr > 0))
        xemacif_input(netif);

    if (dhcp_timoutcntr <= 0) {
        if ((netif->ip_addr.addr) == 0) {
            xil_printf("ERROR: DHCP request timed out\r\n");
            xil_printf("Configuring default IP of 192.168.1.10\r\n");
            assign_default_ip(&(netif->ip_addr), &(netif->netmask), &(netif->gw));
        }
    }

#else
    assign_default_ip(&(netif->ip_addr), &(netif->netmask), &(netif->gw));
#endif
    print_ip_settings(&(netif->ip_addr), &(netif->netmask), &(netif->gw));
#endif // LWIP_IPV6

	start_tcp();
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
//	xil_printf("TCP connection aborted\n\r");
//}

void tcp_transfer_data(void)
{
	int Status;
	char msg[60];

    if (TcpFastTmrFlag) {
        tcp_fasttmr();
        TcpFastTmrFlag = 0;
    }
    if (TcpSlowTmrFlag) {
        tcp_slowtmr();
        TcpSlowTmrFlag = 0;
    }
	xemacif_input(&server_netif);
	if (start_update_flag)
	{
//		Status = update_qspi(&QspiInstance, QSPIPSU_DEVICE_ID, ReceivedCount, FlashRxBuffer) ;
//		if (Status != XST_SUCCESS)
//			xil_printf("Update Flash Error!\r\n") ;
//		StartUpdate = 0 ;
//		ReceivedCount = 0;
        xil_printf("Start QSPI Update!\r\n");
        xil_printf("file size of BOOT.bin is %lu Bytes\r\n", total_bytes);
        sprintf(msg, "file size of BOOT.bin is %lu Bytes\r\n", total_bytes);
        sent_msg(msg);
        if (qspi_update(total_bytes, rxbuffer) != XST_SUCCESS)
        {
            sent_msg("Update Qspi Error!\r\n");
            xil_printf("Update Qspi Error!\r\n");
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
