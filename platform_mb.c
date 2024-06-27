#include "bsp.h"


#if defined (__MICROBLAZE__)

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
#define PLATFORM_TIMER_INTERRUPT_INTR XPAR_PROCESSOR_SUBSYSTEM_MICROBLAZE_0_AXI_INTC_ETHERNET_SUBSYSTEM_AXI_TIMER_0_INTERRUPT_INTR
#define PLATFORM_TIMER_INTERRUPT_MASK (1 << XPAR_PROCESSOR_SUBSYSTEM_MICROBLAZE_0_AXI_INTC_ETHERNET_SUBSYSTEM_AXI_TIMER_0_INTERRUPT_INTR)

#endif // #if defined (UDP_UPDATE) || defined (TCP_UPDATE) || defined (TCP_COMMAND_SRV) || defined (UDP_COMMAND_SRV)


void enable_caches(void)
{
#ifdef XPAR_MICROBLAZE_USE_ICACHE
    Xil_ICacheInvalidate();
    Xil_ICacheEnable();
#endif
#ifdef XPAR_MICROBLAZE_USE_DCACHE
    Xil_DCacheInvalidate();
    Xil_DCacheEnable();
#endif
}

void disable_caches(void)
{
#ifdef __MICROBLAZE__
#ifdef XPAR_MICROBLAZE_USE_DCACHE
    Xil_DCacheDisable();
#endif
#ifdef XPAR_MICROBLAZE_USE_ICACHE
    Xil_ICacheDisable();
#endif
#endif
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

void timer00_callback(void)
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

void Timer0Handler(void *CallBackRef, u8 TmrCtrNumber)
{
	XTmrCtr *InstancePtr = (XTmrCtr *)CallBackRef;
//	static int counter = 0;
	/*
	 * Check if the timer counter has expired, checking is not necessary
	 * since that's the reason this function is executed, this just shows
	 * how the callback reference can be used as a pointer to the instance
	 * of the timer counter that expired, increment a shared variable so
	 * the main thread of execution can see the timer expired
	 */
	if (XTmrCtr_IsExpired(InstancePtr, TmrCtrNumber)) {
		if (TmrCtrNumber == 0) {
			timer00_callback();
		}
//		if (TmrCtrNumber == 1) {
//
//		}
	}
}

#endif // #if defined (__AXI_TIMER_H_) && defined (XPAR_ETHERNET_SUBSYSTEM_AXI_TIMER_0_DEVICE_ID)
#endif // #if defined (UDP_UPDATE) || defined (TCP_UPDATE) || defined (TCP_COMMAND_SRV) || defined (UDP_COMMAND_SRV)

void Timer1Handler(void *CallBackRef, u8 TmrCtrNumber)
{
	XTmrCtr *InstancePtr = (XTmrCtr *)CallBackRef;
//	static int counter = 0;
	/*
	 * Check if the timer counter has expired, checking is not necessary
	 * since that's the reason this function is executed, this just shows
	 * how the callback reference can be used as a pointer to the instance
	 * of the timer counter that expired, increment a shared variable so
	 * the main thread of execution can see the timer expired
	 */
	if (XTmrCtr_IsExpired(InstancePtr, TmrCtrNumber)) {
		if (TmrCtrNumber == 0) {
#if defined (MODBUS_RTU_SLAVE)
			g_mods_timeout = 1;
//			XTmrCtr_SetOptions(InstancePtr, TmrCtrNumber, 0);
			XTmrCtr_Stop(InstancePtr, TmrCtrNumber);
#endif // #if defined (MODBUS_RTU_SLAVE)
		}
//		if (TmrCtrNumber == 1) {
//
//		}
	}
}


#if defined (INTC_DEVICE_ID) || defined (INTC)

int platform_setup_interrupts(void)
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
#if defined (__AXI_TIMER_H_) && defined (XPAR_ETHERNET_SUBSYSTEM_AXI_TIMER_0_DEVICE_ID)
	Status = timer0_init();
	if (Status != XST_SUCCESS) {
		bsp_printf(TXT_RED "timer0_init Failed\r\n" TXT_RST);
		return XST_FAILURE;
	}
#endif // #if defined (__AXI_TIMER_H_) && defined (XPAR_ETHERNET_SUBSYSTEM_AXI_TIMER_0_DEVICE_ID)
#endif // #if defined (UDP_UPDATE) || defined (TCP_UPDATE) || defined (TCP_COMMAND_SRV) || defined (UDP_COMMAND_SRV)

#if defined (__AXI_TIMER_H_) && defined (MODBUS_RTU_SLAVE)
	Status = timer1_init();
	if (Status != XST_SUCCESS) {
		bsp_printf(TXT_RED "timer1_init Failed\r\n" TXT_RST);
		return XST_FAILURE;
	}
#endif // #if defined (__AXI_TIMER_H_) && defined (MODBUS_RTU_SLAVE)

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
//	XIntc_Enable(&InterruptController, PLATFORM_TIMER_INTERRUPT_INTR); // already enabled in timer0_init()
	XIntc_Enable(&InterruptController, XPAR_INTC_0_AXIETHERNET_0_VEC_ID); // WTF is this?
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
	//microblaze_enable_interrupts();
	Xil_ExceptionEnable();
}

#endif // #if defined (INTC_DEVICE_ID) || defined (INTC)

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

#endif // __MICROBLAZE__

