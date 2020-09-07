#include <stm32f10x.h>  
#include "stm32f10x_it.h"
#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "as608.h"
#include "EXTI.h"
#include "myTIM.h"
#include "servo.h"
#include "usmart.h"


u8 FP_id = 0;
u8 FP_Trig_Flag = 0;
u8 servo_Stauts = 0;	//��״̬ 1����   0����

int main()
{
	u8 ensure;
	SystemInit();//����ʱ�ӳ�ʼ��
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//�����ж����ȼ�����Ϊ��2��2λ��ռ���ȼ���2λ��Ӧ���ȼ�
	DelayInit();//��ʱ������ʼ��
	uart_init(115200);//����1��ʼ��
	usart2_init(usart2_baund);//��ʼ������2,������ָ��ģ��ͨѶ
	usart3_init(115200);//WIFIģ��
	PS_StaGPIO_Init();	//��ʼ��FR��״̬����
	FP_WAK_Init();//ָ���жϳ�ʼ��
	TIM2_Int_Init(100, 7200 - 1); //T = 10ms
	user_led_init();
	//usmart_dev.init(72);
	servo_init();
	

	ensure=PS_ReadSysPara(&AS608Para);  //��AS608ģ����� 
	if(ensure==0x00)
	{
		printf("\r\nRemainNum:%d    Level:%d", AS608Para.PS_max-ValidN,AS608Para.PS_level);
	}
	GPIO_SetBits(GPIOC, GPIO_Pin_13);
	//Add_FR();	//¼ָ��
	while(1)
	{
		
//		DelayMs(100);
//		GPIO_ResetBits(GPIOC, GPIO_Pin_13);
//		DelayMs(100);

		if(FP_WAK_Trig)
		{
			press_FR();
			FP_WAK_Trig = 0;
		}

		if(FP_Trig_Flag)
		{
			GPIO_ResetBits(GPIOC, GPIO_Pin_13);
			servo_control(1);
			servo_Stauts = 1;
			FP_Trig_Flag = 0;
		}
		
	}
}


void user_led_init(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC,ENABLE);
	GPIO_DeInit(GPIOC);
	
	GPIO_InitTypeDef GPIOType = {
			GPIO_Pin_13,
			GPIO_Speed_50MHz,
			GPIO_Mode_Out_OD
	};
	
	GPIO_Init(GPIOC , &GPIOType);
}



//void WIFI_EN_GPIO_Init(void)
//{
//	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);
//	GPIO_InitTypeDef  GPIO_InitStructure;
//	
//	/* RST��IO������ */
//	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8 ;
//	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;		//�������
//	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
//	GPIO_Init(GPIOB, &GPIO_InitStructure);
//	GPIO_SetBits(GPIOB,GPIO_Pin_8);
//	

//}


void WIFI_send_id(u8 id)
{
	u8 buf[3] = {'[',id,']'};
	
	for(int i = 0; i < 3; i++){
		while(USART_GetFlagStatus(USART3,USART_FLAG_TC)==RESET); //ѭ������,ֱ���������   
		USART_SendData(USART3,buf[i]); 
	}
}





