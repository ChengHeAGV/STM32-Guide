#include "adc.h"
#include "dma.h"
#define N 30
float  After_filter[8];
void ADC1_Configuration(void)   
{   
    ADC_InitTypeDef ADC_InitStructure;  
	GPIO_InitTypeDef GPIO_InitStructure;	
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE); 
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_ADC1,ENABLE); 

	GPIO_InitStructure.GPIO_Pin =GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_2|GPIO_Pin_3|GPIO_Pin_4|GPIO_Pin_5|GPIO_Pin_6|GPIO_Pin_7;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN; 
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	RCC_ADCCLKConfig(RCC_PCLK2_Div6);	
	
	ADC_DeInit(ADC1);	
    ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;  
    ADC_InitStructure.ADC_ScanConvMode = ENABLE;   
    ADC_InitStructure.ADC_ContinuousConvMode = ENABLE; //连续转换开启
    ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;   
    ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;   
    ADC_InitStructure.ADC_NbrOfChannel = 8;  //设置转换序列长度
    ADC_Init(ADC1, &ADC_InitStructure);   

    ADC_TempSensorVrefintCmd(ENABLE);//ADC内置温度传感器失使能   
        
    ADC_RegularChannelConfig(ADC1, ADC_Channel_0, 1, ADC_SampleTime_239Cycles5);   
	ADC_RegularChannelConfig(ADC1, ADC_Channel_1, 2, ADC_SampleTime_239Cycles5); 
	ADC_RegularChannelConfig(ADC1, ADC_Channel_2, 3, ADC_SampleTime_239Cycles5); 
	ADC_RegularChannelConfig(ADC1, ADC_Channel_3, 4, ADC_SampleTime_239Cycles5);  	
    ADC_RegularChannelConfig(ADC1, ADC_Channel_4, 5, ADC_SampleTime_239Cycles5);   
	ADC_RegularChannelConfig(ADC1, ADC_Channel_5, 6, ADC_SampleTime_239Cycles5); 
	ADC_RegularChannelConfig(ADC1, ADC_Channel_6, 7, ADC_SampleTime_239Cycles5); 
	ADC_RegularChannelConfig(ADC1, ADC_Channel_7, 8, ADC_SampleTime_239Cycles5); 
	
    ADC_Cmd(ADC1, ENABLE);   
 
    ADC_DMACmd(ADC1, ENABLE);//开启DAM   
    //ADC自校准 
    ADC_ResetCalibration(ADC1);   
 
    while(ADC_GetResetCalibrationStatus(ADC1));   
   
    ADC_StartCalibration(ADC1);   
  
    while(ADC_GetCalibrationStatus(ADC1));   

    ADC_SoftwareStartConvCmd(ADC1, ENABLE); //启动ADC 
}  

u16 filter_super(u16 *num, u8 n)       
{
	u8 i, j,k;
	u16 sum= 0;	
	u16 temp = 0;
	
	u16 start=10;
	u16 stop=n-10;

	for(i = 0; i < n; i++) 
	{
		for(j = 0; i + j < n - 1; j++)
		{
			if(num[j] > num[j + 1])
			{
				temp = num[j];
				num[j] = num[j + 1];
				num[j + 1] = temp;
			}            
		}
	}
	
	
	for(k=start;k<stop;k++)
	{
		sum += num[k];
	}

	sum=sum / (stop-start+1);
	return sum;
}

void filter(void)
{
   u8 count,i;
   u16 buf[N];
   for(i=0;i<8;i++)
   {
		for (count=0;count<N;count++)
		{
			buf[count]=AD_Value[count][i];
		}
	   After_filter[i]=filter_super(buf,N);
   }  
}
void DMA1_Channel1_IRQHandler(void)
{
	if(DMA_GetITStatus(DMA1_IT_TC1) != RESET)
	{
	  filter();
	  DMA_ClearITPendingBit(DMA1_IT_TC1);
	}
}









