#ifndef _OV7670_FIFO_H
#define _OV7670_FIFO_H	

/**********************camera data**************************/
#define camera_DataIn       //���ݿڷ�������Ϊ����
#define CAMERA2MCU_Data  P2 

#define SCCB_SID_STATE	 SCCB_SID
#define SCCB_SID_IN          
#define SCCB_SID_OUT 
sbit 	SCCB_SIC = P3^7;
sbit	SCCB_SID = P3^6;
/**************************FIFO***************************************/
sbit 	FIFO_OE   = P1^2;//ʹ�ܹضϿ���
sbit	FIFO_RRST = P1^4;//��ָ�븴λ
sbit 	FIFO_RD   = P1^0;//ʱ�ӿ���
sbit	FIFO_WE   = P1^1;//����д��
sbit	FIFO_WRST = P1^3;//дָ�븴λ


void startSCCB(void);

void stopSCCB(void);

void noAck(void);

unsigned char SCCBwriteByte(unsigned char m_data);

unsigned char SCCBreadByte(void);

unsigned char wr_Sensor_Reg(unsigned char regID, unsigned char regDat);

unsigned char rd_Sensor_Reg(unsigned char regID, unsigned char *regDat);

unsigned char Sensor_init(void);

#endif