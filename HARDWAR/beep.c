#include "beep.h"


void Beep_Init(void) //������
{
	GPIO_InitTypeDef GPIO_InitStructure;
 	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);//ʹ��PORTA,PORTBʱ��
	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_8;//IO������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	  //�ٶ�Ϊ50MHz
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;    //���ó���������
 	GPIO_Init(GPIOA, &GPIO_InitStructure);         //��ʼ��GPIOB
	beep=1;
}

void beep_work(u8 time,int cnt)
{
	while(cnt--)
	{
		delay_ms(time/2);
		beep=0;
		delay_ms(time/2);
		beep=1;
	}
}

