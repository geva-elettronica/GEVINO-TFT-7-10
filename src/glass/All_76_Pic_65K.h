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

const INFO code BINARY_INFO[31]=
{
  /*  No. , Width , Height , Size , Start Address  */ 
  {1,31,31,1922,0},          /*     glass_round_31_led_yellow , element 0     */
  {2,31,31,1922,1922},          /*     glass_round_31_led_green , element 1     */
  {3,31,31,1922,3844},          /*     glass_round_31_led_red , element 2     */
  {4,31,31,1922,5766},          /*     glass_round_31_led_orange , element 3     */
  {5,105,40,8400,7688},          /*     glass_rect_105_40_pressed , element 4     */
  {6,105,40,8400,16088},          /*     glass_rect_105_40_released , element 5     */
  {7,105,55,11550,24488},          /*     glass_rect_105_55_pressed , element 6     */
  {8,105,55,11550,36038},          /*     glass_rect_105_55_released , element 7     */
  {9,105,70,14700,47588},          /*     glass_rect_105_70_pressed , element 8     */
  {10,105,70,14700,62288},          /*     glass_rect_105_70_released , element 9     */
  {11,146,40,11680,76988},          /*     glass_rect_146_40_pressed , element 10     */
  {12,146,40,11680,88668},          /*     glass_rect_146_40_released , element 11     */
  {13,146,55,16060,100348},          /*     glass_rect_146_55_pressed , element 12     */
  {14,146,55,16060,116408},          /*     glass_rect_146_55_released , element 13     */
  {15,146,70,20440,132468},          /*     glass_rect_146_70_pressed , element 14     */
  {16,146,70,20440,152908},          /*     glass_rect_146_70_released , element 15     */
  {17,251,40,20080,173348},          /*     glass_rect_251_40_pressed , element 16     */
  {18,251,40,20080,193428},          /*     glass_rect_251_40_released , element 17     */
  {19,251,55,27610,213508},          /*     glass_rect_251_55_pressed , element 18     */
  {20,251,55,27610,241118},          /*     glass_rect_251_55_released , element 19     */
  {21,251,70,35140,268728},          /*     glass_rect_251_70_pressed , element 20     */
  {22,251,70,35140,303868},          /*     glass_rect_251_70_released , element 21     */
  {23,31,31,1922,339008},          /*     glass_round_31_led_off , element 22     */
  {24,31,31,1922,340930},          /*     glass_round_31_pressed , element 23     */
  {25,31,31,1922,342852},          /*     glass_round_31_released , element 24     */
  {26,48,48,4608,344774},          /*     glass_round_48_pressed , element 25     */
  {27,48,48,4608,349382},          /*     glass_round_48_released , element 26     */
  {28,60,60,7200,353990},          /*     glass_round_60_pressed , element 27     */
  {29,60,60,7200,361190},          /*     glass_round_60_released , element 28     */
  {30,70,70,9800,368390},          /*     glass_round_70_pressed , element 29     */
  {31,70,70,9800,378190},          /*     glass_round_70_released , element 30     */
};

typedef enum
{
  glass_round_31_led_yellow=0,          /*     0     */
  glass_round_31_led_green,          /*     1     */
  glass_round_31_led_red,          /*     2     */
  glass_round_31_led_orange,          /*     3     */
  glass_rect_105_40_pressed,          /*     4     */
  glass_rect_105_40_released,          /*     5     */
  glass_rect_105_55_pressed,          /*     6     */
  glass_rect_105_55_released,          /*     7     */
  glass_rect_105_70_pressed,          /*     8     */
  glass_rect_105_70_released,          /*     9     */
  glass_rect_146_40_pressed,          /*     10     */
  glass_rect_146_40_released,          /*     11     */
  glass_rect_146_55_pressed,          /*     12     */
  glass_rect_146_55_released,          /*     13     */
  glass_rect_146_70_pressed,          /*     14     */
  glass_rect_146_70_released,          /*     15     */
  glass_rect_251_40_pressed,          /*     16     */
  glass_rect_251_40_released,          /*     17     */
  glass_rect_251_55_pressed,          /*     18     */
  glass_rect_251_55_released,          /*     19     */
  glass_rect_251_70_pressed,          /*     20     */
  glass_rect_251_70_released,          /*     21     */
  glass_round_31_led_off,          /*     22     */
  glass_round_31_pressed,          /*     23     */
  glass_round_31_released,          /*     24     */
  glass_round_48_pressed,          /*     25     */
  glass_round_48_released,          /*     26     */
  glass_round_60_pressed,          /*     27     */
  glass_round_60_released,          /*     28     */
  glass_round_70_pressed,          /*     29     */
  glass_round_70_released,          /*     30     */
}PICTURE_NAME;

