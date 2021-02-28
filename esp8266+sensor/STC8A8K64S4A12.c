#include<STC8.h>
void ConfigUART(unsigned char baud);
code char string0[]="AT+GMR\r\nAT+CWMODE_DEF=1\r\nAT+CWSAP=\"ChinaNet-CYH\",\"22335359\"\r\nAT+CWAUTOCONN=1\r\nAT+CIPSTART=\"TCP\",\"aip.baidubce.com\",80\r\nAT+CIPMODE=1\r\nAT+CIPSEND\r\nPOST /oauth/2.0/token?grant_type=client_credentials&client_id=fcFMaGozqkIPMilGTLgbwepI&client_secret=nG4Tx8OsDwnoyiydKS7YiRb8m9W96tTq HTTP/1.1\r\nHost:aip.baidubce.com\r\nConnection:close\r\n\r\n";
code char string1[]="AT+GMR\r\nAT+CWMODE_DEF=1\r\nAT+CWSAP=\"ChinaNet-CYH\",\"22335359\"\r\nAT+CWAUTOCONN=1\r\nAT+CIPSTART=\"TCP\",\"aip.baidubce.com\",80\r\nAT+CIPMODE=1\r\nAT+CIPSEND\r\nPOST /rest/2.0/face/v3/search?access_token=25.2214ff43d5229081ff560cd0c139fdc9.315360000.1929884113.282335-23649586 HTTP/1.1\r\nHost:aip.baidubce.com\r\nConnection:close\r\n\r\n";
char returnData;

void main()
{ 
	TMOD = 0x00;
  TL0 = 0x66;                                
  TH0 = 0xfc;
  TR0 = 1;                                   
  ET0 = 1;                                    
  EA = 1;
	P00=1;
	P01=1;
	P02=1;
	ConfigUART(9600);
	

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
void CheckSensor() interrupt 1
{
	if (P03||P04)
	{
		while(1)
	{
		while(!RI);
		returnData = SBUF;
		if(returnData>=90)
		{
		P05=1;
		}
		RI=0;
		while(!TI);
		SBUF = string1;
		TI = 0;
	}
	}
}