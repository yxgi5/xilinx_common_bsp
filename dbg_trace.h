#ifndef __DEBUG_TRACE_H__
#define __DEBUG_TRACE_H__

#define TRACE_ON 1
#define bsp_printf(fmt, arg...) 		\
	do { 								\
		if (TRACE_ON)					\
			xil_printf(fmt , ## arg); 	\
	} while (0)

#define bsp_info(fmt, arg...)			\
	do { 								\
		if (TRACE_ON)					\
			xil_printf("%s(%d): "fmt, __FILE__, __LINE__, ## arg) \
	} while (0)

#define bsp_dbg(fmt, arg...)			\
	do { 								\
		if (TRACE_ON)					\
			xil_printf("%s(%d)-<%s>: "fmt, __FILE__, __LINE__, __FUNCTION__, ## arg) \
	} while (0)


#define _DEBUG_TRACE_ZZG_ 3
#if 0 != _DEBUG_TRACE_ZZG_
//	#include "stdio.h"	// printf()支持专义, 支持浮点数
	#include "xil_printf.h" // xil_printf()支持专义但不支持浮点数, print()只用来打印字符串
#endif

#if 1==_DEBUG_TRACE_ZZG_ //普通打印
	#define TRACE_ZZG xil_printf
#elif 2==_DEBUG_TRACE_ZZG_ //打印文件名、行号
	#define TRACE_ZZG(fmt,...) \
		xil_printf("%s(%d): "fmt, __FILE__, __LINE__, ##__VA_ARGS__)
#elif 3==_DEBUG_TRACE_ZZG_ //打印文件名、行号、函数名
	#define TRACE_ZZG(fmt,...) \
		xil_printf("%s(%d)-<%s>: "fmt, __FILE__, __LINE__, __FUNCTION__, ##__VA_ARGS__)
#else
	#define TRACE_ZZG	// 关闭所有打印
#endif

#if 0
#define DEBUG_PRINT_ALWAYS    (0x00000001U)    /* unconditional messages */
#define DEBUG_GENERAL	      (0x00000002U)    /* general debug  messages */
#define DEBUG_INFO	          (0x00000004U)    /* More debug information */
#define DEBUG_DETAILED	      (0x00000008U)    /* More debug information */

#if defined (XXX_DEBUG_DETAILED)
#define XFsblDbgCurrentTypes ((DEBUG_DETAILED) | (DEBUG_INFO) | \
         (DEBUG_GENERAL) | (DEBUG_PRINT_ALWAYS))
#elif defined (XXX_DEBUG_INFO)
#define XFsblDbgCurrentTypes ((DEBUG_INFO) | (DEBUG_GENERAL) | \
         (DEBUG_PRINT_ALWAYS))
#elif defined (XXX_DEBUG)
#define XFsblDbgCurrentTypes ((DEBUG_GENERAL) | (DEBUG_PRINT_ALWAYS))
#elif defined (XXX_PRINT)
#define XFsblDbgCurrentTypes (DEBUG_PRINT_ALWAYS)
#else
#define XFsblDbgCurrentTypes (0U)
#endif
#define bsp_printf(DebugType,...) \
		if(((DebugType) & CurDbgTypes)!=0)) {xil_printf (__VA_ARGS__); }
#endif

#if 0
#define    LOG_EMERG    0
#define    LOG_ALERT    1
#define    LOG_CRIT     2
#define    LOG_ERR      3
#define    LOG_WARNING  4
#define    LOG_NOTICE   5
#define    LOG_INFO     6
#define    LOG_DEBUG    7
#define    LOG_LEVEL	(LOG_INFO)
#define bsp_printf(level, fmt, arg...) \
	do { 								\
		if (debug >= (level))					\
			xil_printf(fmt , ## arg); 	\
	} while (0)
#define bsp_err(fmt, arg...) \
	xil_printf(fmt , ## arg)
#define bsp_warn(fmt, arg...)			\
	xil_printf(fmt , ## arg)
#define bsp_info(fmt, arg...)			\
	xil_printf(fmt , ## arg)
#define bsp_dbg(level, debug, fmt, arg...)			\
	do { 								\
		if (debug >= (level))					\
			xil_printf(fmt , ## arg); 	\
	} while (0)
#endif

#endif // __DEBUG_TRACE_H__
