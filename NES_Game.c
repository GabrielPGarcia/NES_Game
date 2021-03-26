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
#define NUM_ACTORS 1
#define DEF_METASPRITE_2x2(name,code,pal)\
const unsigned char name[]={\
  0,0, (code)+0, pal, \
  0,8, (code)+1, pal, \
  8,0, (code)+2, pal, \
  8,8, (code)+3, pal,\
  128\
};
#define DEF_METASPRITE_2x2_FLIP(name,code,pal)\
const unsigned char name[]={\
        8,      0,      (code)+0,   (pal)|OAM_FLIP_H, \
        8,      8,      (code)+1,   (pal)|OAM_FLIP_H, \
        0,      0,      (code)+2,   (pal)|OAM_FLIP_H, \
        0,      8,      (code)+3,   (pal)|OAM_FLIP_H, \
        128};
const char PALETTE[32] = { 
  0x99,			// screen color

  0x0f,0x21,0x31,0xf,	// background palette 0
  0x00,0x00,0x00,0x0,	// background palette 1
  0x00,0x00,0x00,0x0,	// background palette 2
  0x00,0x00,0x00,0x0,   // background palette 3

  0x0f,0x00,0x30,0x0,	// sprite palette 0
  0x0f,0xc1,0x30,0x0,	// sprite palette 0
};
//-----------------Player's sprite--------

DEF_METASPRITE_2x2(playerRStand, 0xd8, 0);
DEF_METASPRITE_2x2(playerRRun1, 0xdc, 0);
DEF_METASPRITE_2x2(playerRRun2, 0xe0, 0);
DEF_METASPRITE_2x2(playerRRun3, 0xe4, 0);
DEF_METASPRITE_2x2_FLIP(playerLStand, 0xd8, 0);
DEF_METASPRITE_2x2_FLIP(playerLRun1, 0xdc, 0);
DEF_METASPRITE_2x2_FLIP(playerLRun2, 0xe0, 0);
DEF_METASPRITE_2x2_FLIP(playerLRun3, 0xe4, 0);

const unsigned char* const playerRunSeq[16] = 
{
  playerLRun1, playerLRun2, playerLRun3, playerLRun1, playerLRun2, playerLRun3, 
  playerLRun1, playerLRun2, playerRRun1, playerRRun2, playerRRun3, 
  playerRRun1, playerRRun2, playerRRun3, playerRRun1, playerRRun2  
};

byte actor_x[NUM_ACTORS];
byte actor_y[NUM_ACTORS];
sbyte actor_dx[NUM_ACTORS];
sbyte actor_dy[NUM_ACTORS];
int i;
char pad;
char oam_id;
byte runseq;
word x_scroll;		// X scroll amount in pixels
byte seg_height;	// segment height in metatiles
byte seg_width;		// segment width in metatiles
byte seg_char;		// character to draw
byte seg_palette;	// attribute table value
// number of rows in scrolling playfield (without status bar)
#define PLAYROWS 24
// a vertical slice of attribute table entries
char attrbuf[PLAYROWS/4];
// buffers that hold vertical slices of nametable data
char ntbuf1[PLAYROWS];	// left side
char ntbuf2[PLAYROWS];	// right side

typedef struct Actor {
  word yy;		// Y position in pixels (16 bit)
  byte x;		// X position in pixels (8 bit)
  byte y;		// X position in pixels (8 bit)
  byte state;		// ActorState
  int onscreen:1;	// is actor onscreen?
} Actor;

Actor actors;	// all actors
int playerp;
void actors_setup()
{
  actor_x[0] = 100;
  actor_y[0] = 100;
  actor_dx[0] = 0;
  actor_dy[0] = 1;
}
// setup PPU and tables
void setup_graphics() {
  ppu_off();
  oam_clear();
  //oam_hide_rest(0);
  pal_all(PALETTE);
  ppu_on_all();
}
void player_action()
{ 
  if(pad&PAD_A);
  if (pad&PAD_UP)actor_dy[0]=-3;
  else if (pad&PAD_DOWN)actor_dy[0]=3;
  //-------------wall collision----------
  if(actor_y[0] >= 25 &&  actor_y[0] != 185)
  {
    actor_dy[0] = - actor_dy[0];  
  }
  if(actor_y[0] <= 185 &&  actor_y[0] != 25)
  {
    actor_dy[0] = - actor_dy[0];
  }
  for (i=0; i<NUM_ACTORS; i++) {
    byte runseq = actor_y[i] & 7;
    if (actor_dx[i] >= 0 )
      runseq += 8;
    oam_id = oam_meta_spr(actor_x[i], actor_y[i], oam_id, playerRunSeq[runseq]);
    actor_x[i] += actor_dx[i];
    actor_y[i] += actor_dy[i];
 ppu_wait_frame();
  }
  if (oam_id!=0) oam_hide_rest(oam_id);
}

//------------------main Game loop--------------
void game_loop()
{ 
  player_action();  
  ppu_wait_frame();
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
// function to scroll window up and down until end
void Level_one(const byte* pal, const byte* rle,const byte* rle2) {
  // disable rendering
  ppu_off();
  // set palette, virtual bright to 0 (total black)
  pal_bg(pal);
  
  // unpack nametable into the VRAM
  vram_adr(0x2000);
  vram_unrle(rle);
 vram_adr(0x2400);
  vram_unrle(rle2);
  // enable rendering
  ppu_on_all();
}


void Start()
{
  ppu_off();
  vram_adr(NTADR_A(7,9));
  vram_write("Air Conbat Chopper", 18);
  
  vram_adr(NTADR_A(10,12));
  vram_write("[A] to start!", 12);
  ppu_on_all();
}

void main(void)
{
  int InGame = 0;  
  int GameState = 1;
  int x = 0;   // x scroll position
  int y = 0;   // y scroll position
  int dx = 1;  // y scroll direction
   //show_title_screen(Title_Screen_pal, Title_Screen_rle);
   //show_title_screen(Level_1E_pal, Level_1E_rle);
   //show_title_screen(Level_2E_pal, Level_2E_rle);
   //show_title_screen(Level_3E_pal, Level_3E_rle);
  setup_graphics();
  Level_one(Level_1E_pal, Level_1E_rle,Level_2E_rle);
  // infinite loop
  while(1) 
  {   
    switch(InGame)
    {
      case 0: 
        show_title_screen(Title_Screen_pal, Title_Screen_rle);        
        Start();        
        actors_setup();
        InGame = 2;
        break;
      case 1:   
        oam_id = 0;
        game_loop();
        break;
      case 2: 
        
        show_title_screen(Level_1E_pal, Level_1E_rle);           
        oam_id = 0;
        game_loop();        
        InGame = 1;
	break;
      case 3: 

        break;
      case 4:  

        break;
        
    }
        x += dx;
        scroll(x, y);   
    
    pad = pad_trigger(0);
    if(pad&PAD_A){InGame = 2;}  

  }

}
