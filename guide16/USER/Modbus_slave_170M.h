#ifndef __Modbus_slave_170M_H
#define	__Modbus_slave_170M_H	 
#include "stm32f10x.h"

void AnalyzeRecieve2(void);                              
void send2_comm(void);
void Recirve2_01(void);
void Recirve2_02(void);
void Recirve2_03(void);
void Recirve2_04(void);
void Recirve2_05(void);
void Recirve2_06(void);
void Recirve2_10(void);
void errorsend2(u8 func,u8 type);
u16 CalcCrc2(unsigned char *chData,unsigned short uNo);
//extern u8  Reciver_bit;//����λ��һ
extern u8 PLC2_InPut[64];//PLC����״̬�Ĵ���
extern u8 PLC2_OutPut[64];//PLC���״̬�Ĵ���
extern u16 PLC2_Data[64];//PLC���ݼĴ���
#endif
