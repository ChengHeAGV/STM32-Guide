#include "dma.h"
#define ADC1_DR_Address    ((uint32_t)0x4001244C)

u8  Uart_Rx[UART_RX_LEN];
u16 AD_Value[30][8];
void DMA_USART1_Configuration(void)
{
    DMA_InitTypeDef DMA_InitStructure;

    DMA_DeInit(DMA1_Channel5); 
 
    DMA_InitStructure.DMA_PeripheralBaseAddr = (u32)(&USART1->DR); 

    DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)Uart_Rx; 

    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC; 

    DMA_InitStructure.DMA_BufferSize = UART_RX_LEN; 

    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable; 

    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable; 

    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte; 

    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte; 

    DMA_InitStructure.DMA_Mode = DMA_Mode_Normal; 

    DMA_InitStructure.DMA_Priority = DMA_Priority_VeryHigh; 

    DMA_InitStructure.DMA_M2M = DMA_M2M_Disable; 
	
    DMA_Init(DMA1_Channel5,&DMA_InitStructure); 

    DMA_Cmd(DMA1_Channel5,ENABLE);
}
void DMA_ADC_Configuration(void)   
{ 
	DMA_InitTypeDef DMA_InitStructure; 
    DMA_DeInit(DMA1_Channel1);   
    DMA_InitStructure.DMA_PeripheralBaseAddr = ADC1_DR_Address;   
    DMA_InitStructure.DMA_MemoryBaseAddr = (u32)&AD_Value;   
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;   
 
    DMA_InitStructure.DMA_BufferSize = 240;   
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;   
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;   
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;   
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;   
  
    DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;   
    DMA_InitStructure.DMA_Priority = DMA_Priority_High;   
    DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;   
    DMA_Init(DMA1_Channel1, &DMA_InitStructure);   
 
    DMA_Cmd(DMA1_Channel1, ENABLE);
	DMA_ITConfig(DMA1_Channel1, DMA_IT_TC, ENABLE);   
} 
