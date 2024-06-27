#include "bsp.h"


#if defined (__PPC__)

#if defined (INTC_DEVICE_ID) || defined (INTC)
INTC InterruptController;
#endif

#ifdef STDOUT_IS_16550
	#include "xuartns550_l.h"
	#define UART_BAUD 9600
#endif

#if defined (UDP_UPDATE) || defined (TCP_UPDATE) || defined (TCP_COMMAND_SRV) || defined (UDP_COMMAND_SRV)

#if !defined (XPAR_XAXIETHERNET_NUM_INSTANCES)
#error "No ethernet in design"
#endif

#if defined (__LWIPOPTS_H_)
#include "arch/cc.h"
#endif // __LWIPOPTS_H_

/* Platform timer is calibrated for 250 ms, so kept interval value 4 to call
 * eth_link_detect() at every one second
 */
#define ETH_LINK_DETECT_INTERVAL (4)
void tcp_fasttmr(void);
void tcp_slowtmr(void);
volatile int TcpFastTmrFlag = 0;
volatile int TcpSlowTmrFlag = 0;
//XIntc intc;
#if LWIP_DHCP==1
	volatile int dhcp_timoutcntr = 24;
	void dhcp_fine_tmr();
	void dhcp_coarse_tmr();
#endif // LWIP_DHCP

//#include "xtmrctr.h"
//#include "xtmrctr_l.h"
#define PLATFORM_EMAC_BASEADDR XPAR_AXIETHERNET_0_BASEADDR
#define PLATFORM_TIMER_BASEADDR XPAR_ETHERNET_SUBSYSTEM_AXI_TIMER_0_BASEADDR
#define PLATFORM_TIMER_INTERRUPT_INTR XPAR_PROCESSOR_SUBSYSTEM_MICROBLAZE_0_AXI_INTC_PROCESSOR_SUBSYSTEM_AXI_TIMER_0_INTERRUPT_INTR
#define PLATFORM_TIMER_INTERRUPT_MASK (1 << XPAR_PROCESSOR_SUBSYSTEM_MICROBLAZE_0_AXI_INTC_PROCESSOR_SUBSYSTEM_AXI_TIMER_0_INTERRUPT_INTR)

#endif // #if defined (UDP_UPDATE) || defined (TCP_UPDATE) || defined (TCP_COMMAND_SRV) || defined (UDP_COMMAND_SRV)

#define MHZ 400
#define PIT_INTERVAL (250*MHZ*1000)

void enable_caches(void)
{
	Xil_ICacheEnableRegion(CACHEABLE_REGION_MASK);
	Xil_DCacheEnableRegion(CACHEABLE_REGION_MASK);
}

void disable_caches(void)
{
//#if defined (ARMR5) || (__aarch64__) || (__arm__) || (__PPC__)
	Xil_ICacheDisable();
	Xil_DCacheDisable();
//#endif
}

void init_uart(void)
{
#ifdef STDOUT_IS_16550
    XUartNs550_SetBaud(STDOUT_BASEADDR, XPAR_XUARTNS550_CLOCK_HZ, UART_BAUD);
    XUartNs550_SetLineControlReg(STDOUT_BASEADDR, XUN_LCR_8_DATA_BITS);
#endif
    /* Bootrom/BSP configures PS7/PSU UART to 115200 bps */
}


#if defined (UDP_UPDATE) || defined (TCP_UPDATE) || defined (TCP_COMMAND_SRV) || defined (UDP_COMMAND_SRV)
#if defined (__AXI_TIMER_H_) && defined (XPAR_ETHERNET_SUBSYSTEM_AXI_TIMER_0_DEVICE_ID)

void timer_callback(void)
{
	static int DetectEthLinkStatus = 0;
	/* we need to call tcp_fasttmr & tcp_slowtmr at intervals specified by lwIP.
	 * It is not important that the timing is absoluetly accurate.
	 */
	static int odd = 1;
#if LWIP_DHCP==1
    static int dhcp_timer = 0;
#endif
	DetectEthLinkStatus++;

	TcpFastTmrFlag = 1;
	odd = !odd;
	if (odd) {

#if LWIP_DHCP==1
		dhcp_timer++;
		dhcp_timoutcntr--;
#endif
		TcpSlowTmrFlag = 1;
#if LWIP_DHCP==1
		dhcp_fine_tmr();
		if (dhcp_timer >= 120) {
			dhcp_coarse_tmr();
			dhcp_timer = 0;
		}
#endif
	}

	/* For detecting Ethernet phy link status periodically */
	if (DetectEthLinkStatus == ETH_LINK_DETECT_INTERVAL) {
		eth_link_detect(&server_netif);
		DetectEthLinkStatus = 0;
	}
}

void xadapter_timer_handler(void *p)
{
	timer_callback();

	XTime_TSRClearStatusBits(XREG_TSR_CLEAR_ALL);
}

void platform_setup_timer(void)
{
#ifdef XPAR_CPU_PPC440_CORE_CLOCK_FREQ_HZ
	Xil_ExceptionRegisterHandler(XIL_EXCEPTION_ID_DEC_INT,
			(XExceptionHandler)xadapter_timer_handler, NULL);

	/* Set DEC to interrupt every 250 mseconds */
	XTime_DECSetInterval(PIT_INTERVAL);
	XTime_TSRClearStatusBits(XREG_TSR_CLEAR_ALL);
	XTime_DECEnableAutoReload();
	XTime_DECEnableInterrupt();
#else
	Xil_ExceptionRegisterHandler(XIL_EXCEPTION_ID_PIT_INT,
			(XExceptionHandler)xadapter_timer_handler, NULL);

	/* Set PIT to interrupt every 250 mseconds */
	XTime_PITSetInterval(PIT_INTERVAL);
	XTime_TSRClearStatusBits(XREG_TSR_CLEAR_ALL);
	XTime_PITEnableAutoReload();
	XTime_PITEnableInterrupt();
#endif
}

#endif // #if defined (__AXI_TIMER_H_) && defined (XPAR_ETHERNET_SUBSYSTEM_AXI_TIMER_0_DEVICE_ID)
#endif // #if defined (UDP_UPDATE) || defined (TCP_UPDATE) || defined (TCP_COMMAND_SRV) || defined (UDP_COMMAND_SRV)

void platform_setup_interrupts(void)
{
	int Status;

	Status = XIntc_Initialize(&InterruptController, XPAR_INTC_0_DEVICE_ID);
	if (Status != XST_SUCCESS) {
		bsp_printf(TXT_RED "XIntc_Initialize Failed\r\n" TXT_RST);
		return XST_FAILURE;
	}

	Status = XIntc_Start(&InterruptController, XIN_REAL_MODE);
	if (Status != XST_SUCCESS) {
		bsp_printf(TXT_RED "XIntc_Start Failed\r\n" TXT_RST);
		return XST_FAILURE;
	}

	Xil_ExceptionInit();
	Xil_ExceptionRegisterHandler(XIL_EXCEPTION_ID_INT, (XExceptionHandler)INTC_HANDLER, &InterruptController);

#if defined (UDP_UPDATE) || defined (TCP_UPDATE) || defined (TCP_COMMAND_SRV) || defined (UDP_COMMAND_SRV)
#if defined (__AXI_TIMER_H_) && defined (XPAR_TMRCTR_0_DEVICE_ID)
	platform_setup_timer();
#endif // #if defined (__AXI_TIMER_H_) && defined (XPAR_TMRCTR_0_DEVICE_ID)
#endif // #if defined (UDP_UPDATE) || defined (TCP_UPDATE) || defined (TCP_COMMAND_SRV) || defined (UDP_COMMAND_SRV)

#ifdef XPAR_ETHERNET_MAC_IP2INTC_IRPT_MASK
	/* Enable timer and EMAC interrupts in the interrupt controller */
	XIntc_EnableIntr(XPAR_INTC_0_BASEADDR,
#ifdef __MICROBLAZE__
			PLATFORM_TIMER_INTERRUPT_MASK |
#endif
			XPAR_ETHERNET_MAC_IP2INTC_IRPT_MASK);
#endif


#ifdef XPAR_INTC_0_LLTEMAC_0_VEC_ID
//#ifdef __MICROBLAZE__
//	XIntc_Enable(intcp, PLATFORM_TIMER_INTERRUPT_INTR);
//#endif
	XIntc_Enable(intcp, XPAR_INTC_0_LLTEMAC_0_VEC_ID);
#endif


#ifdef XPAR_INTC_0_AXIETHERNET_0_VEC_ID
//	XIntc_Enable(&InterruptController, PLATFORM_TIMER_INTERRUPT_INTR);
	XIntc_Enable(&InterruptController, XPAR_INTC_0_AXIETHERNET_0_VEC_ID);
#endif


#ifdef XPAR_INTC_0_EMACLITE_0_VEC_ID
//#ifdef __MICROBLAZE__
//	XIntc_Enable(intcp, PLATFORM_TIMER_INTERRUPT_INTR);
//#endif
	XIntc_Enable(intcp, XPAR_INTC_0_EMACLITE_0_VEC_ID);
#endif

	return Status;
}

void platform_enable_interrupts()
{
	Xil_ExceptionEnable();
}

void init_platform(void)
{
    /*
     * If you want to run this example outside of SDK,
     * uncomment one of the following two lines and also #include "ps7_init.h"
     * or #include "ps7_init.h" at the top, depending on the target.
     * Make sure that the ps7/psu_init.c and ps7/psu_init.h files are included
     * along with this example source files for compilation.
     */
//#if defined (ARMR5) || (__arm__)
//    ps7_init();
//#endif
//#if defined (__aarch64__)
//    psu_init();
//#endif

    enable_caches();
    init_uart();

#if defined (INTC_DEVICE_ID) || defined (INTC)
	platform_setup_interrupts();
#endif // #if defined (INTC_DEVICE_ID) || defined (INTC)

}

void cleanup_platform(void)
{
    disable_caches();
}

#if defined (ARMR5) || (__aarch64__) || (__arm__) || (__PPC__)
//	#include "xtime_l.h"
uint64_t get_time_ms(void)
{
#define COUNTS_PER_MILLI_SECOND (COUNTS_PER_SECOND/1000)

#if defined(ARMR5)
	XTime tCur = 0;
	static XTime tlast = 0, tHigh = 0;
	u64_t time;
	XTime_GetTime(&tCur);
	if (tCur < tlast)
		tHigh++;
	tlast = tCur;
	time = (((u64_t) tHigh) << 32U) | (u64_t)tCur;
	return (time / COUNTS_PER_MILLI_SECOND);
#else // (__aarch64__) || (__arm__)
	XTime tCur = 0;
	XTime_GetTime(&tCur);
	return (tCur / COUNTS_PER_MILLI_SECOND);
#endif
}
float get_time_s(void)
{
#if defined(ARMR5)
	XTime tCur = 0;
	static XTime tlast = 0, tHigh = 0;
	u64_t time;
	XTime_GetTime(&tCur);
	if (tCur < tlast)
		tHigh++;
	tlast = tCur;
	time = (((u64_t) tHigh) << 32U) | (u64_t)tCur;
	return (time / (float) COUNTS_PER_SECOND);
#else // (__aarch64__) || (__arm__)
    XTime tCur = 0;
    XTime_GetTime(&tCur);
    return (tCur / (float) COUNTS_PER_SECOND);
#endif
}
#endif // #if defined (ARMR5) || (__aarch64__) || (__arm__) || (__PPC__)


#endif // __PPC__



