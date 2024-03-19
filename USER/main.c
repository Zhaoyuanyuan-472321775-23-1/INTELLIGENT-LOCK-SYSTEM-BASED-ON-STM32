#include "stm32f10x.h"
#include <stdio.h>
#include <string.h>	
#include "delay.h"
#include "sys.h"
#include "pwm.h"
#include "usart.h"
#include "timer.h"
#include "key4_4.h"
#include "stmflash.h"
#include "as608.h"
#include "IO_Init.h"
int key;
u8 open_door_flag=0; 													//开门标志位
u8 Password[6], Password2[6]={"123456"}; 			//六位用户密码，管理员密码
u8 password_eeror,figer_error;		//错误次数

void open_door()  		//开门函数
{
	if(open_door_flag>0)   //ok，就开门
	{
		UsartPrintf(USART3,"OPEN"); //发送给LCD	//发送给LCD显示
		Relay=0;
		delay_ms(1000);
		delay_ms(1000);
		Relay=1;
		if(open_door_flag==1)//非管理员密码，自动回复原始状态
			open_door_flag=0;
		USART3_RX_STA=0;
		memset(USART3_RX_BUF,0,sizeof(USART3_RX_BUF));
		UsartPrintf(USART3,"  "); //发送给LCD	//发送给LCD显示
	}
}

void face_check() //人脸开门验证
{
	if(USART3_RX_STA>5)
	{
		delay_ms(10);
		if(strstr((const char *)USART3_RX_BUF,"success"))
		{
			open_door_flag=1;
		}
		USART3_RX_STA=0;
		memset(USART3_RX_BUF,0,sizeof(USART3_RX_BUF));
	}
}
void ADD_finger()			//添加指纹
{
	int i=0;
	UsartPrintf(USART3,"Put Finger");
	i=Add_FR();
	if(i>0)
		UsartPrintf(USART3,"ADD:%d",i);
	else
		UsartPrintf(USART3,"ADD Fail");
	delay_ms(1000);
	UsartPrintf(USART3," ");
}
void finger_check() 	//刷指纹开门验证
{
	u16 uid;
	uid=press_FR();
	if(uid<300)
	{
		open_door_flag=1;
		UsartPrintf(USART1,"指纹识别成功");
		UsartPrintf(USART3,"ID:%d",uid);
		figer_error=0;
		delay_ms(1000);
		delay_ms(1000);
	}
	else if(uid==0xffff)
	{		
		UsartPrintf(USART1,"指纹识别失败");
		UsartPrintf(USART3,"Error",uid);
		delay_ms(1000);
		delay_ms(1000);
		figer_error++;
		UsartPrintf(USART3," ");
	}
}
void finger_admin()		//指纹管理
{
	while(key!='*')  //*退出
	{
		key=Key44_Scan();
		switch (key)
		{
			case'A' :ADD_finger();break;//添加
			case'B' :PS_Empty();UsartPrintf(USART3,"Delet ALL Finger");delay_ms(1700);UsartPrintf(USART3,"  ");break;//删除
			default :UsartPrintf(USART3,"Finger Admin");delay_ms(200);break;//管理显示界面				
		}
	}
}
int main(void) //主函数
{	
  vu16 falsh,reclen,t=0;
	SystemInit();
	delay_init();
	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); //   2组中断分组化
	usart3_Init(115200);			//串口3初始化
	usart2_Init(57600);			//串口2初始化
	usart1_Init(9600);			//串口1初始化
	delay_ms(1000);
	Beep_Init();
	Key44_Init(); 						//矩阵按键初始
	Relay_Init();
	Relay = 1;
	STMFLASH_Read(FLASH_PASSWORD_SAVE_ADDR,(u16*)Password,6);  //读取密码 下载程序后，数据是0XFF
	UsartPrintf(USART3," ");
  while(1)
	{
		delay_ms(1);
		if(pass_cnt==0)				//没有在密码输入的时候
		{
			if(t++>200)
			{
				t=0;
				finger_check();
			}
				face_check();				//人脸解锁
		}
		password_cheak();			//密码检验
		
		open_door();				//开门函数
//		if(password_eeror>2)
//		{
//			reclen=10;
//			while(reclen--)
//			{
//				UsartPrintf(USART3,"Locked:%d",reclen);	//发送给LCD显示
//				delay_ms(1000);
//			}
//			UsartPrintf(USART3," ");	//发送给LCD显示
//			password_eeror=2; //再次密码错误就锁定
//		}
		while(open_door_flag==2)  //输入管理员密码进入设置
		{
			key=Key44_Scan();
			switch (key)
			{
				case'C' :password_admin();break;						//密码管理
				case'A' :finger_admin();	break;						//指纹管理
				case'*' :open_door_flag=0;UsartPrintf(USART3,"  ");	break;//退出
				default :UsartPrintf(USART3,"User Admin");delay_ms(200);break;
			}
		}
		
	}
}






