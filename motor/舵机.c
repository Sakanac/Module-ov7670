#include<reg51.h>
#define uchar unsigned char
uchar count;
sbit pwm=P0^0;		//PWM�ź������
uchar jia=1;			//�Ƕ����Ӱ������I/O��
uchar jan=1;			//�Ƕȼ��ٰ������I/O��
uchar jd; 				//�Ƕȱ�ʶ
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
	TMOD=0x21;		 //T1�����ڷ�ʽ1
	TH1=0xfd; 			//���ò�����Ϊ9600
	TL1=0xfd;
	TR1=1;	  		//����T1
	REN=1;				//�����пڽ���
	SM0=0; 				//���ڹ�����ʽ1
	SM1=1;
	EA=1;  				//���������ж�
	ES=1;					//�����п��ж�
}
void Time0_Init()	 //��ʱ����ʼ��
{
	TMOD=0x01;		//��ʱ��0�����ڷ�ʽ1
	IE=0x82;
	TH0=0xff;
	TL0=0xa3;	  //11.0592MHz����0.1ms
	TR0=1;	   //��ʱ����ʼ
}
void Time0_Int() interrupt 1
{
	TH0=0xff;
	TL0=0xa3;
	if(count<jd)	   //�ж�0.5ms�����Ƿ�С�ڽǶȱ�ʶ
		pwm=1;		  		//ȷʵС�ڣ�pwm����ߵ�ƽ
	else
		pwm=0;		 			//����������͵�ƽ
	count=(count+1);  //0.5ms������1
	count=count%40;		 //����ʼ�ձ���Ϊ40����������Ϊ20ms
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
							jd=19;	//�Ѿ���180�ȣ��򱣳�
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
									jd=5;	  //�Ѿ���0�ȣ��򱣳�
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
	ES=0;	  //�رտ��ж�
	RI=0;	//����ڽ��ձ�־λ
	a=SBUF;	//��ȡ�ַ�
	flag=a;
	ES=1;	//�������ж�
	P2 = ~P2;
		if(flag==1)
		{
			jd++;
					count=0;	//�������� ��20ms���ڴ��¿�ʼ
				if(jd==20)
					jd=19;	//�Ѿ���180�ȣ��򱣳�
		
		}
		if(flag==2)
		{
			jd--;
			count=0;
			if(jd==4)
				jd=5;	  //�Ѿ���0�ȣ��򱣳�
		}
		if(flag==3)
		{
			
			jd=12;
			count=0;
		} 	
	
}
