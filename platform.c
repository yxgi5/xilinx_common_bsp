#include "bsp.h"


/*
 * Uncomment one of the following two lines, depending on the target,
 * if ps7/psu init source files are added in the source directory for
 * compiling example outside of SDK.
 */
//#if defined (ARMR5) || (__arm__)
//#include "ps7_init.h"
//#endif
//#if defined (__aarch64__)
//#include "psu_init.h"
//#endif 
#ifdef STDOUT_IS_16550
	#include "xuartns550_l.h"
	#define UART_BAUD 9600
#endif

#if defined (ARMR5) || (__aarch64__) || (__arm__) || (__PPC__)
	#include "xtime_l.h"
#endif

#if defined (__UDP_UPDATE_H__) || defined (__TCP_UPDATE_H__)
	#if defined (PLATFORM_ZYNQ) || defined (PLATFORM_ZYNQMP)
		#include "arch/cc.h"
//		#define INTC_DEVICE_ID			XPAR_SCUGIC_SINGLE_DEVICE_ID
		#define INTC_DEVICE_ID			XPAR_SCUGIC_0_DEVICE_ID
		#define INTC_BASE_ADDR 			XPAR_SCUGIC_0_CPU_BASEADDR
		#define INTC_DIST_BASE_ADDR 	XPAR_SCUGIC_0_DIST_BASEADDR
		/* Platform timer is calibrated for 250 ms, so kept interval value 4 to call
		 * eth_link_detect() at every one second
		 */
		#define ETH_LINK_DETECT_INTERVAL (4)
		void tcp_fasttmr(void);
		void tcp_slowtmr(void);
		volatile int TcpFastTmrFlag = 0;
		volatile int TcpSlowTmrFlag = 0;
		XScuGic InterruptController; 	     /* Instance of the Interrupt Controller */
		#if LWIP_DHCP==1
			volatile int dhcp_timoutcntr = 24;
			void dhcp_fine_tmr();
			void dhcp_coarse_tmr();
		#endif // LWIP_DHCP
	#endif // PLATFORM_ZYNQ || PLATFORM_ZYNQMP

	#if defined (PLATFORM_ZYNQMP)
		#include "xttcps.h"
		#define TIMER_DEVICE_ID			XPAR_XTTCPS_0_DEVICE_ID
		#define TIMER_IRPT_INTR 		XPAR_XTTCPS_0_INTR
		#define PLATFORM_TIMER_INTR_RATE_HZ (4)
		static XInterval Interval;
		static u8 Prescaler;
		XTtcPs TimerInstance;
		//extern struct netif *netif;
	#endif // PLATFORM_ZYNQMP

	#if defined (PLATFORM_ZYNQ)
		#include "xscutimer.h"
		#define TIMER_DEVICE_ID			XPAR_SCUTIMER_DEVICE_ID
		#define TIMER_IRPT_INTR			XPAR_SCUTIMER_INTR
		#define RESET_RX_CNTR_LIMIT	(400)
		#ifndef USE_SOFTETH_ON_ZYNQ
			static int ResetRxCntr = 0;
		#endif
		XScuTimer TimerInstance;
		//extern struct netif *netif;
	#endif // PLATFORM_ZYNQ


	#if defined (__MICROBLAZE__) || defined (__PPC__)
		#include "arch/cc.h"
		/* Platform timer is calibrated for 250 ms, so kept interval value 4 to call
		 * eth_link_detect() at every one second
		 */
		#define ETH_LINK_DETECT_INTERVAL (4)
		void tcp_fasttmr(void);
		void tcp_slowtmr(void);
		volatile int TcpFastTmrFlag = 0;
		volatile int TcpSlowTmrFlag = 0;
		XIntc intc;
		#if LWIP_DHCP==1
			volatile int dhcp_timoutcntr = 24;
			void dhcp_fine_tmr();
			void dhcp_coarse_tmr();
		#endif // LWIP_DHCP
		//extern struct netif *echo_netif;
	#endif // __MICROBLAZE__ || __PPC__

	#if defined (__MICROBLAZE__)
		#include "xtmrctr_l.h"
		#define PLATFORM_EMAC_BASEADDR XPAR_AXI_ETHERNET_0_BASEADDR
		#define PLATFORM_TIMER_BASEADDR XPAR_AXI_TIMER_0_BASEADDR
		#define PLATFORM_TIMER_INTERRUPT_INTR XPAR_MICROBLAZE_0_AXI_INTC_AXI_TIMER_0_INTERRUPT_INTR
		#define PLATFORM_TIMER_INTERRUPT_MASK (1 << XPAR_MICROBLAZE_0_AXI_INTC_AXI_TIMER_0_INTERRUPT_INTR)
	#endif // __MICROBLAZE__

	#if defined (__PPC__)
		#include "xexception_l.h"
//		#include "xtime_l.h"
	#endif // __PPC__

#endif // __UDP_UPDATE_H__

void enable_caches(void)
{
#ifdef __PPC__
    Xil_ICacheEnableRegion(CACHEABLE_REGION_MASK);
    Xil_DCacheEnableRegion(CACHEABLE_REGION_MASK);
#elif __MICROBLAZE__
#ifdef XPAR_MICROBLAZE_USE_ICACHE
    Xil_ICacheInvalidate();
    Xil_ICacheEnable();
#endif
#ifdef XPAR_MICROBLAZE_USE_DCACHE
    Xil_DCacheInvalidate();
    Xil_DCacheEnable();
#endif
#endif
#if defined (ARMR5) || (__aarch64__) || (__arm__)
	Xil_ICacheEnable();
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
#if defined (ARMR5) || (__aarch64__) || (__arm__) || (__PPC__)
	Xil_ICacheDisable();
	Xil_DCacheDisable();
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

#if defined (UDP_UPDATE) || defined (TCP_UPDATE)

#if defined (__MICROBLAZE__) || defined (__PPC__)
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
#endif // __MICROBLAZE__ || __PPC__

#if defined (PLATFORM_ZYNQ)
void timer_callback(XScuTimer * TimerInstance)
{
	static int DetectEthLinkStatus = 0;
	/* we need to call tcp_fasttmr & tcp_slowtmr at intervals specified
	 * by lwIP. It is not important that the timing is absoluetly accurate.
	 */
	static int odd = 1;
#if LWIP_DHCP==1
    static int dhcp_timer = 0;
#endif
	DetectEthLinkStatus++;
	 TcpFastTmrFlag = 1;

	odd = !odd;
#ifndef USE_SOFTETH_ON_ZYNQ
	ResetRxCntr++;
#endif
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

	/* For providing an SW alternative for the SI #692601. Under heavy
	 * Rx traffic if at some point the Rx path becomes unresponsive, the
	 * following API call will ensures a SW reset of the Rx path. The
	 * API xemacpsif_resetrx_on_no_rxdata is called every 100 milliseconds.
	 * This ensures that if the above HW bug is hit, in the worst case,
	 * the Rx path cannot become unresponsive for more than 100
	 * milliseconds.
	 */
#ifndef USE_SOFTETH_ON_ZYNQ
	if (ResetRxCntr >= RESET_RX_CNTR_LIMIT) {
		xemacpsif_resetrx_on_no_rxdata(&server_netif);
		ResetRxCntr = 0;
	}
#endif
	/* For detecting Ethernet phy link status periodically */
	if (DetectEthLinkStatus == ETH_LINK_DETECT_INTERVAL) {
		eth_link_detect(&server_netif);
		DetectEthLinkStatus = 0;
	}

	XScuTimer_ClearInterruptStatus(TimerInstance);
}
#endif // PLATFORM_ZYNQ

#if defined (PLATFORM_ZYNQMP)
void platform_clear_interrupt( XTtcPs * TimerInstance )
{
	u32 StatusEvent;

	StatusEvent = XTtcPs_GetInterruptStatus(TimerInstance);
	XTtcPs_ClearInterruptStatus(TimerInstance, StatusEvent);
}


void timer_callback(XTtcPs * TimerInstance)
{
	static int DetectEthLinkStatus = 0;
	/* we need to call tcp_fasttmr & tcp_slowtmr at intervals specified
	 * by lwIP. It is not important that the timing is absoluetly accurate.
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

	platform_clear_interrupt(TimerInstance);
}
#endif // PLATFORM_ZYNQMP


#if defined (__MICROBLAZE__)
void xadapter_timer_handler(void *p)
{
	timer_callback();

	/* Load timer, clear interrupt bit */
	XTmrCtr_SetControlStatusReg(PLATFORM_TIMER_BASEADDR, 0,
			XTC_CSR_INT_OCCURED_MASK
			| XTC_CSR_LOAD_MASK);

	XTmrCtr_SetControlStatusReg(PLATFORM_TIMER_BASEADDR, 0,
			XTC_CSR_ENABLE_TMR_MASK
			| XTC_CSR_ENABLE_INT_MASK
			| XTC_CSR_AUTO_RELOAD_MASK
			| XTC_CSR_DOWN_COUNT_MASK);

	XIntc_AckIntr(XPAR_INTC_0_BASEADDR, PLATFORM_TIMER_INTERRUPT_MASK);
}

#define MHZ (66)
#define TIMER_TLR (25000000*((float)MHZ/100))
#endif // __MICROBLAZE__

#if defined (__PPC__)
void xadapter_timer_handler(void *p)
{
	timer_callback();

	XTime_TSRClearStatusBits(XREG_TSR_CLEAR_ALL);
}

#define MHZ 400
#define PIT_INTERVAL (250*MHZ*1000)
#endif // __PPC__

void platform_setup_timer(void)
{
#if defined (__MICROBLAZE__)
	/* set the number of cycles the timer counts before interrupting */
	/* 100 Mhz clock => .01us for 1 clk tick. For 100ms, 10000000 clk ticks need to elapse  */
	XTmrCtr_SetLoadReg(PLATFORM_TIMER_BASEADDR, 0, TIMER_TLR);

	/* reset the timers, and clear interrupts */
	XTmrCtr_SetControlStatusReg(PLATFORM_TIMER_BASEADDR, 0, XTC_CSR_INT_OCCURED_MASK | XTC_CSR_LOAD_MASK );

	/* start the timers */
	XTmrCtr_SetControlStatusReg(PLATFORM_TIMER_BASEADDR, 0,
			XTC_CSR_ENABLE_TMR_MASK | XTC_CSR_ENABLE_INT_MASK
			| XTC_CSR_AUTO_RELOAD_MASK | XTC_CSR_DOWN_COUNT_MASK);

	/* Register Timer handler */
	XIntc_RegisterHandler(XPAR_INTC_0_BASEADDR,
			PLATFORM_TIMER_INTERRUPT_INTR,
			(XInterruptHandler)xadapter_timer_handler,
			0);
#endif // __MICROBLAZE__

#if  defined (__PPC__)
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
#endif // __PPC__

#if defined (PLATFORM_ZYNQ)
	int Status = XST_SUCCESS;
	XScuTimer_Config *ConfigPtr;
	int TimerLoadValue = 0;

	ConfigPtr = XScuTimer_LookupConfig(TIMER_DEVICE_ID);
	Status = XScuTimer_CfgInitialize(&TimerInstance, ConfigPtr,
			ConfigPtr->BaseAddr);
	if (Status != XST_SUCCESS) {

		xil_printf("In %s: Scutimer Cfg initialization failed...\r\n",
		__func__);
		return;
	}

	Status = XScuTimer_SelfTest(&TimerInstance);
	if (Status != XST_SUCCESS) {
		xil_printf("In %s: Scutimer Self test failed...\r\n",
		__func__);
		return;

	}

	XScuTimer_EnableAutoReload(&TimerInstance);
	/*
	 * Set for 250 milli seconds timeout.
	 */
	TimerLoadValue = XPAR_CPU_CORTEXA9_0_CPU_CLK_FREQ_HZ / 8;

	XScuTimer_LoadTimer(&TimerInstance, TimerLoadValue);
#endif // PLATFORM_ZYNQ

#if defined (PLATFORM_ZYNQMP)
	int Status;
	XTtcPs * Timer = &TimerInstance;
	XTtcPs_Config *Config;


	Config = XTtcPs_LookupConfig(TIMER_DEVICE_ID);

	Status = XTtcPs_CfgInitialize(Timer, Config, Config->BaseAddress);
	if (Status != XST_SUCCESS) {
		xil_printf("In %s: Timer Cfg initialization failed...\r\n",
				__func__);
				return;
	}
	XTtcPs_SetOptions(Timer, XTTCPS_OPTION_INTERVAL_MODE | XTTCPS_OPTION_WAVE_DISABLE);
	XTtcPs_CalcIntervalFromFreq(Timer, PLATFORM_TIMER_INTR_RATE_HZ, &Interval, &Prescaler);
	XTtcPs_SetInterval(Timer, Interval);
	XTtcPs_SetPrescaler(Timer, Prescaler);
#endif // PLATFORM_ZYNQMP
}

void platform_setup_interrupts(void)
{
#if defined (PLATFORM_ZYNQMP)  || defined (PLATFORM_ZYNQ)
	int Status;
	XScuGic_Config *GicConfig;    /* The configuration parameters of the controller */

	Xil_ExceptionInit();

//	XScuGic_DeviceInitialize(INTC_DEVICE_ID);

	/*
	 * Initialize the interrupt controller driver so that it is ready to
	 * use.
	 */
	GicConfig = XScuGic_LookupConfig(INTC_DEVICE_ID);
	if (NULL == GicConfig) {
		return XST_FAILURE;
	}

	Status = XScuGic_CfgInitialize(&InterruptController, GicConfig,
					GicConfig->CpuBaseAddress);
	if (Status != XST_SUCCESS) {
		return XST_FAILURE;
	}
	/*
	 * Connect the interrupt controller interrupt handler to the hardware
	 * interrupt handling logic in the processor.
	 */
	Xil_ExceptionRegisterHandler(XIL_EXCEPTION_ID_IRQ_INT,
			(Xil_ExceptionHandler)XScuGic_DeviceInterruptHandler,
			(void *)INTC_DEVICE_ID);
//	/*
//	 * Connect the device driver handler that will be called when an
//	 * interrupt for the device occurs, the handler defined above performs
//	 * the specific interrupt processing for the device.
//	 */
//	XScuGic_RegisterHandler(INTC_BASE_ADDR, TIMER_IRPT_INTR,
//					(Xil_ExceptionHandler)timer_callback,
//					(void *)&TimerInstance);
//	/*
//	 * Enable the interrupt for scu timer.
//	 */
//	XScuGic_EnableIntr(INTC_DIST_BASE_ADDR, TIMER_IRPT_INTR);

	/*
	 * Connect a device driver handler that will be called when an
	 * interrupt for the device occurs, the device driver handler performs
	 * the specific interrupt processing for the device
	 */
	Status = XScuGic_Connect(&InterruptController, TIMER_IRPT_INTR,
			   (Xil_ExceptionHandler)timer_callback,
			   (void *)&TimerInstance);

	if (Status != XST_SUCCESS) {
		return XST_FAILURE;
	}

	return;
#endif // PLATFORM_ZYNQMP || PLATFORM_ZYNQ

#if defined (__MICROBLAZE__) || defined (__PPC__)
	XIntc *intcp;
	intcp = &intc;

	XIntc_Initialize(intcp, XPAR_INTC_0_DEVICE_ID);
	XIntc_Start(intcp, XIN_REAL_MODE);

	/* Start the interrupt controller */
	XIntc_MasterEnable(XPAR_INTC_0_BASEADDR);

#ifdef __PPC__
	Xil_ExceptionInit();
	Xil_ExceptionRegisterHandler(XIL_EXCEPTION_ID_INT,
			(XExceptionHandler)XIntc_DeviceInterruptHandler,
			(void*) XPAR_INTC_0_DEVICE_ID);
#elif __MICROBLAZE__
	microblaze_register_handler((XInterruptHandler)XIntc_InterruptHandler, intcp);
#endif

	platform_setup_timer();

#ifdef XPAR_ETHERNET_MAC_IP2INTC_IRPT_MASK
	/* Enable timer and EMAC interrupts in the interrupt controller */
	XIntc_EnableIntr(XPAR_INTC_0_BASEADDR,
#ifdef __MICROBLAZE__
			PLATFORM_TIMER_INTERRUPT_MASK |
#endif
			XPAR_ETHERNET_MAC_IP2INTC_IRPT_MASK);
#endif


#ifdef XPAR_INTC_0_LLTEMAC_0_VEC_ID
#ifdef __MICROBLAZE__
	XIntc_Enable(intcp, PLATFORM_TIMER_INTERRUPT_INTR);
#endif
	XIntc_Enable(intcp, XPAR_INTC_0_LLTEMAC_0_VEC_ID);
#endif


#ifdef XPAR_INTC_0_AXIETHERNET_0_VEC_ID
	XIntc_Enable(intcp, PLATFORM_TIMER_INTERRUPT_INTR);
	XIntc_Enable(intcp, XPAR_INTC_0_AXIETHERNET_0_VEC_ID);
#endif


#ifdef XPAR_INTC_0_EMACLITE_0_VEC_ID
#ifdef __MICROBLAZE__
	XIntc_Enable(intcp, PLATFORM_TIMER_INTERRUPT_INTR);
#endif
	XIntc_Enable(intcp, XPAR_INTC_0_EMACLITE_0_VEC_ID);
#endif

#endif // __MICROBLAZE__ || __PPC__
}

void platform_enable_interrupts()
{
#if defined (__MICROBLAZE__)
	microblaze_enable_interrupts();
#endif // __MICROBLAZE__

#if defined (__PPC__)
	Xil_ExceptionEnable();
#endif // __PPC__

#if defined (PLATFORM_ZYNQ)
	/*
	 * Enable non-critical exceptions.
	 */
	Xil_ExceptionEnableMask(XIL_EXCEPTION_IRQ);
	XScuGic_Enable(&InterruptController, TIMER_IRPT_INTR);
	XScuTimer_EnableInterrupt(&TimerInstance);
	XScuTimer_Start(&TimerInstance);
#endif // PLATFORM_ZYNQ
#if defined (PLATFORM_ZYNQMP)
	/*
	 * Enable non-critical exceptions.
	 */
	Xil_ExceptionEnableMask(XIL_EXCEPTION_IRQ);
//	XScuGic_EnableIntr(XPAR_SCUGIC_0_DIST_BASEADDR, XPAR_XTTCPS_0_INTR);
//	XTtcPs_EnableInterrupts(&TimerInstance, XTTCPS_IXR_INTERVAL_MASK);

	/*
	 * Enable the interrupt for the Timer counter
	 */
	XScuGic_Enable(&InterruptController, TIMER_IRPT_INTR);

	/*
	 * Enable the interrupts for the tick timer/counter
	 * We only care about the interval timeout.
	 */
	XTtcPs_EnableInterrupts(&TimerInstance, XTTCPS_IXR_INTERVAL_MASK);

	XTtcPs_Start(&TimerInstance);
	return;
#endif // PLATFORM_ZYNQMP
}

#if defined (ARMR5) || (__aarch64__) || (__arm__)
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
#endif

#endif // UDP_UPDATE || TCP_UPDATE

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

#if defined (UDP_UPDATE) || defined (TCP_UPDATE)

	#if defined (PLATFORM_ZYNQ) || defined (PLATFORM_ZYNQMP)
		platform_setup_timer();
	#endif // PLATFORM_ZYNQ  || PLATFORM_ZYNQMP

		platform_setup_interrupts();

#endif // UDP_UPDATE || TCP_UPDATE
}

void cleanup_platform(void)
{
    disable_caches();
}
