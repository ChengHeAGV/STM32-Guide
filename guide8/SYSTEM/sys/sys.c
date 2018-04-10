#include "sys.h"

void NVIC_Configuration(void)
{
    NVIC_InitTypeDef NVIC_InitStructure;
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);	//…Ë÷√NVIC÷–∂œ∑÷◊È2:2Œª«¿’º”≈œ»º∂£¨2ŒªœÏ”¶”≈œ»º∂
	
	//Usart1 NVIC ≈‰÷√	
	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3 ;//«¿’º”≈œ»º∂3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;		//◊””≈œ»º∂3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQÕ®µ¿ πƒ‹
	NVIC_Init(&NVIC_InitStructure);	//∏˘æ›÷∏∂®µƒ≤Œ ˝≥ı ºªØVICºƒ¥Ê∆
	
	NVIC_InitStructure.NVIC_IRQChannel = DMA1_Channel4_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
	
	//ADC NVIC ≈‰÷√
	NVIC_InitStructure.NVIC_IRQChannel = DMA1_Channel1_IRQn; 
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1; 
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0; 
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; 
    NVIC_Init(&NVIC_InitStructure);
}
