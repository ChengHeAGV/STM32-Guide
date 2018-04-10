#include "flash.h"
#include "stm32f10x_flash.h"                     //flash操作接口文件（在库文件中），必须要包含



FLASH_FLAG FLASH_STATUS;
u32 address=0x8000000+0x10000-200;
static u32 count = 0;
/**---------------------------------------------------------------------------------
  * @brief   FLASH写入必不可少步骤
  * @param  输入数据起始地址
  * @retval 函数有无执行完毕
  --------------------------------------------------------------------------------*/
unsigned char FLASH_WRITE(unsigned short int * memory_data,u16 t)
{	
	count = 0;
	/*Enables or disables the Internal High Speed oscillator (HSI).*/
	RCC_HSICmd(ENABLE);	
	/*打开FLASH可擦除控制器*/
	FLASH_Unlock();
	/*将flash三个标志位全清*/
	FLASH_ClearFlag(FLASH_FLAG_EOP|FLASH_FLAG_PGERR|FLASH_FLAG_WRPRTERR);
	FLASH_ErasePage(address);
	while(count <= t)
	{
		/*flash 为一个字节存储，16位数据必须地址加2*/
		FLASH_ProgramHalfWord((address +count*2),*(memory_data+count)); 
		count++;
		if(count > t)
		{
			FLASH_STATUS = FLASH_WRIKE_NO;
			return FLASH_STATUS;
		}
	}
	/* Locks the FLASH Program Erase Controller.*/
	FLASH_Lock(); 
	RCC_HSICmd(DISABLE);
	FLASH_STATUS = FLASH_WRITE_OK;
	return FLASH_STATUS;
}

/**---------------------------------------------------------------------------------
  * @brief   FLASH读出函数
  * @param  memory_data：输出存放地址，n 输出个数
  * @retval 函数有无执行完毕
  --------------------------------------------------------------------------------*/
unsigned char FLASH_READ(unsigned short int * memory_data,unsigned short int n)
{	
	count = 0;
	while(count < n)
	{
	  *(memory_data+count) = *(u16 *)(address + count*2);
		//printf("\r %d \r",*(memory_data+count));   //读取
		count++;
		if(count > n)
		{
			FLASH_STATUS = FLASH_READ_NO;	
			return FLASH_STATUS;
		}
	}
	//FLASH_ErasePage(address);
	FLASH_STATUS = FLASH_READ_OK;
	return FLASH_STATUS;
}

/*********************************************END OF FILE**********************/




#define  STARTADDR  0x08010000                   	 //STM32F103RB 其他型号基本适用，未测试 
volatile FLASH_Status FLASHStatus = FLASH_COMPLETE;      //Flash操作状态变量


/****************************************************************
*Name:		ReadFlashNBtye
*Function:	从内部Flash读取N字节数据
*Input:		ReadAddress：数据地址（偏移地址）ReadBuf：数据指针	ReadNum：读取字节数
*Output:	读取的字节数  
*Author:    ValerianFan
*Date:		2014/04/09
*E-Mail:	fanwenjingnihao@163.com
*Other:		
****************************************************************/
int ReadFlashNBtye(uint32_t ReadAddress, uint16_t *ReadBuf, int32_t ReadNum) 
{
        int DataNum = 0;
		ReadAddress = (uint32_t)STARTADDR + ReadAddress; 
        while(DataNum < ReadNum) 
		{
           *(ReadBuf + DataNum) = *(__IO uint16_t*) ReadAddress++;
           DataNum++;
        }
        return DataNum;
}

/****************************************************************
*Name:		WriteFlashOneWord
*Function:	向内部Flash写入16位数据
*Input:		WriteAddress：数据地址（偏移地址）WriteData：写入数据
*Output:	NULL 
*Author:    ValerianFan
*Date:		2014/04/09
*E-Mail:	fanwenjingnihao@163.com
*Other:		
****************************************************************/

void WriteFlashOneWord(uint32_t WriteAddress,uint16_t WriteData)
{
	FLASH_UnlockBank1();
	FLASH_ClearFlag(FLASH_FLAG_EOP | FLASH_FLAG_PGERR | FLASH_FLAG_WRPRTERR); 
    FLASHStatus = FLASH_ErasePage(STARTADDR);

	if(FLASHStatus == FLASH_COMPLETE)
	{
		FLASHStatus = FLASH_ProgramHalfWord(STARTADDR + WriteAddress, WriteData);    //flash.c 中API函数
	}
	FLASH_LockBank1();
} 
