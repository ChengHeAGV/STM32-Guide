#include "dma.h"
#include "sys.h"
#include "Modbus_slave_170M.h"
u8 PLC2_InPut[64];//PLC����״̬�Ĵ���
u8 PLC2_OutPut[64];//PLC���״̬�Ĵ���
u16 PLC2_Data[64]={1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1};//PLC���ݼĴ���

u16 sendlength2;	
void AnalyzeRecieve2()                              //������������Ӧ����
 {
	uint16_t _crc;					  
	uint8_t a1,a2;	
	send3_buf[0]=0x04;	 
	_crc=CalcCrc2(rece3_buf,(rece3_index-2));  //����У��CRC
	a1=_crc&0xff;                          //CRC���ֽ�
	a2=(_crc>>8)&0xff;                     //CRC���ֽ�
	if(a1==rece3_buf[rece3_index-2]&&a2==rece3_buf[rece3_index-1])  //У����ȷ
	{				
		switch(rece3_buf[1])
		{
			case 0x01:Recirve2_01();break;                    //��λ״̬ 
			case 0x02:Recirve2_02();break;                    //������λ״̬	ok				      
			case 0x03:Recirve2_03();break;	                   //���Ĵ���
			case 0x04:Recirve2_04();break;                    //������Ĵ���
			case 0x05:Recirve2_05();break;                    //д����λ״̬ok
			case 0x06:Recirve2_06();break;                    //д�����Ĵ���
			case 0x10:Recirve2_10();break;                    //д����Ĵ���
			default:errorsend2(rece3_buf[1],0x04);break;      //��֧�ֵĹ�����		
		}	   
	} 
	else                                              //����У�������� 
	{
	 switch(rece3_buf[1])
	   {
		 case 0x01:errorsend2(0x01,0x08);break; 
		 case 0x02:errorsend2(0x02,0x08);break;
		 case 0x03:errorsend2(0x03,0x08);break;
		 case 0x04:errorsend2(0x04,0x08);break;
		 case 0x05:errorsend2(0x05,0x08);break;
		 case 0x06:errorsend2(0x06,0x08);break;
		 case 0x10:errorsend2(0x10,0x08);break;       		   
	   }	   
	}   
 }

 /*�ӻ���Ӧ����*/

void send2_comm( void )
{
	Uart1_Start_DMA_Tx(sendlength2);
}

/*����01�����뱨�ģ�������Ӧ����*/ 

void Recirve2_01()
 {
   
	uint16_t startadd;
	uint16_t bit_num;
	uint8_t startaddH,startaddL;
	uint8_t bit_numH,bit_numL;
	uint16_t i,j;
	uint16_t aakj;

	startaddH=rece3_buf[2];
	startaddL=rece3_buf[3];
	bit_numH=rece3_buf[4];
	bit_numL=rece3_buf[5];
	startadd=(startaddH<<8)+startaddL;  //Ҫ���ص���ʼ��ַ               
	bit_num=(bit_numH<<8)+bit_numL;	   //Ҫ�����ֽ�����,��λ��λ
	if((startadd+bit_num)>=64)                //�������32��λ���ӵ�4��λ��ʼ��
	 {
	   errorsend2(0x01,0x02);     //��Ӧ�Ĵ�������������Χ
	  }
	else
	{
	send3_buf[1]=0x01;           //������
	if((bit_num%8)==0)
	send3_buf[2]=(bit_num)/8;      //Ҫ���ص��ֽ���
	else
	 send3_buf[2]=((bit_num)/8)+1;  //��������8��ʱ��Ҫ�෵��һ���ֽ�
	for(i=0;i<send3_buf[2];i++)				  
	 {
	   send3_buf[3+i]=0;                       //�����㸴λ
	   for(j=0;j<8;j++)                  //ÿ8��λ״̬���һ���ֽڷ���
	   {
		 send3_buf[3+i]=(uint8_t)((PLC2_OutPut[startadd+i*8+j]&0x01)<<j)+send3_buf[3+i];     //��λ��ǰ����λ�ں�
		 
		}	                       
	 }
	aakj=CalcCrc2(send3_buf,(send3_buf[2]+3));                 //CRCУ��
	send3_buf[3+send3_buf[2]]=(uint8_t)(aakj&0xff);                   //CRC���ֽ�
	send3_buf[4+send3_buf[2]]=(uint8_t)((aakj>>8)&0xff);              //CRC���ֽ�
	sendlength2=send3_buf[2]+5; 
	}
 } 
	  
/*����02�����뱨�ģ�������Ӧ����*/ 
void Recirve2_02()// �㶨****************************************************
 {

	uint16_t startadd;
	uint16_t bit_num;
	uint8_t startaddH,startaddL;
	uint8_t bit_numH,bit_numL;
	uint16_t i,j;
	uint16_t aakj; 
	startaddH=rece3_buf[2];
	startaddL=rece3_buf[3];
	bit_numH=rece3_buf[4];
	bit_numL=rece3_buf[5];
	startadd=(startaddH<<8)+startaddL;  //Ҫ���ص���ʼ��ַ               
	bit_num=(bit_numH<<8)+bit_numL;	   //Ҫ�����ֽ�����,��λ��λ
	if((startadd+bit_num)>=64||startadd>11)   //��������ֻ��4������λ�ɹ���
	  {
	   errorsend2(0x01,0x02);                //��Ӧ�Ĵ�������������Χ
	  }
	else
	{
		send3_buf[1]=0x02;           //������
		if((bit_num%8)==0)
		send3_buf[2]=(bit_num)/8;      //Ҫ���ص��ֽ���
		else
		send3_buf[2]=((bit_num)/8)+1;  //��������8��ʱ��Ҫ�෵��һ���ֽ�
		for(i=0;i<send3_buf[2];i++)				  
		 {
		   send3_buf[3+i]=0;                       //�����㸴λ
		   for(j=0;j<8;j++)                  //ÿ8��λ״̬���һ���ֽڷ���
		   {
			 send3_buf[3+i]=(uint8_t)((PLC2_InPut[startadd+i*8+j]&0x01)<<j)+send3_buf[3+i];     //��λ��ǰ����λ�ں�
			 
			}	                       
		}
		aakj=CalcCrc2(send3_buf,(send3_buf[2]+3));                 //CRCУ��
		send3_buf[3+send3_buf[2]]=(uint8_t)(aakj&0xff);                   //CRC���ֽ�
		send3_buf[4+send3_buf[2]]=(uint8_t)((aakj>>8)&0xff);              //CRC���ֽ�
		sendlength2=send3_buf[2]+5; 
	}
 } 
	

/*����03�����뱨�ģ�������Ӧ����*/ 
void Recirve2_03()
 {

	uint16_t startadd;
	uint16_t length;
	uint8_t startaddH,startaddL;
	uint8_t lengthH,lengthL;
	uint16_t i;
	uint16_t aakj;
	 
	startaddH=rece3_buf[2];
	startaddL=rece3_buf[3];
	lengthH=rece3_buf[4];
	lengthL=rece3_buf[5];
	startadd=(startaddH<<8)+startaddL;  //Ҫ���ص���ʼ��ַ                 
	length=(lengthH<<8)+lengthL;	   //Ҫ�����ֽ�����
	if((startadd+length)>64)                //���ֻ�ܷ���32���Ĵ���,64���ֽڣ�ע�ⷵ�صĳ��Ȳ��ܳ����������鳤�ȣ������������´���
	 {
	   errorsend2(0x03,0x02);     //��Ӧ�Ĵ�������������Χ
	  }
	else
	{
	send3_buf[1]=0x03;           //������
	send3_buf[2]=length*2;      //Ҫ���ص��ֽ����������ĵĵ�����ֽ�*2
	for(i=0;i<length;i++)
	 {		 
		 send3_buf[3+i*2]=(PLC2_Data[startadd+i]>>8)&0xff;   //���ؼĴ���ֵ�ĸ��ֽ�
		 send3_buf[4+i*2]=PLC2_Data[startadd+i]&0xff;        //���ؼĴ���ֵ�õ��ֽ�
	 }
	aakj=CalcCrc2(send3_buf,(length*2)+3);                 //CRCУ��
	send3_buf[3+length*2]=(uint8_t)(aakj&0xff);                   //CRC���ֽ�
	send3_buf[4+length*2]=(uint8_t)((aakj>>8)&0xff);              //CRC���ֽ�
	sendlength2=(length*2)+5;    
	}
 }
 
 /*����04�����뱨�ģ�������Ӧ����*/ 
 /*��߷��ص�������ڵļĴ���ֵ*/

void Recirve2_04()
 {

	uint16_t startadd;
	uint16_t bit_num;
	uint8_t startaddH,startaddL;
	uint8_t bit_numH,bit_numL;
	uint16_t i,j;
	uint16_t aakj;
	startaddH=rece3_buf[2];
	startaddL=rece3_buf[3];
	bit_numH=rece3_buf[4];
	bit_numL=rece3_buf[5];
	startadd=(startaddH<<8)+startaddL;  //Ҫ���ص���ʼ��ַ               
	bit_num=(bit_numH<<8)+bit_numL;	   //Ҫ�����ֽ�����,��λ��λ
	if((startadd+bit_num)>=64||startadd>1)//��������ֻ��4������λ1���Ĵ����ɹ���
	 {
	   errorsend2(0x01,0x02);     //��Ӧ�Ĵ�������������Χ
	  }
	else
	{
	for(i=2;i<6;i++)
	for(i=4;i<32;i++)
	PLC2_InPut[i]=0;                              //û��λ״̬������
	 
	send3_buf[1]=0x04;           //������
	send3_buf[2]=bit_num*2;
	for(i=0;i<send3_buf[2];i++)				  
	 {
	   send3_buf[3+i]=0;                       //�����㸴λ
	   for(j=0;j<8;j++)                  //ÿ8��λ״̬���һ���ֽڷ���
	   {
		 send3_buf[3+i]=(uint8_t)((PLC2_InPut[startadd+i*8+j]&0x01)<<j)+send3_buf[3+i];     //��λ��ǰ����λ�ں�
		 
		}	                       
	 }
	aakj=CalcCrc2(send3_buf,(send3_buf[2]+3));                 //CRCУ��
	send3_buf[3+send3_buf[2]]=(uint8_t)(aakj&0xff);                   //CRC���ֽ�
	send3_buf[4+send3_buf[2]]=(uint8_t)((aakj>>8)&0xff);              //CRC���ֽ�
	sendlength2=send3_buf[2]+5; 
	}
 }  

/*����05�����뱨�ģ�������Ӧ����*/ 

void Recirve2_05()
 {

	uint16_t startadd;
	uint8_t startaddH,startaddL;
	uint8_t bit_valueH,bit_valueL;
	uint16_t aakj;
	startaddH=rece3_buf[2];
	startaddL=rece3_buf[3];
	bit_valueH=rece3_buf[4];
	bit_valueL=rece3_buf[5];
	startadd=(startaddH<<8)+startaddL;  //Ҫд��ĵ�ַ               
	if(startadd>=64)               
	 {
	   errorsend2(0x01,0x02);     //��Ӧ�Ĵ�������������Χ
	  }
	else
	{
	 
	 if(bit_valueH==0xff&&bit_valueL==0x00)   //��λ��Ȧ
	 
		 {
			 PLC2_OutPut[startadd] = 1;
			 }
	 if(bit_valueH==0x00&&bit_valueL==0x00)   //��λ��Ȧ
	 {
		 PLC2_OutPut[startadd] = 0;
	 }
		send3_buf[1]=0x05;           //������
		send3_buf[2]=startaddH;     //��ַ���ֽ�
		send3_buf[3]=startaddL;     //��ַ���ֽ�
		send3_buf[4]=bit_valueH;     //��ַ���ֽ�
		send3_buf[5]=bit_valueL;     //��ַ���ֽ�
		aakj=CalcCrc2(send3_buf,6);                 //CRCУ��
		send3_buf[6]=(uint8_t)(aakj&0xff);                   //CRC���ֽ�
		send3_buf[7]=(uint8_t)((aakj>>8)&0xff);              //CRC���ֽ�
		sendlength2=8; 
	}
 } 

   


/*����06�����뱨�ģ�������Ӧ����*/ 
void Recirve2_06()//���ֽ�д��
 {

	uint16_t startadd;
	uint16_t wdata_06;
	uint8_t startaddH,startaddL;
	uint8_t wdataH_06,wdataL_06;
	uint16_t aakj;
	startaddH=rece3_buf[2];
	startaddL=rece3_buf[3];
	wdataH_06=rece3_buf[4];
	wdataL_06=rece3_buf[5];
	startadd=(startaddH<<8)+startaddL;  //Ҫд�����ʼ��ַ                 
	wdata_06=(wdataH_06<<8)+wdataL_06;	           //Ҫд�����ֵ

	if(startadd>64)                    //�Ĵ�����ַ������Χ
		errorsend2(0x06,0x02);         //��Ӧ�Ĵ�������������Χ
	else if(wdata_06>0xFFFF)
		errorsend2(0x06,0x03);         //��Ӧ���ݴ���
	else
	{	 
		PLC2_Data[startadd]=wdata_06;      //����ֵд��Ĵ���
		send3_buf[1]=0x06;           //������
		send3_buf[2]=startaddH;      //���ص�ַ���ֽ�
		send3_buf[3]=startaddL;      //���ص�ַ���ֽ�
		send3_buf[4]=(uint8_t)(((PLC2_Data[startadd])>>8)&0xff);  //���ؼĴ���ֵ���ֽ�
		send3_buf[5]=(uint8_t)(PLC2_Data[startadd]&0xff);         //���ؼĴ���ֵ���ֽ�
		aakj=CalcCrc2(send3_buf,6);                 //CRCУ��
		send3_buf[6]=(uint8_t)(aakj&0xff);                   //CRC���ֽ�
		send3_buf[7]=(uint8_t)((aakj>>8)&0xff);              //CRC���ֽ�
		sendlength2=8;                                  //����8���ֽڳ���     
	}
 }

/*����10�����뱨�ģ�������Ӧ����*/ 
void Recirve2_10()//���յ�����
 {

	uint16_t startadd;  
	uint8_t startaddH,startaddL;
	uint8_t register_numH,register_numL;
	uint8_t length;
	uint16_t i;
	uint16_t aakj;
	startaddH=rece3_buf[2];
	startaddL=rece3_buf[3];
	register_numH=rece3_buf[4];
	register_numL=rece3_buf[5];
	startadd=(startaddH<<8)+startaddL;  //Ҫ���ص���ʼ��ַ 
	length=rece3_buf[6];	                               //Ҫд���ֽ�����

	if((startadd+(length/2))>64)                //�������д32���Ĵ���
	 {
	   errorsend2(0x10,0x02);     //��Ӧ�Ĵ�������������Χ
	  }
	else
	{
	for(i=0;i<(length/2);i++)        //��ֵд��Ĵ���
	 {
	   PLC2_Data[startadd+i]=(rece3_buf[7+i*2]<<8)+rece3_buf[8+i*2]&0xff;	 	 
	 }
	send3_buf[1]=0x10;           //������
	send3_buf[2]=startaddH;      //���ص�ַ��λ
	send3_buf[3]=startaddL;      //���ص�ַ��λ
	send3_buf[4]=register_numH;
	send3_buf[5]=register_numL;
	aakj=CalcCrc2(send3_buf,6);                 //CRCУ��
	send3_buf[6]=(uint8_t)(aakj&0xff);                   //CRC���ֽ�
	send3_buf[7]=(uint8_t)((aakj>>8)&0xff);              //CRC���ֽ�
	sendlength2=8;    
	}
 } 



/*���󷵻�*/

void errorsend2(uint8_t func,uint8_t type)
{
	uint16_t _crc;
	uint8_t crcH,crcL;

	switch(type)
	 {
	   case 0x08: 
			  send3_buf[1]=0x80+func;      //���ش�������
			  send3_buf[2]=0x08;           //���ش�����룬08��CRCУ�����
			  break;
	   case 0x01: 
			  send3_buf[1]=0x80+func;      //���ش�������
			  send3_buf[2]=0x01;           //���ش�����룬01�����������
			  break;
	   case 0x02: 
			  send3_buf[1]=0x80+func;      //���ش�������
			  send3_buf[2]=0x02;           //���ش�����룬02����ַ����
			  break;
	   case 0x03: 
			  send3_buf[1]=0x80+func;      //���ش�������
			  send3_buf[2]=0x03;           //���ش�����룬03�����ݴ���
			  break;
	   case 0x04: 
			  send3_buf[0]=0x80+func;      //���ش�������
			  send3_buf[1]=0x04;           //���ش�����룬04����֧�ֵĹ�����
			  break;	 
	 }
	_crc=CalcCrc2(send3_buf,3);
	crcH=(uint8_t)((_crc>>8)&0xff);
	crcL=(uint8_t)(_crc&0xff);
	send3_buf[3]=crcL;                      //У����ֽ�
	send3_buf[4]=crcH;                      //У����ֽ�
	sendlength2=5;
}

 
/*************************************************
crc16У������㺯��,�����㷨��
1������crcУ��Ĵ����ĳ�ֵΪ0xffff;
2�������ĵĵ�һ���ֽ���crc�Ĵ����ĵ��ֽ���򣬽������crc�Ĵ���
3���ж�crc�ĵ�һλ�Ƿ�Ϊ1�������1,crc����1λ���0xa001������Ϊ0��������1λ��
4���ظ�����3��ֱ�����8��λ��
5���ظ�����2��3��4ֱ����������ֽ�
6�����ؼ�����
***********************************************/
uint16_t CalcCrc2(unsigned char *chData,unsigned short uNo)
{
	uint16_t crc=0xffff;
	uint16_t i,j;
	for(i=0;i<uNo;i++)
	{
	  crc^=chData[i];
	  for(j=0;j<8;j++)
	  {
		if(crc&1)
		{
		 crc>>=1;
		 crc^=0xA001;
		}
		else
		 crc>>=1;
	  }
	}
	return (crc);
}
