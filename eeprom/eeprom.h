#ifndef __EEPROM_H__

//#include "xparameters.h"
#if defined (XPAR_XIICPS_NUM_INSTANCES) && defined (__PS_I2C_H__)
#define __EEPROM_H__
#include "xil_types.h"
//#if defined (ARMR5) || (__aarch64__) || (__arm__)
#include "xiicps.h"
//#endif
typedef u16 AddressType;

#if 0
int I2cMux_Eeprom(void);
#endif
s32 EepromWriteData(XIicPs *IicInstance, u8 * WriteBuffer, u16 ByteCount);
s32 EepromWriteDataContinus(XIicPs *IicInstance, u8 * Buffer, u16 ByteCount, u16 start_addr);
s32 EepromReadData(XIicPs *IicInstance, u8 *BufferPtr, u16 ByteCount);
#if 0
s32 IicPsEepromPolledExample();
#endif

#endif // XPAR_XIICPS_NUM_INSTANCES && __PS_I2C_H__

#endif // __EEPROM_H__

