#include <reg52.h>      					//  ����51��Ƭ���Ĵ��������ͷ�ļ�
#include <intrins.h>    					//����_nop_()���������ͷ�ļ�
#include <string.h>
#include <12864.h>
#include <KEYPAD4_3.h>
#include "uart.h"
#include "delay.h"
#include "buzz.h"
#include "FPM10A.h"
#include "led.h"
#define N 8
#define	NOP5() {_nop_();_nop_();_nop_();_nop_();_nop_();}	// ִ��5���ղ���,��ʱ5΢��
unsigned char read=0;  						//��ȡ�ɹ�Ϊ0������Ϊ1
unsigned char write=0; 						//д��ɹ�Ϊ0������Ϊ1
unsigned char Rbuff[10]={0}; 
unsigned char Wbuff[10]="0000";	 	//��ʼ����
sbit SDA=P2^7;          
sbit SCL=P2^6;          
sbit led = P2^0;
unsigned char str[]="123456789*0#";
unsigned char s[10]={0};

 void delay_ms(unsigned int n) //��ʱ����
{
   unsigned int i,j;	
	 for(i=0;i<n;i++)
	  for(j=0;j<110;j++);		 
 }

 
 /***************************************************
�������ܣ���ʼ���ݴ���
***************************************************/
void start()
// ��ʼλ
{
	SDA = 1;    //SDA��ʼ��Ϊ�ߵ�ƽ��1��
    SCL = 1;    //��ʼ���ݴ���ʱ��Ҫ��SCLΪ�ߵ�ƽ��1��
	NOP5();
	SDA = 0;    //SDA���½��ر���Ϊ�ǿ�ʼ�ź�
	NOP5();
	SCL = 0;    //SCLΪ�͵�ƽʱ��SDA�����ݲ�����仯(�������Ժ�����ݴ��ݣ�  
}
/***************************************************
�������ܣ��������ݴ���
***************************************************/
void stop()
// ֹͣλ
{
	SDA = 0;     //SDA��ʼ��Ϊ�͵�ƽ��0��	_n
	SCL = 1;     //�������ݴ���ʱ��Ҫ��SCLΪ�ߵ�ƽ��1��
	NOP5();
	SDA = 1;    //SDA�������ر���Ϊ�ǽ����ź�
	NOP5();	
}
/***************************************************
�������ܣ�����Ӧ��/��Ӧ���ź�
***************************************************/
void Send_Ack(bit ack)
//ack=0��ʾӦ��ack=1��ʾ��Ӧ��
{
	SCL = 0; //����SCLΪ������ݵ�SDA��׼��
	SDA = ack; //���ack��SDA����ʾӦ��/��Ӧ��
	SCL = 1;
	NOP5();   //NOP5()>4us
    SCL = 0;    //SCL��0-1-0Ϊһ��������ʱ������
    SDA=1;//�ͷ�����
}
/***************************************************
�������ܣ���ѯ/����Ӧ���ź�
***************************************************/
unsigned char Rec_Ack(void)    
//����ֵΪ0��ʾӦ��Ϊ1��ʾ��Ӧ��
{unsigned char i,ack;
 SCL=0;  //����SCLΪ������ݵ�SDA��׼��
 SDA = 1; //�ͷ����ߣ��ô��豸��������ݵ�SDA
 SCL=1;
 while((SDA==1)&&(i<250)) i++;
/*�ȴ����豸����Ӧ���źţ���Ϊ0����ʾӦ��ѭ����������Ϊ1�������Ǵӻ���δ���ź���������ѭ����ʱ��i>250������Ϊ1����ʱ����Ϊ��δӦ��*/
 ack=SDA; 
 SCL = 0;//SCL��0-1-0Ϊһ��������ʱ������
 SDA=1; //�ͷ�����
 return(ack);
}
/***************************************************
�������ܣ���AT24Cxx��ȡ����
���ڲ�����recbyte
***************************************************/
unsigned char ReadData()
// ��AT24Cxx�ж�ȡһ���ֽ����ݵ�MCU
{
	unsigned char i;
	unsigned char recbyte;   //�����AT24Cxx�ж���������
	SCL=0;         //����SCLΪ���豸������ݵ�SDA��׼��
    SDA=1; //�ͷ�����
    for(i = 0; i < 8; i++)
	{
		SCL = 1;                //SCL��Ϊ�ߵ�ƽ
		NOP5();
        recbyte=(recbyte<<1)|SDA;       //��SDA
        SCL=0;                       //SCL��0-1-0Ϊһ��������ʱ������
    }
    SDA=1;
	return(recbyte);         //������������
}
/***************************************************
�������ܣ���AT24Cxx�ĵ�ǰ��ַд������
��ڲ�����sendbyte (�����д������ݣ�
***************************************************/
//�ڵ��ô�����д�뺯��ǰ�����ȵ��ÿ�ʼ����start(),����SCL=0
void WriteData(unsigned char sendbyte)
{
	unsigned char i;
	for(i = 0; i < 8; i++)		// ѭ������8��λ
	{
    	sendbyte<<=1; //����ʱ���λ��0�����λ����PSW��CYλ
        SDA=CY;        //������ݵ�SDA
        SCL = 1;            //��SCL�������ؽ�����д��AT24Cxx      
   	    NOP5();                 
	    SCL = 0;            //��SCL������Ϊ�͵�ƽ������SC�����γɴ�����������ģ�������
	 }
	SDA = 1;  // �����豸��������Ӧ��ʱ������ĸߵ�ƽ�ڼ�(SCL=1)�ͷ�SDA�ߣ�
}
/***************************************************
�������ܣ���AT24Cxx�е�ָ����ַд������
��ڲ�����add (����ָ���ĵ�ַ����dat(�����д������ݣ�
***************************************************/
void WriteRom(unsigned char add, unsigned char dat[],unsigned char j)
// ���������j���ֽڵ�����д�뵽��ʼ��ַΪaddr��������������
{  unsigned char flag=0,k=0,i=3;//iΪ���������д������������i�β���ʧЧ��������ֹ������������
   while(i--)
   {start();             //��ʼ���ݴ���
	WriteData(0xa0);  //ѡ��Ҫ������AT24CxxоƬ������֪Ҫ����д�����ݣ�������ַ�Լ�д�����Ϊ1010 0000B��0xa0��
	if(Rec_Ack()) continue;
    WriteData(add);   //д�������ڲ���ַ
	if(Rec_Ack()) continue;
    while(j--)
	{WriteData(dat[k++]);   //��ǰ��ַ������ָ���ĵ�ַ��д������
	 if(!Rec_Ack()) continue;
     flag=1;
	 break;
	}
	if(flag)  continue;/*�ܲ��ܸĳɣ�if(j>=0)  continue? ��j>=0��ζ������δд�꣬��������δӦ��Ҫ��д��*/
	break;
   }
   stop();                //ֹͣ���ݴ���         
   if(i<0) write=1;     
}
/***************************************************
�������ܣ���AT24Cxx�е�ָ����ַ��ȡ����
��ڲ�����set_addr
���ڲ�����x 
***************************************************/
void ReadRom(unsigned char set_addr,unsigned char dat[],unsigned char j)
// ��ָ����ַ��ʼ������ȡj���ֽڣ��������ݴ�ŵ�������
{ unsigned char i=3,k=0;//iΪ��������ض�������������i�β���ʧЧ��������ֹ������������
  while(i--)
  {	start();             //��ʼ���ݴ���
	WriteData(0xa0);     //ѡ��Ҫ������AT24CxxоƬ������֪Ҫ����д�����ݣ�������ַ�Լ�д�����Ϊ1010 0000B��0xa0��
	if(Rec_Ack()) continue;
    WriteData(set_addr);       //д��ָ����ַ
	if(Rec_Ack()) continue;
    start();//���������ߺ���
    WriteData(0xa1);    //ѡ������Ҫ������AT24CxxоƬ������֪Ҫ�����ȡ���ݣ�������ַ�Լ���ȡ����Ϊ1010 0001B��0xa1��
    if(Rec_Ack()) continue;//�������ʧ��,���˳�
    while(--j)
	{ dat[k++]=ReadData();
	  Send_Ack(0);	  
	}//����ǰj-1���ֽڣ���Ӧ��
	dat[k++]=ReadData();//�������һ���ֽ�
	Send_Ack(1);//����豸������Ӧ���źţ��������ݴ���
    break;
  } 
  stop();
  if(i<0) read=1;  
}




void start_screen()
{
	lcd_wcmd(0x01);
	lcd12864_pos(0,0);
 	LCD12864disp("��ӭʹ�ÿ��ڻ���");
	lcd12864_pos(1,0);
	LCD12864disp("1.ʹ��ָ�ƽ���");
	lcd12864_pos(2,0);
	LCD12864disp("2.ʹ���������");
}

//�������ʱ
void wrong(){
	lcd_wcmd(0x01);
	lcd12864_pos(0,0);
 	LCD12864disp("�������");
	lcd12864_pos(1,0);
	LCD12864disp("���������룡");
	delay_ms(1000);
	lcd_wcmd(0x01);
	lcd12864_pos(0,0);
	LCD12864disp("���������룺");
	lcd12864_pos(1,0);
}


//������ȷʱ
unsigned char correct(){
  	unsigned char key;
		lcd_wcmd(0x01);
		lcd12864_pos(0,0);
		LCD12864disp("�����ɹ���");
		lcd12864_pos(1,0);
		LCD12864disp("�Ƿ��޸����룿");
		lcd12864_pos(2,0);
		LCD12864disp("�����ǣ�������");
		lcd12864_pos(3,0);
		while(1){
			key=matrixkeyscan();
			if(key==1 || key==2)
				{
				if(key==1)  LCD12864disp("��");
				else        LCD12864disp("��");
				delay_ms(1000);
				return key;
				}
		}
}

//������
void change(){
	unsigned char key,a=0;
	unsigned char s1[10]={0};
	unsigned char s2[10]={0};
	lcd_wcmd(0x01);
	lcd12864_pos(0,0);
	LCD12864disp("���������룺");
	lcd12864_pos(1,0);
	while(1){
		key = matrixkeyscan();
		if(key<12)
		{
			if(key==11)	 LCD12864_backspace();
			if(key==10)  break;
			if(key!=10 && key!=11){s1[a++]=str[key]; LCD12864disp("��");}
		}
	}
	lcd12864_pos(2,0);
	LCD12864disp("�ٴ��������룺");
	lcd12864_pos(3,0);
	a=0;
	while(1){
	key=matrixkeyscan();	
	if(key<14)
	{
		if(key==11)	 LCD12864_backspace();
		if(key==10)  break;
		if(key!=10 && key!=11){s2[a++]=str[key]; LCD12864disp("��");}
		}
	}
	if(strcmp(s1,s2)==0)
		{
		SDA = 1;           						//I2C��ʼ����SDA=1,SCL=1,ʹ�����豸���ڿ���״̬
   	SCL = 1; 
		memset(Rbuff,0,sizeof(s));   	//�ٴγ�ʼ������
		strcpy(Rbuff,s2); 
		WriteRom(0x32,s2,10);   			//����ʼ��ַΪ��0x32��������������д������Wbuff�е�5������
   	ReadRom(0x32,Rbuff,10);    		//����ʼ��ַΪ��0x32�������������ж�ȡ5�����ݵ�����Rbuff��
	   	
		lcd_wcmd(0x01);
		lcd12864_pos(0,0);
		LCD12864disp("�����޸ĳɹ�");
		delay_ms(2000);

	   	lcd_wcmd(0x01);
		lcd12864_pos(0,0);
		LCD12864disp("���������룺");
		lcd12864_pos(1,0);
	}
	else
	{
		lcd_wcmd(0x01);
		lcd12864_pos(0,0);
		LCD12864disp("�������벻һ��");
		lcd12864_pos(1,0);
		LCD12864disp("�����޸�ʧ��");
		delay_ms(2000);

		lcd_wcmd(0x01);
		lcd12864_pos(0,0);
		LCD12864disp("���������룺");
		lcd12864_pos(1,0);	
	}
}

void main()
{  
   int a=0,flage=1;
   unsigned char i=0;
   unsigned char key,f;
   SDA = 1;           				//I2C��ʼ����SDA=1,SCL=1,ʹ�����豸���ڿ���״̬
   SCL = 1;  
   //led = 1;	       
   WriteRom(0x32,Wbuff,10);   //����ʼ��ַΪ��0x32��������������д������Wbuff�е�5������
   ReadRom(0x32,Rbuff,10);    //����ʼ��ַΪ��0x32�������������ж�ȡ5�����ݵ�����Rbuff��
   	
    //��ʼ��
	
	lcd12864_init();
  start_screen();
	if(key<14)
	{
		if(key==2)
		{
			lcd_wcmd(0x01);
			lcd12864_pos(0,0);
			LCD12864disp("���������룺");
			lcd12864_pos(1,0);
			while(1)
				{
					key=matrixkeyscan();
					if(key<14)
				{ 
					if(key==11) {LCD12864_backspace();}  		//ɾ��
					if(key==10) 																	//����ȷ��
				{
			   	if(strcmp(Rbuff,s)==0) flage=0;  					//��ȷ
				  else flage=1;					   										//����
				  if(flage==0)
				{
				   	led=0;   																//����
				  	f=correct();
					  if(f==0)
						{					      
							change();
					  }
					  if(f==1){
					  	lcd_wcmd(0x01);
					  	lcd12864_pos(0,0);
							LCD12864disp("���������룺");
							lcd12864_pos(1,0);	
					  }
					  led = 1;   								//����
					  a=0;
					  memset(s,0,sizeof(s));   	//�ٴγ�ʼ������
				  }
				  else
				  {
				  	wrong();
					  a=0;
					  memset(s,0,sizeof(s));   	//�ٴγ�ʼ������
				  }
			   } 
			  if(key!=10 && key!=11) {s[a++]=str[key]; LCD12864disp("��");}
			  }	  
			}
		}
		if(key==1)
		{			
			Uart_Init();
			Led_Init();
			Buzz_Init();
			Device_Check();
			Delay_Ms(500); //��ʱ500MS���ȴ�ָ��ģ�鸴λ
			Buzz_Times(1);
			
			lcd_wcmd(0x01);
			lcd12864_pos(0,0);
			LCD12864disp("ָ�ƽ���ϵͳ");
			lcd12864_pos(1,0);
			LCD12864disp("  1.���ָ��");
			lcd12864_pos(2,0);
			LCD12864disp("  2.����ָ��");
			lcd12864_pos(3,0);
			LCD12864disp("  3.���ָ�ƿ�");

	while(1)
	{
		if(key<14)
			{	 						
			 switch(key)
				{
					case 1:	 //���ָ��
					Led_Green_On();
					FPM10A_Add_Fingerprint();
					Led_Green_Off();
					start_screen();	
					break; 
												
					case 2://����ָ��
					Led_Yellow_On();						
					FPM10A_Find_Fingerprint();
					Led_Yellow_Off();							
					start_screen();																	
					break;									
					
					case 3:	//���ָ��
					FPM10A_Delete_All_Fingerprint();								
					start_screen();
		  		break;
				}
			}	
	}
}
		}
		
		
		
		
  }		 


