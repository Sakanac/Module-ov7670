#include <reg52.h>      					//  包含51单片机寄存器定义的头文件
#include <intrins.h>    					//包含_nop_()函数定义的头文件
#include <string.h>
#include <12864.h>
#include <KEYPAD4_3.h>
#include "uart.h"
#include "delay.h"
#include "buzz.h"
#include "FPM10A.h"
#include "led.h"
#define N 8
#define	NOP5() {_nop_();_nop_();_nop_();_nop_();_nop_();}	// 执行5个空操作,延时5微秒
unsigned char read=0;  						//读取成功为0，否则为1
unsigned char write=0; 						//写入成功为0，否则为1
unsigned char Rbuff[10]={0}; 
unsigned char Wbuff[10]="0000";	 	//初始密码
sbit SDA=P2^7;          
sbit SCL=P2^6;          
sbit led = P2^0;
unsigned char str[]="123456789*0#";
unsigned char s[10]={0};

 void delay_ms(unsigned int n) //延时程序
{
   unsigned int i,j;	
	 for(i=0;i<n;i++)
	  for(j=0;j<110;j++);		 
 }

 
 /***************************************************
函数功能：开始数据传送
***************************************************/
void start()
// 开始位
{
	SDA = 1;    //SDA初始化为高电平“1”
    SCL = 1;    //开始数据传送时，要求SCL为高电平“1”
	NOP5();
	SDA = 0;    //SDA的下降沿被认为是开始信号
	NOP5();
	SCL = 0;    //SCL为低电平时，SDA上数据才允许变化(即允许以后的数据传递）  
}
/***************************************************
函数功能：结束数据传送
***************************************************/
void stop()
// 停止位
{
	SDA = 0;     //SDA初始化为低电平“0”	_n
	SCL = 1;     //结束数据传送时，要求SCL为高电平“1”
	NOP5();
	SDA = 1;    //SDA的上升沿被认为是结束信号
	NOP5();	
}
/***************************************************
函数功能：发送应答/非应答信号
***************************************************/
void Send_Ack(bit ack)
//ack=0表示应答；ack=1表示非应答
{
	SCL = 0; //拉低SCL为输出数据到SDA做准备
	SDA = ack; //输出ack到SDA，表示应答/不应答
	SCL = 1;
	NOP5();   //NOP5()>4us
    SCL = 0;    //SCL从0-1-0为一个完整的时钟周期
    SDA=1;//释放总线
}
/***************************************************
函数功能：查询/接收应答信号
***************************************************/
unsigned char Rec_Ack(void)    
//返回值为0表示应答，为1表示非应答
{unsigned char i,ack;
 SCL=0;  //拉低SCL为输出数据到SDA做准备
 SDA = 1; //释放总线，让从设备能输出数据到SDA
 SCL=1;
 while((SDA==1)&&(i<250)) i++;
/*等待从设备发送应答信号，若为0，表示应答，循环结束；若为1，可能是从机还未将信号送上来，循环延时到i>250，若仍为1，此时才认为是未应答*/
 ack=SDA; 
 SCL = 0;//SCL从0-1-0为一个完整的时钟周期
 SDA=1; //释放总线
 return(ack);
}
/***************************************************
函数功能：从AT24Cxx读取数据
出口参数：recbyte
***************************************************/
unsigned char ReadData()
// 从AT24Cxx中读取一个字节数据到MCU
{
	unsigned char i;
	unsigned char recbyte;   //储存从AT24Cxx中读出的数据
	SCL=0;         //拉低SCL为从设备输出数据到SDA做准备
    SDA=1; //释放总线
    for(i = 0; i < 8; i++)
	{
		SCL = 1;                //SCL置为高电平
		NOP5();
        recbyte=(recbyte<<1)|SDA;       //读SDA
        SCL=0;                       //SCL从0-1-0为一个完整的时钟周期
    }
    SDA=1;
	return(recbyte);         //返回所读数据
}
/***************************************************
函数功能：向AT24Cxx的当前地址写入数据
入口参数：sendbyte (储存待写入的数据）
***************************************************/
//在调用此数据写入函数前需首先调用开始函数start(),所以SCL=0
void WriteData(unsigned char sendbyte)
{
	unsigned char i;
	for(i = 0; i < 8; i++)		// 循环移入8个位
	{
    	sendbyte<<=1; //左移时最低位补0，最高位移入PSW的CY位
        SDA=CY;        //输出数据到SDA
        SCL = 1;            //在SCL的上升沿将数据写入AT24Cxx      
   	    NOP5();                 
	    SCL = 0;            //将SCL重新置为低电平，以在SCＬ线形成传送数据所需的８个脉冲
	 }
	SDA = 1;  // 发送设备（主机）应在时钟脉冲的高电平期间(SCL=1)释放SDA线，
}
/***************************************************
函数功能：向AT24Cxx中的指定地址写入数据
入口参数：add (储存指定的地址）；dat(储存待写入的数据）
***************************************************/
void WriteRom(unsigned char add, unsigned char dat[],unsigned char j)
// 将数组里的j个字节的数据写入到起始地址为addr处的连续区域中
{  unsigned char flag=0,k=0,i=3;//i为允许最大重写次数，若出现i次操作失效后，则函数中止操作，并返回
   while(i--)
   {start();             //开始数据传递
	WriteData(0xa0);  //选择要操作的AT24Cxx芯片，并告知要对其写入数据，器件地址以及写入操作为1010 0000B（0xa0）
	if(Rec_Ack()) continue;
    WriteData(add);   //写入器件内部地址
	if(Rec_Ack()) continue;
    while(j--)
	{WriteData(dat[k++]);   //向当前地址（上面指定的地址）写入数据
	 if(!Rec_Ack()) continue;
     flag=1;
	 break;
	}
	if(flag)  continue;/*能不能改成：if(j>=0)  continue? （j>=0意味着数据未写完，即出现了未应答，要重写）*/
	break;
   }
   stop();                //停止数据传递         
   if(i<0) write=1;     
}
/***************************************************
函数功能：从AT24Cxx中的指定地址读取数据
入口参数：set_addr
出口参数：x 
***************************************************/
void ReadRom(unsigned char set_addr,unsigned char dat[],unsigned char j)
// 在指定地址开始连续读取j个字节，并将数据存放到数组中
{ unsigned char i=3,k=0;//i为允许最大重读次数，若出现i次操作失效后，则函数中止操作，并返回
  while(i--)
  {	start();             //开始数据传递
	WriteData(0xa0);     //选择要操作的AT24Cxx芯片，并告知要对其写入数据，器件地址以及写入操作为1010 0000B（0xa0）
	if(Rec_Ack()) continue;
    WriteData(set_addr);       //写入指定地址
	if(Rec_Ack()) continue;
    start();//调启动总线函数
    WriteData(0xa1);    //选择器件要操作的AT24Cxx芯片，并告知要对其读取数据，器件地址以及读取操作为1010 0001B（0xa1）
    if(Rec_Ack()) continue;//如果操作失败,就退出
    while(--j)
	{ dat[k++]=ReadData();
	  Send_Ack(0);	  
	}//接收前j-1个字节，并应答
	dat[k++]=ReadData();//接收最后一个字节
	Send_Ack(1);//向从设备发出非应答信号，结束数据传输
    break;
  } 
  stop();
  if(i<0) read=1;  
}




void start_screen()
{
	lcd_wcmd(0x01);
	lcd12864_pos(0,0);
 	LCD12864disp("欢迎使用考勤机！");
	lcd12864_pos(1,0);
	LCD12864disp("1.使用指纹解锁");
	lcd12864_pos(2,0);
	LCD12864disp("2.使用密码解锁");
}

//密码错误时
void wrong(){
	lcd_wcmd(0x01);
	lcd12864_pos(0,0);
 	LCD12864disp("密码错误！");
	lcd12864_pos(1,0);
	LCD12864disp("请重新输入！");
	delay_ms(1000);
	lcd_wcmd(0x01);
	lcd12864_pos(0,0);
	LCD12864disp("请输入密码：");
	lcd12864_pos(1,0);
}


//密码正确时
unsigned char correct(){
  	unsigned char key;
		lcd_wcmd(0x01);
		lcd12864_pos(0,0);
		LCD12864disp("开锁成功！");
		lcd12864_pos(1,0);
		LCD12864disp("是否修改密码？");
		lcd12864_pos(2,0);
		LCD12864disp("１：是，２：否");
		lcd12864_pos(3,0);
		while(1){
			key=matrixkeyscan();
			if(key==1 || key==2)
				{
				if(key==1)  LCD12864disp("１");
				else        LCD12864disp("２");
				delay_ms(1000);
				return key;
				}
		}
}

//改密码
void change(){
	unsigned char key,a=0;
	unsigned char s1[10]={0};
	unsigned char s2[10]={0};
	lcd_wcmd(0x01);
	lcd12864_pos(0,0);
	LCD12864disp("请输入密码：");
	lcd12864_pos(1,0);
	while(1){
		key = matrixkeyscan();
		if(key<12)
		{
			if(key==11)	 LCD12864_backspace();
			if(key==10)  break;
			if(key!=10 && key!=11){s1[a++]=str[key]; LCD12864disp("＊");}
		}
	}
	lcd12864_pos(2,0);
	LCD12864disp("再次输入密码：");
	lcd12864_pos(3,0);
	a=0;
	while(1){
	key=matrixkeyscan();	
	if(key<14)
	{
		if(key==11)	 LCD12864_backspace();
		if(key==10)  break;
		if(key!=10 && key!=11){s2[a++]=str[key]; LCD12864disp("＊");}
		}
	}
	if(strcmp(s1,s2)==0)
		{
		SDA = 1;           						//I2C初始化：SDA=1,SCL=1,使主从设备处于空闲状态
   	SCL = 1; 
		memset(Rbuff,0,sizeof(s));   	//再次初始化数组
		strcpy(Rbuff,s2); 
		WriteRom(0x32,s2,10);   			//向起始地址为“0x32”的连续区域中写入数组Wbuff中的5个数据
   	ReadRom(0x32,Rbuff,10);    		//从起始地址为“0x32”的连续区域中读取5个数据到数组Rbuff中
	   	
		lcd_wcmd(0x01);
		lcd12864_pos(0,0);
		LCD12864disp("密码修改成功");
		delay_ms(2000);

	   	lcd_wcmd(0x01);
		lcd12864_pos(0,0);
		LCD12864disp("请输入密码：");
		lcd12864_pos(1,0);
	}
	else
	{
		lcd_wcmd(0x01);
		lcd12864_pos(0,0);
		LCD12864disp("两次输入不一致");
		lcd12864_pos(1,0);
		LCD12864disp("密码修改失败");
		delay_ms(2000);

		lcd_wcmd(0x01);
		lcd12864_pos(0,0);
		LCD12864disp("请输入密码：");
		lcd12864_pos(1,0);	
	}
}

void main()
{  
   int a=0,flage=1;
   unsigned char i=0;
   unsigned char key,f;
   SDA = 1;           				//I2C初始化：SDA=1,SCL=1,使主从设备处于空闲状态
   SCL = 1;  
   //led = 1;	       
   WriteRom(0x32,Wbuff,10);   //向起始地址为“0x32”的连续区域中写入数组Wbuff中的5个数据
   ReadRom(0x32,Rbuff,10);    //从起始地址为“0x32”的连续区域中读取5个数据到数组Rbuff中
   	
    //初始化
	
	lcd12864_init();
  start_screen();
	if(key<14)
	{
		if(key==2)
		{
			lcd_wcmd(0x01);
			lcd12864_pos(0,0);
			LCD12864disp("请输入密码：");
			lcd12864_pos(1,0);
			while(1)
				{
					key=matrixkeyscan();
					if(key<14)
				{ 
					if(key==11) {LCD12864_backspace();}  		//删除
					if(key==10) 																	//密码确认
				{
			   	if(strcmp(Rbuff,s)==0) flage=0;  					//正确
				  else flage=1;					   										//错误
				  if(flage==0)
				{
				   	led=0;   																//灯亮
				  	f=correct();
					  if(f==0)
						{					      
							change();
					  }
					  if(f==1){
					  	lcd_wcmd(0x01);
					  	lcd12864_pos(0,0);
							LCD12864disp("请输入密码：");
							lcd12864_pos(1,0);	
					  }
					  led = 1;   								//灯灭
					  a=0;
					  memset(s,0,sizeof(s));   	//再次初始化数组
				  }
				  else
				  {
				  	wrong();
					  a=0;
					  memset(s,0,sizeof(s));   	//再次初始化数组
				  }
			   } 
			  if(key!=10 && key!=11) {s[a++]=str[key]; LCD12864disp("＊");}
			  }	  
			}
		}
		if(key==1)
		{			
			Uart_Init();
			Led_Init();
			Buzz_Init();
			Device_Check();
			Delay_Ms(500); //延时500MS，等待指纹模块复位
			Buzz_Times(1);
			
			lcd_wcmd(0x01);
			lcd12864_pos(0,0);
			LCD12864disp("指纹解锁系统");
			lcd12864_pos(1,0);
			LCD12864disp("  1.添加指纹");
			lcd12864_pos(2,0);
			LCD12864disp("  2.搜索指纹");
			lcd12864_pos(3,0);
			LCD12864disp("  3.清空指纹库");

	while(1)
	{
		if(key<14)
			{	 						
			 switch(key)
				{
					case 1:	 //添加指纹
					Led_Green_On();
					FPM10A_Add_Fingerprint();
					Led_Green_Off();
					start_screen();	
					break; 
												
					case 2://搜索指纹
					Led_Yellow_On();						
					FPM10A_Find_Fingerprint();
					Led_Yellow_Off();							
					start_screen();																	
					break;									
					
					case 3:	//清空指纹
					FPM10A_Delete_All_Fingerprint();								
					start_screen();
		  		break;
				}
			}	
	}
}
		}
		
		
		
		
  }		 


