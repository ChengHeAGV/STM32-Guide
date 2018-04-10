#include "usart.h"	
#include "dma.h"
////////////////////////////////////////////////////////////////////////////////// 	 
//���ʹ��ucos,����������ͷ�ļ�����.
#if SYSTEM_SUPPORT_UCOS
#include "includes.h"					//ucos ʹ��	  
#endif 
 
//////////////////////////////////////////////////////////////////
//�������´���,֧��printf����,������Ҫѡ��use MicroLIB	  
#if 1
#pragma import(__use_no_semihosting)             
//��׼����Ҫ��֧�ֺ���                 
struct __FILE 
{ 
	int handle; 

}; 

FILE __stdout;       
//����_sys_exit()�Ա���ʹ�ð�����ģʽ    
_sys_exit(int x) 
{ 
	x = x; 
} 
//�ض���fputc���� 
int fputc(int ch, FILE *f)
{      
	while((USART1->SR&0X40)==0);//ѭ������,ֱ���������   
    USART1->DR = (u8) ch;      
	return ch;
}
#endif 
#if EN_USART1_RX   //���ʹ���˽���
//����1�жϷ������
//ע��,��ȡUSARTx->SR�ܱ���Ī������Ĵ���   	
u8 USART_RX_BUF[USART_REC_LEN];     //���ջ���,���USART_REC_LEN���ֽ�.
//����״̬
//bit15��	������ɱ�־
//bit14��	���յ�0x0d
//bit13~0��	���յ�����Ч�ֽ���Ŀ
u16 USART_RX_STA=0;       //����״̬���	  
  
void uart_init(u32 bound)
{
	//GPIO�˿�����
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1|RCC_APB2Periph_GPIOA, ENABLE);	//ʹ��USART1��GPIOAʱ��
	//USART1_TX   PA.9
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9; //PA.9
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//�����������
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	//USART1_RX	  PA.10
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//��������
	GPIO_Init(GPIOA, &GPIO_InitStructure);  

	//USART ��ʼ������

	USART_InitStructure.USART_BaudRate = bound;//һ������Ϊ9600;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//�ֳ�Ϊ8λ���ݸ�ʽ
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//һ��ֹͣλ
	USART_InitStructure.USART_Parity = USART_Parity_No;//����żУ��λ
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//��Ӳ������������
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//�շ�ģʽ

	USART_Init(USART1, &USART_InitStructure); //��ʼ������
	
	USART_ITConfig(USART1,USART_IT_TC,DISABLE); 
    USART_ITConfig(USART1,USART_IT_RXNE,DISABLE); 
    USART_ITConfig(USART1,USART_IT_IDLE,ENABLE);//�������ڿ����ж�
	
	USART_DMACmd(USART1,USART_DMAReq_Rx,ENABLE);//����DMA��ʽ����
	USART_Cmd(USART1, ENABLE); 
}
float mi[4]={10.0,10.0,10.0,10.0};
void Tomi(void)
{
	//��һ��̽ͷ
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
	//�ڶ���̽ͷ
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
	//������̽ͷ
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
	//���ĸ�̽ͷ
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
void USART1_IRQHandler(void)                	//����1�жϷ������
{
#ifdef OS_TICKS_PER_SEC	 	//���ʱ�ӽ�����������,˵��Ҫʹ��ucosII��.
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
#ifdef OS_TICKS_PER_SEC	 	//���ʱ�ӽ�����������,˵��Ҫʹ��ucosII��.
	OSIntExit();  											 
#endif
} 
#endif	

