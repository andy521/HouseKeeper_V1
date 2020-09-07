/***************************�������ӿƼ����޹�˾****************************
**  �ļ����ƣ�����ģ��ײ�����
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
#include "ldchip.h"
#define delay_ms DelayMs
//#include "sdcard.h"
//#include "ff.h"
//#include "file.h"
/** @defgroup SD  source
* @{
*/
//FATFS    FsFat; //�ļ�ϵͳ���
//FILETYP  ConfigFile;//SD���洢�������ļ����
//BYTE file_stat;
static unsigned char stringBuf[10];    //for MFID from SD�����ޱ��ļ�ʹ��
//u8*const KAIDENG="/����.mp3";		//GBK24�Ĵ��λ��
//u8*const GUANDENG="/�ص�.mp3";		//GBK24�Ĵ��λ��
//u8*const DANWEI="/��λ.mp3";		//GBK24�Ĵ��λ��
//u8*const XINGMING="/����.mp3";		//GBK24�Ĵ��λ��
//u8*const TEST_PAT="/erge.bin";		//GBK24�Ĵ��λ��
//u8*const GUANDENG="/���Ų�ѩ.mp3";
uint32  stringSize;  

/**
* @}
*/
extern uint8  nAsrStatus;
uint32 nMp3StartPos=0;
uint32 nMp3Size=0;
uint32 nMp3Pos=0;
uint32 nCurMp3Pos=0;
uint8  nLD_Mode = LD_MODE_IDLE;		//	������¼��ǰ���ڽ���ASRʶ�����ڲ���MP3
uint8 bMp3Play=0;							//	������¼����MP3��״̬
uint8 ucRegVal;
uint8 ucHighInt;
uint8 ucLowInt;
uint8 ucStatus;
uint8 ucSPVol=15; // MAX=15 MIN=0		//	Speaker�������������
uint8 buffer[10];
unsigned long fpPoint=0;

/***********************************************************
* ��    �ƣ�void LD_reset(void)
* ��    �ܣ�LDоƬӲ����ʼ��
* ��ڲ�����  
* ���ڲ�����
* ˵    ����
* ���÷����� 
**********************************************************/ 
void LD_reset(void)
{
   //u8 aa,bb,cc;
	LD_RST_H();
	delay_ms(1);
	LD_RST_L();
	delay_ms(1);
	LD_RST_H();
	delay_ms(1);
	LD_CS_L();
	delay_ms(1);
	LD_CS_H();		
	delay_ms(1);
}

/***********************************************************
* ��    �ƣ� void LD_Init_Common(void)
* ��    �ܣ� ��ʼ������
* ��ڲ�����  
* ���ڲ�����
* ˵    ����
* ���÷����� 
**********************************************************/ 
void LD_Init_Common(void)
{

	LD_ReadReg(0x06);  
	LD_WriteReg(0x17, 0x35); 
	delay_ms(5);
	LD_ReadReg(0x06);  

	LD_WriteReg(0x89, 0x03);  
	delay_ms(5);
	LD_WriteReg(0xCF, 0x43);   
	delay_ms(5);
	LD_WriteReg(0xCB, 0x02);
	
	/*PLL setting*/
	LD_WriteReg(0x11, LD_PLL_11);       
	if (nLD_Mode == LD_MODE_MP3)
	{
		LD_WriteReg(0x1E, 0x00); 
		LD_WriteReg(0x19, LD_PLL_MP3_19);   
		LD_WriteReg(0x1B, LD_PLL_MP3_1B);   
		LD_WriteReg(0x1D, LD_PLL_MP3_1D);
	}
	else
	{
		LD_WriteReg(0x1E,0x00);
		LD_WriteReg(0x19, LD_PLL_ASR_19); 
		LD_WriteReg(0x1B, LD_PLL_ASR_1B);		
	    LD_WriteReg(0x1D, LD_PLL_ASR_1D);
	}
	delay_ms(5);
	
	LD_WriteReg(0xCD, 0x04);
	LD_WriteReg(0x17, 0x4c); 
	LD3320_delay(1);
	LD_WriteReg(0xB9, 0x00);
	LD_WriteReg(0xCF, 0x4F); 
	LD_WriteReg(0x6F, 0xFF); 
}

/***********************************************************
* ��    �ƣ�void LD_Init_ASR(void)
* ��    �ܣ���ʼ��ASR
* ��ڲ�����  
* ���ڲ�����
* ˵    ����
* ���÷����� 
**********************************************************/ 	
void LD_Init_ASR(void)
{
	nLD_Mode=LD_MODE_ASR_RUN;
	LD_Init_Common();

	LD_WriteReg(0xBD, 0x00);
	LD_WriteReg(0x17, 0x48);	
	LD3320_delay( 100 );
	LD_WriteReg(0x3C, 0x80);    
	LD_WriteReg(0x3E, 0x07);
	LD_WriteReg(0x38, 0xff);    
	LD_WriteReg(0x3A, 0x07);
	LD_WriteReg(0x40, 0x00);          
	LD_WriteReg(0x42, 0x08);
	LD_WriteReg(0x44, 0x00);    
	LD_WriteReg(0x46, 0x08); 
	LD3320_delay( 100 );
  LD_WriteReg(0x35, 0x4C); 
  LD3320_delay( 10 );
  LD_WriteReg(0xB3, 0x28); 
  LD_WriteReg(0xB4, 0x28); 
//  LD_WriteReg(0xB5, 0x64); 
  delay_ms( 10 );
}

/***********************************************************
* ��    �ƣ�void ProcessInt0(void)
* ��    �ܣ�ʶ������
* ��ڲ�����  
* ���ڲ�����
* ˵    �����������ò�ѯ��ʽ�����жϷ�ʽ���е���
* ���÷����� 
**********************************************************/ 
void ProcessInt0(void)
{
	uint8 nAsrResCount=0;
	u8 err;
	if(nLD_Mode == LD_MODE_ASR_RUN)
	{
		uint8 Ab2,Bf;
		ucRegVal = LD_ReadReg(0x2B);
		Ab2 = LD_ReadReg(0xc5);
		Bf = LD_ReadReg(0xc7);
		//     LCD_ShowxNum(100,150,ucRegVal,2,24,0);
		//     LCD_ShowxNum(150,150,Ab2,4,24,0);
		//     LCD_ShowxNum(200,150,Bf,4,24,0);
		printf("ucRegVal:%d\r\n",ucRegVal);
		printf("Ab2:%d\r\n",Ab2);
		printf("Bf:%d\r\n",Bf);
		// ����ʶ��������ж�
		// �����������룬����ʶ��ɹ���ʧ�ܶ����жϣ�
		LD_WriteReg(0x29,0) ;
		LD_WriteReg(0x02,0) ;
		if((ucRegVal & 0x10) && LD_ReadReg(0xb2)==0x21 && LD_ReadReg(0xbf)==0x35)		
		{	 
				nAsrResCount = LD_ReadReg(0xba);
				//LCD_ShowString(100,200,100,24,24,"enter");
				printf("enter\r\n");
				//OSFlagPost(Flags,0x0001,OS_FLAG_SET,&err);/* ʶ��ɹ� 1*/
				if(nAsrResCount>0 && nAsrResCount<=4) 
			{
				//LCD_ShowString(200,200,100,24,24,"success");
				printf("success\r\n");
				//LCD_ShowxNum(300,150,nAsrResCount,4,24,0);
				printf("nAsrResCount:%d\r\n",nAsrResCount);
				nAsrStatus=LD_ASR_FOUNDOK; 	
				LD_WriteReg(0xb2,  0);
			}
			else
			{
				//                 LD_reset();
				//LCD_ShowString(200,230,100,24,24,"enter fail");
				printf("enter fail\r\n");
				nAsrStatus=LD_ASR_FOUNDZERO;
			}	
		}
		if( LD_ReadReg(0xbf)==0x35 ) 
		{
			//OSFlagPost(Flags,0x0002,OS_FLAG_SET,&err);/* ʶ��ʧ�� 2*/
			//LCD_ShowString(100,230,100,24,24,"fail");
			printf("fail\r\n");
			//        LD_reset();
			delay_ms(20);
			ucRegVal = LD_ReadReg(0x2B);

			nAsrStatus=LD_ASR_FOUNDZERO;	//ִ��û��ʶ��
		}

		//     LD_WriteReg(0x2b, 0);
		//		  LD_WriteReg(0x1C,0);/*д0:ADC������*/

		//     LD_WriteReg(0x29,0) ;
		//     LD_WriteReg(0x02,0) ;
		//     LD_WriteReg(0x2B,  0);
		//     LD_WriteReg(0xBA, 0);	
		//     LD_WriteReg(0xBC,0);	
		//     LD_WriteReg(0x08,1);	 /*���FIFO_DATA*/
		//     LD_WriteReg(0x08,0);	/*���FIFO_DATA�� �ٴ�д0*/
		//      LD_WriteReg(0x1C,0x0b);
		//      LD_AsrRun();
		//LD3320_delay(1);
		//RunASR();
	}
	if(nLD_Mode == LD_MODE_MP3 )
	{
		ucRegVal = LD_ReadReg(0x2B);
		ucHighInt = LD_ReadReg(0x29); 
		ucLowInt=LD_ReadReg(0x02); 
		LD_WriteReg(0x29,0) ;
		LD_WriteReg(0x02,0) ;
		if( LD_ReadReg(0xBA)&CAUSE_MP3_SONG_END)
		{
			// A. ����������ȫ�������ꡣ

			LD_WriteReg(0x2B,0);	
			LD_WriteReg(0xBA, 0);	
			LD_WriteReg(0xBC,0x0);	
			bMp3Play=0;					// ��������ȫ����������޸�bMp3Play�ı���
			nMp3Pos = 0;
			LD_WriteReg(0x08,1);
			delay_ms(1);
			LD_WriteReg(0x08,0);
			LD_WriteReg(0x33, 0);
			//        f_close(&ConfigFile);
			//OSFlagPost(Flags,0x0002,OS_FLAG_SET,&err);/* ������� 2*/
			return ;
		}
		//     
		if(nMp3Pos>=nMp3Size)
		{
			// B. ���������ѷ�����ϡ�

			LD_WriteReg(0xBC,0x01);
			LD_WriteReg(0x29,0x10) ;
			//		bMp3Play=0;				//	��ʱ��ֻ������MCU������MP3���ݷ��͵�LD3320оƬ�ڣ����ǻ�û�а����������ȫ���������
			return;	
		}
		//        if(nMp3Pos == nMp3Size)
		//        {
		//           bMp3Play = 0;
		//           OSFlagPost(Flags,0x0002,OS_FLAG_SET,&err);/* ʶ��ʧ�� 2*/
		//           return;
		//        }
		   
		//LD_ReloadMp3Data();

		LD_WriteReg(0x29,ucHighInt);
		LD_WriteReg(0x02,ucLowInt) ;
		//     
	}
//  if((LD_ReadReg(0xBA)&CAUSE_MP3_SONG_END))
//     return;
//     if(nMp3Pos>=nMp3Size)
//        LD_WriteReg(0xBC,0x01);
//     else
//     {
//        if(LD_ReadReg(0x2B)&0x04 )
//        {
//           LD_ReloadMp3Data();
//           LD_WriteReg(0x2B,0x00);
//        }
//    
//     }

}

/***********************************************************
* ��    �ƣ�uint8 LD_Check_ASRBusyFlag_b2(void)
* ��    �ܣ���� ASR �Ƿ�æ״̬
* ��ڲ�����flag ---1������״̬  
* ���ڲ�����
* ˵    ����
* ���÷����� 
**********************************************************/ 
uint8 LD_Check_ASRBusyFlag_b2(void)
{
	uint8 j;
	uint8 flag = 0;
	for (j=0; j<10; j++)
	{
		if (LD_ReadReg(0xb2) == 0x21)
		{
			flag = 1;
			break;
		}
		delay_ms(10);		
	}
	return flag;
}
/***********************************************************
* ��    �ƣ� void LD_AsrStart(void)
* ��    �ܣ�
* ��ڲ�����  ASR��ʼ��
* ���ڲ�����
* ˵    ����
* ���÷����� 
**********************************************************/ 
void LD_AsrStart(void)
{
	LD_Init_ASR();
}
/***********************************************************
* ��    �ƣ� uint8 LD_AsrRun(void)
* ��    �ܣ� ASRִ�к���
* ��ڲ�����  
* ���ڲ�����
* ˵    ���� VADϵ�мĴ���
* ���÷����� B3�������˵��⣬ԽСԽ����������ԽԶ�����и��󣬻��������ô�ֵ
          �� B4����Ҫ������ò�����Ϊ��������ʼ��Ĭ��150ms����λ10ms
          �� B5����B4�෴��Ĭ��600ms
          �� B6����⵽������ʼ�������õ�ʱ�䣬Ĭ��6�룬��λ100ms
          �� B7������ǰ���õ����ݣ���λ20ms��Ĭ��40ms��
          �� B8���ʶ�𳤶ȣ�Ĭ��60s��
**********************************************************/ 
uint8 LD_AsrRun(void)
{
	LD_WriteReg(0x35, MIC_VOL);                /*��˷�����:Խ��Խ��������ʶ��Խ�࣬���ӻ�����Сֵ*/
  LD_WriteReg(0xB3, 0x28);
	LD_WriteReg(0x1C, 0x09);
	LD_WriteReg(0xBD, 0x20);
	LD_WriteReg(0x08, 0x01);                   /*���FIFO���ݵ�0λ��д��1�����FIFO_DATA ��2λ��д��1�����FIFO_EXT*/
	delay_ms( 1 );
	LD_WriteReg(0x08, 0x00);                   /*���FIFO���ݵ�0λ�����ָ��FIFO����д��һ��00H��*/
	delay_ms( 1);

	if(LD_Check_ASRBusyFlag_b2() == 0)
	{
		return 0;
	}

	LD_WriteReg(0xB2, 0xff);	
	LD_WriteReg(0x37, 0x06);                   /*д06H ֪ͨDSP��ʼʶ������*/
	delay_ms( 1);
	LD_WriteReg(0x1C, 0x0b);                   /*ADC���ؿ���  д0BH ��˷�����ADCͨ������*/
	LD_WriteReg(0x29, 0x10);                   /*�ж����� ��2λ��FIFO �ж����� ��4λ��ͬ���ж����� 1����0������*/
	LD_WriteReg(0xBD, 0x00);                   /*��ʼ�����ƼĴ��� д��00 Ȼ������ ΪASRģ�� */
	return 1;
}

/***********************************************************
* ��    �ƣ�uint8 LD_AsrAddFixed(void)
* ��    �ܣ����ʶ��ؼ�����
* ��ڲ�����  
* ���ڲ����� flag-- 1��success
* ˵    ���������߿���ѧϰ"����ʶ��оƬLD3320�߽��ؼ�.pdf"��
						���������������մ�����÷������ʶ���ʡ�
* ���÷����� 
**********************************************************/ 
uint8 LD_AsrAddFixed(void)
{
	uint8 k, flag;
	uint8 nAsrAddLength;
	#define DATE_A 20    /*�����ά��ֵ*/
	#define DATE_B 50		/*����һά��ֵ*/
	 uint8  sRecog[DATE_A][DATE_B] = {"guang dong","he nan","si chuan","xin nian hao","kai deng",
"guan deng","dan wei"," ming zi","xin ming","deng","dan","wei","kai","guan","xing","ming","xin nian","guang"};	/*��ӹؼ��ʣ��û��޸�*/
	 uint8  pCode[DATE_A] = {CODE_GUANGDONG,CODE_HENAN,CODE_SICHUAN,CODE_CHILD,CODE_OPEN,CODE_CLOSE,CODE_CPN,CODE_QM,CODE_QM,CODE_ERRO,CODE_ERRO,CODE_ERRO,CODE_ERRO,CODE_ERRO,CODE_ERRO,CODE_ERRO,CODE_CHILD,CODE_ERRO};	/*���ʶ���룬�û��޸�*/
	flag = 1;
	for (k=0; k<DATE_A; k++)
	{			
		if(LD_Check_ASRBusyFlag_b2() == 0)
		{
			flag = 0;
			break;
		}
		
		LD_WriteReg(0xc1, pCode[k] );
		LD_WriteReg(0xc3, 0 ); 
		LD_WriteReg(0x08, 0x04);
		delay_ms(1);
		LD_WriteReg(0x08, 0x00);
		delay_ms(1);

		for (nAsrAddLength=0; nAsrAddLength<DATE_B; nAsrAddLength++)
		{
			if (sRecog[k][nAsrAddLength] == 0)
				break;
			LD_WriteReg(0x5, sRecog[k][nAsrAddLength]);
		}
		LD_WriteReg(0xb9, nAsrAddLength);
		LD_WriteReg(0xb2, 0xff);
		LD_WriteReg(0x37, 0x04);
//		LD_WriteReg(0x37, 0x04);
	}	 

    return flag;
}

/***********************************************************
* ��    �ƣ� uint8 LD_GetResult(void)
* ��    �ܣ� ��ȡʶ����
* ��ڲ�����  
* ���ڲ����� ����ʶ���� LD_ReadReg(0xc5 );
* ˵    ����
* ���÷����� 
**********************************************************/ 
uint8 LD_GetResult(void)
{
	return LD_ReadReg(0xc5 );
}
/******************************************************************************* 
* Function     : LD_Init_MP3 
* Description  : --MP3��ʼ�� 
* Input	       : none 
* Output       : none 
* Created      : 2018/2/3, by WangLei
*******************************************************************************/ 
void LD_Init_MP3(void)
{
   nLD_Mode = LD_MODE_MP3;
   LD_Init_Common();
   
   LD_WriteReg(0xBD,0x02);
   LD_WriteReg(0x17, 0x48);
   delay_ms(10);
   LD_WriteReg(0x85, 0x52); 
   LD_WriteReg(0x8F, 0x00);  
   LD_WriteReg(0x81, 0x00);
   LD_WriteReg(0x83, 0x00);
   LD_WriteReg(0x8E, 0xff);
   LD_WriteReg(0x8D, 0xff);
   delay_ms(1);
   LD_WriteReg(0x87, 0xff);
   LD_WriteReg(0x89, 0xff);
   delay_ms(1);
   LD_WriteReg(0x22, 0x00);    
   LD_WriteReg(0x23, 0x00);
   LD_WriteReg(0x20, 0xef);    
   LD_WriteReg(0x21, 0x07);
   LD_WriteReg(0x24, 0x77);          
   LD_WriteReg(0x25, 0x03);
   LD_WriteReg(0x26, 0xbb);    
   LD_WriteReg(0x27, 0x01); 

}
/******************************************************************************* 
* Function     : LD_AdjustMIX2SPVolume 
* Description  : --����������������  8e�Ĵ��� ����val��ʾ����ֵ����Χ0-15
* Input	       : val 
* Output       : none 
* Created      : 2018/2/3, by WangLei
*******************************************************************************/ 
void LD_AdjustMIX2SPVolume(u8 val)		   
{
	ucSPVol = val;
	val = ((15-val)&0x0f) << 2;
	LD_WriteReg(0x8E, val | 0xc3); 
  LD_WriteReg(0x87, 0xF8);
}	
/******************************************************************************* 
* Function     : LD_play() 
* Description  : --����MP3 
* Input	       :  
* Output       :  
* Created      : 2018/2/3, by WangLei
*******************************************************************************/ 
//void LD_play(u8* Name)
//{
//	nMp3Pos=0;
//	bMp3Play=1;
//  SD_Read_function(Name);
//  nMp3Size = ConfigFile.fsize;
//	if (nMp3Pos >=  nMp3Size)
//		return ; 
//	LD_ReloadMp3Data();						   //����MP3����
//  LD_WriteReg(0xBA, 0x00); //					   ��
//	LD_WriteReg(0x17, 0x48);					   //����DSP
//	LD_WriteReg(0x33, 0x01);					   //��ʼ����
//	LD_WriteReg(0x29, 0x04);											  
//	LD_WriteReg(0x02, 0x01); 					  //����FIFO_DATA�ж�
//	LD_WriteReg(0x85, 0x5A);					  //����
//  
//}
///******************************************************************************* 
//* Function     : LD_ReloadMp3Data 
//* Description  : --װ��MP3���� 
//* Input	       :  
//* Output       :  
//* Created      : 2018/2/3, by WangLei
//*******************************************************************************/ 
//void LD_ReloadMp3Data()		  //�������� SPI
//{
//   OS_CPU_SR cpu_sr = 0;
//   OS_ENTER_CRITICAL();
//	ucStatus = LD_ReadReg(0x06);
//	while ( !(ucStatus&MASK_FIFO_STATUS_AFULL) && nMp3Pos<nMp3Size )
//	{
//        stringBuf[0]=0;
//        file_read(&ConfigFile,stringBuf,1,&stringSize);//�޸���SD��ʱ�� 1M  ���ļ����ݶ��������ݻ�����
//        LD_WriteReg(0x01,stringBuf[0]);
//        nMp3Pos++;
//        ucStatus = LD_ReadReg(0x06);
//		
//	}
//  OS_EXIT_CRITICAL();
//}
///******************************************************************************* 
//* Function     : SD_Read_function() 
//* Description  : --�Զ����ȡ���� 
//* Input	       : none 
//* Output       : none 
//* Created      : 2017/12/25, by WangLei
//*******************************************************************************/ 
//void SD_Read_function( u8* name)
//{
//   f_mount (&FsFat,"1:",1);
//   f_chdrive("1:");//���д˾���ܽ���ǰ�̷���дΪ1����sd��������Ĭ����0��
//   u8 result;
//   result= file_open(&ConfigFile,(const TCHAR*)name);
////   result = ConfigFile.fsize;
////   file_read_anyposition(&ConfigFile,stringBuf,0,10,&stringSize);
////   f_close(&ConfigFile);
//}

//#define LD_RST_H() GPIO_SetBits(GPIOA, GPIO_Pin_11)
//#define LD_RST_L() GPIO_ResetBits(GPIOA, GPIO_Pin_11)

//#define LD_CS_H()	GPIO_SetBits(GPIOA, GPIO_Pin_4)
//#define LD_CS_L()	GPIO_ResetBits(GPIOA, GPIO_Pin_4)
void LD3320_GPIO_Cfg(void)
{	
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOA,ENABLE);

	GPIO_InitStructure.GPIO_Pin =GPIO_Pin_11;//;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(GPIOA,&GPIO_InitStructure);
	LD_RST_H();
	//LD3320_SPI_init();
	 
}

void LD3320_init(void)
{
	LD3320_GPIO_Cfg();	
	//LD3320_SPI_init();	
	SIM_SPI_init();	
	LD_reset();
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

//uint8 LD_Check_ASRBusyFlag_b2()
//{
//	uint8 j;
//	uint8 flag = 0;
//	for (j=0; j<10; j++)
//	{
//	if (LD_ReadReg(0xb2) == 0x21)
//	{
//	flag = 1;
//	break;
//	}
//	delay(10); 
//	}
//	return flag;
//}
