/*
####################################
####### __Hien_Thi_SP__#############
######__Writen By: Ng Thai Duy######

*/
// config thach anh noi, 
//FOSC11.0692 Mhz

#include <N78E055A_059A_517A.h>
#include <intrins.h>

/*khai bao dia chi led*/
#define ADD1	P44
#define ADD2	P10
#define ADD3	P11
#define ADD4	P12
#define ADD5	P13
#define ADD6	P14
#define ADD7	P16
#define ADD8	P24
/*Watch Dog*/
#define Xoa_co WDCLR

/* Chan_Chot	HC595	*/
#define LATCH P06
/* 						Coi		*/
#define BEEP  P41	

/*  led    */
#define Led1	P25
#define Led2	P23
#define Led3	P22

/*nut nhan*/
#define BT1		P26
#define BT2		P47
#define BT3		P46
#define BT4		P43

/*I2C*/
#define SCK		P01
#define SDA		P00

/* ma tran phim*/
#define row1	P21
#define row2	P32
#define row3	P33
#define row4	P34
#define cot1	P35
#define cot2	P36
#define cot3	P37
#define cot4	P20

/*Dia chi DS1307*/
#define DS1307_SEC      	0x00 //Giay
#define DS1307_MIN      	0x01 //Phut
#define DS1307_HOUR     	0x02 //Gio
#define DS1307_DAY       	0x03 //Thu
#define DS1307_DATE      	0x04 //Ngay
#define DS1307_MONTH    	0x05 //Thang
#define DS1307_YEAR         0x06 //Nam

//50 BCD=0101 0000,DEC=0011 0010 //BCD>>4*10+BCD&0x0f

unsigned char code maled[13]={0x3F,0x06,0x5B,0x4F,0x66,0x6D,0x7D,0x07,0x7F,0x6F,0x00,0x77,0x39};
/*//tu 0 den D,sao den thang*/
unsigned char code maphim[16]={0xEB,0x77,0x7B,0x7D,0xB7,0xBB,0xBD,0xD7,0xDB,0xDD,0x7E,0xBE,0xDE,0xEE,0xE7,0xED};
unsigned char xdata data_led[32];
unsigned char xdata received_data[2],Buff_data[4];
volatile unsigned char date=1,month=1,year=19,hour=12,min=59,sec=50,line=7,ca=1,tong_line=15;
volatile unsigned char stb_scan=0,vrucUart_read=0;
volatile unsigned char vrucHour1_start=8 ,vrucMin1_start=00,vrucHour1_stop=10,vrucMin1_stop =00;
volatile unsigned char vrucHour2_start=10,vrucMin2_start=15,vrucHour2_stop=12,vrucMin2_stop=15;
volatile unsigned char vrucHour3_start=13,vrucMin3_start=00,vrucHour3_stop=15,vrucMin3_stop=00;
volatile unsigned char vrucHour4_start=15,vrucMin4_start=15,vrucHour4_stop=17,vrucMin4_stop=15;
volatile unsigned char vrucHour5_start=17,vrucMin5_start=30,vrucHour5_stop=19,vrucMin5_stop=30;

volatile unsigned short int	tong_sp=2000,sp_dat=6,sp_loi=5,count=0;
unsigned char  scan_phim=0xEE,check,blink=1,MODE,old_sec,stb1,stb2,stb3,stb4,vaoca=0,hetca=0;

/*Khai bao nguyen ham*/
void day_data_led(unsigned char address);
void Spi_write(unsigned char byte);
void delay_us(unsigned int time);
void delay_ms(unsigned int time);
void tat_led (void);
void quet_led(unsigned char address);
void day_data_led(unsigned char address);
void quet_phim(unsigned char scan);
void xoa_data_led(void);
/*----------- Ham_Con ------------*/
void Init_all()
{
	/*-----Clock-------*/ //ISP  Disable 22.1186MHz thach anh noi
	//CHPCON  |=0x01;
	/* -------SPI------- */
	SPCR =0x51;
	SPSR|=0x08;
	LATCH=0;	
/*-------- Timer ------------ */
	TMOD|=0x11;
	TL0=0x00;	//20000us// 50hz quet 8 led ->2500
	TH0=0xF7;
	TR0=1;
	
/*------------- I2C ---------*/
	//SDA va SCK muc cao
		SDA=1;
		SCK=1;	

/*------Timer 2--------------------Baudrate for UART*/
	C_T2=0;// dem bang clock noi
	RCLK =1;// use for baud rate
	TCLK=1;//use for baud rate
	CP_RL2=0;// auto reload
	TR2=1;	//enable timer 2
	RCAP2H=0xFF;			//9600//FOSC 22 Mhz
	RCAP2L=0xB8;			//RI, TI=1 la truyen xong luc dau muc 0

/*---Uart-------*/	//Mode 1// start bit =0 stop=1
	SM0=0;	
	SM1=1; 						//UART mode 1 SM0=0/SM1=1 Timer2 overflow/16
	SM2=1;
	REN=1;
	TI=0;
	RI=0;
		
/*Interrupt_Init*/
	ET0=1;	// Enable interupt Timer 0
	ES=1;	//enable interupt Uart
	EA=1;		//enable interrupt

	
}
/*------Truyen Data------*/
void Spi_write(unsigned char byte)
{
	SPDR=byte;
	while(!(SPSR &0x80));// cho truyen xong
	SPSR&=~0x80;//xoa co
}
///*-----Truyen Uart------*/
//void Uart_send(unsigned char byte)
//{
//	SBUF=byte;
//	while(!TI);
//	TI=0;
//}

/*---------Uart_read--------*/
unsigned char Uart_read(void)
{
	unsigned char vrucTemp;
	if(received_data[0] != 0xFF && received_data[1] != 0xFF)
	{
	vrucTemp = received_data[0];
	vrucTemp = vrucTemp*10 + received_data[1];
	received_data[0] = 0xFF;
	received_data[1] = 0xFF;
	return vrucTemp;
	}
	else return 0xFF;
	
}

/*------- Delay ms -------*/
void delay_ms(unsigned int time)
{
unsigned int x,y;
for(x=0;x<time;x++)
	{
		for(y=0;y<123;y++);
	}			 
}
void fnDecomCot(unsigned char _vrucData)
{
	cot1 = _vrucData&0x01;
	cot2 = _vrucData&0x02;
	cot3 = _vrucData&0x04;
	cot4 = _vrucData&0x08;
}
void fnDecomRow(unsigned char _vrucData)	// gan gia tri bien vao tung bit
{
	row1 = _vrucData&0x01;
	row2 = _vrucData&0x02;
	row3 = _vrucData&0x04;
	row4 = _vrucData&0x08;
}
unsigned char fnucKeyScan(unsigned char _vrucScan)
{	
	
	unsigned char temp;
	fnDecomRow(0xff);	// cho hang bang 1
	fnDecomCot(_vrucScan);	//cot dich bit0
	if(row1==0||row2==0||row3==0||row4==0)
	{
		
		if(row1==0||row2==0||row3==0||row4==0)
		{
		
		temp=(((char)cot1<<4)|((char)cot2<<5)|((char)cot3<<6)|((char)cot4<<7)		
			|((char)row1<<0)|((char)row2<<1)|((char)row3<<2)|((char)row4<<3));
		}
	}
	
	if		(temp == maphim[0]) return 0 ;	
	else if(temp == maphim[1]) return 1	;
	else if(temp == maphim[2]) return 2	;
	else if(temp == maphim[3]) return 3	;
	else if(temp == maphim[4]) return 4	;
	else if(temp == maphim[5]) return 5	;
	else if(temp == maphim[6]) return 6	;
	else if(temp == maphim[7]) return 7	;
	else if(temp == maphim[8]) return 8	;
	else if(temp == maphim[9]) return 9	;
	else if(temp == maphim[10]) return 10 ;
	else if(temp == maphim[11]) return 11 ;
	else if(temp == maphim[12]) return 12 ;
	else if(temp == maphim[13]) return 13 ;
	else if(temp == maphim[14]) return 14 ;
	else if(temp == maphim[15]) return 15 ;
	else return 16;
}

unsigned char BT_On()
{	
	unsigned char temp;
	fnDecomRow(0xff);	// cho hang bang 1
	fnDecomCot(scan_phim);	//cot dich bit0
	if(row1==0||row2==0||row3==0||row4==0)
	{
		if(row1==0||row2==0||row3==0||row4==0) temp = 1;
	
	}
	else temp = 0;
	return temp;
}


/*--------Quet Led ---------*/
void quet_led(unsigned char address)		 
{
	ADD1=0;
	ADD2=0;
	ADD3=0;
	ADD4=0;
	ADD5=0;
	ADD6=0;
	ADD7=0;
	ADD8=0;
	day_data_led(address);
	ADD1=address&0x01;
	ADD2=address&0x02;
	ADD3=address&0x04;	
	ADD4=address&0x08;
	ADD5=address&0x10;
	ADD6=address&0x20;
	ADD7=address&0x40;
	ADD8=address&0x80;
		
}

/*------Day Du Lieu Vao Led---------*/
void day_data_led(unsigned char address)
{	
	if	(address==0x01)address = 0	;
	else if(address==0x02)address = 1	;
	else if(address==0x04)address = 2	;
	else if(address==0x08)address = 3	;
	else if(address==0x10)address = 4	;
	else if(address==0x20)address = 5	;
	else if(address==0x40)address = 6	;
	else if(address==0x80)address = 7	;
	Spi_write(maled[data_led[24+address]]);
	Spi_write(maled[data_led[16+address]]);
	Spi_write(maled[data_led[8+address]]);
	Spi_write(maled[data_led[0+address]]);
	LATCH=0;
	LATCH=1;
}
/*-----Xoa Du Lieu Led-------------*/
void xoa_data_led(void)
	{
		Spi_write(0x00);
		Spi_write(0x00);
		Spi_write(0x00);
		Spi_write(0x00);
		LATCH=0;
		LATCH=1;	
	}

/*--------------------------*/
void tat_led (void)
{
	unsigned char i=0;
	for(i=0;i<32;i++)
	data_led[i]=10;
}
/*------Kiem Tra Phim --------*/
void check_phim()
{		
	static unsigned char dem_stb2,dem_stb3,dem_stb4;
	static unsigned int dem_stb1;
		if(BT1==0)
	{
		dem_stb1++;
		if(dem_stb1==40)stb1=1;
		if(dem_stb1==500)
		{
		stb1=2;
		dem_stb1=250;
		}
	}
	else dem_stb1=0;
	
	
	if(BT2==0)
	{
		dem_stb2++;
		if(dem_stb2==40)stb2=1;
		if(dem_stb2==250)
			{
			stb2=0;
			dem_stb2=140;	
			}
		
	}
	else dem_stb2=0;
	
		if(BT3==0)
	{
		dem_stb3++;
		if(dem_stb3==40)stb3=1;
		if(dem_stb3==250)
		{	stb3=0;
			dem_stb3=140;	
		}
	}
	else dem_stb3=0;
	
		if(BT4==0)
	{
		dem_stb4++;
		if(dem_stb4==40)stb4=1;
		if(dem_stb4==250)
		{
			stb4=0;
			dem_stb4=140;
		}
	}
	else dem_stb4=0;
}

/* -----I2C Start------*/
void I2C_Start()	//SDA keo xuong muc thap khi SCK muc 1 
{	
	SCK=1;
	SDA=1;
	SDA=0;
	SCK=0;
}

/*--------I2C Stop--------*/
void I2C_Stop()	//SDA len muc cao khi SCK van muc 1
{	
	SCK=1;
	SDA=0;
	SDA=1;
}

/*----------I2C Write---------*/
unsigned char I2C_Write(unsigned char byte)	// sau khi truyen 8 xung, gui them 1 xung xac nhan ACK
{
	unsigned char i=8;
	bit result;
	while(i--)
		{
				SDA=byte&(0x80);
				SCK=1;
				SCK=0;
				byte<<=1;
		}
		SCK=1;
		result=SDA;
		SCK=0;
		return result;
}

/*----------I2C Read---------*/
unsigned char I2C_Read()// nhan 8 bit gui them bit ACK hoac NAK SDA xuong thap tai SCK thu 9- neu ACK
{	
	unsigned char data_in=0,i=8;
	while(i--)
	{
		data_in<<=1;
		SCK=1;
		data_in|=SDA;
		SCK=0;
	}	
	SCK=1;
	SCK=0;
	return data_in;
}

/*------------------DS1037----------------*/

/* ----------Ghi vào DS1307 ------*/
void DS1307_Write(unsigned char address,unsigned char data_in)
{	
	data_in = (data_in/10)<<4|(data_in%10);// chuyen sang ma BCD
	I2C_Start();
	I2C_Write(0xD0);//dia chi chip
	I2C_Write(address);//dia chi can ghi
	I2C_Write(data_in);// du lieu can ghi
	I2C_Stop();
}

/*----------Doc gia tri tu DS1307---------*/
unsigned char DS1307_Read(unsigned char address)
{
	unsigned char result;
	I2C_Start();
	I2C_Write(0xD0);//dia chi chip
	I2C_Write(address);// dia chi can ghi
	I2C_Start();
	I2C_Write(0xD1);//gui lenh doc
	result=I2C_Read();
	I2C_Stop();
	return result=(result>>4)*10+(result&0x0F);//BCD sang DEC
}

/*--------Setup du lieu vao DS1307----------*/
void DS1307_Set()
{
          //Ghi du lieu ngay gio vao DS1307
          DS1307_Write(DS1307_SEC,sec);
          DS1307_Write(DS1307_MIN,min);
          DS1307_Write(DS1307_HOUR,hour);
          DS1307_Write(DS1307_DATE,date);
          DS1307_Write(DS1307_MONTH,month);
          DS1307_Write(DS1307_YEAR,year);
}

/*Lay gia tri gio phut day*/
void DS1307_GetTime()
{
          //Doc du lieu gio tu DS1307
          hour=DS1307_Read(DS1307_HOUR);
          min=DS1307_Read(DS1307_MIN);
          sec=DS1307_Read(DS1307_SEC);
}

/*Lay gia tri ngay thang nam*/
void DS1307_GetDate()
{
          //Doc du lieu ngay tu DS1307
          date=DS1307_Read(DS1307_DATE);
          month=DS1307_Read(DS1307_MONTH);
          year=DS1307_Read(DS1307_YEAR);
}

//End DS1307

/*Conver DEC to BCD */
/*
void DEC2BCD(unsigned char data_in )
{
	return(data_in/10)<<4 | (data_in%10);
}
*/
	
/*Hien Thi Gio Phut Giay*/
void hien_thi_time()
{
	if(old_sec!=sec)
	{	  
		old_sec=sec;
		/* gio */
		data_led[14]= hour %10;
		data_led[15]= hour /10;
		/* phut*/
		data_led[12]= min %10;
		data_led[13]= min /10;
		/* giay */
		data_led[22]= sec %10;
		data_led[23]= sec /10;
	}  
}
void hien_thi_date()
{
		/* ngay thang nam */
	data_led[0] = year %10;
	data_led[1] = year /10;
	data_led[2] = 0;
	data_led[3] = 2;
	data_led[4] = month %10;
    data_led[5] = month /10;
	data_led[6] = date %10;
	data_led[7] = date /10;
}
void hien_thi_sp()
{
		/* tong sp*/
		data_led[8]	= tong_sp %10				;
		data_led[9]	= (tong_sp	/10)%10			;
		data_led[10]= (tong_sp /100)%10			;
		data_led[11]= tong_sp /1000				;		
	
		/*so line*/
		data_led[20]=tong_line %10	;
		data_led[21]=tong_line /10  ;
		
		/*Sp dat*/
		data_led[24]=	sp_dat			%10 ;
		data_led[25]= (sp_dat/10)		%10	;
		data_led[26]= (sp_dat/100)		%10	;
		data_led[27]=  sp_dat/1000			;		
	
		/*line*/
		data_led[28]=line %10				;
		data_led[29]=line /10 				;
	
		/*ca*/
		data_led[30]=ca %10					;
		data_led[31]=ca /10 				;		
		
		/* Sp loi */
		data_led[16]= 	sp_loi			%10 ;
		data_led[17]=  (sp_loi/10)		%10	;
		data_led[18]= (sp_loi/100)		%10	;
		data_led[19]=  sp_loi/1000			;	
}	

void fnWarning_Start()
{
	BEEP=0;
	vaoca=1;
}
void fnWarning_End()
{
	BEEP=0;
	hetca=1;
}
void Init_WD(void)
{
		EA=0;	//disable interrupt
		TA = 0xAA;
		TA = 0x55;
		WDCON |= 0xC2; //0xc3 6.5s reset// muon tang len thi doi gia tri thanh 0xc4 hoac 0xc5
		EA=1;
}
																																																																																																																																																																																																																																																																																																																																																																																																																				
void FnReset_WD(void)
{
		EA=0;	
		TA = 0xAA;
		TA = 0x55;
		WDCON |= 0x40;	
		EA=1;
}


/*-------- Giao Dien Hien Thi ---------*/
void hien_thi(void)
{	
	stb1=0;
	while(1)
	{	
		FnReset_WD();
		DS1307_GetDate();
		DS1307_GetTime();		
		stb_scan=fnucKeyScan(scan_phim);
		while(BT_On());
		delay_ms(10);
		vrucUart_read= Uart_read();
			if(stb1==2 || stb_scan==14 || vrucUart_read==139)	// chinh sua thoi gian
			{	
				MODE=1;	
				stb1=0;
				stb_scan=16;
				vrucUart_read=0xFF;
				break;
				
			}
			if(stb1==1)// luu vao data_
			{
				 stb1=0;
				
			}

			if(stb2==1)	 // tang gia tri sp_dat
			{
				stb2=0;
				if(sp_loi+sp_dat<tong_sp)	sp_dat++;
			
			}
			if(stb3==1)	 // tang gia tri sp_loi
			{
				stb3=0;
				if(sp_loi+sp_dat<tong_sp)sp_loi++;

			}
			if(stb4==1)	// reset sp ve 0
			{
				stb4=0;
				sp_loi=sp_dat=0;

			}
			/*thoi gian hien thi ca*/
			if		( hour>=vrucHour1_start && hour<vrucHour1_stop ) ca=1;
			else if( hour>=vrucHour2_start && hour<vrucHour2_stop  ) ca=2;
			else if( hour>=vrucHour3_start && hour<vrucHour3_stop  ) ca=3;
			else if( hour>=vrucHour4_start && hour<vrucHour4_stop  ) ca=4;
			else if( hour>=vrucHour5_start && hour<vrucHour5_stop  ) ca=5;
			else ca=0;
			
			
			/*bat dau ca*/
			if( hour == vrucHour1_start && min == vrucMin1_start && sec==0 )
			{
				fnWarning_Start();
			}
			else if( hour == vrucHour2_start && min == vrucMin2_start && sec==0 )
			{

				fnWarning_Start();
			}
			else if( hour == vrucHour3_start && min == vrucMin3_start && sec==0 )
			{

				 fnWarning_Start();
			}
			else if( hour == vrucHour4_start && min == vrucMin4_start && sec==0)
			{
				
		
				fnWarning_Start();
			}
			else if( hour == vrucHour5_start && min == vrucMin5_start && ca==0 )
			{
		
				fnWarning_Start();
			}
			/*ket thuc ca*/
			else if( hour == vrucHour1_stop && min == vrucMin1_stop && sec==0 )
			{
				
				fnWarning_End();
			}
			else if( hour == vrucHour2_stop && min == vrucMin2_stop && sec==0 )
			{
				
				fnWarning_End();
			}
			else if( hour == vrucHour3_stop && min == vrucMin3_stop && sec==0 )
			{	
				 fnWarning_End();	
			}
			else if( hour == vrucHour4_stop && min == vrucMin4_stop && sec==0 )
			{
				
				fnWarning_End();
			}
			else if( hour == vrucHour5_stop && min == vrucMin5_stop && sec==0 )
			{
				
				fnWarning_End();
			}
		
			hien_thi_date();
			hien_thi_time();
			hien_thi_sp();
			

	}

}
/*-------- Giao Dien Chinh Sua ---------*/
void chinh_sua(void)
{	

	unsigned char ds_menu=1,i=0,j=0;
	stb1=0;
	while(1)
		{	
			FnReset_WD();
			vrucUart_read=0xFF;
			check =16;
			stb_scan=fnucKeyScan(scan_phim);
			while(BT_On());	
			delay_ms(10);
			vrucUart_read = Uart_read();
			delay_ms(10);
				
			if(stb1==2||stb_scan==14||vrucUart_read == 139)	
			{	
				MODE=2;	
				stb1=0;
				stb_scan=16;
				vrucUart_read=0xFF;
				DS1307_Set();
				break;
				
			}
			if(ds_menu>10) ds_menu=0;
			if(stb1==1||stb_scan==15||vrucUart_read == 140)
			{
				ds_menu++;
				stb1=0;
				vrucUart_read=0xFF;
				stb_scan=16;
			}
			
			switch(ds_menu)
			{
				case 1:	//nam
				{	
					data_led[30]= ca %10;
					data_led[31]= ca /10;
					if(blink==1)
					{
					data_led[0]= year %10;
					data_led[1]= year /10;
					data_led[2]= 0;
					data_led[3]= 2;
					}
					else
					{
					data_led[0]= 10;
					data_led[1]= 10;
					data_led[2]= 10;
					data_led[3]= 10;
					}
					if( vrucUart_read !=0xFF)
					{
						year = vrucUart_read;
						vrucUart_read =0xFF;
					}
					check=fnucKeyScan(scan_phim);
					while(BT_On());	
					delay_ms(10);				
					if(i==0 && check<10)
					{	
							
							i=1;
							year=check;
							check=16;
		
					}
					else if(i==1 && check<10)
					{
							i=0;
							year=year*10 + check;
							check=16;					
					}
					
					break;	
				}					
				
				case 2:			//thang
				{
				data_led[0]= year %10;
				data_led[1]= year /10;
				data_led[2]= 0;
				data_led[3]= 2;
			
				if(blink==1)
				{
				data_led[4]= month %10;
				data_led[5]= month /10;
				}
				else
				{
				data_led[4]=10;
				data_led[5]=10;
				}
					if( vrucUart_read !=0xFF)
					{
						month = vrucUart_read;
						vrucUart_read =0xFF;
					}						
					check=fnucKeyScan(scan_phim);
					while(BT_On());	
					delay_ms(10);				
					if(i==0 && check<10)
					{	
							
							i=1;
							month=check;
							check=16;
		
					}
				
					else if(i==1 && check<10)
					{
						
							i=0;
							month=month*10 + check;
							check=16;					
					}
								
					break;
				}
				
				case 3:			//ngay
				{
				data_led[4]= month %10;
				data_led[5]= month /10;
				if(blink==1)
				{
				data_led[6]= date %10;
				data_led[7]= date /10;
				}
				else
				{
				data_led[6]=10;
				data_led[7]=10;
				}
					if( vrucUart_read !=0xFF)
					{
						date = vrucUart_read;
						vrucUart_read =0xFF;
					}	
					check=fnucKeyScan(scan_phim);
					while(BT_On());	
					delay_ms(10);				
					if(i==0 && check<10)
					{	
							
							i=1;
							date=check;
							check=16;
		
					}
				
					else if(i==1 && check<10)
					{
						
							i=0;
							date=date*10 + check;
							check=16;					
					}				
					break;
					
				}
				
				case 4:			//gio
				{
				data_led[6]= date %10;
				data_led[7]= date /10;
					
				if(blink==1)
				{
				data_led[14]= hour%10;
				data_led[15]= hour/10;
				}
				else
				{
				data_led[14]=10;
				data_led[15]=10;
				}	
					if( vrucUart_read !=0xFF)
					{
						hour = vrucUart_read;
						vrucUart_read =0xFF;
					}		
					check=fnucKeyScan(scan_phim);
					while(BT_On());	
					delay_ms(10);				
					if(i==0 && check<10)
					{			
							i=1;
							hour=check;
							check=16;
					}
				
					else if(i==1 && check<10)
					{
							i=0;
							hour=hour*10 + check;
							check=16;					
					}		
					break;
				}
				case 5:			//phut
				{
				data_led[14]= hour%10;
				data_led[15]= hour/10;
			
				if(blink==1)
				{
				data_led[12]= min%10;
				data_led[13]= min/10;
				}
				else
				{
				data_led[12]=10;
				data_led[13]=10;
				}
				
					if( vrucUart_read !=0xFF)
					{
						min = vrucUart_read;
						vrucUart_read =0xFF;
					}	
					check=fnucKeyScan(scan_phim);
					while(BT_On());	
					delay_ms(10);				
					if(i==0 && check<10)
					{	
							
							i=1;
							min=check;
							check=16;
		
					}
				
					else if(i==1 && check<10)
					{
						
							i=0;
							min=min*10 + check;
							check=16;					
					}		
					break;
				}
				case 6:			//giay
				{
				data_led[12]= min%10;
				data_led[13]= min/10;
				
				if(blink==1)
				{
				data_led[22]= sec%10;
				data_led[23]= sec/10;
				}
				else 
				{
				data_led[22]=10;
				data_led[23]=10;
				}	
					if( vrucUart_read !=0xFF)
					{
						sec = vrucUart_read;
						vrucUart_read =0xFF;
					}	
		
					check=fnucKeyScan(scan_phim);
					while(BT_On());	
					delay_ms(10);				
					if(i==0 && check<10)
					{	
							
							i=1;
							sec=check;
							check=16;
		
					}
				
					else if(i==1 && check<10)
					{
						
							i=0;
							sec=sec*10 + check;
							check=16;					
					}	
					break;
				}
				case 7:		//tong_sp ti lam tiep
				{
					
				data_led[22]= sec%10;	
				data_led[23]= sec/10;
				if(blink==1)
				{
				data_led[8]	= tong_sp %10				;
				data_led[9]	= (tong_sp	/10)%10			;
				data_led[10]= (tong_sp /100)%10			;
				data_led[11]= tong_sp /1000				;		
				}
				else
				{
				data_led[8]	=	10;
				data_led[9]	=	10;
				data_led[10]=	10;
				data_led[11]=	10;	
				}
					if( vrucUart_read !=0xFF && j==0 )
					{
					tong_sp = vrucUart_read;
					vrucUart_read =0xFF;
					j=1;
					}
					if( vrucUart_read !=0xFF && j==1 )
					{
					tong_sp = tong_sp*100 + vrucUart_read;
					vrucUart_read =0xFF;
					j=0;
					}
					check=fnucKeyScan(scan_phim);
					while(BT_On());	
					delay_ms(10);				
					if(i==0 && check<10)
					{	
							
							i=1;
							tong_sp=check;
							check=16;
		
					}
				
					else if(i==1 && check<10)
					{
						
							i=2;
							tong_sp=tong_sp*10 + check;
							check=16;					
					}
					
					else if(i==2 && check<10)
					{
						
							i=3;
							tong_sp=tong_sp*10 + check;
							check=16;					
					}
					
					else if(i==3 && check<10)
					{
						
							i=0;
							tong_sp=tong_sp*10 + check;
							check=16;					
					}	
					
					break;
				}
				case 8:	//	tong_line con led
				{
				data_led[8]	= tong_sp %10				;
				data_led[9]	= (tong_sp	/10)%10			;
				data_led[10]= (tong_sp /100)%10			;
				data_led[11]= tong_sp /1000				;	
				
				if(blink==1)
				{
				data_led[20]= tong_line %10;
				data_led[21]= tong_line /10;
				}
				else
				{
				data_led[20]=10;
				data_led[21]=10;
				}
				
				if (vrucUart_read !=0xFF)
				  {
				    tong_line = vrucUart_read;
				    vrucUart_read =0xFF;
				   }				
					check=fnucKeyScan(scan_phim);
					while(BT_On());	
					delay_ms(10);				
					if(i==0 && check<10)
					{	
							
							i=1;
							tong_line=check;
							check=16;
		
					}
				
					else if(i==1 && check<10)
					{
						
							i=0;
							tong_line=tong_line*10 + check;
							check=16;					
					}		
					
					break;
				}
				case 9:		//line
				{
				data_led[20]= tong_line %10;
				data_led[21]= tong_line /10;
				
				if(blink==1)
				{
				data_led[28]= line %10;
				data_led[29]= line /10;
				}
				else
				{
				data_led[28]=10;
				data_led[29]=10;
				}
				
				if(vrucUart_read !=0xFF)
				{
				    line = vrucUart_read;
				    vrucUart_read =0xFF;
				   }				
					check=fnucKeyScan(scan_phim);
					while(BT_On());	
					delay_ms(10);				
					if(i==0 && check<10)
					{	
							
							i=1;
							line=check;
							check=16;
		
					}
				
					else if(i==1 && check<10)
					{
						
							i=0;
							line=line*10 + check;
							check=16;					
					}		
					
					break;
				}
				case 10:		//ca
				{
				data_led[28]= line %10;
				data_led[29]= line /10;
				
				if(blink==1)
				{
				data_led[30]= ca %10;
				data_led[31]= ca /10;
				}
				else
				{
				data_led[30]=10;
				data_led[31]=10;
				}			
				if(vrucUart_read !=0xFF)
				{
				    ca = vrucUart_read;
				    vrucUart_read =0xFF;
				   }	
					check=fnucKeyScan(scan_phim);
					while(BT_On());	
					delay_ms(10);				
					if(i==0 && check<10)
					{	
							
							i=1;
							ca=check;
							check=16;
		
					}
				
					else if(i==1 && check<10)
					{
						
							i=0;
							ca=ca*10 + check;
							check=16;					
					}				
					break;
				}
	

			}
			
		}

}

/*--------Giao Dien Hien Thi Chon Ca -----------*/
void hien_thi_ca()
{

	unsigned char ds_menu=1,i=0,j=0;
	tat_led();
	while(1)
	{
	data_led[0]=3;
	data_led[7]=12;//hien thu chu C
	data_led[6]=11;//hien thu chu A
	/*hien thi so ca*/
	data_led[4]= ds_menu %10;
	data_led[5]= ds_menu /10;	

		
	vrucUart_read=0xFF;
	check =16;
	stb_scan=fnucKeyScan(scan_phim);
	while(BT_On());	
	delay_ms(10);
	vrucUart_read = Uart_read();
	delay_ms(10);
	if(ds_menu == 6) ds_menu =1;
	if(stb1==2||stb_scan==14||vrucUart_read == 139)	
	{	
    	MODE=3;	
		stb1=0;
		ca=ds_menu;
    	stb_scan=16;
		vrucUart_read=0xFF;
		break;	
	}
	if(stb1==1||stb_scan==15||vrucUart_read == 140)
	{
		ds_menu++;
		stb1=0;
		vrucUart_read=0xFF;
		stb_scan=16;
	}
	switch(ds_menu)
	{
		
		case 1 :
		{
			/*gio bat dau*/
			data_led[14]= vrucHour1_start %10;		
			data_led[15]= vrucHour1_start /10;	
			/*phut bat dau */	
			data_led[12]= vrucMin1_start %10;	
			data_led[13]= vrucMin1_start /10;	
			/*gio ket thuc*/
			data_led[22]=vrucHour1_stop %10	;	
			data_led[23]=vrucHour1_stop /10  ;
			/*phut ket thuc*/
			data_led[10]=vrucMin1_stop %10;
			data_led[11]=vrucMin1_stop /10;
		
			break;
		}
			case 2 :
		{
			/*gio bat dau*/
			data_led[14]= vrucHour2_start %10;		
			data_led[15]= vrucHour2_start /10;	
			/*phut bat dau */	
			data_led[12]= vrucMin2_start %10;	
			data_led[13]= vrucMin2_start /10;	
			/*gio ket thuc*/
			data_led[22]=vrucHour2_stop %10	;	
			data_led[23]=vrucHour2_stop /10  ;
			/*phut ket thuc*/
			data_led[10]=vrucMin2_stop %10;
			data_led[11]=vrucMin2_stop /10;
		
			break;
		}
			case 3 :
		{
			/*gio bat dau*/
			data_led[14]= vrucHour3_start %10;		
			data_led[15]= vrucHour3_start /10;	
			/*phut bat dau */	
			data_led[12]= vrucMin3_start %10;	
			data_led[13]= vrucMin3_start /10;	
			/*gio ket thuc*/
			data_led[22]=vrucHour3_stop %10	;	
			data_led[23]=vrucHour3_stop /10  ;
			/*phut ket thuc*/
			data_led[10]=vrucMin3_stop %10;
			data_led[11]=vrucMin3_stop /10;
		
			break;
		}
			case 4 :
		{
			/*gio bat dau*/
			data_led[14]= vrucHour4_start %10;		
			data_led[15]= vrucHour4_start /10;	
			/*phut bat dau */	
			data_led[12]= vrucMin4_start %10;	
			data_led[13]= vrucMin4_start /10;	
			/*gio ket thuc*/
			data_led[22]=vrucHour4_stop %10	;	
			data_led[23]=vrucHour4_stop /10  ;
			/*phut ket thuc*/
			data_led[10]=vrucMin4_stop %10;
			data_led[11]=vrucMin4_stop /10;
		
			break;
		}
			case 5 :
			{
			/*gio bat dau*/
			data_led[14]= vrucHour5_start %10;		
			data_led[15]= vrucHour5_start /10;	
			/*phut bat dau */	
			data_led[12]= vrucMin5_start %10;	
			data_led[13]= vrucMin5_start /10;	
			/*gio ket thuc*/
			data_led[22]=vrucHour5_stop %10	;	
			data_led[23]=vrucHour5_stop /10  ;
			/*phut ket thuc*/
			data_led[10]=vrucMin5_stop %10;
			data_led[11]=vrucMin5_stop /10;
			break;
			}		
		}
	}
}
/*----------Giao Dien Chinh Sua Ca ---------------*/
void chinh_sua_ca()
{
	unsigned char ds_menu=1,i=0,j=0;
	tat_led();
	while(1)
	{
	FnReset_WD();
	data_led[0]=3;
	data_led[7]=12;//hien thu chu C
	data_led[6]=11;//hien thu chu A
	/*hien thi so ca*/
	data_led[4]= ds_menu %10;
	data_led[5]= ds_menu /10;	
	vrucUart_read=0xFF;
	check =16;
	stb_scan=fnucKeyScan(scan_phim);
	while(BT_On());	
	delay_ms(10);
	vrucUart_read = Uart_read();
	delay_ms(10);
	if(ds_menu == 6) ds_menu =1;
	if(stb1==2||stb_scan==14||vrucUart_read == 139)	
	{	
    	MODE=0;	
		stb1=0;
    	stb_scan=16;
		vrucUart_read=0xFF;
		break;	
	}
	if(stb_scan==0||vrucUart_read == 141)
	{
		stb_scan=16;
		vrucUart_read=0xFF;
		j++;
	}
	if(j==5) j=0;
	if(stb1==1||stb_scan==15||vrucUart_read == 140)
	{
		ds_menu++;
		stb1=0;
		vrucUart_read=0xFF;
		stb_scan=16;
	}
	switch(ds_menu)
	{
		case 1:
		{
			/*gio bat dau*/
			data_led[14]= vrucHour1_start %10;		
			data_led[15]= vrucHour1_start /10;	
			/*phut bat dau */	
			data_led[12]= vrucMin1_start %10;	
			data_led[13]= vrucMin1_start /10;	
			/*gio ket thuc*/
			data_led[22]=vrucHour1_stop %10	;	
			data_led[23]=vrucHour1_stop /10  ;
			/*phut ket thuc*/
			data_led[10]=vrucMin1_stop %10;
			data_led[11]=vrucMin1_stop /10;
				
			if(blink==1)
			{
				if(j==1)
				{
				data_led[14] = vrucHour1_start %10;		
				data_led[15] = vrucHour1_start /10;	
				}
				if(j==2)
				{
				/*phut bat dau */	
				data_led[12] = vrucMin1_start %10;	
				data_led[13] = vrucMin1_start /10;
		
				}
				if(j==3)
				{
				data_led[22] = vrucHour1_stop %10;	
				data_led[23] = vrucHour1_stop /10;	
				}
				if(j==4)
				{
				/*phut ket thuc*/
				data_led[10] = vrucMin1_stop %10  ;
				data_led[11] = vrucMin1_stop /10  ;	
				}	
			}
			else
			 {
			 if(j==1)
			 {
			 data_led[14]=10;
			 data_led[15]=10;
			 }
			 if(j==2)
			 {
			 data_led[12]=10;
			 data_led[13]=10;
			 }
			 if(j==3)
			 {
			 data_led[22]=10;
			 data_led[23]=10;
			 }
			 if(j==4)
			 {
			 data_led[10]=10;
			 data_led[11]=10;	
			 }
				}
					if(j==1)
					{
						data_led[10] = vrucMin1_stop %10  ;
						data_led[11] = vrucMin1_stop /10  ;						
						if( vrucUart_read !=0xFF)
						{
							vrucHour1_start = vrucUart_read;
							vrucUart_read =0xFF;
						}						
						check=fnucKeyScan(scan_phim);
						while(BT_On());	
						delay_ms(10);				
						if(i==0 && check<10)
						{	
								
								i=1;
								vrucHour1_start=check;
								check=16;
			
						}
					
						else if(i==1 && check<10)
						{
							
								i=0;
								vrucHour1_start=vrucHour1_start*10 + check;
								check=16;					
						}
					}
			
					if(j==2)
					{
						data_led[14] = vrucHour1_start %10;		
						data_led[15] = vrucHour1_start /10;	
					  if( vrucUart_read !=0xFF)
					{
						vrucMin1_start = vrucUart_read;
						vrucUart_read =0xFF;
					}						
					check=fnucKeyScan(scan_phim);
					while(BT_On());	
					delay_ms(10);				
					if(i==0 && check<10)
					{	
							
							i=1;
							vrucMin1_start=check;
							check=16;
		
					}
				
					else if(i==1 && check<10)
						{
						
							i=0;
							vrucMin1_start=vrucMin1_start*10 + check;
							check=16;					
						}
					}
					
						if(j==3)
					{
					data_led[12] = vrucMin1_start %10;	
					data_led[13] = vrucMin1_start /10;	
						
					if( vrucUart_read !=0xFF)
					{
						vrucHour1_stop = vrucUart_read;
						vrucUart_read =0xFF;
					}						
					check=fnucKeyScan(scan_phim);
					while(BT_On());	
					delay_ms(10);				
					if(i==0 && check<10)
					{	
							
							i=1;
							vrucHour1_stop=check;
							check=16;
		
					}
				
					else if(i==1 && check<10)
						{
							i=0;
							vrucHour1_stop=vrucHour1_stop*10 + check;
							check=16;					
						}
					}
					
					if(j==4)
					{
						data_led[22] = vrucHour1_stop %10;
						data_led[23] = vrucHour1_stop /10;			
					  if( vrucUart_read !=0xFF)
					{
						vrucMin1_stop = vrucUart_read;
						vrucUart_read =0xFF;
					}						
					check=fnucKeyScan(scan_phim);
					while(BT_On());	
					delay_ms(10);				
					if(i==0 && check<10)
					{	
							
							i=1;
							vrucMin1_stop=check;
							check=16;
		
					}
				
					else if(i==1 && check<10)
					{
						
							i=0;
							vrucMin1_stop=vrucMin1_stop*10 + check;
							check=16;					
					}
				}	
			
				
			break;
		}
		
		
		
		case 2 :
		{
			/*gio bat dau*/
			data_led[14]= vrucHour2_start %10;		
			data_led[15]= vrucHour2_start /10;	
			/*phut bat dau */	
			data_led[12]= vrucMin2_start %10;	
			data_led[13]= vrucMin2_start /10;	
			/*gio ket thuc*/
			data_led[22]=vrucHour2_stop %10	;	
			data_led[23]=vrucHour2_stop /10  ;
			/*phut ket thuc*/
			data_led[10]=vrucMin2_stop %10;
			data_led[11]=vrucMin2_stop /10;	
			
			if(blink==1)
			{
				if(j==1)
				{
				data_led[14] = vrucHour2_start %10;		
				data_led[15] = vrucHour2_start /10;	
				}
				if(j==2)
				{
				/*phut bat dau */	
				data_led[12] = vrucMin2_start %10;	
				data_led[13] = vrucMin2_start /10;
		
				}
				if(j==3)
				{
				data_led[22] = vrucHour2_stop %10;	
				data_led[23] = vrucHour2_stop /10;	
				}
				if(j==4)
				{
				/*phut ket thuc*/
				data_led[10] = vrucMin2_stop %10  ;
				data_led[11] = vrucMin2_stop /10  ;	
				}	
			}
			else
			 {
			 if(j==1)
			 {
			 data_led[14]=10;
			 data_led[15]=10;
			 }
			 if(j==2)
			 {
			 data_led[12]=10;
			 data_led[13]=10;
			 }
			 if(j==3)
			 {
			 data_led[22]=10;
			 data_led[23]=10;
			 }
			 if(j==4)
			 {
			 data_led[10]=10;
			 data_led[11]=10;	
			 }
				}
					if(j==1)
					{
						data_led[10] = vrucMin2_stop %10  ;
						data_led[11] = vrucMin2_stop /10  ;						
						if( vrucUart_read !=0xFF)
						{
							vrucHour2_start = vrucUart_read;
							vrucUart_read =0xFF;
						}						
						check=fnucKeyScan(scan_phim);
						while(BT_On());	
						delay_ms(10);				
						if(i==0 && check<10)
						{	
								
								i=1;
								vrucHour2_start=check;
								check=16;
			
						}
					
						else if(i==1 && check<10)
						{
							
								i=0;
								vrucHour2_start=vrucHour2_start*10 + check;
								check=16;					
						}
					}
			
					if(j==2)
					{
						data_led[14] = vrucHour2_start %10;		
						data_led[15] = vrucHour2_start /10;	
					  if( vrucUart_read !=0xFF)
					{
						vrucMin2_start = vrucUart_read;
						vrucUart_read =0xFF;
					}						
					check=fnucKeyScan(scan_phim);
					while(BT_On());	
					delay_ms(10);				
					if(i==0 && check<10)
					{	
							
							i=1;
							vrucMin2_start=check;
							check=16;
		
					}
				
					else if(i==1 && check<10)
						{
						
							i=0;
							vrucMin2_start=vrucMin2_start*10 + check;
							check=16;					
						}
					}
					
						if(j==3)
					{
					data_led[12] = vrucMin2_start %10;	
					data_led[13] = vrucMin2_start /10;	
						
					if( vrucUart_read !=0xFF)
					{
						vrucHour2_stop = vrucUart_read;
						vrucUart_read =0xFF;
					}						
					check=fnucKeyScan(scan_phim);
					while(BT_On());	
					delay_ms(10);				
					if(i==0 && check<10)
					{	
							
							i=1;
							vrucHour2_stop=check;
							check=16;
		
					}
				
					else if(i==1 && check<10)
						{
							i=0;
							vrucHour2_stop=vrucHour2_stop*10 + check;
							check=16;					
						}
					}
					
					if(j==4)
					{
						data_led[22] = vrucHour2_stop %10;
						data_led[23] = vrucHour2_stop /10;			
					  if( vrucUart_read !=0xFF)
					{
						vrucMin2_stop = vrucUart_read;
						vrucUart_read =0xFF;
					}						
					check=fnucKeyScan(scan_phim);
					while(BT_On());	
					delay_ms(10);				
					if(i==0 && check<10)
					{	
							
							i=1;
							vrucMin2_stop=check;
							check=16;
		
					}
				
					else if(i==1 && check<10)
					{
						
							i=0;
							vrucMin2_stop=vrucMin2_stop*10 + check;
							check=16;					
					}
				}	
			
		
			break;
		}
		
		
		case 3 :
		{
			/*gio bat dau*/
			data_led[14]= vrucHour3_start %10;		
			data_led[15]= vrucHour3_start /10;	
			/*phut bat dau */	
			data_led[12]= vrucMin3_start %10;	
			data_led[13]= vrucMin3_start /10;	
			/*gio ket thuc*/
			data_led[22]=vrucHour3_stop %10	;	
			data_led[23]=vrucHour3_stop /10  ;
			/*phut ket thuc*/
			data_led[10]=vrucMin3_stop %10;
			data_led[11]=vrucMin3_stop /10;		
			
			if(blink==1)
			{
				if(j==1)
				{
				data_led[14] = vrucHour3_start %10;		
				data_led[15] = vrucHour3_start /10;	
				}
				if(j==2)
				{
				/*phut bat dau */	
				data_led[12] = vrucMin3_start %10;	
				data_led[13] = vrucMin3_start /10;
		
				}
				if(j==3)
				{
				data_led[22] = vrucHour3_stop %10;	
				data_led[23] = vrucHour3_stop /10;	
				}
				if(j==4)
				{
				/*phut ket thuc*/
				data_led[10] = vrucMin3_stop %10  ;
				data_led[11] = vrucMin3_stop /10  ;	
				}	
			}
			else
			 {
			 if(j==1)
			 {
			 data_led[14]=10;
			 data_led[15]=10;
			 }
			 if(j==2)
			 {
			 data_led[12]=10;
			 data_led[13]=10;
			 }
			 if(j==3)
			 {
			 data_led[22]=10;
			 data_led[23]=10;
			 }
			 if(j==4)
			 {
			 data_led[10]=10;
			 data_led[11]=10;	
			 }
				}
					if(j==1)
					{
						data_led[10] = vrucMin3_stop %10  ;
						data_led[11] = vrucMin3_stop /10  ;						
						if( vrucUart_read !=0xFF)
						{
							vrucHour3_start = vrucUart_read;
							vrucUart_read =0xFF;
						}						
						check=fnucKeyScan(scan_phim);
						while(BT_On());	
						delay_ms(10);				
						if(i==0 && check<10)
						{	
								
								i=1;
								vrucHour3_start=check;
								check=16;
			
						}
					
						else if(i==1 && check<10)
						{
							
								i=0;
								vrucHour3_start=vrucHour3_start*10 + check;
								check=16;					
						}
					}
			
					if(j==2)
					{
						data_led[14] = vrucHour3_start %10;		
						data_led[15] = vrucHour3_start /10;	
					  if( vrucUart_read !=0xFF)
					{
						vrucMin3_start = vrucUart_read;
						vrucUart_read =0xFF;
					}						
					check=fnucKeyScan(scan_phim);
					while(BT_On());	
					delay_ms(10);				
					if(i==0 && check<10)
					{	
							
							i=1;
							vrucMin3_start=check;
							check=16;
		
					}
				
					else if(i==1 && check<10)
						{
						
							i=0;
							vrucMin3_start=vrucMin3_start*10 + check;
							check=16;					
						}
					}
					
						if(j==3)
					{
					data_led[12] = vrucMin3_start %10;	
					data_led[13] = vrucMin3_start /10;	
						
					if( vrucUart_read !=0xFF)
					{
						vrucHour3_stop = vrucUart_read;
						vrucUart_read =0xFF;
					}						
					check=fnucKeyScan(scan_phim);
					while(BT_On());	
					delay_ms(10);				
					if(i==0 && check<10)
					{	
							
							i=1;
							vrucHour3_stop=check;
							check=16;
		
					}
				
					else if(i==1 && check<10)
						{
							i=0;
							vrucHour3_stop=vrucHour3_stop*10 + check;
							check=16;					
						}
					}
					
					if(j==4)
					{
						data_led[22] = vrucHour3_stop %10;
						data_led[23] = vrucHour3_stop /10;			
					  if( vrucUart_read !=0xFF)
					{
						vrucMin3_stop = vrucUart_read;
						vrucUart_read =0xFF;
					}						
					check=fnucKeyScan(scan_phim);
					while(BT_On());	
					delay_ms(10);				
					if(i==0 && check<10)
					{	
							
							i=1;
							vrucMin3_stop=check;
							check=16;
		
					}
				
					else if(i==1 && check<10)
					{
						
							i=0;
							vrucMin3_stop=vrucMin3_stop*10 + check;
							check=16;					
					}
				}	
	
			break;
		}
		
		
		
		
		case 4 :
		{
			/*gio bat dau*/
			data_led[14]= vrucHour4_start %10;		
			data_led[15]= vrucHour4_start /10;	
			/*phut bat dau */	
			data_led[12]= vrucMin4_start %10;	
			data_led[13]= vrucMin4_start /10;	
			/*gio ket thuc*/
			data_led[22]=vrucHour4_stop %10	;	
			data_led[23]=vrucHour4_stop /10  ;
			/*phut ket thuc*/
			data_led[10]=vrucMin4_stop %10;
			data_led[11]=vrucMin4_stop /10;
			
			
			
			if(blink==1)
			{
				if(j==1)
				{
				data_led[14] = vrucHour4_start %10;		
				data_led[15] = vrucHour4_start /10;	
				}
				if(j==2)
				{
				/*phut bat dau */	
				data_led[12] = vrucMin4_start %10;	
				data_led[13] = vrucMin4_start /10;
		
				}
				if(j==3)
				{
				data_led[22] = vrucHour4_stop %10;	
				data_led[23] = vrucHour4_stop /10;	
				}
				if(j==4)
				{
				/*phut ket thuc*/
				data_led[10] = vrucMin4_stop %10  ;
				data_led[11] = vrucMin4_stop /10  ;	
				}	
			}
			else
			 {
			 if(j==1)
			 {
			 data_led[14]=10;
			 data_led[15]=10;
			 }
			 if(j==2)
			 {
			 data_led[12]=10;
			 data_led[13]=10;
			 }
			 if(j==3)
			 {
			 data_led[22]=10;
			 data_led[23]=10;
			 }
			 if(j==4)
			 {
			 data_led[10]=10;
			 data_led[11]=10;	
			 }
				}
					if(j==1)
					{
						data_led[10] = vrucMin4_stop %10  ;
						data_led[11] = vrucMin4_stop /10  ;						
						if( vrucUart_read !=0xFF)
						{
							vrucHour4_start = vrucUart_read;
							vrucUart_read =0xFF;
						}						
						check=fnucKeyScan(scan_phim);
						while(BT_On());	
						delay_ms(10);				
						if(i==0 && check<10)
						{	
								
								i=1;
								vrucHour4_start=check;
								check=16;
			
						}
					
						else if(i==1 && check<10)
						{
							
								i=0;
								vrucHour4_start=vrucHour4_start*10 + check;
								check=16;					
						}
					}
			
					if(j==2)
					{
						data_led[14] = vrucHour4_start %10;		
						data_led[15] = vrucHour4_start /10;	
					  if( vrucUart_read !=0xFF)
					{
						vrucMin4_start = vrucUart_read;
						vrucUart_read =0xFF;
					}						
					check=fnucKeyScan(scan_phim);
					while(BT_On());	
					delay_ms(10);				
					if(i==0 && check<10)
					{	
							
							i=1;
							vrucMin4_start=check;
							check=16;
		
					}
				
					else if(i==1 && check<10)
						{
						
							i=0;
							vrucMin4_start=vrucMin4_start*10 + check;
							check=16;					
						}
					}
					
						if(j==3)
					{
					data_led[12] = vrucMin4_start %10;	
					data_led[13] = vrucMin4_start /10;	
						
					if( vrucUart_read !=0xFF)
					{
						vrucHour4_stop = vrucUart_read;
						vrucUart_read =0xFF;
					}						
					check=fnucKeyScan(scan_phim);
					while(BT_On());	
					delay_ms(10);				
					if(i==0 && check<10)
					{	
							
							i=1;
							vrucHour4_stop=check;
							check=16;
		
					}
				
					else if(i==1 && check<10)
						{
							i=0;
							vrucHour4_stop=vrucHour4_stop*10 + check;
							check=16;					
						}
					}
					
					if(j==4)
					{
						data_led[22] = vrucHour4_stop %10;
						data_led[23] = vrucHour4_stop /10;			
					  if( vrucUart_read !=0xFF)
					{
						vrucMin4_stop = vrucUart_read;
						vrucUart_read =0xFF;
					}						
					check=fnucKeyScan(scan_phim);
					while(BT_On());	
					delay_ms(10);				
					if(i==0 && check<10)
					{	
							
							i=1;
							vrucMin4_stop=check;
							check=16;
		
					}
				
					else if(i==1 && check<10)
					{
						
							i=0;
							vrucMin4_stop=vrucMin4_stop*10 + check;
							check=16;					
					}
				}	
		
			break;
		}
		
		
		
		
		case 5 :
		{
			/*gio bat dau*/
			data_led[14]= vrucHour5_start %10;		
			data_led[15]= vrucHour5_start /10;	
			/*phut bat dau */	
			data_led[12]= vrucMin5_start %10;	
			data_led[13]= vrucMin5_start /10;	
			/*gio ket thuc*/
			data_led[22]=vrucHour5_stop %10	;	
			data_led[23]=vrucHour5_stop /10  ;
			/*phut ket thuc*/
			data_led[10]=vrucMin5_stop %10;
			data_led[11]=vrucMin5_stop /10;
						
			if(blink==1)
			{
				if(j==1)
				{
				data_led[14] = vrucHour5_start %10;		
				data_led[15] = vrucHour5_start /10;	
				}
				if(j==2)
				{
				/*phut bat dau */	
				data_led[12] = vrucMin5_start %10;	
				data_led[13] = vrucMin5_start /10;
		
				}
				if(j==3)
				{
				data_led[22] = vrucHour5_stop %10;	
				data_led[23] = vrucHour5_stop /10;	
				}
				if(j==4)
				{
				/*phut ket thuc*/
				data_led[10] = vrucMin5_stop %10  ;
				data_led[11] = vrucMin5_stop /10  ;	
				}	
			}
			else
			 {
			 if(j==1)
			 {
			 data_led[14]=10;
			 data_led[15]=10;
			 }
			 if(j==2)
			 {
			 data_led[12]=10;
			 data_led[13]=10;
			 }
			 if(j==3)
			 {
			 data_led[22]=10;
			 data_led[23]=10;
			 }
			 if(j==4)
			 {
			 data_led[10]=10;
			 data_led[11]=10;	
			 }
				}
					if(j==1)
					{
						data_led[10] = vrucMin5_stop %10  ;
						data_led[11] = vrucMin5_stop /10  ;						
						if( vrucUart_read !=0xFF)
						{
							vrucHour5_start = vrucUart_read;
							vrucUart_read =0xFF;
						}						
						check=fnucKeyScan(scan_phim);
						while(BT_On());	
						delay_ms(10);				
						if(i==0 && check<10)
						{	
								
								i=1;
								vrucHour5_start=check;
								check=16;
			
						}
					
						else if(i==1 && check<10)
						{
							
								i=0;
								vrucHour5_start=vrucHour5_start*10 + check;
								check=16;					
						}
					}
			
					if(j==2)
					{
						data_led[14] = vrucHour5_start %10;		
						data_led[15] = vrucHour5_start /10;	
					  if( vrucUart_read !=0xFF)
					{
						vrucMin5_start = vrucUart_read;
						vrucUart_read =0xFF;
					}						
					check=fnucKeyScan(scan_phim);
					while(BT_On());	
					delay_ms(10);				
					if(i==0 && check<10)
					{	
							
							i=1;
							vrucMin5_start=check;
							check=16;
		
					}
				
					else if(i==1 && check<10)
						{
						
							i=0;
							vrucMin5_start=vrucMin5_start*10 + check;
							check=16;					
						}
					}
					
						if(j==3)
					{
					data_led[12] = vrucMin5_start %10;	
					data_led[13] = vrucMin5_start /10;	
						
					if( vrucUart_read !=0xFF)
					{
						vrucHour5_stop = vrucUart_read;
						vrucUart_read =0xFF;
					}						
					check=fnucKeyScan(scan_phim);
					while(BT_On());	
					delay_ms(10);				
					if(i==0 && check<10)
					{	
							
							i=1;
							vrucHour5_stop=check;
							check=16;
		
					}
				
					else if(i==1 && check<10)
						{
							i=0;
							vrucHour5_stop=vrucHour5_stop*10 + check;
							check=16;					
						}
					}
					
					if(j==4)
					{
						data_led[22] = vrucHour5_stop %10;
						data_led[23] = vrucHour5_stop /10;			
					  if( vrucUart_read !=0xFF)
					{
						vrucMin5_stop = vrucUart_read;
						vrucUart_read =0xFF;
					}						
					check=fnucKeyScan(scan_phim);
					while(BT_On());	
					delay_ms(10);				
					if(i==0 && check<10)
					{	
							
							i=1;
							vrucMin5_stop=check;
							check=16;
		
					}
				
					else if(i==1 && check<10)
					{
						
							i=0;
							vrucMin5_stop=vrucMin5_stop*10 + check;
							check=16;					
					}
				}	
		
			break;
		}	
		
	
	}
		
}

	
}

main()
{
	Init_all();
	Init_WD();
	//DS1307_Set();
	MODE=0;
	BEEP=1;
	BT1=BT2=BT3=BT4=1;
	Led1=0;
	Led2=Led3=0;

	while(1)
	{
//	FnReset_WD();//6.5s
//	Led2=~Led2;
//	delay_ms(500);

		if(MODE==0)hien_thi();
		if(MODE==1)chinh_sua();
		if(MODE==2)chinh_sua_ca();
	
	}
}
		
/*---------------Ngat_Uart-------------*/
void Ngat_Uart(void) interrupt 4
{
	static unsigned char dem_data=0;
	unsigned char value;
	if(RI == 1)
	{
		value=SBUF;
		RI=0;
		if(value == 'A')
			{	
				dem_data=0;
				Buff_data[dem_data]=value;
				dem_data++;
			}
		 else if(dem_data!=0 && dem_data <4 )
		{
			Buff_data[dem_data]=value;
			dem_data++;			
			if(value== 'Z')
			{
			received_data[0]=Buff_data[1]-0x30;
			received_data[1]=Buff_data[2]-0x30;	
			dem_data=0;
			}
		}
	}
}
	
/*Ngat timer 0*/
	
void Ngat_Timer(void) interrupt 1
{		
	static unsigned char scan_led=1,dem=0,dem1=0;;

	TL0=0x00;
	TH0=0xF7;	// dat lai gia tri ban dau
	quet_led(scan_led);
	scan_led = _crol_(scan_led,1);
	if(MODE ==1 ||MODE ==3)
	{
		if(++dem ==250)
		{
		blink=~blink;
		dem=0;
		Led1=~Led1;
		}
	}
	if(vaoca==1)
	{
		if(++dem==250) Led2 =~Led2;
	}
	if(hetca==1)
	{
		if(++dem==250) Led3 =~Led3;
	}
	if(++dem1 ==50)
	{
	scan_phim = _crol_(scan_phim,1);
	dem1=0;
	}
		if((sec>=10 && vaoca==1) || (sec>=10 && hetca==1) )
		{
			BEEP=1;
			if(vaoca==1)
			{
				Led3=0;
				vaoca=0;
				Led2=1;
			}
			if(hetca==1)
			{
				Led2=0;
				hetca=0;
				Led3=1;	
			}
		}
	
	check_phim();
	//TF0=0;// Con nuvoton tu xoa co 
}

/*--------------The End---------------*/
