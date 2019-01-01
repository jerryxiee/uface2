#include "uart.h"
#include "FM175XX_REG.h"
#include "FM175XX.h"

#include "gpio.h"
#include <unistd.h>
#include <stdio.h>


#define REST   229

void FM175XX_HPD(unsigned char mode)  //mode = 1 �˳�HPDģʽ ��mode = 0 ����HPDģʽ
{
	if(mode == 0x0)
	{
		usleep(1000);
		gpio_setValue(REST,0);
	/*mDelay(1);//��ʱ1ms,�ȴ�֮ǰ�Ĳ�������
	GPIO_ResetBits(PORT_NRST,PIN_NRST);//NPD = 0	����HPDģʽ

	GPIO_InitStructure.GPIO_Pin = PIN_SCK | PIN_MISO | PIN_MOSI; 
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; //GPIO����Ϊ���״̬
	GPIO_Init(PORT_SPI, &GPIO_InitStructure); 
		
	GPIO_ResetBits(PORT_SPI,PIN_SCK | PIN_MISO | PIN_MOSI);//SCK = 0��MISO = 0��MOSI = 0
		
	GPIO_InitStructure.GPIO_Pin = PIN_NSS;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 
	GPIO_Init(PORT_SPI, &GPIO_InitStructure);	
	GPIO_SetBits(PORT_SPI,PIN_NSS);	//NSS = 1;	*/		
	}
	else
	{
		//usleep(1000);
		gpio_setValue(REST,1);
		usleep(1000);
	/*GPIO_SetBits(PORT_NRST,PIN_NRST);	//NPD = 1 �˳�HPDģʽ
	mDelay(1);//��ʱ1ms���ȴ�FM175XX����
		
	GPIO_InitStructure.GPIO_Pin = PIN_SCK | PIN_MISO | PIN_MOSI; 
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;//GPIO����ΪSPI���� 
	GPIO_Init(PORT_SPI, &GPIO_InitStructure);  
		
	GPIO_InitStructure.GPIO_Pin = PIN_NSS;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 
	GPIO_Init(PORT_SPI, &GPIO_InitStructure);	
	GPIO_SetBits(PORT_SPI,PIN_NSS);	//NSS = 1;		*/		
	}
	return;
}


//***********************************************
//�������ƣ�Read_Reg(unsigned char addr,unsigned char *regdata)
//�������ܣ���ȡ�Ĵ���ֵ
//��ڲ�����addr:Ŀ��Ĵ�����ַ   regdata:��ȡ��ֵ
//���ڲ�����unsigned char  TRUE����ȡ�ɹ�   FALSE:ʧ��
//***********************************************
void GetReg(unsigned char addr,unsigned char *regdata)
{	
	//	GPIO_ResetBits(PORT_SPI,PIN_NSS);	//NSS = 0;
		//addr = (addr << 1) | 0x80;	   
		addr = addr | 0x80;	   
		
		uart_senddat(addr);   /* Send SPI1 data */ 
		//while(SPI_GetFlagStatus(SPI1, SPI_FLAG_TXE)==RESET);	  /* Wait for SPI1 Tx buffer empty */ 
		//while(SPI_GetFlagStatus(SPI2, SPI_FLAG_RXNE)==RESET);
		//SPI_ClearFlag(SPI2,SPI_FLAG_RXNE);	
		*regdata = uart_read();		 /* Read SPI1 received data */
		//SPI_ClearFlag(SPI2, SPI_FLAG_RXNE); 
		//uart_senddat(0x00);   /* Send SPI1 data */
		//while(SPI_GetFlagStatus(SPI1, SPI_FLAG_TXE)==RESET)      
		//while(SPI_GetFlagStatus(SPI2, SPI_FLAG_RXNE)==RESET);
		//SPI_ClearFlag(SPI2,SPI_FLAG_RXNE);	
		//*regdata = uart_read();		 /* Read SPI1 received data */

	//	GPIO_SetBits(PORT_SPI,PIN_NSS);	//NSS = 1;	
	return ;
}

//***********************************************
//�������ƣ�Write_Reg(unsigned char addr,unsigned char* regdata)
//�������ܣ�д�Ĵ���
//��ڲ�����addr:Ŀ��Ĵ�����ַ   regdata:Ҫд���ֵ
//���ڲ�����unsigned char  TRUE��д�ɹ�   FALSE:дʧ��
//***********************************************
void SetReg(unsigned char addr,unsigned char regdata)
{	
		//	GPIO_ResetBits(PORT_SPI,PIN_NSS);	//NSS = 0;
			//addr = (addr << 1) & 0x7F;	
			addr = addr & 0x7F;			
	
			uart_senddat(addr);   /* Send SPI1 data */ 
			//while(SPI_GetFlagStatus(SPI1, SPI_FLAG_TXE)==RESET);	  /* Wait for SPI1 Tx buffer empty */ 
			//while(SPI_GetFlagStatus(SPI2, SPI_FLAG_RXNE)==RESET);
			//SPI_ClearFlag(SPI2,SPI_FLAG_RXNE);	
			uart_read();		 /* Read SPI1 received data */; 
			uart_senddat(regdata);   /* Send SPI1 data */
			//while(SPI_GetFlagStatus(SPI1, SPI_FLAG_TXE)==RESET);		
    	//while(SPI_GetFlagStatus(SPI2, SPI_FLAG_RXNE)==RESET);
		//	SPI_ClearFlag(SPI2,SPI_FLAG_RXNE);	
		//	uart_read();		 /* Read SPI1 received data */
	
		//	GPIO_SetBits(PORT_SPI,PIN_NSS);	//NSS = 1;
	
	return ;
}

//*******************************************************
//�������ƣ�ModifyReg(unsigned char addr,unsigned char* mask,unsigned char set)
//�������ܣ�д�Ĵ���
//��ڲ�����addr:Ŀ��Ĵ�����ַ   mask:Ҫ�ı��λ  
//         set:  0:��־��λ����   ����:��־��λ����
//���ڲ�����
//********************************************************
void ModifyReg(unsigned char addr,unsigned char mask,unsigned char set)
{
	unsigned char regdata;
	
	GetReg(addr,&regdata);
	
		if(set)
		{
			regdata |= mask;
		}
		else
		{
			regdata &= ~mask;
		}

	SetReg(addr,regdata);
	return ;
}

void SPI_Write_FIFO(unsigned char reglen,unsigned char* regbuf)  //SPI�ӿ�����дFIFO�Ĵ��� 
{
	unsigned char  i;
	unsigned char regdata;

	//GPIO_ResetBits(PORT_SPI,PIN_NSS);	//NSS = 0;
	uart_senddat(0x12);   /* Send FIFO addr 0x09<<1 */ 
	//while(SPI_GetFlagStatus(SPI2, SPI_FLAG_RXNE) == RESET);
	regdata = uart_read();		 /* not care data */
	for(i = 0;i < reglen;i++)
	{
		regdata = *(regbuf+i);	  //RegData_i
		uart_senddat(regdata);   /* Send addr_i i��1*/
		///while(SPI_GetFlagStatus(SPI2, SPI_FLAG_RXNE) == RESET);
    	//regdata = uart_read();		 /* not care data */
	}
	//GPIO_SetBits(PORT_SPI,PIN_NSS);	//NSS = 1;

	return;
}

void SPI_Read_FIFO(unsigned char reglen,unsigned char* regbuf)  //SPI�ӿ�������FIFO
{
	unsigned char  i;

	//GPIO_ResetBits(PORT_SPI,PIN_NSS);	//NSS = 0;
	uart_senddat(0x92);   /* Send FIFO addr 0x09<<1|0x80 */ 
	//while(SPI_GetFlagStatus(SPI2, SPI_FLAG_RXNE)==RESET);
	*(regbuf) = uart_read();		 /* not care data */
	
	for(i=1;i<reglen;i++)
	{
		uart_senddat(0x92);   /* Send FIFO addr 0x09<<1|0x80 */ 
		//while(SPI_GetFlagStatus(SPI2, SPI_FLAG_RXNE)==RESET);
		*(regbuf+i-1) = uart_read();  //Data_i-1
	} 
	uart_senddat(0x00);   /* Send recvEnd data 0x00 */      
    //while(SPI_GetFlagStatus(SPI2, SPI_FLAG_RXNE)==RESET);
    *(regbuf+i-1) = uart_read();		 /* Read SPI1 received data */

	//GPIO_SetBits(PORT_SPI,PIN_NSS);	//NSS = 1;

	return;
}


void Clear_FIFO(void)
{
	unsigned char regdata;
		
	GetReg(JREG_FIFOLEVEL,&regdata);
	if((regdata & 0x7F) != 0)			//FIFO������գ���FLUSH FIFO
	{
	SetReg(JREG_FIFOLEVEL,JBIT_FLUSHFIFO);
	}
	return ;
}

void FM175XX_HardReset(void)
{	
/*	GPIO_ResetBits(PORT_ADDR,PIN_I2C);	//I2C = 0 ѡ��SPI�ӿ�ģʽ
	GPIO_SetBits(PORT_ADDR,PIN_EA);			//EA = 1 ѡ��SPI�ӿ�ģʽ
	GPIO_SetBits(PORT_SPI,PIN_NSS);//NSS = 1
	FM175XX_HPD(0);//NPD=0
	mDelay(1);		
	FM175XX_HPD(1);//NPD=1	
	mDelay(1);*/
	gpio_setValue(REST,0);
	usleep(10000);
	gpio_setValue(REST,1);
	usleep(10000);
	printf("FM175XX_HardReset\n");
	return;
}

void rest_init()
{
	gpio_export(REST);
	gpio_setDir(REST,1);
	gpio_setValue(REST,1);
}

//***********************************************
//�������ƣ�GetReg_Ext(unsigned char ExtRegAddr,unsigned char* ExtRegData)
//�������ܣ���ȡ��չ�Ĵ���ֵ
//��ڲ�����ExtRegAddr:��չ�Ĵ�����ַ   ExtRegData:��ȡ��ֵ
//���ڲ�����unsigned char  TRUE����ȡ�ɹ�   FALSE:ʧ��
//***********************************************
void GetReg_Ext(unsigned char ExtRegAddr,unsigned char* ExtRegData)
{

	SetReg(JREG_EXT_REG_ENTRANCE,JBIT_EXT_REG_RD_ADDR + ExtRegAddr);
	GetReg(JREG_EXT_REG_ENTRANCE,&(*ExtRegData));
	return;	
}

//***********************************************
//�������ƣ�SetReg_Ext(unsigned char ExtRegAddr,unsigned char* ExtRegData)
//�������ܣ�д��չ�Ĵ���
//��ڲ�����ExtRegAddr:��չ�Ĵ�����ַ   ExtRegData:Ҫд���ֵ
//���ڲ�����unsigned char  TRUE��д�ɹ�   FALSE:дʧ��
//***********************************************
void SetReg_Ext(unsigned char ExtRegAddr,unsigned char ExtRegData)
{

	SetReg(JREG_EXT_REG_ENTRANCE,JBIT_EXT_REG_WR_ADDR + ExtRegAddr);
	SetReg(JREG_EXT_REG_ENTRANCE,JBIT_EXT_REG_WR_DATA + ExtRegData);
	return; 	
}

//*******************************************************
//�������ƣ�ModifyReg_Ext(unsigned char ExtRegAddr,unsigned char* mask,unsigned char set)
//�������ܣ��Ĵ���λ����
//��ڲ�����ExtRegAddr:Ŀ��Ĵ�����ַ   mask:Ҫ�ı��λ  
//         set:  0:��־��λ����   ����:��־��λ����
//���ڲ�����unsigned char  TRUE��д�ɹ�   FALSE:дʧ��
//********************************************************
void ModifyReg_Ext(unsigned char ExtRegAddr,unsigned char mask,unsigned char set)
{
  unsigned char regdata;
	
	GetReg_Ext(ExtRegAddr,&regdata);
	
		if(set)
		{
			regdata |= mask;
		}
		else
		{
			regdata &= ~(mask);
		}
	
	SetReg_Ext(ExtRegAddr,regdata);
	return;
}

unsigned char FM175XX_SoftReset(void)
{	
	unsigned char reg_data;
	SetReg(JREG_COMMAND,CMD_SOFT_RESET);
	//mDelay(1);//FM175XXоƬ��λ��Ҫ1ms
	usleep(1000);
	GetReg(JREG_COMMAND,&reg_data);
	if(reg_data == 0x20)
		return SUCCESS;
	else
		return ERROR;
}








