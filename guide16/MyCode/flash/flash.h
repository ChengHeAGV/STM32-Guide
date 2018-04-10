#ifndef __FLASH_H
#define __FLASH_H

#include "sys.h"

typedef enum {
	FLASH_WRITE_OK = 1,
	FLASH_WRIKE_NO = 0,
	FLASH_READ_OK = 1,
	FLASH_READ_NO = 0
}FLASH_FLAG ;

extern unsigned char FLASH_WRITE(unsigned short int * memory_data,u16 t);
extern unsigned char FLASH_READ(unsigned short int * memory_data,unsigned short int n);

int ReadFlashNBtye(uint32_t ReadAddress, uint16_t *ReadBuf, int32_t ReadNum);
void WriteFlashOneWord(uint32_t WriteAddress,uint16_t WriteData);


#endif
