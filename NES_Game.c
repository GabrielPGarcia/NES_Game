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

  0x0f,0x30,0x2d,0x0,	// sprite palette 4
  0x00,0xc3,0x15,0x0,	// sprite palette 5
  0x00,0x00,0x2c,0x0,	// sprite palette 6
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



typedef struct Actor {
  word yy;		// Y position in pixels (16 bit)
  byte x;		// X position in pixels (8 bit)
  byte y;		// X position in pixels (8 bit)
  byte state;		// ActorState
  int onscreen:1;	// is actor onscreen?
} Actor;
typedef struct Objects {
  word yy;		// Y position in pixels (16 bit)
  byte x;		// X position in pixels (8 bit)
  byte y;		// X position in pixels (8 bit)
  byte state;		// ActorState
} Objects;

//---global variable---
Objects shot[9];  	//number of bullets
Objects lives[3];  	//number of lives 
Objects arrow[1];  	//arrow
int pLives = 3;   	//player's lives
int pPoints = 0;	//player's points
int ikills = 0;	//player's points
int shotinaction = 0;	//shout in action
Actor actors;	// all actors
Actor enemy1;	// all actors
int InGame = 0; 
int ilevel = 1;

int i;
int iCount;
char pad;
char oam_id;
byte runseq;
int lad=0;
int lads=0;
int iRand;


void actors_setup()
{
  actor_x[0] = 120;
  actor_y[0] = 35;
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
void setup_graphics() 
{
  ppu_off();
  oam_clear();
  pal_all(PALETTE);
  ppu_on_all();
}
void Lives(int l,int k)
{
  
  lives[l].x = 55+k;
  lives[l].y = 7;
  lives[l].state = 14;
  oam_id = oam_spr(lives[l].x, lives[l].y, lives[l].state, 5, oam_id);
  
}
void PlayersLives()
{
  if(pLives >= 1)
  {
    Lives(0,0);
    if(pLives >= 2)
    {
      Lives(0,9);
      if(pLives >= 3)
      {
        Lives(0,18);
      }
    }
  }
  else 
  {
    //if player has no lives left
  }
}
//prints points to top right
void points_count()
{  
  if(pPoints<=0)pPoints=0;//if points is less then 0 keep it 0
  if(pPoints >=100) oam_id = oam_spr(218, 7, (pPoints/100%10)+48, 6, oam_id);
  if(pPoints >=10) oam_id = oam_spr(224, 7, (pPoints/10%10)+48, 6, oam_id);
  oam_id = oam_spr(230, 7, (pPoints%10)+48, 6, oam_id);

}
void setup_Shot()
{
  if(lads == 1)
  {
    shot[0].x = actor_x[0]+8;
    shot[0].y = actor_y[0];
  }
  if(lads == 2)  
  {
    shot[0].x = actor_x[0]+2;
    shot[0].y = actor_y[0];
  }
  if(lads == 3)  
  {
    shot[0].x = actor_x[0];
    shot[0].y = actor_y[0];
  }
  if(lads == 4)  
  {
    shot[0].x = actor_x[0];
    shot[0].y = actor_y[0]+8;
  }
    shot[0].state = 28;
    oam_id = oam_spr(shot[0].x, shot[0].y, shot[0].state, 4, oam_id);
}
void ShotUsed()
{
  if(shotinaction == 1 && iCount <= 50)
  {
      
    if(lads == 1)
    {
      shot[0].y = shot[0].y - 2;
      shot[0].state = 28;
      oam_id = oam_spr(shot[0].x, shot[0].y, shot[0].state, 4, oam_id); 
    }
    if(lads == 2)
    {
      shot[0].y = shot[0].y + 2;
      shot[0].state = 28;
      oam_id = oam_spr(shot[0].x, shot[0].y, shot[0].state, 4, oam_id); 
    }
    if(lads == 3)
    {
      shot[0].x = shot[0].x - 2;
      shot[0].state = 29;
      oam_id = oam_spr(shot[0].x, shot[0].y, shot[0].state, 4, oam_id); 
    }
    if(lads == 4)
    {
      shot[0].x = shot[0].x + 2;
      shot[0].state = 29;
      oam_id = oam_spr(shot[0].x, shot[0].y, shot[0].state, 4, oam_id); 
    }
    iCount = iCount + 1;
  }
  else if(iCount > 0)
  {
    shotinaction = 0;
    iCount = 0;
  }
  else 
  {    
      shot[0].x = 0;
      shot[0].y = 0;
      shot[0].state = 0;
      oam_id = oam_spr(shot[0].x, shot[0].y, shot[0].state, 4, oam_id); 
  }

}
void enemy_action()
{ 

  if( enemy_x[0]!=actor_x[0] || enemy_y[0] != actor_y[0])
  {
    if((shot[0].x >= enemy_x[0]-4 && shot[0].x <= enemy_x[0]+8)&& 
       (shot[0].y >= enemy_y[0] && shot[0].y <= enemy_y[0]+12)) 
    {
      shot[i].state = 0;
      pPoints= pPoints + 1;
      ikills = ikills + 1;
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
      pLives = pLives - 1;
      if(pPoints != 0)
       pPoints= pPoints/2;
      else
        pPoints=0;
      enemy_setup();
    i=0;
  }
  //-------------wall collision----------
  if(enemy_x[0] <= 10 &&  enemy_x[0] != 216 )
  {enemy_x[0] = 10;}
  
  if(enemy_x[0] >= 230 && enemy_x[0] != 10 )
  {enemy_x[0] = 230;}
  
  if(enemy_y[0] <= 25 &&  enemy_y[0] != 191)
  {enemy_y[0] = 25;}
  
  if(enemy_y[0] >= 191 &&  enemy_y[0] != 47)
  {enemy_y[0] = 191;} 
  PlayersLives();
}
void player_action()
{        
  pad = pad_poll(i);

  //if player has shot
  if (pad&PAD_A && shotinaction != 1){setup_Shot();shotinaction = 1;}
  
  if (pad&PAD_UP )
  {
    actor_dy[0]=-2;
    actor_y[0] += actor_dy[0];    
    oam_id = oam_meta_spr(actor_x[0], actor_y[0],  oam_id, playerRRun0);
    lad = 1;
    if(shotinaction != 1)lads = lad;
    
  }  
  else if (pad&PAD_DOWN)
  {
    actor_dy[0]=2;  
    actor_y[0] += actor_dy[0];
    oam_id = oam_meta_spr(actor_x[0], actor_y[0],  oam_id, playerRRun1);
    lad = 2;
    if(shotinaction != 1)lads = lad;
    
  }     
  else if(pad&PAD_LEFT)
  {
    actor_dx[0]=-2;
    actor_x[0] += actor_dx[0];
    oam_id = oam_meta_spr(actor_x[0], actor_y[0], oam_id, playerRRun2);
    lad = 3;
    if(shotinaction != 1)lads = lad;
    
  }
  else if (pad&PAD_RIGHT)
  {
    actor_dx[0]=2;    
    actor_x[0] += actor_dx[0];
    oam_id = oam_meta_spr(actor_x[0], actor_y[0], oam_id, playerRRun3);
    lad = 4;
    if(shotinaction != 1)lads = lad;

  }
  else
  {
    if(lad == 1)oam_id = oam_meta_spr(actor_x[0], actor_y[0], oam_id, playerRRun0);
    else if(lad == 2)oam_id = oam_meta_spr(actor_x[0], actor_y[0], oam_id, playerRRun1);
    else if(lad == 3)oam_id = oam_meta_spr(actor_x[0], actor_y[0], oam_id, playerRRun2);
    else if(lad == 4)oam_id = oam_meta_spr(actor_x[0], actor_y[0], oam_id, playerRRun3);
    else oam_id = oam_meta_spr(actor_x[0], actor_y[0], oam_id, playerRRun0);
  }
    //-------------wall collision----------
  if(actor_x[0] <= 10 &&  actor_x[0] != 216 )
  {actor_x[0] = 10;}
  
  if(actor_x[0] >= 230 && actor_x[0] != 10 )
  {actor_x[0] = 230;}
  
  if(actor_y[0] <= 25 &&  actor_y[0] != 191)
  {actor_y[0] = 25;}
  
  if(actor_y[0] >= 191 &&  actor_y[0] != 47)
  {actor_y[0] = 191;} 

  if (oam_id!=0) oam_hide_rest(oam_id);
  
  points_count();
}
void nextLevel()
{
  if(ikills > 50 && ilevel == 1)
  {
      arrow[0].x = 120;
      arrow[0].y = 200;
      arrow[0].state = 11;
      oam_id = oam_spr(arrow[0].x, arrow[0].y, arrow[0].state, 5, oam_id);  if(actor_y[0] >= 190 &&  actor_y[0] != 47 &&  actor_x[0] >= 100 && actor_x[0] <=130)
      {InGame = 3;ilevel = 2;} 
    
  }  
  if(ikills > 150 && ilevel == 2)
  {
      arrow[0].x = 120;
      arrow[0].y = 200;
      arrow[0].state = 11;
      oam_id = oam_spr(arrow[0].x, arrow[0].y, arrow[0].state, 5, oam_id);  if(actor_y[0] >= 190 &&  actor_y[0] != 47 &&  actor_x[0] >= 100 && actor_x[0] <=130)
      {InGame = 4;ilevel = 3;} 
    
  }
  

}

//------------------main Game loop--------------
void game_loop()
{ 
  ShotUsed();
  enemy_action();
  player_action();  
  nextLevel();
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
void levelSetup()
{
  vram_adr(NTADR_A(1,1)); 
  vram_write("Lives:", 6);  
  vram_adr(NTADR_A(20,1)); 
  vram_write("Points:", 7);
  
}

void Start()
{
  ppu_off();
  vram_adr(NTADR_A(15,12));
  vram_write("The", 3);
  vram_adr(NTADR_A(10,13));
  vram_write("Exterminator", 12);
  
  vram_adr(NTADR_A(10,17));
  vram_write("[A] to start!", 12);
  ppu_on_all();
}

void main(void)
{ 
  int GameState = 1;
  int x = 0;   // x scroll position
  int y = 0;   // y scroll position
  int dx = 1;  // y scroll direction
  
  InGame = 0; 
  iCount = 0;
  setup_graphics();
  Level_one(Level_1E_pal, Level_1E_rle,Level_2E_rle);
  // infinite loop
  show_title_screen(Title_Screen_pal, Title_Screen_rle);        
  Start();     
  while(1) 
  {   
    switch(InGame)
    {
      case 0: 
        
         pad = pad_trigger(0);
         if(pad&PAD_START){InGame = 2;}
        //enemy_setup();
        //InGame = 2;
        break;
      case 1:   
        oam_id = 0;  
        ShotUsed();
        game_loop();
        break;
      case 2: 
        
        show_title_screen(Level_1E_pal, Level_1E_rle);
        setup_Shot();
        levelSetup();        
        actors_setup();
        enemy_setup();    
        InGame = 1;
	break;
      case 3:         
        show_title_screen(Level_1E_pal, Level_2E_rle);
        setup_Shot();
        levelSetup();
        actors_setup();
        enemy_setup();   
        InGame = 1;
        break;
      case 4:  
        show_title_screen(Level_1E_pal, Level_3E_rle);
        setup_Shot();
        levelSetup();        
        actors_setup();
        enemy_setup();     
        InGame = 1;
        break;
        
    }
       // x += dx;
        //scroll(x, y);   
    
    

  }

}
