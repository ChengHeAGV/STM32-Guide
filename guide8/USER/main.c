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
uint16_t Temp_Data[1] = {0};
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
	
	
	ReadFlashNBtye(0, Temp_Data,1);//将片内flash中的数据读入到Temp_Data中
	
	Read_ID=Temp_Data[0];//将读出来的数据赋值给自身ID号
	
	delay_ms(300);

	//读内存
	FLASH_READ(tempmem, 10);
	if(tempmem[8] != 6)
	{
		tempmem[8] = 6;

		tempmem[0] = (After_filter[0] * 330 / 4096);
		tempmem[1] = (After_filter[1] * 330 / 4096);
		tempmem[2] = (After_filter[2] * 330 / 4096);
		tempmem[3] = (After_filter[3] * 330 / 4096);
		tempmem[4] = (After_filter[4] * 330 / 4096);
		tempmem[5] = (After_filter[5] * 330 / 4096);
		tempmem[6] = (After_filter[6] * 330 / 4096);
		tempmem[7] = (After_filter[7] * 330 / 4096);

		FLASH_WRITE(tempmem, 9);
	}

	//读取默认值
	FLASH_READ(adc1, 8);
	
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

		for(i = 0; i < 8; i++)
		{
			if(adc2[i] > value)
			{
				led[i] = 0;
			}
			else
			{ 
				led[i] = 1; 
			}
		}
		cansend[0]=led[7]|led[6]<<1|led[5]<<2|led[4]<<3|led[3]<<4|led[2]<<5|led[1]<<6|led[0]<<7;
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


