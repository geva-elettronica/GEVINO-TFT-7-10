/*********************************************************************/
//
//  Temi GLASS + FLUO + ALU in un unico binario - RAiO RA887x, 1024x600, RGB565
//
//  Generato da tools/theme_merge.py -- NON modificare a mano.
//
//  Un solo All_76_Pic_65K.bin sulla microSD contiene tutti i temi:
//  lo sketch cambia aspetto puntando a un altro blocco di indici,
//  senza riprogrammare la flash.
//
//    glass  indici     0..30     (31 immagini)
//    fluo   indici    31..387    (357 immagini)
//    alu    indici   388..507    (120 immagini)
//
/*********************************************************************/
#ifndef _RAIO_INFO_STRUCT_
#define _RAIO_INFO_STRUCT_
typedef struct _info
{
  unsigned short number;
  unsigned short img_width;
  unsigned short img_height;
  unsigned long img_size;
  unsigned long start_addr;
}INFO;
#endif

  /* The 'code' is KEIL C 8051 instruction. Rimuovere 'code' se non si usa 8051. */

const INFO code BINARY_INFO[508]=
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
  {32,105,40,8400,387990},          /*     fluo_rect_105_40_red_released , element 31     */
  {33,105,40,8400,396390},          /*     fluo_rect_105_40_red_pressed , element 32     */
  {34,105,40,8400,404790},          /*     fluo_rect_105_40_green_released , element 33     */
  {35,105,40,8400,413190},          /*     fluo_rect_105_40_green_pressed , element 34     */
  {36,105,40,8400,421590},          /*     fluo_rect_105_40_blue_released , element 35     */
  {37,105,40,8400,429990},          /*     fluo_rect_105_40_blue_pressed , element 36     */
  {38,105,40,8400,438390},          /*     fluo_rect_105_40_cyan_released , element 37     */
  {39,105,40,8400,446790},          /*     fluo_rect_105_40_cyan_pressed , element 38     */
  {40,105,40,8400,455190},          /*     fluo_rect_105_40_magenta_released , element 39     */
  {41,105,40,8400,463590},          /*     fluo_rect_105_40_magenta_pressed , element 40     */
  {42,105,40,8400,471990},          /*     fluo_rect_105_40_yellow_released , element 41     */
  {43,105,40,8400,480390},          /*     fluo_rect_105_40_yellow_pressed , element 42     */
  {44,105,40,8400,488790},          /*     fluo_rect_105_40_white_released , element 43     */
  {45,105,40,8400,497190},          /*     fluo_rect_105_40_white_pressed , element 44     */
  {46,105,55,11550,505590},          /*     fluo_rect_105_55_red_released , element 45     */
  {47,105,55,11550,517140},          /*     fluo_rect_105_55_red_pressed , element 46     */
  {48,105,55,11550,528690},          /*     fluo_rect_105_55_green_released , element 47     */
  {49,105,55,11550,540240},          /*     fluo_rect_105_55_green_pressed , element 48     */
  {50,105,55,11550,551790},          /*     fluo_rect_105_55_blue_released , element 49     */
  {51,105,55,11550,563340},          /*     fluo_rect_105_55_blue_pressed , element 50     */
  {52,105,55,11550,574890},          /*     fluo_rect_105_55_cyan_released , element 51     */
  {53,105,55,11550,586440},          /*     fluo_rect_105_55_cyan_pressed , element 52     */
  {54,105,55,11550,597990},          /*     fluo_rect_105_55_magenta_released , element 53     */
  {55,105,55,11550,609540},          /*     fluo_rect_105_55_magenta_pressed , element 54     */
  {56,105,55,11550,621090},          /*     fluo_rect_105_55_yellow_released , element 55     */
  {57,105,55,11550,632640},          /*     fluo_rect_105_55_yellow_pressed , element 56     */
  {58,105,55,11550,644190},          /*     fluo_rect_105_55_white_released , element 57     */
  {59,105,55,11550,655740},          /*     fluo_rect_105_55_white_pressed , element 58     */
  {60,105,70,14700,667290},          /*     fluo_rect_105_70_red_released , element 59     */
  {61,105,70,14700,681990},          /*     fluo_rect_105_70_red_pressed , element 60     */
  {62,105,70,14700,696690},          /*     fluo_rect_105_70_green_released , element 61     */
  {63,105,70,14700,711390},          /*     fluo_rect_105_70_green_pressed , element 62     */
  {64,105,70,14700,726090},          /*     fluo_rect_105_70_blue_released , element 63     */
  {65,105,70,14700,740790},          /*     fluo_rect_105_70_blue_pressed , element 64     */
  {66,105,70,14700,755490},          /*     fluo_rect_105_70_cyan_released , element 65     */
  {67,105,70,14700,770190},          /*     fluo_rect_105_70_cyan_pressed , element 66     */
  {68,105,70,14700,784890},          /*     fluo_rect_105_70_magenta_released , element 67     */
  {69,105,70,14700,799590},          /*     fluo_rect_105_70_magenta_pressed , element 68     */
  {70,105,70,14700,814290},          /*     fluo_rect_105_70_yellow_released , element 69     */
  {71,105,70,14700,828990},          /*     fluo_rect_105_70_yellow_pressed , element 70     */
  {72,105,70,14700,843690},          /*     fluo_rect_105_70_white_released , element 71     */
  {73,105,70,14700,858390},          /*     fluo_rect_105_70_white_pressed , element 72     */
  {74,146,40,11680,873090},          /*     fluo_rect_146_40_red_released , element 73     */
  {75,146,40,11680,884770},          /*     fluo_rect_146_40_red_pressed , element 74     */
  {76,146,40,11680,896450},          /*     fluo_rect_146_40_green_released , element 75     */
  {77,146,40,11680,908130},          /*     fluo_rect_146_40_green_pressed , element 76     */
  {78,146,40,11680,919810},          /*     fluo_rect_146_40_blue_released , element 77     */
  {79,146,40,11680,931490},          /*     fluo_rect_146_40_blue_pressed , element 78     */
  {80,146,40,11680,943170},          /*     fluo_rect_146_40_cyan_released , element 79     */
  {81,146,40,11680,954850},          /*     fluo_rect_146_40_cyan_pressed , element 80     */
  {82,146,40,11680,966530},          /*     fluo_rect_146_40_magenta_released , element 81     */
  {83,146,40,11680,978210},          /*     fluo_rect_146_40_magenta_pressed , element 82     */
  {84,146,40,11680,989890},          /*     fluo_rect_146_40_yellow_released , element 83     */
  {85,146,40,11680,1001570},          /*     fluo_rect_146_40_yellow_pressed , element 84     */
  {86,146,40,11680,1013250},          /*     fluo_rect_146_40_white_released , element 85     */
  {87,146,40,11680,1024930},          /*     fluo_rect_146_40_white_pressed , element 86     */
  {88,146,55,16060,1036610},          /*     fluo_rect_146_55_red_released , element 87     */
  {89,146,55,16060,1052670},          /*     fluo_rect_146_55_red_pressed , element 88     */
  {90,146,55,16060,1068730},          /*     fluo_rect_146_55_green_released , element 89     */
  {91,146,55,16060,1084790},          /*     fluo_rect_146_55_green_pressed , element 90     */
  {92,146,55,16060,1100850},          /*     fluo_rect_146_55_blue_released , element 91     */
  {93,146,55,16060,1116910},          /*     fluo_rect_146_55_blue_pressed , element 92     */
  {94,146,55,16060,1132970},          /*     fluo_rect_146_55_cyan_released , element 93     */
  {95,146,55,16060,1149030},          /*     fluo_rect_146_55_cyan_pressed , element 94     */
  {96,146,55,16060,1165090},          /*     fluo_rect_146_55_magenta_released , element 95     */
  {97,146,55,16060,1181150},          /*     fluo_rect_146_55_magenta_pressed , element 96     */
  {98,146,55,16060,1197210},          /*     fluo_rect_146_55_yellow_released , element 97     */
  {99,146,55,16060,1213270},          /*     fluo_rect_146_55_yellow_pressed , element 98     */
  {100,146,55,16060,1229330},          /*     fluo_rect_146_55_white_released , element 99     */
  {101,146,55,16060,1245390},          /*     fluo_rect_146_55_white_pressed , element 100     */
  {102,146,70,20440,1261450},          /*     fluo_rect_146_70_red_released , element 101     */
  {103,146,70,20440,1281890},          /*     fluo_rect_146_70_red_pressed , element 102     */
  {104,146,70,20440,1302330},          /*     fluo_rect_146_70_green_released , element 103     */
  {105,146,70,20440,1322770},          /*     fluo_rect_146_70_green_pressed , element 104     */
  {106,146,70,20440,1343210},          /*     fluo_rect_146_70_blue_released , element 105     */
  {107,146,70,20440,1363650},          /*     fluo_rect_146_70_blue_pressed , element 106     */
  {108,146,70,20440,1384090},          /*     fluo_rect_146_70_cyan_released , element 107     */
  {109,146,70,20440,1404530},          /*     fluo_rect_146_70_cyan_pressed , element 108     */
  {110,146,70,20440,1424970},          /*     fluo_rect_146_70_magenta_released , element 109     */
  {111,146,70,20440,1445410},          /*     fluo_rect_146_70_magenta_pressed , element 110     */
  {112,146,70,20440,1465850},          /*     fluo_rect_146_70_yellow_released , element 111     */
  {113,146,70,20440,1486290},          /*     fluo_rect_146_70_yellow_pressed , element 112     */
  {114,146,70,20440,1506730},          /*     fluo_rect_146_70_white_released , element 113     */
  {115,146,70,20440,1527170},          /*     fluo_rect_146_70_white_pressed , element 114     */
  {116,251,40,20080,1547610},          /*     fluo_rect_251_40_red_released , element 115     */
  {117,251,40,20080,1567690},          /*     fluo_rect_251_40_red_pressed , element 116     */
  {118,251,40,20080,1587770},          /*     fluo_rect_251_40_green_released , element 117     */
  {119,251,40,20080,1607850},          /*     fluo_rect_251_40_green_pressed , element 118     */
  {120,251,40,20080,1627930},          /*     fluo_rect_251_40_blue_released , element 119     */
  {121,251,40,20080,1648010},          /*     fluo_rect_251_40_blue_pressed , element 120     */
  {122,251,40,20080,1668090},          /*     fluo_rect_251_40_cyan_released , element 121     */
  {123,251,40,20080,1688170},          /*     fluo_rect_251_40_cyan_pressed , element 122     */
  {124,251,40,20080,1708250},          /*     fluo_rect_251_40_magenta_released , element 123     */
  {125,251,40,20080,1728330},          /*     fluo_rect_251_40_magenta_pressed , element 124     */
  {126,251,40,20080,1748410},          /*     fluo_rect_251_40_yellow_released , element 125     */
  {127,251,40,20080,1768490},          /*     fluo_rect_251_40_yellow_pressed , element 126     */
  {128,251,40,20080,1788570},          /*     fluo_rect_251_40_white_released , element 127     */
  {129,251,40,20080,1808650},          /*     fluo_rect_251_40_white_pressed , element 128     */
  {130,251,55,27610,1828730},          /*     fluo_rect_251_55_red_released , element 129     */
  {131,251,55,27610,1856340},          /*     fluo_rect_251_55_red_pressed , element 130     */
  {132,251,55,27610,1883950},          /*     fluo_rect_251_55_green_released , element 131     */
  {133,251,55,27610,1911560},          /*     fluo_rect_251_55_green_pressed , element 132     */
  {134,251,55,27610,1939170},          /*     fluo_rect_251_55_blue_released , element 133     */
  {135,251,55,27610,1966780},          /*     fluo_rect_251_55_blue_pressed , element 134     */
  {136,251,55,27610,1994390},          /*     fluo_rect_251_55_cyan_released , element 135     */
  {137,251,55,27610,2022000},          /*     fluo_rect_251_55_cyan_pressed , element 136     */
  {138,251,55,27610,2049610},          /*     fluo_rect_251_55_magenta_released , element 137     */
  {139,251,55,27610,2077220},          /*     fluo_rect_251_55_magenta_pressed , element 138     */
  {140,251,55,27610,2104830},          /*     fluo_rect_251_55_yellow_released , element 139     */
  {141,251,55,27610,2132440},          /*     fluo_rect_251_55_yellow_pressed , element 140     */
  {142,251,55,27610,2160050},          /*     fluo_rect_251_55_white_released , element 141     */
  {143,251,55,27610,2187660},          /*     fluo_rect_251_55_white_pressed , element 142     */
  {144,251,70,35140,2215270},          /*     fluo_rect_251_70_red_released , element 143     */
  {145,251,70,35140,2250410},          /*     fluo_rect_251_70_red_pressed , element 144     */
  {146,251,70,35140,2285550},          /*     fluo_rect_251_70_green_released , element 145     */
  {147,251,70,35140,2320690},          /*     fluo_rect_251_70_green_pressed , element 146     */
  {148,251,70,35140,2355830},          /*     fluo_rect_251_70_blue_released , element 147     */
  {149,251,70,35140,2390970},          /*     fluo_rect_251_70_blue_pressed , element 148     */
  {150,251,70,35140,2426110},          /*     fluo_rect_251_70_cyan_released , element 149     */
  {151,251,70,35140,2461250},          /*     fluo_rect_251_70_cyan_pressed , element 150     */
  {152,251,70,35140,2496390},          /*     fluo_rect_251_70_magenta_released , element 151     */
  {153,251,70,35140,2531530},          /*     fluo_rect_251_70_magenta_pressed , element 152     */
  {154,251,70,35140,2566670},          /*     fluo_rect_251_70_yellow_released , element 153     */
  {155,251,70,35140,2601810},          /*     fluo_rect_251_70_yellow_pressed , element 154     */
  {156,251,70,35140,2636950},          /*     fluo_rect_251_70_white_released , element 155     */
  {157,251,70,35140,2672090},          /*     fluo_rect_251_70_white_pressed , element 156     */
  {158,31,31,1922,2707230},          /*     fluo_round_31_red_released , element 157     */
  {159,31,31,1922,2709152},          /*     fluo_round_31_red_pressed , element 158     */
  {160,31,31,1922,2711074},          /*     fluo_round_31_green_released , element 159     */
  {161,31,31,1922,2712996},          /*     fluo_round_31_green_pressed , element 160     */
  {162,31,31,1922,2714918},          /*     fluo_round_31_blue_released , element 161     */
  {163,31,31,1922,2716840},          /*     fluo_round_31_blue_pressed , element 162     */
  {164,31,31,1922,2718762},          /*     fluo_round_31_cyan_released , element 163     */
  {165,31,31,1922,2720684},          /*     fluo_round_31_cyan_pressed , element 164     */
  {166,31,31,1922,2722606},          /*     fluo_round_31_magenta_released , element 165     */
  {167,31,31,1922,2724528},          /*     fluo_round_31_magenta_pressed , element 166     */
  {168,31,31,1922,2726450},          /*     fluo_round_31_yellow_released , element 167     */
  {169,31,31,1922,2728372},          /*     fluo_round_31_yellow_pressed , element 168     */
  {170,31,31,1922,2730294},          /*     fluo_round_31_white_released , element 169     */
  {171,31,31,1922,2732216},          /*     fluo_round_31_white_pressed , element 170     */
  {172,48,48,4608,2734138},          /*     fluo_round_48_red_released , element 171     */
  {173,48,48,4608,2738746},          /*     fluo_round_48_red_pressed , element 172     */
  {174,48,48,4608,2743354},          /*     fluo_round_48_green_released , element 173     */
  {175,48,48,4608,2747962},          /*     fluo_round_48_green_pressed , element 174     */
  {176,48,48,4608,2752570},          /*     fluo_round_48_blue_released , element 175     */
  {177,48,48,4608,2757178},          /*     fluo_round_48_blue_pressed , element 176     */
  {178,48,48,4608,2761786},          /*     fluo_round_48_cyan_released , element 177     */
  {179,48,48,4608,2766394},          /*     fluo_round_48_cyan_pressed , element 178     */
  {180,48,48,4608,2771002},          /*     fluo_round_48_magenta_released , element 179     */
  {181,48,48,4608,2775610},          /*     fluo_round_48_magenta_pressed , element 180     */
  {182,48,48,4608,2780218},          /*     fluo_round_48_yellow_released , element 181     */
  {183,48,48,4608,2784826},          /*     fluo_round_48_yellow_pressed , element 182     */
  {184,48,48,4608,2789434},          /*     fluo_round_48_white_released , element 183     */
  {185,48,48,4608,2794042},          /*     fluo_round_48_white_pressed , element 184     */
  {186,60,60,7200,2798650},          /*     fluo_round_60_red_released , element 185     */
  {187,60,60,7200,2805850},          /*     fluo_round_60_red_pressed , element 186     */
  {188,60,60,7200,2813050},          /*     fluo_round_60_green_released , element 187     */
  {189,60,60,7200,2820250},          /*     fluo_round_60_green_pressed , element 188     */
  {190,60,60,7200,2827450},          /*     fluo_round_60_blue_released , element 189     */
  {191,60,60,7200,2834650},          /*     fluo_round_60_blue_pressed , element 190     */
  {192,60,60,7200,2841850},          /*     fluo_round_60_cyan_released , element 191     */
  {193,60,60,7200,2849050},          /*     fluo_round_60_cyan_pressed , element 192     */
  {194,60,60,7200,2856250},          /*     fluo_round_60_magenta_released , element 193     */
  {195,60,60,7200,2863450},          /*     fluo_round_60_magenta_pressed , element 194     */
  {196,60,60,7200,2870650},          /*     fluo_round_60_yellow_released , element 195     */
  {197,60,60,7200,2877850},          /*     fluo_round_60_yellow_pressed , element 196     */
  {198,60,60,7200,2885050},          /*     fluo_round_60_white_released , element 197     */
  {199,60,60,7200,2892250},          /*     fluo_round_60_white_pressed , element 198     */
  {200,70,70,9800,2899450},          /*     fluo_round_70_red_released , element 199     */
  {201,70,70,9800,2909250},          /*     fluo_round_70_red_pressed , element 200     */
  {202,70,70,9800,2919050},          /*     fluo_round_70_green_released , element 201     */
  {203,70,70,9800,2928850},          /*     fluo_round_70_green_pressed , element 202     */
  {204,70,70,9800,2938650},          /*     fluo_round_70_blue_released , element 203     */
  {205,70,70,9800,2948450},          /*     fluo_round_70_blue_pressed , element 204     */
  {206,70,70,9800,2958250},          /*     fluo_round_70_cyan_released , element 205     */
  {207,70,70,9800,2968050},          /*     fluo_round_70_cyan_pressed , element 206     */
  {208,70,70,9800,2977850},          /*     fluo_round_70_magenta_released , element 207     */
  {209,70,70,9800,2987650},          /*     fluo_round_70_magenta_pressed , element 208     */
  {210,70,70,9800,2997450},          /*     fluo_round_70_yellow_released , element 209     */
  {211,70,70,9800,3007250},          /*     fluo_round_70_yellow_pressed , element 210     */
  {212,70,70,9800,3017050},          /*     fluo_round_70_white_released , element 211     */
  {213,70,70,9800,3026850},          /*     fluo_round_70_white_pressed , element 212     */
  {214,20,20,800,3036650},          /*     fluo_led_20_red_off , element 213     */
  {215,20,20,800,3037450},          /*     fluo_led_20_red_on , element 214     */
  {216,20,20,800,3038250},          /*     fluo_led_20_green_off , element 215     */
  {217,20,20,800,3039050},          /*     fluo_led_20_green_on , element 216     */
  {218,20,20,800,3039850},          /*     fluo_led_20_blue_off , element 217     */
  {219,20,20,800,3040650},          /*     fluo_led_20_blue_on , element 218     */
  {220,20,20,800,3041450},          /*     fluo_led_20_cyan_off , element 219     */
  {221,20,20,800,3042250},          /*     fluo_led_20_cyan_on , element 220     */
  {222,20,20,800,3043050},          /*     fluo_led_20_magenta_off , element 221     */
  {223,20,20,800,3043850},          /*     fluo_led_20_magenta_on , element 222     */
  {224,20,20,800,3044650},          /*     fluo_led_20_yellow_off , element 223     */
  {225,20,20,800,3045450},          /*     fluo_led_20_yellow_on , element 224     */
  {226,20,20,800,3046250},          /*     fluo_led_20_white_off , element 225     */
  {227,20,20,800,3047050},          /*     fluo_led_20_white_on , element 226     */
  {228,31,31,1922,3047850},          /*     fluo_led_31_red_off , element 227     */
  {229,31,31,1922,3049772},          /*     fluo_led_31_red_on , element 228     */
  {230,31,31,1922,3051694},          /*     fluo_led_31_green_off , element 229     */
  {231,31,31,1922,3053616},          /*     fluo_led_31_green_on , element 230     */
  {232,31,31,1922,3055538},          /*     fluo_led_31_blue_off , element 231     */
  {233,31,31,1922,3057460},          /*     fluo_led_31_blue_on , element 232     */
  {234,31,31,1922,3059382},          /*     fluo_led_31_cyan_off , element 233     */
  {235,31,31,1922,3061304},          /*     fluo_led_31_cyan_on , element 234     */
  {236,31,31,1922,3063226},          /*     fluo_led_31_magenta_off , element 235     */
  {237,31,31,1922,3065148},          /*     fluo_led_31_magenta_on , element 236     */
  {238,31,31,1922,3067070},          /*     fluo_led_31_yellow_off , element 237     */
  {239,31,31,1922,3068992},          /*     fluo_led_31_yellow_on , element 238     */
  {240,31,31,1922,3070914},          /*     fluo_led_31_white_off , element 239     */
  {241,31,31,1922,3072836},          /*     fluo_led_31_white_on , element 240     */
  {242,48,48,4608,3074758},          /*     fluo_led_48_red_off , element 241     */
  {243,48,48,4608,3079366},          /*     fluo_led_48_red_on , element 242     */
  {244,48,48,4608,3083974},          /*     fluo_led_48_green_off , element 243     */
  {245,48,48,4608,3088582},          /*     fluo_led_48_green_on , element 244     */
  {246,48,48,4608,3093190},          /*     fluo_led_48_blue_off , element 245     */
  {247,48,48,4608,3097798},          /*     fluo_led_48_blue_on , element 246     */
  {248,48,48,4608,3102406},          /*     fluo_led_48_cyan_off , element 247     */
  {249,48,48,4608,3107014},          /*     fluo_led_48_cyan_on , element 248     */
  {250,48,48,4608,3111622},          /*     fluo_led_48_magenta_off , element 249     */
  {251,48,48,4608,3116230},          /*     fluo_led_48_magenta_on , element 250     */
  {252,48,48,4608,3120838},          /*     fluo_led_48_yellow_off , element 251     */
  {253,48,48,4608,3125446},          /*     fluo_led_48_yellow_on , element 252     */
  {254,48,48,4608,3130054},          /*     fluo_led_48_white_off , element 253     */
  {255,48,48,4608,3134662},          /*     fluo_led_48_white_on , element 254     */
  {256,28,28,1568,3139270},          /*     fluo_check_28_red_off , element 255     */
  {257,28,28,1568,3140838},          /*     fluo_check_28_red_on , element 256     */
  {258,28,28,1568,3142406},          /*     fluo_check_28_green_off , element 257     */
  {259,28,28,1568,3143974},          /*     fluo_check_28_green_on , element 258     */
  {260,28,28,1568,3145542},          /*     fluo_check_28_blue_off , element 259     */
  {261,28,28,1568,3147110},          /*     fluo_check_28_blue_on , element 260     */
  {262,28,28,1568,3148678},          /*     fluo_check_28_cyan_off , element 261     */
  {263,28,28,1568,3150246},          /*     fluo_check_28_cyan_on , element 262     */
  {264,28,28,1568,3151814},          /*     fluo_check_28_magenta_off , element 263     */
  {265,28,28,1568,3153382},          /*     fluo_check_28_magenta_on , element 264     */
  {266,28,28,1568,3154950},          /*     fluo_check_28_yellow_off , element 265     */
  {267,28,28,1568,3156518},          /*     fluo_check_28_yellow_on , element 266     */
  {268,28,28,1568,3158086},          /*     fluo_check_28_white_off , element 267     */
  {269,28,28,1568,3159654},          /*     fluo_check_28_white_on , element 268     */
  {270,36,36,2592,3161222},          /*     fluo_check_36_red_off , element 269     */
  {271,36,36,2592,3163814},          /*     fluo_check_36_red_on , element 270     */
  {272,36,36,2592,3166406},          /*     fluo_check_36_green_off , element 271     */
  {273,36,36,2592,3168998},          /*     fluo_check_36_green_on , element 272     */
  {274,36,36,2592,3171590},          /*     fluo_check_36_blue_off , element 273     */
  {275,36,36,2592,3174182},          /*     fluo_check_36_blue_on , element 274     */
  {276,36,36,2592,3176774},          /*     fluo_check_36_cyan_off , element 275     */
  {277,36,36,2592,3179366},          /*     fluo_check_36_cyan_on , element 276     */
  {278,36,36,2592,3181958},          /*     fluo_check_36_magenta_off , element 277     */
  {279,36,36,2592,3184550},          /*     fluo_check_36_magenta_on , element 278     */
  {280,36,36,2592,3187142},          /*     fluo_check_36_yellow_off , element 279     */
  {281,36,36,2592,3189734},          /*     fluo_check_36_yellow_on , element 280     */
  {282,36,36,2592,3192326},          /*     fluo_check_36_white_off , element 281     */
  {283,36,36,2592,3194918},          /*     fluo_check_36_white_on , element 282     */
  {284,48,48,4608,3197510},          /*     fluo_check_48_red_off , element 283     */
  {285,48,48,4608,3202118},          /*     fluo_check_48_red_on , element 284     */
  {286,48,48,4608,3206726},          /*     fluo_check_48_green_off , element 285     */
  {287,48,48,4608,3211334},          /*     fluo_check_48_green_on , element 286     */
  {288,48,48,4608,3215942},          /*     fluo_check_48_blue_off , element 287     */
  {289,48,48,4608,3220550},          /*     fluo_check_48_blue_on , element 288     */
  {290,48,48,4608,3225158},          /*     fluo_check_48_cyan_off , element 289     */
  {291,48,48,4608,3229766},          /*     fluo_check_48_cyan_on , element 290     */
  {292,48,48,4608,3234374},          /*     fluo_check_48_magenta_off , element 291     */
  {293,48,48,4608,3238982},          /*     fluo_check_48_magenta_on , element 292     */
  {294,48,48,4608,3243590},          /*     fluo_check_48_yellow_off , element 293     */
  {295,48,48,4608,3248198},          /*     fluo_check_48_yellow_on , element 294     */
  {296,48,48,4608,3252806},          /*     fluo_check_48_white_off , element 295     */
  {297,48,48,4608,3257414},          /*     fluo_check_48_white_on , element 296     */
  {298,28,28,1568,3262022},          /*     fluo_radio_28_red_off , element 297     */
  {299,28,28,1568,3263590},          /*     fluo_radio_28_red_on , element 298     */
  {300,28,28,1568,3265158},          /*     fluo_radio_28_green_off , element 299     */
  {301,28,28,1568,3266726},          /*     fluo_radio_28_green_on , element 300     */
  {302,28,28,1568,3268294},          /*     fluo_radio_28_blue_off , element 301     */
  {303,28,28,1568,3269862},          /*     fluo_radio_28_blue_on , element 302     */
  {304,28,28,1568,3271430},          /*     fluo_radio_28_cyan_off , element 303     */
  {305,28,28,1568,3272998},          /*     fluo_radio_28_cyan_on , element 304     */
  {306,28,28,1568,3274566},          /*     fluo_radio_28_magenta_off , element 305     */
  {307,28,28,1568,3276134},          /*     fluo_radio_28_magenta_on , element 306     */
  {308,28,28,1568,3277702},          /*     fluo_radio_28_yellow_off , element 307     */
  {309,28,28,1568,3279270},          /*     fluo_radio_28_yellow_on , element 308     */
  {310,28,28,1568,3280838},          /*     fluo_radio_28_white_off , element 309     */
  {311,28,28,1568,3282406},          /*     fluo_radio_28_white_on , element 310     */
  {312,36,36,2592,3283974},          /*     fluo_radio_36_red_off , element 311     */
  {313,36,36,2592,3286566},          /*     fluo_radio_36_red_on , element 312     */
  {314,36,36,2592,3289158},          /*     fluo_radio_36_green_off , element 313     */
  {315,36,36,2592,3291750},          /*     fluo_radio_36_green_on , element 314     */
  {316,36,36,2592,3294342},          /*     fluo_radio_36_blue_off , element 315     */
  {317,36,36,2592,3296934},          /*     fluo_radio_36_blue_on , element 316     */
  {318,36,36,2592,3299526},          /*     fluo_radio_36_cyan_off , element 317     */
  {319,36,36,2592,3302118},          /*     fluo_radio_36_cyan_on , element 318     */
  {320,36,36,2592,3304710},          /*     fluo_radio_36_magenta_off , element 319     */
  {321,36,36,2592,3307302},          /*     fluo_radio_36_magenta_on , element 320     */
  {322,36,36,2592,3309894},          /*     fluo_radio_36_yellow_off , element 321     */
  {323,36,36,2592,3312486},          /*     fluo_radio_36_yellow_on , element 322     */
  {324,36,36,2592,3315078},          /*     fluo_radio_36_white_off , element 323     */
  {325,36,36,2592,3317670},          /*     fluo_radio_36_white_on , element 324     */
  {326,48,48,4608,3320262},          /*     fluo_radio_48_red_off , element 325     */
  {327,48,48,4608,3324870},          /*     fluo_radio_48_red_on , element 326     */
  {328,48,48,4608,3329478},          /*     fluo_radio_48_green_off , element 327     */
  {329,48,48,4608,3334086},          /*     fluo_radio_48_green_on , element 328     */
  {330,48,48,4608,3338694},          /*     fluo_radio_48_blue_off , element 329     */
  {331,48,48,4608,3343302},          /*     fluo_radio_48_blue_on , element 330     */
  {332,48,48,4608,3347910},          /*     fluo_radio_48_cyan_off , element 331     */
  {333,48,48,4608,3352518},          /*     fluo_radio_48_cyan_on , element 332     */
  {334,48,48,4608,3357126},          /*     fluo_radio_48_magenta_off , element 333     */
  {335,48,48,4608,3361734},          /*     fluo_radio_48_magenta_on , element 334     */
  {336,48,48,4608,3366342},          /*     fluo_radio_48_yellow_off , element 335     */
  {337,48,48,4608,3370950},          /*     fluo_radio_48_yellow_on , element 336     */
  {338,48,48,4608,3375558},          /*     fluo_radio_48_white_off , element 337     */
  {339,48,48,4608,3380166},          /*     fluo_radio_48_white_on , element 338     */
  {340,70,34,4760,3384774},          /*     fluo_switch_70_34_red_off , element 339     */
  {341,70,34,4760,3389534},          /*     fluo_switch_70_34_red_on , element 340     */
  {342,70,34,4760,3394294},          /*     fluo_switch_70_34_green_off , element 341     */
  {343,70,34,4760,3399054},          /*     fluo_switch_70_34_green_on , element 342     */
  {344,70,34,4760,3403814},          /*     fluo_switch_70_34_blue_off , element 343     */
  {345,70,34,4760,3408574},          /*     fluo_switch_70_34_blue_on , element 344     */
  {346,70,34,4760,3413334},          /*     fluo_switch_70_34_cyan_off , element 345     */
  {347,70,34,4760,3418094},          /*     fluo_switch_70_34_cyan_on , element 346     */
  {348,70,34,4760,3422854},          /*     fluo_switch_70_34_magenta_off , element 347     */
  {349,70,34,4760,3427614},          /*     fluo_switch_70_34_magenta_on , element 348     */
  {350,70,34,4760,3432374},          /*     fluo_switch_70_34_yellow_off , element 349     */
  {351,70,34,4760,3437134},          /*     fluo_switch_70_34_yellow_on , element 350     */
  {352,70,34,4760,3441894},          /*     fluo_switch_70_34_white_off , element 351     */
  {353,70,34,4760,3446654},          /*     fluo_switch_70_34_white_on , element 352     */
  {354,100,48,9600,3451414},          /*     fluo_switch_100_48_red_off , element 353     */
  {355,100,48,9600,3461014},          /*     fluo_switch_100_48_red_on , element 354     */
  {356,100,48,9600,3470614},          /*     fluo_switch_100_48_green_off , element 355     */
  {357,100,48,9600,3480214},          /*     fluo_switch_100_48_green_on , element 356     */
  {358,100,48,9600,3489814},          /*     fluo_switch_100_48_blue_off , element 357     */
  {359,100,48,9600,3499414},          /*     fluo_switch_100_48_blue_on , element 358     */
  {360,100,48,9600,3509014},          /*     fluo_switch_100_48_cyan_off , element 359     */
  {361,100,48,9600,3518614},          /*     fluo_switch_100_48_cyan_on , element 360     */
  {362,100,48,9600,3528214},          /*     fluo_switch_100_48_magenta_off , element 361     */
  {363,100,48,9600,3537814},          /*     fluo_switch_100_48_magenta_on , element 362     */
  {364,100,48,9600,3547414},          /*     fluo_switch_100_48_yellow_off , element 363     */
  {365,100,48,9600,3557014},          /*     fluo_switch_100_48_yellow_on , element 364     */
  {366,100,48,9600,3566614},          /*     fluo_switch_100_48_white_off , element 365     */
  {367,100,48,9600,3576214},          /*     fluo_switch_100_48_white_on , element 366     */
  {368,24,24,1152,3585814},          /*     fluo_knob_24_red , element 367     */
  {369,24,24,1152,3586966},          /*     fluo_knob_24_green , element 368     */
  {370,24,24,1152,3588118},          /*     fluo_knob_24_blue , element 369     */
  {371,24,24,1152,3589270},          /*     fluo_knob_24_cyan , element 370     */
  {372,24,24,1152,3590422},          /*     fluo_knob_24_magenta , element 371     */
  {373,24,24,1152,3591574},          /*     fluo_knob_24_yellow , element 372     */
  {374,24,24,1152,3592726},          /*     fluo_knob_24_white , element 373     */
  {375,31,31,1922,3593878},          /*     fluo_knob_31_red , element 374     */
  {376,31,31,1922,3595800},          /*     fluo_knob_31_green , element 375     */
  {377,31,31,1922,3597722},          /*     fluo_knob_31_blue , element 376     */
  {378,31,31,1922,3599644},          /*     fluo_knob_31_cyan , element 377     */
  {379,31,31,1922,3601566},          /*     fluo_knob_31_magenta , element 378     */
  {380,31,31,1922,3603488},          /*     fluo_knob_31_yellow , element 379     */
  {381,31,31,1922,3605410},          /*     fluo_knob_31_white , element 380     */
  {382,40,40,3200,3607332},          /*     fluo_knob_40_red , element 381     */
  {383,40,40,3200,3610532},          /*     fluo_knob_40_green , element 382     */
  {384,40,40,3200,3613732},          /*     fluo_knob_40_blue , element 383     */
  {385,40,40,3200,3616932},          /*     fluo_knob_40_cyan , element 384     */
  {386,40,40,3200,3620132},          /*     fluo_knob_40_magenta , element 385     */
  {387,40,40,3200,3623332},          /*     fluo_knob_40_yellow , element 386     */
  {388,40,40,3200,3626532},          /*     fluo_knob_40_white , element 387     */
  {389,105,40,8400,3629732},          /*     alu_rect_105_40_alu_released , element 388     */
  {390,105,40,8400,3638132},          /*     alu_rect_105_40_alu_pressed , element 389     */
  {391,105,40,8400,3646532},          /*     alu_rect_105_40_blue_released , element 390     */
  {392,105,40,8400,3654932},          /*     alu_rect_105_40_blue_pressed , element 391     */
  {393,105,55,11550,3663332},          /*     alu_rect_105_55_alu_released , element 392     */
  {394,105,55,11550,3674882},          /*     alu_rect_105_55_alu_pressed , element 393     */
  {395,105,55,11550,3686432},          /*     alu_rect_105_55_blue_released , element 394     */
  {396,105,55,11550,3697982},          /*     alu_rect_105_55_blue_pressed , element 395     */
  {397,105,70,14700,3709532},          /*     alu_rect_105_70_alu_released , element 396     */
  {398,105,70,14700,3724232},          /*     alu_rect_105_70_alu_pressed , element 397     */
  {399,105,70,14700,3738932},          /*     alu_rect_105_70_blue_released , element 398     */
  {400,105,70,14700,3753632},          /*     alu_rect_105_70_blue_pressed , element 399     */
  {401,146,40,11680,3768332},          /*     alu_rect_146_40_alu_released , element 400     */
  {402,146,40,11680,3780012},          /*     alu_rect_146_40_alu_pressed , element 401     */
  {403,146,40,11680,3791692},          /*     alu_rect_146_40_blue_released , element 402     */
  {404,146,40,11680,3803372},          /*     alu_rect_146_40_blue_pressed , element 403     */
  {405,146,55,16060,3815052},          /*     alu_rect_146_55_alu_released , element 404     */
  {406,146,55,16060,3831112},          /*     alu_rect_146_55_alu_pressed , element 405     */
  {407,146,55,16060,3847172},          /*     alu_rect_146_55_blue_released , element 406     */
  {408,146,55,16060,3863232},          /*     alu_rect_146_55_blue_pressed , element 407     */
  {409,146,70,20440,3879292},          /*     alu_rect_146_70_alu_released , element 408     */
  {410,146,70,20440,3899732},          /*     alu_rect_146_70_alu_pressed , element 409     */
  {411,146,70,20440,3920172},          /*     alu_rect_146_70_blue_released , element 410     */
  {412,146,70,20440,3940612},          /*     alu_rect_146_70_blue_pressed , element 411     */
  {413,251,40,20080,3961052},          /*     alu_rect_251_40_alu_released , element 412     */
  {414,251,40,20080,3981132},          /*     alu_rect_251_40_alu_pressed , element 413     */
  {415,251,40,20080,4001212},          /*     alu_rect_251_40_blue_released , element 414     */
  {416,251,40,20080,4021292},          /*     alu_rect_251_40_blue_pressed , element 415     */
  {417,251,55,27610,4041372},          /*     alu_rect_251_55_alu_released , element 416     */
  {418,251,55,27610,4068982},          /*     alu_rect_251_55_alu_pressed , element 417     */
  {419,251,55,27610,4096592},          /*     alu_rect_251_55_blue_released , element 418     */
  {420,251,55,27610,4124202},          /*     alu_rect_251_55_blue_pressed , element 419     */
  {421,251,70,35140,4151812},          /*     alu_rect_251_70_alu_released , element 420     */
  {422,251,70,35140,4186952},          /*     alu_rect_251_70_alu_pressed , element 421     */
  {423,251,70,35140,4222092},          /*     alu_rect_251_70_blue_released , element 422     */
  {424,251,70,35140,4257232},          /*     alu_rect_251_70_blue_pressed , element 423     */
  {425,31,31,1922,4292372},          /*     alu_round_31_alu_released , element 424     */
  {426,31,31,1922,4294294},          /*     alu_round_31_alu_pressed , element 425     */
  {427,31,31,1922,4296216},          /*     alu_round_31_blue_released , element 426     */
  {428,31,31,1922,4298138},          /*     alu_round_31_blue_pressed , element 427     */
  {429,48,48,4608,4300060},          /*     alu_round_48_alu_released , element 428     */
  {430,48,48,4608,4304668},          /*     alu_round_48_alu_pressed , element 429     */
  {431,48,48,4608,4309276},          /*     alu_round_48_blue_released , element 430     */
  {432,48,48,4608,4313884},          /*     alu_round_48_blue_pressed , element 431     */
  {433,60,60,7200,4318492},          /*     alu_round_60_alu_released , element 432     */
  {434,60,60,7200,4325692},          /*     alu_round_60_alu_pressed , element 433     */
  {435,60,60,7200,4332892},          /*     alu_round_60_blue_released , element 434     */
  {436,60,60,7200,4340092},          /*     alu_round_60_blue_pressed , element 435     */
  {437,70,70,9800,4347292},          /*     alu_round_70_alu_released , element 436     */
  {438,70,70,9800,4357092},          /*     alu_round_70_alu_pressed , element 437     */
  {439,70,70,9800,4366892},          /*     alu_round_70_blue_released , element 438     */
  {440,70,70,9800,4376692},          /*     alu_round_70_blue_pressed , element 439     */
  {441,20,20,800,4386492},          /*     alu_led_20_blue_off , element 440     */
  {442,20,20,800,4387292},          /*     alu_led_20_blue_on , element 441     */
  {443,20,20,800,4388092},          /*     alu_led_20_green_off , element 442     */
  {444,20,20,800,4388892},          /*     alu_led_20_green_on , element 443     */
  {445,20,20,800,4389692},          /*     alu_led_20_red_off , element 444     */
  {446,20,20,800,4390492},          /*     alu_led_20_red_on , element 445     */
  {447,20,20,800,4391292},          /*     alu_led_20_amber_off , element 446     */
  {448,20,20,800,4392092},          /*     alu_led_20_amber_on , element 447     */
  {449,20,20,800,4392892},          /*     alu_led_20_white_off , element 448     */
  {450,20,20,800,4393692},          /*     alu_led_20_white_on , element 449     */
  {451,31,31,1922,4394492},          /*     alu_led_31_blue_off , element 450     */
  {452,31,31,1922,4396414},          /*     alu_led_31_blue_on , element 451     */
  {453,31,31,1922,4398336},          /*     alu_led_31_green_off , element 452     */
  {454,31,31,1922,4400258},          /*     alu_led_31_green_on , element 453     */
  {455,31,31,1922,4402180},          /*     alu_led_31_red_off , element 454     */
  {456,31,31,1922,4404102},          /*     alu_led_31_red_on , element 455     */
  {457,31,31,1922,4406024},          /*     alu_led_31_amber_off , element 456     */
  {458,31,31,1922,4407946},          /*     alu_led_31_amber_on , element 457     */
  {459,31,31,1922,4409868},          /*     alu_led_31_white_off , element 458     */
  {460,31,31,1922,4411790},          /*     alu_led_31_white_on , element 459     */
  {461,48,48,4608,4413712},          /*     alu_led_48_blue_off , element 460     */
  {462,48,48,4608,4418320},          /*     alu_led_48_blue_on , element 461     */
  {463,48,48,4608,4422928},          /*     alu_led_48_green_off , element 462     */
  {464,48,48,4608,4427536},          /*     alu_led_48_green_on , element 463     */
  {465,48,48,4608,4432144},          /*     alu_led_48_red_off , element 464     */
  {466,48,48,4608,4436752},          /*     alu_led_48_red_on , element 465     */
  {467,48,48,4608,4441360},          /*     alu_led_48_amber_off , element 466     */
  {468,48,48,4608,4445968},          /*     alu_led_48_amber_on , element 467     */
  {469,48,48,4608,4450576},          /*     alu_led_48_white_off , element 468     */
  {470,48,48,4608,4455184},          /*     alu_led_48_white_on , element 469     */
  {471,28,28,1568,4459792},          /*     alu_check_28_alu_off , element 470     */
  {472,28,28,1568,4461360},          /*     alu_check_28_alu_on , element 471     */
  {473,28,28,1568,4462928},          /*     alu_check_28_blue_off , element 472     */
  {474,28,28,1568,4464496},          /*     alu_check_28_blue_on , element 473     */
  {475,36,36,2592,4466064},          /*     alu_check_36_alu_off , element 474     */
  {476,36,36,2592,4468656},          /*     alu_check_36_alu_on , element 475     */
  {477,36,36,2592,4471248},          /*     alu_check_36_blue_off , element 476     */
  {478,36,36,2592,4473840},          /*     alu_check_36_blue_on , element 477     */
  {479,48,48,4608,4476432},          /*     alu_check_48_alu_off , element 478     */
  {480,48,48,4608,4481040},          /*     alu_check_48_alu_on , element 479     */
  {481,48,48,4608,4485648},          /*     alu_check_48_blue_off , element 480     */
  {482,48,48,4608,4490256},          /*     alu_check_48_blue_on , element 481     */
  {483,28,28,1568,4494864},          /*     alu_radio_28_alu_off , element 482     */
  {484,28,28,1568,4496432},          /*     alu_radio_28_alu_on , element 483     */
  {485,28,28,1568,4498000},          /*     alu_radio_28_blue_off , element 484     */
  {486,28,28,1568,4499568},          /*     alu_radio_28_blue_on , element 485     */
  {487,36,36,2592,4501136},          /*     alu_radio_36_alu_off , element 486     */
  {488,36,36,2592,4503728},          /*     alu_radio_36_alu_on , element 487     */
  {489,36,36,2592,4506320},          /*     alu_radio_36_blue_off , element 488     */
  {490,36,36,2592,4508912},          /*     alu_radio_36_blue_on , element 489     */
  {491,48,48,4608,4511504},          /*     alu_radio_48_alu_off , element 490     */
  {492,48,48,4608,4516112},          /*     alu_radio_48_alu_on , element 491     */
  {493,48,48,4608,4520720},          /*     alu_radio_48_blue_off , element 492     */
  {494,48,48,4608,4525328},          /*     alu_radio_48_blue_on , element 493     */
  {495,70,34,4760,4529936},          /*     alu_switch_70_34_alu_off , element 494     */
  {496,70,34,4760,4534696},          /*     alu_switch_70_34_alu_on , element 495     */
  {497,70,34,4760,4539456},          /*     alu_switch_70_34_blue_off , element 496     */
  {498,70,34,4760,4544216},          /*     alu_switch_70_34_blue_on , element 497     */
  {499,100,48,9600,4548976},          /*     alu_switch_100_48_alu_off , element 498     */
  {500,100,48,9600,4558576},          /*     alu_switch_100_48_alu_on , element 499     */
  {501,100,48,9600,4568176},          /*     alu_switch_100_48_blue_off , element 500     */
  {502,100,48,9600,4577776},          /*     alu_switch_100_48_blue_on , element 501     */
  {503,24,24,1152,4587376},          /*     alu_knob_24_alu , element 502     */
  {504,24,24,1152,4588528},          /*     alu_knob_24_blue , element 503     */
  {505,31,31,1922,4589680},          /*     alu_knob_31_alu , element 504     */
  {506,31,31,1922,4591602},          /*     alu_knob_31_blue , element 505     */
  {507,40,40,3200,4593524},          /*     alu_knob_40_alu , element 506     */
  {508,40,40,3200,4596724},          /*     alu_knob_40_blue , element 507     */
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
  fluo_rect_105_40_red_released,          /*     31     */
  fluo_rect_105_40_red_pressed,          /*     32     */
  fluo_rect_105_40_green_released,          /*     33     */
  fluo_rect_105_40_green_pressed,          /*     34     */
  fluo_rect_105_40_blue_released,          /*     35     */
  fluo_rect_105_40_blue_pressed,          /*     36     */
  fluo_rect_105_40_cyan_released,          /*     37     */
  fluo_rect_105_40_cyan_pressed,          /*     38     */
  fluo_rect_105_40_magenta_released,          /*     39     */
  fluo_rect_105_40_magenta_pressed,          /*     40     */
  fluo_rect_105_40_yellow_released,          /*     41     */
  fluo_rect_105_40_yellow_pressed,          /*     42     */
  fluo_rect_105_40_white_released,          /*     43     */
  fluo_rect_105_40_white_pressed,          /*     44     */
  fluo_rect_105_55_red_released,          /*     45     */
  fluo_rect_105_55_red_pressed,          /*     46     */
  fluo_rect_105_55_green_released,          /*     47     */
  fluo_rect_105_55_green_pressed,          /*     48     */
  fluo_rect_105_55_blue_released,          /*     49     */
  fluo_rect_105_55_blue_pressed,          /*     50     */
  fluo_rect_105_55_cyan_released,          /*     51     */
  fluo_rect_105_55_cyan_pressed,          /*     52     */
  fluo_rect_105_55_magenta_released,          /*     53     */
  fluo_rect_105_55_magenta_pressed,          /*     54     */
  fluo_rect_105_55_yellow_released,          /*     55     */
  fluo_rect_105_55_yellow_pressed,          /*     56     */
  fluo_rect_105_55_white_released,          /*     57     */
  fluo_rect_105_55_white_pressed,          /*     58     */
  fluo_rect_105_70_red_released,          /*     59     */
  fluo_rect_105_70_red_pressed,          /*     60     */
  fluo_rect_105_70_green_released,          /*     61     */
  fluo_rect_105_70_green_pressed,          /*     62     */
  fluo_rect_105_70_blue_released,          /*     63     */
  fluo_rect_105_70_blue_pressed,          /*     64     */
  fluo_rect_105_70_cyan_released,          /*     65     */
  fluo_rect_105_70_cyan_pressed,          /*     66     */
  fluo_rect_105_70_magenta_released,          /*     67     */
  fluo_rect_105_70_magenta_pressed,          /*     68     */
  fluo_rect_105_70_yellow_released,          /*     69     */
  fluo_rect_105_70_yellow_pressed,          /*     70     */
  fluo_rect_105_70_white_released,          /*     71     */
  fluo_rect_105_70_white_pressed,          /*     72     */
  fluo_rect_146_40_red_released,          /*     73     */
  fluo_rect_146_40_red_pressed,          /*     74     */
  fluo_rect_146_40_green_released,          /*     75     */
  fluo_rect_146_40_green_pressed,          /*     76     */
  fluo_rect_146_40_blue_released,          /*     77     */
  fluo_rect_146_40_blue_pressed,          /*     78     */
  fluo_rect_146_40_cyan_released,          /*     79     */
  fluo_rect_146_40_cyan_pressed,          /*     80     */
  fluo_rect_146_40_magenta_released,          /*     81     */
  fluo_rect_146_40_magenta_pressed,          /*     82     */
  fluo_rect_146_40_yellow_released,          /*     83     */
  fluo_rect_146_40_yellow_pressed,          /*     84     */
  fluo_rect_146_40_white_released,          /*     85     */
  fluo_rect_146_40_white_pressed,          /*     86     */
  fluo_rect_146_55_red_released,          /*     87     */
  fluo_rect_146_55_red_pressed,          /*     88     */
  fluo_rect_146_55_green_released,          /*     89     */
  fluo_rect_146_55_green_pressed,          /*     90     */
  fluo_rect_146_55_blue_released,          /*     91     */
  fluo_rect_146_55_blue_pressed,          /*     92     */
  fluo_rect_146_55_cyan_released,          /*     93     */
  fluo_rect_146_55_cyan_pressed,          /*     94     */
  fluo_rect_146_55_magenta_released,          /*     95     */
  fluo_rect_146_55_magenta_pressed,          /*     96     */
  fluo_rect_146_55_yellow_released,          /*     97     */
  fluo_rect_146_55_yellow_pressed,          /*     98     */
  fluo_rect_146_55_white_released,          /*     99     */
  fluo_rect_146_55_white_pressed,          /*     100     */
  fluo_rect_146_70_red_released,          /*     101     */
  fluo_rect_146_70_red_pressed,          /*     102     */
  fluo_rect_146_70_green_released,          /*     103     */
  fluo_rect_146_70_green_pressed,          /*     104     */
  fluo_rect_146_70_blue_released,          /*     105     */
  fluo_rect_146_70_blue_pressed,          /*     106     */
  fluo_rect_146_70_cyan_released,          /*     107     */
  fluo_rect_146_70_cyan_pressed,          /*     108     */
  fluo_rect_146_70_magenta_released,          /*     109     */
  fluo_rect_146_70_magenta_pressed,          /*     110     */
  fluo_rect_146_70_yellow_released,          /*     111     */
  fluo_rect_146_70_yellow_pressed,          /*     112     */
  fluo_rect_146_70_white_released,          /*     113     */
  fluo_rect_146_70_white_pressed,          /*     114     */
  fluo_rect_251_40_red_released,          /*     115     */
  fluo_rect_251_40_red_pressed,          /*     116     */
  fluo_rect_251_40_green_released,          /*     117     */
  fluo_rect_251_40_green_pressed,          /*     118     */
  fluo_rect_251_40_blue_released,          /*     119     */
  fluo_rect_251_40_blue_pressed,          /*     120     */
  fluo_rect_251_40_cyan_released,          /*     121     */
  fluo_rect_251_40_cyan_pressed,          /*     122     */
  fluo_rect_251_40_magenta_released,          /*     123     */
  fluo_rect_251_40_magenta_pressed,          /*     124     */
  fluo_rect_251_40_yellow_released,          /*     125     */
  fluo_rect_251_40_yellow_pressed,          /*     126     */
  fluo_rect_251_40_white_released,          /*     127     */
  fluo_rect_251_40_white_pressed,          /*     128     */
  fluo_rect_251_55_red_released,          /*     129     */
  fluo_rect_251_55_red_pressed,          /*     130     */
  fluo_rect_251_55_green_released,          /*     131     */
  fluo_rect_251_55_green_pressed,          /*     132     */
  fluo_rect_251_55_blue_released,          /*     133     */
  fluo_rect_251_55_blue_pressed,          /*     134     */
  fluo_rect_251_55_cyan_released,          /*     135     */
  fluo_rect_251_55_cyan_pressed,          /*     136     */
  fluo_rect_251_55_magenta_released,          /*     137     */
  fluo_rect_251_55_magenta_pressed,          /*     138     */
  fluo_rect_251_55_yellow_released,          /*     139     */
  fluo_rect_251_55_yellow_pressed,          /*     140     */
  fluo_rect_251_55_white_released,          /*     141     */
  fluo_rect_251_55_white_pressed,          /*     142     */
  fluo_rect_251_70_red_released,          /*     143     */
  fluo_rect_251_70_red_pressed,          /*     144     */
  fluo_rect_251_70_green_released,          /*     145     */
  fluo_rect_251_70_green_pressed,          /*     146     */
  fluo_rect_251_70_blue_released,          /*     147     */
  fluo_rect_251_70_blue_pressed,          /*     148     */
  fluo_rect_251_70_cyan_released,          /*     149     */
  fluo_rect_251_70_cyan_pressed,          /*     150     */
  fluo_rect_251_70_magenta_released,          /*     151     */
  fluo_rect_251_70_magenta_pressed,          /*     152     */
  fluo_rect_251_70_yellow_released,          /*     153     */
  fluo_rect_251_70_yellow_pressed,          /*     154     */
  fluo_rect_251_70_white_released,          /*     155     */
  fluo_rect_251_70_white_pressed,          /*     156     */
  fluo_round_31_red_released,          /*     157     */
  fluo_round_31_red_pressed,          /*     158     */
  fluo_round_31_green_released,          /*     159     */
  fluo_round_31_green_pressed,          /*     160     */
  fluo_round_31_blue_released,          /*     161     */
  fluo_round_31_blue_pressed,          /*     162     */
  fluo_round_31_cyan_released,          /*     163     */
  fluo_round_31_cyan_pressed,          /*     164     */
  fluo_round_31_magenta_released,          /*     165     */
  fluo_round_31_magenta_pressed,          /*     166     */
  fluo_round_31_yellow_released,          /*     167     */
  fluo_round_31_yellow_pressed,          /*     168     */
  fluo_round_31_white_released,          /*     169     */
  fluo_round_31_white_pressed,          /*     170     */
  fluo_round_48_red_released,          /*     171     */
  fluo_round_48_red_pressed,          /*     172     */
  fluo_round_48_green_released,          /*     173     */
  fluo_round_48_green_pressed,          /*     174     */
  fluo_round_48_blue_released,          /*     175     */
  fluo_round_48_blue_pressed,          /*     176     */
  fluo_round_48_cyan_released,          /*     177     */
  fluo_round_48_cyan_pressed,          /*     178     */
  fluo_round_48_magenta_released,          /*     179     */
  fluo_round_48_magenta_pressed,          /*     180     */
  fluo_round_48_yellow_released,          /*     181     */
  fluo_round_48_yellow_pressed,          /*     182     */
  fluo_round_48_white_released,          /*     183     */
  fluo_round_48_white_pressed,          /*     184     */
  fluo_round_60_red_released,          /*     185     */
  fluo_round_60_red_pressed,          /*     186     */
  fluo_round_60_green_released,          /*     187     */
  fluo_round_60_green_pressed,          /*     188     */
  fluo_round_60_blue_released,          /*     189     */
  fluo_round_60_blue_pressed,          /*     190     */
  fluo_round_60_cyan_released,          /*     191     */
  fluo_round_60_cyan_pressed,          /*     192     */
  fluo_round_60_magenta_released,          /*     193     */
  fluo_round_60_magenta_pressed,          /*     194     */
  fluo_round_60_yellow_released,          /*     195     */
  fluo_round_60_yellow_pressed,          /*     196     */
  fluo_round_60_white_released,          /*     197     */
  fluo_round_60_white_pressed,          /*     198     */
  fluo_round_70_red_released,          /*     199     */
  fluo_round_70_red_pressed,          /*     200     */
  fluo_round_70_green_released,          /*     201     */
  fluo_round_70_green_pressed,          /*     202     */
  fluo_round_70_blue_released,          /*     203     */
  fluo_round_70_blue_pressed,          /*     204     */
  fluo_round_70_cyan_released,          /*     205     */
  fluo_round_70_cyan_pressed,          /*     206     */
  fluo_round_70_magenta_released,          /*     207     */
  fluo_round_70_magenta_pressed,          /*     208     */
  fluo_round_70_yellow_released,          /*     209     */
  fluo_round_70_yellow_pressed,          /*     210     */
  fluo_round_70_white_released,          /*     211     */
  fluo_round_70_white_pressed,          /*     212     */
  fluo_led_20_red_off,          /*     213     */
  fluo_led_20_red_on,          /*     214     */
  fluo_led_20_green_off,          /*     215     */
  fluo_led_20_green_on,          /*     216     */
  fluo_led_20_blue_off,          /*     217     */
  fluo_led_20_blue_on,          /*     218     */
  fluo_led_20_cyan_off,          /*     219     */
  fluo_led_20_cyan_on,          /*     220     */
  fluo_led_20_magenta_off,          /*     221     */
  fluo_led_20_magenta_on,          /*     222     */
  fluo_led_20_yellow_off,          /*     223     */
  fluo_led_20_yellow_on,          /*     224     */
  fluo_led_20_white_off,          /*     225     */
  fluo_led_20_white_on,          /*     226     */
  fluo_led_31_red_off,          /*     227     */
  fluo_led_31_red_on,          /*     228     */
  fluo_led_31_green_off,          /*     229     */
  fluo_led_31_green_on,          /*     230     */
  fluo_led_31_blue_off,          /*     231     */
  fluo_led_31_blue_on,          /*     232     */
  fluo_led_31_cyan_off,          /*     233     */
  fluo_led_31_cyan_on,          /*     234     */
  fluo_led_31_magenta_off,          /*     235     */
  fluo_led_31_magenta_on,          /*     236     */
  fluo_led_31_yellow_off,          /*     237     */
  fluo_led_31_yellow_on,          /*     238     */
  fluo_led_31_white_off,          /*     239     */
  fluo_led_31_white_on,          /*     240     */
  fluo_led_48_red_off,          /*     241     */
  fluo_led_48_red_on,          /*     242     */
  fluo_led_48_green_off,          /*     243     */
  fluo_led_48_green_on,          /*     244     */
  fluo_led_48_blue_off,          /*     245     */
  fluo_led_48_blue_on,          /*     246     */
  fluo_led_48_cyan_off,          /*     247     */
  fluo_led_48_cyan_on,          /*     248     */
  fluo_led_48_magenta_off,          /*     249     */
  fluo_led_48_magenta_on,          /*     250     */
  fluo_led_48_yellow_off,          /*     251     */
  fluo_led_48_yellow_on,          /*     252     */
  fluo_led_48_white_off,          /*     253     */
  fluo_led_48_white_on,          /*     254     */
  fluo_check_28_red_off,          /*     255     */
  fluo_check_28_red_on,          /*     256     */
  fluo_check_28_green_off,          /*     257     */
  fluo_check_28_green_on,          /*     258     */
  fluo_check_28_blue_off,          /*     259     */
  fluo_check_28_blue_on,          /*     260     */
  fluo_check_28_cyan_off,          /*     261     */
  fluo_check_28_cyan_on,          /*     262     */
  fluo_check_28_magenta_off,          /*     263     */
  fluo_check_28_magenta_on,          /*     264     */
  fluo_check_28_yellow_off,          /*     265     */
  fluo_check_28_yellow_on,          /*     266     */
  fluo_check_28_white_off,          /*     267     */
  fluo_check_28_white_on,          /*     268     */
  fluo_check_36_red_off,          /*     269     */
  fluo_check_36_red_on,          /*     270     */
  fluo_check_36_green_off,          /*     271     */
  fluo_check_36_green_on,          /*     272     */
  fluo_check_36_blue_off,          /*     273     */
  fluo_check_36_blue_on,          /*     274     */
  fluo_check_36_cyan_off,          /*     275     */
  fluo_check_36_cyan_on,          /*     276     */
  fluo_check_36_magenta_off,          /*     277     */
  fluo_check_36_magenta_on,          /*     278     */
  fluo_check_36_yellow_off,          /*     279     */
  fluo_check_36_yellow_on,          /*     280     */
  fluo_check_36_white_off,          /*     281     */
  fluo_check_36_white_on,          /*     282     */
  fluo_check_48_red_off,          /*     283     */
  fluo_check_48_red_on,          /*     284     */
  fluo_check_48_green_off,          /*     285     */
  fluo_check_48_green_on,          /*     286     */
  fluo_check_48_blue_off,          /*     287     */
  fluo_check_48_blue_on,          /*     288     */
  fluo_check_48_cyan_off,          /*     289     */
  fluo_check_48_cyan_on,          /*     290     */
  fluo_check_48_magenta_off,          /*     291     */
  fluo_check_48_magenta_on,          /*     292     */
  fluo_check_48_yellow_off,          /*     293     */
  fluo_check_48_yellow_on,          /*     294     */
  fluo_check_48_white_off,          /*     295     */
  fluo_check_48_white_on,          /*     296     */
  fluo_radio_28_red_off,          /*     297     */
  fluo_radio_28_red_on,          /*     298     */
  fluo_radio_28_green_off,          /*     299     */
  fluo_radio_28_green_on,          /*     300     */
  fluo_radio_28_blue_off,          /*     301     */
  fluo_radio_28_blue_on,          /*     302     */
  fluo_radio_28_cyan_off,          /*     303     */
  fluo_radio_28_cyan_on,          /*     304     */
  fluo_radio_28_magenta_off,          /*     305     */
  fluo_radio_28_magenta_on,          /*     306     */
  fluo_radio_28_yellow_off,          /*     307     */
  fluo_radio_28_yellow_on,          /*     308     */
  fluo_radio_28_white_off,          /*     309     */
  fluo_radio_28_white_on,          /*     310     */
  fluo_radio_36_red_off,          /*     311     */
  fluo_radio_36_red_on,          /*     312     */
  fluo_radio_36_green_off,          /*     313     */
  fluo_radio_36_green_on,          /*     314     */
  fluo_radio_36_blue_off,          /*     315     */
  fluo_radio_36_blue_on,          /*     316     */
  fluo_radio_36_cyan_off,          /*     317     */
  fluo_radio_36_cyan_on,          /*     318     */
  fluo_radio_36_magenta_off,          /*     319     */
  fluo_radio_36_magenta_on,          /*     320     */
  fluo_radio_36_yellow_off,          /*     321     */
  fluo_radio_36_yellow_on,          /*     322     */
  fluo_radio_36_white_off,          /*     323     */
  fluo_radio_36_white_on,          /*     324     */
  fluo_radio_48_red_off,          /*     325     */
  fluo_radio_48_red_on,          /*     326     */
  fluo_radio_48_green_off,          /*     327     */
  fluo_radio_48_green_on,          /*     328     */
  fluo_radio_48_blue_off,          /*     329     */
  fluo_radio_48_blue_on,          /*     330     */
  fluo_radio_48_cyan_off,          /*     331     */
  fluo_radio_48_cyan_on,          /*     332     */
  fluo_radio_48_magenta_off,          /*     333     */
  fluo_radio_48_magenta_on,          /*     334     */
  fluo_radio_48_yellow_off,          /*     335     */
  fluo_radio_48_yellow_on,          /*     336     */
  fluo_radio_48_white_off,          /*     337     */
  fluo_radio_48_white_on,          /*     338     */
  fluo_switch_70_34_red_off,          /*     339     */
  fluo_switch_70_34_red_on,          /*     340     */
  fluo_switch_70_34_green_off,          /*     341     */
  fluo_switch_70_34_green_on,          /*     342     */
  fluo_switch_70_34_blue_off,          /*     343     */
  fluo_switch_70_34_blue_on,          /*     344     */
  fluo_switch_70_34_cyan_off,          /*     345     */
  fluo_switch_70_34_cyan_on,          /*     346     */
  fluo_switch_70_34_magenta_off,          /*     347     */
  fluo_switch_70_34_magenta_on,          /*     348     */
  fluo_switch_70_34_yellow_off,          /*     349     */
  fluo_switch_70_34_yellow_on,          /*     350     */
  fluo_switch_70_34_white_off,          /*     351     */
  fluo_switch_70_34_white_on,          /*     352     */
  fluo_switch_100_48_red_off,          /*     353     */
  fluo_switch_100_48_red_on,          /*     354     */
  fluo_switch_100_48_green_off,          /*     355     */
  fluo_switch_100_48_green_on,          /*     356     */
  fluo_switch_100_48_blue_off,          /*     357     */
  fluo_switch_100_48_blue_on,          /*     358     */
  fluo_switch_100_48_cyan_off,          /*     359     */
  fluo_switch_100_48_cyan_on,          /*     360     */
  fluo_switch_100_48_magenta_off,          /*     361     */
  fluo_switch_100_48_magenta_on,          /*     362     */
  fluo_switch_100_48_yellow_off,          /*     363     */
  fluo_switch_100_48_yellow_on,          /*     364     */
  fluo_switch_100_48_white_off,          /*     365     */
  fluo_switch_100_48_white_on,          /*     366     */
  fluo_knob_24_red,          /*     367     */
  fluo_knob_24_green,          /*     368     */
  fluo_knob_24_blue,          /*     369     */
  fluo_knob_24_cyan,          /*     370     */
  fluo_knob_24_magenta,          /*     371     */
  fluo_knob_24_yellow,          /*     372     */
  fluo_knob_24_white,          /*     373     */
  fluo_knob_31_red,          /*     374     */
  fluo_knob_31_green,          /*     375     */
  fluo_knob_31_blue,          /*     376     */
  fluo_knob_31_cyan,          /*     377     */
  fluo_knob_31_magenta,          /*     378     */
  fluo_knob_31_yellow,          /*     379     */
  fluo_knob_31_white,          /*     380     */
  fluo_knob_40_red,          /*     381     */
  fluo_knob_40_green,          /*     382     */
  fluo_knob_40_blue,          /*     383     */
  fluo_knob_40_cyan,          /*     384     */
  fluo_knob_40_magenta,          /*     385     */
  fluo_knob_40_yellow,          /*     386     */
  fluo_knob_40_white,          /*     387     */
  alu_rect_105_40_alu_released,          /*     388     */
  alu_rect_105_40_alu_pressed,          /*     389     */
  alu_rect_105_40_blue_released,          /*     390     */
  alu_rect_105_40_blue_pressed,          /*     391     */
  alu_rect_105_55_alu_released,          /*     392     */
  alu_rect_105_55_alu_pressed,          /*     393     */
  alu_rect_105_55_blue_released,          /*     394     */
  alu_rect_105_55_blue_pressed,          /*     395     */
  alu_rect_105_70_alu_released,          /*     396     */
  alu_rect_105_70_alu_pressed,          /*     397     */
  alu_rect_105_70_blue_released,          /*     398     */
  alu_rect_105_70_blue_pressed,          /*     399     */
  alu_rect_146_40_alu_released,          /*     400     */
  alu_rect_146_40_alu_pressed,          /*     401     */
  alu_rect_146_40_blue_released,          /*     402     */
  alu_rect_146_40_blue_pressed,          /*     403     */
  alu_rect_146_55_alu_released,          /*     404     */
  alu_rect_146_55_alu_pressed,          /*     405     */
  alu_rect_146_55_blue_released,          /*     406     */
  alu_rect_146_55_blue_pressed,          /*     407     */
  alu_rect_146_70_alu_released,          /*     408     */
  alu_rect_146_70_alu_pressed,          /*     409     */
  alu_rect_146_70_blue_released,          /*     410     */
  alu_rect_146_70_blue_pressed,          /*     411     */
  alu_rect_251_40_alu_released,          /*     412     */
  alu_rect_251_40_alu_pressed,          /*     413     */
  alu_rect_251_40_blue_released,          /*     414     */
  alu_rect_251_40_blue_pressed,          /*     415     */
  alu_rect_251_55_alu_released,          /*     416     */
  alu_rect_251_55_alu_pressed,          /*     417     */
  alu_rect_251_55_blue_released,          /*     418     */
  alu_rect_251_55_blue_pressed,          /*     419     */
  alu_rect_251_70_alu_released,          /*     420     */
  alu_rect_251_70_alu_pressed,          /*     421     */
  alu_rect_251_70_blue_released,          /*     422     */
  alu_rect_251_70_blue_pressed,          /*     423     */
  alu_round_31_alu_released,          /*     424     */
  alu_round_31_alu_pressed,          /*     425     */
  alu_round_31_blue_released,          /*     426     */
  alu_round_31_blue_pressed,          /*     427     */
  alu_round_48_alu_released,          /*     428     */
  alu_round_48_alu_pressed,          /*     429     */
  alu_round_48_blue_released,          /*     430     */
  alu_round_48_blue_pressed,          /*     431     */
  alu_round_60_alu_released,          /*     432     */
  alu_round_60_alu_pressed,          /*     433     */
  alu_round_60_blue_released,          /*     434     */
  alu_round_60_blue_pressed,          /*     435     */
  alu_round_70_alu_released,          /*     436     */
  alu_round_70_alu_pressed,          /*     437     */
  alu_round_70_blue_released,          /*     438     */
  alu_round_70_blue_pressed,          /*     439     */
  alu_led_20_blue_off,          /*     440     */
  alu_led_20_blue_on,          /*     441     */
  alu_led_20_green_off,          /*     442     */
  alu_led_20_green_on,          /*     443     */
  alu_led_20_red_off,          /*     444     */
  alu_led_20_red_on,          /*     445     */
  alu_led_20_amber_off,          /*     446     */
  alu_led_20_amber_on,          /*     447     */
  alu_led_20_white_off,          /*     448     */
  alu_led_20_white_on,          /*     449     */
  alu_led_31_blue_off,          /*     450     */
  alu_led_31_blue_on,          /*     451     */
  alu_led_31_green_off,          /*     452     */
  alu_led_31_green_on,          /*     453     */
  alu_led_31_red_off,          /*     454     */
  alu_led_31_red_on,          /*     455     */
  alu_led_31_amber_off,          /*     456     */
  alu_led_31_amber_on,          /*     457     */
  alu_led_31_white_off,          /*     458     */
  alu_led_31_white_on,          /*     459     */
  alu_led_48_blue_off,          /*     460     */
  alu_led_48_blue_on,          /*     461     */
  alu_led_48_green_off,          /*     462     */
  alu_led_48_green_on,          /*     463     */
  alu_led_48_red_off,          /*     464     */
  alu_led_48_red_on,          /*     465     */
  alu_led_48_amber_off,          /*     466     */
  alu_led_48_amber_on,          /*     467     */
  alu_led_48_white_off,          /*     468     */
  alu_led_48_white_on,          /*     469     */
  alu_check_28_alu_off,          /*     470     */
  alu_check_28_alu_on,          /*     471     */
  alu_check_28_blue_off,          /*     472     */
  alu_check_28_blue_on,          /*     473     */
  alu_check_36_alu_off,          /*     474     */
  alu_check_36_alu_on,          /*     475     */
  alu_check_36_blue_off,          /*     476     */
  alu_check_36_blue_on,          /*     477     */
  alu_check_48_alu_off,          /*     478     */
  alu_check_48_alu_on,          /*     479     */
  alu_check_48_blue_off,          /*     480     */
  alu_check_48_blue_on,          /*     481     */
  alu_radio_28_alu_off,          /*     482     */
  alu_radio_28_alu_on,          /*     483     */
  alu_radio_28_blue_off,          /*     484     */
  alu_radio_28_blue_on,          /*     485     */
  alu_radio_36_alu_off,          /*     486     */
  alu_radio_36_alu_on,          /*     487     */
  alu_radio_36_blue_off,          /*     488     */
  alu_radio_36_blue_on,          /*     489     */
  alu_radio_48_alu_off,          /*     490     */
  alu_radio_48_alu_on,          /*     491     */
  alu_radio_48_blue_off,          /*     492     */
  alu_radio_48_blue_on,          /*     493     */
  alu_switch_70_34_alu_off,          /*     494     */
  alu_switch_70_34_alu_on,          /*     495     */
  alu_switch_70_34_blue_off,          /*     496     */
  alu_switch_70_34_blue_on,          /*     497     */
  alu_switch_100_48_alu_off,          /*     498     */
  alu_switch_100_48_alu_on,          /*     499     */
  alu_switch_100_48_blue_off,          /*     500     */
  alu_switch_100_48_blue_on,          /*     501     */
  alu_knob_24_alu,          /*     502     */
  alu_knob_24_blue,          /*     503     */
  alu_knob_31_alu,          /*     504     */
  alu_knob_31_blue,          /*     505     */
  alu_knob_40_alu,          /*     506     */
  alu_knob_40_blue,          /*     507     */
}PICTURE_NAME;

#define GLASS_FIRST   0
#define GLASS_COUNT   31
#define FLUO_FIRST   31
#define FLUO_COUNT   357
#define ALU_FIRST   388
#define ALU_COUNT   120

#define TEMI_IMAGE_COUNT   508
#define TEMI_BINARY_SIZE   4599924UL
