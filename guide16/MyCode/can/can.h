#ifndef __CAN_H
#define __CAN_H	 
#include "sys.h"	    
 
//CAN����RX0�ж�ʹ��
#define CAN_RX0_INT_ENABLE	1		//0,��ʹ��;1,ʹ��.								    
										 					
extern u8 Read_ID;

u8 CAN_Mode_Init(u8 tsjw,u8 tbs2,u8 tbs1,u16 brp,u8 mode);//CAN��ʼ��
 
u8 Can_Send_Msg(u8* msg,u8 len);						//��������

u8 Can_Receive_Msg(u8 *buf);							//��������

void send(void);

extern u8 cansend[2];
extern u16 adc1[16];//��һ���ϵ�ADֵ
extern u16 adc2[16];//��ǰADֵ
extern u16 tempmem[20];//�ڴ�
extern u16 led[16];
extern u16 threshold;//��ֵ
#endif
