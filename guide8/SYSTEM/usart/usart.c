#include "usart.h"	
#include "dma.h"
////////////////////////////////////////////////////////////////////////////////// 	 
//如果使用ucos,则包括下面的头文件即可.
#if SYSTEM_SUPPORT_UCOS
#include "includes.h"					//ucos 使用	  
#endif 
 
//////////////////////////////////////////////////////////////////
//加入以下代码,支持printf函数,而不需要选择use MicroLIB	  
#if 1
#pragma import(__use_no_semihosting)             
//标准库需要的支持函数                 
struct __FILE 
{ 
	int handle; 

}; 

FILE __stdout;       
//定义_sys_exit()以避免使用半主机模式    
_sys_exit(int x) 
{ 
	x = x; 
} 
//重定义fputc函数 
int fputc(int ch, FILE *f)
{      
	while((USART1->SR&0X40)==0);//循环发送,直到发送完毕   
    USART1->DR = (u8) ch;      
	return ch;
}
#endif 
#if EN_USART1_RX   //如果使能了接收
//串口1中断服务程序
//注意,读取USARTx->SR能避免莫名其妙的错误   	
u8 USART_RX_BUF[USART_REC_LEN];     //接收缓冲,最大USART_REC_LEN个字节.
//接收状态
//bit15，	接收完成标志
//bit14，	接收到0x0d
//bit13~0，	接收到的有效字节数目
u16 USART_RX_STA=0;       //接收状态标记	  
  
void uart_init(u32 bound)
{
	//GPIO端口设置
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1|RCC_APB2Periph_GPIOA, ENABLE);	//使能USART1，GPIOA时钟
	//USART1_TX   PA.9
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9; //PA.9
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//复用推挽输出
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	//USART1_RX	  PA.10
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//浮空输入
	GPIO_Init(GPIOA, &GPIO_InitStructure);  

	//USART 初始化设置

	USART_InitStructure.USART_BaudRate = bound;//一般设置为9600;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//字长为8位数据格式
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//一个停止位
	USART_InitStructure.USART_Parity = USART_Parity_No;//无奇偶校验位
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//无硬件数据流控制
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//收发模式

	USART_Init(USART1, &USART_InitStructure); //初始化串口
	
	USART_ITConfig(USART1,USART_IT_TC,DISABLE); 
    USART_ITConfig(USART1,USART_IT_RXNE,DISABLE); 
    USART_ITConfig(USART1,USART_IT_IDLE,ENABLE);//开启串口空闲中断
	
	USART_DMACmd(USART1,USART_DMAReq_Rx,ENABLE);//采用DMA方式接收
	USART_Cmd(USART1, ENABLE); 
}
float mi[4]={10.0,10.0,10.0,10.0};
void Tomi(void)
{
	//第一个探头
	if((Uart_Rx[0]==0)&&(Uart_Rx[1]==0x18)&&(Uart_Rx[2]==0x18)&&(Uart_Rx[17]==0))
	{
		if((Uart_Rx[3]==0x18)&&(Uart_Rx[4]==0x18))	{mi[0] = 0.0;}
		if((Uart_Rx[3]==0x98)&&(Uart_Rx[4]==0x18))	{mi[0] = 0.4;}
		if((Uart_Rx[3]==0x98)&&(Uart_Rx[4]==0x98))	{mi[0] = 0.5;}
		if((Uart_Rx[3]==0x98)&&(Uart_Rx[4]==0x1E))	{mi[0] = 0.6;}
		if((Uart_Rx[3]==0x98)&&(Uart_Rx[4]==0x9E))	{mi[0] = 0.7;}
		if((Uart_Rx[3]==0x1E)&&(Uart_Rx[4]==0x18))	{mi[0] = 0.8;}
		if((Uart_Rx[3]==0x1E)&&(Uart_Rx[4]==0x98))	{mi[0] = 0.9;}
		if((Uart_Rx[3]==0x1E)&&(Uart_Rx[4]==0x1E))	{mi[0] = 1.0;}
		if((Uart_Rx[3]==0x1E)&&(Uart_Rx[4]==0x9E))	{mi[0] = 1.1;}
		if((Uart_Rx[3]==0x9E)&&(Uart_Rx[4]==0x18))	{mi[0] = 1.2;}
		if((Uart_Rx[3]==0x9E)&&(Uart_Rx[4]==0x98))	{mi[0] = 1.3;}
		if((Uart_Rx[3]==0x9E)&&(Uart_Rx[4]==0x1E))	{mi[0] = 1.4;}
		if((Uart_Rx[3]==0x9E)&&(Uart_Rx[4]==0x9E))	{mi[0] = 1.5;}
	}
	if((Uart_Rx[0]==0)&&(Uart_Rx[1]==0x18)&&(Uart_Rx[2]==0x98)&&(Uart_Rx[17]==0))
	{
		if((Uart_Rx[3]==0x18)&&(Uart_Rx[4]==0x18))	{mi[0] = 1.6;}
		if((Uart_Rx[3]==0x18)&&(Uart_Rx[4]==0x98))	{mi[0] = 1.7;}	
	}
	if((Uart_Rx[0]==0)&&(Uart_Rx[1]==0x9E)&&(Uart_Rx[2]==0x9E)&&(Uart_Rx[17]==0))
	{
		if((Uart_Rx[3]==0x9E)&&(Uart_Rx[4]==0x9E))	{mi[0] = 2.0;}
	}
	//第二个探头
	if((Uart_Rx[0]==0)&&(Uart_Rx[13]==0x18)&&(Uart_Rx[14]==0x18)&&(Uart_Rx[17]==0))
	{
		if((Uart_Rx[15]==0x18)&&(Uart_Rx[16]==0x18))	{mi[1] = 0.0;}
		if((Uart_Rx[15]==0x98)&&(Uart_Rx[16]==0x18))	{mi[1] = 0.4;}
		if((Uart_Rx[15]==0x98)&&(Uart_Rx[16]==0x98))	{mi[1] = 0.5;}
		if((Uart_Rx[15]==0x98)&&(Uart_Rx[16]==0x1E))	{mi[1] = 0.6;}
		if((Uart_Rx[15]==0x98)&&(Uart_Rx[16]==0x9E))	{mi[1] = 0.7;}
		if((Uart_Rx[15]==0x1E)&&(Uart_Rx[16]==0x18))	{mi[1] = 0.8;}
		if((Uart_Rx[15]==0x1E)&&(Uart_Rx[16]==0x98))	{mi[1] = 0.9;}
		if((Uart_Rx[15]==0x1E)&&(Uart_Rx[16]==0x1E))	{mi[1] = 1.0;}
		if((Uart_Rx[15]==0x1E)&&(Uart_Rx[16]==0x9E))	{mi[1] = 1.1;}
		if((Uart_Rx[15]==0x9E)&&(Uart_Rx[16]==0x18))	{mi[1] = 1.2;}
		if((Uart_Rx[15]==0x9E)&&(Uart_Rx[16]==0x98))	{mi[1] = 1.3;}
		if((Uart_Rx[15]==0x9E)&&(Uart_Rx[16]==0x1E))	{mi[1] = 1.4;}
		if((Uart_Rx[15]==0x9E)&&(Uart_Rx[16]==0x9E))	{mi[1] = 1.5;}
	}
	if((Uart_Rx[0]==0)&&(Uart_Rx[13]==0x18)&&(Uart_Rx[14]==0x98)&&(Uart_Rx[17]==0))
	{
		if((Uart_Rx[15]==0x18)&&(Uart_Rx[16]==0x18))	{mi[1] = 1.6;}
		if((Uart_Rx[15]==0x18)&&(Uart_Rx[16]==0x98))	{mi[1] = 1.7;}	
	}
	if((Uart_Rx[0]==0)&&(Uart_Rx[13]==0x9E)&&(Uart_Rx[14]==0x9E)&&(Uart_Rx[17]==0))
	{
		if((Uart_Rx[15]==0x9E)&&(Uart_Rx[16]==0x9E))	{mi[1] = 2.0;}
	}
	//第三个探头
	if((Uart_Rx[0]==0)&&(Uart_Rx[9]==0x18)&&(Uart_Rx[10]==0x18)&&(Uart_Rx[17]==0))
	{
		if((Uart_Rx[11]==0x18)&&(Uart_Rx[12]==0x18))	{mi[2] = 0.0;}
		if((Uart_Rx[11]==0x98)&&(Uart_Rx[12]==0x18))	{mi[2] = 0.4;}
		if((Uart_Rx[11]==0x98)&&(Uart_Rx[12]==0x98))	{mi[2] = 0.5;}
		if((Uart_Rx[11]==0x98)&&(Uart_Rx[12]==0x1E))	{mi[2] = 0.6;}
		if((Uart_Rx[11]==0x98)&&(Uart_Rx[12]==0x9E))	{mi[2] = 0.7;}
		if((Uart_Rx[11]==0x1E)&&(Uart_Rx[12]==0x18))	{mi[2] = 0.8;}
		if((Uart_Rx[11]==0x1E)&&(Uart_Rx[12]==0x98))	{mi[2] = 0.9;}
		if((Uart_Rx[11]==0x1E)&&(Uart_Rx[12]==0x1E))	{mi[2] = 1.0;}
		if((Uart_Rx[11]==0x1E)&&(Uart_Rx[12]==0x9E))	{mi[2] = 1.1;}
		if((Uart_Rx[11]==0x9E)&&(Uart_Rx[12]==0x18))	{mi[2] = 1.2;}
		if((Uart_Rx[11]==0x9E)&&(Uart_Rx[12]==0x98))	{mi[2] = 1.3;}
		if((Uart_Rx[11]==0x9E)&&(Uart_Rx[12]==0x1E))	{mi[2] = 1.4;}
		if((Uart_Rx[11]==0x9E)&&(Uart_Rx[12]==0x9E))	{mi[2] = 1.5;}
	}
	if((Uart_Rx[0]==0)&&(Uart_Rx[9]==0x18)&&(Uart_Rx[10]==0x98)&&(Uart_Rx[17]==0))
	{
		if((Uart_Rx[11]==0x18)&&(Uart_Rx[12]==0x18))	{mi[2] = 1.6;}
		if((Uart_Rx[11]==0x18)&&(Uart_Rx[12]==0x98))	{mi[2] = 1.7;}	
	}
	if((Uart_Rx[0]==0)&&(Uart_Rx[9]==0x9E)&&(Uart_Rx[10]==0x9E)&&(Uart_Rx[17]==0))
	{
		if((Uart_Rx[11]==0x9E)&&(Uart_Rx[12]==0x9E))	{mi[2] = 2.0;}
	}
	//第四个探头
	if((Uart_Rx[0]==0)&&(Uart_Rx[5]==0x18)&&(Uart_Rx[6]==0x18)&&(Uart_Rx[17]==0))
	{
		if((Uart_Rx[7]==0x18)&&(Uart_Rx[8]==0x18))	{mi[3] = 0.0;}
		if((Uart_Rx[7]==0x98)&&(Uart_Rx[8]==0x18))	{mi[3] = 0.4;}
		if((Uart_Rx[7]==0x98)&&(Uart_Rx[8]==0x98))	{mi[3] = 0.5;}
		if((Uart_Rx[7]==0x98)&&(Uart_Rx[8]==0x1E))	{mi[3] = 0.6;}
		if((Uart_Rx[7]==0x98)&&(Uart_Rx[8]==0x9E))	{mi[3] = 0.7;}
		if((Uart_Rx[7]==0x1E)&&(Uart_Rx[8]==0x18))	{mi[3] = 0.8;}
		if((Uart_Rx[7]==0x1E)&&(Uart_Rx[8]==0x98))	{mi[3] = 0.9;}
		if((Uart_Rx[7]==0x1E)&&(Uart_Rx[8]==0x1E))	{mi[3] = 1.0;}
		if((Uart_Rx[7]==0x1E)&&(Uart_Rx[8]==0x9E))	{mi[3] = 1.1;}
		if((Uart_Rx[7]==0x9E)&&(Uart_Rx[8]==0x18))	{mi[3] = 1.2;}
		if((Uart_Rx[7]==0x9E)&&(Uart_Rx[8]==0x98))	{mi[3] = 1.3;}
		if((Uart_Rx[7]==0x9E)&&(Uart_Rx[8]==0x1E))	{mi[3] = 1.4;}
		if((Uart_Rx[7]==0x9E)&&(Uart_Rx[8]==0x9E))	{mi[3] = 1.5;}
	}
	if((Uart_Rx[0]==0)&&(Uart_Rx[5]==0x18)&&(Uart_Rx[6]==0x98)&&(Uart_Rx[17]==0))
	{
		if((Uart_Rx[7]==0x18)&&(Uart_Rx[8]==0x18))	{mi[3] = 1.6;}
		if((Uart_Rx[7]==0x18)&&(Uart_Rx[8]==0x98))	{mi[3] = 1.7;}	
	}
	if((Uart_Rx[0]==0)&&(Uart_Rx[5]==0x9E)&&(Uart_Rx[6]==0x9E)&&(Uart_Rx[17]==0))
	{
		if((Uart_Rx[7]==0x9E)&&(Uart_Rx[8]==0x9E))	{mi[3] = 2.0;}
	}
}
u16 Length = 0; 
void USART1_IRQHandler(void)                	//串口1中断服务程序
{
#ifdef OS_TICKS_PER_SEC	 	//如果时钟节拍数定义了,说明要使用ucosII了.
	OSIntEnter();    
#endif 
    if(USART_GetITStatus(USART1, USART_IT_IDLE) != RESET) 
    { 
        DMA_Cmd(DMA1_Channel5,DISABLE);
        Length = USART1->SR; 
        Length = USART1->DR; 
        Length = UART_RX_LEN - DMA_GetCurrDataCounter(DMA1_Channel5); 
		Tomi();
        DMA1_Channel5->CNDTR = UART_RX_LEN;
        DMA_Cmd(DMA1_Channel5, ENABLE);
    }  
#ifdef OS_TICKS_PER_SEC	 	//如果时钟节拍数定义了,说明要使用ucosII了.
	OSIntExit();  											 
#endif
} 
#endif	

