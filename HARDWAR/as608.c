#include "as608.h"
#include "usart.h"
u16 user_ID;
unsigned char finger_id = 0;
unsigned char uart_rx_sta;         			//指纹数据接收状态标记	
unsigned char FPM10A_RECEICE_BUFFER[16];//指纹数据存储
//FINGERPRINT通信协议定义
unsigned char FPM10A_Get_Finger_Num[6] ={0x01,0x00,0x03,0x1d,0x00,0x21};												//获得模版总数
unsigned char FPM10A_Pack_Head[6] = {0xEF,0x01,0xFF,0xFF,0xFF,0xFF};  													//协议包头
unsigned char  FPM10A_Get_Img[6] = {0x01,0x00,0x03,0x01,0x00,0x05};    													//获得指纹图像
unsigned char FPM10A_Search[11]={0x01,0x00,0x08,0x04,0x01,0x00,0x00,0x03,0xE7,0x00,0xF8}; 			//搜索指纹搜索范围0 - 999,使用BUFFER1中的特征码搜索
unsigned char  FPM10A_Img_To_Buffer1[7]={0x01,0x00,0x04,0x02,0x01,0x00,0x08}; 									//将图像放入到BUFFER1
unsigned char  FPM10A_Img_To_Buffer2[7]={0x01,0x00,0x04,0x02,0x02,0x00,0x09}; 									//将图像放入到BUFFER2
unsigned char  FPM10A_Reg_Model[6]={0x01,0x00,0x03,0x05,0x00,0x09}; 														//将BUFFER1跟BUFFER2合成特征模版
unsigned char  FPM10A_Delete_All_Model[6]={0x01,0x00,0x03,0x0d,0x00,0x11};											//删除指纹模块里所有的模版
unsigned char  FPM10A_Delete_One_Model[10]={0x01,0x00,0x07,0x0C,0x00,0x00,0x00,0x01,0x00,0x15};																//删除指纹模块里所有的模版
volatile unsigned char FPM10A_Save_Finger[9]={0x01,0x00,0x06,0x06,0x01,0x00,0x0B,0x00,0x19};		//将BUFFER1中的特征码存放到指定的位置
//volatile:系统总是重新从它所在的内存读取数据，即使它前面的指令刚刚从该处读取过数据
/*------------------ FINGERPRINT命令字 --------------------------*/

void USART2_IRQHandler(void)
{
	u8 res;	      
	if(USART_GetITStatus(USART2, USART_IT_RXNE) != RESET)//接收到数据
	{
		
		res =USART_ReceiveData(USART2);	
	
		FPM10A_RECEICE_BUFFER[uart_rx_sta++] = res;              //P0显示串口数据
		if(uart_rx_sta>15) uart_rx_sta=15;
	}  				 											 
}   

void SendData(u8 data)
{
	UsartSendData(USART2,data);
}
//发送包头
void FPM10A_Cmd_Send_Pack_Head(void)
{
	int i;	
	for(i=0;i<6;i++) //包头
		SendData(FPM10A_Pack_Head[i]);   	
}
//接收反馈数据缓冲
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

//FINGERPRINT_获得指纹图像命令
void FPM10A_Cmd_Get_Img(void)
{
  unsigned char i;
  FPM10A_Cmd_Send_Pack_Head(); //发送通信协议包头
	for(i=0;i<6;i++) //发送命令 0x1d
		SendData(FPM10A_Get_Img[i]);
}
//讲图像转换成特征码存放在Buffer1中
void FINGERPRINT_Cmd_Img_To_Buffer1(void)
{
 	unsigned char i;
	FPM10A_Cmd_Send_Pack_Head(); //发送通信协议包头      
  for(i=0;i<7;i++)   //发送命令 将图像转换成 特征码 存放在 CHAR_buffer1
		SendData(FPM10A_Img_To_Buffer1[i]);
}
//将图像转换成特征码存放在Buffer2中
void FINGERPRINT_Cmd_Img_To_Buffer2(void)
{
  unsigned char i;
	for(i=0;i<6;i++)    //发送包头
		SendData(FPM10A_Pack_Head[i]);   
  for(i=0;i<7;i++)   //发送命令 将图像转换成 特征码 存放在 CHAR_buffer1
		SendData(FPM10A_Img_To_Buffer2[i]);
}
//搜索全部用户
void FPM10A_Cmd_Search_Finger(void)
{
	unsigned char i;	   	    
	FPM10A_Cmd_Send_Pack_Head(); //发送通信协议包头
	for(i=0;i<11;i++)
	{
		SendData(FPM10A_Search[i]);   
	}
}
//转换成特征码
void FPM10A_Cmd_Reg_Model(void)
{
	unsigned char i;	   
	FPM10A_Cmd_Send_Pack_Head(); //发送通信协议包头
	for(i=0;i<6;i++)
		SendData(FPM10A_Reg_Model[i]);
}
//删除指纹模块里的所有指纹模版
void FINGERPRINT_Cmd_Delete_All_Model(void)
{
	unsigned char i;    
	for(i=0;i<6;i++) //包头
		SendData(FPM10A_Pack_Head[i]);   
	for(i=0;i<6;i++) //命令合并指纹模版
		SendData(FPM10A_Delete_All_Model[i]);   
}
//保存指纹
void FPM10A_Cmd_Save_Finger( unsigned int storeID )
{
		 unsigned long temp = 0;
		 unsigned char i;
		 FPM10A_Save_Finger[5] =(storeID&0xFF00)>>8;
		 FPM10A_Save_Finger[6] = (storeID&0x00FF);
		 for(i=0;i<7;i++)   //计算校验和
				 temp = temp + FPM10A_Save_Finger[i]; 
		 FPM10A_Save_Finger[7]=(temp & 0x00FF00) >> 8; //存放校验数据
		 FPM10A_Save_Finger[8]= temp & 0x0000FF;		   
		 FPM10A_Cmd_Send_Pack_Head(); //发送通信协议包头	
		 for(i=0;i<9;i++)  
				SendData(FPM10A_Save_Finger[i]);      //发送命令 将图像转换成 特征码 存放在 CHAR_buffer1
}
//添加指纹
u8 Add_FR_AtID(u8 id)
{
	FPM10A_RECEICE_BUFFER[9]=1;
	while(FPM10A_RECEICE_BUFFER[9] != 0x00) //等待获取指纹图像成功或者按键按下
	{
		FPM10A_Cmd_Get_Img(); 	//获得指纹图像
		FPM10A_Receive_Data();	//判断接收到的确认码,等于0指纹获取成功
		delay_ms(100);
	}
	FINGERPRINT_Cmd_Img_To_Buffer1(); //图像转换成特征码存放在Buffer1中
	FPM10A_Receive_Data();
//	delay_ms(100);
//	if(FPM10A_RECEICE_BUFFER[9]!=0x00)  //第一次获取图像成功
//		return 1;
	FPM10A_RECEICE_BUFFER[9]=1;
	while((FPM10A_RECEICE_BUFFER[9] != 0x00)) //等待获取指纹图像成功或者按键按下
	{
		FPM10A_Cmd_Get_Img(); 	//获得指纹图像
		FPM10A_Receive_Data();	//判断接收到的确认码,等于0指纹获取成功
		delay_ms(100);
	}
	FINGERPRINT_Cmd_Img_To_Buffer2(); //图像转换成特征码存放在Buffer1中
	FPM10A_Receive_Data();
	
	if(FPM10A_RECEICE_BUFFER[9]==0)  //第一次获取图像成功
	{
		FPM10A_Cmd_Reg_Model();//转换成特征码
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
//添加指纹
u8 Add_FR()
{
	FPM10A_RECEICE_BUFFER[9]=1;
	while(FPM10A_RECEICE_BUFFER[9] != 0x00) //等待获取指纹图像成功或者按键按下
	{
		FPM10A_Cmd_Get_Img(); 	//获得指纹图像
		FPM10A_Receive_Data();	//判断接收到的确认码,等于0指纹获取成功
		delay_ms(100);
	}
	FINGERPRINT_Cmd_Img_To_Buffer1(); //图像转换成特征码存放在Buffer1中
	FPM10A_Receive_Data();
//	delay_ms(100);
//	if(FPM10A_RECEICE_BUFFER[9]!=0x00)  //第一次获取图像成功
//		return 1;
	FPM10A_RECEICE_BUFFER[9]=1;
	while((FPM10A_RECEICE_BUFFER[9] != 0x00)) //等待获取指纹图像成功或者按键按下
	{
		FPM10A_Cmd_Get_Img(); 	//获得指纹图像
		FPM10A_Receive_Data();	//判断接收到的确认码,等于0指纹获取成功
		delay_ms(100);
	}
	FINGERPRINT_Cmd_Img_To_Buffer2(); //图像转换成特征码存放在Buffer1中
	FPM10A_Receive_Data();
	
	if(FPM10A_RECEICE_BUFFER[9]==0)  //第一次获取图像成功
	{
		FPM10A_Cmd_Reg_Model();//转换成特征码
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

//搜索指纹
u16 press_FR()
{
	unsigned int find_fingerid = 0;
	FPM10A_Cmd_Get_Img(); //获得指纹图像
	FPM10A_Receive_Data();		
		
	if(FPM10A_RECEICE_BUFFER[9]==0)//判断接收到的确认码,等于0指纹获取成功
	{
		delay_ms(100);
		FINGERPRINT_Cmd_Img_To_Buffer1();
		FPM10A_Receive_Data();		
		FPM10A_Cmd_Search_Finger();
		FPM10A_Receive_Data();			
		if(FPM10A_RECEICE_BUFFER[9] == 0) //搜索到  
		{
			find_fingerid = FPM10A_RECEICE_BUFFER[10]*256 + FPM10A_RECEICE_BUFFER[11];//拼接指纹ID数
			return find_fingerid;
		}
		else //没有找到
		{
			return 0xffff;
		}
	}
	return 0xfffe;
}
//删除所有存贮的指纹库
u8 PS_Empty()
{
//	unsigned char i=0;
	FINGERPRINT_Cmd_Delete_All_Model();
	finger_id = 0;
	FPM10A_Receive_Data(); 
	if(FPM10A_RECEICE_BUFFER[9] == 0)					//判断数据低第9位是否接收到0
	{
		return 0;
	}
	else
		return 1;
}
//删除指纹
u8 FPM10A_Delete_One_Fingerprint(u16 id)
{
	unsigned long temp = 0;
	unsigned char i=0;
	delay_ms(500);
	FPM10A_Delete_One_Model[4] = (id & 0xFF00) >>8;
	FPM10A_Delete_One_Model[5] = (id&0x00FF);
	FPM10A_Delete_One_Model[6] = 0x00;
	FPM10A_Delete_One_Model[7] = 0x01;
	for(i=0;i<8;i++)   //计算校验和
		 temp = temp + FPM10A_Delete_One_Model[i]; 
	FPM10A_Delete_One_Model[8]=(temp & 0xFF00) >> 8; //存放校验数据
	FPM10A_Delete_One_Model[9]= temp & 0x00FF;		

	for(i=0;i<6;i++) //包头
		SendData(FPM10A_Pack_Head[i]);   	
	for(i=0;i<10;i++) //命令合并指纹模版
		SendData(FPM10A_Delete_One_Model[i]);
	FPM10A_Receive_Data(); 
	if(FPM10A_RECEICE_BUFFER[9] == 0)					//判断数据低第9位是否接收到0
	{
		return 0;
	}
	else
		return 1;
}
//读取指纹个数
u16 PS_ValidTempleteNum()
{
	unsigned char i;   
	FPM10A_Cmd_Send_Pack_Head(); //发送通信协议包头	
	for(i=0;i<6;i++)
		SendData(FPM10A_Get_Finger_Num[i]);      //发送命令 
	FPM10A_Receive_Data();
	if(FPM10A_RECEICE_BUFFER[9] == 0)					//判断数据低第9位是否接收到0
	{
		return (FPM10A_RECEICE_BUFFER[10]*256)+FPM10A_RECEICE_BUFFER[11];  //
	}
	else
		return 65535;
}


