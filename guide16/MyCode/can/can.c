#include "can.h"
#include "led.h"
#include "delay.h"
#include "usart.h"
#include "flash.h"
#include "adc.h"
#include "stm32f10x_flash.h"

u8 num=0;

//u16 adc1[16];//Ĭ�ϲ���
u16 adc2[16];
u16 tempmem[20];//�ڴ�
u16 led[16];
u16 threshold=0;//��ֵ
u8 Read_ID=0;
//CAN��ʼ��
//tsjw:����ͬ����Ծʱ�䵥Ԫ.��Χ:CAN_SJW_1tq~ CAN_SJW_4tq
//tbs2:ʱ���2��ʱ�䵥Ԫ.   ��Χ:CAN_BS2_1tq~CAN_BS2_8tq;
//tbs1:ʱ���1��ʱ�䵥Ԫ.   ��Χ:CAN_BS1_1tq ~CAN_BS1_16tq
//brp :�����ʷ�Ƶ��.��Χ:1~1024;  tq=(brp)*tpclk1
//������=Fpclk1/((tbs1+1+tbs2+1+1)*brp);
//mode:CAN_Mode_Normal,��ͨģʽ;CAN_Mode_LoopBack,�ػ�ģʽ;
//Fpclk1��ʱ���ڳ�ʼ����ʱ������Ϊ36M,�������CAN_Mode_Init(CAN_SJW_1tq,CAN_BS2_8tq,CAN_BS1_9tq,4,CAN_Mode_LoopBack);
//������Ϊ:36M/((8+9+1)*4)=500Kbps
//����ֵ:0,��ʼ��OK;
//����,��ʼ��ʧ��; 
u8 CAN_Mode_Init(u8 tsjw,u8 tbs2,u8 tbs1,u16 brp,u8 mode)
{ 
	GPIO_InitTypeDef 		GPIO_InitStructure; 
	CAN_InitTypeDef        	CAN_InitStructure;
	CAN_FilterInitTypeDef  	CAN_FilterInitStructure;
#if CAN_RX0_INT_ENABLE 
	NVIC_InitTypeDef  		NVIC_InitStructure;
#endif
                											 
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_CAN1, ENABLE);//ʹ��CAN1ʱ��
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;	//�������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);			//��ʼ��IO

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//��������
	GPIO_Init(GPIOA, &GPIO_InitStructure);			//��ʼ��IO

	//GPIO_PinRemapConfig(GPIO_Remap1_CAN1, ENABLE);//�˿���ӳ��
	/* CAN register init */  
	CAN_DeInit(CAN1);
	CAN_StructInit(&CAN_InitStructure);
	//CAN��Ԫ����
	CAN_InitStructure.CAN_TTCM=DISABLE;			//��ʱ�䴥��ͨ��ģʽ  
	CAN_InitStructure.CAN_ABOM=DISABLE;			//����Զ����߹���	 
	CAN_InitStructure.CAN_AWUM=DISABLE;			//˯��ģʽͨ���������(���CAN->MCR��SLEEPλ)
	CAN_InitStructure.CAN_NART=ENABLE;			//��ֹ�����Զ����� 
	CAN_InitStructure.CAN_RFLM=DISABLE;		 	//���Ĳ�����,�µĸ��Ǿɵ�  
	CAN_InitStructure.CAN_TXFP=DISABLE;			//���ȼ��ɱ��ı�ʶ������ 
	CAN_InitStructure.CAN_Mode= mode;	        //ģʽ���ã� mode:0,��ͨģʽ;1,�ػ�ģʽ; 
	//���ò�����
	CAN_InitStructure.CAN_SJW=tsjw;				//����ͬ����Ծ���(Tsjw)Ϊtsjw+1��ʱ�䵥λ  CAN_SJW_1tq	 CAN_SJW_2tq CAN_SJW_3tq CAN_SJW_4tq
	CAN_InitStructure.CAN_BS1=tbs1; 			//Tbs1=tbs1+1��ʱ�䵥λCAN_BS1_1tq ~CAN_BS1_16tq
	CAN_InitStructure.CAN_BS2=tbs2;				//Tbs2=tbs2+1��ʱ�䵥λCAN_BS2_1tq ~	CAN_BS2_8tq
	CAN_InitStructure.CAN_Prescaler=brp;        //��Ƶϵ��(Fdiv)Ϊbrp+1	
	CAN_Init(CAN1, &CAN_InitStructure);        	//��ʼ��CAN1 

//	CAN_FilterInitStructure.CAN_FilterNumber=0;	//������0
//	CAN_FilterInitStructure.CAN_FilterMode=CAN_FilterMode_IdMask; 	//����λģʽ
//	CAN_FilterInitStructure.CAN_FilterScale=CAN_FilterScale_32bit; 	//32λ�� 
//	CAN_FilterInitStructure.CAN_FilterIdHigh=0x0220;	//32λID����׼֡IDΪ11λ��IDΪ0x10ʱ��������00010000����ȫ11λΪ00000010000����Ӧʮ������Ϊ0x0200.
//	CAN_FilterInitStructure.CAN_FilterIdLow=0x0002;		//���ý��յ�Ϊ��׼ң�أ�Զ�̣�֡��
//	CAN_FilterInitStructure.CAN_FilterMaskIdHigh=0xffe0;//32λMASK��Ϊ1��ѡ�и�λ��ѡ��11λID
//	CAN_FilterInitStructure.CAN_FilterMaskIdLow=0x0006;//ѡ�б�׼ң��֡
//	CAN_FilterInitStructure.CAN_FilterFIFOAssignment=CAN_Filter_FIFO0;//������0������FIFO0
//	CAN_FilterInitStructure.CAN_FilterActivation=ENABLE;//���������0

	CAN_FilterInitStructure.CAN_FilterNumber=0;	//������0
	CAN_FilterInitStructure.CAN_FilterMode=CAN_FilterMode_IdMask; 	//����λģʽ
	CAN_FilterInitStructure.CAN_FilterScale=CAN_FilterScale_32bit; 	//32λ�� 
	CAN_FilterInitStructure.CAN_FilterIdHigh=0x0000;	//32λID
	CAN_FilterInitStructure.CAN_FilterIdLow=0x0000;
	CAN_FilterInitStructure.CAN_FilterMaskIdHigh=0x0000;//32λMASK
	CAN_FilterInitStructure.CAN_FilterMaskIdLow=0x0000;
	CAN_FilterInitStructure.CAN_FilterFIFOAssignment=CAN_Filter_FIFO0;//������0������FIFO0
	CAN_FilterInitStructure.CAN_FilterActivation=ENABLE;//���������0


	CAN_FilterInit(&CAN_FilterInitStructure);			//�˲�����ʼ��
	
#if CAN_RX0_INT_ENABLE 
	CAN_ITConfig(CAN1,CAN_IT_FMP0,ENABLE);				//FIFO0��Ϣ�Һ��ж�����.		    
 
	NVIC_InitStructure.NVIC_IRQChannel = USB_LP_CAN1_RX0_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;     // �����ȼ�Ϊ1
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;            // �����ȼ�Ϊ0
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
#endif
	return 0;
}   
 
#if CAN_RX0_INT_ENABLE	//ʹ��RX0�ж�

//�жϷ�����			    
void USB_LP_CAN1_RX0_IRQHandler(void)
{
	  u8 buf[2];
  	CanRxMsg RxMessage;
    CAN_Receive(CAN1, 0, &RxMessage);
	
		if(RxMessage.StdId==127)//���Ǵŵ����޸�������������ʱ
		{
			//ID
			if(RxMessage.Data[0]==1) 
			{
				if(RxMessage.Data[1]==2) //����
				{
					Read_ID = RxMessage.Data[2];//����ID
					tempmem[17] = Read_ID;
					FLASH_WRITE(tempmem, 20);
				}
			}
			//��ֵ
			else if(RxMessage.Data[0]==3) 
			{
				if(RxMessage.Data[1]==1) //��ѯ
				{
					buf[0] = 3;
					buf[1] = threshold;
					Can_Send_Msg(buf,2);
				}
				else
				if(RxMessage.Data[1]==2) //����
				{
					threshold = RxMessage.Data[2];//������ֵ
					tempmem[18] = threshold;
					FLASH_WRITE(tempmem, 20);
				}
			}
			//�ŵ���Ĭ��ADֵ
			else if(RxMessage.Data[0]==4) 
			{
				if(RxMessage.Data[1]==2) //����
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
					
					FLASH_WRITE(tempmem, 20);
				}
			}
		}
}
#endif

//can����һ������(�̶���ʽ:IDΪ0X12,��׼֡,����֡)	
//len:���ݳ���(���Ϊ8)				     
//msg:����ָ��,���Ϊ8���ֽ�.
//����ֵ:0,�ɹ�;
//����,ʧ��;
u8 Can_Send_Msg(u8* msg,u8 len)
{	
//	u8 mbox;
//	u16 i=0;
//	CanTxMsg TxMessage;
//	TxMessage.StdId=0x12;			// ��׼��ʶ�� 
//	TxMessage.ExtId=0x00;			// ������չ��ʾ�� 
//	TxMessage.IDE=CAN_Id_Standard; 	// ��׼֡
//	TxMessage.RTR=CAN_RTR_Remote;	// Զ��֡
//	TxMessage.DLC=0;				// Ҫ���͵����ݳ���		          
//	CAN_Transmit(CAN1, &TxMessage);   
//	i=0; 
//	while((CAN_TransmitStatus(CAN1, mbox)==CAN_TxStatus_Failed)&&(i<0XFFF))i++;	//�ȴ����ͽ���
//	if(i>=0XFFF)return 1;
//	return 0;
	
	u8 mbox;
	u16 i=0;
	CanTxMsg TxMessage;
	TxMessage.StdId=Read_ID;	// ��׼��ʶ�� 
	TxMessage.ExtId=0x00;			// ������չ��ʾ�� 
	TxMessage.IDE=CAN_Id_Standard; 	// ��׼֡
	TxMessage.RTR=CAN_RTR_Data;		// ����֡
	TxMessage.DLC=len;				// Ҫ���͵����ݳ���
	for(i=0;i<len;i++)
	TxMessage.Data[i]=msg[i];			          
	mbox= CAN_Transmit(CAN1, &TxMessage);   
	i=0; 
	while((CAN_TransmitStatus(CAN1, mbox)==CAN_TxStatus_Failed)&&(i<0XFFF))i++;	//�ȴ����ͽ���
	if(i>=0XFFF)return 1;
	return 0;	 
}
//can�ڽ������ݲ�ѯ
//buf:���ݻ�����;	 
//����ֵ:0,�����ݱ��յ�;
//����,���յ����ݳ���;
u8 Can_Receive_Msg(u8 *buf)
{		   		   
 	u32 i;
	CanRxMsg RxMessage;
    if( CAN_MessagePending(CAN1,CAN_FIFO0)==0)return 0;		//û�н��յ�����,ֱ���˳� 
    CAN_Receive(CAN1, CAN_FIFO0, &RxMessage);//��ȡ����	
    for(i=0;i<8;i++)
    buf[i]=RxMessage.Data[i];  
	return RxMessage.DLC;	
}
u8 cansend[3]={0,0,0};
void send(void)
{
	//cansend[0]=led[7]|led[6]<<1|led[5]<<2|led[4]<<3|led[3]<<4|led[2]<<5|led[1]<<6|led[0]<<7;
	Can_Send_Msg(cansend,3);
}
