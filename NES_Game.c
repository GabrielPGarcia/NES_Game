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
#define NUM_ENEMY 15


#define DEF_METASPRITE_2x2(name,code,pal)\
const unsigned char name[]={\
  0,0, (code)+0, pal, \
  0,8, (code)+1, pal, \
  8,0, (code)+2, pal, \
  8,8, (code)+3, pal,\
  128\
};
#define DEF_METASPRITE_2x2_M1(name,code,pal)\
const unsigned char name[]={\
  0,0, (code)+0, pal, \
  0,8, (code)+1, pal, \
  8,0, (code)+2, pal, \
  8,8, (code)+3, pal,\
  128\
};
#define DEF_METASPRITE_2x2_M2(name,code,pal)\
const unsigned char name[]={\
  0,8, (code)+0, (pal)|OAM_FLIP_V, \
  0,0, (code)+1, (pal)|OAM_FLIP_V, \
  8,8, (code)+2, (pal)|OAM_FLIP_V, \
  8,0, (code)+3, (pal)|OAM_FLIP_V, \
  128\
};
#define DEF_METASPRITE_2x2_M3(name,code,pal)\
const unsigned char name[]={\
  8,0, (code)+0, (pal)|OAM_FLIP_H, \
  8,8, (code)+1, (pal)|OAM_FLIP_H, \
  0,0, (code)+2, (pal)|OAM_FLIP_H, \
  0,8, (code)+3, (pal)|OAM_FLIP_H, \
  128\
};

const char PALETTE[32] = { 
  0x99,			// screen color

  0x0f,0x21,0x31,0xf,	// background palette 0
  0x00,0x00,0x00,0x0,	// background palette 1
  0x00,0x00,0x00,0x0,	// background palette 2
  0x00,0x00,0x00,0x0,   // background palette 3

  0x0f,0x30,0x2d,0x0,	// sprite palette 0
  0x0f,0xc1,0x30,0x0,	// sprite palette 0
};
//-----------------Player's sprite--------

DEF_METASPRITE_2x2(playerRRun0, 0xd8, 0);
DEF_METASPRITE_2x2(playerRRun1, 0xdc, 0);
DEF_METASPRITE_2x2(playerRRun2, 0xe0, 0);
DEF_METASPRITE_2x2(playerRRun3, 0xe4, 0);

DEF_METASPRITE_2x2_M1(EnemyM1U, 0xc4, 0);
DEF_METASPRITE_2x2_M2(EnemyM1D, 0xc4, 0);
DEF_METASPRITE_2x2_M3(EnemyM1R, 0xcc, 0);
DEF_METASPRITE_2x2_M1(EnemyM1L, 0xcc, 0);

DEF_METASPRITE_2x2_M1(EnemyM2U, 0xc8, 0);
DEF_METASPRITE_2x2_M2(EnemyM2D, 0xc8, 0);
DEF_METASPRITE_2x2_M3(EnemyM2R, 0xd0, 0);
DEF_METASPRITE_2x2_M1(EnemyM2L, 0xd0, 0);

byte actor_x[NUM_ACTORS];
byte actor_y[NUM_ACTORS];
sbyte actor_dx[NUM_ACTORS];
sbyte actor_dy[NUM_ACTORS];

byte enemy_x[NUM_ENEMY];
byte enemy_y[NUM_ENEMY];
sbyte enemy_dx[NUM_ENEMY];
sbyte enemy_dy[NUM_ENEMY];

int i;
int iCount;
char pad;
char oam_id;
byte runseq;
int lad=0;
int lads=0;
int iRand;

typedef struct Actor {
  word yy;		// Y position in pixels (16 bit)
  byte x;		// X position in pixels (8 bit)
  byte y;		// X position in pixels (8 bit)
  byte state;		// ActorState
  int onscreen:1;	// is actor onscreen?
} Actor;
typedef struct Shouts {
  word yy;		// Y position in pixels (16 bit)
  byte x;		// X position in pixels (8 bit)
  byte y;		// X position in pixels (8 bit)
  byte state;		// ActorState
} Shouts;

Shouts shout[9];
Actor actors;	// all actors
Actor enemy1;	// all actors
int shoutinaction = 0;
int playerp;

void actors_setup()
{
  actor_x[0] = 50;
  actor_y[0] = 100;
  actor_dx[0] = 0;
  actor_dy[0] = 0;
}
void enemy_setup()
{
  enemy_x[0] = 200;
  enemy_y[0] = 100;
  enemy_dx[0] = 0;
  enemy_dy[0] = 0;
}
// setup PPU and tables
void setup_graphics() {
  ppu_off();
  oam_clear();
  //oam_hide_rest(0);
  pal_all(PALETTE);
  ppu_on_all();
}
void setup_Shout()
{
  if(lads == 1)
    shout[0].x = actor_x[0]+8;
    if(lads == 2)
      shout[0].x = actor_x[0] + 2;
    if(lads == 3)
      shout[0].y = actor_y[0];
    if(lads == 4)
      shout[0].y = actor_y[0]+8;
    shout[0].state = 28;
    oam_id = oam_spr(shout[0].x, shout[0].y, shout[0].state, 4, oam_id);
}
void ShoutUsed()
{
  if(shoutinaction == 1 && iCount <= 50)
  {
    if(lads == 1)
    {
      shout[0].y = shout[0].y - 2;
      shout[0].state = 28;
      oam_id = oam_spr(shout[0].x, shout[0].y, shout[0].state, 4, oam_id); 
    }
    if(lads == 2)
    {
      shout[0].y = shout[0].y + 2;
      shout[0].state = 28;
      oam_id = oam_spr(shout[0].x, shout[0].y, shout[0].state, 4, oam_id); 
    }
    if(lads == 3)
    {
      shout[0].x = shout[0].x - 2;
      shout[0].state = 29;
      oam_id = oam_spr(shout[0].x, shout[0].y, shout[0].state, 4, oam_id); 
    }
    if(lads == 4)
    {
      shout[0].x = shout[0].x + 2;
      shout[0].state = 29;
      oam_id = oam_spr(shout[0].x, shout[0].y, shout[0].state, 4, oam_id); 
    }
    iCount = iCount + 1;
  }
  else if(iCount > 0)
  {
    shoutinaction = 0;
    iCount = 0;
  }
  else 
  {    
      shout[0].x = actor_x[0];
      shout[0].y = actor_y[0];
      shout[0].state = 0;
      oam_id = oam_spr(shout[0].x, shout[0].y, shout[0].state, 4, oam_id); 
  }

}
//void does_missile_hit_player() {
//  byte i;
//  if (player_exploding)
//    return;
//  for (i=0; i<MAX_ATTACKERS; i++) {
//    if (missiles[i].ypos != YOFFSCREEN && 
//        in_rect(missiles[i].xpos, missiles[i].ypos + 16, 
//                player_x, player_y, 16, 16)) {
//      player_exploding = 1;
//      break;
//    }
//  }
//}
void enemy_action()
{ 
  if(shoutinaction == 1)
  {
   if((shout[0].x >= enemy_x[0] + 15 && shout[i].x <= enemy_x[0])&& 
       (shout[0].y >= enemy_y[0] - 15 && shout[i].y <= enemy_y[0]+15)) 
    {
      shout[i].state = 0;
      enemy_setup();
    }
  }
  if( enemy_x[0]!=actor_x[0] || enemy_y[0] != actor_y[0])
  {
    if((shout[0].x >= enemy_x[0]-4 && shout[0].x <= enemy_x[0]+8)&& 
       (shout[0].y >= enemy_y[0]-2 && shout[0].y <= enemy_y[0]+4)) 
    {
      shout[i].state = 0;
      enemy_setup();
    }
    if(actor_x[0] > enemy_x[0]){
      enemy_x[0] = enemy_x[0] + 1;
      oam_id = oam_meta_spr(enemy_x[0], enemy_y[0], oam_id, EnemyM2R);}
    else if(actor_x[0] < enemy_x[0]){
      enemy_x[0] = enemy_x[0] - 1;
      oam_id = oam_meta_spr(enemy_x[0], enemy_y[0], oam_id, EnemyM2L);}
    else if(actor_y[0] > enemy_y[0]){
      enemy_y[0] = enemy_y[0] + 1;
      oam_id = oam_meta_spr(enemy_x[0], enemy_y[0], oam_id, EnemyM2D);}
    else if(actor_y[0] < enemy_y[0]){
      enemy_y[0] = enemy_y[0] - 1;
      oam_id = oam_meta_spr(enemy_x[0], enemy_y[0], oam_id, EnemyM2U);}
  }
    else
  {
    //take damage
    i=0;
  }
}
void player_action()
{           
  if (pad&PAD_A && shoutinaction != 1){setup_Shout();shoutinaction = 1;}
  if (pad&PAD_UP)
  {
    actor_dy[0]=-5;
    actor_y[i] += actor_dy[i];    
    oam_id = oam_meta_spr(actor_x[i], actor_y[i], oam_id, playerRRun0);
    lad = 1;
    if(shoutinaction != 1)lads = lad;
    
  }  
  else if (pad&PAD_DOWN)
  {
    actor_dy[0]=5;  
    actor_y[i] += actor_dy[i];
    oam_id = oam_meta_spr(actor_x[i], actor_y[i], oam_id, playerRRun1);
    lad = 2;
    if(shoutinaction != 1)lads = lad;
    
  }     
  else if(pad&PAD_LEFT)
  {
    actor_dx[0]=-5;
    actor_x[i] += actor_dx[i];
    oam_id = oam_meta_spr(actor_x[i], actor_y[i], oam_id, playerRRun2);
    lad = 3;
    if(shoutinaction != 1)lads = lad;
    
  }
  else if (pad&PAD_RIGHT)
  {
    actor_dx[0]=5;    
    actor_x[i] += actor_dx[i];
    oam_id = oam_meta_spr(actor_x[i], actor_y[i], oam_id, playerRRun3);
    lad = 4;
    if(shoutinaction != 1)lads = lad;

  }
  else
  {
    if(lad == 1)oam_id = oam_meta_spr(actor_x[i], actor_y[i], oam_id, playerRRun0);
    else if(lad == 2)oam_id = oam_meta_spr(actor_x[i], actor_y[i], oam_id, playerRRun1);
    else if(lad == 3)oam_id = oam_meta_spr(actor_x[i], actor_y[i], oam_id, playerRRun2);
    else if(lad == 4)oam_id = oam_meta_spr(actor_x[i], actor_y[i], oam_id, playerRRun3);
    else oam_id = oam_meta_spr(actor_x[i], actor_y[i], oam_id, playerRRun0);
  }
  //-------------wall collision----------
  if(actor_y[0] >= 25 &&  actor_y[0] != 185)
  {
    actor_dy[0] = - actor_dy[0];  
  }
  if(actor_y[0] <= 185 &&  actor_y[0] != 25)
  {
    actor_dy[0] = - actor_dy[0];
  }

  if (oam_id!=0) oam_hide_rest(oam_id);
}


//------------------main Game loop--------------
void game_loop()
{ 
  ShoutUsed();
  enemy_action();
  player_action();  
  ppu_wait_frame();     // ensure VRAM buffer is cleared
  
  ppu_wait_nmi();
  vrambuf_clear();
  
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
  iCount = 0;
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
        //enemy_setup();
        InGame = 2;
        break;
      case 1:   
        oam_id = 0;  
        ShoutUsed();
        game_loop();
        break;
      case 2: 
        
        show_title_screen(Level_3E_pal, Level_1E_rle);
        setup_Shout();
        //oam_id = 0;
        enemy_setup();
        //game_loop();        
        InGame = 1;
	break;
      case 3: 

        break;
      case 4:  

        break;
        
    }
       // x += dx;
        //scroll(x, y);   
    
    pad = pad_trigger(0);
    if(pad&PAD_START){InGame = 2;}
    

  }

}
