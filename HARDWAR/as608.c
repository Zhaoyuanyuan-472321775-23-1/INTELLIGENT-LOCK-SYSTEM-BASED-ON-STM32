#include "as608.h"
#include "usart.h"
u16 user_ID;
unsigned char finger_id = 0;
unsigned char uart_rx_sta;         			//ָ�����ݽ���״̬���	
unsigned char FPM10A_RECEICE_BUFFER[16];//ָ�����ݴ洢
//FINGERPRINTͨ��Э�鶨��
unsigned char FPM10A_Get_Finger_Num[6] ={0x01,0x00,0x03,0x1d,0x00,0x21};												//���ģ������
unsigned char FPM10A_Pack_Head[6] = {0xEF,0x01,0xFF,0xFF,0xFF,0xFF};  													//Э���ͷ
unsigned char  FPM10A_Get_Img[6] = {0x01,0x00,0x03,0x01,0x00,0x05};    													//���ָ��ͼ��
unsigned char FPM10A_Search[11]={0x01,0x00,0x08,0x04,0x01,0x00,0x00,0x03,0xE7,0x00,0xF8}; 			//����ָ��������Χ0 - 999,ʹ��BUFFER1�е�����������
unsigned char  FPM10A_Img_To_Buffer1[7]={0x01,0x00,0x04,0x02,0x01,0x00,0x08}; 									//��ͼ����뵽BUFFER1
unsigned char  FPM10A_Img_To_Buffer2[7]={0x01,0x00,0x04,0x02,0x02,0x00,0x09}; 									//��ͼ����뵽BUFFER2
unsigned char  FPM10A_Reg_Model[6]={0x01,0x00,0x03,0x05,0x00,0x09}; 														//��BUFFER1��BUFFER2�ϳ�����ģ��
unsigned char  FPM10A_Delete_All_Model[6]={0x01,0x00,0x03,0x0d,0x00,0x11};											//ɾ��ָ��ģ�������е�ģ��
unsigned char  FPM10A_Delete_One_Model[10]={0x01,0x00,0x07,0x0C,0x00,0x00,0x00,0x01,0x00,0x15};																//ɾ��ָ��ģ�������е�ģ��
volatile unsigned char FPM10A_Save_Finger[9]={0x01,0x00,0x06,0x06,0x01,0x00,0x0B,0x00,0x19};		//��BUFFER1�е��������ŵ�ָ����λ��
//volatile:ϵͳ�������´������ڵ��ڴ��ȡ���ݣ���ʹ��ǰ���ָ��ոմӸô���ȡ������
/*------------------ FINGERPRINT������ --------------------------*/

void USART2_IRQHandler(void)
{
	u8 res;	      
	if(USART_GetITStatus(USART2, USART_IT_RXNE) != RESET)//���յ�����
	{
		
		res =USART_ReceiveData(USART2);	
	
		FPM10A_RECEICE_BUFFER[uart_rx_sta++] = res;              //P0��ʾ��������
		if(uart_rx_sta>15) uart_rx_sta=15;
	}  				 											 
}   

void SendData(u8 data)
{
	UsartSendData(USART2,data);
}
//���Ͱ�ͷ
void FPM10A_Cmd_Send_Pack_Head(void)
{
	int i;	
	for(i=0;i<6;i++) //��ͷ
		SendData(FPM10A_Pack_Head[i]);   	
}
//���շ������ݻ���
void FPM10A_Receive_Data()
{
	unsigned int i=1000;
	FPM10A_RECEICE_BUFFER[9]=1;
	uart_rx_sta=0;
	i=1000;
	while(i--)
	{
		delay_ms(1);
		if(uart_rx_sta>=10)
		{
			delay_ms(10);
			break;
		}
	}
}

//FINGERPRINT_���ָ��ͼ������
void FPM10A_Cmd_Get_Img(void)
{
  unsigned char i;
  FPM10A_Cmd_Send_Pack_Head(); //����ͨ��Э���ͷ
	for(i=0;i<6;i++) //�������� 0x1d
		SendData(FPM10A_Get_Img[i]);
}
//��ͼ��ת��������������Buffer1��
void FINGERPRINT_Cmd_Img_To_Buffer1(void)
{
 	unsigned char i;
	FPM10A_Cmd_Send_Pack_Head(); //����ͨ��Э���ͷ      
  for(i=0;i<7;i++)   //�������� ��ͼ��ת���� ������ ����� CHAR_buffer1
		SendData(FPM10A_Img_To_Buffer1[i]);
}
//��ͼ��ת��������������Buffer2��
void FINGERPRINT_Cmd_Img_To_Buffer2(void)
{
  unsigned char i;
	for(i=0;i<6;i++)    //���Ͱ�ͷ
		SendData(FPM10A_Pack_Head[i]);   
  for(i=0;i<7;i++)   //�������� ��ͼ��ת���� ������ ����� CHAR_buffer1
		SendData(FPM10A_Img_To_Buffer2[i]);
}
//����ȫ���û�
void FPM10A_Cmd_Search_Finger(void)
{
	unsigned char i;	   	    
	FPM10A_Cmd_Send_Pack_Head(); //����ͨ��Э���ͷ
	for(i=0;i<11;i++)
	{
		SendData(FPM10A_Search[i]);   
	}
}
//ת����������
void FPM10A_Cmd_Reg_Model(void)
{
	unsigned char i;	   
	FPM10A_Cmd_Send_Pack_Head(); //����ͨ��Э���ͷ
	for(i=0;i<6;i++)
		SendData(FPM10A_Reg_Model[i]);
}
//ɾ��ָ��ģ���������ָ��ģ��
void FINGERPRINT_Cmd_Delete_All_Model(void)
{
	unsigned char i;    
	for(i=0;i<6;i++) //��ͷ
		SendData(FPM10A_Pack_Head[i]);   
	for(i=0;i<6;i++) //����ϲ�ָ��ģ��
		SendData(FPM10A_Delete_All_Model[i]);   
}
//����ָ��
void FPM10A_Cmd_Save_Finger( unsigned int storeID )
{
		 unsigned long temp = 0;
		 unsigned char i;
		 FPM10A_Save_Finger[5] =(storeID&0xFF00)>>8;
		 FPM10A_Save_Finger[6] = (storeID&0x00FF);
		 for(i=0;i<7;i++)   //����У���
				 temp = temp + FPM10A_Save_Finger[i]; 
		 FPM10A_Save_Finger[7]=(temp & 0x00FF00) >> 8; //���У������
		 FPM10A_Save_Finger[8]= temp & 0x0000FF;		   
		 FPM10A_Cmd_Send_Pack_Head(); //����ͨ��Э���ͷ	
		 for(i=0;i<9;i++)  
				SendData(FPM10A_Save_Finger[i]);      //�������� ��ͼ��ת���� ������ ����� CHAR_buffer1
}
//���ָ��
u8 Add_FR_AtID(u8 id)
{
	FPM10A_RECEICE_BUFFER[9]=1;
	while(FPM10A_RECEICE_BUFFER[9] != 0x00) //�ȴ���ȡָ��ͼ��ɹ����߰�������
	{
		FPM10A_Cmd_Get_Img(); 	//���ָ��ͼ��
		FPM10A_Receive_Data();	//�жϽ��յ���ȷ����,����0ָ�ƻ�ȡ�ɹ�
		delay_ms(100);
	}
	FINGERPRINT_Cmd_Img_To_Buffer1(); //ͼ��ת��������������Buffer1��
	FPM10A_Receive_Data();
//	delay_ms(100);
//	if(FPM10A_RECEICE_BUFFER[9]!=0x00)  //��һ�λ�ȡͼ��ɹ�
//		return 1;
	FPM10A_RECEICE_BUFFER[9]=1;
	while((FPM10A_RECEICE_BUFFER[9] != 0x00)) //�ȴ���ȡָ��ͼ��ɹ����߰�������
	{
		FPM10A_Cmd_Get_Img(); 	//���ָ��ͼ��
		FPM10A_Receive_Data();	//�жϽ��յ���ȷ����,����0ָ�ƻ�ȡ�ɹ�
		delay_ms(100);
	}
	FINGERPRINT_Cmd_Img_To_Buffer2(); //ͼ��ת��������������Buffer1��
	FPM10A_Receive_Data();
	
	if(FPM10A_RECEICE_BUFFER[9]==0)  //��һ�λ�ȡͼ��ɹ�
	{
		FPM10A_Cmd_Reg_Model();//ת����������
		FPM10A_Receive_Data(); 
		FPM10A_Cmd_Save_Finger(id);                		         
		FPM10A_Receive_Data();
		if(FPM10A_RECEICE_BUFFER[9]==0)
		{
			return 1;
		}
		else
		{
			return 0;
		}
	}
	else
		return 0;
}
//���ָ��
u8 Add_FR()
{
	FPM10A_RECEICE_BUFFER[9]=1;
	while(FPM10A_RECEICE_BUFFER[9] != 0x00) //�ȴ���ȡָ��ͼ��ɹ����߰�������
	{
		FPM10A_Cmd_Get_Img(); 	//���ָ��ͼ��
		FPM10A_Receive_Data();	//�жϽ��յ���ȷ����,����0ָ�ƻ�ȡ�ɹ�
		delay_ms(100);
	}
	FINGERPRINT_Cmd_Img_To_Buffer1(); //ͼ��ת��������������Buffer1��
	FPM10A_Receive_Data();
//	delay_ms(100);
//	if(FPM10A_RECEICE_BUFFER[9]!=0x00)  //��һ�λ�ȡͼ��ɹ�
//		return 1;
	FPM10A_RECEICE_BUFFER[9]=1;
	while((FPM10A_RECEICE_BUFFER[9] != 0x00)) //�ȴ���ȡָ��ͼ��ɹ����߰�������
	{
		FPM10A_Cmd_Get_Img(); 	//���ָ��ͼ��
		FPM10A_Receive_Data();	//�жϽ��յ���ȷ����,����0ָ�ƻ�ȡ�ɹ�
		delay_ms(100);
	}
	FINGERPRINT_Cmd_Img_To_Buffer2(); //ͼ��ת��������������Buffer1��
	FPM10A_Receive_Data();
	
	if(FPM10A_RECEICE_BUFFER[9]==0)  //��һ�λ�ȡͼ��ɹ�
	{
		FPM10A_Cmd_Reg_Model();//ת����������
		FPM10A_Receive_Data(); 
		finger_id = PS_ValidTempleteNum();
		finger_id++;
		user_ID = finger_id;
		FPM10A_Cmd_Save_Finger(finger_id);                		         
		FPM10A_Receive_Data();
		if(FPM10A_RECEICE_BUFFER[9]==0)
		{
			return finger_id;
		}
		else
		{
			return 0;
		}
	}
	else
		return 0;
}

//����ָ��
u16 press_FR()
{
	unsigned int find_fingerid = 0;
	FPM10A_Cmd_Get_Img(); //���ָ��ͼ��
	FPM10A_Receive_Data();		
		
	if(FPM10A_RECEICE_BUFFER[9]==0)//�жϽ��յ���ȷ����,����0ָ�ƻ�ȡ�ɹ�
	{
		delay_ms(100);
		FINGERPRINT_Cmd_Img_To_Buffer1();
		FPM10A_Receive_Data();		
		FPM10A_Cmd_Search_Finger();
		FPM10A_Receive_Data();			
		if(FPM10A_RECEICE_BUFFER[9] == 0) //������  
		{
			find_fingerid = FPM10A_RECEICE_BUFFER[10]*256 + FPM10A_RECEICE_BUFFER[11];//ƴ��ָ��ID��
			return find_fingerid;
		}
		else //û���ҵ�
		{
			return 0xffff;
		}
	}
	return 0xfffe;
}
//ɾ�����д�����ָ�ƿ�
u8 PS_Empty()
{
//	unsigned char i=0;
	FINGERPRINT_Cmd_Delete_All_Model();
	finger_id = 0;
	FPM10A_Receive_Data(); 
	if(FPM10A_RECEICE_BUFFER[9] == 0)					//�ж����ݵ͵�9λ�Ƿ���յ�0
	{
		return 0;
	}
	else
		return 1;
}
//ɾ��ָ��
u8 FPM10A_Delete_One_Fingerprint(u16 id)
{
	unsigned long temp = 0;
	unsigned char i=0;
	delay_ms(500);
	FPM10A_Delete_One_Model[4] = (id & 0xFF00) >>8;
	FPM10A_Delete_One_Model[5] = (id&0x00FF);
	FPM10A_Delete_One_Model[6] = 0x00;
	FPM10A_Delete_One_Model[7] = 0x01;
	for(i=0;i<8;i++)   //����У���
		 temp = temp + FPM10A_Delete_One_Model[i]; 
	FPM10A_Delete_One_Model[8]=(temp & 0xFF00) >> 8; //���У������
	FPM10A_Delete_One_Model[9]= temp & 0x00FF;		

	for(i=0;i<6;i++) //��ͷ
		SendData(FPM10A_Pack_Head[i]);   	
	for(i=0;i<10;i++) //����ϲ�ָ��ģ��
		SendData(FPM10A_Delete_One_Model[i]);
	FPM10A_Receive_Data(); 
	if(FPM10A_RECEICE_BUFFER[9] == 0)					//�ж����ݵ͵�9λ�Ƿ���յ�0
	{
		return 0;
	}
	else
		return 1;
}
//��ȡָ�Ƹ���
u16 PS_ValidTempleteNum()
{
	unsigned char i;   
	FPM10A_Cmd_Send_Pack_Head(); //����ͨ��Э���ͷ	
	for(i=0;i<6;i++)
		SendData(FPM10A_Get_Finger_Num[i]);      //�������� 
	FPM10A_Receive_Data();
	if(FPM10A_RECEICE_BUFFER[9] == 0)					//�ж����ݵ͵�9λ�Ƿ���յ�0
	{
		return (FPM10A_RECEICE_BUFFER[10]*256)+FPM10A_RECEICE_BUFFER[11];  //
	}
	else
		return 65535;
}


