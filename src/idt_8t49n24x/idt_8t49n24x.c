/*****************************************************************************/
/**
*
* @file idt_8t49n24x.c
* @addtogroup IDT_8T49N24x
* @{
* <pre>
* Copyright (c) 2016 Xilinx, Inc.
* Copyright (c) 2016 Adeas B.V. All rights reserved.
*
* MODIFICATION HISTORY:
*
* Ver   Who    Date     Changes
* ----- ------ -------- --------------------------------------------------
* X.XX  XX     YY/MM/DD
* 1.00  RHe    16/06/20 Initial release. A ported version from the
*                       8T49N28X-FrequencyProgrammingGuide-
*                       	register-calculations.py script
* 2.00  MG     16/08/15 Major update
* 2.10  MG     16/09/05 Added LOS variable
* 2.20  GM     18/02/08 Converted math.h functions (e.g. ceil) to
* 							standard functions
* </pre>
*
******************************************************************************/


/***************************** Include Files *********************************/
#include "idt_8t49n24x.h"
#include "xiic.h"
#include "xil_types.h"
#include "xil_assert.h"
#include "xstatus.h"
#include <stdlib.h>

/************************** Constant Definitions *****************************/
#define IDT_8T49N24X_ADV_FUNC_EN 1 /* Enable unused APIs */

/***************** Macros (Inline Functions) Definitions *********************/

/**************************** Type Definitions *******************************/

/************************** Function Prototypes ******************************/

/************************** Variable Definitions *****************************/

/************************** Function Definitions *****************************/
static int IDT_8T49N24x_ModifyRegister(u32 I2CBaseAddress, u8 I2CSlaveAddress,
							u16 RegisterAddress, u8 Value, u8 Mask);
static int IDT_8T49N24x_CheckDeviceID(u32 I2CBaseAddress, u8 I2CSlaveAddress);
static int IDT_8T49N24x_InputMonitorControl(u32 I2CBaseAddress,
							u8 I2CSlaveAddress, u32 Value, u8 Input);
static int IDT_8T49N24x_PreDivider(u32 I2CBaseAddress, u8 I2CSlaveAddress,
							u32 Value, u8 Input);
static int IDT_8T49N24x_M1Feedback(u32 I2CBaseAddress, u8 I2CSlaveAddress,
							u32 Value, u8 Input);
static int IDT_8T49N24x_DSMInteger(u32 I2CBaseAddress, u8 I2CSlaveAddress,
							u16 Value);
static int IDT_8T49N24x_DSMFractional(u32 I2CBaseAddress, u8 I2CSlaveAddress,
							u32 Value);
static int IDT_8T49N24x_NS1_Q0(u32 I2CBaseAddress,
							u8 I2CSlaveAddress, u32 Value);
static int IDT_8T49N24x_NS2_Q0(u32 I2CBaseAddress,
							u8 I2CSlaveAddress, u32 Value);
static int IDT_8T49N24x_OutputDividerInteger(u32 I2CBaseAddress,
							u8 I2CSlaveAddress, u32 Value, u8 Output);
static int IDT_8T49N24x_OutputDividerFractional(u32 I2CBaseAddress,
							u8 I2CSlaveAddress, u32 Value, u8 Output);
static int IDT_8T49N24x_Mode(u32 I2CBaseAddress, u8 I2CSlaveAddress,
							u8 Synthesizer);
//static int IDT_8T49N24x_GetIntDivTable(int FOut, int *DivTable, u8 Bypass);
//static int IDT_8T49N24x_CalculateSettings(int FIn, int FOut,
//							IDT_8T49N24x_Settings* RegSettings);
static int IDT_8T49N24x_CalculateSettings(idt24x_config * cfg,
							IDT_8T49N24x_Settings* RegSettings);
static int IDT_8T49N24x_Enable(u32 I2CBaseAddress, u8 I2CSlaveAddress,
							u8 Enable);
static int IDT_8T49N24x_ReferenceInput(u32 I2CBaseAddress, u8 I2CSlaveAddress,
							u8 Input, u8 Enable);
static int IDT_8T49N24x_Configure_JA(u32 I2CBaseAddress, u8 I2CSlaveAddress);
#if (IDT_8T49N24X_ADV_FUNC_EN == 1)
static int IDT_8T49N24x_GPIO(u32 I2CBaseAddress, u8 I2CSlaveAddress);
static int IDT_8T49N24x_OutputDriver(u32 I2CBaseAddress, u8 I2CSlaveAddress,
							u8 Output, u8 Enable);
static int IDT_8T49N24x_LockMode(u32 I2CBaseAddress, u8 I2CSlaveAddress);
static int IDT_8T49N24x_SelectInputReference(u32 I2CBaseAddress,
							u8 I2CSlaveAddress, u8 Input);
static int IDT_8T49N24x_Configure(u32 I2CBaseAddress, u8 I2CSlaveAddress);
#endif
/*****************************************************************************/
/**
*
* This function reads a single byte to the IDT 8T49N24x
*
* @param I2CBaseAddress is the baseaddress of the I2C core.
* @param I2CSlaveAddress is the 7-bit I2C slave address.
*
* @return
*    - XST_SUCCESS Initialization was successful.
*    - XST_FAILURE I2C write error.
*
* @note None.
*
******************************************************************************/
u8 IDT_8T49N24x_GetRegister(u32 I2CBaseAddress, u8 I2CSlaveAddress,
							u16 RegisterAddress)
{
	u32 ByteCount = 0;
	u8 Buffer[2];
	u8 Data;
	u8 Retry = 0;
	u8 Exit;


	Exit = FALSE;
	Data = 0;

	do {
		/* Set Address */
		Buffer[0] = (RegisterAddress >> 8);
		Buffer[1] = RegisterAddress & 0xff;
		ByteCount = XIic_Send(I2CBaseAddress, I2CSlaveAddress, (u8*)Buffer,
							2, XIIC_REPEATED_START);

#ifdef versal
		/* This delay prevents IIC access from hanging */
		usleep(500);
#endif

		if (ByteCount != 2) {
			Retry++;

			/* Maximum retries */
			if (Retry == 255) {
				Exit = TRUE;
			}
		}

		/* Read data */
		else {
			/* Read data */
			ByteCount = XIic_Recv(I2CBaseAddress, I2CSlaveAddress, (u8*)Buffer,
							1, XIIC_STOP);
			if (ByteCount != 1) {
				Exit = FALSE;
			}

			else {
				Data = Buffer[0];
				Exit = TRUE;
			}
		}
	} while (!Exit);

	return Data;
}

/*****************************************************************************/
/**
*
* This function send a single byte to the IDT 8T49N24x
*
* @param I2CBaseAddress is the baseaddress of the I2C core.
* @param I2CSlaveAddress is the 7-bit I2C slave address.
*
* @return
*    - XST_SUCCESS Initialization was successful.
*    - XST_FAILURE I2C write error.
*
* @note None.
*
******************************************************************************/
int IDT_8T49N24x_SetRegister(u32 I2CBaseAddress, u8 I2CSlaveAddress,
							u16 RegisterAddress, u8 Value)
{
	u32 ByteCount = 0;
	u8 Buffer[3];
	u8 Retry = 0;

	/* Write data */
	Buffer[0] = (RegisterAddress >> 8);
	Buffer[1] = RegisterAddress & 0xff;
	Buffer[2] = Value;

	while (1) {
		ByteCount = XIic_Send(I2CBaseAddress, I2CSlaveAddress, (u8*)Buffer,
							3, XIIC_STOP);

		/* This delay prevents IIC access from hanging */
		usleep(500);


		if (ByteCount != 3) {
			Retry++;

			/* Maximum retries */
			if (Retry == 255) {
				return XST_FAILURE;
			}
		}

		else {
			return XST_SUCCESS;
		}
	}
}

/*****************************************************************************/
/**
*
* This function modifies a single byte to the IDT 8T49N24x
*
* @param I2CBaseAddress is the baseaddress of the I2C core.
* @param I2CSlaveAddress is the 7-bit I2C slave address.
*
* @return
*    - XST_SUCCESS Initialization was successful.
*    - XST_FAILURE I2C write error.
*
* @note None.
*
******************************************************************************/
static int IDT_8T49N24x_ModifyRegister(u32 I2CBaseAddress, u8 I2CSlaveAddress,
							u16 RegisterAddress, u8 Value, u8 Mask)
{
	u8 Data;
	int Result;

	/* Read data */
	Data = IDT_8T49N24x_GetRegister(I2CBaseAddress, I2CSlaveAddress,
							RegisterAddress);

	/* Clear masked bits */
	Data &= ~Mask;

	/* Update */
	Data |= (Value & Mask);

	/* Write data */
	Result = IDT_8T49N24x_SetRegister(I2CBaseAddress, I2CSlaveAddress,
							RegisterAddress, Data);

	return Result;
}

/*****************************************************************************/
/**
*
* This function checks the IDT 8T49N24x device ID
*
* @param I2CBaseAddress is the baseaddress of the I2C core.
* @param I2CSlaveAddress is the 7-bit I2C slave address.
*
* @return
*    - XST_SUCCESS Initialization was successful.
*    - XST_FAILURE I2C write error.
*
* @note None.
*
******************************************************************************/
static int IDT_8T49N24x_CheckDeviceID(u32 I2CBaseAddress, u8 I2CSlaveAddress)
{
	u16 DeviceId;
	u8 Data;

	/* Get DEV_ID[15:12] */
	Data = IDT_8T49N24x_GetRegister(I2CBaseAddress, I2CSlaveAddress, 0x0002);

	/* Mask */
	Data &= 0x0f;

	/* Copy */
	DeviceId = Data;

	/* Shift */
	DeviceId <<= 8;

	/* Get DEV_ID[11:4] */
	Data = IDT_8T49N24x_GetRegister(I2CBaseAddress, I2CSlaveAddress, 0x0003);

	/* Copy */
	DeviceId |= Data;

	/* Shift */
	DeviceId <<= 4;

	/* Get DEV_ID[3:0] */
	Data = IDT_8T49N24x_GetRegister(I2CBaseAddress, I2CSlaveAddress, 0x0004);

	/* Copy */
	DeviceId |= (Data >> 4);

	xil_printf("IDT_8T49N241 device ID 0x%x\r\n", DeviceId);

	/* Check */
	if (DeviceId == 0x0606)
		return XST_SUCCESS;

	else
		return XST_FAILURE;
}


const uint8_t q0_ns1_options[3] = { 5, 6, 4 };

static uint32_t idt_div_u64_u32(uint64_t num, uint32_t den)
{
    return (uint32_t)(num / den);
}

static uint32_t idt_div_u64_u32_rem(
    uint64_t num,
    uint32_t den,
    uint64_t *rem)
{
    uint32_t q = (uint32_t)(num / den);
    if (rem)
        *rem = (uint64_t)(num % den);
    return q;
}

static int idt24x_calc_div_q0(idt24x_config * cfg,
				IDT_8T49N24x_Settings* RegSettings)
{
	uint8_t x;
	uint32_t min_div, max_div, best_vco = 0;
	uint16_t min_ns2, max_ns2;
	uint8_t is_lower_vco = FALSE;

	RegSettings->NS1_Q0 = 0;
	RegSettings->NS2_Q0 = 0;

	if ((cfg->output[0].enable == 0) || (cfg->output[0].target_hz == 0))
		return 0;

	// 数学含义是	
	// min_div = floor(VCO_MIN / (requested * 2)) * 2
	// max_div = floor(VCO_MAX / (requested * 2)) * 2
	// 

	min_div = idt_div_u64_u32(
		(uint64_t)IDT_8T49N24X_FVCO_MIN, cfg->output[0].target_hz * 2) * 2;
	max_div = idt_div_u64_u32(
		(uint64_t)IDT_8T49N24X_FVCO_MAX, cfg->output[0].target_hz * 2) * 2;

	xil_printf(
		"%s. requested: %u, min_div: %u, max_div: %u\n",
		__func__, cfg->output[0].target_hz, min_div, max_div);

	min_ns2 = idt_div_u64_u32(
		(uint64_t)min_div, IDT_8T49N24X_NS1_MAX * 2);
	max_ns2 = idt_div_u64_u32(
		(uint64_t)max_div, IDT_8T49N24X_NS1_MIN * 2);


	xil_printf(
		"%s. min_ns2: %u, max_ns2: %u\n", __func__, min_ns2, max_ns2);

	for (x = 0; x < ARRAY_SIZE(q0_ns1_options); x++) {
		uint16_t y = min_ns2;

		while (y <= max_ns2) {
			uint32_t actual_div = q0_ns1_options[x] * y * 2;
			uint32_t current_vco = actual_div * cfg->output[0].target_hz;

			if (current_vco < IDT_8T49N24X_FVCO_MIN)
				xil_printf(
					"%s. ignore div: (ns1=%u * ns2=%u * 2 * %u) == %u < %lu\n",
					__func__, q0_ns1_options[x], y,
					cfg->output[0].target_hz,
					current_vco, IDT_8T49N24X_FVCO_MIN);
			else if (current_vco > IDT_8T49N24X_FVCO_MAX) {
				xil_printf(
					"%s. ignore div: (ns1=%u * ns2=%u * 2 * %u) == %u > %lu. EXIT LOOP.\n",
					__func__, q0_ns1_options[x], y,
					cfg->output[0].target_hz,
					current_vco, IDT_8T49N24X_FVCO_MAX);
				y = max_ns2;
			} else {
				uint8_t use = FALSE;

				xil_printf(
					"%s. contender: (ns1=%u * ns2=%u * 2 * %u) == %u [in range]\n",
					__func__, q0_ns1_options[x], y,
					cfg->output[0].target_hz,
					current_vco);
				if (current_vco <= IDT_8T49N24X_FVCO_OPT) {
					if (current_vco > best_vco ||
					    !is_lower_vco) {
						is_lower_vco = TRUE;
						use = TRUE;
					}
				} else if (!is_lower_vco &&
					   current_vco > best_vco)
					use = TRUE;
				if (use) {
					RegSettings->NS1_Q0 = x;
					RegSettings->NS2_Q0 = y;
					best_vco = current_vco;
				}
			}
			y++;
		}
	}

	xil_printf(
		"%s. best: (ns1=%u [/%u] * ns2=%u * 2 * %u) == %u\n",
		__func__, RegSettings->NS1_Q0, q0_ns1_options[RegSettings->NS1_Q0],
		RegSettings->NS2_Q0, cfg->output[0].target_hz, best_vco);
	return XST_SUCCESS;
}

static int idt24x_calc_divs(idt24x_config * cfg,
				IDT_8T49N24x_Settings* RegSettings)
{
	uint32_t vco = 0;
	int result;

	result = idt24x_calc_div_q0(cfg, RegSettings);
	if (result < 0)
		return result;

	xil_printf(
		"%s: after idt24x_calc_div_q0. ns1: %u [/%u], ns2: %u\n\r",
		__func__, RegSettings->NS1_Q0, q0_ns1_options[RegSettings->NS1_Q0],
		RegSettings->NS2_Q0);

	RegSettings->DSM_INT = 0;
	RegSettings->DSM_FRAC = 0;

	if ((cfg->output[0].target_hz > 0) && (cfg->output[0].enable != 0)) {
		/* Q0 is in use and is governing the actual VCO freq */
		vco = q0_ns1_options[RegSettings->NS1_Q0] * RegSettings->NS2_Q0 *
			2 * cfg->output[0].target_hz;
	} else {
		uint32_t freq = 0;
		uint32_t walk;
		uint32_t min_div, max_div;
		uint8_t is_lower_vco = FALSE;

		/*
		 * Q0 is not in use. Use the first requested (fractional)
		 * output frequency as the one controlling the VCO.
		 */
		for (walk = 1; walk < NUM_OUTPUTS; walk++) {
			if ((cfg->output[walk].target_hz != 0) && (cfg->output[walk].enable != 0)) {
				freq = cfg->output[walk].target_hz;
				break;
			}
		}

		if (freq == 0) {
			xil_printf(
				"%s: NO FREQUENCIES SPECIFIED\n\r", __func__);
			return -1;
		}

		/*
		 * First, determine the min/max div for the output frequency.
		 */
		min_div = IDT_8T49N24X_MIN_INT_DIVIDER;
		// max_div = div64_u64((uint64_t)IDT24x_VCO_MAX, freq * 2) * 2;
		max_div = idt_div_u64_u32((uint64_t)IDT_8T49N24X_FVCO_MAX, freq * 2) * 2;

		xil_printf(
			"%s: calc_divs for fractional output. freq: %u, min_div: %u, max_div: %u\n\r",
			__func__, freq, min_div, max_div);

		walk = min_div;

		while (walk <= max_div) {
			uint32_t current_vco = freq * walk;

			xil_printf(
				"%s: calc_divs for fractional output. walk: %u, freq: %u, vco: %u\n\r",
				__func__, walk, freq, vco);
			if (current_vco >= IDT_8T49N24X_FVCO_MIN &&
			    vco <= IDT_8T49N24X_FVCO_MAX) {
				if (current_vco <= IDT_8T49N24X_FVCO_OPT) {
					if (current_vco > vco ||
					    !is_lower_vco) {
						is_lower_vco = TRUE;
						vco = current_vco;
					}
				} else if (!is_lower_vco && current_vco > vco) {
					vco = current_vco;
				}
			}
			/* Divider must be even. */
			walk += 2;
		}
	}

	if (vco != 0) {
		uint32_t pfd;
		uint64_t rem;
		int x;

		uint32_t nint[3] = {0};
		uint32_t nfrac[3] = {0};

		/* Setup dividers for outputs with fractional dividers. */
		for (x = 1; x < NUM_OUTPUTS; x++) {
			if ((cfg->output[x].target_hz > 0) && (cfg->output[x].enable != 0)) {
				/*
				 * The value written to the chip is half
				 * the calculated divider.
				 */
				nint[x - 1] = idt_div_u64_u32_rem(
					(uint64_t)vco,
					cfg->output[x].target_hz * 2,
					&rem);

				nfrac[x - 1] = idt_div_u64_u32(
					(uint64_t)rem * 1 << 28,
					cfg->output[x].target_hz * 2);

				xil_printf(
					"%s: div to get Q%i freq %u from vco %u: int part: %u, rem: %lu, frac part: %u\n\r",
					__func__, x,
					cfg->output[0].target_hz,
					vco, nint[x - 1], rem,
					nfrac[x - 1]);
			}
		}
		RegSettings->N_Q1 = nint[0];
		RegSettings->NFRAC_Q1 = nfrac[0];
		RegSettings->N_Q2 = nint[1];
		RegSettings->NFRAC_Q2 = nfrac[1];
		RegSettings->N_Q3 = nint[2];
		RegSettings->NFRAC_Q3 = nfrac[2];



		uint8_t doubler_disabled = FALSE;
		/* Calculate freq for pfd */
		pfd = cfg->input.freq_hz * (doubler_disabled ? 1 : 2);

		/*
		 * Calculate dsmint & dsmfrac:
		 * -----------------------------
		 * dsm = float(vco)/float(pfd)
		 * dsmfrac = dsm-floor(dsm) * 2^21
		 * rem = vco % pfd
		 * therefore:
		 * dsmfrac = (rem * 2^21)/pfd
		 */

		RegSettings->DSM_INT = idt_div_u64_u32_rem(vco, pfd, &rem);
		RegSettings->DSM_FRAC = idt_div_u64_u32((uint64_t)rem * 1 << 21, pfd);
		RegSettings->FVCO = vco;

		xil_printf(
			"%s: vco: %u, pfd: %u, dsmint: %u, dsmfrac: %u, rem: %lu\n\r",
			__func__, RegSettings->FVCO, pfd, RegSettings->DSM_INT,
			RegSettings->DSM_FRAC, rem);
	} else {
		xil_printf(
			"%s: no integer divider in range found. NOT SUPPORTED.\n\r",
			__func__);
		return XST_FAILURE;
	}
	return XST_SUCCESS;
}


/*****************************************************************************/
/**
*
* This function calculates the settings
*
* @param I2CBaseAddress is the baseaddress of the I2C core.
* @param I2CSlaveAddress is the 7-bit I2C slave address.
*
* @return
*    - XST_SUCCESS Initialization was successful.
*    - XST_FAILURE I2C write error.
*
* @note None.
*
******************************************************************************/
// static int IDT_8T49N24x_CalculateSettings(int FIn, int FOut,
// 							IDT_8T49N24x_Settings* RegSettings)
static int IDT_8T49N24x_CalculateSettings(idt24x_config * cfg,
							IDT_8T49N24x_Settings* RegSettings)
{
	double Ratio;

	int LOS;


	int Status;

	Status = idt24x_calc_divs(cfg, RegSettings);
	if(Status)
	{
		xil_printf(
			"%s: idt24x_calc_divs() failed.\n\r",
			__func__);
	}


	/*****************************************************/
	/* Calculate the Lower Loop Feedback divider and
	 * Input Divider
	 *****************************************************/

	Ratio = RegSettings->FVCO/cfg->input.freq_hz;

	int M1 = 0;
	int PMin = (int)cfg->input.freq_hz/IDT_8T49N24X_FPD_MAX;

	/* This M1 divider sets the input PFD frequency at 128KHz, the set max */
	/* int M1Min = (int)(FVCO/IDT_8T49N24X_FPD_MAX); */

	int M1_default;
	int P_default;
	int error_tmp = 999999;
	int error = 99999999;

	int count = 0;

	/* Start from lowest divider and iterate until 0 error is found
	 * or the divider limit is exhausted.
	 */
	/* Keep the setting with the lowest error */
	for (int i = PMin; i <= IDT_8T49N24X_P_MAX; i++) {
		/* M1 = (int)round(i*Ratio); */
		M1 = (int)(i*Ratio + 0.5);
		count++;
		if (M1 < IDT_8T49N24X_M_MAX) {
			error_tmp = (int)(Ratio*1e9 - (M1*1e9 / i));

			if (abs(error_tmp) < error || error_tmp == 0) {
				error = abs(error_tmp);
				M1_default = M1;
				P_default = i;

				if (error_tmp == 0)
					break;
			}
		}
		else {
			break;
		}
	}

	/* Calculate LOS */
	LOS = RegSettings->FVCO / 8 / cfg->input.freq_hz;
	LOS = LOS + 3;
	if (LOS < 6)
		LOS = 6;

	/* Copy registers */
	RegSettings->M1_0 = M1_default;
	RegSettings->PRE_0 = P_default;
	RegSettings->LOS_0 = LOS;
	RegSettings->M1_1 = M1_default;
	RegSettings->PRE_1 = P_default;
	RegSettings->LOS_1 = LOS;

	return XST_SUCCESS;
}


void IDT_8T49N24x_DumpSettings(const IDT_8T49N24x_Settings *s)
{
	if (!s) {
		xil_printf("[IDT] Settings: (null)\n\r");
		return;
	}

	xil_printf("=================================================\n\r");
	xil_printf(" IDT 8T49N24x Settings Dump\n\r");
	xil_printf("=================================================\n\r");


	/* Integer Output Divider */
	xil_printf(" Integer Output Divider:\n\r");
	xil_printf("   NS1_Q0   : %u\n\r", s->NS1_Q0);
	xil_printf("   NS2_Q0   : %u\n\r", s->NS2_Q0);

	/* Fractional Output Divider */
	xil_printf(" Fractional Output Divider:\n\r");
	xil_printf("   N_Q1     : %u\n\r", s->N_Q1);
	xil_printf("   NFRAC_Q1 : %u (0x%06X)\n\r",
	       s->NFRAC_Q1, s->NFRAC_Q1 & 0x3FFFFF);

	xil_printf(" Fractional Output Divider:\n\r");
	xil_printf("   N_Q2     : %u\n\r", s->N_Q2);
	xil_printf("   NFRAC_Q2 : %u (0x%06X)\n\r",
	       s->NFRAC_Q2, s->NFRAC_Q2 & 0x3FFFFF);

	xil_printf(" Fractional Output Divider:\n\r");
	xil_printf("   N_Q3     : %u\n\r", s->N_Q3);
	xil_printf("   NFRAC_Q3 : %u (0x%06X)\n\r",
	       s->NFRAC_Q3, s->NFRAC_Q3 & 0x3FFFFF);

	/* Upper Loop Feedback Divider */
	xil_printf(" Upper Loop Feedback Divider (DSM):\n\r");
	xil_printf("   DSM_INT  : %u\n\r", s->DSM_INT);
	xil_printf("   DSM_FRAC : %u (0x%06X / 2^21)\n\r",
	       s->DSM_FRAC, s->DSM_FRAC & 0x1FFFFF);
	xil_printf("   FVCO   : %u\n\r", s->FVCO);

	/* Lower Loop Dividers */
	xil_printf(" Lower Loop Dividers:\n\r");
	xil_printf("   M1_0     : %u\n\r", s->M1_0);
	xil_printf("   PRE_0    : %u\n\r", s->PRE_0);
	xil_printf("   M1_1     : %u\n\r", s->M1_1);
	xil_printf("   PRE_1    : %u\n\r", s->PRE_1);

	/* Input monitor clocks */
	xil_printf(" Input Monitor:\n\r");
	xil_printf("   LOS_0    : %u\n\r", s->LOS_0);
	xil_printf("   LOS_1    : %u\n\r", s->LOS_1);

	xil_printf("=================================================\n\r");
}

/*****************************************************************************/
/**
*
* This function calculates and sets the IDT 8TN49N24x device with the
* given clock configuration.
*
* @param I2CBaseAddress is the baseaddress of the I2C core.
* @param I2CSlaveAddress is the 7-bit I2C slave address.
* @param FIn specifies the input frequency.
* @param FOut specifies the output frequency.
* @param FreeRun specifies if the operation mode is locked/synthesizer mode.
*    - TRUE Synthesizer mode (Fout only)
*    - FALSE Locked mode (Fout locked to Fin)
*
* @return
*    - XST_SUCCESS Initialization was successful.
*    - XST_FAILURE I2C write error or incorrect parameters detected.
*
* @note
*
******************************************************************************/
//int IDT_8T49N24x_SetClock(u32 I2CBaseAddress, u8 I2CSlaveAddress, int FIn,
//							int FOut, u8 FreeRun)
int IDT_8T49N24x_SetClock(u32 I2CBaseAddress, u8 I2CSlaveAddress, 
							idt24x_config * cfg,
							u8 FreeRun)
{
	int Result = XST_SUCCESS;
	int i;
#if 1
	if ((cfg->input.freq_hz < IDT_8T49N24X_FIN_MIN) &&
	   (cfg->input.freq_hz > IDT_8T49N24X_FIN_MAX)) {
		return XST_FAILURE;
	}

	// if ((FOut < IDT_8T49N24X_FOUT_MIN) &&
	// 	(FOut > IDT_8T49N24X_FOUT_MAX)) {
	// 	return XST_FAILURE;
	// }
	for(i=0; i<NUM_OUTPUTS; i++)
	{
		if((cfg->output[i].target_hz < IDT_8T49N24X_FOUT_MIN) && (cfg->output[i].target_hz > IDT_8T49N24X_FOUT_MAX))
		{
			return XST_FAILURE;
		}
	}

	IDT_8T49N24x_Settings RegSettings;
	memset(&RegSettings, 0, sizeof(RegSettings));
	
	/* Calculate settings */
	// IDT_8T49N24x_CalculateSettings(FIn, FOut, &RegSettings);
	IDT_8T49N24x_CalculateSettings(cfg, &RegSettings);

	IDT_8T49N24x_DumpSettings(&RegSettings);

	/* Disable DPLL and APLL calibration */
	Result |= IDT_8T49N24x_Enable(I2CBaseAddress, I2CSlaveAddress, FALSE);

	/* Mode */
	if (FreeRun == TRUE) {
		/* Disable reference clock input 0 */
		Result |= IDT_8T49N24x_ReferenceInput(I2CBaseAddress, I2CSlaveAddress,
							0, FALSE);

		/* Disable reference clock input 1 */
		Result |= IDT_8T49N24x_ReferenceInput(I2CBaseAddress, I2CSlaveAddress,
							1, FALSE);

		/* Set synthesizer mode */
		Result |= IDT_8T49N24x_Mode(I2CBaseAddress, I2CSlaveAddress, TRUE);
	}

	else {
		/* Enable reference clock input 0 */
		Result |= IDT_8T49N24x_ReferenceInput(I2CBaseAddress, I2CSlaveAddress,
							0, TRUE);

		/* Disable reference clock input 1 */
		Result |= IDT_8T49N24x_ReferenceInput(I2CBaseAddress, I2CSlaveAddress,
							1, FALSE);

		/* Set jitter attentuator mode */
		Result |= IDT_8T49N24x_Mode(I2CBaseAddress, I2CSlaveAddress, FALSE);
	}

	/* Pre-divider Input 0 */
	Result |= IDT_8T49N24x_PreDivider(I2CBaseAddress, I2CSlaveAddress,
							RegSettings.PRE_0, 0);

	/* Pre-divider Input 1 */
	Result |= IDT_8T49N24x_PreDivider(I2CBaseAddress, I2CSlaveAddress,
							RegSettings.PRE_1, 1);

	/* M1 feedback Input 0 */
	Result |= IDT_8T49N24x_M1Feedback(I2CBaseAddress, I2CSlaveAddress,
							RegSettings.M1_0, 0);

	/* M1 feedback Input 1 */
	Result |= IDT_8T49N24x_M1Feedback(I2CBaseAddress, I2CSlaveAddress,
							RegSettings.M1_1, 1);

	/* DSM integer */
	Result |= IDT_8T49N24x_DSMInteger(I2CBaseAddress, I2CSlaveAddress,
							RegSettings.DSM_INT);

	/* DSM fractional */
	Result |= IDT_8T49N24x_DSMFractional(I2CBaseAddress, I2CSlaveAddress,
							RegSettings.DSM_FRAC);


	Result |= IDT_8T49N24x_OutputDriver(I2CBaseAddress, I2CSlaveAddress,
							0, 0);
	Result |= IDT_8T49N24x_OutputDriver(I2CBaseAddress, I2CSlaveAddress,
							1, 0);
	Result |= IDT_8T49N24x_OutputDriver(I2CBaseAddress, I2CSlaveAddress,
							2, 0);
	Result |= IDT_8T49N24x_OutputDriver(I2CBaseAddress, I2CSlaveAddress,
							3, 0);

	if(cfg->output[0].enable)
	{
		/* Output divider integer output 0 */
		Result |= IDT_8T49N24x_NS1_Q0(I2CBaseAddress, I2CSlaveAddress,
								RegSettings.NS1_Q0);

		/* Output divider fractional output 0 */
		Result |= IDT_8T49N24x_NS2_Q0(I2CBaseAddress, I2CSlaveAddress, 
								RegSettings.NS2_Q0);
	}
	
	if (cfg->output[1].enable)
	{
		/* Output divider integer output 1 */
		Result |= IDT_8T49N24x_OutputDividerInteger(I2CBaseAddress, I2CSlaveAddress,
								RegSettings.N_Q1, 1);

		/* Output divider fractional output 1 */
		Result |= IDT_8T49N24x_OutputDividerFractional(I2CBaseAddress,
								I2CSlaveAddress, RegSettings.NFRAC_Q1, 1);
	}
	
	if (cfg->output[2].enable)
	{
		/* Output divider integer output 2 */
		Result |= IDT_8T49N24x_OutputDividerInteger(I2CBaseAddress, I2CSlaveAddress,
								RegSettings.N_Q2, 2);

		/* Output divider fractional output 2 */
		Result |= IDT_8T49N24x_OutputDividerFractional(I2CBaseAddress,
								I2CSlaveAddress, RegSettings.NFRAC_Q2, 2);
	}
	
	if (cfg->output[3].enable)
	{
		/* Output divider integer output 3 */
		Result |= IDT_8T49N24x_OutputDividerInteger(I2CBaseAddress, I2CSlaveAddress,
								RegSettings.N_Q3, 3);

		/* Output divider fractional output 3 */
		Result |= IDT_8T49N24x_OutputDividerFractional(I2CBaseAddress,
								I2CSlaveAddress, RegSettings.NFRAC_Q3, 3);
	}
	
	/* Input monitor control 0 */
	Result |= IDT_8T49N24x_InputMonitorControl(I2CBaseAddress, I2CSlaveAddress,
							RegSettings.LOS_0, 0);

	/* Input monitor control 1 */
	Result |= IDT_8T49N24x_InputMonitorControl(I2CBaseAddress, I2CSlaveAddress,
							RegSettings.LOS_1, 1);
#endif
	/* Enable DPLL and APLL calibration */
	Result |= IDT_8T49N24x_Enable(I2CBaseAddress, I2CSlaveAddress, TRUE);

	return Result;
}

/*****************************************************************************/
/**
*
* This function set the input monitor control
*
* @param I2CBaseAddress is the baseaddress of the I2C core.
* @param I2CSlaveAddress is the 7-bit I2C slave address.
*
* @return
*    - XST_SUCCESS
*    - XST_FAILURE
*
* @note None.
*
******************************************************************************/
static int IDT_8T49N24x_InputMonitorControl(u32 I2CBaseAddress,
							u8 I2CSlaveAddress, u32 Value, u8 Input)
{
	int Result = XST_SUCCESS;
	u8 Data;
	u16 Address;

	if (Input == 1)
		Address = 0x0074;
	else
		Address = 0x0071;

	/* LOSx[16] */
	Data = (Value >> 16) & 0x1;
	Result |= IDT_8T49N24x_SetRegister(I2CBaseAddress, I2CSlaveAddress,
							Address, Data);

	/* LOSx[15:8] */
	Data = (Value >> 8);
	Result |= IDT_8T49N24x_SetRegister(I2CBaseAddress, I2CSlaveAddress,
							Address+1, Data);

	/* LOSx[7:0] */
	Data = (Value & 0xff);
	Result |= IDT_8T49N24x_SetRegister(I2CBaseAddress, I2CSlaveAddress,
							Address+2, Data);

	return Result;
}

/*****************************************************************************/
/**
*
* This function set the pre-divider
*
* @param I2CBaseAddress is the baseaddress of the I2C core.
* @param I2CSlaveAddress is the 7-bit I2C slave address.
*
* @return
*    - XST_SUCCESS
*    - XST_FAILURE
*
* @note None.
*
******************************************************************************/
static int IDT_8T49N24x_PreDivider(u32 I2CBaseAddress, u8 I2CSlaveAddress,
							u32 Value, u8 Input)
{
	int Result = XST_SUCCESS;
	u8 Data;
	u16 Address;

	if (Input == 1)
		Address = 0x000e;
	else
		Address = 0x000b;

	/* PREx[20:16] */
	Data = (Value >> 16) & 0x1f;
	Result |= IDT_8T49N24x_SetRegister(I2CBaseAddress, I2CSlaveAddress,
							Address, Data);

	/* PREx[15:8] */
	Data = (Value >> 8);
	Result |= IDT_8T49N24x_SetRegister(I2CBaseAddress, I2CSlaveAddress,
							Address+1, Data);

	/* PREx[7:0] */
	Data = (Value & 0xff);
	Result |= IDT_8T49N24x_SetRegister(I2CBaseAddress, I2CSlaveAddress,
							Address+2, Data);

	return Result;
}

/*****************************************************************************/
/**
*
* This function set the M1 feedback
*
* @param I2CBaseAddress is the baseaddress of the I2C core.
* @param I2CSlaveAddress is the 7-bit I2C slave address.
*
* @return
*    - XST_SUCCESS
*    - XST_FAILURE
*
* @note None.
*
******************************************************************************/
static int IDT_8T49N24x_M1Feedback(u32 I2CBaseAddress, u8 I2CSlaveAddress,
							u32 Value, u8 Input)
{
	int Result = XST_SUCCESS;
	u8 Data;
	u16 Address;

	if (Input == 1)
		Address = 0x0011;
	else
		Address = 0x0014;

	/* M1x[23:16] */
	Data = (Value >> 16);
	Result |= IDT_8T49N24x_SetRegister(I2CBaseAddress, I2CSlaveAddress,
							Address, Data);

	/* M1x[15:8] */
	Data = (Value >> 8);
	Result |= IDT_8T49N24x_SetRegister(I2CBaseAddress, I2CSlaveAddress,
							Address+1, Data);

	/* M1x[7:0] */
	Data = (Value & 0xff);
	Result |= IDT_8T49N24x_SetRegister(I2CBaseAddress, I2CSlaveAddress,
							Address+2, Data);

	return Result;
}

/*****************************************************************************/
/**
*
* This function set the DSM integer
*
* @param I2CBaseAddress is the baseaddress of the I2C core.
* @param I2CSlaveAddress is the 7-bit I2C slave address.
*
* @return
*    - XST_SUCCESS
*    - XST_FAILURE
*
* @note None.
*
******************************************************************************/
static int IDT_8T49N24x_DSMInteger(u32 I2CBaseAddress, u8 I2CSlaveAddress,
							u16 Value)
{
	int Result = XST_SUCCESS;
	u8 Data;

	/* DSM_INT[8] */
	Data = (Value >> 8) & 0x01;
	Result |= IDT_8T49N24x_SetRegister(I2CBaseAddress, I2CSlaveAddress,
							0x0025, Data);

	/* DSM_INT[7:0] */
	Data = (Value & 0xff);
	Result |= IDT_8T49N24x_SetRegister(I2CBaseAddress, I2CSlaveAddress,
							0x0026, Data);

	return Result;
}

/*****************************************************************************/
/**
*
* This function set the DSM fractional
*
* @param I2CBaseAddress is the baseaddress of the I2C core.
* @param I2CSlaveAddress is the 7-bit I2C slave address.
*
* @return
*    - XST_SUCCESS
*    - XST_FAILURE
*
* @note None.
*
******************************************************************************/
static int IDT_8T49N24x_DSMFractional(u32 I2CBaseAddress, u8 I2CSlaveAddress,
							u32 Value)
{
	int Result = XST_SUCCESS;
	u8 Data;

	/* DSM_FRAC[20:16] */
	Data = (Value >> 16) & 0x1f;
	Result |= IDT_8T49N24x_SetRegister(I2CBaseAddress, I2CSlaveAddress,
							0x0028, Data);

	/* DSM_FRAC[15:8] */
	Data = (Value >> 8);
	Result |= IDT_8T49N24x_SetRegister(I2CBaseAddress, I2CSlaveAddress,
							0x0029, Data);

	/* DSM_FRAC[7:0] */
	Data = (Value & 0xff);
	Result |= IDT_8T49N24x_SetRegister(I2CBaseAddress, I2CSlaveAddress,
							0x002a, Data);

	return Result;
}

/*****************************************************************************/
/**
*
* This function set the Output divider integer
*
* @param I2CBaseAddress is the baseaddress of the I2C core.
* @param I2CSlaveAddress is the 7-bit I2C slave address.
*
* @return
*    - XST_SUCCESS
*    - XST_FAILURE
*
* @note None.
*
******************************************************************************/
static int IDT_8T49N24x_OutputDividerInteger(u32 I2CBaseAddress,
							u8 I2CSlaveAddress, u32 Value, u8 Output)
{
	int Result = XST_SUCCESS;
	u8 Data;
	u16 Address;

	switch (Output) {
		case 0 : Address = 0x003f;
			break;
		case 1 : Address = 0x0042;
			break;
		case 2 : Address = 0x0045;
			break;
		case 3 : Address = 0x0048;
			break;
	}

	/* N_Qm[17:16] */
	Data = (Value >> 16) & 0x03;
	Result |= IDT_8T49N24x_SetRegister(I2CBaseAddress, I2CSlaveAddress,
							Address, Data);

	/* N_Qm[15:8] */
	Data = (Value >> 8);
	Result |= IDT_8T49N24x_SetRegister(I2CBaseAddress, I2CSlaveAddress,
							Address+1, Data);

	/* N_Qm[7:0] */
	Data = (Value & 0xff);
	Result |= IDT_8T49N24x_SetRegister(I2CBaseAddress, I2CSlaveAddress,
							Address+2, Data);

	return Result;
}

static int IDT_8T49N24x_NS1_Q0(u32 I2CBaseAddress,
							u8 I2CSlaveAddress, u32 Value)
{
	int Result = XST_SUCCESS;
	u8 Data;
	u16 Address = 0x003f;;

	/* NS1_Q0[1:0] */
	Data = (Value & 0x03);
	Result |= IDT_8T49N24x_SetRegister(I2CBaseAddress, I2CSlaveAddress,
							Address, Data);

	return Result;
}

static int IDT_8T49N24x_NS2_Q0(u32 I2CBaseAddress,
							u8 I2CSlaveAddress, u32 Value)
{
	int Result = XST_SUCCESS;
	u8 Data;
	u16 Address = 0x0040;


	/* NS2_Q0[15:8] */
	Data = (Value >> 8);
	Result |= IDT_8T49N24x_SetRegister(I2CBaseAddress, I2CSlaveAddress,
							Address, Data);

	/* NS2_Q0[7:0] */
	Data = (Value & 0xff);
	Result |= IDT_8T49N24x_SetRegister(I2CBaseAddress, I2CSlaveAddress,
							Address+1, Data);
							
//	Data = IDT_8T49N24x_GetRegister(I2CBaseAddress, I2CSlaveAddress, 0x0039);
//	Data |= 0x01;
//	Result |= IDT_8T49N24x_SetRegister(I2CBaseAddress, I2CSlaveAddress,
//							0x0039, Data);
	Result |= IDT_8T49N24x_OutputDriver(I2CBaseAddress, I2CSlaveAddress,
							0, 1);

	return Result;
}

/*****************************************************************************/
/**
*
* This function set the Output divider fractional
*
* @param I2CBaseAddress is the baseaddress of the I2C core.
* @param I2CSlaveAddress is the 7-bit I2C slave address.
*
* @return
*    - XST_SUCCESS
*    - XST_FAILURE
*
* @note None.
*
******************************************************************************/
static int IDT_8T49N24x_OutputDividerFractional(u32 I2CBaseAddress,
							u8 I2CSlaveAddress, u32 Value, u8 Output)
{
	int Result = XST_SUCCESS;
	u8 Data;
	u16 Address;

	switch (Output) {
		case 0 : Address = 0x0000;
			break;
		case 1 : Address = 0x0057;
			break;
		case 2 : Address = 0x005b;
			break;
		case 3 : Address = 0x005f;
			break;
	}

	/* NFRAC_Qm[27:24] */
	Data = (Value >> 24) & 0x0f;
	Result |= IDT_8T49N24x_SetRegister(I2CBaseAddress, I2CSlaveAddress,
							Address, Data);

	/* NFRAC_Qm[23:16] */
	Data = (Value >> 16);
	Result |= IDT_8T49N24x_SetRegister(I2CBaseAddress, I2CSlaveAddress,
							Address+1, Data);

	/* NFRAC_Qm[15:8] */
	Data = (Value >> 8);
	Result |= IDT_8T49N24x_SetRegister(I2CBaseAddress, I2CSlaveAddress,
							Address+2, Data);

	/* NFRAC_Qm[7:0] */
	Data = (Value & 0xff);
	Result |= IDT_8T49N24x_SetRegister(I2CBaseAddress, I2CSlaveAddress,
							Address+3, Data);

//	Data = IDT_8T49N24x_GetRegister(I2CBaseAddress, I2CSlaveAddress, 0x0039);
//	switch (Output) {
//		case 1 : 
//			Data |= 0x02;
//			break;
//		case 2 : 
//			Data |= 0x04;
//			break;
//		case 3 : 
//			Data |= 0x08;
//			break;
//	}
//	Result |= IDT_8T49N24x_SetRegister(I2CBaseAddress, I2CSlaveAddress,
//							0x0039, Data);

	Result |= IDT_8T49N24x_OutputDriver(I2CBaseAddress, I2CSlaveAddress,
							Output, 1);
	return Result;
}

/*****************************************************************************/
/**
*
* This function sets the mode (jitter attenuator / synthesizer)
*
* @param I2CBaseAddress is the baseaddress of the I2C core.
* @param I2CSlaveAddress is the 7-bit I2C slave address.
*
* @return
*    - XST_SUCCESS
*    - XST_FAILURE
*
* @note None.
*
******************************************************************************/
static int IDT_8T49N24x_Mode(u32 I2CBaseAddress, u8 I2CSlaveAddress,
							u8 Synthesizer)
{
	int Result = XST_SUCCESS;
	u8 Value;
	u8 Mask;

	/* Digital PLL */
	/* State[1:0] */
	if (Synthesizer) {
		Value = 0x01;		/* Force FREERUN */
		Value |= (1<<4);	/* Disable reference input 0 */
		Value |= (1<<5);	/* Disable reference input 1 */
	}

	else {
		Value = 0x00;		/* Run automatically */
		Value |= (1<<5);	/* Disable reference input 1 */
	}
	Mask = 0x33;
	Result |= IDT_8T49N24x_ModifyRegister(I2CBaseAddress, I2CSlaveAddress,
							0x000a, Value, Mask);

	/* Analog PLL */
	/* SYN_MODE */
	if (Synthesizer) {
		Value = (1<<3);		/* Synthesizer mode */
	}

	else {
		Value = 0x00;		/* Jitter attenuator mode */
	}
	Mask = (1<<3);
	Result |= IDT_8T49N24x_ModifyRegister(I2CBaseAddress, I2CSlaveAddress,
							0x0069, Value, Mask);

	return Result;
}
/*****************************************************************************/
/**
*
* This function sets disables / enables the reference inputs
*
* @param I2CBaseAddress is the baseaddress of the I2C core.
* @param I2CSlaveAddress is the 7-bit I2C slave address.
*
* @return
*    - XST_SUCCESS
*    - XST_FAILURE
*
* @note None.
*
******************************************************************************/
static int IDT_8T49N24x_ReferenceInput(u32 I2CBaseAddress, u8 I2CSlaveAddress,
							u8 Input, u8 Enable)
{
	int Result = XST_SUCCESS;
	u8 Value;
	u8 Mask;
	u8 Shift;

	if (Input == 1) {
		Shift = 5;
	}

	else {
		Shift = 4;
	}

	/* Enable */
	if (Enable) {
		Value = 0x00;		/* Enable */
	}

	/* Disable */
	else {
		Value = (1<<Shift);	/* Disable reference input  */
	}
	Mask = (1<<Shift);
	Result |= IDT_8T49N24x_ModifyRegister(I2CBaseAddress, I2CSlaveAddress,
							0x000a, Value, Mask);

	return Result;
}

/*****************************************************************************/
/**
*
* This function enables/disables the DPLL and APLL calibration
*
* @param I2CBaseAddress is the baseaddress of the I2C core.
* @param I2CSlaveAddress is the 7-bit I2C slave address.
*
* @return
*    - XST_SUCCESS
*    - XST_FAILURE
*
* @note None.
*
******************************************************************************/
static int IDT_8T49N24x_Enable(u32 I2CBaseAddress, u8 I2CSlaveAddress,
							u8 Enable)
{
	int Result = XST_SUCCESS;
	u8 Value;
	u8 Mask;

	if (Enable) {
		/* Digital PLL enabled - Calibration logic for Analog PLL enabled */
		Value = 0x00;
	}

	else {
		/* Digital PLL disabled - Calibration logic for Analog PLL disabled */
		Value = 0x05;
	}
	Mask = 0x05;
	Result |= IDT_8T49N24x_ModifyRegister(I2CBaseAddress, I2CSlaveAddress,
							0x0070, Value, Mask);

	return Result;
}

/*****************************************************************************/
/**
*
* This function initializes the IDT 8TN49N24x with default values
* for use with the Video FMC.
*
* @param I2CBaseAddress is the baseaddress of the I2C core.
* @param I2CSlaveAddress is the 7-bit I2C slave address.
*
* @return
*    - XST_SUCCESS Initialization was successful.
*    - XST_FAILURE I2C write error.
*
* @note None.
*
******************************************************************************/
int IDT_8T49N24x_Init(u32 I2CBaseAddress, u8 I2CSlaveAddress)
{
	int Result;

	/* Check device ID */
	Result = IDT_8T49N24x_CheckDeviceID(I2CBaseAddress, I2CSlaveAddress);

	if (Result == XST_SUCCESS) {

		/* Disable DPLL and APLL calibration
		 * The i2c interface is clocked by the APLL.
		 * During the PLL parameters update, the i2c might become unresponsive.
		 * To prevent this, the DPLL and APLL calibration are disabled during
		 * the i2c transactions
		 */
		Result |= IDT_8T49N24x_Enable(I2CBaseAddress, I2CSlaveAddress, FALSE);

		/* Configure device */
		Result |= IDT_8T49N24x_Configure_JA(I2CBaseAddress, I2CSlaveAddress);

		/* Enable DPLL and APLL calibration */
		Result |= IDT_8T49N24x_Enable(I2CBaseAddress, I2CSlaveAddress, TRUE);

		return Result;
	}

	return XST_FAILURE;
}

static int IDT_8T49N24x_Configure_JA(u32 I2CBaseAddress, u8 I2CSlaveAddress)
{
	int Result;
	u32 Index;

	/* The configuration is started from address 0x08 */
	for (Index=8; Index<sizeof(IDT_8T49N24x_Config_JA); Index++) {
		/* Skip address 0x70 */
		/* Address 0x70 enables the DPLL and APLL calibration */
		if (Index != 0x070) {
			Result = IDT_8T49N24x_SetRegister(I2CBaseAddress, I2CSlaveAddress,
							Index, IDT_8T49N24x_Config_JA[Index]);
		}
	}
	return Result;
}

/*****************************************************************************/
/**
*
* This function displays a registerdump of the IDT 8TN49N24x device.
*
* @param I2CBaseAddress is the baseaddress of the I2C core.
* @param I2CSlaveAddress is the 7-bit I2C slave address.
*
* @return None
*
* @note None.
*
******************************************************************************/
void IDT_8T49N24x_RegisterDump(u32 I2CBaseAddress, u8 I2CSlaveAddress)
{
	u8 Data;
	u32 i;
	int Result;

	xil_printf("Register dump\r\n");

	Result = IDT_8T49N24x_CheckDeviceID(I2CBaseAddress, I2CSlaveAddress);

	if (Result == XST_SUCCESS) {
		print("\r\n");
		print("---------------------\r\n");
		print("- IDT8T49N241 I2C dump:\r\n");
		print("---------------------\r\n");

		xil_printf("     ");
		for (i=0; i<16; i++)
			xil_printf("+%01x ", i);

		xil_printf("\r\n     ");
		for (i=0; i<16; i++)
			xil_printf("---");

		for (i = 0; i < 256; i++) {
			if ((i % 16) == 0) {
				xil_printf("\r\n%02x : ", i);
			}
			Data = IDT_8T49N24x_GetRegister(I2CBaseAddress,
							I2CSlaveAddress, i);
			xil_printf("%02x ", Data);
		}

		xil_printf("\r\n");
	}

	else {
		xil_printf("IDT 8T49N241 not found!\r\n");
	}
}

#if (IDT_8T49N24X_ADV_FUNC_EN == 1)
/*****************************************************************************/
/**
*
* This function set the GPIO outputs
*
* @param I2CBaseAddress is the baseaddress of the I2C core.
* @param I2CSlaveAddress is the 7-bit I2C slave address.
*
* @return
*    - XST_SUCCESS
*    - XST_FAILURE
*
* @note None.
*
******************************************************************************/
static int IDT_8T49N24x_GPIO(u32 I2CBaseAddress, u8 I2CSlaveAddress)
{
	int Result;

	/* GPIO_DIR */
	/* GPIO3 is output */
	/* Rest of the pins are input */
	Result = IDT_8T49N24x_SetRegister(I2CBaseAddress, I2CSlaveAddress,
							0x0030, 0x08);

	/* GPIxSEL[2] */
	Result |= IDT_8T49N24x_SetRegister(I2CBaseAddress, I2CSlaveAddress,
							0x0031, 0x00);

	/* GPIxSEL[1] */
	Result |= IDT_8T49N24x_SetRegister(I2CBaseAddress, I2CSlaveAddress,
							0x0032, 0x00);

	/* GPIxSEL[0] */
	Result |= IDT_8T49N24x_SetRegister(I2CBaseAddress, I2CSlaveAddress,
							0x0033, 0x00);

	/* GPOxSEL[2] */
	Result |= IDT_8T49N24x_SetRegister(I2CBaseAddress, I2CSlaveAddress,
							0x0034, 0x00);

	/* GPOxSEL[1] */
	Result |= IDT_8T49N24x_SetRegister(I2CBaseAddress, I2CSlaveAddress,
							0x0035, 0x00);

	/* GPOxSEL[0] */
	Result |= IDT_8T49N24x_SetRegister(I2CBaseAddress, I2CSlaveAddress,
							0x0036, 0x08);

	if (Result != XST_SUCCESS)
		return XST_FAILURE;
	else
		return XST_SUCCESS;
}

/*****************************************************************************/
/**
*
* This function set the input reference
*
* @param I2CBaseAddress is the baseaddress of the I2C core.
* @param I2CSlaveAddress is the 7-bit I2C slave address.
*
* @return
*    - XST_SUCCESS
*    - XST_FAILURE
*
* @note None.
*
******************************************************************************/
static int IDT_8T49N24x_SelectInputReference(u32 I2CBaseAddress,
							u8 I2CSlaveAddress, u8 Input)
{
	int Result;
	u8 Value;
	u8 Mask;
	u8 Shift;

	Shift = 5;

	/* Clock 1 */
	if (Input == 1) {
		Value = (0x05 << Shift);
	}

	/* Clock 0 */
	else {
		Value = (0x04 << Shift);
	}
	Mask = 0x07 << Shift;

	Result = IDT_8T49N24x_ModifyRegister(I2CBaseAddress, I2CSlaveAddress,
							0x0008, Value, Mask);

	return Result;
}

/*****************************************************************************/
/**
*
* This function set the output drivers
*
* @param I2CBaseAddress is the baseaddress of the I2C core.
* @param I2CSlaveAddress is the 7-bit I2C slave address.
*
* @return
*    - XST_SUCCESS
*    - XST_FAILURE
*
* @note None.
*
******************************************************************************/
static int IDT_8T49N24x_OutputDriver(u32 I2CBaseAddress, u8 I2CSlaveAddress,
							u8 Output, u8 Enable)
{
	int Result;
	u8 Value;
	u8 Mask;
	u16 Address;
	u8 Shift;

	/* OUTEN */
	Mask = (1 << Output);
	if (Enable)
		Value = Mask;
	else
		Value = 0x00;

	Result = IDT_8T49N24x_ModifyRegister(I2CBaseAddress, I2CSlaveAddress,
							0x0039, Value, Mask);

	/* OUTMODE */
	switch (Output) {
		case 0 :
			Address = 0x003e;
			Shift = 1;
			break;

		case 1 :
			Address = 0x003e;
			Shift = 5;
			break;

		case 2 :
			Address = 0x003d;
			Shift = 1;
			break;

		case 3 :
			Address = 0x003d;
			Shift = 5;
			break;
	}

	if (Enable)
		Value = 0x02;	/* LVDS */
	else
		Value = 0x00;	/* High-impedance */

	Value <<= Shift;
	Mask = (0x07 << Shift);

	Result |= IDT_8T49N24x_ModifyRegister(I2CBaseAddress, I2CSlaveAddress,
							Address, Value, Mask);

	/* Qm_DIS */
	if (!Enable)
		Value = (1 << Output);
	else
		Value = 0;

	Mask = (1 << Output);

	Result |= IDT_8T49N24x_ModifyRegister(I2CBaseAddress, I2CSlaveAddress,
							0x006f, Value, Mask);

	if (Result != XST_SUCCESS)
		return XST_FAILURE;
	else
		return XST_SUCCESS;
}

/*****************************************************************************/
/**
*
* This function sets the lock mode
*
* @param I2CBaseAddress is the baseaddress of the I2C core.
* @param I2CSlaveAddress is the 7-bit I2C slave address.
*
* @return
*    - XST_SUCCESS
*    - XST_FAILURE
*
* @note None.
*
******************************************************************************/
static int IDT_8T49N24x_LockMode(u32 I2CBaseAddress, u8 I2CSlaveAddress)
{
	int Result;
	u8 Value;
	u8 Mask;

	Value = 0x02;
	Mask = 0x02;
	Result = IDT_8T49N24x_ModifyRegister(I2CBaseAddress, I2CSlaveAddress,
							0x006c, Value, Mask);

	return Result;
}

int IDT_8T49N24x_SetGPOut(u32 I2CBaseAddress, u8 I2CSlaveAddress, u8 PortID,
							u8 Set)
{
	if (PortID > 3) {
		print("Invalid port ID\r\n");
		return XST_FAILURE;
	}

	u32 ByteCount = 0;
	u8 Data = 0;
	u8 Buffer[3];

	Buffer[0] = 0x00; /* MSB RegAddr */
	Buffer[1] = 0x38; /* LSB RegAddr */
	ByteCount = XIic_Send(I2CBaseAddress, I2CSlaveAddress, (u8*)Buffer, 2,
							XIIC_REPEATED_START);
	if (ByteCount != 2) {
		return XST_FAILURE;
	}
	ByteCount = XIic_Recv(I2CBaseAddress, I2CSlaveAddress, (u8*)Buffer, 1,
							XIIC_STOP);
	if (ByteCount != 1) {
		return XST_FAILURE;
	}
	Data = Buffer[0];

	if (Set == TRUE) {
		Data |= (1<<PortID);
	}
	else {
		Data &= ~(1<<PortID);
	}

	Buffer[0] = 0x00; /* MSB RegAddr */
	Buffer[1] = 0x38; /* LSB RegAddr */
	Buffer[2] = Data;
	ByteCount = XIic_Send(I2CBaseAddress, I2CSlaveAddress, (u8*)Buffer, 3,
							XIIC_STOP);
	if (ByteCount != 3) {
		return XST_FAILURE;
	}

	return XST_SUCCESS;
}

/*****************************************************************************/
/**
*
* This function the IDT 8TN49N24x device with the data from the
* configuration table.
*
* @param I2CBaseAddress is the baseaddress of the I2C core.
* @param I2CSlaveAddress is the 7-bit I2C slave address.
*
* @return None
*
* @note None.
*
******************************************************************************/
static int IDT_8T49N24x_Configure(u32 I2CBaseAddress, u8 I2CSlaveAddress)
{
	int Result;
	u32 Index;
	xil_printf("freerun\r\n");

	/* The configuration is started from address 0x08 */
	for (Index=8; Index<sizeof(IDT_8T49N24x_Config_Syn); Index++) {
		/* Skip address 0x70 */
		/* Address 0x70 enables the DPLL and APLL calibration */
		if (Index != 0x070) {
			Result = IDT_8T49N24x_SetRegister(I2CBaseAddress, I2CSlaveAddress,
							Index, IDT_8T49N24x_Config_Syn[Index]);
		}
	}
	return Result;
}
#endif

/** @} */



/*

	struct idt24x_config cfg = {
        .input = {
            .type    = IDT24X_IN_XTAL,
            .freq_hz = 40000000,   // 40 MHz XTAL
        },
        .output = {
            { .enable = TRUE,	.target_hz = 270000000 }, // Q0
            { .enable = FALSE,	.target_hz = 148500000 }, // Q1
            { .enable = TRUE,	.target_hz = 148500000 }, // Q2
            { .enable = TRUE,	.target_hz = 300000000 }, // Q3
        },
    };
    
    IDT_8T49N24x_SetClock(.., &cfg, ..)

*/

















