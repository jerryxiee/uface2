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
** File name:           mifare_card.c
** Last modified Date:  2016-3-14
** Last Version:        V1.00
** Descriptions:        mifareϵ�п���ز���
**
**--------------------------------------------------------------------------------------------------------
*/
#include "mifare_card.h"
#include "fm175xx.h"
#include "Utility.h"

/*********************************************************************************************************
** Function name:       Mifare_Auth        
** Descriptions:        mifare��ƬУ��
** input parameters:    mode����֤ģʽ��0��key A��֤��1��key B��֤��
**                      sector����֤�������ţ�0~15��
**                      *mifare_key��6�ֽ���֤��Կ����
**                      card_uid 4�ֽڿ�ƬUID����        
** output parameters:   
** Returned value:      TRUE�������ɹ� ERROR������ʧ��    
*********************************************************************************************************/
 unsigned char Mifare_Auth(unsigned char mode,unsigned char sector,unsigned char *mifare_key,unsigned char *card_uid)
{
    unsigned char  send_buff[12],rece_buff[1],result;
    unsigned int  rece_bitlen;
    if(mode==0x0)
        send_buff[0]=0x60;                                              /* kayA��֤ 0x60                */
    if(mode==0x1)
        send_buff[0]=0x61;                                              /* keyB��֤                     */
    send_buff[1] = sector*4;
    send_buff[2] = mifare_key[0];
    send_buff[3] = mifare_key[1];
    send_buff[4] = mifare_key[2];
    send_buff[5] = mifare_key[3];
    send_buff[6] = mifare_key[4];
    send_buff[7] = mifare_key[5];
    send_buff[8] = card_uid[0];
    send_buff[9] = card_uid[1];
    send_buff[10] = card_uid[2];
    send_buff[11] = card_uid[3];

    Pcd_SetTimer(1);
    Clear_FIFO();
    result =Pcd_Comm(MFAuthent,send_buff,12,rece_buff,&rece_bitlen);    /* Authent��֤                  */
    if (result==TRUE) {
        if(Read_Reg(Status2Reg)&0x08)                                   /* �жϼ��ܱ�־λ��ȷ����֤��� */
            return TRUE;
        else
            return FALSE;
    }
    return FALSE;
}

/*********************************************************************************************************
** Function name:       Mifare_Blockset        
** Descriptions:        mifare�����ÿ�Ƭ����ֵ
** input parameters:    block:���
**                      buff:4�ֽڳ�ʼֵ
** output parameters:   
** Returned value:      TRUE�������ɹ� ERROR������ʧ��    
*********************************************************************************************************/
unsigned char Mifare_Blockset(unsigned char block,unsigned char *buff)
{
    unsigned char  block_data[16],result;
    block_data[0]=buff[3];
    block_data[1]=buff[2];
    block_data[2]=buff[1];
    block_data[3]=buff[0];
    block_data[4]=~buff[3];
    block_data[5]=~buff[2];
    block_data[6]=~buff[1];
    block_data[7]=~buff[0];
    block_data[8]=buff[3];
    block_data[9]=buff[2];
    block_data[10]=buff[1];
    block_data[11]=buff[0];
    block_data[12]=block;
    block_data[13]=~block;
    block_data[14]=block;
    block_data[15]=~block;
    result= Mifare_Blockwrite(block,block_data);
    return result;
}

/*********************************************************************************************************
** Function name:       Mifare_Blockread        
** Descriptions:        mifare����ȡ�麯��
** input parameters:    block:���   0x00-0x3f       
** output parameters:   buff:����16�ֽ����� 
** Returned value:      TRUE�������ɹ� ERROR������ʧ��    
*********************************************************************************************************/
unsigned char Mifare_Blockread(unsigned char block,unsigned char *buff)
{    
    unsigned char  send_buff[2],result;
    unsigned int  rece_bitlen;
    Pcd_SetTimer(1);
    send_buff[0]=0x30;                                                  /* ����������                   */
    send_buff[1]=block;                                                 /* ��������ַ                   */
    Clear_FIFO();
    result =Pcd_Comm(Transceive,send_buff,2,buff,&rece_bitlen);//
    if ((result!=TRUE )|(rece_bitlen!=16*8))                            /* ���յ������ݳ���Ϊ16         */
        return FALSE;
    return TRUE;
}

/*********************************************************************************************************
** Function name:       mifare_blockwrite        
** Descriptions:        mifare��д�麯��
** input parameters:    block:���   0x00-0x3f    
**                      buff:����16�ֽ����� 
** output parameters:   
** Returned value:      TRUE�������ɹ� ERROR������ʧ��    
*********************************************************************************************************/
unsigned char Mifare_Blockwrite(unsigned char block,unsigned char *buff)
{    
    unsigned char  result,send_buff[16],rece_buff[1];
    unsigned int  rece_bitlen;
    Pcd_SetTimer(1);
    send_buff[0]=0xa0;                                                  /* д������                     */
    send_buff[1]=block;                                                 /* ���ַ                       */

    Clear_FIFO();
    result =Pcd_Comm(Transceive,send_buff,2,rece_buff,&rece_bitlen);
    if ((result!=TRUE )|((rece_buff[0]&0x0F)!=0x0A))                    /* ���δ���յ�0x0A����ʾ��ACK  */
        return(FALSE);

    Pcd_SetTimer(5);
    Clear_FIFO();
    result =Pcd_Comm(Transceive,buff,16,rece_buff,&rece_bitlen);
    if ((result!=TRUE )|((rece_buff[0]&0x0F)!=0x0A))                    /* ���δ���յ�0x0A����ʾ��ACK  */
        return FALSE;
    return TRUE;
}


/*********************************************************************************************************
** Function name:       Mifare_Blockinc        
** Descriptions:        mifare��Ƭ��ֵ����    
** input parameters:    block:���   0x00-0x3f    
**                      buff:����4�ֽ����� 
** output parameters:   
** Returned value:      TRUE�������ɹ� ERROR������ʧ��    
*********************************************************************************************************/
unsigned char Mifare_Blockinc(unsigned char block,unsigned char *buff)
{    
    unsigned char  result,send_buff[2],rece_buff[1];
    unsigned int  rece_bitlen;
    Pcd_SetTimer(5);
    send_buff[0]=0xc1;                                                  /* ��ֵ��������                 */
    send_buff[1]=block;                                                 /* ���ַ                       */
    Clear_FIFO();
    result = Pcd_Comm(Transceive,send_buff,2,rece_buff,&rece_bitlen);
    if ((result != TRUE )|((rece_buff[0]&0x0F) != 0x0A))                /* ���δ���յ�0x0A����ʾ��ACK  */
        return FALSE;
    Pcd_SetTimer(5);
    Clear_FIFO();
    Pcd_Comm(Transceive,buff,4,rece_buff,&rece_bitlen);
    return result;
}


/*********************************************************************************************************
** Function name:       Mifare_Blockdec        
** Descriptions:        mifare��Ƭ��ֵ����    
** input parameters:    block:���   0x00-0x3f    
**                      buff:����4�ֽ����� 
** output parameters:   
** Returned value:      TRUE�������ɹ� ERROR������ʧ��    
*********************************************************************************************************/
unsigned char Mifare_Blockdec(unsigned char block,unsigned char *buff)
{    
    unsigned char  result,send_buff[2],rece_buff[1];
    unsigned int  rece_bitlen;
    Pcd_SetTimer(5);
    send_buff[0]=0xc0;
    send_buff[1]=block;                                                 /* ���ַ                       */
    Clear_FIFO();
    result = Pcd_Comm(Transceive,send_buff,2,rece_buff,&rece_bitlen);
    if ((result!=TRUE )|((rece_buff[0]&0x0F)!=0x0A))                    /* ���δ���յ�0x0A����ʾ��ACK  */
        return FALSE;
    Pcd_SetTimer(5);
    Clear_FIFO();
    Pcd_Comm(Transceive,buff,4,rece_buff,&rece_bitlen);
    return result;
}


/*********************************************************************************************************
** Function name:       Mifare_Transfer        
** Descriptions:        mifare��Ƭ�����
** input parameters:    block:���   0x00-0x3f    
** output parameters:   
** Returned value:      TRUE�������ɹ� ERROR������ʧ��    
*********************************************************************************************************/
unsigned char Mifare_Transfer(unsigned char block)
{    
    unsigned char   result,send_buff[2],rece_buff[1];
    unsigned int   rece_bitlen;
    Pcd_SetTimer(5);
    send_buff[0] = 0xb0;
    send_buff[1] = block;                                               /* ���ַ                       */
    Clear_FIFO();
    result=Pcd_Comm(Transceive,send_buff,2,rece_buff,&rece_bitlen);
    if ((result != TRUE )|((rece_buff[0]&0x0F) != 0x0A))                /* ���δ���յ�0x0A����ʾ��ACK  */
        return FALSE;
    return result;
}


/*********************************************************************************************************
** Function name:       Mifare_Restore        
** Descriptions:        mifare��Ƭ��������
** input parameters:    block:���   0x00-0x3f    
** output parameters:   
** Returned value:      TRUE�������ɹ� ERROR������ʧ��    
*********************************************************************************************************/
unsigned char Mifare_Restore(unsigned char block)
{    
    unsigned char result,send_buff[4],rece_buff[1];
    unsigned int  rece_bitlen;
    Pcd_SetTimer(5);
    send_buff[0]=0xc2;
    send_buff[1]=block;                                                 /* ���ַ                       */
    Clear_FIFO();
    result = Pcd_Comm(Transceive,send_buff,2,rece_buff,&rece_bitlen);
    if ((result != TRUE )|((rece_buff[0]&0x0F) != 0x0A))                /* ���δ���յ�0x0A����ʾ��ACK  */
        return FALSE;
    Pcd_SetTimer(5);
    send_buff[0]=0x00;
    send_buff[1]=0x00;
    send_buff[2]=0x00;
    send_buff[3]=0x00;
    Clear_FIFO();
    Pcd_Comm(Transceive,send_buff,4,rece_buff,&rece_bitlen);
    return result;
}
