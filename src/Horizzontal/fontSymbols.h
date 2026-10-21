/*********************************************************************/
//
//  Simboli GEVA come font: symbols_32pt, symbols_48pt
//
//  Generato da TFT_temi/tools/symbol_font.py -- NON modificare a mano.
//
//  Il glifo e' quadrato, largo e alto quanto il font: a parita' di x,y
//  cade dove cadeva l'immagine <tema>_sym_<nome>_<32|48>_<colore>.
//  Fondo trasparente, colore a scelta:
//
//    tft.setFont(&symbols_48pt);
//    tft.setTextColor(CYAN);          // un solo argomento = trasparente
//    tft.setCursor(x, y);
//    tft.print(SYM_POWER);
//
//  Le macro sono stringhe: si possono accostare, SYM_PLAY SYM_PAUSE.
//  L'ordine segue fluo_symbols.py: simboli nuovi solo in coda.
//
/*********************************************************************/
#ifndef font_symbols_H
#define font_symbols_H

#define SYM_ARROW_UP         "\x21"
#define SYM_ARROW_DOWN       "\x22"
#define SYM_ARROW_LEFT       "\x23"
#define SYM_ARROW_RIGHT      "\x24"
#define SYM_CHEVRON_UP       "\x25"
#define SYM_CHEVRON_DOWN     "\x26"
#define SYM_CHEVRON_LEFT     "\x27"
#define SYM_CHEVRON_RIGHT    "\x28"
#define SYM_PLAY             "\x29"
#define SYM_PAUSE            "\x2A"
#define SYM_STOP             "\x2B"
#define SYM_RECORD           "\x2C"
#define SYM_NEXT_TRACK       "\x2D"
#define SYM_PREV_TRACK       "\x2E"
#define SYM_FORWARD          "\x2F"
#define SYM_REWIND           "\x30"
#define SYM_VOLUME           "\x31"
#define SYM_MUTE             "\x32"
#define SYM_PLUS             "\x33"
#define SYM_MINUS            "\x34"
#define SYM_CHECK            "\x35"
#define SYM_CROSS            "\x36"
#define SYM_POWER            "\x37"
#define SYM_HOME             "\x38"
#define SYM_MENU             "\x39"
#define SYM_MORE             "\x3A"
#define SYM_GEAR             "\x3B"
#define SYM_REFRESH          "\x3C"
#define SYM_SEARCH           "\x3D"
#define SYM_EDIT             "\x3E"
#define SYM_TRASH            "\x3F"
#define SYM_SAVE             "\x40"
#define SYM_DOWNLOAD         "\x41"
#define SYM_UPLOAD           "\x42"
#define SYM_EXIT             "\x43"
#define SYM_LOCK             "\x44"
#define SYM_UNLOCK           "\x45"
#define SYM_KEY              "\x46"
#define SYM_USER             "\x47"
#define SYM_INFO             "\x48"
#define SYM_WARNING          "\x49"
#define SYM_QUESTION         "\x4A"
#define SYM_STAR             "\x4B"
#define SYM_HEART            "\x4C"
#define SYM_BELL             "\x4D"
#define SYM_DND              "\x4E"
#define SYM_BULB             "\x4F"
#define SYM_FAN              "\x50"
#define SYM_SNOWFLAKE        "\x51"
#define SYM_FLAME            "\x52"
#define SYM_THERMOMETER      "\x53"
#define SYM_SUN              "\x54"
#define SYM_MOON             "\x55"
#define SYM_DROP             "\x56"
#define SYM_LEAF             "\x57"
#define SYM_DOOR             "\x58"
#define SYM_BED              "\x59"
#define SYM_BLINDS           "\x5A"
#define SYM_SHOWER           "\x5B"
#define SYM_CLEAN            "\x5C"
#define SYM_TV               "\x5D"
#define SYM_PHONE            "\x5E"
#define SYM_CLOCK            "\x5F"
#define SYM_CALENDAR         "\x60"
#define SYM_WIFI             "\x61"
#define SYM_BLUETOOTH        "\x62"
#define SYM_BATTERY          "\x63"
#define SYM_PLUG             "\x64"
#define SYM_ECO              "\x65"

#define SYM_COUNT  69

#endif
