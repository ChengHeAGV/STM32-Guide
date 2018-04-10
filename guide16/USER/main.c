#include "stm32f10x.h"
#include "delay.h"
#include "usart.h"
#include "led.h"
#include "dma.h"
#include "adc.h"
#include "can.h"
#include "flash.h"
#include "stm32f10x_flash.h"
#include "wdg.h"

u16 last_cansend=0;
u8 value =35;
int main(void)
{	
	
	u8 num=0;
	u8 time_count=0;
	u8 i=0;	
	delay_init();	
	NVIC_Configuration();
	uart_init(9600);
	DMA_USART1_Configuration();
	DMA_ADC_Configuration();
	ADC1_Configuration();
	Init_LEDpin();
	IWDG_Init(4,625);    //与分频数为64,重载值为625,溢出时间为1s
	CAN_Mode_Init(CAN_SJW_1tq,CAN_BS2_8tq,CAN_BS1_9tq,4,0);
	
	delay_ms(300);

	//读内存
	FLASH_READ(tempmem, 18);
	if(tempmem[16] != 10)
	{

		tempmem[0] = (After_filter[0] * 330 / 4096);
		tempmem[1] = (After_filter[1] * 330 / 4096);
		tempmem[2] = (After_filter[2] * 330 / 4096);
		tempmem[3] = (After_filter[3] * 330 / 4096);
		tempmem[4] = (After_filter[4] * 330 / 4096);
		tempmem[5] = (After_filter[5] * 330 / 4096);
		tempmem[6] = (After_filter[6] * 330 / 4096);
		tempmem[7] = (After_filter[7] * 330 / 4096);
		tempmem[8] = (After_filter[8] * 330 / 4096);
		tempmem[9] = (After_filter[9] * 330 / 4096);
		tempmem[10] = (After_filter[10] * 330 / 4096);
		tempmem[11] = (After_filter[11] * 330 / 4096);
		tempmem[12] = (After_filter[12] * 330 / 4096);
		tempmem[13] = (After_filter[13] * 330 / 4096);
		tempmem[14] = (After_filter[14] * 330 / 4096);
		tempmem[15] = (After_filter[15] * 330 / 4096);
		
		tempmem[16] = 10;
		
		tempmem[17] = 4;//设备ID号
		tempmem[18] = 10;//阈值

		FLASH_WRITE(tempmem, 17);
}

	//读取默认值
	FLASH_READ(adc1, 20);

	Read_ID = tempmem[17];//设备ID号
  threshold = tempmem[18];//阈值

	while(1)
	{
		adc2[0] = (After_filter[0] * 330 / 4096) - adc1[0];
		adc2[1] = (After_filter[1] * 330 / 4096) - adc1[1];
		adc2[2] = (After_filter[2] * 330 / 4096) - adc1[2];
		adc2[3] = (After_filter[3] * 330 / 4096) - adc1[3];
		adc2[4] = (After_filter[4] * 330 / 4096) - adc1[4];
		adc2[5] = (After_filter[5] * 330 / 4096) - adc1[5];
		adc2[6] = (After_filter[6] * 330 / 4096) - adc1[6];
		adc2[7] = (After_filter[7] * 330 / 4096) - adc1[7];
		adc2[8] = (After_filter[8] * 330 / 4096) - adc1[8];
		adc2[9] = (After_filter[9] * 330 / 4096) - adc1[9];
		adc2[10] = (After_filter[10] * 330 / 4096) - adc1[10];
		adc2[11] = (After_filter[11] * 330 / 4096) - adc1[11];
		adc2[12] = (After_filter[12] * 330 / 4096) - adc1[12];
		adc2[13] = (After_filter[13] * 330 / 4096) - adc1[13];
		adc2[14] = (After_filter[14] * 330 / 4096) - adc1[14];
		adc2[15] = (After_filter[15] * 330 / 4096) - adc1[15];

		for(i = 0; i < 16; i++)
		{
			if(adc2[i] > threshold)
			{
				led[i] = 0;
			}
			else
			{ 
				led[i] = 1; 
			}
		}
		cansend[0]=led[15]|led[14]<<1|led[13]<<2|led[12]<<3|led[11]<<4|led[10]<<5|led[9]<<6|led[8]<<7;
		cansend[1]=led[7]|led[6]<<1|led[5]<<2|led[4]<<3|led[3]<<4|led[2]<<5|led[1]<<6|led[0]<<7;
		if(last_cansend!=cansend[0])
		{
			time_count=0;
			send();
			last_cansend=cansend[0];
		}
		if(time_count>200)
		{
			time_count=0;
			send();
			last_cansend=cansend[0];
		}
		time_count++;
		if(num==20)
		{
			num=0;
			IWDG_Feed();
		}
		num++;
		delay_ms(5);
	}
}


