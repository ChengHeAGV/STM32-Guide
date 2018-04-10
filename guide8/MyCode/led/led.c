#include "led.h"

void Init_LEDpin(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	/*失能JATG口*/
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO | RCC_APB2Periph_GPIOA , ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB , ENABLE);	
	GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable,ENABLE);
		
	//input
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_2|GPIO_Pin_3|GPIO_Pin_4|GPIO_Pin_5|GPIO_Pin_6|GPIO_Pin_7;// 端口配置
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; 		 	//输入
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO口速度为50MHz
	GPIO_Init(GPIOA, &GPIO_InitStructure);					 //根据设定参数初始化GPIOB

//	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;// 端口配置
//	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //推挽输出
//	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO口速度为50MHz
//	GPIO_Init(GPIOA, &GPIO_InitStructure);					 //根据设定参数初始化GPIOB
//	GPIO_ResetBits(GPIOA,GPIO_Pin_7);	
	
//	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;// 端口配置
//	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; 		 	//输入
//	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO口速度为50MHz
//	GPIO_Init(GPIOA, &GPIO_InitStructure);					 //根据设定参数初始化GPIOB	
}

