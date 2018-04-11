#include "flash.h"
#include "stm32f10x_flash.h"                     //flash�����ӿ��ļ����ڿ��ļ��У�������Ҫ����



FLASH_FLAG FLASH_STATUS;
u32 address=0x8000000+0x20000-200;
static u32 count = 0;
/**---------------------------------------------------------------------------------
  * @brief   FLASHд��ز����ٲ���
  * @param  ����������ʼ��ַ
  * @retval ��������ִ�����
  --------------------------------------------------------------------------------*/
unsigned char FLASH_WRITE(unsigned short int * memory_data,u16 t)
{	
	count = 0;
	/*Enables or disables the Internal High Speed oscillator (HSI).*/
	RCC_HSICmd(ENABLE);	
	/*��FLASH�ɲ���������*/
	FLASH_Unlock();
	/*��flash������־λȫ��*/
	FLASH_ClearFlag(FLASH_FLAG_EOP|FLASH_FLAG_PGERR|FLASH_FLAG_WRPRTERR);
	FLASH_ErasePage(address);
	while(count <= t)
	{
		/*flash Ϊһ���ֽڴ洢��16λ���ݱ����ַ��2*/
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
  * @brief   FLASH��������
  * @param  memory_data�������ŵ�ַ��n �������
  * @retval ��������ִ�����
  --------------------------------------------------------------------------------*/
unsigned char FLASH_READ(unsigned short int * memory_data,unsigned short int n)
{	
	count = 0;
	while(count < n)
	{
	  *(memory_data+count) = *(u16 *)(address + count*2);
		//printf("\r %d \r",*(memory_data+count));   //��ȡ
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




#define  STARTADDR  0x08010000                   	 //STM32F103RB �����ͺŻ������ã�δ���� 
volatile FLASH_Status FLASHStatus = FLASH_COMPLETE;      //Flash����״̬����


/****************************************************************
*Name:		ReadFlashNBtye
*Function:	���ڲ�Flash��ȡN�ֽ�����
*Input:		ReadAddress�����ݵ�ַ��ƫ�Ƶ�ַ��ReadBuf������ָ��	ReadNum����ȡ�ֽ���
*Output:	��ȡ���ֽ���  
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
*Function:	���ڲ�Flashд��16λ����
*Input:		WriteAddress�����ݵ�ַ��ƫ�Ƶ�ַ��WriteData��д������
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
		FLASHStatus = FLASH_ProgramHalfWord(STARTADDR + WriteAddress, WriteData);    //flash.c ��API����
	}
	FLASH_LockBank1();
} 
