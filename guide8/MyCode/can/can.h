#ifndef __CAN_H
#define __CAN_H	 
#include "sys.h"	    
 
//CAN接收RX0中断使能
#define CAN_RX0_INT_ENABLE	1		//0,不使能;1,使能.								    
										 					
extern u8 Read_ID;

u8 CAN_Mode_Init(u8 tsjw,u8 tbs2,u8 tbs1,u16 brp,u8 mode);//CAN初始化
 
u8 Can_Send_Msg(u8* msg,u8 len);						//发送数据

u8 Can_Receive_Msg(u8 *buf);							//接收数据

void send(void);

extern u8 cansend[1];
extern u16 adc1[8];//第一次上电AD值
extern u16 adc2[8];//当前AD值
extern u16 tempmem[10];//内存
extern u16 led[8];

#endif
