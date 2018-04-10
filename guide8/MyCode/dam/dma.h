#ifndef __DMA_H
#define __DMA_H

#include "sys.h"


extern void DMA_USART1_Configuration(void);
extern void DMA_ADC_Configuration(void);
extern u8 Uart_Rx[UART_RX_LEN];
extern u16 AD_Value[30][8];
#endif


