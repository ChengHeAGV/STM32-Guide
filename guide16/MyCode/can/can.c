#include "can.h"
#include "led.h"
#include "delay.h"
#include "usart.h"
#include "flash.h"
#include "adc.h"
#include "stm32f10x_flash.h"

u8 num=0;

//u16 adc1[16];//默认参数
u16 adc2[16];
u16 tempmem[20];//内存
u16 led[16];
u16 threshold=0;//阈值
u8 Read_ID=0;
//CAN初始化
//tsjw:重新同步跳跃时间单元.范围:CAN_SJW_1tq~ CAN_SJW_4tq
//tbs2:时间段2的时间单元.   范围:CAN_BS2_1tq~CAN_BS2_8tq;
//tbs1:时间段1的时间单元.   范围:CAN_BS1_1tq ~CAN_BS1_16tq
//brp :波特率分频器.范围:1~1024;  tq=(brp)*tpclk1
//波特率=Fpclk1/((tbs1+1+tbs2+1+1)*brp);
//mode:CAN_Mode_Normal,普通模式;CAN_Mode_LoopBack,回环模式;
//Fpclk1的时钟在初始化的时候设置为36M,如果设置CAN_Mode_Init(CAN_SJW_1tq,CAN_BS2_8tq,CAN_BS1_9tq,4,CAN_Mode_LoopBack);
//则波特率为:36M/((8+9+1)*4)=500Kbps
//返回值:0,初始化OK;
//其他,初始化失败; 
u8 CAN_Mode_Init(u8 tsjw,u8 tbs2,u8 tbs1,u16 brp,u8 mode)
{ 
	GPIO_InitTypeDef 		GPIO_InitStructure; 
	CAN_InitTypeDef        	CAN_InitStructure;
	CAN_FilterInitTypeDef  	CAN_FilterInitStructure;
#if CAN_RX0_INT_ENABLE 
	NVIC_InitTypeDef  		NVIC_InitStructure;
#endif
                											 
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_CAN1, ENABLE);//使能CAN1时钟
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;	//上拉输�
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);			//初始化IO

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//复用推挽
	GPIO_Init(GPIOA, &GPIO_InitStructure);			//初始化IO

	//GPIO_PinRemapConfig(GPIO_Remap1_CAN1, ENABLE);//端口重映射
	/* CAN register init */  
	CAN_DeInit(CAN1);
	CAN_StructInit(&CAN_InitStructure);
	//CAN单元设置
	CAN_InitStructure.CAN_TTCM=DISABLE;			//非时间触发通信模式  
	CAN_InitStructure.CAN_ABOM=DISABLE;			//软件自动离线管理	 
	CAN_InitStructure.CAN_AWUM=DISABLE;			//睡眠模式通过软件唤醒(清除CAN->MCR的SLEEP位)
	CAN_InitStructure.CAN_NART=ENABLE;			//禁止报文自动传送 
	CAN_InitStructure.CAN_RFLM=DISABLE;		 	//报文不锁定,新的覆盖旧的  
	CAN_InitStructure.CAN_TXFP=DISABLE;			//优先级由报文标识符决定 
	CAN_InitStructure.CAN_Mode= mode;	        //模式设置： mode:0,普通模式;1,回环模式; 
	//设置波特率
	CAN_InitStructure.CAN_SJW=tsjw;				//重新同步跳跃宽度(Tsjw)为tsjw+1个时间单位  CAN_SJW_1tq	 CAN_SJW_2tq CAN_SJW_3tq CAN_SJW_4tq
	CAN_InitStructure.CAN_BS1=tbs1; 			//Tbs1=tbs1+1个时间单位CAN_BS1_1tq ~CAN_BS1_16tq
	CAN_InitStructure.CAN_BS2=tbs2;				//Tbs2=tbs2+1个时间单位CAN_BS2_1tq ~	CAN_BS2_8tq
	CAN_InitStructure.CAN_Prescaler=brp;        //分频系数(Fdiv)为brp+1	
	CAN_Init(CAN1, &CAN_InitStructure);        	//初始化CAN1 

//	CAN_FilterInitStructure.CAN_FilterNumber=0;	//过滤器0
//	CAN_FilterInitStructure.CAN_FilterMode=CAN_FilterMode_IdMask; 	//屏蔽位模式
//	CAN_FilterInitStructure.CAN_FilterScale=CAN_FilterScale_32bit; 	//32位宽 
//	CAN_FilterInitStructure.CAN_FilterIdHigh=0x0220;	//32位ID，标准帧ID为11位，ID为0x10时，二进制00010000，补全11位为00000010000，对应十六进制为0x0200.
//	CAN_FilterInitStructure.CAN_FilterIdLow=0x0002;		//设置接收的为标准遥控（远程）帧，
//	CAN_FilterInitStructure.CAN_FilterMaskIdHigh=0xffe0;//32位MASK，为1即选中该位，选中11位ID
//	CAN_FilterInitStructure.CAN_FilterMaskIdLow=0x0006;//选中标准遥控帧
//	CAN_FilterInitStructure.CAN_FilterFIFOAssignment=CAN_Filter_FIFO0;//过滤器0关联到FIFO0
//	CAN_FilterInitStructure.CAN_FilterActivation=ENABLE;//激活过滤器0

	CAN_FilterInitStructure.CAN_FilterNumber=0;	//过滤器0
	CAN_FilterInitStructure.CAN_FilterMode=CAN_FilterMode_IdMask; 	//屏蔽位模式
	CAN_FilterInitStructure.CAN_FilterScale=CAN_FilterScale_32bit; 	//32位宽 
	CAN_FilterInitStructure.CAN_FilterIdHigh=0x0000;	//32位ID
	CAN_FilterInitStructure.CAN_FilterIdLow=0x0000;
	CAN_FilterInitStructure.CAN_FilterMaskIdHigh=0x0000;//32位MASK
	CAN_FilterInitStructure.CAN_FilterMaskIdLow=0x0000;
	CAN_FilterInitStructure.CAN_FilterFIFOAssignment=CAN_Filter_FIFO0;//过滤器0关联到FIFO0
	CAN_FilterInitStructure.CAN_FilterActivation=ENABLE;//激活过滤器0


	CAN_FilterInit(&CAN_FilterInitStructure);			//滤波器初始化
	
#if CAN_RX0_INT_ENABLE 
	CAN_ITConfig(CAN1,CAN_IT_FMP0,ENABLE);				//FIFO0消息挂号中断允许.		    
 
	NVIC_InitStructure.NVIC_IRQChannel = USB_LP_CAN1_RX0_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;     // 主优先级为1
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;            // 次优先级为0
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
#endif
	return 0;
}   
 
#if CAN_RX0_INT_ENABLE	//使能RX0中断

//中断服务函数			    
void USB_LP_CAN1_RX0_IRQHandler(void)
{
	  u8 buf[2];
  	CanRxMsg RxMessage;
    CAN_Receive(CAN1, 0, &RxMessage);
	
		if(RxMessage.StdId==127)//当是磁导航修改器的数据来临时
		{
			//ID
			if(RxMessage.Data[0]==1) 
			{
				if(RxMessage.Data[1]==2) //设置
				{
					Read_ID = RxMessage.Data[2];//更新ID
					tempmem[17] = Read_ID;
					FLASH_WRITE(tempmem, 20);
				}
			}
			//阈值
			else if(RxMessage.Data[0]==3) 
			{
				if(RxMessage.Data[1]==1) //查询
				{
					buf[0] = 3;
					buf[1] = threshold;
					Can_Send_Msg(buf,2);
				}
				else
				if(RxMessage.Data[1]==2) //设置
				{
					threshold = RxMessage.Data[2];//更新阈值
					tempmem[18] = threshold;
					FLASH_WRITE(tempmem, 20);
				}
			}
			//磁导航默认AD值
			else if(RxMessage.Data[0]==4) 
			{
				if(RxMessage.Data[1]==2) //设置
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

//can发送一组数据(固定格式:ID为0X12,标准帧,数据帧)	
//len:数据长度(最大为8)				     
//msg:数据指针,最大为8个字节.
//返回值:0,成功;
//其他,失败;
u8 Can_Send_Msg(u8* msg,u8 len)
{	
//	u8 mbox;
//	u16 i=0;
//	CanTxMsg TxMessage;
//	TxMessage.StdId=0x12;			// 标准标识符 
//	TxMessage.ExtId=0x00;			// 设置扩展标示符 
//	TxMessage.IDE=CAN_Id_Standard; 	// 标准帧
//	TxMessage.RTR=CAN_RTR_Remote;	// 远程帧
//	TxMessage.DLC=0;				// 要发送的数据长度		          
//	CAN_Transmit(CAN1, &TxMessage);   
//	i=0; 
//	while((CAN_TransmitStatus(CAN1, mbox)==CAN_TxStatus_Failed)&&(i<0XFFF))i++;	//等待发送结束
//	if(i>=0XFFF)return 1;
//	return 0;
	
	u8 mbox;
	u16 i=0;
	CanTxMsg TxMessage;
	TxMessage.StdId=Read_ID;	// 标准标识符 
	TxMessage.ExtId=0x00;			// 设置扩展标示符 
	TxMessage.IDE=CAN_Id_Standard; 	// 标准帧
	TxMessage.RTR=CAN_RTR_Data;		// 数据帧
	TxMessage.DLC=len;				// 要发送的数据长度
	for(i=0;i<len;i++)
	TxMessage.Data[i]=msg[i];			          
	mbox= CAN_Transmit(CAN1, &TxMessage);   
	i=0; 
	while((CAN_TransmitStatus(CAN1, mbox)==CAN_TxStatus_Failed)&&(i<0XFFF))i++;	//等待发送结束
	if(i>=0XFFF)return 1;
	return 0;	 
}
//can口接收数据查询
//buf:数据缓存区;	 
//返回值:0,无数据被收到;
//其他,接收的数据长度;
u8 Can_Receive_Msg(u8 *buf)
{		   		   
 	u32 i;
	CanRxMsg RxMessage;
    if( CAN_MessagePending(CAN1,CAN_FIFO0)==0)return 0;		//没有接收到数据,直接退出 
    CAN_Receive(CAN1, CAN_FIFO0, &RxMessage);//读取数据	
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
