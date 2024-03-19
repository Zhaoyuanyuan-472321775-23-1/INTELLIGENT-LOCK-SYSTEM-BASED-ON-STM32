#include "beep.h"


void Beep_Init(void) //蜂鸣器
{
	GPIO_InitTypeDef GPIO_InitStructure;
 	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);//使能PORTA,PORTB时钟
	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_8;//IO口配置
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	  //速度为50MHz
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;    //设置成上拉输入
 	GPIO_Init(GPIOA, &GPIO_InitStructure);         //初始化GPIOB
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

