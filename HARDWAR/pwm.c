
/*********************************************************

TIM4���4·pwm,��������ռ�ձ�

**********************************************************/

#include "pwm.h"
#include "delay.h"
void TIM4_PWM_Init(u16 arr,u16 psc)
{  

	GPIO_InitTypeDef GPIO_InitStructure;
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_OCInitTypeDef  TIM_OCInitStructure;

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);// 
 	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB , ENABLE);  //

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9; 
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;  //
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	// �Զ���װ�ؼĴ�����ֵ���ۼ�TIM_Period+1��Ƶ�ʺ����һ�����»����ж�
	TIM_TimeBaseStructure.TIM_Period=arr;	
	// ����CNT��������ʱ�� = Fck_int/(psc+1)
	TIM_TimeBaseStructure.TIM_Prescaler=psc;	
	// ʱ�ӷ�Ƶ���� ��������������ʱ�䣬û�õ�������
	TIM_TimeBaseStructure.TIM_ClockDivision=TIM_CKD_DIV1;		
	// ����������ģʽ������Ϊ���ϼ���
	TIM_TimeBaseStructure.TIM_CounterMode=TIM_CounterMode_Up;		
	// �ظ���������ֵ��û�õ���������������
	TIM_TimeBaseStructure.TIM_RepetitionCounter=0;	
	// ��ʼ����ʱ��
	TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure);
 
 
 
	// ����ΪPWMģʽ1
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;   //����ʱ��ֵС�ڱȽ����趨ֵʱ��TIMX����Ŵ�ʱ�����Ч�ߵ�λ
	// ���ʹ��
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	// �������ʹ��
	TIM_OCInitStructure.TIM_OutputNState = TIM_OutputNState_Enable; 
	// ����ռ�ձȴ�С
	TIM_OCInitStructure.TIM_Pulse = 0;
	// ���ͨ����ƽ��������
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
	// �������ͨ����ƽ��������
	TIM_OCInitStructure.TIM_OCNPolarity = TIM_OCNPolarity_High;
	// ���ͨ�����е�ƽ��������
	TIM_OCInitStructure.TIM_OCIdleState = TIM_OCIdleState_Set;
	// �������ͨ�����е�ƽ��������
	TIM_OCInitStructure.TIM_OCNIdleState = TIM_OCNIdleState_Reset;
	
	TIM_OC4Init(TIM4, &TIM_OCInitStructure);
	TIM_OC4PreloadConfig(TIM4, TIM_OCPreload_Enable);
	
	// ʹ�ܼ�����
	TIM_Cmd(TIM4, ENABLE);
}
/*
angle1: �������սǶ�
servo(50,150);
angle2: ƽʱ���ֽǶ�
*/
void servo(u16 angle1,u16 angle2)
{
	TIM_Cmd(TIM4, ENABLE);
	TIM_SetCompare4(TIM4,angle1);
	delay_ms(1000);
	delay_ms(1000);
	TIM_SetCompare4(TIM4,angle2);
	delay_ms(1000);
	TIM_SetCompare4(TIM4,0);
}


