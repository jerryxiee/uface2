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
** File name:           main.c
** Last modified Date:  2016-3-4
** Last Version:        V1.00
** Descriptions:        ������ʾ��������
**
**--------------------------------------------------------------------------------------------------------
*/


#include "fm175xx.h"
#include "type_a.h"
#include "type_b.h"
#include "cpu_card.h"
#include "Utility.h"
#include "mifare_card.h"
#include "des.h"

#include "string.h"

#include <stdio.h>



/*
** ���Ժ���
*/
extern void cpu_test(int fd);
extern void CPU_Test(int fd);
extern void TyteA_Test(int fd);
extern void TyteB_Test(int fd);
extern void MifareCard_Test (int fd,uint8_t command);
extern int get_card_id(int fd,unsigned char *id);
int get_uid(int fd,unsigned char *id);
extern unsigned char get_version(int fd);

/*********************************************************************************************************
** Function name:       UartCommand()
** Descriptions:        ���ڽ��յ����ݣ�ִ�в�������
** input parameters:    ��
** output parameters:   ��
** Returned value:      ��
*********************************************************************************************************/
void UartCommand(void)
{
    
  /*  if(GucRcvNew ==1)  {                                                // ���ڽ��յ�����               
        MifareCard_Test( Recv_Buff[0] );                                // ��һ����������λ��������     
        GucRcvNew = 0;                                                  // ����������                 
        memset(Recv_Buff,0x00,Maxlength);                               // ������0                      
    }*/
}

int get_id(int fd,unsigned char *uid,int mode)
{
	int ret;
	if(mode)
	{
		ret = get_uid(fd,uid);
	}
	else
	{
		ret = get_card_id(fd,uid);
	}
	return ret;
}
/*********************************************************************************************************
** �����ܣ�Demo��ͨ�����ڽ���������ݲ�ͬ��������ִ�в�ͬ�Ĳ���----�����ʹ̶�115200
** ���̹��ܣ�ͨ��CON���ŵĵ�ƽ�������ֲ�ͬģʽ��  CON=1-----ģʽ1����Ƭ�Զ����ģʽ��---�̵Ƴ���
**     ��ģʽ�£���Ƭ��һֱ��⿨Ƭ�����п�Ƭ��������ʱ�����ȡ��Ƭ��ID�Ų�ͨ�����ڴ�ӡID��
**     �����ʽ������115200���ַ����
**                                                CON=0-----ģʽ2�������������ģʽ���������£�
** ע�⣺����ѭ��Ѱ��ģʽ��Demo��Ͳ��ܽ��������ˣ�Ҫ�뷵��ǰ�������ģʽ������Ҫ�����ϵ�
**
*********************************************************************************************************/
int main(void)
{
#if 1
	int fd = -1;
	int i;
	int card_type = -1;
	unsigned char uid[10];
	unsigned char nbit;
	volatile uint8_t ucRegVal;
    volatile unsigned char statues;
    
    Delay100us(1000);
                   
    pcd_Init();                                                         // ����оƬ��ʼ��               
               
	fd = uart_init(9600,8,'N',1);
	
	
    //cpu_test(fd);
		
	while (1) {
		/*nbit = get_card_id(fd,uid);
		//printf("nbit=%d\n",nbit);
		if(nbit > 0)
		{
			printf("\nUID:");
			for(i = 0;i < nbit;i++)
			{
				printf("%x\t",uid[i]);
			}
			
		}
		sleep(2);
		CPU_Test(fd);    */     
		TyteA_Test(fd);
		//TyteB_Test(fd);
		//sleep(1);
			
			
	}
#else
		
	int ret,len,i;
	unsigned char buff[1024] = {0};
	unsigned char dat[16] = {0x87,0x87,0x87,0x87,0x87,0xf87,0x87,0x87};
	unsigned char key[16] = {0x0E,0x32,0x92,0x32,0xEA,0x6D,0x0D,0x73};
	
	
	len = strlen((char*)dat);
        for(i = 0;i < len;i++)
        {
            printf("%02X",dat[i]);
        }
        printf("\r\n");

        
        //DES ECB ����
        printf("DES ECB ENC::\r\n");
        ret = des_ecb_encrypt(buff,dat,len,key);
        for(i = 0;i < ret;i++)
        {
            printf("%02X",buff[i]);
        }
        printf("\r\n");
        //DES ECB ����
        printf("DES ECB DEC::\r\n");
        memset(dat,0,sizeof(dat));
        des_ecb_decrypt(dat,buff,ret,key);
        for(i = 0;i < ret;i++)
        {
            printf("%02X",dat[i]);
        }
        printf("\r\n");

        //DES CBC ����
        printf("DES CBC ENC::\r\n");
        memset(buff,0,sizeof(buff));
        des_cbc_encrypt(buff,dat,ret,key,NULL);
        for(i = 0;i < ret;i++)
        {
            printf("%02X",buff[i]);
        }
        printf("\r\n");

        //DES CBC ����
        printf("DES CBC DEC::\r\n");
        memset(dat,0,sizeof(dat));
        des_cbc_decrypt(dat,buff,ret,key,NULL);
        for(i = 0;i < ret;i++)
        {
            printf("%02X",dat[i]);
        }
        printf("\r\n");
        printf("\r\n");


        //3DES ECB ����
        printf("3DES ECB ENC::\r\n");
        ret = des3_ecb_encrypt(buff,dat,len,key,16);
        for(i = 0;i < ret;i++)
        {
            printf("%02X",buff[i]);
        }
        printf("\r\n");
        //3DES ECB ����
        printf("3DES ECB DEC::\r\n");
        memset(dat,0,sizeof(dat));
        des3_ecb_decrypt(dat,buff,ret,key,16);
        for(i = 0;i < ret;i++)
        {
            printf("%02X",dat[i]);
        }
        printf("\r\n");

        //3DES CBC ����
        printf("3DES CBC ENC::\r\n");
        memset(buff,0,sizeof(buff));
        des3_cbc_encrypt(buff,dat,ret,key,16,NULL);
        for(i = 0;i < ret;i++)
        {
            printf("%02X",buff[i]);
        }
        printf("\r\n");

        //3DES CBC ����
        printf("3DES CBC DEC::\r\n");
        memset(dat,0,sizeof(dat));
        des3_cbc_decrypt(dat,buff,ret,key,16,NULL);
        for(i = 0;i < ret;i++)
        {
            printf("%02X",dat[i]);
        }
        printf("\r\n");
		
#endif
	
                
    
	
	return 0;
}






