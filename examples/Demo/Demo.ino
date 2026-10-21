// Arduino Touch Test
// 10/09/2021 Adieffe Srl

// Keyboard and keys from image.
// Copy the All_76_Pic_65K.bin file and keep it on the SD for the first boot.
// If the fonts are missing, copy font.bin as well.
// Long-press the logo to open a menu that lets you reload the image into flash.

// Install "Arduino SAMD board"
// Use Arduino Zero Native Port

#include "gevino_tft_io.h"  // Respect this sequence
#include "All_76_Pic_65K.h" // Keep this include order
#include "gevino_tft_display.h"

#include "Adafruit_FRAM_I2C.h"
Adafruit_FRAM_I2C fram     = Adafruit_FRAM_I2C();

// FRAM addresses
#define FRAM_CHKWORD    0x00    // 2 bytes to check if erased (0x6969)
#define FRAM_KEYCODE    0x02    // 4 bytes for Key code
#define FRAM_4MA        0x06    // 2 bytes for 4mA calibration
#define FRAM_20MA       0x08    // 2 bytes for 20mA calibration
#define FRAM_PRESET     0x0A    // 20 x 2bytes for presets

unsigned char *p_FRAM_Byte;

// Touch: the library drives both panels (7" FT5206 / 10" GT9271) and
// auto-detects which one is fitted.
struct struct_TouchPoint
{
  int x;
  int y;
};
#include "gevino_tft_touch.h"

int Number;

struct Clocks_struct
{
  bool c_1s;
  bool c_500ms;
  bool c_250ms;
  bool c_100ms;
  bool c_os_1s;
  bool c_os_500ms;
  bool c_os_250ms;
  bool c_os_100ms;
  bool c_os_n_1s;
  bool c_os_n_500ms;
  bool c_os_n_250ms;
  bool c_os_n_100ms;
}Clocks;


// dedicated Colors 
#define BLUE_CUSTOM  0x11EE

struct struct_touch_object
{
  int x;
  int y;
  int w;
  int h;
  int b;
  int f;
  int p;
};

struct struct_bargraph
{
  int x;  // x pos
  int y;  // y pos
  int w;  // width
  int h;  // height
  int b;  // back color
  int f;  // fore color
  int o;  // 0=Horizontal; 1=Vertical
  int u;  // 0=Down to Up or Left to Right; 1=Up to Down or Right to Left
  int z;  // zero
  int s;  // span
  int v;  // val
  int p;  // pic
};

// *********************
// Main buttons/icons
// *********************
struct_touch_object Buttons[]
{
    0,   0,   0,   0, BLACK,      WHITE,          -1,    // not used
  600, 160, 100, 100, WHITE,      BLUE_CUSTOM,     -1,   // - SPEED
  910, 160, 100, 100, WHITE,      BLUE_CUSTOM,     -1,   // + SPEED
  600, 270, 100, 100, WHITE,      BLUE_CUSTOM,     -1,   // - TIME
  910, 270, 100, 100, WHITE,      BLUE_CUSTOM,     -1,   // + TIME
  760, 160, 140, 100, BLUE_CUSTOM, WHITE,          -1,   // SPEED SET xx%
  600, 380, 200, 100, WHITE,      BLUE_CUSTOM,     -1,   // Auto
  810, 380, 200, 100, WHITE,      BLUE_CUSTOM,     -1,   // Man
  600, 490, 200, 100, WHITE,      BLUE_CUSTOM,     -1,   // ON
  810, 490, 200, 100, WHITE,      BLUE_CUSTOM,     -1,   // OFF
  600, 270, 150, 100, WHITE,      BLUE_CUSTOM,     -1,   // SetTime NOT USED!!!
  760, 270, 140, 100, BLUE_CUSTOM, WHITE,          -1,   // Elapsed/Set
   10,  37,  77,  77, RED, WHITE,                  -1,   // STOP Icon
  101,  37,  77,  77, RED, WHITE,                  -1,   // Inverter ko
  192,  37,  77,  77, RED, WHITE,                  -1,   // Locked (run not allowed)
  283,  37,  77,  77, RED, WHITE,                  -1,   // Motor Clixon
  374,  37,  77,  77, RED, WHITE,                  -1,   // Alarm reset
   10,  30, 495,  95, BLUE_CUSTOM, WHITE,          -1,   // Frame Alarms
  710,  50, 220,  70, BLUE_CUSTOM, WHITE,          -1,   // Company Logo
    0,  50, 340,  40, WHITE,      BLUE_CUSTOM,     -1,   // Sw Version
    0, 100, 790,  40, WHITE,      BLUE_CUSTOM,     -1,   // Frame Chip ID
    0, 150, 190,  40, WHITE,      BLUE_CUSTOM,     -1,   // Frame KeyCode
  200, 150, 140,  40, BLUE_CUSTOM, WHITE,          -1,   // KeyCode_0
  350, 150, 140,  40, BLUE_CUSTOM, WHITE,          -1,   // KeyCode_1
  500, 150, 140,  40, BLUE_CUSTOM, WHITE,          -1,   // KeyCode_2
  650, 150, 140,  40, BLUE_CUSTOM, WHITE,          -1,   // KeyCode_3
  800, 150, 140,  40, WHITE,      BLUE_CUSTOM,     -1,   // KeyCode Result
    0, 200, 190,  40, WHITE,      BLUE_CUSTOM,     -1,   // Frame 4mA
  200, 200, 140,  40, BLUE_CUSTOM, WHITE,          -1,   // 4mA value
  350, 200, 140,  40, BLUE_CUSTOM,      WHITE,     -1,   // 4mA Test (NOT USED)
    0, 250, 190,  40, WHITE,      BLUE_CUSTOM,     -1,   // Frame 20mA
  200, 250, 140,  40, BLUE_CUSTOM, WHITE,          -1,   // 20mA value
  350, 250, 140,  40, BLUE_CUSTOM,      WHITE,     -1,   // 20mA Test
  650, 495,  90,  90, WHITE,      BLUE_CUSTOM,     -1,   // Locked
    0, 300, 190,  40, BLUE_CUSTOM,      WHITE,     -1,   // Frame Load pics
  710, 160,  50, 100, BLUE_CUSTOM, WHITE,          -1,   // Icon speed
  710, 270,  50, 100, BLUE_CUSTOM, WHITE,          -1,   // Icon Elapsed/Set
  820, 500, 200,  70, BLUE_CUSTOM, WHITE,          -1,   // info Back
    0,   0,   0,   0, WHITE,      BLUE_CUSTOM,     -1    // Free
};

enum 
{
    BUTTON_NULL = 0, 
    BUTTON_MINUS_SPEED,
    BUTTON_PLUS_SPEED,
    BUTTON_MINUS_TIME,
    BUTTON_PLUS_TIME,
    BUTTON_SPEED_SET,
    BUTTON_AUTO,
    BUTTON_MAN,
    BUTTON_ON,
    BUTTON_OFF,
    BUTTON_TIME_SET,
    BUTTON_ELAPSED,
    BUTTON_STOP,
    BUTTON_INVERTER_KO,
    BUTTON_LOCKED,
    BUTTON_CLIXON_KO,
    BUTTON_ALARM_RESET,
    BUTTON_ALARM_FRAME,
    BUTTON_COMPANY,
    BUTTON_SW_VERSION,
    BUTTON_CHIPID,
    BUTTON_KEYCODE_FRAME,
    BUTTON_KEYCODE_0,
    BUTTON_KEYCODE_1,
    BUTTON_KEYCODE_2,
    BUTTON_KEYCODE_3,
    BUTTON_KEYCODE_OK,
    BUTTON_4MA_FRAME,
    BUTTON_4MA_VAL,
    BUTTON_4MA_TEST,
    BUTTON_20MA_FRAME,
    BUTTON_20MA_VAL,
    BUTTON_20MA_TEST,
    BUTTON_PROTECTION,
    BUTTON_LOAD_PICS,
    BUTTON_SPEED_ICON,
    BUTTON_ELAPSED_ICON,
    BUTTON_INFO_BACK,
    BUTTON_FREE
};

int LAST_BUTTON = BUTTON_ELAPSED;

// *********************
// Presets
// *********************
struct_touch_object Presets[]
{
   10, 148, 142, 142, BLUE_CUSTOM,  WHITE, PIC_1,   
  160, 148, 142, 142, BLUE_CUSTOM,  WHITE, PIC_2,   
  310, 148, 142, 142, BLUE_CUSTOM,  WHITE, PIC_3,
   10, 298, 142, 142, BLUE_CUSTOM,  WHITE, PIC_4,   
  160, 298, 142, 142, BLUE_CUSTOM,  WHITE, PIC_5,  
  310, 298, 142, 142, BLUE_CUSTOM,  WHITE, PIC_6,   
   10, 448, 142, 142, BLUE_CUSTOM,  WHITE, PIC_7,   
  160, 448, 142, 142, WHITE,       WHITE,  PIC_8,           // CHEF  
  310, 448, 142, 142, WHITE,       WHITE,  PIC_9,           // Save button
    0,  90, 460, 460, BLUE_CUSTOM,  WHITE, -1            // frame
};

enum 
{
  PRESET_1=0,          
  PRESET_2,         
  PRESET_3,      
  PRESET_4,          
  PRESET_5,         
  PRESET_6,          
  PRESET_7,          
  PRESET_8,
  PRESET_9,
  PRESET_FRAME
};

int LAST_PRESET = PRESET_9;

// *********************
// Objects
// *********************

struct struct_bargraph SpeedGraph
{
  600, 130, 410,  20, WHITE, BLUE_CUSTOM, 0, 0, 0, 100, 0, -1
};

// *********************
// Keypad
// *********************
struct_touch_object NumPad[]
{
// X    Y    W     H     BC           FC
  280, 460, 116, 100, DARK_GREY,   WHITE,  -1, // 0
  280, 360, 116, 100, DARK_GREY,   WHITE,  -1, // 1
  396, 360, 116, 100, DARK_GREY,   WHITE,  -1, // 2
  512, 360, 116, 100, DARK_GREY,   WHITE,  -1, // 3
  280, 260, 116, 100, DARK_GREY,   WHITE,  -1, // 4
  396, 260, 116, 100, DARK_GREY,   WHITE,  -1, // 5
  512, 260, 116, 100, DARK_GREY,   WHITE,  -1, // 6
  280, 160, 116, 100, DARK_GREY,   WHITE,  -1, // 7
  396, 160, 116, 100, DARK_GREY,   WHITE,  -1, // 8
  512, 160, 116, 100, DARK_GREY,   WHITE,  -1, // 9
  628, 460, 116, 100, RED,         WHITE,  -1, // ESC
  628, 260, 116, 200, GREEN,       BLUE,   -1, // OK
  628, 160, 116, 100, GREEN,       BLUE,   -1, // C
  512, 460, 116, 100, GREEN,       BLUE,   -1, // COMMA
  396, 460, 116, 100, GREEN,       BLUE,   -1, // MINUS
  280,  90, 464,  60, WHITE,       BLUE_CUSTOM,   -1, // DISPLAY
  270,  10, 484, 560, LIGHT_GREY,  BLUE,   -1, // FRAME
  280,  20, 464,  60, BLUE_CUSTOM,  WHITE,  -1, // TITLE
  279, 159, 464, 400, DARK_GREY,   WHITE,  PIC_KEYPAD  // PIC
};

enum 
{
    NUMPAD_0 = 0, 
    NUMPAD_1, 
    NUMPAD_2, 
    NUMPAD_3, 
    NUMPAD_4, 
    NUMPAD_5, 
    NUMPAD_6, 
    NUMPAD_7, 
    NUMPAD_8, 
    NUMPAD_9, 
    NUMPAD_ESC, 
    NUMPAD_OK,
    NUMPAD_C,
    NUMPAD_COMMA,
    NUMPAD_MINUS,
    NUMPAD_DISPLAY,
    NUMPAD_FRAME,
    NUMPAD_TITLE,
    NUMPAD_PIC
};

int LAST_NUMPAD = NUMPAD_MINUS;

// *********************
// Machine related
// *********************
#define SW_VERSION        200
#define MACHINE_OFF       0
#define MACHINE_ON        1

int Status = MACHINE_OFF;
bool SavePreset = 0;

#define MACHINE_AUTO      0
#define MACHINE_MANUAL    1

int Mode = MACHINE_AUTO;

bool InverterReset = 0;

#define MAX_SPEED         100
#define MIN_SPEED         20
int Speed_Set = (MAX_SPEED - MIN_SPEED) / 2 + MIN_SPEED;

#define MAX_TIME          99 * 60
#define MIN_TIME          10
int Time_Set = 5 * 60;

int Act_Speed = 0;
int Act_Elapsed = 0;
int Act_Elapsed_Minutes = 0;
int Act_Elapsed_Seconds = 0;

bool BlinkUpdate_1s = 0;
bool BlinkShow_1s = 0;
bool BlinkUpdate_500ms = 0;
bool BlinkShow_500ms = 0;

// *********************
// I/O
// *********************
#define OUT_INVERTER_RUN_HIGH   setOut0
#define OUT_INVERTER_RUN_LOW    resOut0

#define OUT_INVERTER_RESET_HIGH   setOut1
#define OUT_INVERTER_RESET_LOW    resOut1

#define IN_MACHINE_CLOSED      In0
#define IN_MOTOR_OK       In1
#define IN_STOP_PRESSED   In2
#define IN_INVERTER_KO    In3

// *********************
// Protection
// *********************
unsigned long ChipSerialNumber[4]=
{
  *(volatile uint32_t *)0x0080A00C,
  *(volatile uint32_t *)0x0080A040,
  *(volatile uint32_t *)0x0080A044,
  *(volatile uint32_t *)0x0080A048
};

struct struct_setpoints
{
  int Time;
  int Speed;
};

unsigned int ChkWord = 0;
unsigned long KeyCode = 0;
unsigned long Checksum = 0;
unsigned int range_4mA = 185;
unsigned int range_20mA = 974;
struct_setpoints Sets[]=
{
  180, 50,
  180, 55,
  120, 50,
  240, 80,
  180, 60,
  300, 95,
  120, 70,
  180, 50,
  0,  0,
  0,  0
};

void setup() 
{
  struct_TouchPoint p;

  resOut0;
  resOut1;
  resOut2;
  resOut3;
  resOut4;
  resOut5;
  resOut6;
  resOut7;
  updateIO();  

    // Analog Out on Pin A0
  //  pinMode(A0, OUTPUT);        // DO NOT DECLARE A0 as output as it will not work!!!
  DAC->CTRLB.bit.REFSEL = 0;      // Set Up 1V as DAC Output reference.
  analogWrite(A0, 0);             // Initialize Dac  to Zero

  SerialUSB.begin(115200);        // open serial port

  delay(2000);                    // wait for PC USB to MAP the board

  SerialUSB.println("Start");

  gevino_io_setup();             // DO NOT REMOVE: Contains all barda declarations and low level functions
  tft.init();
  tft.selectInternalFont(RA8876_FONT_SIZE_32);
  CopySDonFlash();                // if exist All_76_Pic_65K.bin or font.bin copy on flash

  
  // Test Touch: probe both controllers and initialise the one that answers
  if (gevino_touch_setup())
  {
    SerialUSB.print("Touch OK - ");   SerialUSB.println(gevino_touch_name());
    tft.print("Touch OK - ");         tft.println(gevino_touch_name());
    delay(200);
  }
  else
  {
    SerialUSB.println("Touch FAIL");
    tft.println("Touch FAIL");
    while(1);
  }

  // Test FRAM
  if (fram.begin()) 
  {
    tft.println("FRAM OK");
    delay(200);
  }
  else 
  {
    tft.println("NOT Found I2C FRAM");
    //while(1);
  }

  tft.print("chip id: ");
  for (int ind=0; ind<4; ind++)
  {
    tft.print(ChipSerialNumber[ind], HEX);
    tft.print("-");
  }
  tft.println();

  SerialUSB.println();
}

// generates
void ClocksGenerator()
{
  static long Last_1s, Last_500ms, Last_250ms, Last_100ms;

  Clocks.c_os_1s = 0;
  Clocks.c_os_500ms = 0;
  Clocks.c_os_250ms = 0;
  Clocks.c_os_100ms = 0;
  Clocks.c_os_n_1s = 0;
  Clocks.c_os_n_500ms = 0;
  Clocks.c_os_n_250ms = 0;
  Clocks.c_os_n_100ms = 0;
  
  if ((millis() - Last_1s) > 500)
  {
    Last_1s = millis();

    if (Clocks.c_1s)
    {
      Clocks.c_1s = 0;
      Clocks.c_os_n_1s = 1;
    }
    else
    {
      Clocks.c_1s = 1;
      Clocks.c_os_1s = 1;
    }
  }
  
  if ((millis() - Last_500ms) > 250)
  {
    Last_500ms = millis();

    if (Clocks.c_500ms)
    {
      Clocks.c_500ms = 0;
      Clocks.c_os_n_500ms = 1;
    }
    else
    {
      Clocks.c_500ms = 1;
      Clocks.c_os_500ms = 1;
    }
  }
  
  if ((millis() - Last_250ms) > 124)
  {
    Last_250ms = millis();

    if (Clocks.c_250ms)
    {
      Clocks.c_250ms = 0;
      Clocks.c_os_n_250ms = 1;
    }
    else
    {
      Clocks.c_250ms = 1;
      Clocks.c_os_250ms = 1;
    }
  }

  if ((millis() - Last_100ms) > 50)
  {
    Last_100ms = millis();

    if (Clocks.c_100ms)
    {
      Clocks.c_100ms = 0;
      Clocks.c_os_n_100ms = 1;
    }
    else
    {
      Clocks.c_100ms = 1;
      Clocks.c_os_100ms = 1;
    }
  }
}


void LightsAndIconsUpdate()
{
  static long LastHighlightsUpdate;
  static int HighlightStep = 0;
  int offset_x = 15, offset_y = 15, radius = 10;

  if ((millis() - LastHighlightsUpdate) > 5)
  {
    switch(HighlightStep)
    {
      // Auto light
      case 0:
        tft.drawCircle(Buttons[BUTTON_AUTO].x + offset_x, Buttons[BUTTON_AUTO].y + offset_y, radius, WHITE);
        
        if (Mode == MACHINE_AUTO)
          tft.fillCircle(Buttons[BUTTON_AUTO].x + offset_x, Buttons[BUTTON_AUTO].y + offset_y, radius - 1, GREEN);
        else
          tft.fillCircle(Buttons[BUTTON_AUTO].x + offset_x, Buttons[BUTTON_AUTO].y + offset_y, radius - 1, DARK_GREY);

        HighlightStep++;
        break;

      // Man light
      case 1:
        tft.drawCircle(Buttons[BUTTON_MAN].x + offset_x, Buttons[BUTTON_MAN].y + offset_y, radius, WHITE);
        
        if (Mode != MACHINE_AUTO)
          tft.fillCircle(Buttons[BUTTON_MAN].x + offset_x, Buttons[BUTTON_MAN].y + offset_y, radius - 1, YELLOW);
        else
          tft.fillCircle(Buttons[BUTTON_MAN].x + offset_x, Buttons[BUTTON_MAN].y + offset_y, radius - 1, DARK_GREY);

        HighlightStep++;
        break;

      // ON light
      case 2:
        tft.drawCircle(Buttons[BUTTON_ON].x + offset_x, Buttons[BUTTON_ON].y + offset_y, radius, WHITE);
        
        if (Status == MACHINE_ON)
          tft.fillCircle(Buttons[BUTTON_ON].x + offset_x, Buttons[BUTTON_ON].y + offset_y, radius - 1, GREEN);
        else
          tft.fillCircle(Buttons[BUTTON_ON].x + offset_x, Buttons[BUTTON_ON].y + offset_y, radius - 1, DARK_GREY);

        HighlightStep++;
        break;

      // OFF light
      case 3:
        tft.drawCircle(Buttons[BUTTON_OFF].x + offset_x, Buttons[BUTTON_OFF].y + offset_y, radius, WHITE);
        
        if (Status != MACHINE_ON)
          tft.fillCircle(Buttons[BUTTON_OFF].x + offset_x, Buttons[BUTTON_OFF].y + offset_y, radius - 1, RED);
        else
          tft.fillCircle(Buttons[BUTTON_OFF].x + offset_x, Buttons[BUTTON_OFF].y + offset_y, radius - 1, DARK_GREY);

        HighlightStep++;
        break;

      // Alarm reset
      case 4:
        HighlightStep++;
        break;

      default:
        HighlightStep = 0;
        break;
    }

    LastHighlightsUpdate = millis();
  }
}

void Update_TimeCounter()
{
  if (Clocks.c_os_1s)
  {
    if (Status == MACHINE_OFF)
      return;

    if (Mode == MACHINE_AUTO)
    {
      if (Act_Elapsed)
      {
        Act_Elapsed--;

        if (Act_Elapsed == 0)
          Status = MACHINE_OFF;
      }
    }
    else
    {
      Act_Elapsed++;
    }

    Act_Elapsed_Minutes = Act_Elapsed / 60;
    Act_Elapsed_Seconds = Act_Elapsed % 60;
    Update_Elapsed_Box();
  }
}

/********************************************************************//**
 * @brief     detects a touch event and converts touch data 
 * @param[in] None
 * @return    boolean (true = touch pressed, false = touch unpressed) 
 *********************************************************************/
bool Touch_Event(struct struct_TouchPoint &p)
{
  int16_t x, y;

  // The library already returns screen coordinates (0..1023, 0..599),
  // whatever panel is fitted.
  if (gevino_touch_read(&x, &y))
  {
    p.x = x;
    p.y = y;
    return true;
  }
  else
    return false;
}

int DetectMainButtons(int X, int Y)
{
  int ind, pressed = 0;
  static long Last = 0;

  if ((millis() - Last) < 100)
    return pressed;

  Last = millis();

  for (ind = 1; ind <= LAST_BUTTON; ind++)
  {
    if ((X > Buttons[ind].x) && (X < (Buttons[ind].x + Buttons[ind].w)))
      if ((Y > Buttons[ind].y) && (Y < (Buttons[ind].y + Buttons[ind].h)))
      {
        pressed = ind;          
        break;
      }
  }

  if ((X > Buttons[BUTTON_COMPANY].x) && (X < (Buttons[BUTTON_COMPANY].x + Buttons[BUTTON_COMPANY].w)))
  {
    if ((Y > Buttons[BUTTON_COMPANY].y) && (Y < (Buttons[BUTTON_COMPANY].y + Buttons[BUTTON_COMPANY].h)))
    {
      pressed = BUTTON_COMPANY;          
    }
  }

  if ((X > Buttons[BUTTON_ALARM_RESET].x) && (X < (Buttons[BUTTON_ALARM_RESET].x + Buttons[BUTTON_ALARM_RESET].w)))
  {
    if ((Y > Buttons[BUTTON_ALARM_RESET].y) && (Y < (Buttons[BUTTON_ALARM_RESET].y + Buttons[BUTTON_ALARM_RESET].h)))
    {
      pressed = BUTTON_ALARM_RESET;          
    }
  }

  return pressed;
}

int DetectInfoButtons(int X, int Y)
{
  int ind, pressed = 0;
  static long Last = 0;

  if ((millis() - Last) < 100)
    return pressed;

  Last = millis();

  for (ind = BUTTON_KEYCODE_0; ind <= BUTTON_KEYCODE_3; ind++)
  {
    if ((X > Buttons[ind].x) && (X < (Buttons[ind].x + Buttons[ind].w)))
      if ((Y > Buttons[ind].y) && (Y < (Buttons[ind].y + Buttons[ind].h)))
      {
        pressed = ind;          
        break;
      }
  }

  if ((X > Buttons[BUTTON_4MA_VAL].x) && (X < (Buttons[BUTTON_4MA_VAL].x + Buttons[BUTTON_4MA_VAL].w)))
  {
    if ((Y > Buttons[BUTTON_4MA_VAL].y) && (Y < (Buttons[BUTTON_4MA_VAL].y + Buttons[BUTTON_4MA_VAL].h)))
    {
      pressed = BUTTON_4MA_VAL;          
    }
  }
  
  if ((X > Buttons[BUTTON_4MA_TEST].x) && (X < (Buttons[BUTTON_4MA_TEST].x + Buttons[BUTTON_4MA_TEST].w)))
  {
    if ((Y > Buttons[BUTTON_4MA_TEST].y) && (Y < (Buttons[BUTTON_4MA_TEST].y + Buttons[BUTTON_4MA_TEST].h)))
    {
      pressed = BUTTON_4MA_TEST;          
    }
  }
  
  if ((X > Buttons[BUTTON_20MA_VAL].x) && (X < (Buttons[BUTTON_20MA_VAL].x + Buttons[BUTTON_20MA_VAL].w)))
  {
    if ((Y > Buttons[BUTTON_20MA_VAL].y) && (Y < (Buttons[BUTTON_20MA_VAL].y + Buttons[BUTTON_20MA_VAL].h)))
    {
      pressed = BUTTON_20MA_VAL;          
    }
  }
  
  if ((X > Buttons[BUTTON_20MA_TEST].x) && (X < (Buttons[BUTTON_20MA_TEST].x + Buttons[BUTTON_20MA_TEST].w)))
  {
    if ((Y > Buttons[BUTTON_20MA_TEST].y) && (Y < (Buttons[BUTTON_20MA_TEST].y + Buttons[BUTTON_20MA_TEST].h)))
    {
      pressed = BUTTON_20MA_TEST;          
    }
  }

  if ((X > Buttons[BUTTON_INFO_BACK].x) && (X < (Buttons[BUTTON_INFO_BACK].x + Buttons[BUTTON_INFO_BACK].w)))
  {
    if ((Y > Buttons[BUTTON_INFO_BACK].y) && (Y < (Buttons[BUTTON_INFO_BACK].y + Buttons[BUTTON_INFO_BACK].h)))
    {
      pressed = BUTTON_INFO_BACK;          
    }
  }
  
  if ((X > Buttons[BUTTON_LOAD_PICS].x) && (X < (Buttons[BUTTON_LOAD_PICS].x + Buttons[BUTTON_LOAD_PICS].w)))
  {
    if ((Y > Buttons[BUTTON_LOAD_PICS].y) && (Y < (Buttons[BUTTON_LOAD_PICS].y + Buttons[BUTTON_LOAD_PICS].h)))
    {
      pressed = BUTTON_LOAD_PICS;          
    }
  }

  return pressed;
}


int DetectPresetButtons(int X, int Y)
{
  int ind, pressed = -1;
  static long Last = 0;

  if ((millis() - Last) < 100)
    return pressed;

  Last = millis();

  for (ind = 0; ind <= LAST_PRESET; ind++)
  {
    if ((X > Presets[ind].x) && (X < (Presets[ind].x + Presets[ind].w)))
      if ((Y > Presets[ind].y) && (Y < (Presets[ind].y + Presets[ind].h)))
      {
        pressed = ind;          
        break;
      }
  }

  return pressed;
}

/********************************************************************//**
 * @brief     detecting pressed buttons with the given touchscreen values
 * @param[in] None
 * @return    None
 *********************************************************************/
int HandleNumPadButtons(int X, int Y)
{
  int ind, pressed = -1;
  static long Last = 0;

  if ((millis() - Last) < 500)
    return 0;

  Last = millis();

  for (ind = 0; ind <= LAST_NUMPAD; ind++)
  {
    if ((X > NumPad[ind].x) && (X < (NumPad[ind].x + NumPad[ind].w)))
      if ((Y > NumPad[ind].y) && (Y < (NumPad[ind].y + NumPad[ind].h)))
      {
        pressed = ind;       
        break;   
      }
  }

  if ((pressed >= NUMPAD_0) && (pressed <= NUMPAD_9))
  {
    ////tone(BEEPER, 1000, 50);
    delay(100);
    Number = Number * 10 + pressed;
    Update_NumPadValue();
    return 0;
  }
    
  if (pressed == NUMPAD_ESC)
  {
    ////tone(BEEPER, 1000, 50);
    delay(100);
    return -1;
  }

  if (pressed == NUMPAD_C)
  {
    ////tone(BEEPER, 1000, 50);
    Number /= 10;
    Update_NumPadValue();
    delay(100);
    return 0;
  }

  if (pressed == NUMPAD_OK)
  {
    ////tone(BEEPER, 1000, 50);
    delay(100);
    return 1;
  }
}

void LoadPicsFromSD()
{
  byte ers = 0;   // Erased

  tft.setTextColor(WHITE);
  
  if(sd.begin(12, SD_SCK_MHZ(12)))
  {
    tft.println("SD OK! Please wait...");

    if (sd.exists("All_76_Pic_65K.bin"))
    {
      if( ers ) 
        WriteFlash(Pic_Offset, "All_76_Pic_65K.bin", 0);  // Copy, "All_76_Pic_65K.bin" on the chip flash, erase flash chip, pic area
      else  
        WriteFlash(Pic_Offset, "All_76_Pic_65K.bin", 2);
        
      gevino_io_setup();    
      tft.init();
      tft.selectInternalFont(RA8876_FONT_SIZE_32);
      tft.println("SD ok, PICs loaded");
    }
    else
      tft.println("<All_76_Pic_65K.bin> File not found!");
  }
  else  
    tft.println("SD fail");
}

void LoadFontsFromSD()
{
  byte ers = 0;   // Erased
  
  tft.setTextColor(WHITE);
  
  if(sd.begin(12, SD_SCK_MHZ(12)))
  {
    tft.println("SD OK! Please wait...");

    if (sd.exists("font.bin"))
    {
      WriteFlash(0, "font.bin", 1);  // Copy, From Address 0, "font.bin" on the chip flash, before erase flash chip
      ers = 1;
      gevino_io_setup();    
      tft.init();
      tft.selectInternalFont(RA8876_FONT_SIZE_32);
      tft.println("SD ok, FONTs loaded");
    }
    else
      tft.println("<font.bin> File not found!");
  }
  else  
    tft.println("SD fail");
}

/********************************************************************//**
 * @brief     shows the info screen in setup procedure
 * @param[in] None
 * @return    None
 *********************************************************************/
void InfoScreen()
{
  long last;
  unsigned char *p_byte;
  String dummy_s;
  bool stay = 1;

  int Pressed_Button = 0;
  int Step = 0, FollowingStep = 0;
  bool Touch_pressed;
  struct_TouchPoint p;
  bool debounce = 0;
  long debounce_time = 0;
  int X,Y;
  int KeyLongPressed = 0;
  long LastKeyPressedTime = 0;
  

  while(stay)
  {
    switch(Step)
    {
      // Init
      case 0:
        tft.pic(PIC_MAIN_BACKGROUND, 0, 0);
        draw_Button(Buttons[BUTTON_INFO_BACK].x, Buttons[BUTTON_INFO_BACK].y, Buttons[BUTTON_INFO_BACK].w, Buttons[BUTTON_INFO_BACK].h, Buttons[BUTTON_INFO_BACK].b, Buttons[BUTTON_INFO_BACK].f, Buttons[BUTTON_INFO_BACK].p, "Back");

        // Sw Version
        draw_Button(Buttons[BUTTON_SW_VERSION].x, Buttons[BUTTON_SW_VERSION].y, Buttons[BUTTON_SW_VERSION].w, Buttons[BUTTON_SW_VERSION].h, Buttons[BUTTON_SW_VERSION].b, Buttons[BUTTON_SW_VERSION].f, Buttons[BUTTON_SW_VERSION].p, "");
        tft.setCursor(Buttons[BUTTON_SW_VERSION].x + 10, Buttons[BUTTON_SW_VERSION].y + ((Buttons[BUTTON_SW_VERSION].h - 20) / 2));
        tft.setFont(&notoSansBold_20pt);
        tft.setTextColor(BLUE_CUSTOM);
        tft.print("Software Version: ");
        tft.print((float)((float)SW_VERSION / 100.0));
      
        draw_Button(Buttons[BUTTON_CHIPID].x, Buttons[BUTTON_CHIPID].y, Buttons[BUTTON_CHIPID].w, Buttons[BUTTON_CHIPID].h, Buttons[BUTTON_CHIPID].b, Buttons[BUTTON_CHIPID].f, Buttons[BUTTON_CHIPID].p, "");
        tft.setCursor(Buttons[BUTTON_CHIPID].x + 10, Buttons[BUTTON_CHIPID].y + ((Buttons[BUTTON_CHIPID].h - 20) / 2));
      
        // Chip ID
        tft.setFont(&notoSansBold_20pt);
        tft.setTextColor(BLUE_CUSTOM);
        tft.print("Chip-ID: ");
        for (int ind=0; ind<4; ind++)
        {
          tft.print(ChipSerialNumber[ind], HEX);
      
          if (ind < 3)
            tft.print("-");
          else
            tft.println("");
        }
      
        draw_Button(Buttons[BUTTON_KEYCODE_FRAME].x, Buttons[BUTTON_KEYCODE_FRAME].y, Buttons[BUTTON_KEYCODE_FRAME].w, Buttons[BUTTON_KEYCODE_FRAME].h, Buttons[BUTTON_KEYCODE_FRAME].b, Buttons[BUTTON_KEYCODE_FRAME].f, Buttons[BUTTON_KEYCODE_FRAME].p, "");
        tft.setCursor(Buttons[BUTTON_KEYCODE_FRAME].x + 10, Buttons[BUTTON_KEYCODE_FRAME].y + ((Buttons[BUTTON_KEYCODE_FRAME].h - 20) / 2));
        tft.setFont(&notoSansBold_20pt);

        tft.setTextColor(BLUE_CUSTOM);
        tft.print("KeyCode:  ");
        
        p_byte = (unsigned char *)&KeyCode;
        for (int ind=0; ind<4; ind++)
        {
          dummy_s = String(*p_byte++);
          draw_Button(Buttons[BUTTON_KEYCODE_0 + ind].x, Buttons[BUTTON_KEYCODE_0 + ind].y, Buttons[BUTTON_KEYCODE_0 + ind].w, Buttons[BUTTON_KEYCODE_0 + ind].h, Buttons[BUTTON_KEYCODE_0 + ind].b, Buttons[BUTTON_KEYCODE_0 + ind].f, Buttons[BUTTON_KEYCODE_0 + ind].p, dummy_s);
        }  
      
        if (KeyCode == Checksum)
          draw_Button(Buttons[BUTTON_KEYCODE_OK].x, Buttons[BUTTON_KEYCODE_OK].y, Buttons[BUTTON_KEYCODE_OK].w, Buttons[BUTTON_KEYCODE_OK].h, Buttons[BUTTON_KEYCODE_OK].b, Buttons[BUTTON_KEYCODE_OK].f, Buttons[BUTTON_KEYCODE_OK].p, "OK");
        else
          draw_Button(Buttons[BUTTON_KEYCODE_OK].x, Buttons[BUTTON_KEYCODE_OK].y, Buttons[BUTTON_KEYCODE_OK].w, Buttons[BUTTON_KEYCODE_OK].h, Buttons[BUTTON_KEYCODE_OK].b, Buttons[BUTTON_KEYCODE_OK].f, Buttons[BUTTON_KEYCODE_OK].p, "KO");
      
        draw_Button(Buttons[BUTTON_4MA_FRAME].x, Buttons[BUTTON_4MA_FRAME].y, Buttons[BUTTON_4MA_FRAME].w, Buttons[BUTTON_4MA_FRAME].h, Buttons[BUTTON_4MA_FRAME].b, Buttons[BUTTON_4MA_FRAME].f, Buttons[BUTTON_4MA_FRAME].p, " ");
        tft.setFont(&notoSansBold_20pt);
        tft.setTextColor(BLUE_CUSTOM);
        tft.setCursor(Buttons[BUTTON_4MA_FRAME].x + 10, Buttons[BUTTON_4MA_FRAME].y + ((Buttons[BUTTON_4MA_FRAME].h - 20) / 2));
        tft.print("MIN Value: ");
        dummy_s = String(range_4mA);
        draw_Button(Buttons[BUTTON_4MA_VAL].x, Buttons[BUTTON_4MA_VAL].y, Buttons[BUTTON_4MA_VAL].w, Buttons[BUTTON_4MA_VAL].h, Buttons[BUTTON_4MA_VAL].b, Buttons[BUTTON_4MA_VAL].f, Buttons[BUTTON_4MA_VAL].p, dummy_s);
        
        draw_Button(Buttons[BUTTON_20MA_FRAME].x, Buttons[BUTTON_20MA_FRAME].y, Buttons[BUTTON_20MA_FRAME].w, Buttons[BUTTON_20MA_FRAME].h, Buttons[BUTTON_20MA_FRAME].b, Buttons[BUTTON_20MA_FRAME].f, Buttons[BUTTON_20MA_FRAME].p, " ");
        tft.setFont(&notoSansBold_20pt);

        tft.setTextColor(BLUE_CUSTOM);
        tft.setCursor(Buttons[BUTTON_20MA_FRAME].x + 10, Buttons[BUTTON_20MA_FRAME].y + ((Buttons[BUTTON_20MA_FRAME].h - 20) / 2));
        tft.print("MAX Value: ");
        dummy_s = String(range_20mA);
        draw_Button(Buttons[BUTTON_20MA_VAL].x, Buttons[BUTTON_20MA_VAL].y, Buttons[BUTTON_20MA_VAL].w, Buttons[BUTTON_20MA_VAL].h, Buttons[BUTTON_20MA_VAL].b, Buttons[BUTTON_20MA_VAL].f, Buttons[BUTTON_20MA_VAL].p, dummy_s);
        draw_Button(Buttons[BUTTON_20MA_TEST].x, Buttons[BUTTON_20MA_TEST].y, Buttons[BUTTON_20MA_TEST].w, Buttons[BUTTON_20MA_TEST].h, Buttons[BUTTON_20MA_TEST].b, Buttons[BUTTON_20MA_TEST].f, Buttons[BUTTON_20MA_TEST].p, "Test");

        // Load pics from SD
        draw_Button(Buttons[BUTTON_LOAD_PICS].x, Buttons[BUTTON_LOAD_PICS].y, Buttons[BUTTON_LOAD_PICS].w, Buttons[BUTTON_LOAD_PICS].h, Buttons[BUTTON_LOAD_PICS].b, Buttons[BUTTON_LOAD_PICS].f, Buttons[BUTTON_LOAD_PICS].p, " ");
        tft.setFont(&notoSansBold_20pt);

        tft.setTextColor(WHITE);
        tft.setCursor(Buttons[BUTTON_LOAD_PICS].x + 10, Buttons[BUTTON_LOAD_PICS].y + ((Buttons[BUTTON_LOAD_PICS].h - 20) / 2));
        tft.print("Load PICS");
        
        // wait for debounce
        last = millis();
        while(1)
        {
          Touch_pressed = Touch_Event(p);
          if (Touch_pressed == true) 
            last = millis(); 
        
          if ((millis() - last) > 500)
            break;
        }  
        Step = 100;
        break;

      // Main info page
      case 100:
        debounce_time = 100;
        debounce = 0;        
        
        // check touch screen for new events
        Touch_pressed = Touch_Event(p);
                
        if (Touch_pressed == true) 
        {
          Pressed_Button = DetectInfoButtons(p.x, p.y);

          switch(Pressed_Button)
          {
            case BUTTON_KEYCODE_0:
              //if (KeyCode != Checksum)
              {
                Step = 110;
                FollowingStep = 200;
              }
              break;
              
            case BUTTON_KEYCODE_1:
              if (KeyCode != Checksum)
              {
                Step = 110;
                FollowingStep = 205;
              }
              break;
              
            case BUTTON_KEYCODE_2:
              if (KeyCode != Checksum)
              {
                Step = 110;
                FollowingStep = 210;
              }
              break;
              
            case BUTTON_KEYCODE_3:
              if (KeyCode != Checksum)
              {
                Step = 110;
                FollowingStep = 215;
              }
              break;
              
            case BUTTON_4MA_VAL:
              Step = 110;
              FollowingStep = 220;
              break;
              
            case BUTTON_20MA_VAL:
              Step = 110;
              FollowingStep = 225;
              break;

            case BUTTON_4MA_TEST:
              draw_Button(Buttons[BUTTON_4MA_TEST].x, Buttons[BUTTON_4MA_TEST].y, Buttons[BUTTON_4MA_TEST].w, Buttons[BUTTON_4MA_TEST].h, Buttons[BUTTON_4MA_TEST].f, Buttons[BUTTON_4MA_TEST].b, Buttons[BUTTON_4MA_TEST].p, "Test");
              last = millis();
              Step = 230;
              break;

            case BUTTON_20MA_TEST:
              draw_Button(Buttons[BUTTON_20MA_TEST].x, Buttons[BUTTON_20MA_TEST].y, Buttons[BUTTON_20MA_TEST].w, Buttons[BUTTON_20MA_TEST].h, Buttons[BUTTON_20MA_TEST].f, Buttons[BUTTON_20MA_TEST].b, Buttons[BUTTON_20MA_TEST].p, "Test");
              last = millis();
              Step = 230;
              break;

            case BUTTON_LOAD_PICS:
              tft.clearScreen(BLACK);
              LoadPicsFromSD();
              delay(2000);
              Step = 0;
              break;

            //case BUTTON_COMPANY:
            case BUTTON_INFO_BACK:
              stay = 0;
              Step = 0;
              break;
              
            default:
              break;
          }
        }

        Pressed_Button = BUTTON_NULL;

        if (debounce)
        {
          last = millis();
          Step = 102;
          break;
        }
        break;

      // wait until XXXms not pressed to avoid bouncing keys
      case 102:
        Touch_pressed = Touch_Event(p);
        if (Touch_pressed == true) 
          last = millis(); 

        if ((millis() - last) > debounce_time)
          Step = 100;
        break;

      // NumPad init
      case 110:
        Number = 0;
        draw_NumPad(" ");
        last = millis(); 
        Step = 112;
        break;

      // wait until 1s not pressed to avoid bouncing keys
      case 112:
        Touch_pressed = Touch_Event(p);
        if (Touch_pressed == true) 
          last = millis(); 

        if ((millis() - last) > 500)
          Step = 115;
        break;
        
      // NumPad handling
      case 115:
        Touch_pressed = Touch_Event(p);
        if (Touch_pressed == true) 
        { 
          Pressed_Button = HandleNumPadButtons(p.x, p.y);
          
          if (Pressed_Button == 1) 
          {
            last = millis(); 
            Step = 120;
          }
          
          if (Pressed_Button == -1)
            Step = 0;
        } 
        break;

      // wait until 1s not pressed to avoid bouncing keys
      case 120:
        Touch_pressed = Touch_Event(p);
        if (Touch_pressed == true) 
          last = millis(); 

        if ((millis() - last) > 250)
          Step = FollowingStep;
        break;

      // KeyCode 0
      case 200:
        if (Number < 0)
          Number = 0;
  
        if (Number > 255)
          Number = 255;

        p_byte = (unsigned char *)&KeyCode;
        p_byte += 0;
        *p_byte = Number;
        Step = 0;
        break;
        
      // KeyCode 1
      case 205:
        if (Number < 0)
          Number = 0;
  
        if (Number > 255)
          Number = 255;

        p_byte = (unsigned char *)&KeyCode;
        p_byte += 1;
        *p_byte = Number;
        Step = 0;
        break;
        
      // KeyCode 2
      case 210:
        if (Number < 0)
          Number = 0;
  
        if (Number > 255)
          Number = 255;

        p_byte = (unsigned char *)&KeyCode;
        p_byte += 2;
        *p_byte = Number;
        Step = 0;
        break;
        
      // KeyCode 3
      case 215:
        if (Number < 0)
          Number = 0;
  
        if (Number > 255)
          Number = 255;

        p_byte = (unsigned char *)&KeyCode;
        p_byte += 3;
        *p_byte = Number;
        Step = 0;
        break;
        
      // 4mA
      case 220:
        if (Number < 0)
          Number = 0;
  
        if (Number > 1023)
          Number = 1023;

        if (Number < range_20mA)
          range_4mA = Number;
          
        out_4_20mA(0);
        Step = 0;
        break;
        
      // 20mA
      case 225:
        if (Number < 0)
          Number = 0;
  
        if (Number > 1023)
          Number = 1023;

        if (Number > range_4mA)
          range_20mA = Number;

        out_4_20mA(0);
        Step = 0;
        break;

      // Sets analog output to 20mA to adjust range
      case 230:
        out_4_20mA(100);
        Touch_pressed = Touch_Event(p);
        if (Touch_pressed == true) 
          last = millis(); 

        if ((millis() - last) > debounce_time)
        {
          draw_Button(Buttons[BUTTON_20MA_TEST].x, Buttons[BUTTON_20MA_TEST].y, Buttons[BUTTON_20MA_TEST].w, Buttons[BUTTON_20MA_TEST].h, Buttons[BUTTON_20MA_TEST].b, Buttons[BUTTON_20MA_TEST].f, Buttons[BUTTON_20MA_TEST].p, "Test");
          out_4_20mA(0);
          Step = 100;
        }
        break;

      default:
        Step = 0;
        break;
    }
  }
}

/********************************************************************//**
 * @brief     draws the keypad
 * @param[in] None
 * @return    None
 *********************************************************************/
void draw_NumPad(String txt)
{
  
  draw_Button(NumPad[NUMPAD_FRAME].x, NumPad[NUMPAD_FRAME].y, NumPad[NUMPAD_FRAME].w, NumPad[NUMPAD_FRAME].h, NumPad[NUMPAD_FRAME].b, NumPad[NUMPAD_FRAME].f, NumPad[NUMPAD_FRAME].p, "");

  tft.setFont(&notoSansBold_32pt);
  draw_Button(NumPad[NUMPAD_TITLE].x, NumPad[NUMPAD_TITLE].y, NumPad[NUMPAD_TITLE].w, NumPad[NUMPAD_TITLE].h, NumPad[NUMPAD_TITLE].b, NumPad[NUMPAD_TITLE].f, NumPad[NUMPAD_TITLE].p, txt);
  tft.setFont(&notoSansBold_20pt);
  draw_Button(NumPad[NUMPAD_DISPLAY].x, NumPad[NUMPAD_DISPLAY].y, NumPad[NUMPAD_DISPLAY].w, NumPad[NUMPAD_DISPLAY].h, NumPad[NUMPAD_DISPLAY].b, NumPad[NUMPAD_DISPLAY].f, NumPad[NUMPAD_DISPLAY].p, "");
  draw_Button(NumPad[NUMPAD_PIC].x, NumPad[NUMPAD_PIC].y, NumPad[NUMPAD_PIC].w, NumPad[NUMPAD_PIC].h, NumPad[NUMPAD_PIC].b, NumPad[NUMPAD_PIC].f, NumPad[NUMPAD_PIC].p, "");
}

void drawFastHLine(int x, int y, int w, int c)
{
  tft.drawLine(x, y, x+w-1, y, c);
}

void drawFastVLine(int x, int y, int h, int c)
{
  tft.drawLine(x, y, x, y+h-1, c);
}

void draw_Button(int xb, int yb, int wb, int hb, int bc, int fc, int p, String txt)
{
  int xt, yt;
  char str[50];
  int len_str;
  
  drawFastHLine(xb, yb, wb, WHITE);
  drawFastVLine(xb, yb, hb, WHITE);
  drawFastHLine(xb, yb + hb, wb, LIGHT_GREY);
  drawFastVLine(xb + wb, yb, hb, LIGHT_GREY);
  tft.fillRect(xb + 1, yb + 1, xb + wb - 1, yb + hb - 1, bc);

  if (p >= 0)
    tft.pic(p, xb + 1, yb + 1);

  if (hb <= 40)
    tft.setFont(&notoSansBold_20pt);
  else
    tft.setFont(&notoSansBold_32pt);
    
  len_str = txt.length();
  txt.toCharArray(str, len_str); 
  int16_t x, y;
  uint16_t w, h;
  tft.getTextBounds(str, 0, 0, &x, &y, &w, &h);  

  xt = xb + (wb / 2) - (w / 2);
  yt = yb + (hb / 2) - (h / 2);
  tft.setCursor(xt, yt); 
  tft.setTextColor(fc);
  tft.print(txt);
}

void draw_Label(int xb, int yb, int wb, int hb, int bc, int fc, int p, String txt)
{
  int xt, yt;
  char str[50];
  int len_str;
  
  drawFastHLine(xb, yb, wb, bc);
  drawFastVLine(xb, yb, hb, bc);
  drawFastHLine(xb, yb + hb, wb, bc);
  drawFastVLine(xb + wb, yb, hb, bc);
  tft.fillRect(xb + 1, yb + 1, xb + wb - 1, yb + hb - 1, bc);

  if (p >= 0)
    tft.pic(p, xb + 1, yb + 1);

  if (hb <= 40)
    tft.setFont(&notoSansBold_20pt);
  else
    tft.setFont(&notoSansBold_32pt);
    
  len_str = txt.length();
  txt.toCharArray(str, len_str); 
  int16_t x, y;
  uint16_t w, h;
  tft.getTextBounds(str, 0, 0, &x, &y, &w, &h);  

  xt = xb + (wb / 2) - (w / 2);
  yt = yb + (hb / 2) - (h / 2);
  tft.setCursor(xt, yt); 
  tft.setTextColor(fc);
  tft.print(txt);
}

void draw_BarGraph(struct_bargraph SG)
{
  int delta, val, bc, fc;
  float f_val;
  
  // draw white outline
  tft.drawRect(SG.x, SG.y, SG.x + SG.w - 1, SG.y + SG.h - 1, WHITE);
  
  // draw inner black border
  tft.drawRect(SG.x + 1, SG.y + 1, SG.x + SG.w - 2, SG.y + SG.h - 2, BLACK);

  if (SG.o == 0)
    delta = SG.w - 5;
  else
    delta = SG.h - 5;

  if (SG.u == 0)
  {
    bc = SG.b;
    fc = SG.f;
  }
  else
  {
    bc = SG.f;
    fc = SG.b;
  }

  f_val = (float)(SG.v * delta);
  f_val /= 100.0;
  val = (int)f_val;
  
  if (SG.o == 0)
  {
    if (SG.v)
    {
      tft.fillRect(SG.x + 2, SG.y + 2, SG.x + val, SG.y + SG.h - 3, fc);  // draw value bar
      tft.fillRect(SG.x + 2 + val, SG.y + 2, SG.x + 2 + delta, SG.y + SG.h - 3, bc);  // draw rest of the bar
    }
    else
      tft.fillRect(SG.x + 2, SG.y + 2, SG.x + 2 + delta, SG.y + SG.h - 3, bc);  // draw rest of the bar
  }
  else
  {
    if (SG.v)    
    {
      tft.fillRect(SG.x + 2, SG.y + 2, SG.x + SG.w - 3, SG.y + val, fc);  // draw value bar
      tft.fillRect(SG.x + 2, SG.y + 2 + val, SG.x + SG.w - 3, SG.y + delta + 2, bc);  // draw rest of the bar
    }
    else
      tft.fillRect(SG.x + 2, SG.y + 2, SG.x + SG.w - 3, SG.y + delta + 2, bc);  // draw rest of the bar
  }
}

void draw_Presets()
{
  for (int ind = 0; ind <= LAST_PRESET; ind++)
      draw_Button(Presets[ind].x, Presets[ind].y, Presets[ind].w, Presets[ind].h, Presets[ind].b, Presets[ind].f, Presets[ind].p, "");
}  

void draw_MainPage()
{
  String dummy_s;
  
  tft.pic(PIC_MAIN_BACKGROUND, 0, 0);
  tft.pic(LOGO, Buttons[BUTTON_COMPANY].x, Buttons[BUTTON_COMPANY].y);
  
  //Draw ON and OFF buttons
  draw_Button(Buttons[BUTTON_ON].x, Buttons[BUTTON_ON].y, Buttons[BUTTON_ON].w, Buttons[BUTTON_ON].h, Buttons[BUTTON_ON].b, Buttons[BUTTON_ON].f, Buttons[BUTTON_ON].p, "ON ");
  draw_Button(Buttons[BUTTON_OFF].x, Buttons[BUTTON_OFF].y, Buttons[BUTTON_OFF].w, Buttons[BUTTON_OFF].h, Buttons[BUTTON_OFF].b, Buttons[BUTTON_OFF].f, Buttons[BUTTON_OFF].p, "OFF");

  //Draw Auto and Man buttons
  draw_Button(Buttons[BUTTON_AUTO].x, Buttons[BUTTON_AUTO].y, Buttons[BUTTON_AUTO].w, Buttons[BUTTON_AUTO].h, Buttons[BUTTON_AUTO].b, Buttons[BUTTON_AUTO].f, Buttons[BUTTON_AUTO].p, "Auto");
  draw_Button(Buttons[BUTTON_MAN].x, Buttons[BUTTON_MAN].y, Buttons[BUTTON_MAN].w, Buttons[BUTTON_MAN].h, Buttons[BUTTON_MAN].b, Buttons[BUTTON_MAN].f, Buttons[BUTTON_MAN].p, "Man");

  //Draw the Speed Plus and Minus buttons
  draw_Button(Buttons[BUTTON_PLUS_SPEED].x, Buttons[BUTTON_PLUS_SPEED].y, Buttons[BUTTON_PLUS_SPEED].w, Buttons[BUTTON_PLUS_SPEED].h, Buttons[BUTTON_PLUS_SPEED].b, Buttons[BUTTON_PLUS_SPEED].f, Buttons[BUTTON_PLUS_SPEED].p, "+  ");
  draw_Button(Buttons[BUTTON_MINUS_SPEED].x, Buttons[BUTTON_MINUS_SPEED].y, Buttons[BUTTON_MINUS_SPEED].w, Buttons[BUTTON_MINUS_SPEED].h, Buttons[BUTTON_MINUS_SPEED].b, Buttons[BUTTON_MINUS_SPEED].f, Buttons[BUTTON_MINUS_SPEED].p, "-  ");

  //Draw the Time Plus and Minus buttons
  draw_Button(Buttons[BUTTON_PLUS_TIME].x, Buttons[BUTTON_PLUS_TIME].y, Buttons[BUTTON_PLUS_TIME].w, Buttons[BUTTON_PLUS_TIME].h, Buttons[BUTTON_PLUS_TIME].b, Buttons[BUTTON_PLUS_TIME].f, Buttons[BUTTON_PLUS_TIME].p, "+  ");
  draw_Button(Buttons[BUTTON_MINUS_TIME].x, Buttons[BUTTON_MINUS_TIME].y, Buttons[BUTTON_MINUS_TIME].w, Buttons[BUTTON_MINUS_TIME].h, Buttons[BUTTON_MINUS_TIME].b, Buttons[BUTTON_MINUS_TIME].f, Buttons[BUTTON_MINUS_TIME].p, "-  ");

  //Draw the Speed & elapsed Icon
  tft.fillRect(Buttons[BUTTON_SPEED_ICON].x, Buttons[BUTTON_SPEED_ICON].y, Buttons[BUTTON_SPEED_ICON].x + Buttons[BUTTON_SPEED_ICON].w, Buttons[BUTTON_SPEED_ICON].y + Buttons[BUTTON_SPEED_ICON].h, BLUE_CUSTOM);
  tft.pic(MESCOLAMENTO_45, Buttons[BUTTON_SPEED_ICON].x + 5, Buttons[BUTTON_SPEED_ICON].y + 25);

  tft.fillRect(Buttons[BUTTON_ELAPSED_ICON].x, Buttons[BUTTON_ELAPSED_ICON].y, Buttons[BUTTON_ELAPSED_ICON].x + Buttons[BUTTON_ELAPSED_ICON].w, Buttons[BUTTON_ELAPSED_ICON].y + Buttons[BUTTON_ELAPSED_ICON].h, BLUE_CUSTOM);
  tft.pic(TIMER_45, Buttons[BUTTON_ELAPSED_ICON].x + 5, Buttons[BUTTON_ELAPSED_ICON].y + 22);

    // Draw the Locked icon above ON button
  //if (Checksum != KeyCode)
  //  tft.pic(PIC_LOCKED_90, Buttons[BUTTON_PROTECTION].x, Buttons[BUTTON_PROTECTION].y);

  draw_Presets();
}

void Update_NumPadValue()
{
  draw_Button(NumPad[NUMPAD_DISPLAY].x, NumPad[NUMPAD_DISPLAY].y, NumPad[NUMPAD_DISPLAY].w, NumPad[NUMPAD_DISPLAY].h, NumPad[NUMPAD_DISPLAY].b, NumPad[NUMPAD_DISPLAY].f, NumPad[NUMPAD_DISPLAY].p, String(Number));
}

void Update_Speed_Box()
{
  String dummy_s;
  
  dummy_s = String(Speed_Set) + "% ";
  draw_Label(Buttons[BUTTON_SPEED_SET].x, Buttons[BUTTON_SPEED_SET].y, Buttons[BUTTON_SPEED_SET].w, Buttons[BUTTON_SPEED_SET].h, Buttons[BUTTON_SPEED_SET].b, Buttons[BUTTON_SPEED_SET].f, NumPad[BUTTON_SPEED_SET].p, dummy_s);
}

void Update_Time_Box()
{
  String dummy_s;

  Update_Elapsed_Box();
}

void Update_Speed_Graph(struct_bargraph SG, int v)
{
  SG.v = Act_Speed;
  draw_BarGraph(SG);
}

void Update_Elapsed_Box()
{
  String dummy_s, tot;
  int len;

  if (Status == MACHINE_OFF)
  {
    dummy_s = String(Time_Set / 60);
    dummy_s = "00" + dummy_s;
    len = dummy_s.length();
    tot = dummy_s.substring(len - 2) + ":";
    dummy_s = String(Time_Set % 60);
    dummy_s = "00" + dummy_s;
    len = dummy_s.length();
    tot = tot + dummy_s.substring(len - 2) + " ";
  }
  else
  {
    dummy_s = String(Act_Elapsed_Minutes);
    dummy_s = "00" + dummy_s;
    len = dummy_s.length();
    tot = dummy_s.substring(len - 2) + ":";
    dummy_s = String(Act_Elapsed_Seconds);
    dummy_s = "00" + dummy_s;
    len = dummy_s.length();
    tot = tot + dummy_s.substring(len - 2) + " ";
  }
  
  draw_Label(Buttons[BUTTON_ELAPSED].x, Buttons[BUTTON_ELAPSED].y, Buttons[BUTTON_ELAPSED].w, Buttons[BUTTON_ELAPSED].h, Buttons[BUTTON_ELAPSED].b, Buttons[BUTTON_ELAPSED].f, NumPad[BUTTON_ELAPSED].p, tot);
}

void ActivityLed()
{
  static bool Led = 0;

  if (!Clocks.c_os_100ms)
    return;

  Led = !Led;
  
  if (Led)
    setOut7;
  else
    resOut7;
}

void out_4_20mA(int vv)
{
  float range, val;

  val = (float)vv;
  range = (float)(range_20mA - range_4mA);
  val = ((val * range) / 100.0) + range_4mA;

  analogWrite(A0, (int)val);
}

void UpdateMachineOutputs()
{
  static long InverterResetStart;
  
  if (!Clocks.c_os_100ms)
    return;

  if (Status == MACHINE_ON)
    OUT_INVERTER_RUN_HIGH;
  else
    OUT_INVERTER_RUN_LOW;

  if ((millis() - InverterResetStart) >= 1000)
    InverterReset = 0;
    
  if (InverterReset)
    OUT_INVERTER_RESET_HIGH;
  else
  {
    OUT_INVERTER_RESET_LOW;
    InverterResetStart = millis();
  }

  out_4_20mA(Act_Speed);
}

void BlinkBitsGeneration()
{
  // 1s period
  BlinkUpdate_1s = 0;
  BlinkShow_1s = Clocks.c_1s;
  
  if (Clocks.c_os_1s || Clocks.c_os_n_1s)
    BlinkUpdate_1s = 1;

  // 500ms period
  BlinkUpdate_500ms = 0;
  BlinkShow_500ms = Clocks.c_500ms;
  
  if (Clocks.c_os_500ms || Clocks.c_os_n_500ms)
    BlinkUpdate_500ms = 1;
}

void loop() 
{
  static int Pressed_Button = 0;
  static int Step = 0, FollowingStep = 0;
  static bool Touch_pressed;
  static struct_TouchPoint p;
  static long last;
  static bool debounce = 0;
  static long debounce_time = 0;
  static int X,Y;
  static int KeyLongPressed = 0;
  static long LastKeyPressedTime = 0;
  
  ClocksGenerator();
  ActivityLed();
  UpdateMachineOutputs();
  updateIO();  
  BlinkBitsGeneration();

  if ((Step == 100) || (Step == 102))
  {
    LightsAndIconsUpdate();
    Update_TimeCounter();
  }

  if (Status == MACHINE_ON)
    Act_Speed = Speed_Set;
  else
    Act_Speed = 0;

  switch(Step)
  {
    // Init
    case 0:
      draw_MainPage();
      Update_Elapsed_Box();
      Update_Speed_Box();
      Update_Time_Box();
      Step = 100;
      break;

    // Main page
    case 100:
      debounce_time = 100;
      debounce = 0;        
      
      // check touch screen for new events
      Touch_pressed = Touch_Event(p);
              
      if (Touch_pressed == true) 
      {
        LastKeyPressedTime = millis();
        Pressed_Button = DetectMainButtons(p.x, p.y);

        switch(Pressed_Button)
        {
          case BUTTON_PLUS_SPEED:
            if (Speed_Set < MAX_SPEED)
            {
              Speed_Set++;
              Update_Speed_Box();
              debounce = 0;
            } 
            break;
    
          case BUTTON_MINUS_SPEED:
            if (Speed_Set > MIN_SPEED)
            {
              Speed_Set--;
              Update_Speed_Box();
              debounce = 0;
            } 
            break;
    
          case BUTTON_SPEED_SET:
            Step = 110;
            FollowingStep = 200;
            break;
    
          case BUTTON_PLUS_TIME:
            if (Time_Set < MAX_TIME)
            {
              Time_Set++;
              Update_Elapsed_Box();
              debounce = 0;
            } 
            break;
    
          case BUTTON_MINUS_TIME:
            if (Time_Set > MIN_TIME)
            {
              Time_Set--;
              Update_Elapsed_Box();
              debounce = 0;
            } 
            break;
    
          case BUTTON_ELAPSED:
            Step = 110;
            FollowingStep = 210;
            break;
    
          case BUTTON_MAN:
            if (Status == MACHINE_OFF)
              Mode = MACHINE_MANUAL;
            break;
    
          case BUTTON_AUTO:
            if (Status == MACHINE_OFF)
              Mode = MACHINE_AUTO;
            break;
    
          case BUTTON_ON:
            if (Status == MACHINE_OFF)
              if (Mode == MACHINE_AUTO)
                Act_Elapsed = Time_Set; // Act_Elapsed = Time_Set * 60;
              else
                Act_Elapsed = 0;

            Status = MACHINE_ON;
            break;
    
          case BUTTON_OFF:
            Status = MACHINE_OFF;
            break;
    
          case BUTTON_COMPANY:
            if (Status == MACHINE_OFF) 
            {
              if (++KeyLongPressed > 20)
                Step = 300;
            }
            break;
    
          default:
            //debounce = 0;
            break;
        }

        Pressed_Button = BUTTON_NULL;

        if (debounce)
        {
          last = millis();
          Step = 102;
          break;
        }

        /* *************** */
        // Preset values
        /* *************** */
        Pressed_Button = DetectPresetButtons(p.x, p.y);
        debounce = 0;
        
        switch(Pressed_Button)
        {
          case PRESET_1:
          case PRESET_2:
          case PRESET_3:
          case PRESET_4:
          case PRESET_5:
          case PRESET_6:
          case PRESET_7:
          case PRESET_8:
            if (SavePreset)
            {
              if (++KeyLongPressed > 20)
              {
                Sets[Pressed_Button].Time = Time_Set;
                Sets[Pressed_Button].Speed = Speed_Set;
                SavePreset = 0;
                draw_Presets();
              }
            }
            else
            {
              if (Status != MACHINE_OFF)
                break;

              Time_Set = Sets[Pressed_Button].Time;
              Speed_Set = Sets[Pressed_Button].Speed;
              Update_Speed_Box();
              Update_Time_Box();
              debounce = 1;
            }
            break;

          default:
            break;
        }
      
        Pressed_Button = BUTTON_NULL;

        if (debounce)
        {
          last = millis();
          Step = 102;
          break;
        }
      }

      // used for memorization of preset setpoints
      if ((millis() - LastKeyPressedTime) >= 1000)
      {
        LastKeyPressedTime = millis();
        KeyLongPressed = 0;
      }
      break;

    // wait until XXXms not pressed to avoid bouncing keys
    case 102:
      Touch_pressed = Touch_Event(p);
      if (Touch_pressed == true) 
        last = millis(); 

      if ((millis() - last) > debounce_time)
        Step = 100;
      break;

    // NumPad init
    case 110:
      Number = 0;
      draw_NumPad(" ");
      last = millis(); 
      Step = 112;
      break;

    // wait until 1s not pressed to avoid bouncing keys
    case 112:
      Touch_pressed = Touch_Event(p);
      if (Touch_pressed == true) 
        last = millis(); 

      if ((millis() - last) > 500)
        Step = 115;
      break;
      
    // NumPad handling
    case 115:
      Touch_pressed = Touch_Event(p);
      if (Touch_pressed == true) 
      { 
        Pressed_Button = HandleNumPadButtons(p.x, p.y);
        
        if (Pressed_Button == 1) 
        {
          last = millis(); 
          Step = 120;
        }
        
        if (Pressed_Button == -1)
          Step = 0;
      } 
      break;

    // wait until 1s not pressed to avoid bouncing keys
    case 120:
      Touch_pressed = Touch_Event(p);
      if (Touch_pressed == true) 
        last = millis(); 

      if ((millis() - last) > 250)
        Step = FollowingStep;
      break;

    // Speed Set
    case 200:
      if (Number < MIN_SPEED)
        Number = MIN_SPEED;

      if (Number > MAX_SPEED)
        Number = MAX_SPEED;

      Speed_Set = Number;
      Step = 0;
      break;

    // Time Set
    case 210:
      if ((Number * 60) < MIN_TIME)
        Number = MIN_TIME / 60;

      if ((Number * 60) > MAX_TIME)
        Number = MAX_TIME / 60;

      Time_Set = Number * 60;
      Step = 0;
      break;

   // Info screen
    case 300:
      InfoScreen();
      last = millis();
      Step = 302;
      break;

    // wait until XXXms not pressed to avoid bouncing keys
    case 302:
      Touch_pressed = Touch_Event(p);
      if (Touch_pressed == true) 
        last = millis(); 

      if ((millis() - last) > debounce_time)
        Step = 0;
      break;

    default:
      break;
  }
}
