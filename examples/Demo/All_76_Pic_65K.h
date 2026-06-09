/*********************************************************************/
//
//	brief: This file provides an easy way to obtain image data information.
//
//
//	BINARY_INFO[PICTURE_NAME].number      => To obtain sequence number information of the "PICTURE_NAME"(.bmp or .jpg) which is generated in the excel file
//	BINARY_INFO[PICTURE_NAME].img_width       => To obtain image width information of the "PICTURE_NAME" (.bmp or .jpg)
//	BINARY_INFO[PICTURE_NAME].img_height      => To obtain image height information of the "PICTURE_NAME" (.bmp or .jpg)
//	BINARY_INFO[PICTURE_NAME].img_size        => To obtain an image size which is converted and stored in the binary file, please notice, the binary file is combined and converted from the several files (.bmp or .jpg)
//	BINARY_INFO[PICTURE_NAME].start_addr  => To obtain start address of binary file which is converted from the file "PICTURE_NAME" (.bmp or .jpg)
//
//	For example, the struct and enum type as:
//
//	const INFO (code) BINARY_INFO[3]=
//	{
//  	/*  No. , Width , Height , Size (bytes) , Start Address  */
//  	{1,1280,800,2048000,0},          /*     pic_01_1280x800 , element 0     */
//  	{2,320,240,153600,2048000},      /*     RAiO , element 1     */
//  	{3,128,128,32768,2201600},       /*     S1_16 , element 2     */
//	}
//
//
//  typedef enum
//  {
//    pic_01_1280x800=0,  /*     0     */
//    RAiO,               /*     1     */
//    S1_16,              /*     2     */
//  }PICTURE_NAME;
//
//	(1).  To obtain size informations of the file "pic_01_1280x800" (.bmp or jpg),
//        the C code is:
//
//									long param1 = 0;
//
//									param1 = BINARY_INFO[pic_01_1280x800].img_size;
//									/*  or  */
//									param1 = BINARY_INFO[0].img_size;
//
//									/* the param1 is 2048000 (bytes) */
//
//
//	(2).  To obtain start address informations of the file "S1_16" (.bmp or jpg),
//        the C code is:
//
//									long param2 = 0;
//
//									param2 = BINARY_INFO[S1_16].start_addr;
//									/*  or  */
//									param2 = BINARY_INFO[2].start_addr;
//
//									/* the param2 is 2201600 (bytes) */
//
/*********************************************************************/
typedef struct _info
{
  unsigned short number;
  unsigned short img_width;
  unsigned short img_height;
  unsigned long img_size;
  unsigned long start_addr;
}INFO;

  /* The 'code' is KEIL C 8051 instruction, please refer to http://www.keil.com/support/man/docs/c51/c51_le_code.htm */
  /* If you do not use the 8051 microcontroller system, please remove the 'code' instruction. */

const INFO code BINARY_INFO[14]=
{
  /*  No. , Width , Height , Size , Start Address  */ 
  {1,1024,600,1228800,0},          /*     PIC_MAIN_BACKGROUND , element 0     */
  {2,466,403,375596,1228800},          /*     PIC_KEYPAD , element 1     */
  {3,140,140,39200,1604396},          /*     PIC_1 , element 2     */
  {4,140,140,39200,1643596},          /*     PIC_2 , element 3     */
  {5,140,140,39200,1682796},          /*     PIC_3 , element 4     */
  {6,140,140,39200,1721996},          /*     PIC_4 , element 5     */
  {7,140,140,39200,1761196},          /*     PIC_5 , element 6     */
  {8,140,140,39200,1800396},          /*     PIC_6 , element 7     */
  {9,140,140,39200,1839596},          /*     PIC_7 , element 8     */
  {10,140,140,39200,1878796},          /*     PIC_8 , element 9     */
  {11,140,140,39200,1917996},          /*     PIC_9 , element 10     */
  {12,190,70,26600,1957196},          /*     LOGO , element 11     */
  {13,45,45,4050,1983796},          /*     MESCOLAMENTO_45 , element 12     */
  {14,45,45,4050,1987846},          /*     TIMER_45 , element 13     */
};

typedef enum
{
  PIC_MAIN_BACKGROUND=0,          /*     0     */
  PIC_KEYPAD,          /*     1     */
  PIC_1,          /*     2     */
  PIC_2,          /*     3     */
  PIC_3,          /*     4     */
  PIC_4,          /*     5     */
  PIC_5,          /*     6     */
  PIC_6,          /*     7     */
  PIC_7,          /*     8     */
  PIC_8,          /*     9     */
  PIC_9,          /*     10     */
  LOGO,          /*     11     */
  MESCOLAMENTO_45,          /*     12     */
  TIMER_45,          /*     13     */
}PICTURE_NAME;

