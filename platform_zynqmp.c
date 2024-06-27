#include "bsp.h"


#if defined (PLATFORM_ZYNQMP)

#if defined (INTC_DEVICE_ID) || defined (INTC)
INTC InterruptController;
#endif

#ifdef STDOUT_IS_16550
	#include "xuartns550_l.h"
	#define UART_BAUD 9600
#endif

#if defined (UDP_UPDATE) || defined (TCP_UPDATE) || defined (TCP_COMMAND_SRV) || defined (UDP_COMMAND_SRV)

#if defined (__LWIPOPTS_H_)
#include "arch/cc.h"
#endif // __LWIPOPTS_H_

//		#define INTC_DEVICE_ID			XPAR_SCUGIC_SINGLE_DEVICE_ID
//#define INTC_DEVICE_ID			XPAR_SCUGIC_0_DEVICE_ID
//#define INTC_BASE_ADDR 			XPAR_SCUGIC_0_CPU_BASEADDR
//#define INTC_DIST_BASE_ADDR 	XPAR_SCUGIC_0_DIST_BASEADDR
/* Platform timer is calibrated for 250 ms, so kept interval value 4 to call
 * eth_link_detect() at every one second
 */
#define ETH_LINK_DETECT_INTERVAL (4)
void tcp_fasttmr(void);
void tcp_slowtmr(void);
volatile int TcpFastTmrFlag = 0;
volatile int TcpSlowTmrFlag = 0;
#if LWIP_DHCP==1
	volatile int dhcp_timoutcntr = 24;
	void dhcp_fine_tmr();
	void dhcp_coarse_tmr();
#endif // LWIP_DHCP

#include "xttcps.h"
#define TIMER_DEVICE_ID			XPAR_XTTCPS_0_DEVICE_ID
#define TIMER_IRPT_INTR 		XPAR_XTTCPS_0_INTR
#define PLATFORM_TIMER_INTR_RATE_HZ (4)
static XInterval Interval;
static u8 Prescaler;
XTtcPs TimerInstance;

#define PLATFORM_EMAC_BASEADDR XPAR_XEMACPS_0_BASEADDR

#endif // #if defined (UDP_UPDATE) || defined (TCP_UPDATE) || defined (TCP_COMMAND_SRV) || defined (UDP_COMMAND_SRV)

void enable_caches(void)
{
#if defined (ARMR5) || (__aarch64__) || (__arm__)
	Xil_ICacheEnable();
	Xil_DCacheEnable();
#endif
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

void platform_clear_interrupt( XTtcPs * TimerInstance )
{
	u32 StatusEvent;

	StatusEvent = XTtcPs_GetInterruptStatus(TimerInstance);
	XTtcPs_ClearInterruptStatus(TimerInstance, StatusEvent);
}

#if defined (UDP_UPDATE) || defined (TCP_UPDATE) || defined defined (TCP_COMMAND_SRV) || defined (UDP_COMMAND_SRV)

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


void platform_setup_timer(void)
{
	int Status;
	XTtcPs * Timer = &TimerInstance;
	XTtcPs_Config *Config;


	Config = XTtcPs_LookupConfig(TIMER_DEVICE_ID);

	Status = XTtcPs_CfgInitialize(Timer, Config, Config->BaseAddress);
	if (Status != XST_SUCCESS) {
		bsp_printf(TXT_RED "In %s: Timer Cfg initialization failed...\r\n" TXT_RST, __func__);
				return;
	}
	XTtcPs_SetOptions(Timer, XTTCPS_OPTION_INTERVAL_MODE | XTTCPS_OPTION_WAVE_DISABLE);
	XTtcPs_CalcIntervalFromFreq(Timer, PLATFORM_TIMER_INTR_RATE_HZ, &Interval, &Prescaler);
	XTtcPs_SetInterval(Timer, Interval);
	XTtcPs_SetPrescaler(Timer, Prescaler);
}

#endif // #if defined (UDP_UPDATE) || defined (TCP_UPDATE) || defined (TCP_COMMAND_SRV) || defined (UDP_COMMAND_SRV)

int platform_setup_interrupts(void)
{
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
		bsp_printf(TXT_RED "In %s: XScuGic_CfgInitialize failed...\r\n" TXT_RST, __func__);
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

#if defined (UDP_UPDATE) || defined (TCP_UPDATE) || defined (TCP_COMMAND_SRV) || defined (UDP_COMMAND_SRV)
	platform_setup_timer();
	Status = XScuGic_Connect(&InterruptController, TIMER_IRPT_INTR,
			   (Xil_ExceptionHandler)timer_callback,
			   (void *)&TimerInstance);
	if (Status != XST_SUCCESS) {
		bsp_printf(TXT_RED "In %s: Scutimer intr setup failed...\r\n" TXT_RST, __func__);
		return XST_FAILURE;
	}
#endif // #if defined (UDP_UPDATE) || defined (TCP_UPDATE) || defined (TCP_COMMAND_SRV) || defined (UDP_COMMAND_SRV)

#if defined (XPAR_MODBUS_RTU_0_AXI_TIMER_0_DEVICE_ID) && !defined (XPAR_ETHERNET_SUBSYSTEM_AXI_TIMER_0_DEVICE_ID)
	Status = timer0_init();
	if (Status != XST_SUCCESS) {
		bsp_printf(TXT_RED "In %s: timer0_init failed...\r\n" TXT_RST, __func__);
		return XST_FAILURE;
	}
#endif // #if defined (XPAR_MODBUS_RTU_0_AXI_TIMER_0_DEVICE_ID) && !defined (XPAR_ETHERNET_SUBSYSTEM_AXI_TIMER_0_DEVICE_ID)

	return Status;
}

void platform_enable_interrupts()
{
	/*
	 * Enable non-critical exceptions.
	 */
//	Xil_ExceptionEnableMask(XIL_EXCEPTION_IRQ);
//	XScuGic_EnableIntr(XPAR_SCUGIC_0_DIST_BASEADDR, XPAR_XTTCPS_0_INTR);
//	XTtcPs_EnableInterrupts(&TimerInstance, XTTCPS_IXR_INTERVAL_MASK);

	Xil_ExceptionEnable();

#if defined (UDP_UPDATE) || defined (TCP_UPDATE) || defined (TCP_COMMAND_SRV) || defined (UDP_COMMAND_SRV)
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
#endif // #if defined (UDP_UPDATE) || defined (TCP_UPDATE) || defined (TCP_COMMAND_SRV) || defined (UDP_COMMAND_SRV)
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


#endif // PLATFORM_ZYNQMP


