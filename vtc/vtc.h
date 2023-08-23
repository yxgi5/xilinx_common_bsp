
#ifndef _VTIMING_GEN_H_
#define _VTIMING_GEN_H_

#ifdef XPAR_XVTC_NUM_INSTANCES
// *****************************************************
// Notes
// *****************************************************
//   - The purpose of this driver is to provide a layer
//     of abstraction over the Video Timing Controller
//     drivers when it is used in generate-only mode. It
//     assumes the caller has allocated and initialized
//     the XVtc instance to be used.

// *****************************************************
// Dependencies
// *****************************************************
#include "xbasic_types.h" // microblaze_0/include/
#include "xvtc.h"


// *****************************************************
// Function status return values
// *****************************************************
#define VTIMING_GEN_SUCCESS        0
#define VTIMING_GEN_ERROR_UNKNOWN -1

typedef enum video_resolution_no
{
	VIDEO_RESOLUTION_VGA = 0,
	VIDEO_RESOLUTION_480P,
	VIDEO_RESOLUTION_576P,
	VIDEO_RESOLUTION_SVGA,
	VIDEO_RESOLUTION_XGA,
	VIDEO_RESOLUTION_720P,
	VIDEO_RESOLUTION_SXGA,
	VIDEO_RESOLUTION_1080P,
	VIDEO_RESOLUTION_FHDPLUS,
	VIDEO_RESOLUTION_UXGA,
	VIDEO_RESOLUTION_1920X960,
	VIDEO_RESOLUTION_4K,//4K30
	VIDEO_RESOLUTION_4KFULL,//4K30W
	VIDEO_RESOLUTION_CUSTOM1,
	VIDEO_RESOLUTION_4KCUST,
	VIDEO_RESOLUTION_BAIDUSF,

	VIDEO_RESOLUTION_NO_BUTT,
} resolution_no;

struct struct_vres_timing_t
{
	char *pName;
	Xuint32 VActiveVideo;
	Xuint32 VFrontPorch;
	Xuint32 VSyncWidth;
	Xuint32 VBackPorch;
	Xuint32 VSyncPolarity;
	Xuint32 HActiveVideo;
	Xuint32 HFrontPorch;
	Xuint32 HSyncWidth;
	Xuint32 HBackPorch;
	Xuint32 HSyncPolarity;
};
typedef struct struct_vres_timing_t vres_timing_t;


#if XPAR_XVTC_NUM_INSTANCES==1
extern XVtc        VtcInst0;
#elif XPAR_XVTC_NUM_INSTANCES==2
extern XVtc        VtcInst0;
extern XVtc        VtcInst1;
#elif XPAR_XVTC_NUM_INSTANCES==3
extern XVtc        VtcInst0;
extern XVtc        VtcInst1;
extern XVtc        VtcInst2;
#elif XPAR_XVTC_NUM_INSTANCES==4
extern XVtc        VtcInst0;
extern XVtc        VtcInst1;
extern XVtc        VtcInst2;
extern XVtc        VtcInst3;
#endif


// *****************************************************
// Public functions
// *****************************************************

char *  vres_get_name(Xuint32 resolutionId);
Xuint32 vres_get_width(Xuint32 resolutionId);
Xuint32 vres_get_height(Xuint32 resolutionId);
Xuint32 vres_get_timing(Xuint32 resolutionId, vres_timing_t *pTiming );

Xint32 vres_detect( Xuint32 width, Xuint32 height );

// vtiming_gen_run() - Set up and start VTC generator registers and enable.
//   - p_vtg_inst    - Pointer to object to work on
//   - resolution_id - Which video resolution id to use from video_resolution.h
//   - verbose       - Whether or not to print debug info
//   - return        - None
void vtiming_gen_run
(
	XVtc* p_vtg_inst,
	int   resolution_id,
	int   verbose
);


int vtc_config(void);

#endif // XPAR_XVTC_NUM_INSTANCES
#endif // _VTIMING_GEN_H_

