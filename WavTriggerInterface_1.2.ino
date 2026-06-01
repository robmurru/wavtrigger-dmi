#include <U8glib.h>
#include <EEPROM.h>
#include <AltSoftSerial.h>
#include <wavTrigger.h>
#include <MIDI.h>

#define FAST_FORWARD_THRESHOLD 50
#define FAST_FORWARD_INTERVAL 100

unsigned long button_down_start_time = 0;
unsigned long button_up_start_time = 0;
unsigned long both_buttons_start_time = 0;
bool fast_forward_down_active = false;
bool fast_forward_up_active = false;
bool both_buttons_active = false;

//MIDI_CREATE_INSTANCE(HardwareSerial, Serial, MIDI);
MIDI_CREATE_DEFAULT_INSTANCE();

wavTrigger wTrig;

// 'LogoDMI', 128x64px
const unsigned char epd_bitmap_LogoDMI[] PROGMEM = {
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xf7, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xfc, 0x0f, 0xff, 0xf7, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xf3, 0xf3, 0xff, 0xf7, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xef, 0xfd, 0xfe, 0xf3, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xef, 0xfd, 0xfb, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xdf, 0xfe, 0xff, 0xbf, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xef, 0xdf, 0xfe, 0xf7, 0x7f, 0x7f, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xff, 0xff, 0xff, 0xf7, 0x7f, 0xdf, 0xfe, 0xff, 0xff, 0x7f, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xff, 0xff, 0xff, 0xfd, 0xdd, 0xc0, 0x02, 0xff, 0xff, 0x7f, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xff, 0xff, 0xff, 0xfb, 0xbb, 0xe8, 0x05, 0xfe, 0xff, 0x7f, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xf7, 0xfb, 0xf7, 0xbf, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xfb, 0xf7, 0xfb, 0xde, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xfe, 0x1f, 0xfd, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x07, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xf7, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xf7, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xf7, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xff, 0xff, 0xfe, 0x6d, 0xff, 0xc1, 0xcf, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xff, 0xff, 0xfe, 0x4d, 0xff, 0xc1, 0xcf, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xff, 0xff, 0xff, 0x4d, 0xff, 0xe7, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xff, 0xff, 0xff, 0x41, 0x0b, 0x24, 0x08, 0x61, 0x8c, 0x7f, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xff, 0xff, 0xff, 0x51, 0x69, 0x64, 0xc9, 0x69, 0x24, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xff, 0xff, 0xff, 0x53, 0xe9, 0x64, 0xcb, 0x6d, 0x25, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xff, 0xff, 0xff, 0x13, 0x0c, 0x64, 0xcb, 0x6d, 0x05, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xff, 0xff, 0xff, 0x13, 0x6c, 0xe4, 0xcb, 0x6d, 0x25, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xff, 0xff, 0xff, 0xb3, 0x0c, 0xe4, 0xc8, 0x61, 0x0d, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x7d, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xf9, 0x69, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xfc, 0x63, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xc3, 0xff, 0xff, 0xfd, 0xef, 0xff, 0x3d, 0xff, 0xfc, 0xff, 0xff, 0xf3, 0xff, 0xff, 0xff,
  0xff, 0xcd, 0xff, 0xff, 0xfc, 0xcf, 0xff, 0x3f, 0xff, 0xfc, 0xfe, 0xff, 0xe7, 0xff, 0xff, 0xff,
  0xff, 0xcd, 0xff, 0xff, 0xfc, 0xcf, 0xff, 0x3f, 0xff, 0xfc, 0xfe, 0xff, 0xe7, 0xff, 0xff, 0xff,
  0xff, 0xcd, 0x8b, 0x40, 0x3c, 0x8c, 0x21, 0x05, 0x84, 0x3c, 0x80, 0x42, 0x02, 0x10, 0x87, 0xff,
  0xff, 0xcd, 0xbb, 0x49, 0x3c, 0x2f, 0x24, 0x25, 0xa4, 0xbc, 0x92, 0xda, 0x67, 0x92, 0x93, 0xff,
  0xff, 0xcd, 0xbb, 0x49, 0x3d, 0x2c, 0x27, 0x25, 0xa4, 0x3c, 0x92, 0xc2, 0x66, 0x13, 0x83, 0xff,
  0xff, 0xcd, 0xbb, 0x49, 0x3d, 0x29, 0x27, 0x25, 0xa5, 0xfc, 0x92, 0xde, 0x64, 0x93, 0x9f, 0xff,
  0xff, 0xcd, 0xba, 0x49, 0x3d, 0xe9, 0x24, 0x25, 0xa4, 0xbc, 0x92, 0xda, 0x64, 0x92, 0x93, 0xff,
  0xff, 0xc3, 0xb9, 0x49, 0x3d, 0xec, 0x31, 0x25, 0xa6, 0x3c, 0x92, 0x42, 0x66, 0x18, 0xc7, 0xff,
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xf7, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xf5, 0xcf, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xc5, 0xcf, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xc4, 0xcf, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xc4, 0x8f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xc4, 0x8f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xc4, 0x8f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xdf, 0xff, 0xff, 0xfb, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xdf, 0xfd, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xff, 0xff, 0xff, 0xef, 0x5f, 0xad, 0x77, 0xea, 0xdf, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xff, 0xff, 0xff, 0xeb, 0x59, 0xfd, 0x96, 0xeb, 0xe7, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xff, 0xff, 0xff, 0xed, 0x56, 0xbd, 0x55, 0xea, 0x97, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff
};

const unsigned char bitmap_scrollbar_background[] PROGMEM = {
  0x00, 0x02, 0x00, 0x02, 0x00, 0x02, 0x00, 0x02, 0x00, 0x02, 0x00, 0x02, 0x00, 0x02, 0x00, 0x02,
  0x00, 0x02, 0x00, 0x02, 0x00, 0x02, 0x00, 0x02, 0x00, 0x02, 0x00, 0x02, 0x00, 0x02, 0x00, 0x02,
  0x00, 0x02, 0x00, 0x02, 0x00, 0x02, 0x00, 0x02, 0x00, 0x02, 0x00, 0x02, 0x00, 0x02, 0x00, 0x02,
  0x00, 0x02, 0x00, 0x02, 0x00, 0x02, 0x00, 0x02, 0x00, 0x02, 0x00, 0x02, 0x00, 0x02, 0x00, 0x00
};

const unsigned char bitmap_item_sel_outline[] PROGMEM = {
  0x1f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xc0,
  0x20, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x20,
  0x40, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x30,
  0x40, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x30,
  0x40, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x30,
  0x40, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x30,
  0x40, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x30,
  0x40, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x30,
  0x40, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x30,
  0x40, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x30,
  0x40, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x30,
  0x40, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x30,
  0x40, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x30,
  0x40, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x30,
  0x40, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x30,
  0x40, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x30,
  0x40, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x30,
  0x40, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x30,
  0x40, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x30,
  0x3f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xe0,
  0x1f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xc0
};



int sample_number = 0;

int last_sw;
int sw_cnt;
int re_pos = 0;

int minWTsample = 0;     // minimum sample playable by the WavTrigger
int maxWTsample = 2047;  // maximum samples playable by the WavTrigger: 2047 for the OG, 4095 for Tsunami

#define BUTTON_UP_PIN 2
#define BUTTON_SELECT_PIN 3
#define BUTTON_DOWN_PIN 4
#define SC_output 6  //physical output for sidechain out

int button_up_clicked = 0;
int button_select_clicked = 0;
int button_down_clicked = 0;


U8GLIB_SSD1306_128X64 u8g(U8G_I2C_OPT_DEV_0 | U8G_I2C_OPT_NO_ACK | U8G_I2C_OPT_FAST);
//U8GLIB_SSD1306_128X64_2X u8g(U8G_I2C_OPT_NONE);
//U8GLIB_SSD1306_128X64 u8g(U8G_I2C_OPT_NO_ACK);

const int NUM_ITEMS = 5;
const int MAX_ITEM_LENGTH = 20;

char menu_items[NUM_ITEMS][MAX_ITEM_LENGTH] = {
  { "Drum Kit" },
  { "Sidechain Out" },
  { "Save Kit" },
  { "Load Kit" },
  { "Play Kit" }
};

const int sample_ITEMS = 17;
const int sample_MAX_ITEM_LENGTH = 8;

char sample_items[sample_ITEMS][sample_MAX_ITEM_LENGTH] = {
  { "Kick" },     //C1 --> 36
  { "Snare" },    //D1 --> 38
  { "Clap" },     //Eb1 --> 39
  { "C Hat" },    //F#1 --> 42
  { "O Hat" },    //Bb1 --> 46
  { "Tom 1" },    //D2 --> 50 (Hi Tom)
  { "Tom 2" },    //B1 --> 47 (Low-Mid Tom)
  { "Tom 3" },    //G1 --> 43 (Hi Floor Tom)
  { "Crash" },    //C#2 --> 49
  { "Ride" },     //Eb2 --> 51
  { "Cowbell" },  //Ab2 --> 56
  { "Conga" },    //Eb3 --> 63 (Hi Conga)
  { "Flexi 1" },  // --> 82
  { "Flexi 2" },  // --> 83
  { "Flexi 3" },  // --> 84
  { "Flexi 4" },  // --> 85
  { "Confirm" }
};

const int sidechain_ITEMS = 17;
const int sidechain_MAX_ITEM_LENGTH = 8;

char sidechain_items[sidechain_ITEMS][sidechain_MAX_ITEM_LENGTH] = {
  { "Kick" },
  { "Snare" },
  { "Clap" },
  { "C Hat" },
  { "O Hat" },
  { "Tom 1" },
  { "Tom 2" },
  { "Tom 3" },
  { "Crash" },
  { "Ride" },
  { "Cowbell" },
  { "Conga" },
  { "Flexi 1" },
  { "Flexi 2" },
  { "Flexi 3" },
  { "Flexi 4" },
  { "No SC" }
};

const int savedkit_ITEMS = 10;
const int savedkit_MAX_ITEM_LENGTH = 7;

char savedkit_items[savedkit_ITEMS][savedkit_MAX_ITEM_LENGTH] = {
  { "Kit 1" },
  { "Kit 2" },
  { "Kit 3" },
  { "Kit 4" },
  { "Kit 5" },
  { "Kit 6" },
  { "Kit 7" },
  { "Kit 8" },
  { "Kit 9" },
  { "Kit 10" }
};
const int loadkit_ITEMS = 11;
const int loadkit_MAX_ITEM_LENGTH = 9;

char loadkit_items[loadkit_ITEMS][loadkit_MAX_ITEM_LENGTH] = {
  { "Kit 1" },
  { "Kit 2" },
  { "Kit 3" },
  { "Kit 4" },
  { "Kit 5" },
  { "Kit 6" },
  { "Kit 7" },
  { "Kit 8" },
  { "Kit 9" },
  { "Kit 10" },
  { "Wipe All" }
};

char sidechainOut[8];

int item_selected = 0;
int item_sel_previous;
int item_sel_next;

int sample_selected = 0;
int sample_sel_previous;
int sample_sel_next;

int sidechain_selected = 16;
int sidechain_sel_previous;
int sidechain_sel_next;

int savedkit_selected = 0;
int savedkit_sel_previous;
int savedkit_sel_next;

int loadkit_selected = 0;
int loadkit_sel_previous;
int loadkit_sel_next;

int current_screen = 10;  //0 = Main, 1 = Drum Kit, 2 = Samples Screen, 3 = Sidechain Out, 4 = Save Kit, 5 = Load Kit, 6 = Saved Kit, 7 = Loaded Kit, 8 = Wiped Memory, 9 = Playing Midi, 10 = Startup

const int NUM_SLOTS = 10;
const int ARRAY_SIZE = 16;
const int SLOT_SIZE = ARRAY_SIZE * sizeof(uint32_t);
const int MAGIC_KEY_ADDRESS = 0;
const int DATA_START_ADDRESS = 4;
const int KEY = 12345;

uint32_t selSample[ARRAY_SIZE] = { 0 };
uint32_t loadedData[ARRAY_SIZE] = { 0 };

int midi_notes[ARRAY_SIZE] = { 36, 38, 39, 42, 46, 50, 47, 43, 49, 51, 56, 63, 82, 83, 84, 85 };  //MIDI values for midi mapping

void setup() {
  u8g.setColorIndex(1);

  MIDI.begin(10);  //MIDI CHANNEL 10
  MIDI.setHandleNoteOn(NoteOn);
  MIDI.setHandleNoteOff(NoteOff);

  pinMode(BUTTON_UP_PIN, INPUT_PULLUP);
  pinMode(BUTTON_SELECT_PIN, INPUT_PULLUP);
  pinMode(BUTTON_DOWN_PIN, INPUT_PULLUP);

  int storedKey;
  EEPROM.get(MAGIC_KEY_ADDRESS, storedKey);
  if (storedKey != KEY) {
    saveToEEPROM(0, selSample);

    EEPROM.put(MAGIC_KEY_ADDRESS, KEY);
    for (int i = 0; i < ARRAY_SIZE; i++) {
      loadedData[i] = selSample[i];
    }
  } else {
    loadFromEEPROM(0, loadedData);
  }

  // WAV Trigger startup at 57600
  wTrig.start();
  delay(10);

  wTrig.stopAllTracks();
  wTrig.samplerateOffset(0);
}

void saveToEEPROM(int slot, uint32_t* data) {
  int startAddress = DATA_START_ADDRESS + slot * SLOT_SIZE;
  for (int i = 0; i < ARRAY_SIZE; i++) {
    EEPROM.put(startAddress + i * sizeof(uint32_t), data[i]);
  }
}

void loadFromEEPROM(int slot, uint32_t* data) {
  int startAddress = DATA_START_ADDRESS + slot * SLOT_SIZE;
  for (int i = 0; i < ARRAY_SIZE; i++) {
    EEPROM.get(startAddress + i * sizeof(uint32_t), data[i]);
  }
}

void resetEEPROM() {
  int resetKey = 0;
  EEPROM.put(MAGIC_KEY_ADDRESS, resetKey);
  uint32_t resetData[ARRAY_SIZE] = { 0 };
  for (int slot = 0; slot < NUM_SLOTS; slot++) {
    saveToEEPROM(slot, resetData);
  }
}

void updateSelection(int& selected, int& previous, int& next, int numItems) {
  previous = selected - 1;
  if (previous < 0) {
    previous = numItems - 1;
  }
  next = selected + 1;
  if (next >= numItems) {
    next = 0;
  }
}


void NoteOn(byte channel, byte pitch, byte velocity) {
  if (velocity == 0) return; //

  if (sidechain_selected < 16 && pitch == midi_notes[sidechain_selected]) {
    digitalWrite(SC_output, HIGH);
  }

  for (int i = 0; i < 16; i++) {
    if (pitch == midi_notes[i]) {
      int gain = map(velocity, 1, 127, -70, 0); 
      wTrig.trackGain(selSample[i], gain);
      wTrig.trackPlayPoly(selSample[i]);
    }
  }
}



void NoteOff(byte channel, byte pitch, byte velocity) {
  if (sidechain_selected < 16) {
    if (pitch == midi_notes[sidechain_selected]) {
      digitalWrite(SC_output, LOW);
    }
  }
}


void loop() {

  if (current_screen == 9) {  // PLAY WITH MIDI SCREEN

    MIDI.read();


    if ((digitalRead(BUTTON_UP_PIN) == LOW) && (digitalRead(BUTTON_DOWN_PIN) == LOW) && (button_up_clicked == 0) && (button_down_clicked == 0)) {
      current_screen = 0;
      button_down_clicked = 1;
      button_up_clicked = 1;
    }

    if ((digitalRead(BUTTON_UP_PIN) == HIGH) && (button_up_clicked == 1)) {
      button_up_clicked = 0;
    }
    if ((digitalRead(BUTTON_DOWN_PIN) == HIGH) && (button_down_clicked == 1)) {
      button_down_clicked = 0;
    }
  } else {
    menuHandle();
    screenHandle();
  }
}

void menuHandle() {

  if (current_screen == 0) {  // MENU SCREEN

    if ((digitalRead(BUTTON_UP_PIN) == LOW) && (button_up_clicked == 0)) {
      item_selected = item_selected - 1;
      button_up_clicked = 1;
      if (item_selected < 0) {
        item_selected = NUM_ITEMS - 1;
      }
    } else if ((digitalRead(BUTTON_DOWN_PIN) == LOW) && (button_down_clicked == 0)) {
      item_selected = item_selected + 1;
      button_down_clicked = 1;
      if (item_selected >= NUM_ITEMS) {
        item_selected = 0;
      }
    }

    if ((digitalRead(BUTTON_UP_PIN) == HIGH) && (button_up_clicked == 1)) {
      button_up_clicked = 0;
    }
    if ((digitalRead(BUTTON_DOWN_PIN) == HIGH) && (button_down_clicked == 1)) {
      button_down_clicked = 0;
    }

    if ((digitalRead(BUTTON_SELECT_PIN) == LOW) && (button_select_clicked == 0)) {
      button_select_clicked = 1;
      if (item_selected == 0) {
        current_screen = 1;  // Drum Kit Screen
      } else if (item_selected == 1) {
        current_screen = 3;  // Sidechain Out Screen
      } else if (item_selected == 2) {
        current_screen = 4;  // Save Kit Screen
      } else if (item_selected == 3) {
        current_screen = 5;  // Load Kit Screen
      } else if (item_selected == 4) {
        current_screen = 9;  // Play Screen
      }
    }
    if ((digitalRead(BUTTON_SELECT_PIN) == HIGH) && (button_select_clicked == 1)) {
      button_select_clicked = 0;
    }
  }
  if (current_screen == 1) {  // DRUM KIT SCREEN


    if ((digitalRead(BUTTON_UP_PIN) == LOW) && (button_up_clicked == 0)) {
      sample_selected = sample_selected - 1;
      button_up_clicked = 1;
      if (sample_selected < 0) {
        sample_selected = sample_ITEMS - 1;
      }
    } else if ((digitalRead(BUTTON_DOWN_PIN) == LOW) && (button_down_clicked == 0)) {
      sample_selected = sample_selected + 1;
      button_down_clicked = 1;
      if (sample_selected >= sample_ITEMS) {
        sample_selected = 0;
      }
    }

    if ((digitalRead(BUTTON_UP_PIN) == HIGH) && (button_up_clicked == 1)) {
      button_up_clicked = 0;
    }
    if ((digitalRead(BUTTON_DOWN_PIN) == HIGH) && (button_down_clicked == 1)) {
      button_down_clicked = 0;
    }

    if ((digitalRead(BUTTON_SELECT_PIN) == LOW) && (button_select_clicked == 0)) {
      button_select_clicked = 1;

      if (sample_selected >= 0 && sample_selected <= 15) {
        current_screen = 2;
      } else if (sample_selected == 16) {
        current_screen = 0;  // if "Confirm", go back to main menu
        sample_selected = 0;
      }
    }
    if ((digitalRead(BUTTON_SELECT_PIN) == HIGH) && (button_select_clicked == 1)) {
      button_select_clicked = 0;
    }
  }


  if (current_screen == 2) {

    if ((digitalRead(BUTTON_UP_PIN) == LOW) && (button_up_clicked == 0)) {
      sample_number = sample_number - 1;
      wTrig.trackPlayPoly(sample_number);
      button_up_clicked = 1;

      if (sample_number < minWTsample) {
        sample_number = maxWTsample;
      }

      selSample[sample_selected] = sample_number;
      button_up_start_time = millis();
      fast_forward_up_active = true;
    }

    if ((digitalRead(BUTTON_DOWN_PIN) == LOW) && (button_down_clicked == 0)) {
      sample_number = sample_number + 1;
      wTrig.trackPlayPoly(sample_number);
      button_down_clicked = 1;

      if (sample_number > maxWTsample) {
        sample_number = minWTsample;
      }

      selSample[sample_selected] = sample_number;
      button_down_start_time = millis();
      fast_forward_down_active = true;
    }

    // Check if both buttons are pressed
    if (digitalRead(BUTTON_UP_PIN) == LOW && digitalRead(BUTTON_DOWN_PIN) == LOW) {
      if (!both_buttons_active) {
        both_buttons_start_time = millis();
        both_buttons_active = true;
      } else if (millis() - both_buttons_start_time >= FAST_FORWARD_THRESHOLD) {
        sample_selected = 0;          // Reset sample selection if both buttons are held
        both_buttons_active = false;  // Prevent multiple triggers
      }
    } else {
      both_buttons_active = false;  // Reset if any button is released
    }

    // Fast forward for DOWN button
    if (digitalRead(BUTTON_DOWN_PIN) == LOW && fast_forward_down_active) {
      if (millis() - button_down_start_time >= FAST_FORWARD_THRESHOLD) {
        if (millis() - button_down_start_time >= FAST_FORWARD_INTERVAL) {
          sample_number = sample_number + 1;
          if (sample_number > maxWTsample) {
            sample_number = minWTsample;
          }
          selSample[sample_selected] = sample_number;
          wTrig.trackPlayPoly(sample_number);
          button_down_start_time = millis();  // Reset interval timer
        }
      }
    }

    // Fast forward for UP button
    if (digitalRead(BUTTON_UP_PIN) == LOW && fast_forward_up_active) {
      if (millis() - button_up_start_time >= FAST_FORWARD_THRESHOLD) {
        if (millis() - button_up_start_time >= FAST_FORWARD_INTERVAL) {
          sample_number = sample_number - 1;
          if (sample_number < minWTsample) {
            sample_number = maxWTsample;
          }
          selSample[sample_selected] = sample_number;
          wTrig.trackPlayPoly(sample_number);
          button_up_start_time = millis();  // Reset interval timer
        }
      }
    }

    if ((digitalRead(BUTTON_UP_PIN) == HIGH) && (button_up_clicked == 1)) {
      button_up_clicked = 0;
      fast_forward_up_active = false;
    }

    if ((digitalRead(BUTTON_DOWN_PIN) == HIGH) && (button_down_clicked == 1)) {
      button_down_clicked = 0;
      fast_forward_down_active = false;
    }

    if ((digitalRead(BUTTON_SELECT_PIN) == LOW) && (button_select_clicked == 0)) {
      button_select_clicked = 1;
      current_screen = 1;
      sample_number = 0;
    }

    if ((digitalRead(BUTTON_SELECT_PIN) == HIGH) && (button_select_clicked == 1)) {
      button_select_clicked = 0;
    }
  }


  if (current_screen == 3) {  // SIDECHAIN SELECTION SCREEN


    if ((digitalRead(BUTTON_UP_PIN) == LOW) && (button_up_clicked == 0)) {
      sidechain_selected = sidechain_selected - 1;
      button_up_clicked = 1;
      if (sidechain_selected < 0) {
        sidechain_selected = sidechain_ITEMS - 1;
      }
    } else if ((digitalRead(BUTTON_DOWN_PIN) == LOW) && (button_down_clicked == 0)) {
      sidechain_selected = sidechain_selected + 1;
      button_down_clicked = 1;
      if (sidechain_selected >= sidechain_ITEMS) {
        sidechain_selected = 0;
      }
    }

    if ((digitalRead(BUTTON_UP_PIN) == HIGH) && (button_up_clicked == 1)) {
      button_up_clicked = 0;
    }
    if ((digitalRead(BUTTON_DOWN_PIN) == HIGH) && (button_down_clicked == 1)) {
      button_down_clicked = 0;
    }

    if ((digitalRead(BUTTON_SELECT_PIN) == LOW) && (button_select_clicked == 0)) {
      button_select_clicked = 1;
      //strcpy(sidechainOut, sidechain_items[sidechain_selected]);
      current_screen = 0;
    }
    if ((digitalRead(BUTTON_SELECT_PIN) == HIGH) && (button_select_clicked == 1)) {
      button_select_clicked = 0;
    }
  }

  if (current_screen == 4) {  // SAVE KIT SCREEN


    if ((digitalRead(BUTTON_UP_PIN) == LOW) && (button_up_clicked == 0)) {
      savedkit_selected = savedkit_selected - 1;
      button_up_clicked = 1;
      if (savedkit_selected < 0) {
        savedkit_selected = savedkit_ITEMS - 1;
      }
    } else if ((digitalRead(BUTTON_DOWN_PIN) == LOW) && (button_down_clicked == 0)) {
      savedkit_selected = savedkit_selected + 1;
      button_down_clicked = 1;
      if (savedkit_selected >= savedkit_ITEMS) {
        savedkit_selected = 0;
      }
    }

    if ((digitalRead(BUTTON_UP_PIN) == HIGH) && (button_up_clicked == 1)) {
      button_up_clicked = 0;
    }
    if ((digitalRead(BUTTON_DOWN_PIN) == HIGH) && (button_down_clicked == 1)) {
      button_down_clicked = 0;
    }

    if ((digitalRead(BUTTON_SELECT_PIN) == LOW) && (button_select_clicked == 0)) {
      button_select_clicked = 1;
      saveToEEPROM(savedkit_selected, selSample);
      if (current_screen == 6) {
        current_screen = 0;
      }
      current_screen = 6;
    }
    if ((digitalRead(BUTTON_SELECT_PIN) == HIGH) && (button_select_clicked == 1)) {
      button_select_clicked = 0;
    }
  }

  if (current_screen == 5) {  // LOAD KIT SCREEN


    if ((digitalRead(BUTTON_UP_PIN) == LOW) && (button_up_clicked == 0)) {
      loadkit_selected = loadkit_selected - 1;
      button_up_clicked = 1;
      if (loadkit_selected < 0) {
        loadkit_selected = loadkit_ITEMS - 1;
      }
    } else if ((digitalRead(BUTTON_DOWN_PIN) == LOW) && (button_down_clicked == 0)) {
      loadkit_selected = loadkit_selected + 1;
      button_down_clicked = 1;
      if (loadkit_selected >= loadkit_ITEMS) {
        loadkit_selected = 0;
      }
    }

    if ((digitalRead(BUTTON_UP_PIN) == HIGH) && (button_up_clicked == 1)) {
      button_up_clicked = 0;
    }
    if ((digitalRead(BUTTON_DOWN_PIN) == HIGH) && (button_down_clicked == 1)) {
      button_down_clicked = 0;
    }

    if ((digitalRead(BUTTON_SELECT_PIN) == LOW) && (button_select_clicked == 0)) {
      button_select_clicked = 1;
      if (loadkit_selected >= 0 && loadkit_selected <= 9) {
        loadFromEEPROM(loadkit_selected, selSample);
        if (current_screen == 7) {
          current_screen = 0;
        } else {
          current_screen = 7;
        }
      } else if (loadkit_selected == 10) {
        resetEEPROM();
        if (current_screen == 8) {
          current_screen = 0;
        } else {
          current_screen = 8;
        }
      }
    }
    if ((digitalRead(BUTTON_SELECT_PIN) == HIGH) && (button_select_clicked == 1)) {
      button_select_clicked = 0;
    }
  }

  if (current_screen == 6) {  // KIT SAVED SCREEN

    if ((digitalRead(BUTTON_UP_PIN) == LOW) && (button_up_clicked == 0)) {
      current_screen = 0;
      savedkit_selected = 0;
      button_up_clicked = 1;
    }

    else if ((digitalRead(BUTTON_DOWN_PIN) == LOW) && (button_down_clicked == 0)) {
      current_screen = 0;
      savedkit_selected = 0;
      button_down_clicked = 1;
    }

    if ((digitalRead(BUTTON_UP_PIN) == HIGH) && (button_up_clicked == 1)) {
      button_up_clicked = 0;
    }
    if ((digitalRead(BUTTON_DOWN_PIN) == HIGH) && (button_down_clicked == 1)) {
      button_down_clicked = 0;
    }

    if ((digitalRead(BUTTON_SELECT_PIN) == LOW) && (button_select_clicked == 0)) {
      current_screen = 0;
      savedkit_selected = 0;
      button_select_clicked = 1;
    }
    if ((digitalRead(BUTTON_SELECT_PIN) == HIGH) && (button_select_clicked == 1)) {
      button_select_clicked = 0;
    }
  }

  if (current_screen == 7) {  // KIT LOADED SCREEN

    if ((digitalRead(BUTTON_UP_PIN) == LOW) && (button_up_clicked == 0)) {
      current_screen = 0;
      loadkit_selected = 0;
      button_up_clicked = 1;
    } else if ((digitalRead(BUTTON_DOWN_PIN) == LOW) && (button_down_clicked == 0)) {
      current_screen = 0;
      loadkit_selected = 0;
      button_down_clicked = 1;
    }

    if ((digitalRead(BUTTON_UP_PIN) == HIGH) && (button_up_clicked == 1)) {
      button_up_clicked = 0;
    }
    if ((digitalRead(BUTTON_DOWN_PIN) == HIGH) && (button_down_clicked == 1)) {
      button_down_clicked = 0;
    }

    if ((digitalRead(BUTTON_SELECT_PIN) == LOW) && (button_select_clicked == 0)) {
      current_screen = 0;
      loadkit_selected = 0;
      button_select_clicked = 1;
    }
    if ((digitalRead(BUTTON_SELECT_PIN) == HIGH) && (button_select_clicked == 1)) {
      button_select_clicked = 0;
    }
  }

  if (current_screen == 8) {  // MEMORY WIPED SCREEN

    if ((digitalRead(BUTTON_UP_PIN) == LOW) && (button_up_clicked == 0)) {
      current_screen = 0;
      loadkit_selected = 0;
      button_up_clicked = 1;
    } else if ((digitalRead(BUTTON_DOWN_PIN) == LOW) && (button_down_clicked == 0)) {
      current_screen = 0;
      loadkit_selected = 0;
      button_down_clicked = 1;
    }

    if ((digitalRead(BUTTON_UP_PIN) == HIGH) && (button_up_clicked == 1)) {
      button_up_clicked = 0;
    }
    if ((digitalRead(BUTTON_DOWN_PIN) == HIGH) && (button_down_clicked == 1)) {
      button_down_clicked = 0;
    }

    if ((digitalRead(BUTTON_SELECT_PIN) == LOW) && (button_select_clicked == 0)) {
      current_screen = 0;
      loadkit_selected = 0;
      button_select_clicked = 1;
    }
    if ((digitalRead(BUTTON_SELECT_PIN) == HIGH) && (button_select_clicked == 1)) {
      button_select_clicked = 0;
    }
  }

  if (current_screen == 10) {  // STARTUP SCREEN
    if ((digitalRead(BUTTON_UP_PIN) == LOW) && (button_up_clicked == 0)) {
      current_screen = 0;
      button_up_clicked = 1;
    } else if ((digitalRead(BUTTON_DOWN_PIN) == LOW) && (button_down_clicked == 0)) {
      current_screen = 0;
      button_down_clicked = 1;
    }

    if ((digitalRead(BUTTON_UP_PIN) == HIGH) && (button_up_clicked == 1)) {
      button_up_clicked = 0;
    }
    if ((digitalRead(BUTTON_DOWN_PIN) == HIGH) && (button_down_clicked == 1)) {
      button_down_clicked = 0;
    }

    if ((digitalRead(BUTTON_SELECT_PIN) == LOW) && (button_select_clicked == 0)) {
      current_screen = 0;
      button_select_clicked = 1;
    }
    if ((digitalRead(BUTTON_SELECT_PIN) == HIGH) && (button_select_clicked == 1)) {
      button_select_clicked = 0;
    }
  }


  updateSelection(item_selected, item_sel_previous, item_sel_next, NUM_ITEMS);
  updateSelection(sample_selected, sample_sel_previous, sample_sel_next, sample_ITEMS);
  updateSelection(sidechain_selected, sidechain_sel_previous, sidechain_sel_next, sidechain_ITEMS);
  updateSelection(savedkit_selected, savedkit_sel_previous, savedkit_sel_next, savedkit_ITEMS);
  updateSelection(loadkit_selected, loadkit_sel_previous, loadkit_sel_next, loadkit_ITEMS);
}

void screenHandle() {

  u8g.firstPage();
  do {

    if (current_screen == 0) {  // MENU SCREEN

      u8g.drawBitmapP(0, 22, 128 / 8, 21, bitmap_item_sel_outline);

      u8g.setFont(u8g_font_7x14);
      u8g.drawStr(25, 15, menu_items[item_sel_previous]);

      u8g.setFont(u8g_font_7x14B);
      u8g.drawStr(25, 15 + 20 + 2, menu_items[item_selected]);

      u8g.setFont(u8g_font_7x14);
      u8g.drawStr(25, 15 + 20 + 20 + 2 + 2, menu_items[item_sel_next]);

      u8g.drawBitmapP(128 - 8, 0, 8 / 8, 64, bitmap_scrollbar_background);

      u8g.drawBox(125, 64 / NUM_ITEMS * item_selected, 3, 64 / NUM_ITEMS);

    } else if (current_screen == 1) {  // SAMPLES SCREEN

      char s_sel_prev[5];
      char s_sel_act[5];
      char s_sel_next[5];
      sprintf(s_sel_prev, "%d", selSample[sample_sel_previous]);
      sprintf(s_sel_act, "%d", selSample[sample_selected]);
      sprintf(s_sel_next, "%d", selSample[sample_sel_next]);

      u8g.drawBitmapP(0, 22, 128 / 8, 21, bitmap_item_sel_outline);


      u8g.setFont(u8g_font_7x14);
      u8g.drawStr(25, 15, sample_items[sample_sel_previous]);
      if (sample_sel_previous >= 0 && sample_sel_previous <= 15) {
        u8g.drawStr(80, 15, s_sel_prev);
      }


      u8g.setFont(u8g_font_7x14B);
      u8g.drawStr(25, 15 + 20 + 2, sample_items[sample_selected]);
      if (sample_selected >= 0 && sample_selected <= 15) {
        u8g.drawStr(80, 15 + 20 + 2, s_sel_act);
      }


      u8g.setFont(u8g_font_7x14);
      u8g.drawStr(25, 15 + 20 + 20 + 2 + 2, sample_items[sample_sel_next]);
      if (sample_sel_next >= 0 && sample_sel_next <= 15) {
        u8g.drawStr(80, 15 + 20 + 20 + 2 + 2, s_sel_next);
      }


      u8g.drawBitmapP(128 - 8, 0, 8 / 8, 64, bitmap_scrollbar_background);


      u8g.drawBox(125, 64 / sample_ITEMS * sample_selected, 3, 64 / sample_ITEMS);

    } else if (current_screen == 2) {  // SAMPLE SELECTION SCREEN

      u8g.drawBitmapP(0, 22, 128 / 8, 21, bitmap_item_sel_outline);

      u8g.setFont(u8g_font_7x14);
      u8g.drawStr(25, 15, sample_items[sample_selected]);

      u8g.setFont(u8g_font_7x14B);
      char s_num[5];
      sprintf(s_num, "%d", selSample[sample_selected]);
      u8g.drawStr(25, 15 + 20 + 2, s_num);

    } else if (current_screen == 3) {  // SIDECHAIN SELECTION SCREEN

      u8g.drawBitmapP(0, 22, 128 / 8, 21, bitmap_item_sel_outline);

      u8g.setFont(u8g_font_7x14);
      u8g.drawStr(25, 15, "Sidechain Out");

      u8g.setFont(u8g_font_7x14B);
      u8g.drawStr(25, 15 + 20 + 2, sidechain_items[sidechain_selected]);
    } else if (current_screen == 4) {  // SAVE KIT SCREEN

      u8g.drawBitmapP(0, 22, 128 / 8, 21, bitmap_item_sel_outline);


      u8g.setFont(u8g_font_7x14);
      u8g.drawStr(25, 15, savedkit_items[savedkit_sel_previous]);

      u8g.setFont(u8g_font_7x14B);
      u8g.drawStr(25, 15 + 20 + 2, savedkit_items[savedkit_selected]);

      u8g.setFont(u8g_font_7x14);
      u8g.drawStr(25, 15 + 20 + 20 + 2 + 2, savedkit_items[savedkit_sel_next]);


      u8g.drawBitmapP(128 - 8, 0, 8 / 8, 64, bitmap_scrollbar_background);


      u8g.drawBox(125, 64 / savedkit_ITEMS * savedkit_selected, 3, 64 / savedkit_ITEMS);
    } else if (current_screen == 5) {  // LOAD KIT SCREEN

      u8g.drawBitmapP(0, 22, 128 / 8, 21, bitmap_item_sel_outline);


      u8g.setFont(u8g_font_7x14);
      u8g.drawStr(25, 15, loadkit_items[loadkit_sel_previous]);

      u8g.setFont(u8g_font_7x14B);
      u8g.drawStr(25, 15 + 20 + 2, loadkit_items[loadkit_selected]);

      u8g.setFont(u8g_font_7x14);
      u8g.drawStr(25, 15 + 20 + 20 + 2 + 2, loadkit_items[loadkit_sel_next]);


      u8g.drawBitmapP(128 - 8, 0, 8 / 8, 64, bitmap_scrollbar_background);


      u8g.drawBox(125, 64 / loadkit_ITEMS * loadkit_selected, 3, 64 / loadkit_ITEMS);
    } else if (current_screen == 6) {  // KIT SAVED SCREEN
      u8g.setFont(u8g_font_6x12);
      u8g.drawStr(4, 5 + 20 + 2, "Kit has been saved!");
      u8g.drawStr(4, 5 + 20 + 20 + 2 + 2, "Press to continue...");

    } else if (current_screen == 7) {  // KIT LOADED SCREEN
      u8g.setFont(u8g_font_6x12);
      u8g.drawStr(4, 5 + 20 + 2, "Kit has been loaded!");
      u8g.drawStr(4, 5 + 20 + 20 + 2 + 2, "Press to continue...");

    }

    else if (current_screen == 8) {  // MEMORY WIPED SCREEN
      u8g.setFont(u8g_font_6x12);
      u8g.drawStr(4, 5 + 20 + 2, "Memory neuralyzed!");
      u8g.drawStr(4, 5 + 20 + 20 + 2 + 2, "Press to continue...");
    }

    else if (current_screen == 9) {  // PLAYING MIDI SCREEN
      u8g.setFont(u8g_font_6x12);
      u8g.drawStr(4, 5 + 20 + 2, "MIDI In Activated");
      u8g.drawStr(4, 5 + 20 + 20 + 2 + 2, "UP + DOWN to exit");
    }

    else if (current_screen == 10) {  // STARTUP SCREEN
      u8g.drawBitmapP(0, 0, 128 / 8, 64, epd_bitmap_LogoDMI);
    }

  } while (u8g.nextPage());
}