/***************************�������ӿƼ����޹�˾****************************
**  �������ƣ�YS-V0.4����ʶ�𿪷������
**	CPU: STM32f103RCT6
**	����8MHZ
**	�����ʣ�9600 bit/S
**	���ײ�Ʒ��Ϣ��YS-V0.4����ʶ�𿪷���
**                http://yuesheng001.taobao.com
**  ���ߣ�zdings
**  ��ϵ��751956552@qq.com
**  �޸����ڣ�2012.4.11
**  ˵���������� �߱�����ʶ�𡢴���ͨ�š����������������ʾ��
***************************�������ӿƼ����޹�˾******************************/
#include "reg_rw.h"

/*************�˿���Ϣ********************
 * ����˵��
 * RST      PB6
 * CS   	PB8
 * WR/SPIS  PB9
 * P2/SDCK  PB3
 * P1/SDO   PB4
 * P0/SDI   PB5
 * IRQ      PC1
 * A0				PB7
 * RD       PA0
*****************************************/



/************************************************************************************
//	nAsrStatus ������main�������б�ʾ�������е�״̬������LD3320оƬ�ڲ���״̬�Ĵ���
//	LD_ASR_NONE:		��ʾû������ASRʶ��
//	LD_ASR_RUNING��		��ʾLD3320������ASRʶ����
//	LD_ASR_FOUNDOK:		��ʾһ��ʶ�����̽�������һ��ʶ����
//	LD_ASR_FOUNDZERO:	��ʾһ��ʶ�����̽�����û��ʶ����
//	LD_ASR_ERROR:		��ʾһ��ʶ��������LD3320оƬ�ڲ����ֲ���ȷ��״̬
*********************************************************************************/
uint8 nAsrStatus=0;	

void LD3320_Init(void);



void LD3320_EXTI_Cfg(void);
void LD3320_Spi_cfg(void);
void LD3320_GPIO_Cfg(void);
void LED_gpio_cfg(void);

/***********************************************************
* ��    �ƣ� LD3320_main(void)
* ��    �ܣ� ������LD3320�������
* ��ڲ�����  
* ���ڲ�����
* ˵    ����
* ���÷����� 
**********************************************************/ 

void  LD3320_main(void)
{
	uint8 nAsrRes=0;
	LD3320_Init();	  
	nAsrStatus = LD_ASR_NONE;		//	��ʼ״̬��û������ASR
	while(1)
	{
// 	switch(nAsrStatus)
//		{
//			case LD_ASR_RUNING:
//			case LD_ASR_ERROR:		
//												break;
//			case LD_ASR_NONE:
//											nAsrStatus=LD_ASR_RUNING;
//											if (RunASR()==0)	//	����һ��ASRʶ�����̣�ASR��ʼ����ASR��ӹؼ��������ASR����
//											{		
//												nAsrStatus = LD_ASR_ERROR;
//											}
//											break;
//
//			case LD_ASR_FOUNDOK:
//											 nAsrRes = LD_GetResult( );	//	һ��ASRʶ�����̽�����ȥȡASRʶ����										 					
//												 switch(nAsrRes)		   /*�Խ��ִ����ز���,�ͻ��޸�*/
//												  {
//													  case CODE_LSD:			/*�����ˮ�ơ�*/
//															PrintCom(USART1,"����ˮ�ơ�����ʶ��ɹ�\r\n"); /*text.....*/
//																						 break;
//														case CODE_SS:	 /*�����˸��*/
//															PrintCom(USART1,"����˸������ʶ��ɹ�\r\n"); /*text.....*/
//																						 break;
//														case CODE_AJCF:		/*�������������*/
//												
//															PrintCom(USART1,"����������������ʶ��ɹ�\r\n"); /*text.....*/
//																						break;
//														case CODE_QM:		/*���ȫ��*/
//												
//															PrintCom(USART1,"��ȫ������ʶ��ɹ�\r\n"); /*text.....*/
//																						break;
//														default:break;
//													}	
//												nAsrStatus = LD_ASR_NONE;
//											break;
//			
//			case LD_ASR_FOUNDZERO:
//			default:
//								nAsrStatus = LD_ASR_NONE;
//								break;
			}//switch
	}// while


/***********************************************************
* ��    �ƣ�LD3320_Init(void)
* ��    �ܣ�ģ�������˿ڳ�ʼ����
* ��ڲ�����  
* ���ڲ�����
* ˵    ����
* ���÷����� 
**********************************************************/ 
void LD3320_Init(void)
{
	LD3320_GPIO_Cfg();	
	LD3320_Spi_cfg();	 
	LD_reset();
}

/***********************************************************
* ��    �ƣ� void Delay_( int i)
* ��    �ܣ� ����ʱ
* ��ڲ�����  
* ���ڲ�����
* ˵    ����
* ���÷����� 
**********************************************************/ 
void Delay_( int i)
 {     
    while( i--)
       {	

        }
 }	
/***********************************************************
* ��    �ƣ�	LD3320_delay(unsigned long uldata)
* ��    �ܣ�	����ʱ����
* ��ڲ�����  
* ���ڲ�����
* ˵    ����
* ���÷����� 
**********************************************************/ 
 void  LD3320_delay(u16 ms)
{
//	unsigned int j  =  0;
//	unsigned int g  =  0;
//	for (j=0;j<10;j++)
//	{
//		for (g=0;g<uldata;g++)
//		{
//			Delay_(1080);
//		}
//	}
   delay_us(120*ms);
}

/***********************************************************
* ��    �ƣ�	RunASR(void)
* ��    �ܣ�	����ASR
* ��ڲ�����  
* ���ڲ�����
* ˵    ����
* ���÷����� 
**********************************************************/ 
uint8 RunASR(void)
{
	uint8 i=0;
	uint8 asrflag=0;
	for (i=0; i<5; i++)			//	��ֹ����Ӳ��ԭ����LD3320оƬ����������������һ������5������ASRʶ������
	{
		LD_AsrStart();			//��ʼ��ASR
		delay_ms(100);
		if (LD_AsrAddFixed()==0)	//��ӹؼ����ﵽLD3320оƬ��
		{
			LD_reset();			//	LD3320оƬ�ڲ����ֲ���������������LD3320оƬ
			delay_ms(100);			//	���ӳ�ʼ����ʼ����ASRʶ������
			continue;
		}
		delay_ms(10);
		if (LD_AsrRun() == 0)
		{
			LD_reset();			//	LD3320оƬ�ڲ����ֲ���������������LD3320оƬ
			delay_ms(100);			//	���ӳ�ʼ����ʼ����ASRʶ������
			continue;
		}
    LD_ReadReg(0xbf);
		asrflag=1;
		break;					//	ASR���������ɹ����˳���ǰforѭ������ʼ�ȴ�LD3320�ͳ����ж��ź�
	}

	return asrflag;
}

/***********************************************************
* ��    �ƣ�LD3320_GPIO_Cfg(void)
* ��    �ܣ���ʼ����Ҫ�õ���IO��
* ��ڲ�����  
* ���ڲ�����
* ˵    ����
* ���÷����� 
**********************************************************/ 
void LD3320_GPIO_Cfg(void)
{	
	  GPIO_InitTypeDef GPIO_InitStructure;
	 	RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOC,ENABLE);
		/*PC0:MODE, PC1:RSTB, PC2:WR*/
		GPIO_InitStructure.GPIO_Pin =GPIO_Pin_0|GPIO_Pin_0|GPIO_Pin_2;//;
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
		GPIO_Init(GPIOC,&GPIO_InitStructure);
    
		GPIO_SetBits(GPIOC,GPIO_Pin_0);	/*MODEĬ������  ѡ��SPI*/
    GPIO_ResetBits(GPIOC,GPIO_Pin_2);	/*WRĬ������*/
	 
}
/***********************************************************
* ��    �ƣ�LD3320_Spi_cfg(void)
* ��    �ܣ�����SPI���ܺͶ˿ڳ�ʼ��
* ��ڲ�����  
* ���ڲ�����
* ˵    ����
* ���÷����� 
**********************************************************/ 
void LD3320_Spi_cfg(void)
{
	SPI_InitTypeDef  SPI_InitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2,ENABLE);	   //ʹ��SPI2����ʱ��
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB|RCC_APB2Periph_AFIO,ENABLE);
//  GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE); 

	/*P0/P1/P2*/
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_Init(GPIOB,&GPIO_InitStructure);
  GPIO_SetBits(GPIOB,GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;			//CS(NSS)
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
  
	LD_CS_H();

	/* SPI2 ���� */
  SPI_Cmd(SPI2, DISABLE);	
	SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;  //ȫ˫��
	SPI_InitStructure.SPI_Mode = SPI_Mode_Master;				                //��ģʽ
	SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;					          //8λ
	SPI_InitStructure.SPI_CPOL = SPI_CPOL_High;						              //ʱ�Ӽ��� ����״̬ʱ��SCK���ָߵ�ƽ
	SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;						   //ʱ����λ ���ݲ����ӵ�һ��ʱ�ӱ��ؿ�ʼ
	SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;							   //�������NSS
	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_32;   //�����ʿ��� SYSCLK/128
	SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;				   //���ݸ�λ��ǰ
	SPI_InitStructure.SPI_CRCPolynomial = 7;							   //CRC����ʽ�Ĵ�����ʼֵΪ7
	SPI_Init(SPI2, &SPI_InitStructure);
	/* ʹ��SPI2 */
	SPI_Cmd(SPI2, ENABLE);

}
/***********************************************************
* ��    �ƣ� LD3320_EXTI_Cfg(void) 
* ��    �ܣ� �ⲿ�жϹ������ú���ض˿�����
* ��ڲ�����  
* ���ڲ�����
* ˵    ����
* ���÷����� 
**********************************************************/ 
void LD3320_EXTI_Cfg(void)
{

}


/***********************************************************
* ��    �ƣ�  EXTI1_IRQHandler(void)
* ��    �ܣ� �ⲿ�жϺ���
* ��ڲ�����  
* ���ڲ�����
* ˵    ����
* ���÷����� 
**********************************************************/ 
void EXTI1_IRQHandler(void)
{
	if(EXTI_GetITStatus(EXTI_Line1)!= RESET ) 
	{
 		ProcessInt0(); 
		EXTI_ClearFlag(EXTI_Line1);
		EXTI_ClearITPendingBit(EXTI_Line1);
	}
}

