#include<STC12C5A60S2.h>
#include "ov7670_fifo.h"
code char stringSend0[]="AT+GMR\r\nAT+CWMODE_DEF=1\r\nAT+CWSAP=\"ChinaNet-CYH\",\"22335359\"\r\nAT+CWAUTOCONN=1\r\nAT+CIPSTART=\"TCP\",\"aip.baidubce.com\",80\r\nAT+CIPMODE=1\r\nAT+CIPSEND\r\n";
code char stringSend1[]="/rest/2.0/face/v3/search?access_token=24.81718d1f38ca801b7357ee715a99572a.2592000.1616509674.282335-23649586 HTTP/1.1\r\nHost:aip.baidubce.com\r\nConnection: close\r\nContent-Length: 998\r\n";
code char stringSend2[]="\r\n\r\n";
unsigned char bn=0;
void ConfigUART(unsigned char baud);
void main(void)
{
	unsigned int  a,b;
	unsigned int  c_data;
	
	unsigned char tmp;
	
	EX0=1;	  //红外中断
	IT0=1;    //下降沿有效
	EA=1;
	
	FIFO_OE = 0;
	FIFO_WE = 0;
	do
	{
		tmp = Sensor_init();
	}
	while(!tmp);         
	EA=1;	  //总中断 	
	while(1)
	{
		while(bn!=2)
		{
		
		}
		
		FIFO_RRST = 0;        //复位FIFO读指针
		FIFO_RD = 0;
		FIFO_RD = 1;
		FIFO_RD = 0;
		FIFO_RRST = 1;
		FIFO_RD = 1;
		
		EA=0;
		for(a=0;a<320;a++)
		{
			for(b=0;b<240;b++)
			{
				FIFO_RD = 0; 
				c_data = CAMERA2MCU_Data;//读高位
				//SBUF=CAMERA2MCU_Data;
				c_data <<= 8; 
				FIFO_RD = 1; 
				FIFO_RD = 0;
				//SBUF=CAMERA2MCU_Data;
				c_data |= CAMERA2MCU_Data;//读低位
				FIFO_RD = 1;
				TMOD = 0x00;
				TL0 = 0x66;                                
				TH0 = 0xfc;
				TR0 = 1;                                   
				ET0 = 1;                                    
				EA = 1;
				ConfigUART(9600);
				while(!RI);
				RI=0;
				while(!TI);
			  SBUF = stringSend0;
				SBUF = stringSend1;
				SBUF = c_data;
				SBUF = stringSend2;
				TI = 0;
			}
		}
		EA=1;
		bn=0;
	}	
}
	
void int0() interrupt 0
{  
	if(bn==0)
	{
		FIFO_WRST = 0;    //复位FIFO写指针
		FIFO_RD = 0;
		FIFO_RD = 1;
		FIFO_RD = 0;
		FIFO_WRST = 1;
		FIFO_RD = 1;
		bn=1;
		FIFO_WE = 1;
	}
	else if(bn==1)
	{
		FIFO_WE = 0;
		bn=2;
	}
}

void ConfigUART(unsigned char baud)
{
	SCON = 0x50;
	TMOD &= 0x0f;
	TMOD |= 0x20;
	TH1 = 256 - (24000000/12/32)/baud;
	TL1 = TH1;
	ET1 = 0;
	TR1 = 1;
}