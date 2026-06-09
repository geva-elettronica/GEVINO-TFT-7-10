
  /*************** RAIO Technology Inc. ***************
  * file		: RA8876.c
  * author		: RAIO Application Team 
  * version		: V1.0 
  * date		: 2015/09/24  
  * brief		: 	
  ****************************************************/	
#include "gevinoTFT_RA8876.h"
#define	cSetb0		0x01
#define	cSetb1		0x02
#define	cSetb2		0x04
#define	cSetb3		0x08
#define	cSetb4		0x10
#define	cSetb5		0x20
#define	cSetb6		0x40
#define	cSetb7		0x80
#define	cClrb0		0xfe
#define	cClrb1		0xfd
#define	cClrb2		0xfb
#define	cClrb3		0xf7
#define	cClrb4		0xef
#define	cClrb5		0xdf
#define	cClrb6		0xbf
#define	cClrb7		0x7f

//------------------------------------------------------------------------------
// Software Setting
//------------------------------------------------------------------------------

/*==== [SW_(1)]  PLL  =====*/
#define OSC_FREQ	10	// OSC clock frequency, unit: MHz.
#define DRAM_FREQ	140	// SDRAM clock frequency, unit: MHz. 
#define CORE_FREQ	120	// Core (system) clock frequency, unit: MHz. 
#define SCAN_FREQ	56	// Panel Scan clock frequency, unit: MHz.

/*==== [SW_(2)]  select MCU and color depth  =====*/
//	#define MCU_8bit_ColorDepth_8bpp			  
	#define MCU_8bit_ColorDepth_16bpp		
//	#define MCU_8bit_ColorDepth_24bpp
//	#define MCU_16bit_ColorDepth_16bpp		
//	#define MCU_16bit_ColorDepth_24bpp_Mode_1
//	#define MCU_16bit_ColorDepth_24bpp_Mode_2


/*==== [SW_()]  Master I2C  =====*/
#define Enable_I2CM_Noise_Filter
//#define Disable_I2CM_Noise_Filter


//==============================================================================
/******************************************************************************
RA8876 Subroutine		  													 
******************************************************************************/
//==============================================================================

//==============================================================================
void RA8876::Check_Mem_WR_FIFO_not_Full(void)
{
/*[Status Register] bit7
Host Memory Write FIFO full
0: Memory Write FIFO is not full.
1: Memory Write FIFO is full.
Only when Memory Write FIFO is not full, MPU may write another
one pixel.
*/ 
	unsigned short i;

	/* you can using the following */
	// Please according to your MCU speed to modify. // �Ш̾ڱz��MCU�t�׭ק�ɶ�
	for(i=0;i<1000;i++)
	{
		if( (readStatus()&0x80)==0 ){break;}
	}
	/* or the following */
	do{	}
	while( readStatus()&0x80 );
}
//---------------------//
void RA8876::Check_Mem_WR_FIFO_Empty(void)
{
/*[Status Register] bit6
Host Memory Write FIFO empty
0: Memory Write FIFO is not empty.
1: Memory Write FIFO is empty.
When Memory Write FIFO is empty, MPU may write 8bpp data 64
pixels, or 16bpp data 32 pixels, 24bpp data 16 pixels directly.
*/	
	unsigned short i;		 

	/* you can using the following */
	// Please according to your MCU speed to modify. // �Ш̾ڱz��MCU�t�׭ק�ɶ�
	for(i=0;i<1000;i++)
	{
		if( (readStatus()&0x40)==0x40 ){break;}
	}
	/* or the following */
	do{	}
	while( (readStatus()&0x40) == 0x00 );
}
//---------------------//
void RA8876::Check_Mem_RD_FIFO_not_Full(void)
{
/*	[Status Register] bit5
Host Memory Read FIFO full
0: Memory Read FIFO is not full.
1: Memory Read FIFO is full.
When Memory Read FIFO is full, MPU may read 8bpp data 32
pixels, or 16bpp data 16 pixels, 24bpp data 8 pixels directly.
*/
	unsigned short i;

	/* you can using the following */
	// Please according to your MCU speed to modify. // �Ш̾ڱz��MCU�t�׭ק�ɶ�
	for(i=0;i<1000;i++)
	{
		if( (readStatus()&0x20)==0x00 ){break;}
	}
	/* or the following */
	do{	}
	while( readStatus()&0x20 );
}
//---------------------//
void RA8876::Check_Mem_RD_FIFO_Full(void)
{
/*	[Status Register] bit5
Host Memory Read FIFO full
0: Memory Read FIFO is not full.
1: Memory Read FIFO is full.
When Memory Read FIFO is full, MPU may read 8bpp data 32
pixels, or 16bpp data 16 pixels, 24bpp data 8 pixels directly.
*/
	unsigned short i;

	/* you can using the following */
	// Please according to your MCU speed to modify. // �Ш̾ڱz��MCU�t�׭ק�ɶ�
	for(i=0;i<1000;i++)
	{
		if( (readStatus()&0x20)==0x20 ){break;}
	}
	/* or the following */
	do{	}
	while( (readStatus()&0x20)==0x00  );
}
//---------------------//
void RA8876::Check_Mem_RD_FIFO_not_Empty(void)
{
/*	[Status Register] bit4
Host Memory Read FIFO empty
0: Memory Read FIFO is not empty.
1: Memory Read FIFO is empty.
*/
	unsigned short i;

	/* you can using the following */
	// Please according to your MCU speed to modify. // �Ш̾ڱz��MCU�t�׭ק�ɶ�
	for(i=0;i<1000;i++)
	{
		if( (readStatus()&0x10)==0x00 ){break;}
	}
	/* or the following */
	do{	}
	while( readStatus()&0x10 );
}
//---------------------//
void RA8876::Check_2D_Busy(void)
{
/*	[Status Register] bit3
Core task is busy
Following task is running:
BTE, Geometry engine, Serial flash DMA, Text write or Graphic write
0: task is done or idle.
1: task is busy.													
*/
	unsigned short i;
	
	/* you can using the following */
	// Please according to your MCU speed to modify. // �Ш̾ڱz��MCU�t�׭ק�ɶ�
		for(i=0;i<1000;i++)
		{
			if( (readStatus()&0x08)==0x00 ){break;}
		}

	/* or the following */
		do{	}
		while(( readStatus()&0x08)==0x08 );

}  
//---------------------//
void RA8876::Check_SDRAM_Ready(void)
{
/*	[Status Register] bit2
SDRAM ready for access
0: SDRAM is not ready for access
1: SDRAM is ready for access		
*/	
	unsigned short i;
	/* you can using the following */
	// Please according to your MCU speed to modify. // �Ш̾ڱz��MCU�t�׭ק�ɶ�
	for(i=0;i<1000;i++)
	{
		if( (readStatus()&0x04)==0x04 ){break;}
	}
	/* or the following */
	do{	}
	while( (readStatus()&0x04) == 0x00 );
}
//---------------------// 
void RA8876::Check_IC_ready(void)
{
/*	[Status Register] bit1
Operation mode status
0: Normal operation state
1: Inhibit operation state
Inhibit operation state means internal reset event keep running or
initial display still running or chip enter power saving state.		
*/	
	unsigned short i;
	/* you can using the following */
	// Please according to your MCU speed to modify. // �Ш̾ڱz��MCU�t�׭ק�ɶ�
	for(i=0;i<1000;i++)
	{
		if( (readStatus()&0x02)==0x00 ){break;}
	}
	/* or the following */
	do{	}
	while(readStatus() & 0x02);
}
//---------------------//
unsigned char RA8876::Power_Saving_Status(void)
{
/*	[Status Register] bit1
Operation mode status
0: Normal operation state
1: Inhibit operation state
Inhibit operation state means internal reset event keep running or
initial display still running or chip enter power saving state.		
*/
  	  unsigned char temp;

	  if((readStatus()&0x02)==0x02)
	    temp = 1;
	  else
	    temp = 0;

	  return temp;
}

void RA8876::Check_Power_is_Saving(void)
{  
/*	[Status Register] bit1
Operation mode status
0: Normal operation state
1: Inhibit operation state
Inhibit operation state means internal reset event keep running or
initial display still running or chip enter power saving state.		
*/
	unsigned short i;
	/* you can using the following */
	// Please according to your MCU speed to modify. // �Ш̾ڱz��MCU�t�׭ק�ɶ�
	for(i=0;i<1000;i++)
	{
		if( (readStatus()&0x02)==0x02 ){break;}
	}
	/* or the following */
	do{	}
	while( (readStatus()&0x02) == 0x00 );
}
//---------------------//
void RA8876::Check_NO_Interrupt(void)
{
/*	[Status Register] bit0
Interrupt pin state
0: without interrupt active
1: interrupt active		
*/	
	unsigned short i;
	/* you can using the following */
	// Please according to your MCU speed to modify. // �Ш̾ڱz��MCU�t�׭ק�ɶ�
	for(i=0;i<1000;i++)
	{
		if( (readStatus()&0x01)==0x00 ){break;}
	}
	/* or the following */
	do{	}
	while( readStatus()&0x01 );
}
void RA8876::Check_Interrupt_Occur(void)
{
/*	[Status Register] bit0
Interrupt pin state
0: without interrupt active
1: interrupt active		
*/	
	unsigned short i;
	/* you can using the following */
	// Please according to your MCU speed to modify. // �Ш̾ڱz��MCU�t�׭ק�ɶ�
	for(i=0;i<1000;i++)
	{
		if( (readStatus()&0x01)==0x01 ){break;}
	}
	/* or the following */
	do{	}
	while( (readStatus()&0x01) == 0x00 );
}

//==============================================================================



//[00h]=========================================================================

void RA8876::RA8876_SW_Reset(void)
{/*
Software Reset
0: Normal operation.
1: Software Reset.
Software Reset only reset internal state machine. Configuration
Registers value won��t be reset. So all read-only flag in the
register will return to its initial value. User should have proper
action to make sure flag is in desired state.
Note: The bit will auto clear after reset.
*/	
	unsigned short i;
	unsigned char temp;

	writeCmd(0x00);
	temp = readData();
	temp |= 0x01;
	writeData(temp);
	delay(100);			// it must wait 100us after Software_Reset.

	// Please according to your MCU speed to modify. // �Ш̾ڱz��MCU�t�׭ק�ɶ�
	for(i=0;i<100;i++)
	{
  	if( (readReg(0x00)&0x01)==0x00 )
		{break;}
	}

}

//[01h][01h][01h][01h][01h][01h][01h][01h][01h][01h][01h][01h][01h][01h][01h][01h]
void RA8876::Enable_PLL(void)
{
/*  0: PLL disable; allow change PLL parameter.
    1: PLL enable; cannot change PLL parameter.*/
	unsigned char temp;
	unsigned short i;

	writeCmd(0x01);
	temp = readData();
	temp |= cSetb7;
	writeData(temp);

	delay(10);// PLL lock time = 1024 T OSC clocks, if OSC=10MHz, PLL lock time = 10 us.  

	/*check PLL was ready ( Please according to your MCU speed to modify. �Ш̾ڱz��MCU�t�׭ק�ɶ�)	 */
	for(i=0;i<100;i++)
	{
		writeCmd(0x01);
		temp=readData();
		if( (temp&0x80)==0x80 ){break;}
	}
}

void RA8876::Sent_XnWAIT_Status_When_CS_Low(void)
{/*  
	Mask XnWAIT on XnCS deassert
	0 : No mask
		XnWAIT keep assert if internal state keep busy and cannot 
		accept next R/W cycle, no matter XnCS assert/deassert. If
		MCU cycle cannot be extended while XnWAIT keep low, user
		should poll XnWAIT and wait it goes high then start next
		access.
	1 : Mask
		XnWAIT deassert when XnCS deassert. Use in MCU cycle can
		be extended by XnWAIT automatically.
*/
	unsigned char temp;
	writeCmd(0x01);
	temp = readData();
	temp |= cSetb6;
	writeData(temp);    
}
void RA8876::Sent_XnWAIT_Status_At_Any_Time(void)
{/*  
	Mask XnWAIT on XnCS deassert
	0 : No mask
		XnWAIT keep assert if internal state keep busy and cannot 
		accept next R/W cycle, no matter XnCS assert/deassert. If
		MCU cycle cannot be extended while XnWAIT keep low, user
		should poll XnWAIT and wait it goes high then start next
		access.
	1 : Mask
		XnWAIT deassert when XnCS deassert. Use in MCU cycle can
		be extended by XnWAIT automatically.
*/
	unsigned char temp;
	writeCmd(0x01);
	temp = readData();
	temp &= cClrb6;
	writeData(temp);    
}
void RA8876::Key_Scan_Enable(void)
{
/*  0: Disable.
    1: Enable.*/
	unsigned char temp;
	writeCmd(0x01);
	temp = readData();
	temp |= cSetb5;
	writeData(temp);    
}
void RA8876::Key_Scan_Disable(void)
{
/*  0: Disable.
    1: Enable.*/
	unsigned char temp;
	writeCmd(0x01);
	temp = readData();
	temp &= cClrb5;
	writeData(temp);    
}
void RA8876::TFT_24bit(void)//RA8876 only
{
/*  TFT Panel I/F Output pin Setting
00b: 24-bits TFT output.
01b: 18-bits TFT output.
10b: 16-bits TFT output.
11b: w/o TFT output.
Other unused TFT output pins are set as GPIO or Key function.*/
	unsigned char temp;
	writeCmd(0x01);
	temp = readData();
	temp &= cClrb4;
    temp &= cClrb3;
	writeData(temp);    
}
void RA8876::TFT_18bit(void)//RA8876 only
{
/*  TFT Panel I/F Output pin Setting
00b: 24-bits TFT output.
01b: 18-bits TFT output.
10b: 16-bits TFT output.
11b: w/o TFT output.
Other unused TFT output pins are set as GPIO or Key function.*/
	unsigned char temp;
	writeCmd(0x01);
	temp = readData();
	temp &= cClrb4;
    temp |= cSetb3;
	writeData(temp);  
}
void RA8876::TFT_16bit(void)//RA8876 only
{
/*  TFT Panel I/F Output pin Setting
00b: 24-bits TFT output.
01b: 18-bits TFT output.
10b: 16-bits TFT output.
11b: w/o TFT output.
Other unused TFT output pins are set as GPIO or Key function.*/
	unsigned char temp;
	writeCmd(0x01);
	temp = readData();
	temp |= cSetb4;
    temp &= cClrb3;
	writeData(temp);  
}
void RA8876::Without_TFT(void)//RA8876 only
{
/*  TFT Panel I/F Output pin Setting
00b: 24-bits TFT output.
01b: 18-bits TFT output.
10b: 16-bits TFT output.
11b: w/o TFT output.
Other unused TFT output pins are set as GPIO or Key function.*/
	unsigned char temp;
	writeCmd(0x01);
	temp = readData();
	temp |= cSetb4;
    temp |= cSetb3;
	writeData(temp);  
}

void RA8876::RA8876_I2CM_Enable(void)
{
/*  I2C master Interface Enable/Disable
		0: Disable (GPIO function)
		1: Enable (I2C master function)*/
	unsigned char temp;
	writeCmd(0x01);
	temp = readData();
	temp |= cSetb2;
	writeData(temp);    
}

void RA8876::RA8876_I2CM_Disable(void)
{
/*  I2C master Interface Enable/Disable
		0: Disable (GPIO function)
		1: Enable (I2C master function)*/
	unsigned char temp;
	writeCmd(0x01);
	temp = readData();
	temp &= cClrb2;
	writeData(temp);     
}

void RA8876::Enable_SFlash_SPI(void)
{
/*	Serial Flash or SPI Interface Enable/Disable
		0: Disable (GPIO function)
		1: Enable (SPI master function)
		When SDR SDRAM 32bits bus function enable, this bit is ignored
		& Serial flash pins become SDR SDRAM bus function.*/
	unsigned char temp;
	writeCmd(0x01);
	temp = readData();
	temp |= cSetb1;
	writeData(temp);     
}

void RA8876::Disable_SFlash_SPI(void)
{
/*	Serial Flash or SPI Interface Enable/Disable
		0: Disable (GPIO function)
		1: Enable (SPI master function)
		When SDR SDRAM 32bits bus function enable, this bit is ignored
		& Serial flash pins become SDR SDRAM bus function.*/
	unsigned char temp;
	writeCmd(0x01);
	temp = readData();
	temp &= cClrb1;
	writeData(temp);     
}
void RA8876::Host_Bus_8bit(void)
{
/*  Parallel Host Data Bus Width Selection
    0: 8-bit Parallel Host Data Bus.
    1: 16-bit Parallel Host Data Bus.*/
	unsigned char temp;
	writeCmd(0x01);
	temp = readData();
	temp &= cClrb0;
	writeData(temp);
}
void RA8876::Host_Bus_16bit(void)
{
/*  Parallel Host Data Bus Width Selection
    0: 8-bit Parallel Host Data Bus.
    1: 16-bit Parallel Host Data Bus.*/
	unsigned char temp;
	writeCmd(0x01);
	temp = readData();
	temp |= cSetb0;
	writeData(temp);
}

//[02h][02h][02h][02h][02h][02h][02h][02h][02h][02h][02h][02h][02h][02h][02h][02h]

void RA8876::Data_Format_8b_8bpp(void)
{
/* MPU read/write data format when access memory data port.
0xb: Direct write for 
	all 8 bits MPU I/F,
	16 bits MPU I/F with 8bpp data mode 2, 
	16 bits MPU I/F with 16bpp, 
	16 bits MPU I/F with 24bpp data mode 1, 
	and serial host interface.				 				*/
	unsigned char temp;
	writeCmd(0x02);
	temp = readData();
	temp &= cClrb7;
	writeData(temp);
}
void RA8876::Data_Format_8b_16bpp(void)
{  
/* MPU read/write data format when access memory data port.
0xb: Direct write for 
	all 8 bits MPU I/F,
	16 bits MPU I/F with 8bpp data mode 2, 
	16 bits MPU I/F with 16bpp, 
	16 bits MPU I/F with 24bpp data mode 1, 
	and serial host interface.				 				*/
	unsigned char temp;
	writeCmd(0x02);
	temp = readData();
	temp &= cClrb7;
	writeData(temp);
}
void RA8876::Data_Format_8b_24bpp(void)
{
/* MPU read/write data format when access memory data port.
0xb: Direct write for 
	all 8 bits MPU I/F,
	16 bits MPU I/F with 8bpp data mode 2, 
	16 bits MPU I/F with 16bpp, 
	16 bits MPU I/F with 24bpp data mode 1, 
	and serial host interface.				 				*/
	unsigned char temp;
	writeCmd(0x02);
	temp = readData();
	temp &= cClrb7;
	writeData(temp);
}

void RA8876::Data_Format_16b_8bpp(void)
{
/* MPU read/write data format when access memory data port.
10b: Mask high byte of each data (ex. 16 bit MPU I/F with 8-bpp data mode 1)	*/
	unsigned char temp;
	writeCmd(0x02);
	temp = readData();
	temp |= cSetb7;
	temp &= cClrb6;
	writeData(temp);
}
void RA8876::Data_Format_16b_16bpp(void)
{
/* MPU read/write data format when access memory data port.
0xb: Direct write for 
	all 8 bits MPU I/F,
	16 bits MPU I/F with 8bpp data mode 2, 
	16 bits MPU I/F with 16bpp, 
	16 bits MPU I/F with 24bpp data mode 1, 
	and serial host interface.				 				*/
	unsigned char temp;
	writeCmd(0x02);
	temp = readData();
	temp &= cClrb7;
//	temp |= cSetb6;
	writeData(temp);
}
void RA8876::Data_Format_16b_24bpp_mode1(void)
{
/* MPU read/write data format when access memory data port.
0xb: Direct write for 
	all 8 bits MPU I/F,
	16 bits MPU I/F with 8bpp data mode 2, 
	16 bits MPU I/F with 16bpp, 
	16 bits MPU I/F with 24bpp data mode 1, 
	and serial host interface.				 				*/
	unsigned char temp;
	writeCmd(0x02);
	temp = readData();
	temp &= cClrb7;
//	temp &= cClrb6;
	writeData(temp);
}
void RA8876::Data_Format_16b_24bpp_mode2(void)
{
/* MPU read/write data format when access memory data port.
11b: Mask high byte of even data (ex. 16 bit MPU I/F with 24-bpp data mode 2)	*/
	unsigned char temp;
	writeCmd(0x02);
	temp = readData();
	temp |= cSetb7;
	temp |= cSetb6;
	writeData(temp);
}

void RA8876::MemRead_Left_Right_Top_Down(void)
{
/* Host Read Memory Direction (Only for Graphic Mode)
00b: Left .. Right then Top .. Bottom.
Ignored if canvas in linear addressing mode.		*/
	unsigned char temp;
	writeCmd(0x02);
	temp = readData();
	temp &= cClrb5;
	temp &= cClrb4;
	writeData(temp);
}
void RA8876::MemRead_Right_Left_Top_Down(void)
{
/* Host Read Memory Direction (Only for Graphic Mode)
01b: Right .. Left then Top ..Bottom.
Ignored if canvas in linear addressing mode.		*/
	unsigned char temp;
	writeCmd(0x02);
	temp = readData();
	temp &= cClrb5;
	temp |= cSetb4;
	writeData(temp);
}
void RA8876::MemRead_Top_Down_Left_Right(void)
{
/* Host Read Memory Direction (Only for Graphic Mode)
10b: Top .. Bottom then Left .. Right.
Ignored if canvas in linear addressing mode.		*/
	unsigned char temp;
	writeCmd(0x02);
	temp = readData();
	temp |= cSetb5;
    temp &= cClrb4;
	writeData(temp);
}
void RA8876::MemRead_Down_Top_Left_Right(void)
{
/* Host Read Memory Direction (Only for Graphic Mode)
11b: Bottom .. Top then Left .. Right.
Ignored if canvas in linear addressing mode.		*/
	unsigned char temp;
	writeCmd(0x02);
	temp = readData();
	temp |= cSetb5;
	temp |= cSetb4;
	writeData(temp);
}
void RA8876::MemWrite_Left_Right_Top_Down(void)
{
/* Host Write Memory Direction (Only for Graphic Mode)
00b: Left .. Right then Top ..Bottom.
Ignored if canvas in linear addressing mode.		*/
	unsigned char temp;
	writeCmd(0x02);
	temp = readData();
	temp &= cClrb2;
	temp &= cClrb1;
	writeData(temp);
}
void RA8876::MemWrite_Right_Left_Top_Down(void)
{
/* Host Write Memory Direction (Only for Graphic Mode)
01b: Right .. Left then Top .. Bottom.
Ignored if canvas in linear addressing mode.		*/
	unsigned char temp;
	writeCmd(0x02);
	temp = readData();
	temp &= cClrb2;
	temp |= cSetb1;
	writeData(temp);
}
void RA8876::MemWrite_Top_Down_Left_Right(void)
{
/* Host Write Memory Direction (Only for Graphic Mode)
10b: Top .. Bottom then Left .. Right.
Ignored if canvas in linear addressing mode.		*/
	unsigned char temp;
	writeCmd(0x02);
	temp = readData();
	temp |= cSetb2;
    temp &= cClrb1;
	writeData(temp);
}
void RA8876::MemWrite_Down_Top_Left_Right(void)
{
/* Host Write Memory Direction (Only for Graphic Mode)
11b: Bottom .. Top then Left .. Right.
Ignored if canvas in linear addressing mode.		*/
	unsigned char temp;
	writeCmd(0x02);
	temp = readData();
	temp |= cSetb2;
	temp |= cSetb1;
	writeData(temp);
}
//[03h][03h][03h][03h][03h][03h][03h][03h][03h][03h][03h][03h][03h][03h][03h][03h]
void RA8876::Interrupt_Active_Low(void)
{
/*  Output to MPU Interrupt active level
	0 : active low.
	1 : active high.						*/
	unsigned char temp;
	writeCmd(0x03);
	temp = readData();
	temp &= cClrb7;
	writeData(temp);
}
void RA8876::Interrupt_Active_High(void)
{
/*  Output to MPU Interrupt active level
	0 : active low.
	1 : active high.						*/
	unsigned char temp;
	writeCmd(0x03);
	temp = readData();
	temp |= cSetb7;
	writeData(temp);
}
void RA8876::ExtInterrupt_Debounce(void)
{
/*  External interrupt input (XPS[0] pin) de-bounce
    0 : without de-bounce
    1 : enable de-bounce (1024 OSC clock)			*/
	unsigned char temp;
	writeCmd(0x03);
	temp = readData();
	temp |= cSetb6;
	writeData(temp);
}
void RA8876::ExtInterrupt_Nodebounce(void)
{
/*  External interrupt input (XPS[0] pin) de-bounce
    0 : without de-bounce
    1 : enable de-bounce (1024 OSC clock)			*/
	unsigned char temp;
	writeCmd(0x03);
	temp = readData();
	temp &= cClrb6;
	writeData(temp);
}
void RA8876::ExtInterrupt_Input_Low_Level_Trigger(void)
{
/* External interrupt input (XPS[0] pin) trigger type
00 : low level trigger
*/
	unsigned char temp;
	writeCmd(0x03);
	temp = readData();
	temp &= cClrb5;
    temp &= cClrb4;
	writeData(temp);
}
void RA8876::ExtInterrupt_Input_High_Level_Trigger(void)
{
/* External interrupt input (XPS[0] pin) trigger type
01 : falling edge trigger
*/
	unsigned char temp;
	writeCmd(0x03);

	temp = readData();
	temp |= cSetb5;
    temp &= cClrb4;
	writeData(temp);
}
void RA8876::ExtInterrupt_Input_Falling_Edge_Trigger(void)
{
/* External interrupt input (XPS[0] pin) trigger type
10 : high level trigger
*/
	unsigned char temp;
	writeCmd(0x03);
	temp = readData();
    temp &= cClrb5;
    temp |= cSetb4;
	writeData(temp);
}
void RA8876::ExtInterrupt_Input_Rising_Edge_Trigger(void)
{
/* External interrupt input (XPS[0] pin) trigger type
11 : rising edge trigger
*/
	unsigned char temp;
	writeCmd(0x03);
	temp = readData();
	temp |= cSetb5;
    temp |= cSetb4;
	writeData(temp);
}
void RA8876::LVDS_Format1(void)//RA8877 only
{
/* FPD-Link Data Format / LVDS Data Format
0 : Format 1 (VESA format) 
1 : Format 2 (JEIDA format) 				*/
	unsigned char temp;
	writeCmd(0x03);
	temp = readData();
    temp &= cClrb3;
	writeData(temp);
}
void RA8876::LVDS_Format2(void)//RA8877 only
{
/* FPD-Link Data Format / LVDS Data Format
0 : Format 1 (VESA format) 
1 : Format 2 (JEIDA format) 				*/
	unsigned char temp;
	writeCmd(0x03);
	temp = readData();
    temp |= cSetb3;
	writeData(temp);
}
void RA8876::Graphic_Mode(void)
{
	unsigned char temp;
	writeCmd(0x03);
	temp = readData();
    temp &= cClrb2;
	writeData(temp);
}
void RA8876::Text_Mode(void)
{
	unsigned char temp;
	writeCmd(0x03);
	temp = readData();
    temp |= cSetb2;
	writeData(temp);
}
void RA8876::Memory_Select_SDRAM(void)
{
	unsigned char temp;

	writeCmd(0x03);
	temp = readData();
    temp &= cClrb1;
    temp &= cClrb0;
	writeData(temp);

}
void RA8876::Memory_Select_Gamma_Table(void)
{
	unsigned char temp;
	writeCmd(0x03);
	temp = readData();
    temp &= cClrb1;
    temp |= cSetb0;
	writeData(temp);  

}
void RA8876::Memory_Select_Graphic_Cursor_RAM(void)
{
	unsigned char temp;
	writeCmd(0x03);
	temp = readData();
    temp |= cSetb1;
    temp &= cClrb0;
	writeData(temp);
}
void RA8876::Memory_Select_Color_Palette_RAM(void)
{
	unsigned char temp;
	writeCmd(0x03);
	temp = readData();
    temp |= cSetb1;
    temp |= cSetb0;
	writeData(temp);
}

//[05h]=========================================================================
//[06h]=========================================================================
//[07h]=========================================================================
//[08h]=========================================================================
//[09h]=========================================================================
//[0Ah]=========================================================================
//[0Bh]=========================================================================

void RA8876::Enable_Resume_Interrupt(void)
{
/*
Wakeup/resume Interrupt Enable
0: Disable.
1: Enable.
*/
	unsigned char temp;
	writeCmd(0x0B);
	temp = readData();
    temp |= cSetb7;
	writeData(temp);
}
void RA8876::Disable_Resume_Interrupt(void)
{
/*
Wakeup/resume Interrupt Enable
0: Disable.
1: Enable.
*/
	unsigned char temp;
	writeCmd(0x0B);
	temp = readData();
    temp &= cClrb7;
	writeData(temp);
}
void RA8876::Enable_ExtInterrupt_Input(void)
{
/*
External Interrupt (PS[0] pin) Enable
0: Disable.
1: Enable.
*/
	unsigned char temp;
	writeCmd(0x0B);
	temp = readData();
    temp |= cSetb6;
	writeData(temp);
}
void RA8876::Disable_ExtInterrupt_Input(void)
{
/*
External Interrupt (PS[0] pin) Enable
0: Disable.
1: Enable.
*/
	unsigned char temp;
	writeCmd(0x0B);
	temp = readData();
    temp &= cClrb6;
	writeData(temp);
}
void RA8876::Enable_I2CM_Interrupt(void)
{
/*
I2C Master Interrupt Enable
0: Disable.
1: Enable.
*/
	unsigned char temp;
	writeCmd(0x0B);
	temp = readData();
    temp |= cSetb5;
	writeData(temp);
}
void RA8876::Disable_I2CM_Interrupt(void)
{
/*
I2C Master Interrupt Enable
0: Disable.
1: Enable.
*/
	unsigned char temp;
	writeCmd(0x0B);
	temp = readData();
    temp &= cClrb5;
	writeData(temp);
}
void RA8876::Enable_Vsync_Interrupt(void)
{
/*
Vsync time base interrupt Enable Bit
0: Disable Interrupt.
1: Enable Interrupt.
*/
	unsigned char temp;
	writeCmd(0x0B);
	temp = readData();
    temp |= cSetb4;
	writeData(temp);
}
void RA8876::Disable_Vsync_Interrupt(void)
{
/*
Vsync time base interrupt Enable Bit
0: Disable Interrupt.
1: Enable Interrupt.
*/
	unsigned char temp;
	writeCmd(0x0B);
	temp = readData();
    temp &= cClrb4;
	writeData(temp);
}
void RA8876::Enable_KeyScan_Interrupt(void)
{
/*
Key Scan Interrupt Enable Bit
0: Disable Key scan interrupt.
1: Enable Key scan interrupt.
*/
	unsigned char temp;
	writeCmd(0x0B);
	temp = readData();
    temp |= cSetb3;
	writeData(temp);
}
void RA8876::Disable_KeyScan_Interrupt(void)
{
/*
Key Scan Interrupt Enable Bit
0: Disable Key scan interrupt.
1: Enable Key scan interrupt.
*/
	unsigned char temp;
	writeCmd(0x0B);
	temp = readData();
    temp &= cClrb3;
	writeData(temp);
}
void RA8876::Enable_DMA_Draw_BTE_Interrupt(void)
{
/*
Serial flash DMA Complete | Draw task finished | BTE Process
Complete etc. Interrupt Enable
0: Disable Interrupt.
1: Enable Interrupt.
*/
	unsigned char temp;
	writeCmd(0x0B);
	temp = readData();
    temp |= cSetb2;
	writeData(temp);
}
void RA8876::Disable_DMA_Draw_BTE_Interrupt(void)
{
/*
Serial flash DMA Complete | Draw task finished | BTE Process
Complete etc. Interrupt Enable
0: Disable Interrupt.
1: Enable Interrupt.
*/
	unsigned char temp;
	writeCmd(0x0B);
	temp = readData();
    temp &= cClrb2;
	writeData(temp);
}
void RA8876::Enable_PWM1_Interrupt(void)
{
/*
PWM timer 1 Interrupt Enable Bit
0: Disable Interrupt.
1: Enable Interrupt.
*/
	unsigned char temp;
	writeCmd(0x0B);
	temp = readData();
    temp |= cSetb1;
	writeData(temp);
}
void RA8876::Disable_PWM1_Interrupt(void)
{
/*
PWM timer 1 Interrupt Enable Bit
0: Disable Interrupt.
1: Enable Interrupt.
*/
	unsigned char temp;
	writeCmd(0x0B);
	temp = readData();
    temp &= cClrb1;
	writeData(temp);
}
void RA8876::Enable_PWM0_Interrupt(void)
{
/*
PWM timer 0 Interrupt Enable Bit
0: Disable Interrupt.
1: Enable Interrupt.
*/
	unsigned char temp;
	writeCmd(0x0B);
	temp = readData();
    temp |= cSetb0;
	writeData(temp);
}
void RA8876::Disable_PWM0_Interrupt(void)
{
/*
PWM timer 0 Interrupt Enable Bit
0: Disable Interrupt.
1: Enable Interrupt.
*/
	unsigned char temp;
	writeCmd(0x0B);
	temp = readData();
    temp &= cClrb0;
	writeData(temp);
}

//[0Ch]=========================================================================
unsigned char RA8876::Read_Interrupt_status(void)
{
/*
[Bit7]Read Function ..Resume Interrupt Status
0: No Resume interrupt happens.
1: Resume interrupt happens.
[Bit6]Read Function .. PS[0] pin Interrupt Status
0: No PS[0] pin interrupt happens.
1: PS[0] pin interrupt happens.
[Bit5]Read Function .. I2C master Interrupt Status
0: No I2C master interrupt happens.
1: I2C master interrupt happens.
[Bit4]Read Function .. Vsync Interrupt Status
0: No interrupt happens.
1: interrupt happens.
[Bit3]Read Function ..Key Scan Interrupt Status
0: No Key Scan interrupt happens.
1: Key Scan interrupt happens.
[Bit2]Read Function..Interrupt Status
0: No interrupt happens.
1: interrupt happens.
[Bit1]Read Function..Interrupt Status
0: No interrupt happens.
1: interrupt happens.
[Bit0]Read Function..Interrupt Status
0: No interrupt happens.
1: interrupt happens.
*/
	unsigned char temp;
	writeCmd(0x0C);
	temp = readData();
    return temp;
}

void RA8876::Check_Vsync_finished(void)
{
/*[Bit4]
Vsync Time base interrupt flag
Read Function .. Vsync Interrupt Status
0: No interrupt happens.
1: interrupt happens.
*/
	unsigned char temp;
	writeCmd(0x0C);
	writeData(0x10);
	do{	   
		temp=readData();
	
	}while((temp&0x10) == 0x00); 

}

void RA8876::Clear_Resume_Interrupt_Flag(void)
{
/*
Resume Interrupt flag
Write Function .. Resume Interrupt Clear Bit
0: No operation.
1: Clear Resume interrupt.
*/
	unsigned char temp;
	writeCmd(0x0C);
	temp = readData();
    temp |= cSetb7;
	writeData(temp);
}
void RA8876::Clear_ExtInterrupt_Input_Flag(void)
{
/*
External Interrupt (PS[0] pin) flag
Write Function .. PS[0] pin edge Interrupt Clear Bit
0: No operation.
1: Clear the PS[0] pin edge interrupt.
*/
	unsigned char temp;
	writeCmd(0x0C);
	temp = readData();
    temp |= cSetb6;
	writeData(temp);
}
void RA8876::Clear_I2CM_Interrupt_Flag(void)
{
/*
I2C master Interrupt flag
Write Function.. I2C master Interrupt Clear Bit
0: No operation.
1: Clear the I2C master interrupt.
*/
	unsigned char temp;
	writeCmd(0x0C);
	temp = readData();
    temp |= cSetb5;
	writeData(temp);
}
void RA8876::Clear_Vsync_Interrupt_Flag(void)
{
/*
Vsync Time base interrupt flag
Write Function ..Vsync Interrupt Clear Bit
0: No operation.
1: Clear the interrupt.
*/
	unsigned char temp;
	writeCmd(0x0C);
	temp = readData();
    temp |= cSetb4;
	writeData(temp);
}
void RA8876::Clear_KeyScan_Interrupt_Flag(void)
{
/*
Key Scan Interrupt flag
Write Function..Key Scan Interrupt Clear Bit
0: No operation.
1: Clear the Key Scan interrupt.
*/
	unsigned char temp;
	writeCmd(0x0C);
	temp = readData();
    temp |= cSetb3;
	writeData(temp);
}
void RA8876::Clear_DMA_Draw_BTE_Interrupt_Flag(void)
{
/*
Serial flash DMA Complete | Draw task finished | BTE
Process Complete etc. Interrupt flag
Write Function.. Interrupt Clear Bit
0: No operation.
1: Clear interrupt.
*/
	unsigned char temp;
	writeCmd(0x0C);
	temp = readData();
    temp |= cSetb2;
	writeData(temp);
}
void RA8876::Clear_PWM1_Interrupt_Flag(void)
{
/*
PWM 1 timer Interrupt flag
Write Function..Interrupt Clear Bit
0: No operation.
1: Clear interrupt.
*/
	unsigned char temp;
	writeCmd(0x0C);
	temp = readData();
    temp |= cSetb1;
	writeData(temp);
}
void RA8876::Clear_PWM0_Interrupt_Flag(void)
{
/*
PWM 0 timer Interrupt flag
Write Function..Interrupt Clear Bit
0: No operation.
1: Clear interrupt.
*/
	unsigned char temp;
	writeCmd(0x0C);
	temp = readData();
    temp |= cSetb0;
	writeData(temp);
}
//[0Dh]=========================================================================
void RA8876::XnINTR_Mask_Resume_Interrupt_Flag(void)
{
/*
Mask Resume Interrupt Flag
0: Enable.
1: Mask.
*/
	unsigned char temp;
	writeCmd(0x0D);
	temp = readData();
    temp |= cSetb7;
	writeData(temp);
}
void RA8876::XnINTR_Mask_ExtInterrupt_Input_Flag(void)
{
/*
Mask External Interrupt (PS[0] pin) Flag
0: Enable.
1: Mask.
*/
	unsigned char temp;
	writeCmd(0x0D);
	temp = readData();
    temp |= cSetb6;
	writeData(temp);
}
void RA8876::XnINTR_Mask_I2CM_Interrupt_Flag(void)
{
/*
Mask I2C Master Interrupt Flag
0: Enable.
1: Mask.
*/
	unsigned char temp;
	writeCmd(0x0D);
	temp = readData();
    temp |= cSetb5;
	writeData(temp);
}
void RA8876::XnINTR_Mask_Vsync_Interrupt_Flag(void)
{
/*
Mask Vsync time base interrupt Flag
0: Enable.
1: Mask.
*/
	unsigned char temp;
	writeCmd(0x0D);
	temp = readData();
    temp |= cSetb4;
	writeData(temp);
}
void RA8876::XnINTR_Mask_KeyScan_Interrupt_Flag(void)
{
/*
Mask Key Scan Interrupt Flag
0: Enable.
1: Mask.
*/
	unsigned char temp;
	writeCmd(0x0D);
	temp = readData();
    temp |= cSetb3;
	writeData(temp);
}
void RA8876::XnINTR_Mask_DMA_Draw_BTE_Interrupt_Flag(void)
{
/*
Mask Serial flash DMA Complete | Draw task finished | BTE
Process Complete etc. Interrupt Flag
0: Enable.
1: Mask.
*/
	unsigned char temp;
	writeCmd(0x0D);
	temp = readData();
    temp |= cSetb2;
	writeData(temp);
}
void RA8876::XnINTR_Mask_PWM1_Interrupt_Flag(void)
{
/*
Mask PWM timer 1 Interrupt Flag
0: Enable.
1: Mask.
*/
	unsigned char temp;
	writeCmd(0x0D);
	temp = readData();
    temp |= cSetb1;
	writeData(temp);
}
void RA8876::XnINTR_Mask_PWM0_Interrupt_Flag(void)
{
/*
Mask PWM timer 0 Interrupt Flag
0: Enable.
1: Mask.
*/
	unsigned char temp;
	writeCmd(0x0D);
	temp = readData();
    temp |= cSetb0;
	writeData(temp);
}

void RA8876::XnINTR_Unmask_Resume_Interrupt_Flag(void)
{
/*
Mask Resume Interrupt Flag
0: Enable.
1: Mask.
*/
	unsigned char temp;
	writeCmd(0x0D);
	temp = readData();
    temp &= cClrb7;
	writeData(temp);
}
void RA8876::XnINTR_Unmask_ExtInterrupt_Input_Flag(void)
{
/*
Mask External Interrupt (PS[0] pin) Flag
0: Enable.
1: Mask.
*/
	unsigned char temp;
	writeCmd(0x0D);
	temp = readData();
    temp &= cClrb6;
	writeData(temp);
}
void RA8876::XnINTR_Unmask_I2CM_Interrupt_Flag(void)
{
/*
Mask I2C Master Interrupt Flag
0: Enable.
1: Mask.
*/
	unsigned char temp;
	writeCmd(0x0D);
	temp = readData();
    temp &= cClrb5;
	writeData(temp);
}
void RA8876::XnINTR_Unmask_Vsync_Interrupt_Flag(void)
{
/*
Mask Vsync time base interrupt Flag
0: Enable.
1: Mask.
*/
	unsigned char temp;
	writeCmd(0x0D);
	temp = readData();
    temp &= cClrb4;
	writeData(temp);
}
void RA8876::XnINTR_Unmask_KeyScan_Interrupt_Flag(void)
{
/*
Mask Key Scan Interrupt Flag
0: Enable.
1: Mask.
*/
	unsigned char temp;
	writeCmd(0x0D);
	temp = readData();
    temp &= cClrb3;
	writeData(temp);
}
void RA8876::XnINTR_Unmask_DMA_Draw_BTE_Interrupt_Flag(void)
{
/*
Mask Serial flash DMA Complete | Draw task finished | BTE
Process Complete etc. Interrupt Flag
0: Enable.
1: Mask.
*/
	unsigned char temp;
	writeCmd(0x0D);
	temp = readData();
    temp &= cClrb2;
	writeData(temp);
}
void RA8876::XnINTR_Unmask_PWM1_Interrupt_Flag(void)
{
/*
Mask PWM timer 1 Interrupt Flag
0: Enable.
1: Mask.
*/
	unsigned char temp;
	writeCmd(0x0D);
	temp = readData();
    temp &= cClrb1;
	writeData(temp);
}
void RA8876::XnINTR_Unmask_PWM0_Interrupt_Flag(void)
{
/*
Mask PWM timer 0 Interrupt Flag
0: Enable.
1: Mask.
*/
	unsigned char temp;
	writeCmd(0x0D);
	temp = readData();
    temp &= cClrb0;
	writeData(temp);
}

//[0Eh]=========================================================================
void RA8876::Enable_GPIOF_PullUp(void)
{
/*
GPIO_F[7:0] Pull-Up Enable (XPDAT[23:19, 15:13])
0: Pull-Up Disable
1: Pull-Up Enable
*/
	unsigned char temp;
	writeCmd(0x0E);
	temp = readData();
    temp |= cSetb5;
	writeData(temp);
}
void RA8876::Enable_GPIOE_PullUp(void)
{
/*
GPIO_E[7:0] Pull-Up Enable (XPDAT[12:10, 7:3])
0: Pull-Up Disable
1: Pull-Up Enable
*/
	unsigned char temp;
	writeCmd(0x0E);
	temp = readData();
    temp |= cSetb4;
	writeData(temp);
}
void RA8876::Enable_GPIOD_PullUp(void)
{
/*
GPIO_D[7:0] Pull-Up Enable (XPDAT[18, 2, 17, 16, 9, 8, 1,0])
0: Pull-Up Disable
1: Pull-Up Enable
*/
	unsigned char temp;
	writeCmd(0x0E);
	temp = readData();
    temp |= cSetb3;
	writeData(temp);
}
void RA8876::Enable_GPIOC_PullUp(void)
{
/*
GPIO_C[6:0] Pull-Up Enable (XSDA, XSCL, XnSFCS1,
XnSFCS0, XMISO, XMOSI , XSCK)
0: Pull-Up Disable
1: Pull-Up Enable
*/
	unsigned char temp;
	writeCmd(0x0E);
	temp = readData();
    temp |= cSetb2;
	writeData(temp);
}
void RA8876::Enable_XDB15_8_PullUp(void)
{
/*
XDB[15:8] Pull-Up Enable
0: Pull-Up Disable
1: Pull-Up Enable
*/
	unsigned char temp;
	writeCmd(0x0E);
	temp = readData();
    temp |= cSetb1;
	writeData(temp);
}
void RA8876::Enable_XDB7_0_PullUp(void)
{
/*
XDB[7:0] Pull-Up Enable
0: Pull-Up Disable
1: Pull-Up Enable
*/
	unsigned char temp;
	writeCmd(0x0E);
	temp = readData();
    temp |= cSetb0;
	writeData(temp);
}
void RA8876::Disable_GPIOF_PullUp(void)
{
/*
GPIO_F[7:0] Pull-Up Enable (XPDAT[23:19, 15:13])
0: Pull-Up Disable
1: Pull-Up Enable
*/
	unsigned char temp;
	writeCmd(0x0E);
	temp = readData();
    temp &= cClrb5;
	writeData(temp);
}
void RA8876::Disable_GPIOE_PullUp(void)
{
/*
GPIO_E[7:0] Pull-Up Enable (XPDAT[12:10, 7:3])
0: Pull-Up Disable
1: Pull-Up Enable
*/
	unsigned char temp;
	writeCmd(0x0E);
	temp = readData();
    temp &= cClrb4;
	writeData(temp);
}
void RA8876::Disable_GPIOD_PullUp(void)
{
/*
GPIO_D[7:0] Pull-Up Enable (XPDAT[18, 2, 17, 16, 9, 8, 1,0])
0: Pull-Up Disable
1: Pull-Up Enable
*/
	unsigned char temp;
	writeCmd(0x0E);
	temp = readData();
    temp &= cClrb3;
	writeData(temp);
}
void RA8876::Disable_GPIOC_PullUp(void)
{
/*
GPIO_C[6:0] Pull-Up Enable (XSDA, XSCL, XnSFCS1,
XnSFCS0, XMISO, XMOSI , XSCK)
0: Pull-Up Disable
1: Pull-Up Enable
*/
	unsigned char temp;
	writeCmd(0x0E);
	temp = readData();
    temp &= cClrb2;
	writeData(temp);
}
void RA8876::Disable_XDB15_8_PullUp(void)
{
/*
XDB[15:8] Pull-Up Enable
0: Pull-Up Disable
1: Pull-Up Enable
*/
	unsigned char temp;
	writeCmd(0x0E);
	temp = readData();
    temp &= cClrb1;
	writeData(temp);
}
void RA8876::Disable_XDB7_0_PullUp(void)
{
/*
XDB[7:0] Pull-Up Enable
0: Pull-Up Disable
1: Pull-Up Enable
*/
	unsigned char temp;
	writeCmd(0x0E);
	temp = readData();
    temp &= cClrb0;
	writeData(temp);
}
//[0Fh]=========================================================================
void RA8876::XPDAT18_Set_GPIO_D7(void)
{
/*
XPDAT[18] �V not scan function select
0: GPIO-D7
1: KOUT[4]
*/
	unsigned char temp;
	writeCmd(0x0F);
	temp = readData();
    temp &= cClrb7;
	writeData(temp);
}
void RA8876::XPDAT18_Set_KOUT4(void)
{
/*
XPDAT[18] �V not scan function select
0: GPIO-D7
1: KOUT[4]
*/
	unsigned char temp;
	writeCmd(0x0F);
	temp = readData();
    temp |= cSetb7;
	writeData(temp);
}
void RA8876::XPDAT17_Set_GPIO_D5(void)
{
/*
XPDAT[17] �V not scan function select
0: GPIO-D5
1: KOUT[2]
*/
	unsigned char temp;
	writeCmd(0x0F);
	temp = readData();
    temp &= cClrb6;
	writeData(temp);
}
void RA8876::XPDAT17_Set_KOUT2(void)
{
/*
XPDAT[17] �V not scan function select
0: GPIO-D5
1: KOUT[2]
*/
	unsigned char temp;
	writeCmd(0x0F);
	temp = readData();
    temp |= cSetb6;
	writeData(temp);
}
void RA8876::XPDAT16_Set_GPIO_D4(void)
{
/*
XPDAT[16] �V not scan function select
0: GPIO-D4
1: KOUT[1]
*/
	unsigned char temp;
	writeCmd(0x0F);
	temp = readData();
    temp &= cClrb5;
	writeData(temp);
}
void RA8876::XPDAT16_Set_KOUT1(void)
{
/*
XPDAT[16] �V not scan function select
0: GPIO-D4
1: KOUT[1]
*/
	unsigned char temp;
	writeCmd(0x0F);
	temp = readData();
    temp |= cSetb5;
	writeData(temp);
}
void RA8876::XPDAT9_Set_GPIO_D3(void)
{
/*
XPDAT[9] �V not scan function select
0: GPIO-D3
1: KOUT[3]
*/
	unsigned char temp;
	writeCmd(0x0F);
	temp = readData();
    temp &= cClrb4;
	writeData(temp);
}
void RA8876::XPDAT9_Set_KOUT3(void)
{
/*
XPDAT[9] �V not scan function select
0: GPIO-D3
1: KOUT[3]
*/
	unsigned char temp;
	writeCmd(0x0F);
	temp = readData();
    temp |= cSetb4;
	writeData(temp);
}
void RA8876::XPDAT8_Set_GPIO_D2(void)
{
/*
XPDAT[8] �V not scan function select
0: GPIO-D2
1: KIN[3]
*/
	unsigned char temp;
	writeCmd(0x0F);
	temp = readData();
    temp &= cClrb3;
	writeData(temp);
}
void RA8876::XPDAT8_Set_KIN3(void)
{
/*
XPDAT[8] �V not scan function select
0: GPIO-D2
1: KIN[3]
*/
	unsigned char temp;
	writeCmd(0x0F);
	temp = readData();
    temp |= cSetb3;
	writeData(temp);
}
void RA8876::XPDAT2_Set_GPIO_D6(void)
{
/*
XPDAT[2] �V not scan function select
0: GPIO-D6
1: KIN[4]
*/
	unsigned char temp;
	writeCmd(0x0F);
	temp = readData();
    temp &= cClrb2;
	writeData(temp);
}
void RA8876::XPDAT2_Set_KIN4(void)
{
/*
XPDAT[2] �V not scan function select
0: GPIO-D6
1: KIN[4]
*/
	unsigned char temp;
	writeCmd(0x0F);
	temp = readData();
    temp |= cSetb2;
	writeData(temp);
}
void RA8876::XPDAT1_Set_GPIO_D1(void)
{
/*
XPDAT[1] �V not scan function select
0: GPIO-D1
1: KIN[2]
*/
	unsigned char temp;
	writeCmd(0x0F);
	temp = readData();
    temp &= cClrb1;
	writeData(temp);
}
void RA8876::XPDAT1_Set_KIN2(void)
{
/*
XPDAT[1] �V not scan function select
0: GPIO-D1
1: KIN[2]
*/
	unsigned char temp;
	writeCmd(0x0F);
	temp = readData();
    temp |= cSetb1;
	writeData(temp);
}
void RA8876::XPDAT0_Set_GPIO_D0(void)
{
/*
XPDAT[0] �V not scan function select
0: GPIO-D0
1: KIN[1]
*/
	unsigned char temp;
	writeCmd(0x0F);
	temp = readData();
    temp &= cClrb0;
	writeData(temp);
}
void RA8876::XPDAT0_Set_KIN1(void)
{
/*
XPDAT[0] �V not scan function select
0: GPIO-D0
1: KIN[1]
*/
	unsigned char temp;
	writeCmd(0x0F);
	temp = readData();
    temp |= cSetb0;
	writeData(temp);
}

//[10h]=========================================================================
void RA8876::Enable_PIP1(void)
{
/*
PIP 1 window Enable/Disable
0 : PIP 1 window disable.
1 : PIP 1 window enable
PIP 1 window always on top of PIP 2 window.
*/
	unsigned char temp;
	writeCmd(0x10);
	temp = readData();
    temp |= cSetb7;
	writeData(temp);
}
void RA8876::Disable_PIP1(void)
{
/*
PIP 1 window Enable/Disable
0 : PIP 1 window disable.
1 : PIP 1 window enable
PIP 1 window always on top of PIP 2 window.
*/
	unsigned char temp;
	writeCmd(0x10);
	temp = readData();
    temp &= cClrb7;
	writeData(temp);
}
void RA8876::Enable_PIP2(void)
{
/*
PIP 2 window Enable/Disable
0 : PIP 2 window disable.
1 : PIP 2 window enable
PIP 1 window always on top of PIP 2 window.
*/
	unsigned char temp;
	writeCmd(0x10);
	temp = readData();
    temp |= cSetb6;
	writeData(temp);
}
void RA8876::Disable_PIP2(void)
{
/*
PIP 2 window Enable/Disable
0 : PIP 2 window disable.
1 : PIP 2 window enable
PIP 1 window always on top of PIP 2 window.
*/
	unsigned char temp;
	writeCmd(0x10);
	temp = readData();
    temp &= cClrb6;
	writeData(temp);
}
void RA8876::Select_PIP1_Parameter(void)
{
/*
0: To configure PIP 1��s parameters.
1: To configure PIP 2��s parameters..
*/
	unsigned char temp;
	writeCmd(0x10);
	temp = readData();
    temp &= cClrb4;
	writeData(temp);
}
void RA8876::Select_PIP2_Parameter(void)
{
/*
0: To configure PIP 1��s parameters.
1: To configure PIP 2��s parameters..
*/
	unsigned char temp;
	writeCmd(0x10);
	temp = readData();
    temp |= cSetb4;
	writeData(temp);
}
void RA8876::Select_Main_Window_8bpp(void)
{
/*
Main Window Color Depth Setting
00b: 8-bpp generic TFT, i.e. 256 colors.
01b: 16-bpp generic TFT, i.e. 65K colors.
1xb: 24-bpp generic TFT, i.e. 1.67M colors.
*/
	unsigned char temp;
	writeCmd(0x10);
	temp = readData();
    temp &= cClrb3;
    temp &= cClrb2;
	writeData(temp);
}
void RA8876::Select_Main_Window_16bpp(void)
{
/*
Main Window Color Depth Setting
00b: 8-bpp generic TFT, i.e. 256 colors.
01b: 16-bpp generic TFT, i.e. 65K colors.
1xb: 24-bpp generic TFT, i.e. 1.67M colors.
*/
	unsigned char temp;
	writeCmd(0x10);
	temp = readData();
    temp &= cClrb3;
    temp |= cSetb2;
	writeData(temp);
}
void RA8876::Select_Main_Window_24bpp(void)
{
/*
Main Window Color Depth Setting
00b: 8-bpp generic TFT, i.e. 256 colors.
01b: 16-bpp generic TFT, i.e. 65K colors.
1xb: 24-bpp generic TFT, i.e. 1.67M colors.
*/
	unsigned char temp;
	writeCmd(0x10);
	temp = readData();
    temp |= cSetb3;
	writeData(temp);
}

void RA8876::Select_LCD_Sync_Mode(void)
{
/*
To Control panel's synchronous signals
0: Sync Mode: Enable XVSYNC, XHSYNC, XDE
1: DE Mode: Only XDE enable, XVSYNC & XHSYNC in idle state
*/
	unsigned char temp;
	writeCmd(0x10);
	temp = readData();
    temp &= cClrb0;
	writeData(temp);
}
void RA8876::Select_LCD_DE_Mode(void)
{
/*
To Control panel's synchronous signals
0: Sync Mode: Enable XVSYNC, XHSYNC, XDE
1: DE Mode: Only XDE enable, XVSYNC & XHSYNC in idle state
*/
	unsigned char temp;
	writeCmd(0x10);
	temp = readData();
    temp |= cSetb0;
	writeData(temp);
}

//[11h]=========================================================================
void RA8876::Select_PIP1_Window_8bpp(void)
{
/*
PIP 1 Window Color Depth Setting
00b: 8-bpp generic TFT, i.e. 256 colors.
01b: 16-bpp generic TFT, i.e. 65K colors.
1xb: 24-bpp generic TFT, i.e. 1.67M colors.
*/
	unsigned char temp;
	writeCmd(0x11);
	temp = readData();
    temp &= cClrb3;
    temp &= cClrb2;
	writeData(temp);
}
void RA8876::Select_PIP1_Window_16bpp(void)
{
/*
PIP 1 Window Color Depth Setting
00b: 8-bpp generic TFT, i.e. 256 colors.
01b: 16-bpp generic TFT, i.e. 65K colors.
1xb: 24-bpp generic TFT, i.e. 1.67M colors.
*/
	unsigned char temp;
	writeCmd(0x11);
	temp = readData();
    temp &= cClrb3;
    temp |= cSetb2;
	writeData(temp);
}
void RA8876::Select_PIP1_Window_24bpp(void)
{
/*
PIP 1 Window Color Depth Setting
00b: 8-bpp generic TFT, i.e. 256 colors.
01b: 16-bpp generic TFT, i.e. 65K colors.
1xb: 24-bpp generic TFT, i.e. 1.67M colors.
*/
	unsigned char temp;
	writeCmd(0x11);
	temp = readData();
    temp |= cSetb3;
//    temp &= cClrb2;
	writeData(temp);
}
void RA8876::Select_PIP2_Window_8bpp(void)
{
/*
PIP 2 Window Color Depth Setting
00b: 8-bpp generic TFT, i.e. 256 colors.
01b: 16-bpp generic TFT, i.e. 65K colors.
1xb: 24-bpp generic TFT, i.e. 1.67M colors.
*/
	unsigned char temp;
	writeCmd(0x11);
	temp = readData();
    temp &= cClrb1;
    temp &= cClrb0;
	writeData(temp);
}
void RA8876::Select_PIP2_Window_16bpp(void)
{
/*
PIP 2 Window Color Depth Setting
00b: 8-bpp generic TFT, i.e. 256 colors.
01b: 16-bpp generic TFT, i.e. 65K colors.
1xb: 24-bpp generic TFT, i.e. 1.67M colors.
*/
	unsigned char temp;
	writeCmd(0x11);
	temp = readData();
    temp &= cClrb1;
    temp |= cSetb0;
	writeData(temp);
}
void RA8876::Select_PIP2_Window_24bpp(void)
{
/*
PIP 2 Window Color Depth Setting
00b: 8-bpp generic TFT, i.e. 256 colors.
01b: 16-bpp generic TFT, i.e. 65K colors.
1xb: 24-bpp generic TFT, i.e. 1.67M colors.
*/
	unsigned char temp;
	writeCmd(0x11);
	temp = readData();
    temp |= cSetb1;
//    temp &= cClrb0;
	writeData(temp);
}

//[12h]=========================================================================
void RA8876::PCLK_Rising(void)	 
{
/*
PCLK Inversion
0: PDAT, DE, HSYNC etc. Drive(/ change) at PCLK falling edge.
1: PDAT, DE, HSYNC etc. Drive(/ change) at PCLK rising edge.
*/
	unsigned char temp;
	writeCmd(0x12);
	temp = readData();
    temp &= cClrb7;
	writeData(temp);
}
void RA8876::PCLK_Falling(void)
{
/*
PCLK Inversion
0: PDAT, DE, HSYNC etc. Drive(/ change) at PCLK falling edge.
1: PDAT, DE, HSYNC etc. Drive(/ change) at PCLK rising edge.
*/
	unsigned char temp;
	writeCmd(0x12);
	temp = readData();
    temp |= cSetb7;
	writeData(temp);
}
void RA8876::Display_ON(void)
{
/*	
Display ON/OFF
0b: Display Off.
1b: Display On.
*/
	unsigned char temp;
	
	writeCmd(0x12);
	temp = readData();
	temp |= cSetb6;
	writeData(temp);
}

void RA8876::Display_OFF(void)
{
/*	
Display ON/OFF
0b: Display Off.
1b: Display On.
*/
	unsigned char temp;
	
	writeCmd(0x12);
	temp = readData();
	temp &= cClrb6;
	writeData(temp);
}
void RA8876::Color_Bar_ON(void)
{
/*	
Display Test Color Bar
0b: Disable.
1b: Enable.
*/
	unsigned char temp;
	
	writeCmd(0x12);
	temp = readData();
	temp |= cSetb5;
	writeData(temp);
}
void RA8876::Color_Bar_OFF(void)
{
/*	
Display Test Color Bar
0b: Disable.
1b: Enable.
*/
	unsigned char temp;
	
	writeCmd(0x12);
	temp = readData();
	temp &= cClrb5;
	writeData(temp);
}


void RA8876::VSCAN_T_to_B(void)
{
/*	
Vertical Scan direction
0 : From Top to Bottom
1 : From bottom to Top
PIP window will be disabled when VDIR set as 1.
*/
	unsigned char temp;
	
	writeCmd(0x12);
	temp = readData();
	temp &= cClrb3;
	writeData(temp);
}
void RA8876::VSCAN_B_to_T(void)
{
/*	
Vertical Scan direction
0 : From Top to Bottom
1 : From bottom to Top
PIP window will be disabled when VDIR set as 1.
*/
	unsigned char temp;
	
	writeCmd(0x12);
	temp = readData();
	temp |= cSetb3;
	writeData(temp);
}
void RA8876::PDATA_Set_RGB(void)
{
/*parallel PDATA[23:0] Output Sequence
000b : RGB
001b : RBG
010b : GRB
011b : GBR
100b : BRG
101b : BGR
110b : Gray			
*/
	unsigned char temp;
	
	writeCmd(0x12);
	temp = readData();
    temp &=0xf8;
	writeData(temp);
}
void RA8876::PDATA_Set_RBG(void)
{
/*parallel PDATA[23:0] Output Sequence
000b : RGB
001b : RBG
010b : GRB
011b : GBR
100b : BRG
101b : BGR
110b : Gray			
*/
	unsigned char temp;
	
	writeCmd(0x12);
	temp = readData();
	temp &=0xf8;
    temp |= cSetb0;
	writeData(temp);
}
void RA8876::PDATA_Set_GRB(void)
{
/*parallel PDATA[23:0] Output Sequence
000b : RGB
001b : RBG
010b : GRB
011b : GBR
100b : BRG
101b : BGR
110b : Gray			
*/
	unsigned char temp;
	
	writeCmd(0x12);
	temp = readData();
	temp &=0xf8;
    temp |= cSetb1;
	writeData(temp);
}
void RA8876::PDATA_Set_GBR(void)
{
/*parallel PDATA[23:0] Output Sequence
000b : RGB
001b : RBG
010b : GRB
011b : GBR
100b : BRG
101b : BGR
110b : Gray			
*/
	unsigned char temp;
	
	writeCmd(0x12);
	temp = readData();
	temp &=0xf8;
    temp |= cSetb1;
    temp |= cSetb0;
	writeData(temp);
}
void RA8876::PDATA_Set_BRG(void)
{
/*parallel PDATA[23:0] Output Sequence
000b : RGB
001b : RBG
010b : GRB
011b : GBR
100b : BRG
101b : BGR
110b : Gray			
*/
	unsigned char temp;
	
	writeCmd(0x12);
	temp = readData();
	temp &=0xf8;
	temp |= cSetb2;
	writeData(temp);
}
void RA8876::PDATA_Set_BGR(void)
{
/*parallel PDATA[23:0] Output Sequence
000b : RGB
001b : RBG
010b : GRB
011b : GBR
100b : BRG
101b : BGR
110b : Gray			
*/
	unsigned char temp;
	
	writeCmd(0x12);
	temp = readData();
	temp &=0xf8;
	temp |= cSetb2;
    temp |= cSetb0;
	writeData(temp);
}
void RA8876::PDATA_Set_Gray(void)
{
/*parallel PDATA[23:0] Output Sequence
000b : RGB
001b : RBG
010b : GRB
011b : GBR
100b : BRG
101b : BGR
110b : Gray			
*/
	unsigned char temp;
	
	writeCmd(0x12);
	temp = readData();
	temp &=0xf8;
	temp |= cSetb2;
    temp |= cSetb1;
	writeData(temp);
}

void RA8876::PDATA_IDLE_STATE(void)
{
/*
111b : XPDAT pins send out PDAT idle state (all 0 or 1, black or white color).
*/
	unsigned char temp;
	
	writeCmd(0x12);
	temp = readData();
	temp |=0x07;
	writeData(temp);
}


//[13h]=========================================================================
void RA8876::HSYNC_Low_Active(void)
{
/*	
HSYNC Polarity
0 : Low active.
1 : High active.
*/
	unsigned char temp;
	
	writeCmd(0x13);
	temp = readData();
    temp &= cClrb7;
	writeData(temp);
}
void RA8876::HSYNC_High_Active(void)
{
/*	
HSYNC Polarity
0 : Low active.
1 : High active.
*/
	unsigned char temp;
	
	writeCmd(0x13);
	temp = readData();
	temp |= cSetb7;
	writeData(temp);
}
void RA8876::VSYNC_Low_Active(void)
{
/*	
VSYNC Polarity
0 : Low active.
1 : High active.
*/
	unsigned char temp;
	
	writeCmd(0x13);
	temp = readData();
    temp &= cClrb6;
	writeData(temp);
}
void RA8876::VSYNC_High_Active(void)
{
/*	
VSYNC Polarity
0 : Low active.
1 : High active.
*/
	unsigned char temp;
	
	writeCmd(0x13);
	temp = readData();
	temp |= cSetb6;
	writeData(temp);
}
void RA8876::DE_Low_Active(void)
{
/*	
DE Polarity
0 : High active.
1 : Low active.
*/
	unsigned char temp;
	
	writeCmd(0x13);
	temp = readData();
    temp |= cSetb5;
	writeData(temp);
}
void RA8876::DE_High_Active(void)
{
/*	
DE Polarity
0 : High active.
1 : Low active.
*/
	unsigned char temp;
	
	writeCmd(0x13);
	temp = readData();
	temp &= cClrb5;
	writeData(temp);
}
void RA8876::Idle_DE_Low(void)
{
/*	
DE IDLE STATE(When STANDBY or DISPLAY OFF )
0 : Pin ��DE�� output is low.
1 : Pin ��DE�� output is high.
*/
	unsigned char temp;
	
	writeCmd(0x13);
	temp = readData();
    temp &= cClrb4;
	writeData(temp);
}
void RA8876::Idle_DE_High(void)
{
/*	
DE IDLE STATE(When STANDBY or DISPLAY OFF )
0 : Pin ��DE�� output is low.
1 : Pin ��DE�� output is high.
*/
	unsigned char temp;
	
	writeCmd(0x13);
	temp = readData();
	temp |= cSetb4;
	writeData(temp);
}
void RA8876::Idle_PCLK_Low(void)
{
/*	
PCLK IDLE STATE(When STANDBY or DISPLAY OFF )
0 : Pin ��PCLK�� output is low.
1 : Pin ��PCLK�� output is high.
*/
	unsigned char temp;
	
	writeCmd(0x13);
	temp = readData();
    temp &= cClrb3;
	writeData(temp);
}
void RA8876::Idle_PCLK_High(void)
{
/*	
PCLK IDLE STATE(When STANDBY or DISPLAY OFF )
0 : Pin ��PCLK�� output is low.
1 : Pin ��PCLK�� output is high.
*/
	unsigned char temp;
	
	writeCmd(0x13);
	temp = readData();
	temp |= cSetb3;
	writeData(temp);
}
void RA8876::Idle_PDAT_Low(void)
{
/*	
PDAT IDLE STATE(When STANDBY or DISPLAY OFF )
0 : Pins ��PDAT[23:0]�� output is low.
1 : Pins ��PCLK[23:0]�� output is high.
*/
	unsigned char temp;
	
	writeCmd(0x13);
	temp = readData();
    temp &= cClrb2;
	writeData(temp);
}
void RA8876::Idle_PDAT_High(void)
{
/*	
PDAT IDLE STATE(When STANDBY or DISPLAY OFF )
0 : Pins ��PDAT[23:0]�� output is low.
1 : Pins ��PCLK[23:0]�� output is high.
*/
	unsigned char temp;
	
	writeCmd(0x13);
	temp = readData();
	temp |= cSetb2;
	writeData(temp);
}
void RA8876::Idle_HSYNC_Low(void)
{
/*	
HSYNC IDLE STATE(When STANDBY or DISPLAY OFF )
0 : Pin ��HSYNC�� output is low.
1 : Pin ��HSYNC�� output is high.
*/
	unsigned char temp;
	
	writeCmd(0x13);
	temp = readData();
    temp &= cClrb1;
	writeData(temp);
}
void RA8876::Idle_HSYNC_High(void)
{
/*	
HSYNC IDLE STATE(When STANDBY or DISPLAY OFF )
0 : Pin ��HSYNC�� output is low.
1 : Pin ��HSYNC�� output is high.
*/
	unsigned char temp;
	
	writeCmd(0x13);
	temp = readData();
	temp |= cSetb1;
	writeData(temp);
}
void RA8876::Idle_VSYNC_Low(void)
{
/*	
VSYNC IDLE STATE(When STANDBY or DISPLAY OFF )
0 : Pin ��VSYNC�� output is low.
1 : Pin ��VSYNC�� output is high.
*/
	unsigned char temp;
	
	writeCmd(0x13);
	temp = readData();
    temp &= cClrb0;
	writeData(temp);
}
void RA8876::Idle_VSYNC_High(void)
{
/*	
VSYNC IDLE STATE(When STANDBY or DISPLAY OFF )
0 : Pin ��VSYNC�� output is low.
1 : Pin ��VSYNC�� output is high.
*/
	unsigned char temp;
	
	writeCmd(0x13);
	temp = readData();
	temp |= cSetb0;
	writeData(temp);
}

//[14h][15h][1Ah][1Bh]=========================================================================
void RA8876::LCD_HorizontalWidth_VerticalHeight(unsigned short WX,unsigned short HY)
{
/*
[14h] Horizontal Display Width Setting Bit[7:0]
[15h] Horizontal Display Width Fine Tuning (HDWFT) [3:0]
The register specifies the LCD panel horizontal display width in
the unit of 8 pixels resolution.
Horizontal display width(pixels) = (HDWR + 1) * 8 + HDWFTR

[1Ah] Vertical Display Height Bit[7:0]
Vertical Display Height(Line) = VDHR + 1
[1Bh] Vertical Display Height Bit[10:8]
Vertical Display Height(Line) = VDHR + 1
*/
	unsigned char temp;
	if(WX<8)
    {
	writeCmd(0x14);
	writeData(0x00);
    
	writeCmd(0x15);
	writeData(WX);
    
    temp=HY-1;
	writeCmd(0x1A);
	writeData(temp);
	    
	temp=(HY-1)>>8;
	writeCmd(0x1B);
	writeData(temp);
	}
	else
	{
    temp=(WX/8)-1;
	writeCmd(0x14);
	writeData(temp);
    
    temp=WX%8;
	writeCmd(0x15);
	writeData(temp);
    
    temp=HY-1;
	writeCmd(0x1A);
	writeData(temp);
	    
	temp=(HY-1)>>8;
	writeCmd(0x1B);
	writeData(temp);
	}
}
//[16h][17h]=========================================================================
void RA8876::LCD_Horizontal_Non_Display(unsigned short WX)
{
/*
[16h] Horizontal Non-Display Period(HNDR) Bit[4:0]
This register specifies the horizontal non-display period. Also
called back porch.
Horizontal non-display period(pixels) = (HNDR + 1) * 8 + HNDFTR

[17h] Horizontal Non-Display Period Fine Tuning(HNDFT) [3:0]
This register specifies the fine tuning for horizontal non-display
period; it is used to support the SYNC mode panel. Each level of
this modulation is 1-pixel.
Horizontal non-display period(pixels) = (HNDR + 1) * 8 + HNDFTR
*/
	unsigned char temp;
	if(WX<8)
	{
	writeCmd(0x16);
	writeData(0x00);
    
	writeCmd(0x17);
	writeData(WX);
	}
	else
	{
    temp=(WX/8)-1;
	writeCmd(0x16);
	writeData(temp);
    
    temp=WX%8;
	writeCmd(0x17);
	writeData(temp);
	}	
}
//[18h]=========================================================================
void RA8876::LCD_HSYNC_Start_Position(unsigned short WX)
{
/*
[18h] HSYNC Start Position[4:0]
The starting position from the end of display area to the
beginning of HSYNC. Each level of this modulation is 8-pixel.
Also called front porch.
HSYNC Start Position(pixels) = (HSTR + 1)x8
*/
	unsigned char temp;
	if(WX<8)
	{
	writeCmd(0x18);
	writeData(0x00);
	}
	else
	{
    temp=(WX/8)-1;
	writeCmd(0x18);
	writeData(temp);	
	}
}
//[19h]=========================================================================
void RA8876::LCD_HSYNC_Pulse_Width(unsigned short WX)
{
/*
[19h] HSYNC Pulse Width(HPW) [4:0]
The period width of HSYNC.
HSYNC Pulse Width(pixels) = (HPW + 1)x8
*/
	unsigned char temp;
	if(WX<8)
	{
	writeCmd(0x19);
	writeData(0x00);
	}
	else
	{
    temp=(WX/8)-1;
	writeCmd(0x19);
	writeData(temp);	
	}
}
//[1Ch][1Dh]=========================================================================
void RA8876::LCD_Vertical_Non_Display(unsigned short HY)
{
/*
[1Ch] Vertical Non-Display Period Bit[7:0]
Vertical Non-Display Period(Line) = (VNDR + 1)

[1Dh] Vertical Non-Display Period Bit[9:8]
Vertical Non-Display Period(Line) = (VNDR + 1)
*/
	unsigned short temp;
    temp=HY-1;
	writeCmd(0x1C);
	writeData(temp);

	writeCmd(0x1D);
	writeData(temp>>8);
}
//[1Eh]=========================================================================
void RA8876::LCD_VSYNC_Start_Position(unsigned short HY)
{
/*
[1Eh] VSYNC Start Position[7:0]
The starting position from the end of display area to the beginning of VSYNC.
VSYNC Start Position(Line) = (VSTR + 1)
*/
	unsigned char temp;
    temp=HY-1;
	writeCmd(0x1E);
	writeData(temp);
}
//[1Fh]=========================================================================
void RA8876::LCD_VSYNC_Pulse_Width(unsigned short HY)
{
/*
[1Fh] VSYNC Pulse Width[5:0]
The pulse width of VSYNC in lines.
VSYNC Pulse Width(Line) = (VPWR + 1)
*/
	unsigned char temp;
    temp=HY-1;
	writeCmd(0x1F);
	writeData(temp);
}
//[20h][21h][22h][23h]=========================================================================
void RA8876::Main_Image_Start_Address(unsigned long Addr)	
{
/*
[20h] Main Image Start Address[7:2]
[21h] Main Image Start Address[15:8]
[22h] Main Image Start Address [23:16]
[23h] Main Image Start Address [31:24]
*/
	writeCmd(0x20);	writeData(Addr);
	writeCmd(0x21);	writeData(Addr>>8);
	writeCmd(0x22);	writeData(Addr>>16);
	writeCmd(0x23);	writeData(Addr>>24);
}
//[24h][25h]=========================================================================
void RA8876::Main_Image_Width(unsigned short WX)	
{
/*
[24h] Main Image Width [7:0]
[25h] Main Image Width [12:8]
Unit: Pixel.
It must be divisible by 4. MIW Bit [1:0] tie to ��0�� internally.
The value is physical pixel number. Maximum value is 8188 pixels
*/
	writeCmd(0x24);	writeData(WX);
	writeCmd(0x25);	writeData(WX>>8);
}
//[26h][27h][28h][29h]=========================================================================
void RA8876::Main_Window_Start_XY(unsigned short WX,unsigned short HY)	
{
/*
[26h] Main Window Upper-Left corner X-coordination [7:0]
[27h] Main Window Upper-Left corner X-coordination [12:8]
Reference Main Image coordination.
Unit: Pixel
It must be divisible by 4. MWULX Bit [1:0] tie to ��0�� internally.
X-axis coordination plus Horizontal display width cannot large than 8188.

[28h] Main Window Upper-Left corner Y-coordination [7:0]
[29h] Main Window Upper-Left corner Y-coordination [12:8]
Reference Main Image coordination.
Unit: Pixel
Range is between 0 and 8191.
*/
	writeCmd(0x26);	writeData(WX);
	writeCmd(0x27);	writeData(WX>>8);
	writeCmd(0x28);	writeData(HY);
	writeCmd(0x29);	writeData(HY>>8);
}
//[2Ah][2Bh][2Ch][2Dh]=========================================================================
void RA8876::PIP_Display_Start_XY(unsigned short WX,unsigned short HY)	
{
/*
[2Ah] PIP Window Display Upper-Left corner X-coordination [7:0]
[2Bh] PIP Window Display Upper-Left corner X-coordination [12:8]
Reference Main Window coordination.
Unit: Pixel
It must be divisible by 4. PWDULX Bit [1:0] tie to ��0�� internally.
X-axis coordination should less than horizontal display width.
According to bit of Select Configure PIP 1 or 2 Window��s parameters. 
Function bit will be configured for relative PIP window.

[2Ch] PIP Window Display Upper-Left corner Y-coordination [7:0]
[2Dh] PIP Window Display Upper-Left corner Y-coordination [12:8]
Reference Main Window coordination.
Unit: Pixel
Y-axis coordination should less than vertical display height.
According to bit of Select Configure PIP 1 or 2 Window��s parameters.
Function bit will be configured for relative PIP window.
*/
	writeCmd(0x2A);	writeData(WX);
	writeCmd(0x2B);	writeData(WX>>8);
	writeCmd(0x2C);	writeData(HY);
	writeCmd(0x2D);	writeData(HY>>8);
}
//[2Eh][2Fh][30h][31h]=========================================================================
void RA8876::PIP_Image_Start_Address(unsigned long Addr)	
{
/*
[2Eh] PIP Image Start Address[7:2]
[2Fh] PIP Image Start Address[15:8]
[30h] PIP Image Start Address [23:16]
[31h] PIP Image Start Address [31:24]
*/
	writeCmd(0x2E);	writeData(Addr);
	writeCmd(0x2F);	writeData(Addr>>8);
	writeCmd(0x30);	writeData(Addr>>16);
	writeCmd(0x31);	writeData(Addr>>24);
}
//[32h][33h]=========================================================================
void RA8876::PIP_Image_Width(unsigned short WX)	
{
/*
[32h] PIP Image Width [7:0]
[33h] PIP Image Width [12:8]
Unit: Pixel.
It must be divisible by 4. PIW Bit [1:0] tie to ��0�� internally.
The value is physical pixel number.
This width should less than horizontal display width.
According to bit of Select Configure PIP 1 or 2 Window��s parameters.
Function bit will be configured for relative PIP window.
*/
	writeCmd(0x32);	writeData(WX);
	writeCmd(0x33);	writeData(WX>>8);
}
//[34h][35h][36h][37h]=========================================================================
void RA8876::PIP_Window_Image_Start_XY(unsigned short WX,unsigned short HY)	
{

/*
[34h] PIP 1 or 2 Window Image Upper-Left corner X-coordination [7:0]
[35h] PIP Window Image Upper-Left corner X-coordination [12:8]
Reference PIP Image coordination.
Unit: Pixel
It must be divisible by 4. PWIULX Bit [1:0] tie to ��0�� internally.
X-axis coordination plus PIP image width cannot large than 8188.
According to bit of Select Configure PIP 1 or 2 Window��s parameters. 
Function bit will be configured for relative PIP window.

[36h] PIP Windows Display Upper-Left corner Y-coordination [7:0]
[37h] PIP Windows Image Upper-Left corner Y-coordination [12:8]
Reference PIP Image coordination.
Unit: Pixel
Y-axis coordination plus PIP window height should less than 8191.
According to bit of Select Configure PIP 1 or 2 Window��s parameters. 
Function bit will be configured for relative PIP window.
*/
	writeCmd(0x34);	writeData(WX);
	writeCmd(0x35);	writeData(WX>>8);
	writeCmd(0x36);	writeData(HY);
	writeCmd(0x37);	writeData(HY>>8);
}
//[38h][39h][3Ah][3Bh]=========================================================================
void RA8876::PIP_Window_Width_Height(unsigned short WX,unsigned short HY)	
{
/*
[38h] PIP Window Width [7:0]
[39h] PIP Window Width [10:8]
Unit: Pixel.
It must be divisible by 4. The value is physical pixel number.
Maximum value is 8188 pixels.
According to bit of Select Configure PIP 1 or 2 Window��s parameters. 
Function bit will be configured for relative PIP window.

[3Ah] PIP Window Height [7:0]
[3Bh] PIP Window Height [10:8]
Unit: Pixel
The value is physical pixel number. Maximum value is 8191 pixels.
According to bit of Select Configure PIP 1 or 2 Window��s parameters. 
Function bit will be configured for relative PIP window.
*/
	writeCmd(0x38);	writeData(WX);
	writeCmd(0x39);	writeData(WX>>8);
	writeCmd(0x3A);	writeData(HY);
	writeCmd(0x3B);	writeData(HY>>8);
}

//[3Ch]=========================================================================
void RA8876::Enable_Gamma_Correction(void)	
{
/*
Gamma correction Enable
0: Disable
1: Enable
Gamma correction is the last output stage.
*/
	unsigned char temp;
	
	writeCmd(0x3C);
	temp = readData();
	temp |= cSetb7;
	writeData(temp);
}
void RA8876::Disable_Gamma_Correction(void)	
{
/*
Gamma correction Enable
0: Disable
1: Enable
Gamma correction is the last output stage.
*/
	unsigned char temp;
	
	writeCmd(0x3C);
	temp = readData();
	temp &= cClrb7;
	writeData(temp);
}
void RA8876::Gamma_Table_for_Blue(void)	
{
/*
Gamma table select for MPU write gamma data
00b: Gamma table for Blue
01b: Gamma table for Green
10b: Gamma table for Red
*/
	unsigned char temp;
	
	writeCmd(0x3C);
	temp = readData();
	temp &= cClrb6;
	temp &= cClrb5;
	writeData(temp);
}
void RA8876::Gamma_Table_for_Green(void)	
{
/*
Gamma table select for MPU write gamma data
00b: Gamma table for Blue
01b: Gamma table for Green
10b: Gamma table for Red
*/
	unsigned char temp;
	
	writeCmd(0x3C);
	temp = readData();
	temp &= cClrb6;
	temp |= cSetb5;
	writeData(temp);
}
void RA8876::Gamma_Table_for_Red(void)	
{
/*
Gamma table select for MPU write gamma data
00b: Gamma table for Blue
01b: Gamma table for Green
10b: Gamma table for Red
*/
	unsigned char temp;
	
	writeCmd(0x3C);
	temp = readData();
	temp |= cSetb6;
	temp &= cClrb5;
	writeData(temp);
}

void RA8876::Enable_Graphic_Cursor(void)	
{
/*
Graphic Cursor Enable
0 : Graphic Cursor disable.
1 : Graphic Cursor enable.
*/
	unsigned char temp;
	
	writeCmd(0x3C);
	temp = readData();
	temp |= cSetb4;
	writeData(temp);
}
void RA8876::Disable_Graphic_Cursor(void)	
{
/*
Graphic Cursor Enable
0 : Graphic Cursor disable.
1 : Graphic Cursor enable.
*/
	unsigned char temp;
	
	writeCmd(0x3C);
	temp = readData();
	temp &= cClrb4;
	writeData(temp);
}
//
void RA8876::Select_Graphic_Cursor_1(void)	
{
/*
Graphic Cursor Selection Bit
Select one from four graphic cursor types. (00b to 11b)
00b : Graphic Cursor Set 1.
01b : Graphic Cursor Set 2.
10b : Graphic Cursor Set 3.
11b : Graphic Cursor Set 4.
*/
	unsigned char temp;
	
	writeCmd(0x3C);
	temp = readData();
	temp &= cClrb3;
	temp &= cClrb2;
	writeData(temp);
}
void RA8876::Select_Graphic_Cursor_2(void)	
{
/*
Graphic Cursor Selection Bit
Select one from four graphic cursor types. (00b to 11b)
00b : Graphic Cursor Set 1.
01b : Graphic Cursor Set 2.
10b : Graphic Cursor Set 3.
11b : Graphic Cursor Set 4.
*/
	unsigned char temp;
	
	writeCmd(0x3C);
	temp = readData();
	temp &= cClrb3;
	temp |= cSetb2;
	writeData(temp);
}
void RA8876::Select_Graphic_Cursor_3(void)	
{
/*
Graphic Cursor Selection Bit
Select one from four graphic cursor types. (00b to 11b)
00b : Graphic Cursor Set 1.
01b : Graphic Cursor Set 2.
10b : Graphic Cursor Set 3.
11b : Graphic Cursor Set 4.
*/
	unsigned char temp;
	
	writeCmd(0x3C);
	temp = readData();
	temp |= cSetb3;
	temp &= cClrb2;
	writeData(temp);
}
void RA8876::Select_Graphic_Cursor_4(void)	
{
/*
Graphic Cursor Selection Bit
Select one from four graphic cursor types. (00b to 11b)
00b : Graphic Cursor Set 1.
01b : Graphic Cursor Set 2.
10b : Graphic Cursor Set 3.
11b : Graphic Cursor Set 4.
*/
	unsigned char temp;
	
	writeCmd(0x3C);
	temp = readData();
	temp |= cSetb3;
	temp |= cSetb2;
	writeData(temp);
}
//
void RA8876::Enable_Text_Cursor(void)	
{
/*
Text Cursor Enable
0 : Disable.
1 : Enable.
Text cursor & Graphic cursor cannot enable simultaneously.
Graphic cursor has higher priority then Text cursor if enabled simultaneously.
*/
	unsigned char temp;
	
	writeCmd(0x3C);
	temp = readData();
	temp |= cSetb1;
	writeData(temp);
}
void RA8876::Disable_Text_Cursor(void)	
{
/*
Text Cursor Enable
0 : Disable.
1 : Enable.
Text cursor & Graphic cursor cannot enable simultaneously.
Graphic cursor has higher priority then Text cursor if enabled simultaneously.
*/
	unsigned char temp;
	
	writeCmd(0x3C);
	temp = readData();
	temp &= cClrb1;
	writeData(temp);
}
//
void RA8876::Enable_Text_Cursor_Blinking(void)	
{
/*
Text Cursor Blinking Enable
0 : Disable.
1 : Enable.
*/
	unsigned char temp;
	
	writeCmd(0x3C);
	temp = readData();
	temp |= cSetb0;
	writeData(temp);
}
void RA8876::Disable_Text_Cursor_Blinking(void)	
{
/*
Text Cursor Blinking Enable
0 : Disable.
1 : Enable.
*/
	unsigned char temp;
	
	writeCmd(0x3C);
	temp = readData();
	temp &= cClrb0;
	writeData(temp);
}
//[3Dh]=========================================================================
void RA8876::Blinking_Time_Frames(unsigned char temp)	
{
/*
Text Cursor Blink Time Setting (Unit: Frame)
00h : 1 frame time.
01h : 2 frames time.
02h : 3 frames time.
:
FFh : 256 frames time.
*/
	writeCmd(0x3D);
	writeData(temp);
}
//[3Eh]=========================================================================
void RA8876::Text_Cursor_H_V(unsigned short WX,unsigned short HY)
{
/*
[3Eh]
Text Cursor Horizontal Size Setting[4:0]
Unit : Pixel
Zero-based number. Value ��0�� means 1 pixel.
Note : When font is enlarged, the cursor setting will multiply the
same times as the font enlargement.
[3Fh]
Text Cursor Vertical Size Setting[4:0]
Unit : Pixel
Zero-based number. Value ��0�� means 1 pixel.
Note : When font is enlarged, the cursor setting will multiply the
same times as the font enlargement.
*/
	writeCmd(0x3E);
	writeData(WX);
	writeCmd(0x3F);
	writeData(HY);
}
//[40h][41h][42h][43h]=========================================================================
void RA8876::Graphic_Cursor_XY(unsigned short WX,unsigned short HY)
{
/*
[40h] Graphic Cursor Horizontal Location[7:0]
[41h] Graphic Cursor Horizontal Location[12:8]
[42h] Graphic Cursor Vertical Location[7:0]
[43h] Graphic Cursor Vertical Location[12:8]
Reference main Window coordination.
*/	
	writeCmd(0x40);	writeData(WX);
	writeCmd(0x41);	writeData(WX>>8);
	writeCmd(0x42);	writeData(HY);
	writeCmd(0x43);	writeData(HY>>8);
}
//[44h]=========================================================================
void RA8876::Set_Graphic_Cursor_Color_1(unsigned char temp)
{
/*
[44h] Graphic Cursor Color 0 with 256 Colors
RGB Format [7:0] = RRRGGGBB.
*/	
	writeReg(0x44,temp);
}
//[45h]=========================================================================
void RA8876::Set_Graphic_Cursor_Color_2(unsigned char temp)
{
/*
[45h] Graphic Cursor Color 1 with 256 Colors
RGB Format [7:0] = RRRGGGBB.
*/	
	writeReg(0x45,temp);
}

//[50h][51h][52h][53h]=========================================================================
void RA8876::Canvas_Image_Start_address(unsigned long Addr)	
{
/*
[50h] Start address of Canvas [7:0]
[51h] Start address of Canvas [15:8]
[52h] Start address of Canvas [23:16]
[53h] Start address of Canvas [31:24]
*/
	writeCmd(0x50);	writeData(Addr);
	writeCmd(0x51);	writeData(Addr>>8);
	writeCmd(0x52);	writeData(Addr>>16);
	writeCmd(0x53);	writeData(Addr>>24);
}
//[54h][55h]=========================================================================
void RA8876::Canvas_image_width(unsigned short WX)	
{
/*
[54h] Canvas image width [7:2]
[55h] Canvas image width [12:8]
*/
	writeCmd(0x54);	writeData(WX);
	writeCmd(0x55);	writeData(WX>>8);
}
//[56h][57h][58h][59h]=========================================================================
void RA8876::Active_Window_XY(unsigned short WX,unsigned short HY)	
{
/*
[56h] Active Window Upper-Left corner X-coordination [7:0]
[57h] Active Window Upper-Left corner X-coordination [12:8]
[58h] Active Window Upper-Left corner Y-coordination [7:0]
[59h] Active Window Upper-Left corner Y-coordination [12:8]
*/
	writeCmd(0x56);	writeData(WX);
	writeCmd(0x57);	writeData(WX>>8);
	writeCmd(0x58);	writeData(HY);
	writeCmd(0x59);	writeData(HY>>8);

}
//[5Ah][5Bh][5Ch][5Dh]=========================================================================
void RA8876::Active_Window_WH(unsigned short WX,unsigned short HY)	
{
/*
[5Ah] Width of Active Window [7:0]
[5Bh] Width of Active Window [12:8]
[5Ch] Height of Active Window [7:0]
[5Dh] Height of Active Window [12:8]
*/
	writeCmd(0x5A);	writeData(WX);
	writeCmd(0x5B);	writeData(WX>>8);
	writeCmd(0x5C);	writeData(HY);
	writeCmd(0x5D);	writeData(HY>>8);
}
//[5Eh]=========================================================================

void RA8876::Memory_XY_Mode(void)	
{
/*
Canvas addressing mode
0: Block mode (X-Y coordination addressing)
1: linear mode
*/
	unsigned char temp;

	writeCmd(0x5E);
	temp = readData();
	temp &= cClrb2;
	writeData(temp);
}
void RA8876::Memory_Linear_Mode(void)	
{
/*
Canvas addressing mode
0: Block mode (X-Y coordination addressing)
1: linear mode
*/
	unsigned char temp;

	writeCmd(0x5E);
	temp = readData();
	temp |= cSetb2;
	writeData(temp);
}
void RA8876::Memory_8bpp_Mode(void)	
{
/*
Canvas image��s color depth & memory R/W data width
In Block Mode:
00: 8bpp
01: 16bpp
1x: 24bpp
In Linear Mode:
x0: 8-bits memory data read/write.
x1: 16-bits memory data read/write
*/
	unsigned char temp;

	writeCmd(0x5E);
	temp = readData();
	temp &= cClrb1;
	temp &= cClrb0;
	writeData(temp);
}
void RA8876::Memory_16bpp_Mode(void)	
{
/*
Canvas image��s color depth & memory R/W data width
In Block Mode:
00: 8bpp
01: 16bpp
1x: 24bpp
In Linear Mode:
x0: 8-bits memory data read/write.
x1: 16-bits memory data read/write
*/
	unsigned char temp;

	writeCmd(0x5E);
	temp = readData();
	temp &= cClrb1;
	temp |= cSetb0;
	writeData(temp);
}
void RA8876::Memory_24bpp_Mode(void)	
{
/*
Canvas image��s color depth & memory R/W data width
In Block Mode:
00: 8bpp
01: 16bpp
1x: 24bpp
In Linear Mode:
x0: 8-bits memory data read/write.
x1: 16-bits memory data read/write
*/
	unsigned char temp;

	writeCmd(0x5E);
	temp = readData();
	temp |= cSetb1;
//	temp |= cSetb0;
	writeData(temp);
}

//[5Fh][60h][61h][62h]=========================================================================
void RA8876::Goto_Pixel_XY(unsigned short WX,unsigned short HY)	
{
/*
Set Graphic Read/Write position

REG[5Eh] bit3, Select to read back Graphic Read/Write position.
When DPRAM Linear mode:Graphic Read/Write Position [31:24][23:16][15:8][7:0]
When DPRAM Active window mode:Graphic Read/Write 
Horizontal Position [12:8][7:0], 
Vertical Position [12:8][7:0].
Reference Canvas image coordination. Unit: Pixel
*/
	writeCmd(0x5F);
	writeData(WX);
	writeCmd(0x60);
	writeData(WX>>8);
	writeCmd(0x61);
	writeData(HY);
	writeCmd(0x62);
	writeData(HY>>8);
}
void RA8876::Goto_Linear_Addr(unsigned long Addr)
{
/*
Set Graphic Read/Write position

REG[5Eh] bit3, Select to read back Graphic Read/Write position.
When DPRAM Linear mode:Graphic Read/Write Position [31:24][23:16][15:8][7:0]
When DPRAM Active window mode:Graphic Read/Write 
Horizontal Position [12:8][7:0], 
Vertical Position [12:8][7:0].
Reference Canvas image coordination. Unit: Pixel
*/
	writeCmd(0x5F);	writeData(Addr);
	writeCmd(0x60);	writeData(Addr>>8);
	writeCmd(0x61);	writeData(Addr>>16);
	writeCmd(0x62);	writeData(Addr>>24);
}

//[63h][64h][65h][66h]=========================================================================
void RA8876::Goto_Text_XY(unsigned short WX,unsigned short HY)	
{
/*
Set Text Write position
Text Write X-coordination [12:8][7:0]
Text Write Y-coordination [12:8][7:0]
Reference Canvas image coordination.
Unit: Pixel
*/
	writeCmd(0x63);	writeData(WX);
	writeCmd(0x64);	writeData(WX>>8);
	writeCmd(0x65);	writeData(HY);
	writeCmd(0x66);	writeData(HY>>8);
}

//[67h]=========================================================================
/*
[bit7]Draw Line / Triangle Start Signal
Write Function
0 : Stop the drawing function.
1 : Start the drawing function.
Read Function
0 : Drawing function complete.
1 : Drawing function is processing.
[bit5]Fill function for Triangle Signal
0 : Non fill.
1 : Fill.
[bit1]Draw Triangle or Line Select Signal
0 : Draw Line
1 : Draw Triangle
*/
void RA8876::Start_Line(void)
{
	writeCmd(0x67);
	writeData(0x80);

	Check_2D_Busy();
}
void RA8876::Start_Triangle(void)
{
	writeCmd(0x67);
	writeData(0x82);//B1000_0010

	Check_2D_Busy();
}
void RA8876::Start_Triangle_Fill(void)
{
	writeCmd(0x67);
	writeData(0xA2);//B1010_0010

	Check_2D_Busy();
}
//[68h][69h][6Ah][6Bh]=========================================================================
//�u�_�I
void RA8876::Line_Start_XY(unsigned short WX,unsigned short HY)
{
/*
[68h] Draw Line/Square/Triangle Start X-coordination [7:0]
[69h] Draw Line/Square/Triangle Start X-coordination [12:8]
[6Ah] Draw Line/Square/Triangle Start Y-coordination [7:0]
[6Bh] Draw Line/Square/Triangle Start Y-coordination [12:8]
*/
	writeCmd(0x68);
	writeData(WX);

	writeCmd(0x69);
	writeData(WX>>8);

	writeCmd(0x6A);
	writeData(HY);

	writeCmd(0x6B);
	writeData(HY>>8);
}
//[6Ch][6Dh][6Eh][6Fh]=========================================================================
//�u���I
void RA8876::Line_End_XY(unsigned short WX,unsigned short HY)
{
/*
[6Ch] Draw Line/Square/Triangle End X-coordination [7:0]
[6Dh] Draw Line/Square/Triangle End X-coordination [12:8]
[6Eh] Draw Line/Square/Triangle End Y-coordination [7:0]
[6Fh] Draw Line/Square/Triangle End Y-coordination [12:8]
*/
	writeCmd(0x6C);
	writeData(WX);

	writeCmd(0x6D);
	writeData(WX>>8);

	writeCmd(0x6E);
	writeData(HY);

	writeCmd(0x6F);
	writeData(HY>>8);
}
//[68h]~[73h]=========================================================================
//�T��-�I1
void RA8876::Triangle_Point1_XY(unsigned short WX,unsigned short HY)
{
/*
[68h] Draw Line/Square/Triangle Start X-coordination [7:0]
[69h] Draw Line/Square/Triangle Start X-coordination [12:8]
[6Ah] Draw Line/Square/Triangle Start Y-coordination [7:0]
[6Bh] Draw Line/Square/Triangle Start Y-coordination [12:8]
*/
	writeCmd(0x68);
	writeData(WX);

	writeCmd(0x69);
	writeData(WX>>8);

	writeCmd(0x6A);
	writeData(HY);

	writeCmd(0x6B);
	writeData(HY>>8);
}
//�T��-�I2
void RA8876::Triangle_Point2_XY(unsigned short WX,unsigned short HY)
{
/*
[6Ch] Draw Line/Square/Triangle End X-coordination [7:0]
[6Dh] Draw Line/Square/Triangle End X-coordination [12:8]
[6Eh] Draw Line/Square/Triangle End Y-coordination [7:0]
[6Fh] Draw Line/Square/Triangle End Y-coordination [12:8]
*/
	writeCmd(0x6C);
	writeData(WX);

	writeCmd(0x6D);
	writeData(WX>>8);

	writeCmd(0x6E);
	writeData(HY);

	writeCmd(0x6F);
	writeData(HY>>8);
}
//�T��-�I3
void RA8876::Triangle_Point3_XY (unsigned short WX,unsigned short HY)
{
/*
[70h] Draw Triangle Point 3 X-coordination [7:0]
[71h] Draw Triangle Point 3 X-coordination [12:8]
[72h] Draw Triangle Point 3 Y-coordination [7:0]
[73h] Draw Triangle Point 3 Y-coordination [12:8]
*/
	writeCmd(0x70);
	writeData(WX);

	writeCmd(0x71);
	writeData(WX>>8);

	writeCmd(0x72);
	writeData(HY);

	writeCmd(0x73);
	writeData(HY>>8);
}
//��_�I
void RA8876::Square_Start_XY(unsigned short WX,unsigned short HY)
{
/*
[68h] Draw Line/Square/Triangle Start X-coordination [7:0]
[69h] Draw Line/Square/Triangle Start X-coordination [12:8]
[6Ah] Draw Line/Square/Triangle Start Y-coordination [7:0]
[6Bh] Draw Line/Square/Triangle Start Y-coordination [12:8]
*/
	writeCmd(0x68);
	writeData(WX);

	writeCmd(0x69);
	writeData(WX>>8);

	writeCmd(0x6A);
	writeData(HY);

	writeCmd(0x6B);
	writeData(HY>>8);
}
//����I
void RA8876::Square_End_XY(unsigned short WX,unsigned short HY)
{
/*
[6Ch] Draw Line/Square/Triangle End X-coordination [7:0]
[6Dh] Draw Line/Square/Triangle End X-coordination [12:8]
[6Eh] Draw Line/Square/Triangle End Y-coordination [7:0]
[6Fh] Draw Line/Square/Triangle End Y-coordination [12:8]
*/
	writeCmd(0x6C);
	writeData(WX);

	writeCmd(0x6D);
	writeData(WX>>8);

	writeCmd(0x6E);
	writeData(HY);

	writeCmd(0x6F);
	writeData(HY>>8);
}
//[76h]=========================================================================
/*
[bit7]
Draw Circle / Ellipse / Square /Circle Square Start Signal 
Write Function
0 : Stop the drawing function.
1 : Start the drawing function.
Read Function
0 : Drawing function complete.
1 : Drawing function is processing.
[bit6]
Fill the Circle / Ellipse / Square / Circle Square Signal
0 : Non fill.
1 : fill.
[bit5 bit4]
Draw Circle / Ellipse / Square / Ellipse Curve / Circle Square Select
00 : Draw Circle / Ellipse
01 : Draw Circle / Ellipse Curve
10 : Draw Square.
11 : Draw Circle Square.
[bit1 bit0]
Draw Circle / Ellipse Curve Part Select
00 : bottom-left Ellipse Curve
01 : upper-left Ellipse Curve
10 : upper-right Ellipse Curve
11 : bottom-right Ellipse Curve
*/
void RA8876::Start_Circle_or_Ellipse(void)
{
	writeCmd(0x76);
	writeData(0x80);//B1000_XXXX

	Check_2D_Busy();
}
void RA8876::Start_Circle_or_Ellipse_Fill(void)
{
	writeCmd(0x76);
	writeData(0xC0);//B1100_XXXX

	Check_2D_Busy();
}
//
void RA8876::Start_Left_Down_Curve(void)
{
	writeCmd(0x76);
	writeData(0x90);//B1001_XX00

	Check_2D_Busy();
}
void RA8876::Start_Left_Up_Curve(void)
{
	writeCmd(0x76);
	writeData(0x91);//B1001_XX01

	Check_2D_Busy();
}
void RA8876::Start_Right_Up_Curve(void)
{
	writeCmd(0x76);
	writeData(0x92);//B1001_XX10

	Check_2D_Busy();	
}
void RA8876::Start_Right_Down_Curve(void)
{
	writeCmd(0x76);
	writeData(0x93);//B1001_XX11

	Check_2D_Busy();	
}
//
void RA8876::Start_Left_Down_Curve_Fill(void)
{
	writeCmd(0x76);
	writeData(0xD0);//B1101_XX00

	Check_2D_Busy();
}
void RA8876::Start_Left_Up_Curve_Fill(void)
{
	writeCmd(0x76);
	writeData(0xD1);//B1101_XX01

	Check_2D_Busy();
}
void RA8876::Start_Right_Up_Curve_Fill(void)
{
	writeCmd(0x76);
	writeData(0xD2);//B1101_XX10

	Check_2D_Busy();
}
void RA8876::Start_Right_Down_Curve_Fill(void)
{
	writeCmd(0x76);
	writeData(0xD3);//B1101_XX11

	Check_2D_Busy();
}
//
void RA8876::Start_Square(void)
{
	writeCmd(0x76);
	writeData(0xA0);//B1010_XXXX

	Check_2D_Busy();
}
void RA8876::Start_Square_Fill(void)
{
	writeCmd(0x76);
	writeData(0xE0);//B1110_XXXX

	Check_2D_Busy();
}
void RA8876::Start_Circle_Square(void)
{
	writeCmd(0x76);
	writeData(0xB0);//B1011_XXXX

	Check_2D_Busy();	
}
void RA8876::Start_Circle_Square_Fill(void)
{
	writeCmd(0x76);
	writeData(0xF0);//B1111_XXXX

	Check_2D_Busy();
}
//[77h]~[7Eh]=========================================================================
//��b�|
void RA8876::Circle_Radius_R (unsigned short WX)
{
/*
[77h] Draw Circle/Ellipse/Circle Square Major radius [7:0]
[78h] Draw Circle/Ellipse/Circle Square Major radius [12:8]
[79h] Draw Circle/Ellipse/Circle Square Minor radius [7:0]
[7Ah] Draw Circle/Ellipse/Circle Square Minor radius [12:8]
*/
	writeCmd(0x77);
	writeData(WX);

	writeCmd(0x78);
	writeData(WX>>8);

	writeCmd(0x79);
	writeData(WX);

	writeCmd(0x7A);
	writeData(WX>>8);
}

//���b�|
void RA8876::Ellipse_Radius_RxRy (unsigned short WX,unsigned short HY)
{
/*
[77h] Draw Circle/Ellipse/Circle Square Major radius [7:0]
[78h] Draw Circle/Ellipse/Circle Square Major radius [12:8]
[79h] Draw Circle/Ellipse/Circle Square Minor radius [7:0]
[7Ah] Draw Circle/Ellipse/Circle Square Minor radius [12:8]
*/
	writeCmd(0x77);
	writeData(WX);

	writeCmd(0x78);
	writeData(WX>>8);

	writeCmd(0x79);
	writeData(HY);

	writeCmd(0x7A);
	writeData(HY>>8);
}

//����ਤ�b�|
void RA8876::Circle_Square_Radius_RxRy (unsigned short WX,unsigned short HY)
{
/*
[77h] Draw Circle/Ellipse/Circle Square Major radius [7:0]
[78h] Draw Circle/Ellipse/Circle Square Major radius [12:8]
[79h] Draw Circle/Ellipse/Circle Square Minor radius [7:0]
[7Ah] Draw Circle/Ellipse/Circle Square Minor radius [12:8]
*/
	writeCmd(0x77);
	writeData(WX);

	writeCmd(0x78);
	writeData(WX>>8);

	writeCmd(0x79);
	writeData(HY);

	writeCmd(0x7A);
	writeData(HY>>8);
}

//�ꤤ��
void RA8876::Circle_Center_XY(unsigned short WX,unsigned short HY)
{
/*
[7Bh] Draw Circle/Ellipse/Circle Square Center X-coordination [7:0]
[7Ch] Draw Circle/Ellipse/Circle Square Center X-coordination [12:8]
[7Dh] Draw Circle/Ellipse/Circle Square Center Y-coordination [7:0]
[7Eh] Draw Circle/Ellipse/Circle Square Center Y-coordination [12:8]
*/
	writeCmd(0x7B);
	writeData(WX);

	writeCmd(0x7C);
	writeData(WX>>8);

	writeCmd(0x7D);
	writeData(HY);

	writeCmd(0x7E);
	writeData(HY>>8);
}
//��ꤤ��
void RA8876::Ellipse_Center_XY(unsigned short WX,unsigned short HY)
{
/*
[7Bh] Draw Circle/Ellipse/Circle Square Center X-coordination [7:0]
[7Ch] Draw Circle/Ellipse/Circle Square Center X-coordination [12:8]
[7Dh] Draw Circle/Ellipse/Circle Square Center Y-coordination [7:0]
[7Eh] Draw Circle/Ellipse/Circle Square Center Y-coordination [12:8]
*/
	writeCmd(0x7B);
	writeData(WX);

	writeCmd(0x7C);
	writeData(WX>>8);

	writeCmd(0x7D);
	writeData(HY);

	writeCmd(0x7E);
	writeData(HY>>8);
}





//[84h]=========================================================================
void RA8876::Set_PWM_Prescaler_1_to_256(unsigned short WX)
{
/*
PWM Prescaler Register
These 8 bits determine prescaler value for Timer 0 and 1.
Time base is ��Core_Freq / (Prescaler + 1)��
*/
	writeCmd(0x84);
	writeData(WX-1);
}
//[85h]=========================================================================
void RA8876::Select_PWM1_Clock_Divided_By_1(void)
{
/*
Select MUX input for PWM Timer 1.
00 = 1; 01 = 1/2; 10 = 1/4 ; 11 = 1/8;
*/
	unsigned char temp;
	
	writeCmd(0x85);
	temp = readData();
	temp &= cClrb7;
	temp &= cClrb6;
	writeData(temp);
}
void RA8876::Select_PWM1_Clock_Divided_By_2(void)
{
/*
Select MUX input for PWM Timer 1.
00 = 1; 01 = 1/2; 10 = 1/4 ; 11 = 1/8;
*/
	unsigned char temp;
	
	writeCmd(0x85);
	temp = readData();
	temp &= cClrb7;
	temp |= cSetb6;
	writeData(temp);
}
void RA8876::Select_PWM1_Clock_Divided_By_4(void)
{
/*
Select MUX input for PWM Timer 1.
00 = 1; 01 = 1/2; 10 = 1/4 ; 11 = 1/8;
*/
	unsigned char temp;
	
	writeCmd(0x85);
	temp = readData();
	temp |= cSetb7;
	temp &= cClrb6;
	writeData(temp);
}
void RA8876::Select_PWM1_Clock_Divided_By_8(void)
{
/*
Select MUX input for PWM Timer 1.
00 = 1; 01 = 1/2; 10 = 1/4 ; 11 = 1/8;
*/
	unsigned char temp;
	
	writeCmd(0x85);
	temp = readData();
	temp |= cSetb7;
	temp |= cSetb6;
	writeData(temp);
}
void RA8876::Select_PWM0_Clock_Divided_By_1(void)
{
/*
Select MUX input for PWM Timer 0.
00 = 1; 01 = 1/2; 10 = 1/4 ; 11 = 1/8;
*/
	unsigned char temp;
	
	writeCmd(0x85);
	temp = readData();
	temp &= cClrb5;
	temp &= cClrb4;
	writeData(temp);
}
void RA8876::Select_PWM0_Clock_Divided_By_2(void)
{
/*
Select MUX input for PWM Timer 0.
00 = 1; 01 = 1/2; 10 = 1/4 ; 11 = 1/8;
*/
	unsigned char temp;
	
	writeCmd(0x85);
	temp = readData();
	temp &= cClrb5;
	temp |= cSetb4;
	writeData(temp);
}
void RA8876::Select_PWM0_Clock_Divided_By_4(void)
{
/*
Select MUX input for PWM Timer 0.
00 = 1; 01 = 1/2; 10 = 1/4 ; 11 = 1/8;
*/
	unsigned char temp;
	
	writeCmd(0x85);
	temp = readData();
	temp |= cSetb5;
	temp &= cClrb4;
	writeData(temp);
}
void RA8876::Select_PWM0_Clock_Divided_By_8(void)
{
/*
Select MUX input for PWM Timer 0.
00 = 1; 01 = 1/2; 10 = 1/4 ; 11 = 1/8;
*/
	unsigned char temp;
	
	writeCmd(0x85);
	temp = readData();
	temp |= cSetb5;
	temp |= cSetb4;
	writeData(temp);
}
//[85h].[bit3][bit2]=========================================================================
/*
XPWM[1] pin function control
0X:	XPWM[1] output system error flag (REG[00h] bit[1:0], Scan bandwidth insufficient + Memory access out of range)
10:	XPWM[1] enabled and controlled by PWM timer 1
11:	XPWM[1] output oscillator clock
//If XTEST[0] set high, then XPWM[1] will become panel scan clock input.
*/
void RA8876::Select_PWM1_is_ErrorFlag(void)
{
	unsigned char temp;
	
	writeCmd(0x85);
	temp = readData();
	temp &= cClrb3;
	writeData(temp);
}
void RA8876::Select_PWM1(void)
{
	unsigned char temp;
	
	writeCmd(0x85);
	temp = readData();
	temp |= cSetb3;
	temp &= cClrb2;
	writeData(temp);
}
void RA8876::Select_PWM1_is_Osc_Clock(void)
{
	unsigned char temp;
	
	writeCmd(0x85);
	temp = readData();
	temp |= cSetb3;
	temp |= cSetb2;
	writeData(temp);
}
//[85h].[bit1][bit0]=========================================================================
/*
XPWM[0] pin function control
0X: XPWM[0] becomes GPIO-C[7]
10: XPWM[0] enabled and controlled by PWM timer 0
11: XPWM[0] output core clock
*/
void RA8876::Select_PWM0_is_GPIO_C7(void)
{
	unsigned char temp;
	
	writeCmd(0x85);
	temp = readData();
	temp &= cClrb1;
	writeData(temp);
}
void RA8876::Select_PWM0(void)
{
	unsigned char temp;
	
	writeCmd(0x85);
	temp = readData();
	temp |= cSetb1;
	temp &= cClrb0;
	writeData(temp);
}
void RA8876::Select_PWM0_is_Core_Clock(void)
{
	unsigned char temp;
	
	writeCmd(0x85);
	temp = readData();
	temp |= cSetb1;
	temp |= cSetb0;
	writeData(temp);
}
//[86h]=========================================================================
//[86h]PWM1
void RA8876::Enable_PWM1_Inverter(void)
{
/*
PWM Timer 1 output inverter on/off.
Determine the output inverter on/off for Timer 1. 
0 = Inverter off 
1 = Inverter on for PWM1
*/
	unsigned char temp;
	writeCmd(0x86);
	temp = readData();
	temp |= cSetb6;
	writeData(temp);
}
void RA8876::Disable_PWM1_Inverter(void)
{
/*
PWM Timer 1 output inverter on/off.
Determine the output inverter on/off for Timer 1. 
0 = Inverter off 
1 = Inverter on for PWM1
*/
	unsigned char temp;
	writeCmd(0x86);
	temp = readData();
	temp &= cClrb6;
	writeData(temp);
}
void RA8876::Auto_Reload_PWM1(void)
{
/*
PWM Timer 1 auto reload on/off
Determine auto reload on/off for Timer 1. 
0 = One-shot 
1 = Interval mode(auto reload)
*/
	unsigned char temp;
	writeCmd(0x86);
	temp = readData();
	temp |= cSetb5;
	writeData(temp);
}
void RA8876::One_Shot_PWM1(void)
{
/*
PWM Timer 1 auto reload on/off
Determine auto reload on/off for Timer 1. 
0 = One-shot 
1 = Interval mode(auto reload)
*/
	unsigned char temp;
	writeCmd(0x86);
	temp = readData();
	temp &= cClrb5;
	writeData(temp);
}
void RA8876::Start_PWM1(void)
{
/*
PWM Timer 1 start/stop
Determine start/stop for Timer 1. 
0 = Stop 
1 = Start for Timer 1
*/
	unsigned char temp;
	writeCmd(0x86);
	temp = readData();
	temp |= cSetb4;
	writeData(temp);
}
void RA8876::Stop_PWM1(void)
{
/*
PWM Timer 1 start/stop
Determine start/stop for Timer 1. 
0 = Stop 
1 = Start for Timer 1
*/
	unsigned char temp;
	writeCmd(0x86);
	temp = readData();
	temp &= cClrb4;
	writeData(temp);
}
//[86h]PWM0
void RA8876::Enable_PWM0_Dead_Zone(void)
{
/*
PWM Timer 0 Dead zone enable
Determine the dead zone operation. 0 = Disable. 1 = Enable.
*/
	unsigned char temp;
	writeCmd(0x86);
	temp = readData();
	temp |= cSetb3;
	writeData(temp);
}
void RA8876::Disable_PWM0_Dead_Zone(void)
{
/*
PWM Timer 0 Dead zone enable
Determine the dead zone operation. 0 = Disable. 1 = Enable.
*/
	unsigned char temp;
	writeCmd(0x86);
	temp = readData();
	temp &= cClrb3;
	writeData(temp);
}
void RA8876::Enable_PWM0_Inverter(void)
{
/*
PWM Timer 0 output inverter on/off
Determine the output inverter on/off for Timer 0. 
0 = Inverter off 
1 = Inverter on for PWM0
*/
	unsigned char temp;
	writeCmd(0x86);
	temp = readData();
	temp |= cSetb2;
	writeData(temp);
}
void RA8876::Disable_PWM0_Inverter(void)
{
/*
PWM Timer 0 output inverter on/off
Determine the output inverter on/off for Timer 0. 
0 = Inverter off 
1 = Inverter on for PWM0
*/
	unsigned char temp;
	writeCmd(0x86);
	temp = readData();
	temp &= cClrb2;
	writeData(temp);
}
void RA8876::Auto_Reload_PWM0(void)
{
/*
PWM Timer 0 auto reload on/off
Determine auto reload on/off for Timer 0. 
0 = One-shot 
1 = Interval mode(auto reload)
*/
	unsigned char temp;
	writeCmd(0x86);
	temp = readData();
	temp |= cSetb1;
	writeData(temp);
}
void RA8876::One_Shot_PWM0(void)
{
/*
PWM Timer 1 auto reload on/off
Determine auto reload on/off for Timer 1. 
0 = One-shot 
1 = Interval mode(auto reload)
*/
	unsigned char temp;
	writeCmd(0x86);
	temp = readData();
	temp &= cClrb1;
	writeData(temp);
}
void RA8876::Start_PWM0(void)
{
/*
PWM Timer 0 start/stop
Determine start/stop for Timer 0. 
0 = Stop 
1 = Start for Timer 0
*/
	unsigned char temp;
	writeCmd(0x86);
	temp = readData();
	temp |= cSetb0;
	writeData(temp);
}
void RA8876::Stop_PWM0(void)
{
/*
PWM Timer 0 start/stop
Determine start/stop for Timer 0. 
0 = Stop 
1 = Start for Timer 0
*/
	unsigned char temp;
	writeCmd(0x86);
	temp = readData();
	temp &= cClrb0;
	writeData(temp);
}
//[87h]=========================================================================
void RA8876::Set_Timer0_Dead_Zone_Length(unsigned char temp)
{
/*
Timer 0 Dead zone length register
These 8 bits determine the dead zone length. The 1 unit time of
the dead zone length is equal to that of timer 0.
*/
	writeCmd(0x87);
	writeData(temp);
}
//[88h][89h]=========================================================================
void RA8876::Set_Timer0_Compare_Buffer(unsigned short WX)
{
/*
Timer 0 compare buffer register
Compare buffer register total has 16 bits.
When timer counter equal or less than compare buffer register will cause PWM out
high level if inv_on bit is off.
*/
	writeCmd(0x88);
	writeData(WX);
	writeCmd(0x89);
	writeData(WX>>8);
}
//[8Ah][8Bh]=========================================================================
void RA8876::Set_Timer0_Count_Buffer(unsigned short WX)
{
/*
Timer 0 count buffer register
Count buffer register total has 16 bits.
When timer counter equal to 0 will cause PWM timer reload Count buffer register if reload_en bit set as enable.
It may read back timer counter��s real time value when PWM timer start.
*/
	writeCmd(0x8A);
	writeData(WX);
	writeCmd(0x8B);
	writeData(WX>>8);
}
//[8Ch][8Dh]=========================================================================
void RA8876::Set_Timer1_Compare_Buffer(unsigned short WX)
{
/*
Timer 0 compare buffer register
Compare buffer register total has 16 bits.
When timer counter equal or less than compare buffer register will cause PWM out
high level if inv_on bit is off.
*/
	writeCmd(0x8C);
	writeData(WX);
	writeCmd(0x8D);
	writeData(WX>>8);
}
//[8Eh][8Fh]=========================================================================
void RA8876::Set_Timer1_Count_Buffer(unsigned short WX)
{
/*
Timer 0 count buffer register
Count buffer register total has 16 bits.
When timer counter equal to 0 will cause PWM timer reload Count buffer register if reload_en bit set as enable.
It may read back timer counter��s real time value when PWM timer start.
*/
	writeCmd(0x8E);
	writeData(WX);
	writeCmd(0x8F);
	writeData(WX>>8);
}


//[90h]~[B5h]=========================================================================

//[90h]=========================================================================
void RA8876::BTE_Enable(void)
{	
/*
BTE Function Enable
0 : BTE Function disable.
1 : BTE Function enable.
*/
    unsigned char temp;
    writeCmd(0x90);
    temp = readData();
    temp |= cSetb4 ;
	writeData(temp);  
}

//[90h]=========================================================================
void RA8876::BTE_Disable(void)
{	
/*
BTE Function Enable
0 : BTE Function disable.
1 : BTE Function enable.
*/
    unsigned char temp;
    writeCmd(0x90);
    temp = readData();
    temp &= cClrb4 ;
	writeData(temp);  
}

//[90h]=========================================================================
void RA8876::Check_BTE_Busy(void)
{	
/*
case1: Status Register_bit3
0 : BTE function is idle.
1 : BTE function is busy.

case2: [90h]BTE Function Control Register_bit4
Read
0 : BTE function is idle.
1 : BTE function is busy.
*/
	unsigned char temp;
	
	//case1: using status register 	
	do
	{
		temp=readStatus();
	}while(temp&0x08);


	//case2: using BTE Function Control Register
	do
	{
	    writeCmd(0x90);
	    temp = readData();
	}while(temp&0x10);

}
//[90h]=========================================================================
void RA8876::Pattern_Format_8X8(void)
{	
/*
Pattern Format
0 : 8X8
1 : 16X16
*/
    unsigned char temp;
    writeCmd(0x90);
    temp = readData();
    temp &= cClrb0 ;
	writeData(temp);
}	
//[90h]=========================================================================
void RA8876::Pattern_Format_16X16(void)
{	
/*
Pattern Format
0 : 8X8
1 : 16X16
*/
    unsigned char temp;
    writeCmd(0x90);
    temp = readData();
    temp |= cSetb0 ;
	  writeData(temp);
}	

//[91h]=========================================================================
void RA8876::BTE_ROP_Code(unsigned char setx)
{	
/*
BTE ROP Code[Bit7:4]
	
0000 : 0(Blackness)
0001 : ~S0.~S1 or ~ ( S0+S1 )
0010 : ~S0.S1
0011 : ~S0
0100 : S0.~S1
0101 : ~S1
0110 : S0^S1
0111 : ~S0+~S1 or ~ ( S0.S1 )
1000 : S0.S1
1001 : ~ ( S0^S1 )
1010 : S1
1011 : ~S0+S1
1100 : S0
1101 : S0+~S1
1110 : S0+S1
1111 : 1 ( Whiteness )
*/
    unsigned char temp;
    writeCmd(0x91);
    temp = readData();
    temp &= 0x0f ;
		temp |= (setx<<4);
	  writeData(temp);
}
	
//[91h]=========================================================================
void RA8876::BTE_Operation_Code(unsigned char setx)
{	
/*
BTE Operation Code[Bit3:0]
	
0000/ 0 : MPU Write BTE with ROP.
0001    : NA
0010/ 2 : Memory copy (move) BTE in positive direction with ROP.
0011    : NA
0100/ 4 : MPU Write with chroma keying (w/o ROP)
0101/ 5 : Memory copy (move) with chroma keying (w/o ROP)
0110/ 6 : Pattern Fill with ROP
0111/ 7 : Pattern Fill with chroma keying (w/o ROP)
1000/ 8 : MPU Write with Color Expansion (w/o ROP)
1001/ 9 : MPU Write with Color Expansion and chroma keying (w/o ROP)
1010/10 : Memory write with opacity (w/o ROP)
1011/11 : MPU Write with opacity (w/o ROP)
1100/12 : Solid Fill (w/o ROP)
1101    : NA
1110/14 : Memory write with Color Expansion (w/o ROP)
1111/15 : Memory write with Color Expansion and chroma keying (w/o ROP)
*/
    unsigned char temp;
    writeCmd(0x91);
    temp = readData();
	temp &= 0xf0 ;
	temp |= setx ;
	writeData(temp);

}
//[92h]=========================================================================
void RA8876::BTE_S0_Color_8bpp(void)
{	
/*
S0 Color Depth
00 : 256 Color
01 : 64k Color
1x : 16M Color
*/	
    unsigned char temp;
    writeCmd(0x92);
    temp = readData();
    temp &= cClrb6 ;
	temp &= cClrb5 ;
	writeData(temp);
}	
//[92h]=========================================================================
void RA8876::BTE_S0_Color_16bpp(void)
{	
/*
S0 Color Depth
00 : 256 Color
01 : 64k Color
1x : 16M Color
*/	
    unsigned char temp;
    writeCmd(0x92);
    temp = readData();
    temp &= cClrb6 ;
	temp |= cSetb5 ;
	writeData(temp);

}	
//[92h]=========================================================================
void RA8876::BTE_S0_Color_24bpp(void)
{	
/*
S0 Color Depth
00 : 256 Color
01 : 64k Color
1x : 16M Color
*/	
    unsigned char temp;
    writeCmd(0x92);
    temp = readData();
	temp |= cSetb6 ;
	//temp |= cSetb5 ;
	writeData(temp);
}
//[92h]=========================================================================
void RA8876::BTE_S1_Color_8bpp(void)
{	
/*
S1 Color Depth
000 : 256 Color
001 : 64k Color
010 : 16M Color
011 : Constant Color
100 : 8 bit pixel alpha blending
101 : 16 bit pixel alpha blending
*/	
    unsigned char temp;
    writeCmd(0x92);
    temp = readData();
    temp &= cClrb4 ;
		temp &= cClrb3 ;
	  temp &= cClrb2 ;
	  writeData(temp);
}	
//[92h]=========================================================================
void RA8876::BTE_S1_Color_16bpp(void)
{	
/*
S1 Color Depth
000 : 256 Color
001 : 64k Color
010 : 16M Color
011 : Constant Color
100 : 8 bit pixel alpha blending
101 : 16 bit pixel alpha blending
*/	
    unsigned char temp;
    writeCmd(0x92);
    temp = readData();
    temp &= cClrb4 ;
		temp &= cClrb3 ;
	  temp |= cSetb2 ;
	  writeData(temp);

}
//[92h]=========================================================================
void RA8876::BTE_S1_Color_24bpp(void)
{	
/*
S1 Color Depth
000 : 256 Color
001 : 64k Color
010 : 16M Color
011 : Constant Color
100 : 8 bit pixel alpha blending
101 : 16 bit pixel alpha blending
*/	
    unsigned char temp;
    writeCmd(0x92);
    temp = readData();
	temp &= cClrb4 ;
	temp |= cSetb3 ;
	temp &= cClrb2 ;
	writeData(temp);
}

//[92h]=========================================================================
void RA8876::BTE_S1_Color_Constant(void)
{	
/*
S1 Color Depth
000 : 256 Color
001 : 64k Color
010 : 16M Color
011 : Constant Color
100 : 8 bit pixel alpha blending
101 : 16 bit pixel alpha blending
*/	
    unsigned char temp;
    writeCmd(0x92);
    temp = readData();
    temp &= cClrb4 ;
		temp |= cSetb3 ;
	  temp |= cSetb2 ;
	  writeData(temp);
}



//[92h]=========================================================================
void RA8876::BTE_S1_Color_8bit_Alpha(void)
{	
/*
S1 Color Depth
000 : 256 Color
001 : 64k Color
010 : 16M Color
011 : Constant Color
100 : 8 bit pixel alpha blending
101 : 16 bit pixel alpha blending
*/	
    unsigned char temp;
    writeCmd(0x92);
    temp = readData();
    temp |= cSetb4 ;
		temp &= cClrb3 ;
	  temp &= cClrb2 ;
	  writeData(temp);
}

//[92h]=========================================================================
void RA8876::BTE_S1_Color_16bit_Alpha(void)
{	
/*
S1 Color Depth
000 : 256 Color
001 : 64k Color
010 : 16M Color
011 : Constant Color
100 : 8 bit pixel alpha blending
101 : 16 bit pixel alpha blending
*/	
    unsigned char temp;
    writeCmd(0x92);
    temp = readData();
    temp |= cSetb4 ;
		temp &= cClrb3 ;
	  temp |= cSetb2 ;
	  writeData(temp);
}

//[92h]=========================================================================
void RA8876::BTE_Destination_Color_8bpp(void)
{	
/*
Destination Color Depth
00 : 256 Color
01 : 64k Color
1x : 16M Color
*/	
    unsigned char temp;
    writeCmd(0x92);
    temp = readData();
    temp &= cClrb1 ;
		temp &= cClrb0 ;
	  writeData(temp);
}	
//[92h]=========================================================================
void RA8876::BTE_Destination_Color_16bpp(void)
{	
/*
Destination Color Depth
00 : 256 Color
01 : 64k Color
1x : 16M Color
*/	
    unsigned char temp;
    writeCmd(0x92);
    temp = readData();
    temp &= cClrb1 ;
		temp |= cSetb0 ;
	  writeData(temp);

}	
//[92h]=========================================================================
void RA8876::BTE_Destination_Color_24bpp(void)
{	
/*
Destination Color Depth
00 : 256 Color
10 : 64k Color
1x : 16M Color
*/	
    unsigned char temp;
    writeCmd(0x92);
    temp = readData();
    temp |= cSetb1 ;
		//temp |= cSetb0 ;
	  writeData(temp);
}


//[93h][94h][95h][96h]=========================================================================
void RA8876::BTE_S0_Memory_Start_Address(unsigned long Addr)	
{
/*
[93h] BTE S0 Memory Start Address [7:0]
[94h] BTE S0 Memory Start Address [15:8]
[95h] BTE S0 Memory Start Address [23:16]
[96h] BTE S0 Memory Start Address [31:24]
Bit [1:0] tie to ��0�� internally.
*/
	writeReg(0x93,Addr);
	writeReg(0x94,Addr>>8);
	writeReg(0x95,Addr>>16);
	writeReg(0x96,Addr>>24);
}


//[97h][98h]=========================================================================
void RA8876::BTE_S0_Image_Width(unsigned short WX)	
{
/*
[97h] BTE S0 Image Width [7:0]
[98h] BTE S0 Image Width [12:8]
Unit: Pixel.
Bit [1:0] tie to ��0�� internally.
*/
	writeReg(0x97,WX);
	writeReg(0x98,WX>>8);
}


//[99h][9Ah][9Bh][9Ch]=========================================================================
void RA8876::BTE_S0_Window_Start_XY(unsigned short WX,unsigned short HY)	
{
/*
[99h] BTE S0 Window Upper-Left corner X-coordination [7:0]
[9Ah] BTE S0 Window Upper-Left corner X-coordination [12:8]
[9Bh] BTE S0 Window Upper-Left corner Y-coordination [7:0]
[9Ch] BTE S0 Window Upper-Left corner Y-coordination [12:8]
*/

	writeReg(0x99,WX);
	writeReg(0x9A,WX>>8);

	writeReg(0x9B,HY);
	writeReg(0x9C,HY>>8);
}




//[9Dh][9Eh][9Fh][A0h]=========================================================================
void RA8876::BTE_S1_Memory_Start_Address(unsigned long Addr)	
{
/*
[9Dh] BTE S1 Memory Start Address [7:0]
[9Eh] BTE S1 Memory Start Address [15:8]
[9Fh] BTE S1 Memory Start Address [23:16]
[A0h] BTE S1 Memory Start Address [31:24]
Bit [1:0] tie to ��0�� internally.
*/
	writeReg(0x9D,Addr);
	writeReg(0x9E,Addr>>8);
	writeReg(0x9F,Addr>>16);
	writeReg(0xA0,Addr>>24);
}


//Input data format:R3G3B2
void RA8876::S1_Constant_color_256(unsigned char temp)
{
    writeCmd(0x9D);
    writeData(temp);

    writeCmd(0x9E);
    writeData(temp<<3);

    writeCmd(0x9F);
    writeData(temp<<6);
}

//Input data format:R5G6B6
void RA8876::S1_Constant_color_65k(unsigned short temp)
{
    writeCmd(0x9D);
    writeData(temp>>8);

    writeCmd(0x9E);
    writeData(temp>>3);

    writeCmd(0x9F);
    writeData(temp<<3);
}

//Input data format:R8G8B8
void RA8876::S1_Constant_color_16M(unsigned long temp)
{
    writeCmd(0x9D);
    writeData(temp>>16);

    writeCmd(0x9E);
    writeData(temp>>8);

    writeCmd(0x9F);
    writeData(temp);
}




//[A1h][A2h]=========================================================================
void RA8876::BTE_S1_Image_Width(unsigned short WX)	
{
/*
[A1h] BTE S1 Image Width [7:0]
[A2h] BTE S1 Image Width [12:8]
Unit: Pixel.
Bit [1:0] tie to ��0�� internally.
*/
	writeReg(0xA1,WX);
	writeReg(0xA2,WX>>8);
}


//[A3h][A4h][A5h][A6h]=========================================================================
void RA8876::BTE_S1_Window_Start_XY(unsigned short WX,unsigned short HY)	
{
/*
[A3h] BTE S1 Window Upper-Left corner X-coordination [7:0]
[A4h] BTE S1 Window Upper-Left corner X-coordination [12:8]
[A5h] BTE S1 Window Upper-Left corner Y-coordination [7:0]
[A6h] BTE S1 Window Upper-Left corner Y-coordination [12:8]
*/

	writeReg(0xA3,WX);
	writeReg(0xA4,WX>>8);

	writeReg(0xA5,HY);
	writeReg(0xA6,HY>>8);
}




//[A7h][A8h][A9h][AAh]=========================================================================
void RA8876::BTE_Destination_Memory_Start_Address(unsigned long Addr)	
{
/*
[A7h] BTE Destination Memory Start Address [7:0]
[A8h] BTE Destination Memory Start Address [15:8]
[A9h] BTE Destination Memory Start Address [23:16]
[AAh] BTE Destination Memory Start Address [31:24]
Bit [1:0] tie to ��0�� internally.
*/
	writeReg(0xA7,Addr);
	writeReg(0xA8,Addr>>8);
	writeReg(0xA9,Addr>>16);
	writeReg(0xAA,Addr>>24);
}


//[ABh][ACh]=========================================================================
void RA8876::BTE_Destination_Image_Width(unsigned short WX)	
{
/*
[ABh] BTE Destination Image Width [7:0]
[ACh] BTE Destination Image Width [12:8]
Unit: Pixel.
Bit [1:0] tie to ��0�� internally.
*/
	writeReg(0xAB,WX);
	writeReg(0xAC,WX>>8);
}


//[ADh][AEh][AFh][B0h]=========================================================================
void RA8876::BTE_Destination_Window_Start_XY(unsigned short WX,unsigned short HY)	
{
/*
[ADh] BTE Destination Window Upper-Left corner X-coordination [7:0]
[AEh] BTE Destination Window Upper-Left corner X-coordination [12:8]
[AFh] BTE Destination Window Upper-Left corner Y-coordination [7:0]
[B0h] BTE Destination Window Upper-Left corner Y-coordination [12:8]
*/

	writeReg(0xAD,WX);
	writeReg(0xAE,WX>>8);

	writeReg(0xAF,HY);
	writeReg(0xB0,HY>>8);
}


//[B1h][B2h][B3h][B4h]===============================================================

void RA8876::BTE_Window_Size(unsigned short WX, unsigned short HY)

{
/*
[B1h] BTE Window Width [7:0]
[B2h] BTE Window Width [12:8]

[B3h] BTE Window Height [7:0]
[B4h] BTE Window Height [12:8]
*/
        writeReg(0xB1,WX);
        writeReg(0xB2,WX>>8);
	
	    writeReg(0xB3,HY);
        writeReg(0xB4,HY>>8);
}

//[B5h]=========================================================================
void RA8876::BTE_Alpha_Blending_Effect(unsigned char temp)
{	
/*
Window Alpha Blending effect for S0 & S1
The value of alpha in the color code ranges from 0.0 to 1.0,
where 0.0 represents a fully transparent color, and 1.0
represents a fully opaque color.
00h: 0
01h: 1/32
02h: 2/32
:
1Eh: 30/32
1Fh: 31/32
2Xh: 1
Output Effect = (S0 image x (1 - alpha setting value)) + (S1 image x alpha setting value)
*/
    writeCmd(0xB5);
	writeData(temp);  
}


//[B6h]=========================================================================
void RA8876::Start_SFI_DMA(void)
{
	unsigned char temp;
	writeCmd(0xB6);
	temp = readData();
    temp |= cSetb0;
	writeData(temp);
}

void RA8876::Check_Busy_SFI_DMA(void)
{
	writeCmd(0xB6);
	do
	{		
	}while((readData()&0x01)==0x01);
}


//[B7h]=========================================================================
void RA8876::Select_SFI_0(void)
{
/*[bit7]
Serial Flash/ROM I/F # Select
0: Serial Flash/ROM 0 I/F is selected.
1: Serial Flash/ROM 1 I/F is selected.
*/
	unsigned char temp;
	writeCmd(0xB7);
	temp = readData();
    temp &= cClrb7;
	writeData(temp);
}
void RA8876::Select_SFI_1(void)
{
/*[bit7]
Serial Flash/ROM I/F # Select
0: Serial Flash/ROM 0 I/F is selected.
1: Serial Flash/ROM 1 I/F is selected.
*/
	unsigned char temp;
	writeCmd(0xB7);
	temp = readData();
    temp |= cSetb7;
	writeData(temp);
}
void RA8876::Select_SFI_Font_Mode(void)
{
/*[bit6]
Serial Flash /ROM Access Mode
0: Font mode �V for external cgrom
1: DMA mode �V for cgram , pattern , bootstart image or osd
*/
	unsigned char temp;
	writeCmd(0xB7);
	temp = readData();
    temp &= cClrb6;
	writeData(temp);
}
void RA8876::Select_SFI_DMA_Mode(void)
{
/*[bit6]
Serial Flash /ROM Access Mode
0: Font mode �V for external cgrom
1: DMA mode �V for cgram , pattern , bootstart image or osd
*/
	unsigned char temp;
	writeCmd(0xB7);
	temp = readData();
    temp |= cSetb6;
	writeData(temp);
}
void RA8876::Select_SFI_24bit_Address(void)
{
/*[bit5]
Serial Flash/ROM Address Mode
0: 24 bits address mode
1: 32 bits address mode
*/
	unsigned char temp;
	writeCmd(0xB7);
	temp = readData();
    temp &= cClrb5;
	writeData(temp);
}
void RA8876::Select_SFI_32bit_Address(void)
{
/*[bit5]
Serial Flash/ROM Address Mode
0: 24 bits address mode
1: 32 bits address mode
*/
	unsigned char temp;
	writeCmd(0xB7);
	temp = readData();
    temp |= cSetb5;
	writeData(temp);
}
void RA8876::Select_standard_SPI_Mode0_or_Mode3(void)
{
/*[bit4]
RA8875 compatible mode
0: standard SPI mode 0 or mode 3 timing
1: Follow RA8875 mode 0 & mode 3 timing
*/
	unsigned char temp;
	writeCmd(0xB7);
	temp = readData();
    temp &= cClrb4;
	writeData(temp);
}
void RA8876::Select_RA8875_SPI_Mode0_and_Mode3(void)
{
/*[bit4]
RA8875 compatible mode
0: standard SPI mode 0 or mode 3 timing
1: Follow RA8875 mode 0 & mode 3 timing
*/
	unsigned char temp;
	writeCmd(0xB7);
	temp = readData();
    temp |= cSetb4;
	writeData(temp);
}

void RA8876::Select_SFI_Single_Mode_Dummy_0T_03h(void)
{
/*
000xb: 1x read command code = 03h. Without dummy cycle between address and data.
010xb: 1x read command code = 0Bh. 8 dummy cycles inserted between address and data.
1x0xb: 1x read command code = 1Bh. 16 dummy cycles inserted between address and data.
*/
	unsigned char temp;
	writeCmd(0xB7);
	temp = readData();
    temp &= 0xF0;
	writeData(temp);
}
void RA8876::Select_SFI_Single_Mode_Dummy_8T_0Bh(void)
{
/*
000xb: 1x read command code = 03h. Without dummy cycle between address and data.
010xb: 1x read command code = 0Bh. 8 dummy cycles inserted between address and data.
1x0xb: 1x read command code = 1Bh. 16 dummy cycles inserted between address and data.
*/
	unsigned char temp;
	writeCmd(0xB7);
	temp = readData();
	temp &= 0xF0;
   temp |= cSetb2;
	writeData(temp);
}
void RA8876::Select_SFI_Single_Mode_Dummy_16T_1Bh(void)
{
/*
000xb: 1x read command code = 03h. Without dummy cycle between address and data.
010xb: 1x read command code = 0Bh. 8 dummy cycles inserted between address and data.
1x0xb: 1x read command code = 1Bh. 16 dummy cycles inserted between address and data.
*/
	unsigned char temp;
	writeCmd(0xB7);
	temp = readData();
	temp &= 0xF0;
   temp |= cSetb3;
	writeData(temp);
}
void RA8876::Select_SFI_Dual_Mode_Dummy_8T_3Bh(void)
{
/*
xx10b: 2x read command code = 3Bh. 8 dummy cycles inserted between address and data phase. (mode 0)
xx11b: 2x read command code = BBh. 4 dummy cycles inserted between address and data phase. (mode 1)
*/
	unsigned char temp;
	writeCmd(0xB7);
	temp = readData();
	temp &= 0xF0;
  temp |= 0x02;
	writeData(temp);
}
void RA8876::Select_SFI_Dual_Mode_Dummy_4T_BBh(void)
{
/*
xx10b: 2x read command code = 3Bh. 8 dummy cycles inserted between address and data phase. (mode 0)
xx11b: 2x read command code = BBh. 4 dummy cycles inserted between address and data phase. (mode 1)
*/
	unsigned char temp;
	writeCmd(0xB7);
	temp = readData();
	temp &= 0xF0;
  temp |= 0x03;
	writeData(temp);
}







//REG[B8h] SPI master Tx /Rx FIFO Data Register (SPIDR) 
unsigned char RA8876::SPI_Master_FIFO_Data_Put(unsigned char Data)
{
    unsigned char temp;
	writeCmd(0xB8);
	writeData(Data);
	while(Tx_FIFO_Empty_Flag()==0);	//�ŤF�A����U�@��
	temp = SPI_Master_FIFO_Data_Get();

	return temp;
}

unsigned char RA8876::SPI_Master_FIFO_Data_Get(void)
{
   unsigned char temp;

	while(Rx_FIFO_Empty_Flag()==1);//���O�Ū����U����
	writeCmd(0xB8);
	temp=readData();
	//while(Rx_FIFO_full_flag()); //�s��g�J16����Ƥ~�ݭn
   return temp;
}

//REG[B9h] SPI master Control Register (SPIMCR2) 
//void Mask_SPI_Master_Interrupt_Flag(void)(B9h�ק�)
void RA8876::Enable_SPI_Master_Interrupt(void)
{
/*
SPI Master Interrupt enable
0: Disable interrupt.
1: Enable interrupt.
*/
  unsigned char temp;
  writeCmd(0xB9);
  temp = readData();
  temp |= cSetb6;
  writeData(temp);
} 
void RA8876::Disable_SPI_Master_Interrupt(void)
{
/*
SPI Master Interrupt enable
0: Disable interrupt.
1: Enable interrupt.
*/
  unsigned char temp;
  writeCmd(0xB9);
  temp = readData();
  temp &= cClrb6;
  writeData(temp);
}

void RA8876::Select_nSS_drive_on_xnsfcs0(void)
{
  unsigned char temp;
  writeCmd(0xB9);
  temp = readData();
  temp &= cClrb5;
  writeData(temp);

}

void RA8876::Select_nSS_drive_on_xnsfcs1(void)
{
  unsigned char temp;
  writeCmd(0xB9);
  temp = readData();
  temp |= cSetb5;
  writeData(temp);

}

//0: inactive (nSS port will goes high) 
void RA8876::nSS_Inactive(void)
{
  unsigned char temp;
  writeCmd(0xB9);
  temp = readData();
  temp &= cClrb4;
  writeData(temp);
}
//1: active (nSS port will goes low) 
void RA8876::nSS_Active(void)
{
  unsigned char temp;
  writeCmd(0xB9);
  temp = readData();
  temp |= cSetb4;
  writeData(temp);
}

//Mask interrupt for FIFO overflow error [OVFIRQEN]
//void OVFIRQEN_Enable(void)(B9h�ק�)
void RA8876::Mask_FIFO_overflow_error_Interrupt(void)
{
/*
Mask interrupt for FIFO overflow error [OVFIRQEN]
0: unmask
1: mask
*/
  unsigned char temp;
  writeCmd(0xB9);
  temp = readData();
  temp |= cSetb3;
  writeData(temp);
}
void RA8876::Unmask_FIFO_overflow_error_Interrupt(void)
{
/*
Mask interrupt for FIFO overflow error [OVFIRQEN]
0: unmask
1: mask
*/
  unsigned char temp;
  writeCmd(0xB9);
  temp = readData();
  temp &= cClrb3;
  writeData(temp);
}


//void EMTIRQEN_Enable(void)(B9h�ק�)
void RA8876::Mask_EMTIRQEN_Interrupt(void)
{
/*
Mask interrupt for while Tx FIFO empty & SPI engine/FSM idle [EMTIRQEN]
0: unmask
1: mask
*/
  unsigned char temp;
  writeCmd(0xB9);
  temp = readData();
  temp |= cSetb2;
  writeData(temp);
}
void RA8876::Unmask_EMTIRQEN_Interrupt(void)
{
/*
Mask interrupt for while Tx FIFO empty & SPI engine/FSM idle [EMTIRQEN]
0: unmask
1: mask
*/
  unsigned char temp;
  writeCmd(0xB9);
  temp = readData();
  temp &= cClrb2;
  writeData(temp);
}

/*
SPI operation mode
Only support mode 0 & mode 3, when enable serial flash��s DMA
or access Getop��s character serial ROM device.
mode / CPOL:Clock Polarity bit / CPHA:Clock Phase bit
	0	0	0
	1	0	1
	2	1	0
	3	1	1
*/
void RA8876::Reset_CPOL(void)
{
  unsigned char temp;
  writeCmd(0xB9);
  temp = readData();
  temp &= cClrb1;
  writeData(temp);
}

void RA8876::Set_CPOL(void)
{
  unsigned char temp;
  writeCmd(0xB9);
  temp = readData();
  temp |= cSetb1;
  writeData(temp);
}

void RA8876::Reset_CPHA(void)
{
  unsigned char temp;
  writeCmd(0xB9);
  temp = readData();
  temp &= cClrb0;
  writeData(temp);
}

void RA8876::Set_CPHA(void)
{
  unsigned char temp;
  writeCmd(0xB9);
  temp = readData();
  temp |= cSetb0;
  writeData(temp);
}







//REG[BAh] SPI master Status Register (SPIMSR) 
unsigned char RA8876::Tx_FIFO_Empty_Flag(void)
{
  writeCmd(0xBA);
  if((readData()&0x80)==0x80)
  return 1;
  else
  return 0;
}

unsigned char RA8876::Tx_FIFO_Full_Flag(void)
{
  writeCmd(0xBA);
  if((readData()&0x40)==0x40)
  return 1;
  else
  return 0;
} 

unsigned char RA8876::Rx_FIFO_Empty_Flag(void)
{
  writeCmd(0xBA);
  if((readData()&0x20)==0x20)
  return 1;
  else
  return 0;
} 

unsigned char RA8876::Rx_FIFO_full_flag(void)
{
   writeCmd(0xBA);
   if((readData()&0x10)==0x10)
   return 1;
   else
   return 0;
} 

unsigned char RA8876::OVFI_Flag(void)
{
   writeCmd(0xBA);
   if((readData()&0x08)==0x08)
   return 1;
   else
   return 0;
}

void RA8876::Clear_OVFI_Flag(void)
{
   unsigned char temp;
   writeCmd(0xBA);
   temp = readData();
   temp |= cSetb3;
   writeData(temp);
}

unsigned char RA8876::EMTI_Flag(void)
{
   writeCmd(0xBA);
   if((readData()&0x04)==0x04)
   return 1;
   else
   return 0;
}

void RA8876::Clear_EMTI_Flag(void)
{
   unsigned char temp;
   writeCmd(0xBA);
   temp = readData();
   temp |= cSetb2;
   writeData(temp);
}


//REG[BB] SPI Clock period (SPIDIV) 
void RA8876::SPI_Clock_Period(unsigned char temp)
{
/*
SPI_clock= CORE_FREQ / ((Divisor+1)x2)
*/
   writeCmd(0xBB);
   writeData(temp);
} 

//[BCh][BDh][BEh][BFh]=========================================================================
void RA8876::SFI_DMA_Source_Start_Address(unsigned long Addr)
{
/*
DMA Source START ADDRESS
This bits index serial flash address [7:0][15:8][23:16][31:24]
*/
	writeCmd(0xBC);
	writeData(Addr);
	writeCmd(0xBD);
	writeData(Addr>>8);
	writeCmd(0xBE);
	writeData(Addr>>16);
	writeCmd(0xBF);
	writeData(Addr>>24);
}
//[C0h][C1h][C2h][C3h]=========================================================================
void RA8876::SFI_DMA_Destination_Start_Address(unsigned long Addr)
{
/*
DMA Destination START ADDRESS 
[1:0]Fix at 0
This bits index SDRAM address [7:0][15:8][23:16][31:24]
*/
	writeCmd(0xC0);
	writeData(Addr);
	writeCmd(0xC1);
	writeData(Addr>>8);
	writeCmd(0xC2);
	writeData(Addr>>16);
	writeCmd(0xC3);
	writeData(Addr>>24);
}
//[C0h][C1h][C2h][C3h]=========================================================================
void RA8876::SFI_DMA_Destination_Upper_Left_Corner(unsigned short WX,unsigned short HY)
{
/*
C0h
This register defines DMA Destination Window Upper-Left corner 
X-coordination [7:0] on Canvas area. 
When REG DMACR bit 1 = 1 (Block Mode) 
This register defines Destination address [7:2] in SDRAM. 
C1h
When REG DMACR bit 1 = 0 (Linear Mode) 
This register defines DMA Destination Window Upper-Left corner 
X-coordination [12:8] on Canvas area. 
When REG DMACR bit 1 = 1 (Block Mode) 
This register defines Destination address [15:8] in SDRAM.
C2h
When REG DMACR bit 1 = 0 (Linear Mode) 
This register defines DMA Destination Window Upper-Left corner
Y-coordination [7:0] on Canvas area. 
When REG DMACR bit 1 = 1 (Block Mode) 
This register defines Destination address [23:16] in SDRAM. 
C3h
When REG DMACR bit 1 = 0 (Linear Mode) 
This register defines DMA Destination Window Upper-Left corner 
Y-coordination [12:8] on Canvas area. 
When REG DMACR bit 1 = 1 (Block Mode) 
This register defines Destination address [31:24] in SDRAM. 
*/
 
 writeCmd(0xC0);
 writeData(WX);
 writeCmd(0xC1);
 writeData(WX>>8);
 
 writeCmd(0xC2);
 writeData(HY);
 writeCmd(0xC3);
 writeData(HY>>8);
}



//[C6h][C7h][C8h][C9h]=========================================================================
void RA8876::SFI_DMA_Transfer_Number(unsigned long Addr)
{
/*
When REG DMACR bit 1 = 0 (Linear Mode)
DMA Transfer Number [7:0][15:8][23:16][31:24]

When REG DMACR bit 1 = 1 (Block Mode)
DMA Block Width [7:0][15:8]
DMA Block HIGH[7:0][15:8]
*/
	writeCmd(0xC6);
	writeData(Addr);
	writeCmd(0xC7);
	writeData(Addr>>8);
	writeCmd(0xC8);
	writeData(Addr>>16);
	writeCmd(0xC9);
	writeData(Addr>>24);
}
void RA8876::SFI_DMA_Transfer_Width_Height(unsigned short WX,unsigned short HY)
{
/*
When REG DMACR bit 1 = 0 (Linear Mode)
DMA Transfer Number [7:0][15:8][23:16][31:24]

When REG DMACR bit 1 = 1 (Block Mode)
DMA Block Width [7:0][15:8]
DMA Block HIGH[7:0][15:8]
*/
	writeCmd(0xC6);
	writeData(WX);
	writeCmd(0xC7);
	writeData(WX>>8);

	writeCmd(0xC8);
	writeData(HY);
	writeCmd(0xC9);
	writeData(HY>>8);
}
//[CAh][CBh]=========================================================================
void RA8876::SFI_DMA_Source_Width(unsigned short WX)
{
/*
DMA Source Picture Width [7:0][12:8]
Unit: pixel
*/
	writeCmd(0xCA);
	writeData(WX);
	writeCmd(0xCB);
	writeData(WX>>8);
}

//[CCh]=========================================================================

void RA8876::Font_Select_UserDefine_Mode(void)
{
/*[bit7-6]
User-defined Font /CGROM Font Selection Bit in Text Mode
00 : Internal CGROM
01 : Genitop serial flash
10 : User-defined Font
*/
	unsigned char temp;
	writeCmd(0xCC);
	temp = readData();
    temp |= cSetb7;
	temp &= cClrb6;
	writeData(temp);
}
void RA8876::CGROM_Select_Internal_CGROM(void)
{
/*[bit7-6]
User-defined Font /CGROM Font Selection Bit in Text Mode
00 : Internal CGROM
01 : Genitop serial flash
10 : User-defined Font
*/
	unsigned char temp;
	writeCmd(0xCC);
	temp = readData();
	temp &= cClrb7;
    temp &= cClrb6;
	writeData(temp);
}
void RA8876::CGROM_Select_Genitop_FontROM(void)
{
/*[bit7-6]
User-defined Font /CGROM Font Selection Bit in Text Mode
00 : Internal CGROM
01 : Genitop serial flash
10 : User-defined Font
*/
	unsigned char temp;
	writeCmd(0xCC);
	temp = readData();
	temp &= cClrb7;
    temp |= cSetb6;
	writeData(temp);
}
void RA8876::Font_Select_8x16_16x16(void)
{
/*[bit5-4]
Font Height Setting
00b : 8x16 / 16x16.
01b : 12x24 / 24x24.
10b : 16x32 / 32x32.
*** User-defined Font width is decided by font code. Genitop
serial flash��s font width is decided by font code or GT Font ROM
control register.
*/
	unsigned char temp;
	writeCmd(0xCC);
	temp = readData();
    temp &= cClrb5;
    temp &= cClrb4;
	writeData(temp);
}
void RA8876::Font_Select_12x24_24x24(void)
{
/*[bit5-4]
Font Height Setting
00b : 8x16 / 16x16.
01b : 12x24 / 24x24.
10b : 16x32 / 32x32.
*** User-defined Font width is decided by font code. Genitop
serial flash��s font width is decided by font code or GT Font ROM
control register.
*/
	unsigned char temp;
	writeCmd(0xCC);
	temp = readData();
    temp &= cClrb5;
    temp |= cSetb4;
	writeData(temp);
}
void RA8876::Font_Select_16x32_32x32(void)
{
/*[bit5-4]
Font Height Setting
00b : 8x16 / 16x16.
01b : 12x24 / 24x24.
10b : 16x32 / 32x32.
*** User-defined Font width is decided by font code. Genitop
serial flash��s font width is decided by font code or GT Font ROM
control register.
*/
	unsigned char temp;
	writeCmd(0xCC);
	temp = readData();
    temp |= cSetb5;
    temp &= cClrb4;
	writeData(temp);
}
void RA8876::Internal_CGROM_Select_ISOIEC8859_1(void)
{
/*
Font Selection for internal CGROM
When FNCR0 B7 = 0 and B5 = 0, Internal CGROM supports the
8x16 character sets with the standard coding of ISO/IEC 8859-1,2,4,5,
which supports English and most of European country languages.
00b : ISO/IEC 8859-1.
01b : ISO/IEC 8859-2.
10b : ISO/IEC 8859-4.
11b : ISO/IEC 8859-5.
*/
	unsigned char temp;
	writeCmd(0xCC);
	temp = readData();
    temp &= cClrb1;
    temp &= cClrb0;
	writeData(temp);
}
void RA8876::Internal_CGROM_Select_ISOIEC8859_2(void)
{
/*
Font Selection for internal CGROM
When FNCR0 B7 = 0 and B5 = 0, Internal CGROM supports the
8x16 character sets with the standard coding of ISO/IEC 8859-1,2,4,5,
which supports English and most of European country languages.
00b : ISO/IEC 8859-1.
01b : ISO/IEC 8859-2.
10b : ISO/IEC 8859-4.
11b : ISO/IEC 8859-5.
*/
	unsigned char temp;
	writeCmd(0xCC);
	temp = readData();
    temp &= cClrb1;
    temp |= cSetb0;
	writeData(temp);
}
void RA8876::Internal_CGROM_Select_ISOIEC8859_4(void)
{
/*
Font Selection for internal CGROM
When FNCR0 B7 = 0 and B5 = 0, Internal CGROM supports the
8x16 character sets with the standard coding of ISO/IEC 8859-1,2,4,5, 
which supports English and most of European country languages.
00b : ISO/IEC 8859-1.
01b : ISO/IEC 8859-2.
10b : ISO/IEC 8859-4.
11b : ISO/IEC 8859-5.
*/
	unsigned char temp;
	writeCmd(0xCC);
	temp = readData();
    temp |= cSetb1;
    temp &= cClrb0;
	writeData(temp);
}
void RA8876::Internal_CGROM_Select_ISOIEC8859_5(void)
{
/*
Font Selection for internal CGROM
When FNCR0 B7 = 0 and B5 = 0, Internal CGROM supports the
8x16 character sets with the standard coding of ISO/IEC 8859-1,2,4,5, 
which supports English and most of European country languages.
00b : ISO/IEC 8859-1.
01b : ISO/IEC 8859-2.
10b : ISO/IEC 8859-4.
11b : ISO/IEC 8859-5.
*/
	unsigned char temp;
	writeCmd(0xCC);
	temp = readData();
    temp |= cSetb1;
    temp |= cSetb0;
	writeData(temp);
}
//[CDh]=========================================================================
void RA8876::Enable_Font_Alignment(void)
{
/*
Full Alignment Selection Bit
0 : Full alignment disable.
1 : Full alignment enable.
*/
	unsigned char temp;
	writeCmd(0xCD);
	temp = readData();
    temp |= cSetb7;
	writeData(temp);
}
void RA8876::Disable_Font_Alignment(void)
{
/*
Full Alignment Selection Bit
0 : Full alignment disable.
1 : Full alignment enable.
*/
	unsigned char temp;
	writeCmd(0xCD);
	temp = readData();
    temp &= cClrb7;
	writeData(temp);
}
void RA8876::Font_Background_select_Original_Canvas(void)
{
/*		
Font Transparency
0 : Character�s background displayed with specified color.
1 : Character�s background displayed with original canvas background.
*/
	unsigned char temp;
	writeCmd(0xCD);
	temp = readData();
    temp |= cSetb6;
	writeData(temp);
}
void RA8876::Font_Background_select_Color(void)
{
/*
Font Transparency
0 : Character�s background displayed with specified color.
1 : Character�s background displayed with original canvas background.
*/
	unsigned char temp;
	writeCmd(0xCD);
	temp = readData();
    temp &= cClrb6;
	writeData(temp);
}
void RA8876::Font_0_degree(void)
{
/*
Font Rotation
0 : Normal
Text direction from left to right then from top to bottom
1 : Counterclockwise 90 degree & horizontal flip
Text direction from top to bottom then from left to right
(it should accommodate with set VDIR as 1)
This attribute can be changed only when previous font write
finished (core_busy = 0)
*/
	unsigned char temp;
	writeCmd(0xCD);
	temp = readData();
    temp &= cClrb4;
	writeData(temp);
}
void RA8876::Font_90_degree(void)
{
/*
Font Rotation
0 : Normal
Text direction from left to right then from top to bottom
1 : Counterclockwise 90 degree & horizontal flip
Text direction from top to bottom then from left to right
(it should accommodate with set VDIR as 1)
This attribute can be changed only when previous font write
finished (core_busy = 0)
*/
	unsigned char temp;
	writeCmd(0xCD);
	temp = readData();
    temp |= cSetb4;
	writeData(temp);
}
void RA8876::Font_Width_X1(void)
{
/*
Horizontal Font Enlargement
00b : X1.
01b : X2.
10b : X3.
11b : X4.
*/
	unsigned char temp;
	writeCmd(0xCD);
	temp = readData();
    temp &= cClrb3;
    temp &= cClrb2;
	writeData(temp);
}
void RA8876::Font_Width_X2(void)
{
/*
Horizontal Font Enlargement
00b : X1.
01b : X2.
10b : X3.
11b : X4.
*/
	unsigned char temp;
	writeCmd(0xCD);
	temp = readData();
    temp &= cClrb3;
    temp |= cSetb2;
	writeData(temp);
}
void RA8876::Font_Width_X3(void)
{
/*
Horizontal Font Enlargement
00b : X1.
01b : X2.
10b : X3.
11b : X4.
*/
	unsigned char temp;
	writeCmd(0xCD);
	temp = readData();
    temp |= cSetb3;
    temp &= cClrb2;
	writeData(temp);
}
void RA8876::Font_Width_X4(void)
{
/*
Horizontal Font Enlargement
00b : X1.
01b : X2.
10b : X3.
11b : X4.
*/
	unsigned char temp;
	writeCmd(0xCD);
	temp = readData();
    temp |= cSetb3;
    temp |= cSetb2;
	writeData(temp);
}
void RA8876::Font_Height_X1(void)
{
/*
Vertical Font Enlargement
00b : X1.
01b : X2.
10b : X3.
11b : X4.
*/
	unsigned char temp;
	writeCmd(0xCD);
	temp = readData();
    temp &= cClrb1;
    temp &= cClrb0;
	writeData(temp);
}
void RA8876::Font_Height_X2(void)
{
/*
Vertical Font Enlargement
00b : X1.
01b : X2.
10b : X3.
11b : X4.
*/
	unsigned char temp;
	writeCmd(0xCD);
	temp = readData();
    temp &= cClrb1;
    temp |= cSetb0;
	writeData(temp);
}
void RA8876::Font_Height_X3(void)
{
/*
Vertical Font Enlargement
00b : X1.
01b : X2.
10b : X3.
11b : X4.
*/
	unsigned char temp;
	writeCmd(0xCD);
	temp = readData();
    temp |= cSetb1;
    temp &= cClrb0;
	writeData(temp);
}
void RA8876::Font_Height_X4(void)
{
/*
Vertical Font Enlargement
00b : X1.
01b : X2.
10b : X3.
11b : X4.
*/
	unsigned char temp;
	writeCmd(0xCD);
	temp = readData();
    temp |= cSetb1;
    temp |= cSetb0;
	writeData(temp);
}

//[CEh]=========================================================================
void RA8876::GTFont_Select_GT21L16T1W(void)
{
/*
GT Serial Font ROM Select
000b: GT21L16T1W
001b: GT30L16U2W
010b: GT30L24T3Y
011b: GT30L24M1Z
100b: GT30L32S4W
101b: GT20L24F6Y
110b: GT21L24S1W
*/
	unsigned char temp;
	writeCmd(0xCE);
	temp = readData();
    temp &= cClrb7;
    temp &= cClrb6;
    temp &= cClrb5;
	writeData(temp);
}
void RA8876::GTFont_Select_GT30L16U2W(void)
{
/*
GT Serial Font ROM Select
000b: GT21L16T1W
001b: GT30L16U2W
010b: GT30L24T3Y
011b: GT30L24M1Z
100b: GT30L32S4W
101b: GT20L24F6Y
110b: GT21L24S1W
*/
	unsigned char temp;
	writeCmd(0xCE);
	temp = readData();
    temp &= cClrb7;
    temp &= cClrb6;
    temp |= cSetb5;
	writeData(temp);
}
void RA8876::GTFont_Select_GT30L24T3Y(void)
{
/*
GT Serial Font ROM Select
000b: GT21L16T1W
001b: GT30L16U2W
010b: GT30L24T3Y
011b: GT30L24M1Z
100b: GT30L32S4W
101b: GT20L24F6Y
110b: GT21L24S1W
*/
	unsigned char temp;
	writeCmd(0xCE);
	temp = readData();
    temp &= cClrb7;
    temp |= cSetb6;
    temp &= cClrb5;
	writeData(temp);
}
void RA8876::GTFont_Select_GT30L24M1Z(void)
{
/*
GT Serial Font ROM Select
000b: GT21L16T1W
001b: GT30L16U2W
010b: GT30L24T3Y
011b: GT30L24M1Z
100b: GT30L32S4W
101b: GT20L24F6Y
110b: GT21L24S1W
*/
	unsigned char temp;
	writeCmd(0xCE);
	temp = readData();
    temp &= cClrb7;
    temp |= cSetb6;
    temp |= cSetb5;
	writeData(temp);
}
void RA8876::GTFont_Select_GT30L32S4W(void)
{
/*
GT Serial Font ROM Select
000b: GT21L16T1W
001b: GT30L16U2W
010b: GT30L24T3Y
011b: GT30L24M1Z
100b: GT30L32S4W
101b: GT20L24F6Y
110b: GT21L24S1W
*/
	unsigned char temp;
	writeCmd(0xCE);
	temp = readData();
    temp |= cSetb7;
    temp &= cClrb6;
    temp &= cClrb5;
	writeData(temp);
}
void RA8876::GTFont_Select_GT20L24F6Y(void)
{
/*
GT Serial Font ROM Select
000b: GT21L16T1W
001b: GT30L16U2W
010b: GT30L24T3Y
011b: GT30L24M1Z
100b: GT30L32S4W
101b: GT20L24F6Y
110b: GT21L24S1W
*/
	unsigned char temp;
	writeCmd(0xCE);
	temp = readData();
    temp |= cSetb7;
    temp &= cClrb6;
    temp |= cSetb5;
	writeData(temp);
}
void RA8876::GTFont_Select_GT21L24S1W(void)
{
/*
GT Serial Font ROM Select
000b: GT21L16T1W
001b: GT30L16U2W
010b: GT30L24T3Y
011b: GT30L24M1Z
100b: GT30L32S4W
101b: GT20L24F6Y
110b: GT21L24S1W
*/
	unsigned char temp;
	writeCmd(0xCE);
	temp = readData();
    temp |= cSetb7;
    temp |= cSetb6;
    temp &= cClrb5;
	writeData(temp);
}


//[CFh]=========================================================================
void RA8876::Set_GTFont_Decoder(unsigned char temp)
{
/*
[bit7-3]
FONT ROM Coding Setting
For specific GT serial Font ROM, the coding method must be set for decoding.
00000b: GB2312
00001b: GB12345/GB18030
00010b: BIG5
00011b: UNICODE
00100b: ASCII
00101b: UNI-Japanese
00110b: JIS0208
00111b: Latin/Greek/ Cyrillic / Arabic/Thai/Hebrew
01000b: Korea
10001b: ISO-8859-1
10010b: ISO-8859-2
10011b: ISO-8859-3
10100b: ISO-8859-4
10101b: ISO-8859-5
10110b: ISO-8859-6
10111b: ISO-8859-7
11000b: ISO-8859-8
11001b: ISO-8859-9
11010b: ISO-8859-10
11011b: ISO-8859-11
11100b: ISO-8859-12
11101b: ISO-8859-13
11110b: ISO-8859-14
11111b: ISO-8859-15

[bit1-0]
ASCII / Latin/Greek/ Cyrillic / Arabic
		(ASCII)		(Latin/Greek/Cyrillic)	(Arabic)
00b		Normal			Normal 					NA
01b		Arial 		Variable Width 			Presentation Forms-A
10b		Roman 			NA 					Presentation Forms-B
11b		Bold			NA 						NA
*/
	writeCmd(0xCF);
	writeData(temp);
}
//[D0h]=========================================================================
void RA8876::Font_Line_Distance(unsigned char temp)
{
/*[bit4-0]
Font Line Distance Setting
Setting the font character line distance when setting memory font
write cursor auto move. (Unit: pixel)
*/
	writeCmd(0xD0);
	writeData(temp);
}
//[D1h]=========================================================================
void RA8876::Set_Font_to_Font_Width(unsigned char temp)
{
/*[bit5-0]
Font to Font Width Setting (Unit: pixel)
*/
	writeCmd(0xD1);
	writeData(temp);
}
//[D2h]~[D4h]=========================================================================
void RA8876::Foreground_RGB(unsigned char R,unsigned char G,unsigned char B)
{
/*
[D2h] Foreground Color - Red, for draw, text or color expansion
[D3h] Foreground Color - Green, for draw, text or color expansion
[D4h] Foreground Color - Blue, for draw, text or color expansion
*/  
    writeCmd(0xD2);
 writeData(R);
 
    writeCmd(0xD3);
 writeData(G);
  
    writeCmd(0xD4);
 writeData(B);
}

//Input data format:R3G3B2  
void RA8876::Foreground_color_256(unsigned char temp) 
{
    writeCmd(0xD2);
 writeData(temp);
 
    writeCmd(0xD3);
 writeData(temp<<3);
  
    writeCmd(0xD4);
 writeData(temp<<6);
}
 
//Input data format:R5G6B5 
void RA8876::Foreground_color_65k(unsigned short temp)
{
    writeCmd(0xD2);
 writeData(temp>>8);
 
    writeCmd(0xD3);
 writeData(temp>>3);
  
    writeCmd(0xD4);
 writeData(temp<<3);
}
 
//Input data format:R8G8B8 
void RA8876::Foreground_color_16M(unsigned long temp)
{
    writeCmd(0xD2);
 writeData(temp>>16);
 
    writeCmd(0xD3);
 writeData(temp>>8);
  
    writeCmd(0xD4);
 writeData(temp);
}
 
 
 
//[D5h]~[D7h]=========================================================================
/*
[D5h] Background Color - Red, for Text or color expansion
[D6h] Background Color - Green, for Text or color expansion
[D7h] Background Color - Blue, for Text or color expansion
*/ 
void RA8876::Background_RGB(unsigned char R,unsigned char G,unsigned char B)
{
   
    writeCmd(0xD5);
 writeData(R);
  
    writeCmd(0xD6);
 writeData(G);
   
    writeCmd(0xD7);
 writeData(B);
}
 
//Input data format:R3G3B2
void RA8876::Background_color_256(unsigned char temp)
{
    writeCmd(0xD5);
 writeData(temp);
  
    writeCmd(0xD6);
 writeData(temp<<3);
   
    writeCmd(0xD7);
 writeData(temp<<6);
}
 
//Input data format:R5G6B6
void RA8876::Background_color_65k(unsigned short temp)
{
    writeCmd(0xD5);
 writeData(temp>>8);
  
    writeCmd(0xD6);
 writeData(temp>>3);
   
    writeCmd(0xD7);
 writeData(temp<<3);
}
 
//Input data format:R8G8B8
void RA8876::Background_color_16M(unsigned long temp)
{
    writeCmd(0xD5);
 writeData(temp>>16);
  
    writeCmd(0xD6);
 writeData(temp>>8);
   
    writeCmd(0xD7);
 writeData(temp);
}

//[DBh]~[DEh]=========================================================================
void RA8876::CGRAM_Start_address(unsigned long Addr)
{
/*
CGRAM START ADDRESS [31:0]
*/	 
    writeCmd(0xDB);
	writeData(Addr);
    writeCmd(0xDC);
	writeData(Addr>>8);
    writeCmd(0xDD);
	writeData(Addr>>16);
    writeCmd(0xDE);
	writeData(Addr>>24);
}

//[DFh]=========================================================================
/*
[bit7] Enter Power saving state
0: Normal state.
1: Enter power saving state.
[bit1][bit0] Power saving Mode definition
00: NA
01: Standby Mode
10: Suspend Mode
11: Sleep Mode
*/
void RA8876::Power_Normal_Mode(void)
{
	writeCmd(0xDF);
	writeData(0x00);
	Check_IC_ready();
}
void RA8876::Power_Saving_Standby_Mode(void)
{
	writeCmd(0xDF);
//	writeData(0x01);
//	writeCmd(0xDF);
	writeData(0x81);
}
void RA8876::Power_Saving_Suspend_Mode(void)
{
	writeCmd(0xDF);
//	writeData(0x02);
//	writeCmd(0xDF);
	writeData(0x82);
}
void RA8876::Power_Saving_Sleep_Mode(void)
{
	writeCmd(0xDF);
	writeData(0x03);
	writeCmd(0xDF);
	writeData(0x83);
}










//[E5h]~[E6h]=========================================================================
void RA8876::RA8876_I2CM_Clock_Prescale(unsigned short WX)
{
/*
I2C Master Clock Pre-scale [7:0]
I2C Master Clock Pre-scale [15:8]
XSCL = CCLK / (5*(prescale + 2))
*/  
    writeCmd(0xE5);
 writeData(WX);
    writeCmd(0xE6);
 writeData(WX>>8);
}
//[E7h]=========================================================================
void RA8876::RA8876_I2CM_Transmit_Data(unsigned char temp)
{
/*
I2C Master Transmit[7:0]
*/  
    writeCmd(0xE7);
 writeData(temp);
}
//[E8h]=========================================================================
unsigned char RA8876::RA8876_I2CM_Receiver_Data(void)
{
/*
I2C Master Receiver [7:0]
*/  
 unsigned char temp;
    writeCmd(0xE8);
 temp=readData();
 return temp;
}
//[E9h]=========================================================================
/*
[bit7] START
	Generate (repeated) start condition and be cleared by hardware automatically
	Note : This bit is always read as 0.
[bit6] STOP
	Generate stop condition and be cleared by hardware automatically
	Note : This bit is always read as 0.
[bit5] READ
	READ(READ and WRITE can��t be used simultaneously)
	Read form slave and be cleared by hardware automatically
	Note : This bit is always read as 0.
[bit4] WRITE
	WRITE(READ and WRITE can��t be used simultaneously)
	Write to slave and be cleared by hardware automatically
	Note : This bit is always read as 0.
[bit3] ACKNOWLEDGE
	When as a I2C master receiver
	0 : Sent ACK.
	1 : Sent NACK.
	Note : This bit is always read as 0.
[bit0] Noise Filter
	0 : Enable.
	1 : Disable.
*/

//[bit7] START
void RA8876::RA8876_I2CM_Write_With_Start(void)
{
 writeCmd(0xE9);
#ifdef Disable_I2CM_Noise_Filter 
 writeData(0x90);
#endif
 
#ifdef Enable_I2CM_Noise_Filter 
 writeData(0x91);
#endif		  
}

//[bit6] STOP 
void RA8876::RA8876_I2CM_Stop(void)
{
 writeCmd(0xE9);
#ifdef Disable_I2CM_Noise_Filter 
 writeData(0x40);
#endif
 
#ifdef Enable_I2CM_Noise_Filter 
 writeData(0x41);
#endif
}

//[bit5] READ
void RA8876::RA8876_I2CM_Read_With_Ack(void)
{		  
 writeCmd(0xE9);
#ifdef Disable_I2CM_Noise_Filter 
 writeData(0x20);
#endif
#ifdef Enable_I2CM_Noise_Filter 
 writeData(0x21);
#endif
}
 
void RA8876::RA8876_I2CM_Read_With_Nack(void)
{		  
 writeCmd(0xE9);
#ifdef Disable_I2CM_Noise_Filter 
 writeData(0x68);
#endif
#ifdef Enable_I2CM_Noise_Filter 
 writeData(0x69);
#endif
}

//[bit4] WRITE 
void RA8876::RA8876_I2CM_Write(void)
{
 writeCmd(0xE9);
#ifdef Disable_I2CM_Noise_Filter 
 writeData(0x10);
#endif
#ifdef Enable_I2CM_Noise_Filter 
 writeData(0x11);
#endif
}
 
 
 
//[EAh]=========================================================================

unsigned char RA8876::RA8876_I2CM_Check_Slave_ACK(void)
{
/*[bit7]
Received acknowledge from slave
0 : Acknowledge received.
1 : No Acknowledge received. 
*/ 
unsigned char temp;
 
 writeCmd(0xEA);
 temp=readData();
 if((temp&0x80)==0x80)
  return 1;
 else
  return 0; 
}

unsigned char RA8876::RA8876_I2CM_Bus_Busy(void)
{
/*[bit6]
I2C Bus is Busy
0 : Idle.
1 : Busy.  
*/ 
 unsigned char temp; 

 writeCmd(0xEA);
 temp=readData();
 if((temp&0x40)==0x40)
  return 1;
 else
  return 0;    
}

unsigned char RA8876::RA8876_I2CM_transmit_Progress(void)
{
/*[bit6]
 0=Complete
 1=Transferring
*/ 
 unsigned char temp; 

 writeCmd(0xEA);
 temp=readData();
 if((temp&0x02)==0x02)
 return 1;
 else
 return 0;
}
 
unsigned char RA8876::RA8876_I2CM_Arbitration(void)
{
/*[bit6]
I2C Bus is Busy
0 : Idle.
1 : Busy.  
*/ 
 unsigned char temp; 

 writeCmd(0xEA);
 temp=readData();
 temp&=0x01;
 return temp;
}


//[F0h]=========================================================================
void RA8876::Set_GPIO_A_In_Out(unsigned char temp)
{
/*
GPO-A_dir[7:0] : General Purpose I/O direction control.
0: Output
1: Input
*/
	writeCmd(0xF0);
	writeData(temp);
}
//[F1h]=========================================================================
void RA8876::Write_GPIO_A_7_0(unsigned char temp)
{
/*
GPI-A[7:0] : General Purpose Input, share with DB[15:8]
GPO-A[7:0] : General Purpose Output, share with DB[15:8]
*/
	writeCmd(0xF1);
	writeData(temp);
}
unsigned char RA8876::Read_GPIO_A_7_0(void)
{
/*
GPI-A[7:0] : General Purpose Input, share with DB[15:8]
GPO-A[7:0] : General Purpose Output, share with DB[15:8]
*/
	unsigned char temp;
	writeCmd(0xF1);
	temp=readData();
	return temp;
}
//[F2h]=========================================================================
void RA8876::Write_GPIO_B_7_4(unsigned char temp)
{
/*
Bit [7:4] is port B's General Purpose Output bit [7:4]. They are share with XKOUT[3:0]
Bit [3:0] is not writable.
*/
	writeCmd(0xF2);
	writeData(temp);
}
unsigned char RA8876::Read_GPIO_B_7_0(void)
{
/*
Bit[7:0] are share with {XKIN[3:0], XA0, XnWR, XnRD, XnCS}
Bit[3:0] are only available in serial MPU I/F, otherwise fix at 0.
*/
	unsigned char temp;
	writeCmd(0xF2);
	temp=readData();
	return temp;
}

//[F3h]=========================================================================
void RA8876::Set_GPIO_C_In_Out(unsigned char temp)
{
/*
GPIO-C_dir[7:0] : General Purpose I/O direction control.
0: Output
1: Input
*/
	writeCmd(0xF3);
	writeData(temp);
}
//[F4h]=========================================================================
void RA8876::Write_GPIO_C_7_0(unsigned char temp)
{
/*
REG[F4h] GPIO-C
GPIO-C[7] : XPWM0,
GPIO_C[4:0] : XnSFCS1, XnSFCS0, XMISO, XMOSI, XSCLK.
*/
	writeCmd(0xF4);
	writeData(temp);
}
unsigned char RA8876::Read_GPIO_C_7_0(void)
{
/*
REG[F4h] GPIO-C
GPIO-C[7] : XPWM0,
GPIO_C[4:0] : XnSFCS1, XnSFCS0, XMISO, XMOSI, XSCLK.
*/
	unsigned char temp;
	writeCmd(0xF4);
	temp=readData();
	return temp;
}
//[F5h]=========================================================================
void RA8876::Set_GPIO_D_In_Out(unsigned char temp)
{
/*
GPIO-D_dir[7:0] : General Purpose I/O direction control.
0: Output
1: Input
*/
	writeCmd(0xF5);
	writeData(temp);
}
//[F6h]=========================================================================
void RA8876::Write_GPIO_D_7_0(unsigned char temp)
{
/*
GPIO-D[7:0] : General Purpose Input/Output
*/
	writeCmd(0xF6);
	writeData(temp);
}
unsigned char RA8876::Read_GPIO_D_7_0(void)
{
/*
GPIO-D[7:0] : General Purpose Input/Output
*/
	unsigned char temp;
	writeCmd(0xF6);
	temp=readData();
	return temp;
}
//[F7h]=========================================================================
void RA8876::Set_GPIO_E_In_Out(unsigned char temp)
{
/*
GPIO-E_dir[7:0] : General Purpose I/O direction control.
0: Output
1: Input
*/
	writeCmd(0xF7);
	writeData(temp);
}
//[F8h]=========================================================================
void RA8876::Write_GPIO_E_7_0(unsigned char temp)
{
/*
GPIO-E[7:0] : General Purpose Input/Output.
share with {PDAT[23:19], PDAT[15:13]}
*/
	writeCmd(0xF8);
	writeData(temp);
}
unsigned char RA8876::Read_GPIO_E_7_0(void)
{
/*
GPIO-E[7:0] : General Purpose Input/Output.
share with {PDAT[23:19], PDAT[15:13]}
*/
	unsigned char temp;
	writeCmd(0xF8);
	temp=readData();
	return temp;
}
//[F9h]=========================================================================
void RA8876::Set_GPIO_F_In_Out(unsigned char temp)
{
/*
GPIO-F_dir[7:0] : General Purpose I/O direction control.
0: Output
1: Input
*/
	writeCmd(0xF9);
	writeData(temp);
}
//[FAh]=========================================================================
void RA8876::Write_GPIO_F_7_0(unsigned char temp)
{
/*
GPIO-F[7:0] : General Purpose Input/Output.
share with {XPDAT[12:10], XPDAT[7:3]}
*/
	writeCmd(0xFA);
	writeData(temp);
}
unsigned char RA8876::Read_GPIO_F_7_0(void)
{
/*
GPIO-F[7:0] : General Purpose Input/Output.
share with {XPDAT[12:10], XPDAT[7:3]}
*/
	unsigned char temp;
	writeCmd(0xFA);
	temp=readData();
	return temp;
}

//[FBh]=========================================================================


void RA8876::Long_Key_enable(void)
{
 /*
Key-Scan Control Register 1
[bit6]		LongKey Enable Bit
1 : Enable. Long key period is set by KSCR2 bit4-2.
0 : Disable.
*/
	unsigned char temp;
	writeCmd(0xFB);
	temp=readData();
	temp |= cSetb6;
	writeData(temp);
}


void RA8876::Key_Scan_Freg(unsigned char setx)
{
/*KF2-0: Key-Scan Frequency */
  unsigned char temp;
  writeCmd(0xFB);
  temp = readData();
  temp &= 0xf0;
  temp|= (setx&0x07);
  writeData(temp);
}


//[FCh]=========================================================================

void RA8876::Key_Scan_Wakeup_Function_Enable(void)
{
/*
Key-Scan Controller Register 2
[bit7]	
Key-Scan Wakeup Function Enable Bit
0: Key-Scan Wakeup function is disabled.
1: Key-Scan Wakeup function is enabled.
*/
	unsigned char temp;
	writeCmd(0xFC);
	temp=readData();
	temp |= cSetb7;
	writeData(temp);
}


void RA8876::Long_Key_Timing_Adjustment(unsigned char setx)
{
 /*Long Key Timing Adjustment*/ 
  unsigned char temp,temp1;
  temp = setx & 0x1c;
  writeCmd(0xFC);
  temp1 = readData();
  temp1|=temp;
  writeData(temp1);   
 
}

unsigned char RA8876::Numbers_of_Key_Hit(void)
{  
   unsigned char temp;
   writeCmd(0xFC);
   temp = readData();   //read key touch number
   temp = temp & 0x03;	 //�T�{���X�ӫ���Q���U
   return temp;
}

//[FDh][FEh][FFh]=========================================================================
unsigned char RA8876::Read_Key_Strobe_Data_0(void)
{
/*
Key Strobe Data 0
The corresponding key code 0 that is pressed.
*/
	unsigned char temp;
	writeCmd(0xFD);
	temp=readData();
	return temp;
}
unsigned char RA8876::Read_Key_Strobe_Data_1(void)
{
/*
Key Strobe Data 1
The corresponding key code 1 that is pressed.
*/
	unsigned char temp;
	writeCmd(0xFE);
	temp=readData();
	return temp;
}
unsigned char RA8876::Read_Key_Strobe_Data_2(void)
{
/*
Key Strobe Data 2
The corresponding key code 2 that is pressed.
*/
	unsigned char temp;
	writeCmd(0xFF);
	temp=readData();
	return temp;
}



/*------------------------------------//------------------------------------
//------------------------------------//----------------------------------*/

void RA8876::Set_LCD_Panel(void)
{
	#ifdef EJ080NA_04B	//##INNOLUX_1024x768_EJ080NA-04B

		//**[03h]**//
			LVDS_Format1();//(VESA format)//only for RA8877
		//	LVDS_Format2();//(JEIDA format) //only for RA8877

		//**[10h]**//
			Select_LCD_Sync_Mode();	// Enable XVSYNC, XHSYNC, XDE.
		//	Select_LCD_DE_Mode();	// XVSYNC & XHSYNC in idle state.

		//**[12h]**//
		//	PCLK_Rising();
			PCLK_Falling();

			VSCAN_T_to_B();
		//	VSCAN_B_to_T();

			PDATA_Set_RGB();
		//	PDATA_Set_RBG();
		//	PDATA_Set_GRB();
		//	PDATA_Set_GBR();
		//	PDATA_Set_BRG();
		//	PDATA_Set_BGR();
		//	PDATA_Set_Gray();
		//	PDATA_IDLE_STATE();

		//**[13h]**//
			HSYNC_Low_Active();
		//	HSYNC_High_Active();

			VSYNC_Low_Active();
		//	VSYNC_High_Active();
			
		//	DE_Low_Active();
			DE_High_Active();
	
			//**[14h][15h][1Ah][1Bh]**//
			LCD_HorizontalWidth_VerticalHeight(1024,768);// INNOLUX 1024x768�C
		
		/*	[16h][17h] : Figure 19-3 [HND] 
			[18h] :		 Figure 19-3 [HST] 
			[19h] :		 Figure 19-3 [HPW]	*/
			LCD_Horizontal_Non_Display(100);	//Non Display or Back-Porch //Blank=90~376, TYP.=320�C
			LCD_HSYNC_Start_Position(100);		//Start Position or Front-Porch
			LCD_HSYNC_Pulse_Width(120);			//Pulse Width

		/*	[1Ch][1Dh] : Figure 19-3 [VND]
			[1Eh] :		 Figure 19-3 [VST] 
			[1Fh] :		 Figure 19-3 [VPW]	*/
			LCD_Vertical_Non_Display(10);		//Non Display or Back-Porch  //Blank=10~77, TYP.=38�C
			LCD_VSYNC_Start_Position(10);		//Start Position or Front-Porch
			LCD_VSYNC_Pulse_Width(18);			//Pulse Width


			
	#endif

	#ifdef LQ150X1LGN2C	//##SHARP_LQ150X1LGN2C_LVDS1 & LVDS2

		//**[03h]**//
			LVDS_Format1();//(VESA format)//only for RA8877
		//	LVDS_Format2();//(JEIDA format) //only for RA8877

		//**[10h]**//
			Select_LCD_Sync_Mode();	// Enable XVSYNC, XHSYNC, XDE.
		//	Select_LCD_DE_Mode();	// XVSYNC & XHSYNC in idle state.

		//**[12h]**//
		//	PCLK_Rising();
			PCLK_Falling();

			VSCAN_T_to_B();
		//	VSCAN_B_to_T();

			PDATA_Set_RGB();
		//	PDATA_Set_RBG();
		//	PDATA_Set_GRB();
		//	PDATA_Set_GBR();
		//	PDATA_Set_BRG();
		//	PDATA_Set_BGR();
		//	PDATA_Set_Gray();
		//	PDATA_IDLE_STATE();

		//**[13h]**//
			HSYNC_Low_Active();
		//	HSYNC_High_Active();

			VSYNC_Low_Active();
		//	VSYNC_High_Active();
			
		//	DE_Low_Active();
			DE_High_Active();
	
		//**[14h][15h][1Ah][1Bh]**//
			LCD_HorizontalWidth_VerticalHeight(1024,768);//SHARP1024x768�C
		//**[16h][17h]**//
			LCD_Horizontal_Non_Display(30);//SHARP1024x768�ABlank=32~320~696�C
		//**[18h]**//
			LCD_HSYNC_Start_Position(10);//SHARP1024x768�C
		//**[19h]**//
			LCD_HSYNC_Pulse_Width(10);//SHARP1024x768�C
		//**[1Ch][1Dh]**//
			LCD_Vertical_Non_Display(20);//SHARP1024x768�ABlank=5~38~222�C
		//**[1Eh]**//
			LCD_VSYNC_Start_Position(10);//SHARP1024x768�C
		//**[1Fh]**//
			LCD_VSYNC_Pulse_Width(10);//SHARP1024x768�C
	#endif

	#ifdef N070ICG_LD1	//##INNOLUX_1280x800_N070ICG-LD1

		//**[03h]**//
			LVDS_Format1();//(VESA format)//only for RA8877
		//	LVDS_Format2();//(JEIDA format) //only for RA8877

		//**[10h]**//
			Select_LCD_Sync_Mode();	// Enable XVSYNC, XHSYNC, XDE.
			Select_LCD_DE_Mode();	// XVSYNC & XHSYNC in idle state.


		//**[12h]**//
		//	PCLK_Rising();
			PCLK_Falling();

			VSCAN_T_to_B();
		//	VSCAN_B_to_T();

			PDATA_Set_RGB();
		//	PDATA_Set_RBG();
		//	PDATA_Set_GRB();
		//	PDATA_Set_GBR();
		//	PDATA_Set_BRG();
		//	PDATA_Set_BGR();
		//	PDATA_Set_Gray();
		//	PDATA_IDLE_STATE();

		//**[13h]**//
			HSYNC_Low_Active();
		//	HSYNC_High_Active();

			VSYNC_Low_Active();
		//	VSYNC_High_Active();
			
		//	DE_Low_Active();
			DE_High_Active();
	
		//**[14h][15h][1Ah][1Bh]**//
			LCD_HorizontalWidth_VerticalHeight(1280,800);//
		//**[16h][17h]**//
			LCD_Horizontal_Non_Display(50);//
		//**[18h]**//
			LCD_HSYNC_Start_Position(50);//
		//**[19h]**//
			LCD_HSYNC_Pulse_Width(60);//
		//**[1Ch][1Dh]**//
			LCD_Vertical_Non_Display(8);//
		//**[1Eh]**//
			LCD_VSYNC_Start_Position(8);//
		//**[1Fh]**//
			LCD_VSYNC_Pulse_Width(8);//	


	#endif

	#ifdef B116XW03_V0	//##AUO1366x768_B116XW03 V0

		//**[03h]**//
			LVDS_Format1();//(VESA format)//only for RA8877
		//	LVDS_Format2();//(JEIDA format) //only for RA8877

		//**[10h]**//
			//Select_LCD_Sync_Mode();	// Enable XVSYNC, XHSYNC, XDE.
			Select_LCD_DE_Mode();	// XVSYNC & XHSYNC in idle state.

		//**[12h]**//
		//	PCLK_Rising();
			PCLK_Falling();

			VSCAN_T_to_B();
		//	VSCAN_B_to_T();

			PDATA_Set_RGB();
		//	PDATA_Set_RBG();
		//	PDATA_Set_GRB();
		//	PDATA_Set_GBR();
		//	PDATA_Set_BRG();
		//	PDATA_Set_BGR();
		//	PDATA_Set_Gray();
		//	PDATA_IDLE_STATE();

		//**[13h]**//
		//	HSYNC_Low_Active();
		//	HSYNC_High_Active();

		//	VSYNC_Low_Active();
		//	VSYNC_High_Active();
			
		//	DE_Low_Active();
			DE_High_Active();
	
		//**[14h][15h][1Ah][1Bh]**//
			LCD_HorizontalWidth_VerticalHeight(1366,768);//
		//**[16h][17h]**//
			LCD_Horizontal_Non_Display(15);//
		//**[18h]**//
			LCD_HSYNC_Start_Position(15);//
		//**[19h]**//
			LCD_HSYNC_Pulse_Width(44);//
		//**[1Ch][1Dh]**//
			LCD_Vertical_Non_Display(8);//
		//**[1Eh]**//
			LCD_VSYNC_Start_Position(8);//
		//**[1Fh]**//
			LCD_VSYNC_Pulse_Width(8);//
	#endif
		
	#ifdef LQ121S1LG81	//##SHARP_LQ121S1LG81

		//**[03h]**//
			LVDS_Format1();//(VESA format)//only for RA8877
		//	LVDS_Format2();//(JEIDA format) //only for RA8877

		//**[10h]**//
			Select_LCD_Sync_Mode();	// Enable XVSYNC, XHSYNC, XDE.
		//	Select_LCD_DE_Mode();	// XVSYNC & XHSYNC in idle state.

		//**[12h]**//
		//	PCLK_Rising();
			PCLK_Falling();

			VSCAN_T_to_B();
		//	VSCAN_B_to_T();

			PDATA_Set_RGB();
		//	PDATA_Set_RBG();
		//	PDATA_Set_GRB();
		//	PDATA_Set_GBR();
		//	PDATA_Set_BRG();
		//	PDATA_Set_BGR();
		//	PDATA_Set_Gray();
		//	PDATA_IDLE_STATE();

		//**[13h]**//
			HSYNC_Low_Active();
		//	HSYNC_High_Active();

			VSYNC_Low_Active();
		//	VSYNC_High_Active();
			
		//	DE_Low_Active();
			DE_High_Active();
	
		//**[14h][15h][1Ah][1Bh]**//
			LCD_HorizontalWidth_VerticalHeight(800,600);//
		//**[16h][17h]**//
			LCD_Horizontal_Non_Display(88);//
		//**[18h]**//
			LCD_HSYNC_Start_Position(40);//
		//**[19h]**//
			LCD_HSYNC_Pulse_Width(128);//
		//**[1Ch][1Dh]**//
			LCD_Vertical_Non_Display(23);//
		//**[1Eh]**//
			LCD_VSYNC_Start_Position(39);//
		//**[1Fh]**//
			LCD_VSYNC_Pulse_Width(4);//
			
	#endif


	#ifdef LQ035NC111

		//**[10h]**//
			Select_LCD_Sync_Mode();	// Enable XVSYNC, XHSYNC, XDE.
		//	Select_LCD_DE_Mode();	// XVSYNC & XHSYNC in idle state.

		//PCLK_Falling();
		PCLK_Rising();

		VSCAN_T_to_B();
		PDATA_Set_RGB();


		//HSYNC_High_Active();
		//VSYNC_High_Active();

		HSYNC_Low_Active();
		VSYNC_Low_Active();
		//DE_High_Active();
		DE_Low_Active();

		LCD_HorizontalWidth_VerticalHeight(320,240);//

		LCD_Horizontal_Non_Display(44);//
		LCD_HSYNC_Start_Position(20);//
		LCD_HSYNC_Pulse_Width(24);//	non_display + pulse width = 68

		LCD_Vertical_Non_Display(15);//
		LCD_VSYNC_Start_Position(2);//
		LCD_VSYNC_Pulse_Width(1);//
		
		
		  
	#endif


	#ifdef AT080TN52

		//**[10h]**//
			Select_LCD_Sync_Mode();	// Enable XVSYNC, XHSYNC, XDE.
		//	Select_LCD_DE_Mode();	// XVSYNC & XHSYNC in idle state.
		
		PCLK_Falling();
		//PCLK_Rising();

		VSCAN_T_to_B();
		PDATA_Set_RGB();

		HSYNC_Low_Active();
		VSYNC_Low_Active();
		DE_High_Active();
		//DE_Low_Active();

		LCD_HorizontalWidth_VerticalHeight(800,600);//INNOLUX 800x600�C
		LCD_Horizontal_Non_Display(46);//INNOLUX800x600�A46�C
		LCD_HSYNC_Start_Position(16);//INNOLUX800x600�A16~354�C
		LCD_HSYNC_Pulse_Width(3);//INNOLUX800x600�A1~40�C
		LCD_Vertical_Non_Display(23);//INNOLUX800x600�A23�C
		LCD_VSYNC_Start_Position(3);//INNOLUX800x600�A1~77�C
		LCD_VSYNC_Pulse_Width(3);//INNOLUX800x600�A1~20�C

	#endif

	#ifdef AT070TN92

		//**[10h]**//
			Select_LCD_Sync_Mode();	// Enable XVSYNC, XHSYNC, XDE.
		//	Select_LCD_DE_Mode();	// XVSYNC & XHSYNC in idle state.
		
		PCLK_Falling();
		//PCLK_Rising();

		VSCAN_T_to_B();
		PDATA_Set_RGB();

		HSYNC_Low_Active();
		VSYNC_Low_Active();
		DE_High_Active();
		//DE_Low_Active();

		LCD_HorizontalWidth_VerticalHeight(800,480);//INNOLUX 800x480�C
		LCD_Horizontal_Non_Display(46);//INNOLUX800x600�A46�C
		LCD_HSYNC_Start_Position(210);//INNOLUX800x600�A16~354�C
		LCD_HSYNC_Pulse_Width(10);//INNOLUX800x600�A1~40�C
		LCD_Vertical_Non_Display(23);//INNOLUX800x600�A23�C
		LCD_VSYNC_Start_Position(22);//INNOLUX800x600�A1~147�C
		LCD_VSYNC_Pulse_Width(10);//INNOLUX800x600�A1~20�C

	#endif
	#ifdef MD070STN92	//##EDT_1024x600_MD070STN92

		//**[03h]**//
			LVDS_Format1();//(VESA format)//only for RA8877
		//	LVDS_Format2();//(JEIDA format) //only for RA8877

		//**[10h]**//
			Select_LCD_Sync_Mode();	// Enable XVSYNC, XHSYNC, XDE.
		//	Select_LCD_DE_Mode();	// XVSYNC & XHSYNC in idle state.

		//**[12h]**//
		//	PCLK_Rising();
			PCLK_Falling();

			
			VSCAN_T_to_B();
		//	VSCAN_B_to_T();

			PDATA_Set_RGB();
		//	PDATA_Set_RBG();
		//	PDATA_Set_GRB();
		//	PDATA_Set_GBR();
		//	PDATA_Set_BRG();
		//	PDATA_Set_BGR();
		//	PDATA_Set_Gray();
		//	PDATA_IDLE_STATE();

		//**[13h]**//
			HSYNC_Low_Active();
		//	HSYNC_High_Active();

			VSYNC_Low_Active();
		//	VSYNC_High_Active();
			
		//	DE_Low_Active();
			DE_High_Active();
	
		//**[14h][15h][1Ah][1Bh]**//
			LCD_HorizontalWidth_VerticalHeight(1024,600);//EDT_1024x600�C
		//**[16h][17h]**//
			LCD_Horizontal_Non_Display(46);//EDT_1024x600�ABlank=90~376@320�C
		//**[18h]**//
			LCD_HSYNC_Start_Position(210);//EDT_1024x600�ABlank=90~376@320�C
		//**[19h]**//
			LCD_HSYNC_Pulse_Width(30);//EDT_1024x600�ABlank=90~376@320�C
		//**[1Ch][1Dh]**//
			LCD_Vertical_Non_Display(23);//EDT_1024x600�ABlank=10~200@35�C
		//**[1Eh]**//
			LCD_VSYNC_Start_Position(22);//EDT_1024x600�ABlank=10~200@35�C
		//**[1Fh]**//
			LCD_VSYNC_Pulse_Width(5);//EDT_1024x600�ABlank=10~200@35�C	
			
	#endif


	#ifdef ET101000DM6	//##EDT_1024x600_ET101000DM6

		//**[03h]**//
			LVDS_Format1();//(VESA format)//only for RA8877
		//	LVDS_Format2();//(JEIDA format) //only for RA8877

		//**[10h]**//
			Select_LCD_Sync_Mode();	// Enable XVSYNC, XHSYNC, XDE.
		//	Select_LCD_DE_Mode();	// XVSYNC & XHSYNC in idle state.

		//**[12h]**//
		//	PCLK_Rising();
			PCLK_Falling();

			VSCAN_T_to_B();
		//	VSCAN_B_to_T();

			PDATA_Set_RGB();
		//	PDATA_Set_RBG();
		//	PDATA_Set_GRB();
		//	PDATA_Set_GBR();
		//	PDATA_Set_BRG();
		//	PDATA_Set_BGR();
		//	PDATA_Set_Gray();
		//	PDATA_IDLE_STATE();

		//**[13h]**//
			HSYNC_Low_Active();
		//	HSYNC_High_Active();

			VSYNC_Low_Active();
		//	VSYNC_High_Active();
			
		//	DE_Low_Active();
			DE_High_Active();
	
		//**[14h][15h][1Ah][1Bh]**//
			LCD_HorizontalWidth_VerticalHeight(1024,600);//EDT_1024x600�C
		//**[16h][17h]**//
			LCD_Horizontal_Non_Display(160);//EDT_1024x600�ABlank=90~376@320�C
		//**[18h]**//
			LCD_HSYNC_Start_Position(160);//EDT_1024x600�ABlank=90~376@320�C
		//**[19h]**//
			LCD_HSYNC_Pulse_Width(60);//EDT_1024x600�ABlank=90~376@320�C
		//**[1Ch][1Dh]**//
			LCD_Vertical_Non_Display(23);//EDT_1024x600�ABlank=10~200@35�C
		//**[1Eh]**//
			LCD_VSYNC_Start_Position(12);//EDT_1024x600�ABlank=10~200@35�C
		//**[1Fh]**//
			LCD_VSYNC_Pulse_Width(5);//EDT_1024x600�ABlank=10~200@35�C	
			
	#endif


	#ifdef G190SVT01	//##AUO_1680x342_G190SVT01

		//**[10h]**//
			Select_LCD_Sync_Mode();	// Enable XVSYNC, XHSYNC, XDE.
		//	Select_LCD_DE_Mode();	// XVSYNC & XHSYNC in idle state.

 		//**[12h]**//
		//	PCLK_Rising();
			PCLK_Falling();

			VSCAN_T_to_B();
		//	VSCAN_B_to_T();

			PDATA_Set_RGB();
		//	PDATA_Set_RBG();
		//	PDATA_Set_GRB();
		//	PDATA_Set_GBR();
		//	PDATA_Set_BRG();
		//	PDATA_Set_BGR();
		//	PDATA_Set_Gray();
		//	PDATA_IDLE_STATE();

		//**[13h]**//
			HSYNC_Low_Active();
		//	HSYNC_High_Active();

			VSYNC_Low_Active();
		//	VSYNC_High_Active();
			
		//	DE_Low_Active();
			DE_High_Active();
	
		//**[14h][15h][1Ah][1Bh]**//
			LCD_HorizontalWidth_VerticalHeight(1680,350);//1680x342
		//**[16h][17h]**//
			LCD_Horizontal_Non_Display(30);//Blank=288�C
		//**[18h]**//
			LCD_HSYNC_Start_Position(30);//
		//**[19h]**//
			LCD_HSYNC_Pulse_Width(30);//
		//**[1Ch][1Dh]**//
			LCD_Vertical_Non_Display(6);//Blank=16�C
		//**[1Eh]**//
			LCD_VSYNC_Start_Position(6);//
		//**[1Fh]**//
			LCD_VSYNC_Pulse_Width(6);//	    
			
	#endif


	#ifdef ZJ070NA_01B	//##INNOLUX_1024x600_ZJ070NA_01B

		//**[03h]**//
			LVDS_Format1();//(VESA format)//only for RA8877
		//	LVDS_Format2();//(JEIDA format) //only for RA8877

		//**[10h]**//
			Select_LCD_Sync_Mode();	// Enable XVSYNC, XHSYNC, XDE.
		//	Select_LCD_DE_Mode();	// XVSYNC & XHSYNC in idle state.

 		//**[12h]**//
		//	PCLK_Rising();
			PCLK_Falling();

			VSCAN_T_to_B();
		//	VSCAN_B_to_T();

			PDATA_Set_RGB();
		//	PDATA_Set_RBG();
		//	PDATA_Set_GRB();
		//	PDATA_Set_GBR();
		//	PDATA_Set_BRG();
		//	PDATA_Set_BGR();
		//	PDATA_Set_Gray();
		//	PDATA_IDLE_STATE();

		//**[13h]**//
			HSYNC_Low_Active();
		//	HSYNC_High_Active();

			VSYNC_Low_Active();
		//	VSYNC_High_Active();
			
		//	DE_Low_Active();
			DE_High_Active();
	
		//**[14h][15h][1Ah][1Bh]**//
			LCD_HorizontalWidth_VerticalHeight(1024,600);//1024x600
		//**[16h][17h]**//
			LCD_Horizontal_Non_Display(50);//Blank=320�C
		//**[18h]**//
			LCD_HSYNC_Start_Position(50);//
		//**[19h]**//
			LCD_HSYNC_Pulse_Width(220);//
		//**[1Ch][1Dh]**//
			LCD_Vertical_Non_Display(8);//Blank=35�C
		//**[1Eh]**//
			LCD_VSYNC_Start_Position(7);//
		//**[1Fh]**//
			LCD_VSYNC_Pulse_Width(20);//		    
			
	#endif

	#ifdef LQ190E1LW52	//##SHARP_1280x1024_LQ190E1LW52

		//**[10h]**//
			Select_LCD_Sync_Mode();	// Enable XVSYNC, XHSYNC, XDE.
		//	Select_LCD_DE_Mode();	// XVSYNC & XHSYNC in idle state.

 		//**[12h]**//
		//	PCLK_Rising();
			PCLK_Falling();

			VSCAN_T_to_B();
		//	VSCAN_B_to_T();

			PDATA_Set_RGB();
		//	PDATA_Set_RBG();
		//	PDATA_Set_GRB();
		//	PDATA_Set_GBR();
		//	PDATA_Set_BRG();
		//	PDATA_Set_BGR();
		//	PDATA_Set_Gray();
		//	PDATA_IDLE_STATE();

		//**[13h]**//
			HSYNC_Low_Active();
		//	HSYNC_High_Active();

			VSYNC_Low_Active();
		//	VSYNC_High_Active();
			
		//	DE_Low_Active();
			DE_High_Active();
	
		//**[14h][15h][1Ah][1Bh]**//
			LCD_HorizontalWidth_VerticalHeight(1280,1024);//1024x600
		//**[16h][17h]**//
			LCD_Horizontal_Non_Display(110);//Blank=204x2�C
		//**[18h]**//
			LCD_HSYNC_Start_Position(110);//
		//**[19h]**//
			LCD_HSYNC_Pulse_Width(110);//
		//**[1Ch][1Dh]**//
			LCD_Vertical_Non_Display(15);//Blank=42�C
		//**[1Eh]**//
			LCD_VSYNC_Start_Position(15);//
		//**[1Fh]**//
			LCD_VSYNC_Pulse_Width(12);//	    
			
	#endif	

	#ifdef HJ070IA_02F	//##INNOLUX_1280x800_HJ070IA_02F

		//**[03h]**//
			LVDS_Format1();//(VESA format)//only for RA8877
		//	LVDS_Format2();//(JEIDA format) //only for RA8877

		//**[10h]**//
			Select_LCD_Sync_Mode();	// Enable XVSYNC, XHSYNC, XDE.
		//	Select_LCD_DE_Mode();	// XVSYNC & XHSYNC in idle state.

 		//**[12h]**//
		//	PCLK_Rising();
			PCLK_Falling();

			VSCAN_T_to_B();
		//	VSCAN_B_to_T();

			PDATA_Set_RGB();
		//	PDATA_Set_RBG();
		//	PDATA_Set_GRB();
		//	PDATA_Set_GBR();
		//	PDATA_Set_BRG();
		//	PDATA_Set_BGR();
		//	PDATA_Set_Gray();
		//	PDATA_IDLE_STATE();

		//**[13h]**//
			HSYNC_Low_Active();
		//	HSYNC_High_Active();

			VSYNC_Low_Active();
		//	VSYNC_High_Active();
			
		//	DE_Low_Active();
			DE_High_Active();
	
		//**[14h][15h][1Ah][1Bh]**//
			LCD_HorizontalWidth_VerticalHeight(1280,800);//1280x800
		//**[16h][17h]**//
			LCD_Horizontal_Non_Display(30);//Blank=160�C
		//**[18h]**//
			LCD_HSYNC_Start_Position(30);//
		//**[19h]**//
			LCD_HSYNC_Pulse_Width(100);//
		//**[1Ch][1Dh]**//
			LCD_Vertical_Non_Display(3);//Blank=23�C
		//**[1Eh]**//
			LCD_VSYNC_Start_Position(3);//
		//**[1Fh]**//
			LCD_VSYNC_Pulse_Width(18);//	    
	#endif


	#ifdef LQ156M1LG21	//##SHARP_1920x1080_LQ156M1LG21

		//**[03h]**//
			LVDS_Format1();//(VESA format)//only for RA8877
		//	LVDS_Format2();//(JEIDA format) //only for RA8877

		//**[10h]**//
			Select_LCD_Sync_Mode();	// Enable XVSYNC, XHSYNC, XDE.
		//	Select_LCD_DE_Mode();	// XVSYNC & XHSYNC in idle state.

 		//**[12h]**//
		//	PCLK_Rising();
			PCLK_Falling();

			VSCAN_T_to_B();
		//	VSCAN_B_to_T();

			PDATA_Set_RGB();
		//	PDATA_Set_RBG();
		//	PDATA_Set_GRB();
		//	PDATA_Set_GBR();
		//	PDATA_Set_BRG();
		//	PDATA_Set_BGR();
		//	PDATA_Set_Gray();
		//	PDATA_IDLE_STATE();

		//**[13h]**//
		//	HSYNC_Low_Active();
			HSYNC_High_Active();

		//	VSYNC_Low_Active();
			VSYNC_High_Active();
			
		//	DE_Low_Active();
			DE_High_Active();
	
		//**[14h][15h][1Ah][1Bh]**//
			LCD_HorizontalWidth_VerticalHeight(1920,1080);//1280x800
		//**[16h][17h]**//
			LCD_Horizontal_Non_Display(40);//Blank min.=140�C
		//**[18h]**//
			LCD_HSYNC_Start_Position(20);//
		//**[19h]**//
			LCD_HSYNC_Pulse_Width(100);//
		//**[1Ch][1Dh]**//
			LCD_Vertical_Non_Display(3);//Blank min.=31�C
		//**[1Eh]**//
			LCD_VSYNC_Start_Position(3);//
		//**[1Fh]**//
			LCD_VSYNC_Pulse_Width(20);//	    
	#endif

	#ifdef LQ201U1LW32	//##SHARP_1600x1200_LQ201U1LW32

		//**[03h]**//
			LVDS_Format1();//(VESA format)//only for RA8877
		//	LVDS_Format2();//(JEIDA format) //only for RA8877

		//**[10h]**//
			Select_LCD_Sync_Mode();	// Enable XVSYNC, XHSYNC, XDE.
		//	Select_LCD_DE_Mode();	// XVSYNC & XHSYNC in idle state.

 		//**[12h]**//
		//	PCLK_Rising();
			PCLK_Falling();

			VSCAN_T_to_B();
		//	VSCAN_B_to_T();

			PDATA_Set_RGB();
		//	PDATA_Set_RBG();
		//	PDATA_Set_GRB();
		//	PDATA_Set_GBR();
		//	PDATA_Set_BRG();
		//	PDATA_Set_BGR();
		//	PDATA_Set_Gray();
		//	PDATA_IDLE_STATE();

		//**[13h]**//
		//	HSYNC_Low_Active();
			HSYNC_High_Active();

		//	VSYNC_Low_Active();
			VSYNC_High_Active();
			
		//	DE_Low_Active();
			DE_High_Active();
	
		//**[14h][15h][1Ah][1Bh]**//
			LCD_HorizontalWidth_VerticalHeight(1600,1200);//
		//**[16h][17h]**//
			LCD_Horizontal_Non_Display(15);
		//**[18h]**//
			LCD_HSYNC_Start_Position(15);
		//**[19h]**//
			LCD_HSYNC_Pulse_Width(40);
		//**[1Ch][1Dh]**//
			LCD_Vertical_Non_Display(5);
		//**[1Eh]**//
			LCD_VSYNC_Start_Position(5);
		//**[1Fh]**//
			LCD_VSYNC_Pulse_Width(5);
	#endif
}


	 
void RA8876::Set_Serial_Flash_IF(void)
{

//(*) Using Serial Flash
	#ifdef MX25L25635E
		Enable_SFlash_SPI();
	#endif

//(*) Using GT_ROM
	#ifdef GT21L16T1W
		GTFont_Select_GT21L16T1W();
		Enable_SFlash_SPI();
	#endif
	#ifdef GT30L16U2W
		GTFont_Select_GT30L16U2W();
		Enable_SFlash_SPI();
	#endif
	#ifdef GT30L24T3Y
		GTFont_Select_GT30L24T3Y();
		Enable_SFlash_SPI();
	#endif
	#ifdef GT30L24M1Z	
		GTFont_Select_GT30L24M1Z();
		Enable_SFlash_SPI();
	#endif
	#ifdef GT30L32S4W
		GTFont_Select_GT30L32S4W();
		Enable_SFlash_SPI();
	#endif
	#ifdef GT20L24F6Y
		GTFont_Select_GT20L24F6Y();
		Enable_SFlash_SPI();
	#endif
	#ifdef GT21L24S1W
		GTFont_Select_GT21L24S1W();
		Enable_SFlash_SPI();
	#endif

//(*) Set RA8876/77 Master SPI address width. 
	Select_SFI_24bit_Address();
	//Select_SFI_32bit_Address();
	
//(*)
	Select_standard_SPI_Mode0_or_Mode3();
	//Select_RA8875_SPI_Mode0_and_Mode3();
	
//(*)
//	Select_SFI_Single_Mode_Dummy_0T_03h();
	Select_SFI_Single_Mode_Dummy_8T_0Bh();
//	Select_SFI_Single_Mode_Dummy_16T_1Bh();
//	Select_SFI_Dual_Mode_Dummy_8T_3Bh();
//	Select_SFI_Dual_Mode_Dummy_4T_BBh();

//(*)
	Reset_CPOL();
//	Set_CPOL();
	Reset_CPHA();
//	Set_CPHA();

/******************************/
}


 
void RA8876::RA8876_SDRAM_initial(void)
{
unsigned char	CAS_Latency;
unsigned short	Auto_Refresh;
//
//Auto_Refresh=(64*DRAM_FREQ*1000)/(8192);

#ifdef RA8876M
 if(DRAM_FREQ<=133) CAS_Latency=2;
 else     CAS_Latency=3;
 
 Auto_Refresh=(64*DRAM_FREQ*1000)/(4096);
 Auto_Refresh=Auto_Refresh-2;//���e�Ұ�refresh�A�קK��n�I�췥���d��C// Start [refresh] in advance to avoid just reaching the limits.
 
 writeReg(0xe0,0x28); 
 writeReg(0xe1,CAS_Latency);
 writeReg(0xe2,Auto_Refresh);
 writeReg(0xe3,Auto_Refresh>>8);
 writeReg(0xe4,0x01);
#endif

#ifdef IS42SM16160D

	if(DRAM_FREQ<=133)	CAS_Latency=2;
	else 				CAS_Latency=3;

	Auto_Refresh=(64*DRAM_FREQ*1000)/(8192) - 2;
	
	writeReg(0xe0,0xf9);        
	writeReg(0xe1,CAS_Latency);      //CAS:2=0x02�ACAS:3=0x03
	writeReg(0xe2,Auto_Refresh);
	writeReg(0xe3,Auto_Refresh>>8);
	writeReg(0xe4,0x09);
#endif

#ifdef IS42S16320B
	
	if(DRAM_FREQ<=133)	CAS_Latency=2;
	else 				CAS_Latency=3;
	
	Auto_Refresh=(64*DRAM_FREQ*1000)/(8192) - 2;
	
	writeReg(0xe0,0x32);	
	writeReg(0xe1,CAS_Latency);
	writeReg(0xe2,Auto_Refresh);
	writeReg(0xe3,Auto_Refresh>>8);
	writeReg(0xe4,0x09);

#endif

#ifdef IS42S16400F

	if(DRAM_FREQ<143)	CAS_Latency=2;
	else 				CAS_Latency=3;

	Auto_Refresh=(64*DRAM_FREQ*1000)/(4096) - 2;
	
	writeReg(0xe0,0x28);      
	writeReg(0xe1,CAS_Latency);      //CAS:2=0x02�ACAS:3=0x03
	writeReg(0xe2,Auto_Refresh);
	writeReg(0xe3,Auto_Refresh>>8);
	writeReg(0xe4,0x01);
#endif

#ifdef M12L32162A
	CAS_Latency=3;
	Auto_Refresh=(64*DRAM_FREQ*1000)/(4096) - 2;
	
	writeReg(0xe0,0x08);      
	writeReg(0xe1,CAS_Latency);      //CAS:2=0x02�ACAS:3=0x03
	writeReg(0xe2,Auto_Refresh);
	writeReg(0xe3,Auto_Refresh>>8);
	writeReg(0xe4,0x09);
#endif

#ifdef M12L2561616A
	CAS_Latency=3;	
	Auto_Refresh=(64*DRAM_FREQ*1000)/(8192) - 2;
	
	writeReg(0xe0,0x31);      
	writeReg(0xe1,CAS_Latency);      //CAS:2=0x02�ACAS:3=0x03
	writeReg(0xe2,Auto_Refresh);
	writeReg(0xe3,Auto_Refresh>>8);
	writeReg(0xe4,0x01);
#endif

#ifdef M12L64164A
	CAS_Latency=3;
	Auto_Refresh=(64*DRAM_FREQ*1000)/(4096) - 2;
	
	writeReg(0xe0,0x28);      
	writeReg(0xe1,CAS_Latency);      //CAS:2=0x02�ACAS:3=0x03
	writeReg(0xe2,Auto_Refresh);
	writeReg(0xe3,Auto_Refresh>>8);
	writeReg(0xe4,0x09);
#endif

#ifdef W9825G6JH
	CAS_Latency=3;
	Auto_Refresh=(64*DRAM_FREQ*1000)/(8192) - 2;
	
	writeReg(0xe0,0x31);      
	writeReg(0xe1,CAS_Latency);      //CAS:2=0x02�ACAS:3=0x03
	writeReg(0xe2,Auto_Refresh);
	writeReg(0xe3,Auto_Refresh>>8);
	writeReg(0xe4,0x01);
#endif

#ifdef W9812G6JH
	CAS_Latency=3;
	Auto_Refresh=(64*DRAM_FREQ*1000)/(4096) - 2;
	
	writeReg(0xe0,0x29);      
	writeReg(0xe1,CAS_Latency);      //CAS:2=0x02�ACAS:3=0x03
	writeReg(0xe2,Auto_Refresh);
	writeReg(0xe3,Auto_Refresh>>8);
	writeReg(0xe4,0x01);
#endif

#ifdef MT48LC4M16A
	CAS_Latency=3;
	Auto_Refresh=(64*DRAM_FREQ*1000)/(4096) - 2;
	
	writeReg(0xe0,0x28);      
	writeReg(0xe1,CAS_Latency);      //CAS:2=0x02�ACAS:3=0x03
	writeReg(0xe2,Auto_Refresh);
	writeReg(0xe3,Auto_Refresh>>8);
	writeReg(0xe4,0x01);
#endif

#ifdef K4S641632N
	CAS_Latency=3;
	Auto_Refresh=(64*DRAM_FREQ*1000)/(4096) - 2;
	
	writeReg(0xe0,0x28);      
	writeReg(0xe1,CAS_Latency);      //CAS:2=0x02�ACAS:3=0x03
	writeReg(0xe2,Auto_Refresh);
	writeReg(0xe3,Auto_Refresh>>8);
	writeReg(0xe4,0x01);
#endif

#ifdef K4S281632K
	CAS_Latency=3;
	Auto_Refresh=(64*DRAM_FREQ*1000)/(4096) - 2;
	
	writeReg(0xe0,0x29);      
	writeReg(0xe1,CAS_Latency);      //CAS:2=0x02�ACAS:3=0x03
	writeReg(0xe2,Auto_Refresh);
	writeReg(0xe3,Auto_Refresh>>8);
	writeReg(0xe4,0x01);
#endif

#ifdef IS42S16100
	CAS_Latency=3;
	Auto_Refresh=(32*DRAM_FREQ*1000)/(2048) - 2;
	
	writeReg(0xe0,0x00);      
	writeReg(0xe1,CAS_Latency);      //CAS:2=0x02�ACAS:3=0x03
	writeReg(0xe2,Auto_Refresh);
	writeReg(0xe3,Auto_Refresh>>8);
	writeReg(0xe4,0x01);
#endif

	Check_SDRAM_Ready();


}

/*------------------------------------//------------------------------------
//------------------------------------//----------------------------------*/
void RA8876::RA8876_PLL(void) 
{
/*
(1) 10MHz <= OSC_FREQ <= 15MHz
(2) 10MHz <= (OSC_FREQ/PLLDIVM) <= 40MHz
(3) 250MHz <= [OSC_FREQ/(PLLDIVM+1)]x(PLLDIVN+1) <= 500MHz
 
PLLDIVM:0
PLLDIVN:1~63
PLLDIVK:CPLL & MPLL = 1/2/4/8.SPLL = 1/2/4/8/16/32/64/128.
 
ex:
 OSC_FREQ = 10MHz
 Set X_DIVK=2
 Set X_DIVM=0
 => (X_DIVN+1)=(XPLLx4)/10
*/
 
unsigned short x_Divide,PLLC1,PLLC2;
unsigned short pll_m_lo, pll_m_hi;
unsigned char temp;
 

  // Set pixel clock
  if(SCAN_FREQ>=63)        //&&(SCAN_FREQ<=100))
  {
   writeReg(0x05,0x04);    //PLL Divided by 4
   writeReg(0x06,(SCAN_FREQ*4/OSC_FREQ)-1);
  }
  if((SCAN_FREQ>=32)&&(SCAN_FREQ<=62))
  {           
   writeReg(0x05,0x06);    //PLL Divided by 8
   writeReg(0x06,(SCAN_FREQ*8/OSC_FREQ)-1);
  }
  if((SCAN_FREQ>=16)&&(SCAN_FREQ<=31))
  {           
   writeReg(0x05,0x16);    //PLL Divided by 16
   writeReg(0x06,(SCAN_FREQ*16/OSC_FREQ)-1);
  }
  if((SCAN_FREQ>=8)&&(SCAN_FREQ<=15))
  {
   writeReg(0x05,0x26);    //PLL Divided by 32
   writeReg(0x06,(SCAN_FREQ*32/OSC_FREQ)-1);
  }
  if((SCAN_FREQ>0)&&(SCAN_FREQ<=7))
  {
   writeReg(0x05,0x36);    //PLL Divided by 64
   writeReg(0x06,(SCAN_FREQ*64/OSC_FREQ)-1);
  }            
 
  
  // Set SDRAM clock
  if(DRAM_FREQ>=125)        //&&(DRAM_FREQ<=166))
  {
   writeReg(0x07,0x02);    //PLL Divided by 2
   writeReg(0x08,(DRAM_FREQ*2/OSC_FREQ)-1);
  }
  if((DRAM_FREQ>=63)&&(DRAM_FREQ<=124))   //&&(DRAM_FREQ<=166)
  {
   writeReg(0x07,0x04);    //PLL Divided by 4
   writeReg(0x08,(DRAM_FREQ*4/OSC_FREQ)-1);
  }
  if((DRAM_FREQ>=32)&&(DRAM_FREQ<=62))
  {           
   writeReg(0x07,0x06);    //PLL Divided by 8
   writeReg(0x08,(DRAM_FREQ*8/OSC_FREQ)-1);
  }

 

  // Set Core clock
  if(CORE_FREQ>=125)
  {
   writeReg(0x09,0x02);    //PLL Divided by 2
   writeReg(0x0A,(CORE_FREQ*2/OSC_FREQ)-1);
  }
  if((CORE_FREQ>=63)&&(CORE_FREQ<=124))     
  {
   writeReg(0x09,0x04);    //PLL Divided by 4
   writeReg(0x0A,(CORE_FREQ*4/OSC_FREQ)-1);
  }
  if((CORE_FREQ>=32)&&(CORE_FREQ<=62))
  {           
   writeReg(0x09,0x06);    //PLL Divided by 8
   writeReg(0x0A,(CORE_FREQ*8/OSC_FREQ)-1);
  }

 
 Enable_PLL();
 delay(100);
 
}




unsigned char RA8876::Chk_Key_Press(void)
{
  unsigned char temp;
writeCmd(0x0C);
temp = readData();

	if ((temp&0x08)==0x08)
	return 1;
	else 
	return 0;

}
