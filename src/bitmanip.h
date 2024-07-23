#ifndef BITMANIP_H
#define BITMANIP_H

//#define U8 		unsigned char
//#define U16 	unsigned int
//#define U32 	unsigned long int
//#define U64 	unsigned long long int
//#define S8 		signed char
//#define S16 	signed int
//#define uchar	unsigned char
//#define uint	unsigned int

// it is better to include <stdint.h> to use defines like uint32_t to ensure bits
#define U8 		uint8_t
#define U16 	uint16_t
#define U32 	uint32_t
#define U64 	uint64_t
#define S8 		int8_t
#define S16 	int16_t
#define S32 	int32_t
#define S64 	int64_t
#define uchar	unsigned char
#define uint	unsigned int

/*------------*/
#define NOP()	asm("nop")
#define WDR() 	asm("wdr")

#define BIT(x) (1 << (x)) 	        //select bit (normally implicit 16bit)
#define BIT32(x) ((U32)1 << (x))    //select bit explicit 32bit
#define BIT64(x) ((U64)1u << (x))	//select bit explicit 64bit
#define SETB(p,b) ((p)|=(b))         //set bit
#define CLRB(p,b) ((p)&=~(b))        //clear bit
#define FLPB(p,b) ((p)^=(b))         //flip bit
#define CHKB(p,b) ((p) & (b))        //check bit

#define __BITSPERBYTE 8
#define __BITS(type)  (__BITSPERBYTE * (int)sizeof(type))
#ifndef BITS
#define BITS(type) __BITS(type)
#endif

#define mdelay(msecs)		usleep(1000*msecs)
#define udelay(usecs)		usleep(usecs)

#define ARRAY_SIZE(ar)		(sizeof(ar)/sizeof(ar[0]))
//when handling string (char*), use strlen() define in <string.h>

#define	BIT0		0x01
#define	BIT1		0x02
#define	BIT2		0x04
#define	BIT3		0x08
#define	BIT4		0x10
#define	BIT5		0x20
#define	BIT6		0x40
#define	BIT7		0x80

//#define	NULL		0
#define	ON			1
#define	OFF			0

#define	LOW			0
#define	MIDDLE		1
#define	HIGH		2

// if not include stdbool.h
#ifndef _STDBOOL_H
typedef enum bool
{
    false = 0,
	true,
} bool;
#endif

#endif //BITMANIP_H
