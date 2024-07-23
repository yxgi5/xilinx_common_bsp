/*
 * version.h
 *
 *  Created on: Dec 20, 2018
 *      Author: alex
 */

#ifndef __VERSION_H__

#if defined (SW_VER_BY_COMPILE_TIME)
#define __VERSION_H__

#include <string.h>
#include <stdio.h>
#include <ctype.h>
#include "xparameters.h"
#include "xil_types.h"
#include "xstatus.h"

extern u32 __SW_VER__;
#if defined (XPAR_AXI_LITE_REG_NUM_INSTANCES) && (XPAR_AXI_LITE_REG_0_DEVICE_ID == 0)
extern volatile u32 __HW_VER__;
#endif

//时间结构体
typedef struct
{
    u8 nHour;
    u8 nMinute;
    u8 nSecond;

    //公历日月年周
    u8  nMonth;
    u8  nDay;
    u8  nWeek;
    u16 nYear;

} RTC_TIME_DEF;

static char *str_n_cpy(char *dest, const char *src, int n);
static RTC_TIME_DEF GetSoftWareBuildTargetTime(void);
static u8 Bcd2ToByte(u8 Value);
static u8 ByteToBcd2(u8 Value);
u32 GetSoftWareVersion(void);

#endif // SW_VER_BY_COMPILE_TIME
#endif // __VERSION_H__
