#include "dma.h"
#include "sys.h"
#include "Modbus_slave_170M.h"
u8 PLC2_InPut[64];//PLC输入状态寄存器
u8 PLC2_OutPut[64];//PLC输出状态寄存器
u16 PLC2_Data[64]={1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1};//PLC数据寄存器

u16 sendlength2;	
void AnalyzeRecieve2()                              //分析并生成响应报文
 {
	uint16_t _crc;					  
	uint8_t a1,a2;	
	send3_buf[0]=0x04;	 
	_crc=CalcCrc2(rece3_buf,(rece3_index-2));  //计算校验CRC
	a1=_crc&0xff;                          //CRC低字节
	a2=(_crc>>8)&0xff;                     //CRC高字节
	if(a1==rece3_buf[rece3_index-2]&&a2==rece3_buf[rece3_index-1])  //校验正确
	{				
		switch(rece3_buf[1])
		{
			case 0x01:Recirve2_01();break;                    //读位状态 
			case 0x02:Recirve2_02();break;                    //读输入位状态	ok				      
			case 0x03:Recirve2_03();break;	                   //读寄存器
			case 0x04:Recirve2_04();break;                    //读输入寄存器
			case 0x05:Recirve2_05();break;                    //写单个位状态ok
			case 0x06:Recirve2_06();break;                    //写单个寄存器
			case 0x10:Recirve2_10();break;                    //写多个寄存器
			default:errorsend2(rece3_buf[1],0x04);break;      //不支持的功能码		
		}	   
	} 
	else                                              //返回校验错误代码 
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

 /*从机响应函数*/

void send2_comm( void )
{
	Uart1_Start_DMA_Tx(sendlength2);
}

/*分析01功能码报文，产生响应报文*/ 

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
	startadd=(startaddH<<8)+startaddL;  //要返回的起始地址               
	bit_num=(bit_numH<<8)+bit_numL;	   //要读的字节数量,单位是位
	if((startadd+bit_num)>=64)                //最多允许32个位，从第4个位开始读
	 {
	   errorsend2(0x01,0x02);     //响应寄存器数量超出范围
	  }
	else
	{
	send3_buf[1]=0x01;           //功能码
	if((bit_num%8)==0)
	send3_buf[2]=(bit_num)/8;      //要返回的字节数
	else
	 send3_buf[2]=((bit_num)/8)+1;  //不能整除8的时候要多返回一个字节
	for(i=0;i<send3_buf[2];i++)				  
	 {
	   send3_buf[3+i]=0;                       //先清零复位
	   for(j=0;j<8;j++)                  //每8个位状态组成一个字节返回
	   {
		 send3_buf[3+i]=(uint8_t)((PLC2_OutPut[startadd+i*8+j]&0x01)<<j)+send3_buf[3+i];     //低位在前，高位在后
		 
		}	                       
	 }
	aakj=CalcCrc2(send3_buf,(send3_buf[2]+3));                 //CRC校验
	send3_buf[3+send3_buf[2]]=(uint8_t)(aakj&0xff);                   //CRC低字节
	send3_buf[4+send3_buf[2]]=(uint8_t)((aakj>>8)&0xff);              //CRC高字节
	sendlength2=send3_buf[2]+5; 
	}
 } 
	  
/*分析02功能码报文，产生响应报文*/ 
void Recirve2_02()// 搞定****************************************************
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
	startadd=(startaddH<<8)+startaddL;  //要返回的起始地址               
	bit_num=(bit_numH<<8)+bit_numL;	   //要读的字节数量,单位是位
	if((startadd+bit_num)>=64||startadd>11)   //本案例中只有4个输入位可供读
	  {
	   errorsend2(0x01,0x02);                //响应寄存器数量超出范围
	  }
	else
	{
		send3_buf[1]=0x02;           //功能码
		if((bit_num%8)==0)
		send3_buf[2]=(bit_num)/8;      //要返回的字节数
		else
		send3_buf[2]=((bit_num)/8)+1;  //不能整除8的时候要多返回一个字节
		for(i=0;i<send3_buf[2];i++)				  
		 {
		   send3_buf[3+i]=0;                       //先清零复位
		   for(j=0;j<8;j++)                  //每8个位状态组成一个字节返回
		   {
			 send3_buf[3+i]=(uint8_t)((PLC2_InPut[startadd+i*8+j]&0x01)<<j)+send3_buf[3+i];     //低位在前，高位在后
			 
			}	                       
		}
		aakj=CalcCrc2(send3_buf,(send3_buf[2]+3));                 //CRC校验
		send3_buf[3+send3_buf[2]]=(uint8_t)(aakj&0xff);                   //CRC低字节
		send3_buf[4+send3_buf[2]]=(uint8_t)((aakj>>8)&0xff);              //CRC高字节
		sendlength2=send3_buf[2]+5; 
	}
 } 
	

/*分析03功能码报文，产生响应报文*/ 
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
	startadd=(startaddH<<8)+startaddL;  //要返回的起始地址                 
	length=(lengthH<<8)+lengthL;	   //要读的字节数量
	if((startadd+length)>64)                //最多只能返回32个寄存器,64个字节，注意返回的长度不能超过发送数组长度，否则会溢出导致错误
	 {
	   errorsend2(0x03,0x02);     //响应寄存器数量超出范围
	  }
	else
	{
	send3_buf[1]=0x03;           //功能码
	send3_buf[2]=length*2;      //要返回的字节数是请求报文的第五个字节*2
	for(i=0;i<length;i++)
	 {		 
		 send3_buf[3+i*2]=(PLC2_Data[startadd+i]>>8)&0xff;   //返回寄存器值的高字节
		 send3_buf[4+i*2]=PLC2_Data[startadd+i]&0xff;        //返回寄存器值得低字节
	 }
	aakj=CalcCrc2(send3_buf,(length*2)+3);                 //CRC校验
	send3_buf[3+length*2]=(uint8_t)(aakj&0xff);                   //CRC低字节
	send3_buf[4+length*2]=(uint8_t)((aakj>>8)&0xff);              //CRC高字节
	sendlength2=(length*2)+5;    
	}
 }
 
 /*分析04功能码报文，产生响应报文*/ 
 /*这边返回的是输入口的寄存器值*/

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
	startadd=(startaddH<<8)+startaddL;  //要返回的起始地址               
	bit_num=(bit_numH<<8)+bit_numL;	   //要读的字节数量,单位是位
	if((startadd+bit_num)>=64||startadd>1)//本案例中只有4个输入位1个寄存器可供读
	 {
	   errorsend2(0x01,0x02);     //响应寄存器数量超出范围
	  }
	else
	{
	for(i=2;i<6;i++)
	for(i=4;i<32;i++)
	PLC2_InPut[i]=0;                              //没有位状态，清零
	 
	send3_buf[1]=0x04;           //功能码
	send3_buf[2]=bit_num*2;
	for(i=0;i<send3_buf[2];i++)				  
	 {
	   send3_buf[3+i]=0;                       //先清零复位
	   for(j=0;j<8;j++)                  //每8个位状态组成一个字节返回
	   {
		 send3_buf[3+i]=(uint8_t)((PLC2_InPut[startadd+i*8+j]&0x01)<<j)+send3_buf[3+i];     //低位在前，高位在后
		 
		}	                       
	 }
	aakj=CalcCrc2(send3_buf,(send3_buf[2]+3));                 //CRC校验
	send3_buf[3+send3_buf[2]]=(uint8_t)(aakj&0xff);                   //CRC低字节
	send3_buf[4+send3_buf[2]]=(uint8_t)((aakj>>8)&0xff);              //CRC高字节
	sendlength2=send3_buf[2]+5; 
	}
 }  

/*分析05功能码报文，产生响应报文*/ 

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
	startadd=(startaddH<<8)+startaddL;  //要写入的地址               
	if(startadd>=64)               
	 {
	   errorsend2(0x01,0x02);     //响应寄存器数量超出范围
	  }
	else
	{
	 
	 if(bit_valueH==0xff&&bit_valueL==0x00)   //置位线圈
	 
		 {
			 PLC2_OutPut[startadd] = 1;
			 }
	 if(bit_valueH==0x00&&bit_valueL==0x00)   //复位线圈
	 {
		 PLC2_OutPut[startadd] = 0;
	 }
		send3_buf[1]=0x05;           //功能码
		send3_buf[2]=startaddH;     //地址高字节
		send3_buf[3]=startaddL;     //地址低字节
		send3_buf[4]=bit_valueH;     //地址高字节
		send3_buf[5]=bit_valueL;     //地址低字节
		aakj=CalcCrc2(send3_buf,6);                 //CRC校验
		send3_buf[6]=(uint8_t)(aakj&0xff);                   //CRC低字节
		send3_buf[7]=(uint8_t)((aakj>>8)&0xff);              //CRC高字节
		sendlength2=8; 
	}
 } 

   


/*分析06功能码报文，产生响应报文*/ 
void Recirve2_06()//单字节写入
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
	startadd=(startaddH<<8)+startaddL;  //要写入的起始地址                 
	wdata_06=(wdataH_06<<8)+wdataL_06;	           //要写入的数值

	if(startadd>64)                    //寄存器地址超出范围
		errorsend2(0x06,0x02);         //响应寄存器数量超出范围
	else if(wdata_06>0xFFFF)
		errorsend2(0x06,0x03);         //响应数据错误
	else
	{	 
		PLC2_Data[startadd]=wdata_06;      //将数值写入寄存器
		send3_buf[1]=0x06;           //功能码
		send3_buf[2]=startaddH;      //返回地址高字节
		send3_buf[3]=startaddL;      //返回地址低字节
		send3_buf[4]=(uint8_t)(((PLC2_Data[startadd])>>8)&0xff);  //返回寄存器值高字节
		send3_buf[5]=(uint8_t)(PLC2_Data[startadd]&0xff);         //返回寄存器值低字节
		aakj=CalcCrc2(send3_buf,6);                 //CRC校验
		send3_buf[6]=(uint8_t)(aakj&0xff);                   //CRC低字节
		send3_buf[7]=(uint8_t)((aakj>>8)&0xff);              //CRC高字节
		sendlength2=8;                                  //返回8个字节长度     
	}
 }

/*分析10功能码报文，产生响应报文*/ 
void Recirve2_10()//接收到数据
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
	startadd=(startaddH<<8)+startaddL;  //要返回的起始地址 
	length=rece3_buf[6];	                               //要写的字节数量

	if((startadd+(length/2))>64)                //最多允许写32个寄存器
	 {
	   errorsend2(0x10,0x02);     //响应寄存器数量超出范围
	  }
	else
	{
	for(i=0;i<(length/2);i++)        //将值写入寄存器
	 {
	   PLC2_Data[startadd+i]=(rece3_buf[7+i*2]<<8)+rece3_buf[8+i*2]&0xff;	 	 
	 }
	send3_buf[1]=0x10;           //功能码
	send3_buf[2]=startaddH;      //返回地址高位
	send3_buf[3]=startaddL;      //返回地址低位
	send3_buf[4]=register_numH;
	send3_buf[5]=register_numL;
	aakj=CalcCrc2(send3_buf,6);                 //CRC校验
	send3_buf[6]=(uint8_t)(aakj&0xff);                   //CRC低字节
	send3_buf[7]=(uint8_t)((aakj>>8)&0xff);              //CRC高字节
	sendlength2=8;    
	}
 } 



/*错误返回*/

void errorsend2(uint8_t func,uint8_t type)
{
	uint16_t _crc;
	uint8_t crcH,crcL;

	switch(type)
	 {
	   case 0x08: 
			  send3_buf[1]=0x80+func;      //返回错误功能码
			  send3_buf[2]=0x08;           //返回错误代码，08：CRC校验错误
			  break;
	   case 0x01: 
			  send3_buf[1]=0x80+func;      //返回错误功能码
			  send3_buf[2]=0x01;           //返回错误代码，01：功能码错误
			  break;
	   case 0x02: 
			  send3_buf[1]=0x80+func;      //返回错误功能码
			  send3_buf[2]=0x02;           //返回错误代码，02：地址错误
			  break;
	   case 0x03: 
			  send3_buf[1]=0x80+func;      //返回错误功能码
			  send3_buf[2]=0x03;           //返回错误代码，03：数据错误
			  break;
	   case 0x04: 
			  send3_buf[0]=0x80+func;      //返回错误功能码
			  send3_buf[1]=0x04;           //返回错误代码，04：不支持的功能码
			  break;	 
	 }
	_crc=CalcCrc2(send3_buf,3);
	crcH=(uint8_t)((_crc>>8)&0xff);
	crcL=(uint8_t)(_crc&0xff);
	send3_buf[3]=crcL;                      //校验低字节
	send3_buf[4]=crcH;                      //校验高字节
	sendlength2=5;
}

 
/*************************************************
crc16校验码计算函数,计算算法：
1、设置crc校验寄存器的初值为0xffff;
2、将报文的第一个字节与crc寄存器的低字节异或，结果存入crc寄存器
3、判断crc的第一位是否为1，如果是1,crc右移1位后和0xa001异或，如果为0，则再移1位；
4、重复步骤3，直到完成8个位；
5、重复步骤2、3、4直到完成所有字节
6、返回计算结果
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
