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

//#if defined (UDP_UPDATE) && defined (PLATFORM_ZYNQMP)
#if defined (__UDP_UPDATE_H__) && defined (PLATFORM_ZYNQMP)
#include "xttcps.h"
#include "arch/cc.h"
#define ETH_LINK_DETECT_INTERVAL 4
#define PLATFORM_TIMER_INTR_RATE_HZ (4)
static XInterval Interval;
static u8 Prescaler;
volatile int TcpFastTmrFlag = 0;
volatile int TcpSlowTmrFlag = 0;
XScuGic InterruptController; 	     /* Instance of the Interrupt Controller */
XTtcPs TimerInstance;
#endif // __UDP_UPDATE_H__ && PLATFORM_ZYNQMP

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

#if defined (UDP_UPDATE)
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
		eth_link_detect(echo_netif);
		DetectEthLinkStatus = 0;
	}

	platform_clear_interrupt(TimerInstance);
}
#endif // PLATFORM_ZYNQMP

void platform_setup_timer(void)
{
#if defined (PLATFORM_ZYNQMP)
	int Status;
	XTtcPs * Timer = &TimerInstance;
	XTtcPs_Config *Config;


	Config = XTtcPs_LookupConfig(XPAR_XTTCPS_0_DEVICE_ID);

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
#if defined (PLATFORM_ZYNQMP)
	int Status;
	XScuGic_Config *GicConfig;    /* The configuration parameters of the controller */

	Xil_ExceptionInit();

//	XScuGic_DeviceInitialize(XPAR_SCUGIC_SINGLE_DEVICE_ID);

	/*
	 * Initialize the interrupt controller driver so that it is ready to
	 * use.
	 */
	GicConfig = XScuGic_LookupConfig(XPAR_SCUGIC_0_DEVICE_ID);
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
			(void *)XPAR_SCUGIC_SINGLE_DEVICE_ID);
//	/*
//	 * Connect the device driver handler that will be called when an
//	 * interrupt for the device occurs, the handler defined above performs
//	 * the specific interrupt processing for the device.
//	 */
//	XScuGic_RegisterHandler(XPAR_SCUGIC_0_CPU_BASEADDR, XPAR_XTTCPS_0_INTR,
//					(Xil_ExceptionHandler)timer_callback,
//					(void *)&TimerInstance);
//	/*
//	 * Enable the interrupt for scu timer.
//	 */
//	XScuGic_EnableIntr(XPAR_SCUGIC_0_DIST_BASEADDR, XPAR_XTTCPS_0_INTR);

	/*
	 * Connect a device driver handler that will be called when an
	 * interrupt for the device occurs, the device driver handler performs
	 * the specific interrupt processing for the device
	 */
	Status = XScuGic_Connect(&InterruptController, XPAR_XTTCPS_0_INTR,
			   (Xil_ExceptionHandler)timer_callback,
			   (void *)&TimerInstance);

	if (Status != XST_SUCCESS) {
		return XST_FAILURE;
	}

	return;
#endif // PLATFORM_ZYNQMP
}

void platform_enable_interrupts()
{
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
	XScuGic_Enable(&InterruptController, XPAR_XTTCPS_0_INTR);

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
u64_t get_time_ms()
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
	return (time/COUNTS_PER_MILLI_SECOND);
#else // (__aarch64__) || (__arm__)
	XTime tCur = 0;
	XTime_GetTime(&tCur);
	return (tCur/COUNTS_PER_MILLI_SECOND);
#endif
}
#endif

#endif // UDP_UPDATE

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
#if defined (UDP_UPDATE)
	platform_setup_timer();
	platform_setup_interrupts();
#endif
}

void cleanup_platform(void)
{
    disable_caches();
}
