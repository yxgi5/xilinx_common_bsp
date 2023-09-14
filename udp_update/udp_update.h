#ifndef __UDP_UPDATE_H__

//#include "xparameters.h"
#if defined (XPAR_XEMACPS_NUM_INSTANCES) && defined (UDP_UPDATE)
#define __UDP_UPDATE_H__
#include "lwipopts.h"
#include "lwip/priv/tcp_priv.h"
#include "lwip/init.h"
#include "lwip/inet.h" //
#include "lwip/err.h"
#include "netif/xadapter.h"
#include "lwip/udp.h" //

#if LWIP_IPV6==1
	#include "lwip/ip6_addr.h"
	#include "lwip/ip6.h"
#else
	#if LWIP_DHCP==1
		#include "lwip/dhcp.h"
		extern volatile int dhcp_timoutcntr;
		err_t dhcp_start(struct netif *netif);
	#endif // LWIP_DHCP
#endif // LWIP_IPV6

extern volatile int TcpFastTmrFlag;
extern volatile int TcpSlowTmrFlag;

#define PLATFORM_EMAC_BASEADDR XPAR_XEMACPS_0_BASEADDR
#define MAX_FLASH_LEN   32*1024*1024

extern struct netif *echo_netif;

void Start_UDP_Updata(void);
void Udp_Setting(void);

#endif // XPAR_XEMACPS_NUM_INSTANCES && UDP_UPDATE

#endif // __UDP_UPDATE_H__
