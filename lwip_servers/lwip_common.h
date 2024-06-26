#ifndef __LWIP_COMMON_H__

//#include "xparameters.h"
#if defined (XPAR_XEMACPS_NUM_INSTANCES) || defined (XPAR_XAXIETHERNET_NUM_INSTANCES)
#if defined (UDP_UPDATE) || defined (TCP_UPDATE) || defined (TCP_COMMAND_SRV) || defined (UDP_COMMAND_SRV)
#define __LWIP_COMMON_H__
#include "lwipopts.h"
#include "lwip/priv/tcp_priv.h"
#include "lwip/init.h"
#include "lwip/inet.h"
#include "lwip/err.h"
#include "netif/xadapter.h"
#include "lwip/udp.h"
#include "lwip/tcp.h"

#if LWIP_IPV6==1
	#include "lwip/ip6_addr.h"
	#include "lwip/ip6.h"
#else
	#if LWIP_DHCP==1
		#include "lwip/dhcp.h"
		extern volatile int dhcp_timoutcntr;
		err_t dhcp_start(struct netif *netif);
	#endif // LWIP_DHCP
	#define DEFAULT_IP_ADDRESS  "192.168.1.10"
	#define DEFAULT_IP_MASK     "255.255.255.0"
	#define DEFAULT_GW_ADDRESS  "192.168.1.1"
#endif // LWIP_IPV6

#define SEND_MSG	(0U)		// 1 - enable, 0 - disable

extern volatile int TcpFastTmrFlag;
extern volatile int TcpSlowTmrFlag;

#if defined (XPAR_XEMACPS_NUM_INSTANCES)
#define PLATFORM_EMAC_BASEADDR XPAR_XEMACPS_0_BASEADDR
#endif // XPAR_XEMACPS_NUM_INSTANCES
#if defined (XPAR_XAXIETHERNET_NUM_INSTANCES)
#define PLATFORM_EMAC_BASEADDR XPAR_AXIETHERNET_0_BASEADDR
#endif // XPAR_XAXIETHERNET_NUM_INSTANCES

//#define MAX_FLASH_LEN   32*1024*1024


//#if defined (TCP_COMMAND_SRV) || defined (UDP_COMMAND_SRV)
//extern uint8_t receivebuf[1500] = {0};
//extern uint8_t send_buf[1500] = {0};
//extern int receivelen = 0;
//extern int sendlen = 0;
//extern u8 cerrent_ch;
//extern u8 reset_pl;
//#endif // if defined (TCP_COMMAND_SRV) || defined (UDP_COMMAND_SRV)

extern struct netif server_netif;

void send_msg(const char *msg);
void process_print(u8 percent);
int lwip_common_init(struct netif *netif);
void transfer_data(struct netif *netif);

#endif // #if defined (UDP_UPDATE) || defined (TCP_UPDATE) || defined (TCP_COMMAND_SRV) || defined (UDP_COMMAND_SRV)
#endif // XPAR_XEMACPS_NUM_INSTANCES || XPAR_XAXIETHERNET_NUM_INSTANCES
#endif // #ifndef __LWIP_COMMON_H__
