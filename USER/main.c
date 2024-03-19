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
u8 open_door_flag=0; 													//���ű�־λ
u8 Password[6], Password2[6]={"123456"}; 			//��λ�û����룬����Ա����
u8 password_eeror,figer_error;		//�������

void open_door()  		//���ź���
{
	if(open_door_flag>0)   //ok���Ϳ���
	{
		UsartPrintf(USART3,"OPEN"); //���͸�LCD	//���͸�LCD��ʾ
		Relay=0;
		delay_ms(1000);
		delay_ms(1000);
		Relay=1;
		if(open_door_flag==1)//�ǹ���Ա���룬�Զ��ظ�ԭʼ״̬
			open_door_flag=0;
		USART3_RX_STA=0;
		memset(USART3_RX_BUF,0,sizeof(USART3_RX_BUF));
		UsartPrintf(USART3,"  "); //���͸�LCD	//���͸�LCD��ʾ
	}
}

void face_check() //����������֤
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
void ADD_finger()			//���ָ��
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
void finger_check() 	//ˢָ�ƿ�����֤
{
	u16 uid;
	uid=press_FR();
	if(uid<300)
	{
		open_door_flag=1;
		UsartPrintf(USART1,"ָ��ʶ��ɹ�");
		UsartPrintf(USART3,"ID:%d",uid);
		figer_error=0;
		delay_ms(1000);
		delay_ms(1000);
	}
	else if(uid==0xffff)
	{		
		UsartPrintf(USART1,"ָ��ʶ��ʧ��");
		UsartPrintf(USART3,"Error",uid);
		delay_ms(1000);
		delay_ms(1000);
		figer_error++;
		UsartPrintf(USART3," ");
	}
}
void finger_admin()		//ָ�ƹ���
{
	while(key!='*')  //*�˳�
	{
		key=Key44_Scan();
		switch (key)
		{
			case'A' :ADD_finger();break;//���
			case'B' :PS_Empty();UsartPrintf(USART3,"Delet ALL Finger");delay_ms(1700);UsartPrintf(USART3,"  ");break;//ɾ��
			default :UsartPrintf(USART3,"Finger Admin");delay_ms(200);break;//������ʾ����				
		}
	}
}
int main(void) //������
{	
  vu16 falsh,reclen,t=0;
	SystemInit();
	delay_init();
	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); //   2���жϷ��黯
	usart3_Init(115200);			//����3��ʼ��
	usart2_Init(57600);			//����2��ʼ��
	usart1_Init(9600);			//����1��ʼ��
	delay_ms(1000);
	Beep_Init();
	Key44_Init(); 						//���󰴼���ʼ
	Relay_Init();
	Relay = 1;
	STMFLASH_Read(FLASH_PASSWORD_SAVE_ADDR,(u16*)Password,6);  //��ȡ���� ���س����������0XFF
	UsartPrintf(USART3," ");
  while(1)
	{
		delay_ms(1);
		if(pass_cnt==0)				//û�������������ʱ��
		{
			if(t++>200)
			{
				t=0;
				finger_check();
			}
				face_check();				//��������
		}
		password_cheak();			//�������
		
		open_door();				//���ź���
//		if(password_eeror>2)
//		{
//			reclen=10;
//			while(reclen--)
//			{
//				UsartPrintf(USART3,"Locked:%d",reclen);	//���͸�LCD��ʾ
//				delay_ms(1000);
//			}
//			UsartPrintf(USART3," ");	//���͸�LCD��ʾ
//			password_eeror=2; //�ٴ�������������
//		}
		while(open_door_flag==2)  //�������Ա�����������
		{
			key=Key44_Scan();
			switch (key)
			{
				case'C' :password_admin();break;						//�������
				case'A' :finger_admin();	break;						//ָ�ƹ���
				case'*' :open_door_flag=0;UsartPrintf(USART3,"  ");	break;//�˳�
				default :UsartPrintf(USART3,"User Admin");delay_ms(200);break;
			}
		}
		
	}
}






