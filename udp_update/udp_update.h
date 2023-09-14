#ifndef __UDP_UPDATE_H__

//#include "xparameters.h"
#if defined (XPAR_XEMACPS_NUM_INSTANCES) && defined (UDP_UPDATE)
#define __UDP_UPDATE_H__

#include "netif/xadapter.h"
#include "lwip/udp.h"

#if LWIP_IPV6==1
#include "lwip/ip.h"
#else
#if LWIP_DHCP==1
#include "lwip/dhcp.h"
#endif
#endif

#define PLATFORM_EMAC_BASEADDR XPAR_XEMACPS_0_BASEADDR
#define MAX_FLASH_LEN   32*1024*1024

extern struct netif *echo_netif;

void Start_UDP_Updata(void);
void Udp_Setting(void);

#endif // XPAR_XEMACPS_NUM_INSTANCES && UDP_UPDATE

#endif // __UDP_UPDATE_H__
