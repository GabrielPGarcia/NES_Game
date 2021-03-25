
#include <stdlib.h>
#include <string.h>

// include NESLIB header
#include "neslib.h"

// include CC65 NES Header (PPU)
#include <nes.h>

// link the pattern table into CHR ROM
//#link "chr_generic.s"

// BCD arithmetic support
#include "bcd.h"
//#link "bcd.c"

// VRAM update buffer
#include "vrambuf.h"
//#link "vrambuf.c"

extern const byte Title_Screen_pal[16];
extern const byte Title_Screen_rle[];

extern const byte Level_1E_pal[16];
extern const byte Level_1E_rle[];

extern const byte Level_2E_pal[16];
extern const byte Level_2E_rle[];

extern const byte Level_3E_pal[16];
extern const byte Level_3E_rle[];

// link the pattern table into CHR ROM
//#link "Title_Screen.s"
//#link "Level_1E.s"
//#link "Level_2E.s"
//#link "Level_3E.s"

/*{pal:"nes",layout:"nes"}*/
const char PALETTE[32] = { 
  0x0F,			// screen color

  0x07,0x0B,0x1B,0x00,	// background palette 0
  0x1C,0x20,0x2C,0x00,	// background palette 1
  0x00,0x10,0x20,0x00,	// background palette 2
  0x06,0x16,0x26,0x00,   // background palette 3

  0x16,0x35,0x24,0x00,	// sprite palette 0
  0x00,0x37,0x25,0x00,	// sprite palette 1
  0x0D,0x2D,0x3A,0x00,	// sprite palette 2
  0x0D,0x27,0x2A	// sprite palette 3
};

// setup PPU and tables
void setup_graphics() {
  // clear sprites
  oam_clear();
  // set palette colors
  pal_all(PALETTE);
}
void show_title_screen(const byte* pal, const byte* rle) {
  // disable rendering
  ppu_off();
  // set palette, virtual bright to 0 (total black)
  pal_bg(pal);
  
  // unpack nametable into the VRAM
  vram_adr(0x2000);
  vram_unrle(rle);
  // enable rendering
  ppu_on_all();
}
void main(void)
{

   //show_title_screen(Title_Screen_pal, Title_Screen_rle);
   show_title_screen(Level_1E_pal, Level_1E_rle);
   //show_title_screen(Level_2E_pal, Level_2E_rle);
   //show_title_screen(Level_3E_pal, Level_3E_rle);
  
  // infinite loop
  while(1) {
  }
}
