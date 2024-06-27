#include "../bsp.h"
#if defined (XPAR_XV_GAMMA_LUT_NUM_INSTANCES)

#include "xilinx-gamma-coeff_8b+10b.h"
//#include "xilinx-gamma-coeff_10b.h"
//#include "../config.h"

#if 0
int gamma_lut_init(u16 deviceid,u32 hsize,u32 vsize) {
	XV_gamma_lut gamma_lut;
	XV_gamma_lut_Config *gamma_lut_config;

	if ( (gamma_lut_config = XV_gamma_lut_LookupConfig(deviceid)) == NULL) {
		bsp_printf("XV_gamma_lut_LookupConfig() failed\r\n");
		return XST_FAILURE;
	}
	if (XV_gamma_lut_CfgInitialize(&gamma_lut, gamma_lut_config, gamma_lut_config->BaseAddress)) {
		bsp_printf("XV_gamma_lut_CfgInitialize() failed\r\n");
		return XST_FAILURE;
	}

	XV_gamma_lut_Set_HwReg_width(&gamma_lut, hsize);
	XV_gamma_lut_Set_HwReg_height(&gamma_lut, vsize);
    XV_gamma_lut_Set_HwReg_video_format(&gamma_lut, 0); // RGB

	if (XV_gamma_lut_Write_HwReg_gamma_lut_0_Words(&gamma_lut, 0, (int *) xgamma8_06,
			sizeof(xgamma8_10)/sizeof(int)) != sizeof(xgamma8_10)/sizeof(int)) {
		bsp_printf("Gamma correction LUT write failed\r\n");
		return XST_FAILURE;
	}
	if (XV_gamma_lut_Write_HwReg_gamma_lut_1_Words(&gamma_lut, 0, (int *) xgamma8_08,
			sizeof(xgamma8_10)/sizeof(int)) != sizeof(xgamma8_10)/sizeof(int)) {
		bsp_printf("Gamma correction LUT write failed\r\n");
		return XST_FAILURE;
	}
	if (XV_gamma_lut_Write_HwReg_gamma_lut_2_Words(&gamma_lut, 0, (int *) xgamma8_06,
			sizeof(xgamma8_10)/sizeof(int)) != sizeof(xgamma8_10)/sizeof(int)) {
		bsp_printf("Gamma correction LUT write failed\r\n");
		return XST_FAILURE;
	}

	XV_gamma_lut_EnableAutoRestart(&gamma_lut);
	XV_gamma_lut_Start(&gamma_lut);

	bsp_printf("Gamma correction LUT initialized\r\n");

	return XST_SUCCESS;
}
#endif


int gamma_lut_init(u16 deviceid,u32 hsize,u32 vsize,u8 format) {
	XV_gamma_lut gamma_lut;
	XV_gamma_lut_Config *gamma_lut_config;

	if ( (gamma_lut_config = XV_gamma_lut_LookupConfig(deviceid)) == NULL) {
		bsp_printf("XV_gamma_lut_LookupConfig() failed\r\n");
		return XST_FAILURE;
	}
	if (XV_gamma_lut_CfgInitialize(&gamma_lut, gamma_lut_config, gamma_lut_config->BaseAddress)) {
		bsp_printf("XV_gamma_lut_CfgInitialize() failed\r\n");
		return XST_FAILURE;
	}

	XV_gamma_lut_Set_HwReg_width(&gamma_lut, hsize);
	XV_gamma_lut_Set_HwReg_height(&gamma_lut, vsize);
    XV_gamma_lut_Set_HwReg_video_format(&gamma_lut, 0); // RGB

	if (XV_gamma_lut_Write_HwReg_gamma_lut_0_Words(&gamma_lut, 0, (int *) xgamma8_curves[format],
			sizeof(xgamma8_10)/sizeof(int)) != sizeof(xgamma8_10)/sizeof(int)) {
		bsp_printf("Gamma correction LUT write failed\r\n");
		return XST_FAILURE;
	}
	if (XV_gamma_lut_Write_HwReg_gamma_lut_1_Words(&gamma_lut, 0, (int *) xgamma8_curves[format],
			sizeof(xgamma8_10)/sizeof(int)) != sizeof(xgamma8_10)/sizeof(int)) {
		bsp_printf("Gamma correction LUT write failed\r\n");
		return XST_FAILURE;
	}
	if (XV_gamma_lut_Write_HwReg_gamma_lut_2_Words(&gamma_lut, 0, (int *) xgamma8_curves[format],
			sizeof(xgamma8_10)/sizeof(int)) != sizeof(xgamma8_10)/sizeof(int)) {
		bsp_printf("Gamma correction LUT write failed\r\n");
		return XST_FAILURE;
	}

	XV_gamma_lut_EnableAutoRestart(&gamma_lut);
	XV_gamma_lut_Start(&gamma_lut);

	bsp_printf("Gamma correction LUT initialized\r\n");

	return XST_SUCCESS;
}

#endif // XPAR_XV_GAMMA_LUT_NUM_INSTANCES
