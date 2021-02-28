#include<reg51.h>
#define uchar unsigned char
uchar count;
sbit pwm=P0^0;		//PWM信号输出口
uchar jia=1;			//角度增加按键检测I/O口
uchar jan=1;			//角度减少按键检测I/O口
uchar jd; 				//角度标识
uchar a;
uchar i;
uchar flag;
void delay(uchar z)
{
	uchar x,y;
	for(x=125;x>0;x--)
		for(y=z;y>0;y--);
}
void Com_Init()
{
	TMOD=0x21;		 //T1工作于方式1
	TH1=0xfd; 			//设置波特率为9600
	TL1=0xfd;
	TR1=1;	  		//启动T1
	REN=1;				//允许串行口接收
	SM0=0; 				//串口工作方式1
	SM1=1;
	EA=1;  				//开放所有中断
	ES=1;					//允许串行口中断
}
void Time0_Init()	 //定时器初始化
{
	TMOD=0x01;		//定时器0工作在方式1
	IE=0x82;
	TH0=0xff;
	TL0=0xa3;	  //11.0592MHz晶振，0.1ms
	TR0=1;	   //定时器开始
}
void Time0_Int() interrupt 1
{
	TH0=0xff;
	TL0=0xa3;
	if(count<jd)	   //判断0.5ms次数是否小于角度标识
		pwm=1;		  		//确实小于，pwm输出高电平
	else
		pwm=0;		 			//大于则输出低电平
	count=(count+1);  //0.5ms次数加1
	count=count%40;		 //次数始终保持为40即保持周期为20ms
}

void main()
{
	jd=12;
	count=0;
	Time0_Init();
	Com_Init();
	while(1)
	{

		if(flag==4)
		{
			do
			{
				if(flag != 4)
				break;
				
				jd=5;
				count=0;
				delay(500);
				
				 	for(i=1;i<=14;i++)
					{
						if(flag != 4)
						 	break;
					
						jd++;
						count=0;
						if(jd==20)
							jd=19;	//已经是180度，则保持
						delay(500);
						if(i==14)
						{
							
							for(i=1;i<=14;i++)
							{
								if(flag != 4)
							 	break;
								jd--;
								count=0;
								if(jd==4)
									jd=5;	  //已经是0度，则保持
								delay(500);
							}
						}
					}
			}while(flag!=4);
		
		}
	}

}
void ser() interrupt 4
{
	if(RI==0) return;
	ES=0;	  //关闭口中断
	RI=0;	//清除口接收标志位
	a=SBUF;	//读取字符
	flag=a;
	ES=1;	//允许串口中断
	P2 = ~P2;
		if(flag==1)
		{
			jd++;
					count=0;	//按键按下 则20ms周期从新开始
				if(jd==20)
					jd=19;	//已经是180度，则保持
		
		}
		if(flag==2)
		{
			jd--;
			count=0;
			if(jd==4)
				jd=5;	  //已经是0度，则保持
		}
		if(flag==3)
		{
			
			jd=12;
			count=0;
		} 	
	
}
