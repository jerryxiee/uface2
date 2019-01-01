/****************************************Copyright (c)****************************************************
**                            Guangzhou ZLGMCU Technology Co., LTD
**
**                                 http://www.zlgmcu.com
**
**      ������������Ƭ���Ƽ����޹�˾���ṩ�����з�������ּ��Э���ͻ����ٲ�Ʒ���з����ȣ��ڷ�����������ṩ
**  ���κγ����ĵ������Խ����������֧�ֵ����Ϻ���Ϣ���������ο����ͻ���Ȩ��ʹ�û����вο��޸ģ�����˾��
**  �ṩ�κε������ԡ��ɿ��Եȱ�֤�����ڿͻ�ʹ�ù��������κ�ԭ����ɵ��ر�ġ�żȻ�Ļ��ӵ���ʧ������˾��
**  �е��κ����Ρ�
**                                                                        ����������������Ƭ���Ƽ����޹�˾
**
**--------------File Info---------------------------------------------------------------------------------
** File name:           type_a.c
** Last modified Date:  2016-3-14
** Last Version:        V1.00
** Descriptions:        ISO/IEC144443A����ز���
**
**--------------------------------------------------------------------------------------------------------
*/

#include <stdint.h>
#include "fm175xx.h"
#include "Utility.h"
#include <string.h>    

#include "uart.h"
#include "gpio.h"
#include <unistd.h>

//#define TRUE 1
//#define FALSE -1

unsigned char FM175XX_IRQ;

#define MI_NOTAGERR 0xEE
#define MAXRLEN 64

#define REST   229

/*
void Delay100us(int time)
{
	usleep(100*time);
}*/

void rest_init()
{
	gpio_export(REST);
	gpio_setDir(REST,1);
	gpio_setValue(REST,1);
}


/*********************************************************************************************************
** Function name:       pcd_Init
** Descriptions:        MCU��ʼ������������SPI��UART�ĳ�ʼ��
** input parameters:    N/A
** output parameters:   N/A
** Returned value:      
*********************************************************************************************************/
void pcd_Init(void)
{
    //SPI_Init();
    //CD_CfgTPD();                                                        /* ���ø�λ�ܽ�                 */
	
	rest_init();
	uart_init(9600,8,'N',1);
}

static unsigned char spi_GetReg(unsigned char Reg)
{
	unsigned char addr;
	
	addr = Reg | 0x80;
	uart_senddat(addr);
	
	return uart_read();
	
}

static unsigned char spi_SetReg(unsigned char Reg, unsigned char value)
{
	unsigned char addr;
	
	addr = Reg & 0x7F;	
	uart_senddat(addr);
	uart_read();
	uart_senddat(value);
	return 0;
}
/*********************************************************************************************************
** Function name:       MCU_TO_PCD_TEST
** Descriptions:        MCU�����оƬͨ�Ų��Ժ���
**                      ��ͬ�Ķ���оƬ��ѡȡ���ڲ��ԵļĴ�����һ������Ҫ���߾���оƬ��ȷ��Ŀ��Ĵ���
** input parameters:    N/A
** output parameters:   N/A
** Returned value:      TRUE---����ͨ��    FALSE----����ʧ��
*********************************************************************************************************/
uint8_t MCU_TO_PCD_TEST(void)
{
    volatile uint8_t ucRegVal;
    pcd_RST();                                                      /* Ӳ����λ                         */
    ucRegVal = spi_GetReg(ControlReg);
    spi_SetReg(ControlReg, 0x10);                                   /* ������д��ģʽ                   */
    ucRegVal = spi_GetReg(ControlReg);
    spi_SetReg(GsNReg, 0xF0 | 0x04);                                /* CWGsN = 0xF; ModGsN = 0x4        */
    ucRegVal = spi_GetReg(GsNReg);
    if(ucRegVal != 0xF4)                                            /* ��֤�ӿ���ȷ                     */
        return FALSE;
    return TRUE;
}


/*********************************************************************************************************
** Function name:       pcd_RST
** Descriptions:        PCD����PICCӲ����λ����
** input parameters:    N/A
** output parameters:   N/A
** Returned value:      �Ĵ�����ֵ
*********************************************************************************************************/
void pcd_RST(void)
{
    /*CD_SetTPD();
    Delay100us(3);
    CD_ClrTPD();                                                        // ��λ�ܽ�����                 
    Delay100us(5);
    CD_SetTPD();                                                        // ������                       
    Delay100us(10);*/
	
	gpio_setValue(REST,0);
	usleep(10000);
	gpio_setValue(REST,1);
	usleep(10000);
	
	return ;
}


/*********************************************************************************************************
** Function name:       Read_Reg
** Descriptions:        ��ȡ�Ĵ���                
** input parameters:    reg_add:�Ĵ�����ֵ
** output parameters:   N/A
** Returned value:      �Ĵ�����ֵ
*********************************************************************************************************/
unsigned char Read_Reg(unsigned char reg_add)
{
    unsigned char  reg_value;       
    reg_value=spi_GetReg(reg_add);
    return reg_value;
}

/*********************************************************************************************************
** Function name:       Read_Reg_All
** Descriptions:        ��ȡȫ���Ĵ���                
** input parameters:    reg_value:�Ĵ�����ֵ
** output parameters:   N/A
** Returned value:      TRUE�������ɹ� ERROR������ʧ��    
*********************************************************************************************************/
unsigned char Read_Reg_All(unsigned char *reg_value)
{
    unsigned char data i;
    for (i=0;i<64;i++)       
        *(reg_value+i) = spi_GetReg(i);
    return TRUE;
}


/*********************************************************************************************************
** Function name:       Write_Reg
** Descriptions:        д�Ĵ�������                
** input parameters:    reg_add:�Ĵ�����ַ
**                      reg_value:�Ĵ�����ֵ
** output parameters:   N/A
** Returned value:      TRUE�������ɹ� ERROR������ʧ��    
*********************************************************************************************************/
unsigned char Write_Reg(unsigned char reg_add,unsigned char reg_value)
{
    spi_SetReg(reg_add,reg_value);
    return TRUE;
}


static void SPIRead_Sequence(unsigned char sequence_length,unsigned char ucRegAddr,unsigned char *reg_value)    
{
    uint8_t i;
    if (sequence_length==0)
		return;
    
    for(i=0;i<sequence_length;i++) {
        *(reg_value+i) = spi_GetReg(ucRegAddr);
    }
   
    return;

}

static void SPIWrite_Sequence(unsigned char sequence_length,unsigned char ucRegAddr,unsigned char *reg_value)
{
    
    uint8_t i;
    if(sequence_length==0)
        return;
    for(i=0;i<sequence_length;i++) {
        spi_SetReg(ucRegAddr, *(reg_value+i));
    }
 
    return ;    
}

/*********************************************************************************************************
** Function name:       Read_FIFO
** Descriptions:        ����FIFO������         
** input parameters:    length:��ȡ���ݳ���
**                      *fifo_data:���ݴ��ָ��
** output parameters:   N/A
** Returned value:      TRUE�������ɹ� ERROR������ʧ��    
*********************************************************************************************************/
void Read_FIFO(unsigned char length,unsigned char *fifo_data)
{     
    SPIRead_Sequence(length,FIFODataReg,fifo_data);
    return;
}


/*********************************************************************************************************
** Function name:       Write_FIFO
** Descriptions:        д��FIFO         
** input parameters:    length:��ȡ���ݳ���
**                      *fifo_data:���ݴ��ָ��
** output parameters:   N/A
** Returned value:      TRUE�������ɹ� ERROR������ʧ��    
*********************************************************************************************************/
void Write_FIFO(unsigned char length,unsigned char *fifo_data)
{
    SPIWrite_Sequence(length,FIFODataReg,fifo_data);
    return;
}


/*********************************************************************************************************
** Function name:       Clear_FIFO
** Descriptions:        ���FIFO              
** input parameters:   
** output parameters:   N/A
** Returned value:      TRUE�������ɹ� ERROR������ʧ��    
*********************************************************************************************************/
unsigned char Clear_FIFO(void)
{
    Set_BitMask(FIFOLevelReg,0x80);                                     /* ���FIFO����                 */
    if ( spi_GetReg(FIFOLevelReg) == 0 )
        return TRUE;
    else
        return FALSE;
}


/*********************************************************************************************************
** Function name:       Set_BitMask
** Descriptions:        ��λ�Ĵ�������    
** input parameters:    reg_add���Ĵ�����ַ
**                      mask���Ĵ������λ
** output parameters:   N/A
** Returned value:      TRUE�������ɹ� ERROR������ʧ��    
*********************************************************************************************************/
unsigned char Set_BitMask(unsigned char reg_add,unsigned char mask)
{
    unsigned char result;
    result=spi_SetReg(reg_add,Read_Reg(reg_add) | mask);                /* set bit mask                 */
    return result;
}

/*********************************************************************************************************
** Function name:       Clear_BitMask
** Descriptions:        ���λ�Ĵ�������
** input parameters:    reg_add���Ĵ�����ַ
**                      mask���Ĵ������λ
** output parameters:   N/A
** Returned value:      TRUE�������ɹ� ERROR������ʧ��    
*********************************************************************************************************/
unsigned char Clear_BitMask(unsigned char reg_add,unsigned char mask)
{
    unsigned char result;
    result=Write_Reg(reg_add,Read_Reg(reg_add) & ~mask);                /* clear bit mask               */
    return result;
}


/*********************************************************************************************************
** Function name:       Set_RF
** Descriptions:        ������Ƶ���
** input parameters:    mode����Ƶ���ģʽ
**                      0���ر����
**                      1������TX1���
**                      2������TX2���
**                      3��TX1��TX2�������TX2Ϊ�������
** output parameters:   N/A
** Returned value:      TRUE�������ɹ� ERROR������ʧ��    
*********************************************************************************************************/
unsigned char Set_Rf(unsigned char mode)
{
    unsigned char result;
    if( (Read_Reg(TxControlReg)&0x03) == mode )
        return TRUE;
    if( mode == 0 )
        result = Clear_BitMask(TxControlReg,0x03);                      /* �ر�TX1��TX2���             */
    if( mode== 1 )
        result = Clear_BitMask(TxControlReg,0x01);                      /* ����TX1���                */
    if( mode == 2)
        result = Clear_BitMask(TxControlReg,0x02);                      /* ����TX2���                */
    if (mode==3)
        result=Set_BitMask(TxControlReg,0x03);                          /* ��TX1��TX2���             */
    Delay100us(1000);
    return result;
}
 
/*********************************************************************************************************
** Function name:       Pcd_Comm
** Descriptions:        ������ͨ�� ������IRQ�ܽŵ����
** input parameters:    Command:ͨ�Ų�������
**                      pInData:������������
**                      InLenByte:�������������ֽڳ���
**                      pOutData:������������
**                      pOutLenBit:�������ݵ�λ����
** output parameters:   N/A
** Returned value:      TRUE�������ɹ� ERROR������ʧ��    
*********************************************************************************************************/
unsigned char Pcd_Comm(    unsigned char Command, 
                         unsigned char *pInData, 
                         unsigned char InLenByte,
                         unsigned char *pOutData, 
                         unsigned int *pOutLenBit)
{
    uint8_t status  = FALSE;
    uint8_t irqEn   = 0x00;                                             /* ʹ�ܵ��ж�                   */
    uint8_t waitFor = 0x00;                                             /* �ȴ����ж�                   */
    uint8_t lastBits;
    uint8_t n;
    uint32_t i;
    Write_Reg(ComIrqReg, 0x7F);                                         /* ���IRQ���                  */
    Write_Reg(TModeReg,0x80);                                           /* ����TIMER�Զ�����            */
    switch (Command) {
    case MFAuthent:                                                     /* Mifare��֤                   */
        irqEn   = 0x12;
        waitFor = 0x10;
        break;
    case Transceive:                                       /* ����FIFO�е����ݵ����ߣ�����󼤻���յ�·*/
        irqEn   = 0x77;
        waitFor = 0x30;
        break;
    default:
        break;
    }
   
    Write_Reg(ComIEnReg, irqEn | 0x80);
   // Clear_BitMask(ComIrqReg, 0x80);
    Write_Reg(CommandReg, Idle);
    Set_BitMask(FIFOLevelReg, 0x80);
    
    for (i=0; i < InLenByte; i++) {
        Write_Reg(FIFODataReg, pInData[i]);
    }
    Write_Reg(CommandReg, Command);

    if (Command == Transceive) {
        Set_BitMask(BitFramingReg, 0x80);
    }

    i = 3000;                                              /* ����ʱ��Ƶ�ʵ���������M1�����ȴ�ʱ��25ms*/

    do {
        n = Read_Reg(ComIrqReg);
        i--;                                                            /* i==0��ʾ��ʱ����             */
    } while ((i != 0) && !(n & 0x01) && !(n & waitFor));            /* n&0x01!=1��ʾPCDsettimerʱ��δ�� */
                                                                        /* n&waitFor!=1��ʾָ��ִ����� */
    Clear_BitMask(BitFramingReg, 0x80);
    if (i != 0) {
        if(!(Read_Reg(ErrorReg) & 0x1B)) {
            status = TRUE;
            if (n & irqEn & 0x01) {
                status = MI_NOTAGERR;
            }
            if (Command == Transceive) {
                n = Read_Reg(FIFOLevelReg);
                lastBits = Read_Reg(ControlReg) & 0x07;
                if (lastBits) {
                    *pOutLenBit = (n - 1) * 8 + lastBits;
                } else {
                    *pOutLenBit = n * 8;
                }
                if (n == 0) {
                    n = 1;
                }
                if (n > MAXRLEN) {
                    n = MAXRLEN;
                }
                for (i = 0; i < n; i++) {
                    pOutData[i] = Read_Reg(FIFODataReg);
                }
            }
        } else {
            status = FALSE;
        }
    }
       Clear_BitMask(BitFramingReg,0x80);//�رշ���
    return status;
}


/*********************************************************************************************************
** Function name:       Pcd_SetTimer
** Descriptions:        ���ý�����ʱ
** input parameters:    delaytime����ʱʱ�䣨��λΪ���룩  
** output parameters:   N/A
** Returned value:      TRUE�������ɹ� ERROR������ʧ��    
*********************************************************************************************************/
unsigned char Pcd_SetTimer(unsigned long delaytime)
{
    unsigned long  TimeReload;
    unsigned int Prescaler;

    Prescaler=0;
    TimeReload=0;
    while(Prescaler<0xfff) {
        TimeReload = ((delaytime*(long)13560)-1)/(Prescaler*2+1);
        if( TimeReload<0xffff)
            break;
        Prescaler++;
    }
    TimeReload=TimeReload&0xFFFF;
    Set_BitMask(TModeReg,Prescaler>>8);
    Write_Reg(TPrescalerReg,Prescaler&0xFF);                    
    Write_Reg(TReloadMSBReg,TimeReload>>8);
    Write_Reg(TReloadLSBReg,TimeReload&0xFF);
    return TRUE;
}

/*********************************************************************************************************
** Function name:       Pcd_ConfigISOType
** Descriptions:        ����ISO14443A/BЭ��
** input parameters:    type = 0��ISO14443AЭ�飻
**                                 type = 1��ISO14443BЭ�飻   
** output parameters:   N/A
** Returned value:      TRUE�������ɹ� ERROR������ʧ��    
*********************************************************************************************************/
unsigned char Pcd_ConfigISOType(unsigned char data type)
{
    if (type == 0)   {                                                  /* ����ΪISO14443_A             */
        Set_BitMask(ControlReg, 0x10);                                /* ControlReg 0x0C ����readerģʽ */
        Set_BitMask(TxAutoReg, 0x40);                                  /* TxASKReg 0x15 ����100%ASK��Ч */
        Write_Reg(TxModeReg, 0x00);                 /* TxModeReg 0x12 ����TX CRC��Ч��TX FRAMING =TYPE A */
        Write_Reg(RxModeReg, 0x00);                 /* RxModeReg 0x13 ����RX CRC��Ч��RX FRAMING =TYPE A */
    }
    if (type == 1)   {                                                  /* ����ΪISO14443_B           */
        Write_Reg(ControlReg,0x10);
        Write_Reg(TxModeReg,0x83);                                      /* BIT1~0 = 2'b11:ISO/IEC 14443B */
        Write_Reg(RxModeReg,0x83);                                      /* BIT1~0 = 2'b11:ISO/IEC 14443B */
        Write_Reg(TxAutoReg,0x00);
        Write_Reg(RxThresholdReg,0x55);
        Write_Reg(RFCfgReg,0x48);
        Write_Reg(TxBitPhaseReg,0x87);                                  /* Ĭ��ֵ                         */
        Write_Reg(GsNReg,0x83);    
        Write_Reg(CWGsPReg,0x30);
        Write_Reg(GsNOffReg,0x38);
        Write_Reg(ModGsPReg,0x20);
    
    }
    Delay100us(30);
    return TRUE;
}


/*********************************************************************************************************
** Function name:       FM175X_SoftReset
** Descriptions:        FM175xx�����λ
** input parameters:    N/A       
** output parameters:   N/A
** Returned value:      TRUE�������ɹ� ERROR������ʧ��    
*********************************************************************************************************/
unsigned char  FM175X_SoftReset(void)
{    
    Write_Reg(CommandReg,SoftReset);
    return    Set_BitMask(ControlReg,0x10);                               /* 17520��ʼֵ����              */
}

/*********************************************************************************************************
** Function name:       FM175X_HardReset
** Descriptions:        FM175xxӲ����λ
** input parameters:    N/A       
** output parameters:   N/A
** Returned value:      TRUE�������ɹ� ERROR������ʧ��    
*********************************************************************************************************/
unsigned char FM175X_HardReset(void)
{    
    gpio_setValue(REST,0);
	usleep(10000);
	gpio_setValue(REST,1);
	usleep(10000);
	
    return TRUE;
}

    
/*********************************************************************************************************
** Function name:       FM175X_SoftPowerdown
** Descriptions:        Ӳ���͹��Ĳ���
** input parameters:    N/A       
** output parameters:   N/A
** Returned value:      TRUE�������ɹ� ERROR������ʧ��    
*********************************************************************************************************/
unsigned char FM175X_SoftPowerdown(void)
{
    if(Read_Reg(CommandReg)&0x10) {
       Clear_BitMask(CommandReg,0x10);                                 /* �˳��͹���ģʽ               */
       return FALSE;
    }
    else
        Set_BitMask(CommandReg,0x10);                                       /* ����͹���ģʽ               */
    return TRUE;
}

/*********************************************************************************************************
** Function name:       FM175X_HardPowerdown
** Descriptions:        Ӳ���͹��Ĳ���
** input parameters:    N/A       
** output parameters:   N/A
** Returned value:      TRUE�������ɹ� ERROR������ʧ��    
*********************************************************************************************************/
unsigned char FM175X_HardPowerdown(void)
{    
    //NPD=~NPD;
    //if(NPD==1)                                                          /* ����͹���ģʽ               */
    return TRUE;                                
//    else
        //return FALSE;                                                     /* �˳��͹���ģʽ               */
}

/*********************************************************************************************************
** Function name:       Read_Ext_Reg
** Descriptions:        ��ȡ��չ�Ĵ���
** input parameters:    reg_add���Ĵ�����ַ��         
** output parameters:   reg_value���Ĵ�����ֵ
** Returned value:      TRUE�������ɹ� ERROR������ʧ��    
*********************************************************************************************************/
unsigned char Read_Ext_Reg(unsigned char reg_add)
{
     Write_Reg(0x0F,0x80+reg_add);
     return Read_Reg(0x0F);
}

/*********************************************************************************************************
** Function name:       Write_Ext_Reg
** Descriptions:        д����չ�Ĵ���
** input parameters:    reg_add���Ĵ�����ַ��
**                      reg_value���Ĵ�����ֵ
** output parameters:   
** Returned value:      TRUE�������ɹ� ERROR������ʧ��    
*********************************************************************************************************/
unsigned char Write_Ext_Reg(unsigned char reg_add,unsigned char reg_value)
{
    Write_Reg(0x0F,0x40+reg_add);
    return (Write_Reg(0x0F,0xC0+reg_value));
}


