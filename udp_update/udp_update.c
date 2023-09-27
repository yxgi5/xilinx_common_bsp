#include "../bsp.h"

#if defined (XPAR_XEMACPS_NUM_INSTANCES) || defined (XPAR_XAXIETHERNET_NUM_INSTANCES)
#if defined (UDP_UPDATE)

static struct udp_pcb *client_pcb = NULL;
//ip_addr_t target_addr;
//unsigned char ip_export[4];
//unsigned char mac_export[6];

//char FlashRxBuffer[MAX_FLASH_LEN] ;
u8 rxbuffer[MAX_FLASH_LEN];
//unsigned int ReceivedCount = 0 ;
u32 total_bytes = 0;

//#if (XPAR_XQSPIPSU_NUM_INSTANCES == 1U)
//extern XQspiPsu QspiInstance;
//#endif // XPAR_XQSPIPSU_NUM_INSTANCES == 1U
//#if (XPAR_XQSPIPS_NUM_INSTANCES==1)
//extern XQspiPs QspiInstance;
//#endif // XPAR_XQSPIPS_NUM_INSTANCES==1

int start_update_flag = 0 ;

//int FrameLengthCurr = 0 ;

/* defined by each RAW mode application */
//int start_udp();
//int transfer_data(const char *pData, int len, const ip_addr_t *addr) ;
//int send_data(const char *frame, int data_len);

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
	xil_printf("\n\r-----LwIP UDP Remote Update------\n\r");
	xil_printf("UDP packets sent to port %d\n\r", UDP_SER_PORT);
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
	static struct pbuf *pbuf2sent;

	pbuf2sent = pbuf_alloc(PBUF_TRANSPORT, strlen(msg), PBUF_POOL);
    if (!pbuf2sent)
        xil_printf("Error allocating pbuf\r\n");

    memcpy(pbuf2sent->payload, msg, strlen(msg));

    if (udp_send(client_pcb, pbuf2sent) != ERR_OK)
        xil_printf("UDP send error\r\n");

    pbuf_free(pbuf2sent);
#endif
}

/*
 * Call back funtion for udp receiver
 *
 * @param arg is argument
 * @param pcb is udp pcb pointer
 * @param p_rx is pbuf pointer
 * @param addr is IP address
 * @param port is udp port
 *
 */
//void udp_recv_callback(void *arg, struct udp_pcb *pcb, struct pbuf *p_rx,
//		const ip_addr_t *ip, u16_t port) {
//
//	char *pData;
//	pcb->remote_ip = *ip;
//	pcb->remote_port = port;
//	client_pcb = pcb;
//
//	if (p_rx != NULL)
//	{
//		pData = (char *) p_rx->payload;
//
//		int udp_len = p_rx->len ;
//
//		if (!(memcmp("update", p_rx->payload + p_rx->len - 6, 6)))
//		{
//			memcpy(&FlashRxBuffer[ReceivedCount], pData, udp_len - 6);
//			ReceivedCount += udp_len - 6 ;
//			xil_printf("Received Size is %u Bytes\r\n", ReceivedCount) ;
//			xil_printf("Initialization done, programming the memory\r\n") ;
//			start_update_flag = 1 ;
//		}
//		else
//		{
//			memcpy(&FlashRxBuffer[ReceivedCount], pData, udp_len);
//			ReceivedCount += udp_len ;
//		}
//
//		pbuf_free(p_rx);
//	}
//}
void udp_recv_callback(void *arg, struct udp_pcb *upcb, struct pbuf *p,
		const ip_addr_t *ip, u16_t port)
{
    struct pbuf *q;
    q = p;
    upcb->remote_ip = *ip;
    upcb->remote_port = port;
    client_pcb = upcb;

    if (q->tot_len == 6 && !(memcmp("update", p->payload, 6))) {
        start_update_flag = 1;
        sent_msg("Start QSPI Update\r\n");
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

    pbuf_free(p);
}
/*
 * Create new pcb, bind pcb and port, set call back function
 */
int start_udp(void)
{
	struct udp_pcb *pcb;
	err_t err;

    err = qspi_init();
    if (err != XST_SUCCESS) {
        bsp_printf("QSPI init Failed\r\n");
    }
    bsp_printf("Successfully init QSPI\r\n");

	pcb = udp_new();
	if (!pcb) {
		bsp_printf("Error creating PCB. Out of Memory\n\r");
		return -1;
	}
	/* bind to specified @port */
	err = udp_bind(pcb, IP_ADDR_ANY, UDP_SER_PORT);
	if (err != ERR_OK) {
		bsp_printf("Unable to bind to port %d: err = %d\n\r", UDP_SER_PORT, err);
		udp_remove(pcb);
		return -2;
	}
	udp_recv(pcb, udp_recv_callback, NULL);
//	IP4_ADDR(&target_addr, 192,168,1,35);
	bsp_printf("UDP server started @ port %d\n\r", UDP_SER_PORT);

	return 0;
}

int udp_server_setup(void)
{

	 int Status;
	 struct netif *netif;

//#if LWIP_IPV6==0
//	 ip_addr_t ipaddr, netmask, gw;
//#endif

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

//#if LWIP_IPV6==0
//#if LWIP_DHCP==1
//	ipaddr.addr = 0;
//	gw.addr = 0;
//	netmask.addr = 0;
//#else
//	/* initliaze IP addresses to be used */
//	IP4_ADDR(&ipaddr,  192, 168,   1, 10);
//	IP4_ADDR(&netmask, 255, 255, 255,  0);
//	IP4_ADDR(&gw,      192, 168,   1,  1);
//#endif
//#endif

	print_app_header();

	lwip_init();

//#if (LWIP_IPV6 == 0)
//	/* Add network interface to the netif_list, and set it as default */
//	if (!xemac_add(netif, &ipaddr, &netmask,
//			&gw, mac_ethernet_address,
//			PLATFORM_EMAC_BASEADDR)) {
//		xil_printf("Error adding N/W interface\n\r");
//		return -1;
//	}
//#else
//	/* Add network interface to the netif_list, and set it as default */
//	if (!xemac_add(netif, NULL, NULL, NULL, mac_ethernet_address,
//			PLATFORM_EMAC_BASEADDR)) {
//		xil_printf("Error adding N/W interface\n\r");
//		return -1;
//	}
//	netif->ip6_autoconfig_enabled = 1;
//
//	netif_create_ip6_linklocal_address(netif, 1);
//	netif_ip6_addr_set_state(netif, 0, IP6_ADDR_VALID);
//
//	print_ip6("\n\rBoard IPv6 address ", &netif->ip6_addr[0].u_addr.ip6);
//
//#endif

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
	platform_enable_interrupts();

	/* specify that the network if is up */
	netif_set_up(netif);

#if (LWIP_IPV6 == 0)
#if (LWIP_DHCP==1)
	/* Create a new DHCP client for this interface.
	 * Note: you must call dhcp_fine_tmr() and dhcp_coarse_tmr() at
	 * the predefined regular intervals after starting the client.
	 */
	dhcp_start(netif);
	dhcp_timoutcntr = 24;

	while(((netif->ip_addr.addr) == 0) && (dhcp_timoutcntr > 0))
		xemacif_input(netif);

	if (dhcp_timoutcntr <= 0) {
		if ((netif->ip_addr.addr) == 0) {
			xil_printf("ERROR: DHCP request timed out\r\n");
			xil_printf("Configuring default IP of 192.168.1.10\r\n");
//			IP4_ADDR(&(netif->ip_addr),  192, 168,   1, 10);
//			IP4_ADDR(&(netif->netmask), 255, 255, 255,  0);
//			IP4_ADDR(&(netif->gw),      192, 168,   1,  1);
			assign_default_ip(&(netif->ip_addr), &(netif->netmask), &(netif->gw));
		}
	}

//	ipaddr.addr = netif->ip_addr.addr;
//	gw.addr = netif->gw.addr;
//	netmask.addr = netif->netmask.addr;
#else
	assign_default_ip(&(netif->ip_addr), &(netif->netmask), &(netif->gw));
#endif // LWIP_DHCP
//	print_ip_settings(&ipaddr, &netmask, &gw);
	print_ip_settings(&(netif->ip_addr), &(netif->netmask), &(netif->gw));

//	memcpy(ip_export, &ipaddr, 4);
//	memcpy(mac_export, &mac_ethernet_address, 6);
#endif // LWIP_IPV6

	/* start the application (web server, rxtest, txtest, etc..) */
	start_udp();
}


void udp_transfer_data(void)
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


#endif // UDP_UPDATE

#endif // XPAR_XEMACPS_NUM_INSTANCES || XPAR_XAXIETHERNET_NUM_INSTANCES
