/************************************************************************************/
//	��Ȩ���У�Copyright (c) 2005 - 2010 ICRoute INC.
/************************************************************************************/

#include "sys.h"
#include "Reg_RW.h"


void LD_WriteReg( unsigned char address, unsigned char dataout )
{
	LD_CS_L();
	// �������Ӳ��SPI�ڵĲ������룺
	SPI_SendByte(0x04); // ���� 0x04
	SPI_SendByte(address); // ���� address
	SPI_SendByte(dataout); // ���� dataout
	LD_CS_H();
}
unsigned char LD_ReadReg( unsigned char address )
{
	LD_CS_L();
	// �������Ӳ��SPI�ڵĲ������룺
	SPI_SendByte(0x05); // ���� 0x05
	SPI_SendByte(address); // ���� address
	LD_CS_H();
	return (SPI_SendByte(0)); // ��������, ������
	
}
	

//SPIx ��дһ���ֽ�
//TxData:Ҫд����ֽ�
//����ֵ:��ȡ�����ֽ�
unsigned char SPI_TR( unsigned char TxData )
{		
	u8 retry=0;				 	
	while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET) //���ָ����SPI��־λ�������:���ͻ���ձ�־λ
	{
		retry++;
		if(retry>200)return 0;
	}			  
	SPI_I2S_SendData(SPI1, TxData); //ͨ������SPIx����һ������
	retry=0;

	while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) == RESET) //���ָ����SPI��־λ�������:���ܻ���ǿձ�־λ
	{
		retry++;
		if(retry>200)return 0;
	}	  						    
	return SPI_I2S_ReceiveData(SPI1); //����ͨ��SPIx������յ�����					    
}

void LD3320_SPI_init(void)
{

	/* ����SPI�ṹ����� */
	GPIO_InitTypeDef  GPIO_InitStructure;
	SPI_InitTypeDef   SPI_InitStructure;
	
	/* SPI��IO�ں�SPI�����ʱ�� */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);

	/* CS��IO������ */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;  
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;  		//�������
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	GPIO_SetBits(GPIOA,GPIO_Pin_4);					//��ʼhigh��ƽ

	
	/* SPI��IO������ */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7 ;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;		//�����������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	GPIO_SetBits(GPIOA,GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7);


	/* SPI�Ļ������� */
	SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;  // ����SPI�������˫�������ģʽ:SPI����Ϊ˫��˫��ȫ˫��
	SPI_InitStructure.SPI_Mode = SPI_Mode_Master;	  // ����SPI����ģʽ:����Ϊ��SPI
	SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b; // ����SPI�����ݴ�С:SPI���ͽ���8λ֡�ṹ
	SPI_InitStructure.SPI_CPOL = SPI_CPOL_High;		  // ����ͬ��ʱ�ӵĿ���״̬ΪHIGH��ƽ
	SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;	  // ����ͬ��ʱ�ӵĵ�1�������أ��������½������ݱ�����
	SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;		  // NSS�ź���Ӳ����NSS�ܽţ����������ʹ��SSIλ������:�ڲ�NSS�ź���SSIλ����
	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_64;// ���岨����Ԥ��Ƶ��ֵ:������Ԥ��ƵֵΪ16
	SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;// ָ�����ݴ����MSBλ����LSBλ��ʼ:���ݴ����MSBλ��ʼ
	SPI_InitStructure.SPI_CRCPolynomial = 7;		  // CRCֵ����Ķ���ʽ
	SPI_Init(SPI1, &SPI_InitStructure);  			  // ����SPI_InitStruct��ָ���Ĳ�����ʼ������SPIx�Ĵ���
	
	SPI_Cmd(SPI1, ENABLE); 	  // ʹ��SPI����

}



void SIM_SPI_init()
{
		/* ����SPI�ṹ����� */
	GPIO_InitTypeDef  GPIO_InitStructure;
	SPI_InitTypeDef   SPI_InitStructure;
	
	/* SPI��IO�ڴ�ʱ�� */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);


	/* CS��IO������ */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;  
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;  		//�������
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	GPIO_SetBits(GPIOA,GPIO_Pin_4);					//��ʼhigh��ƽ
	
	/* CLK��IO������ */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;  
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;  		//�������
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	GPIO_SetBits(GPIOA,GPIO_Pin_5);					//��ʼhigh��ƽ
	
	/* MISO��IO������ */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;  
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;  		//�������
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	GPIO_SetBits(GPIOA,GPIO_Pin_6);					//��ʼhigh��ƽ
	
	/* MOSI��IO������ */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;  
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;  		//�������
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	GPIO_SetBits(GPIOA,GPIO_Pin_7);					//��ʼhigh��ƽ
}
	
 

 

unsigned char SPI_SendByte(unsigned char dt)

{

	u8 i;

	u8 temp=0;

	for(i=8;i>0;i--)

	{

		if(dt&0x80)MOSI_H;

		else MOSI_L;

		dt<<=1;

		SCLK_H;

		__nop();

		__nop();

		__nop();

		temp<<=1;

		if(MISO)temp++;

		__nop();

		__nop();

		__nop();

		SCLK_L;

	}

	return temp;

}

