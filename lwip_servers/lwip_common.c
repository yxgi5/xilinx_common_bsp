#include "../bsp.h"

#if defined (XPAR_XEMACPS_NUM_INSTANCES) || defined (XPAR_XAXIETHERNET_NUM_INSTANCES)
#if defined (UDP_UPDATE) || defined (TCP_UPDATE) || defined (TCP_COMMAND_SRV) || defined (UDP_COMMAND_SRV)

//#if defined (UDP_UPDATE) || defined (TCP_UPDATE)
//u8 rxbuffer[MAX_FLASH_LEN];
//u32 total_bytes = 0;
//int start_update_flag = 0 ;
//#endif // #if defined (UDP_UPDATE) || defined (TCP_UPDATE)

//#if defined (TCP_COMMAND_SRV) || defined (UDP_COMMAND_SRV)
//uint8_t receivebuf[1500] = {0};
//uint8_t send_buf[1500] = {0};
//int receivelen = 0;
//int sendlen = 0;
//u8 cerrent_ch;
//u8 reset_pl;
//#endif // if defined (TCP_COMMAND_SRV) || defined (UDP_COMMAND_SRV)


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
//	bsp_printf(" %s\n\r", inet6_ntoa(*ip));
	bsp_printf(" %x:%x:%x:%x:%x:%x:%x:%x\n\r",
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
	bsp_printf("%d.%d.%d.%d\n\r", ip4_addr1(ip), ip4_addr2(ip),
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

	bsp_printf("Configuring default IP %s \r\n", DEFAULT_IP_ADDRESS);

	err = inet_aton(DEFAULT_IP_ADDRESS, ip);
	if (!err)
		bsp_printf("Invalid default IP address: %d\r\n", err);

	err = inet_aton(DEFAULT_IP_MASK, mask);
	if (!err)
		bsp_printf("Invalid default IP MASK: %d\r\n", err);

	err = inet_aton(DEFAULT_GW_ADDRESS, gw);
	if (!err)
		bsp_printf("Invalid default gateway address: %d\r\n", err);
}
#endif // LWIP_IPV6


void print_headers()
{
    bsp_printf("\r\n");
    bsp_printf("%20s %6s\r\n", "Server", "Port");
    bsp_printf("%20s %6s\r\n", "--------------------", "------");
#if defined (UDP_UPDATE)
	print_udp_update_header();
#endif // #if defined (UDP_UPDATE)
#if defined (TCP_UPDATE)
	print_tcp_update_header();
#endif // #if defined (TCP_UPDATE)
#if defined (TCP_COMMAND_SRV)
	print_tcp_cmd_header();
#endif // #if defined (TCP_COMMAND_SRV)
#if defined (UDP_COMMAND_SRV)
	print_udp_cmd_header();
#endif // #if defined (UDP_COMMAND_SRV)
    bsp_printf("\r\n");
}

void start_applications(void)
{
#if defined (UDP_UPDATE) || defined (TCP_UPDATE)
	int Status;
	Status = qspi_init();
    if (Status != XST_SUCCESS) {
        bsp_printf(TXT_RED "In %s: QSPI init failed...\r\n" TXT_RST, __func__);
        // return XST_FAILURE;
    }
//    bsp_printf("Successfully init QSPI\r\n");

#endif // #if defined (UDP_UPDATE) || defined (TCP_UPDATE)

#if defined (UDP_UPDATE)
	start_udp_update_application();
#endif // #if defined (UDP_UPDATE)
#if defined (TCP_UPDATE)
	start_tcp_update_application();
#endif // #if defined (TCP_UPDATE)
#if defined (TCP_COMMAND_SRV)
	start_tcp_cmd_application();
#endif // #if defined (TCP_COMMAND_SRV)
#if defined (UDP_COMMAND_SRV)
	start_udp_cmd_application();
#endif // #if defined (UDP_COMMAND_SRV)

}

void transfer_data(struct netif *netif)
{
    if (TcpFastTmrFlag) {
        tcp_fasttmr();
        TcpFastTmrFlag = 0;
    }
    if (TcpSlowTmrFlag) {
        tcp_slowtmr();
        TcpSlowTmrFlag = 0;
    }
	xemacif_input(netif);

#if defined (UDP_UPDATE)
	transfer_udp_update_data();
#endif // #if defined (UDP_UPDATE)
#if defined (TCP_UPDATE)
	transfer_tcp_update_data();
#endif // #if defined (TCP_UPDATE)
#if defined (TCP_COMMAND_SRV)
	transfer_tcp_cmd_data();
#endif // #if defined (TCP_COMMAND_SRV)
#if defined (UDP_COMMAND_SRV)
	transfer_udp_cmd_data();
#endif // #if defined (UDP_COMMAND_SRV)
}

int lwip_common_init(struct netif *netif)
{
//	 int Status;

#if LWIP_IPV6==0
	 ip_addr_t ipaddr, netmask, gw;
#endif

	/* the mac address of the board. this should be unique per board */
	unsigned char mac_ethernet_address[] = { 0x10, 0x0a, 0x35, 0x00, 0x01, 0x02 };

	netif = &server_netif;

//#if defined (__arm__) && !defined (ARMR5)
//#if XPAR_GIGE_PCS_PMA_SGMII_CORE_PRESENT == 1 || XPAR_GIGE_PCS_PMA_1000BASEX_CORE_PRESENT == 1
//	ProgramSi5324();
//	ProgramSfpPhy();
//#endif
//#endif
//
//		/* Define this board specific macro in order perform PHY reset on ZCU102 */
//#ifdef XPS_BOARD_ZCU102
//	if(IicPhyReset()) {
//		bsp_printf("Error performing PHY reset \n\r");
//		return -1;
//	}
//#endif


#if LWIP_IPV6==0
#if LWIP_DHCP==1
	ipaddr.addr = 0;
	gw.addr = 0;
	netmask.addr = 0;
#else
	/* initliaze IP addresses to be used */
	IP4_ADDR(&ipaddr,  192, 168,   1, 10);
	IP4_ADDR(&netmask, 255, 255, 255,  0);
	IP4_ADDR(&gw,      192, 168,   1,  1);
#endif
#endif

	lwip_init();

#if (LWIP_IPV6 == 0)
	/* Add network interface to the netif_list, and set it as default */
	if (!xemac_add(netif, &ipaddr, &netmask,
			&gw, mac_ethernet_address,
			PLATFORM_EMAC_BASEADDR)) {
		bsp_printf("Error adding N/W interface\n\r");
		return XST_FAILURE;
	}
#else
	/* Add network interface to the netif_list, and set it as default */
	if (!xemac_add(netif, NULL, NULL, NULL, mac_ethernet_address,
			PLATFORM_EMAC_BASEADDR)) {
		bsp_printf("Error adding N/W interface\n\r");
		return XST_FAILURE;
	}
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

#if (LWIP_IPV6 == 0)
#if (LWIP_DHCP==1)
	/* Create a new DHCP client for this interface.
	 * Note: you must call dhcp_fine_tmr() and dhcp_coarse_tmr() at
	 * the predefined regular intervals after starting the client.
	 */
	dhcp_start(netif);
	dhcp_timoutcntr = 24;

	while(((netif->ip_addr.addr) == 0) && (dhcp_timoutcntr > 0)) {
		xemacif_input(netif);
		if (TcpFastTmrFlag) {
			tcp_fasttmr();
			TcpFastTmrFlag = 0;
		}
		if (TcpSlowTmrFlag) {
			tcp_slowtmr();
			TcpSlowTmrFlag = 0;
		}
	}
	if (dhcp_timoutcntr <= 0) {
		if ((netif->ip_addr.addr) == 0) {
			bsp_printf("DHCP request timed out\r\n");
			bsp_printf("Configuring default IP of 192.168.1.10\r\n");
			IP4_ADDR(&(netif->ip_addr),  192, 168,   1, 10);
			IP4_ADDR(&(netif->netmask), 255, 255, 255,  0);
			IP4_ADDR(&(netif->gw),      192, 168,   1,  1);
		}
	}

	ipaddr.addr = netif->ip_addr.addr;
	gw.addr = netif->gw.addr;
	netmask.addr = netif->netmask.addr;
#endif // #if (LWIP_DHCP==1)
	print_ip_settings(&ipaddr, &netmask, &gw);
#endif // #if (LWIP_IPV6 == 0)

	/* start the application (web server, rxtest, txtest, etc..) */
	start_applications();
	print_headers();

	return XST_SUCCESS;
}

void send_msg(const char *msg)
{
#if defined (UDP_UPDATE)
	udp_update_svr_send_msg(msg);
#endif // #if defined (UDP_UPDATE)
#if defined (TCP_UPDATE)
	tcp_update_svr_send_msg(msg);
#endif // #if defined (UDP_UPDATE)
}

void process_print(u8 percent)
{
#if defined (UDP_UPDATE)
	udp_update_process_print(percent);
#endif // #if defined (UDP_UPDATE)
#if defined (TCP_UPDATE)
	tcp_update_process_print(percent);
#endif // #if defined (UDP_UPDATE)
}

#endif // #if defined (UDP_UPDATE) || defined (TCP_UPDATE) || defined (TCP_COMMAND_SRV) || defined (UDP_COMMAND_SRV)
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
