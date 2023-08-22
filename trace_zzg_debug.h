/*
 * trace_zzg_debug.h
 *
 *  Created on: Dec 21, 2018
 *      Author: alex
 *  function: 简单控制全局打印
 */

#ifndef TRACE_ZZG_DEBUG_H_
#define TRACE_ZZG_DEBUG_H_

#define _DEBUG_TRACE_ZZG_ 2
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

#endif /* TRACE_ZZG_DEBUG_H_ */
