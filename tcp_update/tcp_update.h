#ifndef __TCP_UPDATE_H__

//#include "xparameters.h"
#if defined (XPAR_XEMACPS_NUM_INSTANCES) || defined (XPAR_XAXIETHERNET_NUM_INSTANCES)

#if defined (TCP_UPDATE)
#define __TCP_UPDATE_H__
#include "lwipopts.h"
#include "lwip/priv/tcp_priv.h"
#include "lwip/init.h"
#include "lwip/inet.h"
#include "lwip/err.h"
#include "netif/xadapter.h"
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

#define MAX_FLASH_LEN   32*1024*1024

#define TCP_SER_PORT            (6789)

void tcp_transfer_data(void);
int tcp_server_setup(void);

#endif // TCP_UPDATE

#endif // XPAR_XEMACPS_NUM_INSTANCES && TCP_UPDATE

#endif // __TCP_UPDATE_H__
