#include <genesis.h>
#include "resources.h"
#include "rotation.h"


/////////////////////////////////////////////////////////////////////
// Flash effect

static u16 palette[16];
const u16 palette_flash[15] =
{
  0x0EEE,
  0x0EEE,
  0x0EEE,
  0x0EEE,
  0x0EEE,
  0x0EEE,
  0x0EEE,
  0x0EEE,

  0x0EEE,
  0x0EEE,
  0x0EEE,
  0x0EEE,
  0x0EEE,
  0x0EEE,
  0x0EEE
};

static u8 flashScreen = 0;
static bool useFlash = FALSE;

/////////////////////////////////////////////////////////////////////
// Sounds
// define sound constants
#define SND_LASER1       64
#define SND_LASER2       65
#define SND_LASERX_4     66
#define SND_EXPLOSION    67

/////////////////////////////////////////////////////////////////////
// Scrolling Stuff
#define PLANE_MAX_TILE 64

s16 hScrollA[224];
//s16 vScrollA[20];
s16 vScrollUpperA[20];
s16 vScrollLowerA[20];

s16 vScrollB[20] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
s16 planeBDeltas[20] = {9, 9, 7, 7, 5, 5, 5, 4, 3, 2, 2, 3, 4, 5, 5, 5, 7, 7, 9, 9};

/////////////////////////////////////////////////////////////////////
//  Interrupt handlers
//
static vu16  lineDisplay   = 0;     // line position on display screen

HINTERRUPT_CALLBACK HIntHandler()
{
  if( lineDisplay == 11 ) {
    // set vertical rotation component for lwoer part of BG_A
    //memcpy( vScrollA, vScrollLowerA, sizeof(vScrollLowerA));
    VDP_setVerticalScrollTile(BG_A, 0, vScrollLowerA, 20, DMA);
  }
  // Count raster lines
  lineDisplay++;

}
void VBlankHandler()
{
  lineDisplay = 0;
}

/////////////////////////////////////////////////////////////////////
// Entities
typedef struct {
  s16 x1;
  s16 y1;
  s16 x2;
  s16 y2;
  u16 hitpoints;
} CP_HITBOX;

typedef struct {
  Sprite *sprite;
  s16 pos_x;
  s16 pos_y;
  s16 vel_x;
  s16 vel_y;

  CP_HITBOX hb;

  bool active;

  s16 frameSize; 
  s16 tileIndex; 

  u16 ticks;

} CP_SPRITE;

#define PLAYER_FRAME_COUNT  16
#define PLAYER_WIDTH        32
#define PLAYER_HEIGHT       32
#define PLAYER_HEIGHT       32
#define PLAYER_SHOT_WIDTH   8
#define PLAYER_SHOT_HEIGHT  8
#define MAX_PLAYER_SHOTS    12 

#define MAX_BOSS_SHOTS      6 

#define MAX_EXPLOSIONS      10

#define LEFT_EDGE  0
#define RIGHT_EDGE  320
#define TOP_EDGE 0
#define BOTTOM_EDGE 224

CP_SPRITE player;
s16 playerAnim = 5;
CP_SPRITE playerShots[ MAX_PLAYER_SHOTS ];

u16 totalSprites = 33;
u16 shipsheet_ind = 0;
u16 boomsheet_ind = 0;
u16 shots_ind = 0;

CP_SPRITE bossShots[MAX_BOSS_SHOTS];
CP_HITBOX boss_lgun_hb;
//CP_HITBOX boss_mgun_hb;
CP_HITBOX boss_rgun_hb;
CP_HITBOX boss_lvent_hb;
CP_HITBOX boss_rvent_hb;

s16 bossShotDeltaX[8];
s16 bossShotDeltaY[8];

CP_SPRITE explosions[MAX_EXPLOSIONS];
u16 currentExplosion = 0;

u16 bossTicks = 0;
u16 boomTicks = 0;
u16 upperTicks = 0;
u16 lowerTicks = 0;

s16 currUpperAngle = 0;
s16 upperStepDir = 1;
s16 xUpperOffset = 0;
s16 yUpperOffset = 0;
s16 yUpperOffsetDir = 1;

s16 currLowerAngle = 0;
s16 lowerStepDir = 1;
s16 xLowerOffset = 0;
s16 yLowerOffset = -10;
s16 yLowerOffsetDir = 1;


static void addExplosion( s16 pos_x, s16 pos_y ) {
  if( explosions[ currentExplosion ].active == FALSE ){
    // use it
    explosions[currentExplosion].pos_x  = pos_x;
    explosions[currentExplosion].pos_y  = pos_y;

    explosions[currentExplosion].active = TRUE;
    explosions[currentExplosion].tileIndex = 0;
    //SPR_setVisibility( explosions[currentExplosion].sprite, VISIBLE);
    //SPR_setPosition( explosions[currentExplosion].sprite,fix16ToInt(explosions[
    //currentExplosion].pos_x),fix16ToInt(explosions[currentExplosion].pos_y));

    //XGM_startPlayPCM(SND_EXPLOSION,10,SOUND_PCM_CH3);

    // point to next one
    ++currentExplosion;
    if( currentExplosion >= MAX_EXPLOSIONS ) {
      currentExplosion = 0;
    }
  }
}


static void fireBossShots() {
  bool fired = FALSE;
  for( u16 i=0; i < 3; ++i ) {
    u16 shot = 1 + i + i;
    if( boss_lgun_hb.hitpoints > 0 ) {
      bossShots[i].pos_x = lgun[currUpperAngle+currUpperAngle]-4 + xUpperOffset;
      bossShots[i].pos_y = lgun[currUpperAngle+currUpperAngle+1]-4 - yUpperOffset;
      bossShots[i].active = TRUE;
      bossShots[i].vel_x = bossShotDeltaX[shot];
      bossShots[i].vel_y = bossShotDeltaY[shot];
      //SPR_setVisibility( bossShots[i].sprite, VISIBLE);
      fired = TRUE;
    }

    if( boss_rgun_hb.hitpoints > 0 ) {
      bossShots[i+3].pos_x = rgun[currUpperAngle+currUpperAngle]-4 +xUpperOffset;
      bossShots[i+3].pos_y = rgun[currUpperAngle+currUpperAngle+1]-4 - yUpperOffset;
      bossShots[i+3].active = TRUE;
      bossShots[i+3].vel_x = bossShotDeltaX[shot];
      bossShots[i+3].vel_y = bossShotDeltaY[shot];
      //SPR_setVisibility( bossShots[i+3].sprite, VISIBLE);
      fired = TRUE;
    }
  }

/*
  if( boss_mgun_hb.hitpoints > 0 ) {
    for( u16 i=0; i < 4; ++i ) {
      u16 shot =  i + i;
      bossShots[i+6].pos_x = mgun[currUpperAngle+currUpperAngle]-4 +xUpperOffset;
      bossShots[i+6].pos_y = mgun[currUpperAngle+currUpperAngle+1]-4 - yUpperOffset;
      bossShots[i+6].active = TRUE;
      bossShots[i+6].vel_x = bossShotDeltaX[shot];
      bossShots[i+6].vel_y = bossShotDeltaY[shot];
      //SPR_setVisibility( bossShots[i+6].sprite, VISIBLE);
      fired = TRUE;
    }
  }
*/

  if( fired ) {
    //XGM_startPlayPCM(SND_LASERX_4,10,SOUND_PCM_CH4);
  }
}






static void myJoyHandler( u16 joy, u16 changed, u16 state)
{
  if (joy == JOY_1)
  {
    if (state & BUTTON_A) {
      //XGM_startPlayPCM(SND_LASER1,1,SOUND_PCM_CH2);
      u16 addedShot = 0;
      for( u16 i=0; i < MAX_PLAYER_SHOTS; ++i ) {
        if( playerShots[i].active == FALSE ) {
          playerShots[i].active = TRUE;
          // set its starting position
          playerShots[i].pos_x = player.pos_x+ 12;
          playerShots[i].pos_y = player.pos_y;
          switch( addedShot ) {
            case 0:
              playerShots[i].vel_x =  0;
              playerShots[i].vel_y = -6;
              break;

            case 1:
              playerShots[i].vel_x = -2;
              playerShots[i].vel_y = -5;
              break;

            case 2:
              playerShots[i].vel_x = 2;
              playerShots[i].vel_y = -5;
              break;
          }
          ++addedShot;
          if( addedShot >= 3 ) {
            break;
          }
        }
      }

    }

    //Set player velocity if left or right are pressed;
    //set velocity to 0 if no direction is pressed
    if (state & BUTTON_RIGHT)
    {
      player.vel_x = 2;
    }
    else if (state & BUTTON_LEFT)
    {
      player.vel_x = -2;
    } else{
      if( (changed & BUTTON_RIGHT) | (changed & BUTTON_LEFT) ){
        player.vel_x = 0;
      }
    }

    if (state & BUTTON_UP)
    {
      player.vel_y = -2;
    }
    else if (state & BUTTON_DOWN)
    {
      player.vel_y = 2;
    } else{
      if( (changed & BUTTON_UP) | (changed & BUTTON_DOWN) ){
        player.vel_y = 0;
      }
    }
  }
}


static void update() {
  // PLAYER ////////////////////////////////////////////////
  //Check horizontal bounds
  if(player.pos_x < LEFT_EDGE){
    player.pos_x = LEFT_EDGE;
    player.vel_x = 0;
  } else if(player.pos_x + (player.hb.x2 - player.hb.x1 ) > RIGHT_EDGE){
    player.pos_x = RIGHT_EDGE - (player.hb.x2 - player.hb.x1) ;
    player.vel_x = 0;
  }

  if( player.vel_x == 0 ) {
    if ( playerAnim > 5 ) {
      --playerAnim;
    } else if ( playerAnim < 5 ) {
      ++playerAnim;
    }
  } else {
    if( player.vel_x > 0 ) {
      ++playerAnim;
      if( playerAnim > 10 ) {
        playerAnim = 10;
      }
    } else {
      --playerAnim;
      if( playerAnim < 0  ) {
        playerAnim = 0;
      }
    }
  }
  //SPR_setAnim( player.sprite, playerAnim );

  //Check vertical bounds
  if(player.pos_y < TOP_EDGE){
    player.pos_y = TOP_EDGE;
    player.vel_y = 0;
  } else if(player.pos_y + (player.hb.y2 - player.hb.y1 ) > BOTTOM_EDGE){
    player.pos_y = BOTTOM_EDGE - ( player.hb.y2 - player.hb.y1 );
    player.vel_y = 0;
  }

  //Position the ship
  player.pos_x += player.vel_x;
  player.pos_y += player.vel_y;

  //SPR_setPosition( player.sprite, fix16ToInt(player.pos_x), fix16ToInt(player.pos_y) );
  // PLAYER shots///////////////////////////////////////////
  for( u16 i=0; i < MAX_PLAYER_SHOTS; ++i ) {
    if( playerShots[i].active == TRUE ) {
      playerShots[i].pos_x +=  playerShots[i].vel_x;
      playerShots[i].pos_y +=  playerShots[i].vel_y;
      if(playerShots[i].pos_y  < 0 ) {
        //SPR_setVisibility( playerShots[i].sprite, HIDDEN);
        playerShots[i].active = FALSE;
      } else {
        //SPR_setVisibility( playerShots[i].sprite, VISIBLE);
        //SPR_setPosition( playerShots[i].sprite,fix16ToInt(playerShots[i].pos_x),fix16ToInt(playerShots[i].pos_y));
      }
    }
  }

  // BOSS shots/////////////////////////////////////////////
  for( u16 i=0; i < MAX_BOSS_SHOTS; ++i ) {
    if( bossShots[i].active == TRUE ) {
      bossShots[i].pos_x +=  bossShots[i].vel_x;
      bossShots[i].pos_y +=  bossShots[i].vel_y;
      if( (bossShots[i].pos_y  > 224 )  ||
          (bossShots[i].pos_y  < 0 )  ||
          (bossShots[i].pos_x  > 320 )  ||
          (bossShots[i].pos_x  < 0 ) ) {
        bossShots[i].active = FALSE;
        //SPR_setVisibility( bossShots[i].sprite, HIDDEN);
      } else {
        //SPR_setVisibility( bossShots[i].sprite, VISIBLE);
        //SPR_setPosition( bossShots[i].sprite,fix16ToInt(bossShots[i].pos_x),fix16ToInt(bossShots[i].pos_y));
      }
    }
  }

  ++bossTicks;
  if( bossTicks == 255 )  {
    bossTicks = 0;
    fireBossShots();
  }

  // Explosions /////////////////////////////////////////////
  for( u16 i=0; i < MAX_EXPLOSIONS; ++i ) {
    if( explosions[i].active == TRUE ) {
      explosions[i].tileIndex += explosions[i].frameSize;
      if( explosions[i].tileIndex < 6 ) {
        //SPR_setFrame( explosions[i].sprite, explosions[i].tileIndex );
      }
      else {
        explosions[i].active = FALSE;
        //SPR_setVisibility( explosions[i].sprite, HIDDEN);
      }
    }

  }

}

static void checkCollisions() {

  for( u16 i=0; i < MAX_BOSS_SHOTS; ++i ) {
    if( bossShots[i].active == TRUE ) {
      if( (bossShots[i].pos_x + 4 ) < (player.pos_x + player.hb.x2) &&
          (bossShots[i].pos_x + 4 ) > (player.pos_x + player.hb.x1) &&
          (bossShots[i].pos_y + 4 ) < (player.pos_y + player.hb.y2) &&
          (bossShots[i].pos_y + 4 ) > (player.pos_y + player.hb.y1)  )
      {
        bossShots[i].active = FALSE;
        //SPR_setVisibility( bossShots[i].sprite, HIDDEN);
        addExplosion(  bossShots[i].pos_x - 12, bossShots[i].pos_y - 12 );
      }
    }
  }
  // player shots vs boss
  for( u16 j=0; j < MAX_PLAYER_SHOTS; ++j ) {
    if( boss_lgun_hb.hitpoints > 0 &&
        playerShots[j].active == TRUE &&
        boss_lgun_hb.x1 < (playerShots[j].pos_x + 4) &&
        boss_lgun_hb.x2 > (playerShots[j].pos_x + 4) &&
        boss_lgun_hb.y1 < (playerShots[j].pos_y + 4) &&
        boss_lgun_hb.y2 > (playerShots[j].pos_y + 4)  )
    {
      --boss_lgun_hb.hitpoints;
      playerShots[j].active = FALSE;
      //SPR_setVisibility( playerShots[j].sprite, HIDDEN);
      addExplosion( playerShots[j].pos_x - 16, playerShots[j].pos_y - 16);
      if( flashScreen < 1 ) {
        flashScreen = 3;
      }
    }
    if( boss_rgun_hb.hitpoints > 0 &&
        playerShots[j].active == TRUE &&
        boss_rgun_hb.x1 < (playerShots[j].pos_x + 4) &&
        boss_rgun_hb.x2 > (playerShots[j].pos_x + 4) &&
        boss_rgun_hb.y1 < (playerShots[j].pos_y + 4) &&
        boss_rgun_hb.y2 > (playerShots[j].pos_y + 4)  )
    {
      --boss_rgun_hb.hitpoints;
      playerShots[j].active = FALSE;
      //SPR_setVisibility( playerShots[j].sprite, HIDDEN);
      addExplosion( playerShots[j].pos_x - 16, playerShots[j].pos_y - 16);
      if( flashScreen < 1 ) {
        flashScreen = 3;
      }
    }

/*
    if( boss_mgun_hb.hitpoints > 0 &&
        playerShots[j].active == TRUE &&
        boss_mgun_hb.x1 < (playerShots[j].pos_x + 4) &&
        boss_mgun_hb.x2 > (playerShots[j].pos_x + 4) &&
        boss_mgun_hb.y1 < (playerShots[j].pos_y + 4) &&
        boss_mgun_hb.y2 > (playerShots[j].pos_y + 4)  )
    {
      --boss_mgun_hb.hitpoints;
      playerShots[j].active = FALSE;
      //SPR_setVisibility( playerShots[j].sprite, HIDDEN);
      addExplosion( playerShots[j].pos_x - 16, playerShots[j].pos_y -16);
      if( flashScreen < 1 ) {
        flashScreen = 3;
      }
    }
*/

    if( boss_lvent_hb.hitpoints > 0 &&
        playerShots[j].active == TRUE &&
        boss_lvent_hb.x1 < (playerShots[j].pos_x + 4) &&
        boss_lvent_hb.x2 > (playerShots[j].pos_x + 4) &&
        boss_lvent_hb.y1 < (playerShots[j].pos_y + 4) &&
        boss_lvent_hb.y2 > (playerShots[j].pos_y + 4)  )
    {
      --boss_lvent_hb.hitpoints;
      playerShots[j].active = FALSE;
      //SPR_setVisibility( playerShots[j].sprite, HIDDEN);
      addExplosion( playerShots[j].pos_x - 16, playerShots[j].pos_y - 16);
      if( flashScreen < 1 ) {
        flashScreen = 3;
      }
    }
    if( boss_rvent_hb.hitpoints > 0 &&
        playerShots[j].active == TRUE &&
        boss_rvent_hb.x1 < (playerShots[j].pos_x + 4) &&
        boss_rvent_hb.x2 > (playerShots[j].pos_x + 4) &&
        boss_rvent_hb.y1 < (playerShots[j].pos_y + 4) &&
        boss_rvent_hb.y2 > (playerShots[j].pos_y + 4)  )
    {
      --boss_rvent_hb.hitpoints;
      playerShots[j].active = FALSE;
      //SPR_setVisibility( playerShots[j].sprite, HIDDEN);
      addExplosion(  playerShots[j].pos_x - 16, playerShots[j].pos_y - 16);
      if( flashScreen < 1 ) {
        flashScreen = 3;
      }
    }

    ++boomTicks;
    if( boss_lgun_hb.hitpoints <=0 && boomTicks == 16 ) {
      addExplosion(  lgun[currUpperAngle + currUpperAngle]-16 + xUpperOffset, lgun[currUpperAngle + currUpperAngle + 1]-16 - yUpperOffset );
    }
    if( boss_rgun_hb.hitpoints <=0  && boomTicks == 16 ) {
      addExplosion(  rgun[currUpperAngle + currUpperAngle]-16 + xUpperOffset,  rgun[currUpperAngle + currUpperAngle + 1]-16 - yUpperOffset );
    }

    if( boss_lvent_hb.hitpoints <=0 && boomTicks == 8 ) {
      addExplosion(  lvent[currUpperAngle + currUpperAngle]-16 + xUpperOffset,  lvent[currUpperAngle + currUpperAngle + 1]-16 - yUpperOffset );
    }
    if( boss_rvent_hb.hitpoints <=0  && boomTicks == 8 ) {
      addExplosion( rvent[currUpperAngle + currUpperAngle]-16 + xUpperOffset,  rvent[currUpperAngle + currUpperAngle + 1]-16 - yUpperOffset );
    }

  }
  //
}


static void setupPlayer() {
  player.pos_x = 144;
  player.pos_y = 160;
  player.vel_x = 0;
  player.vel_y = 0;
  player.hb.x1 = 2;
  player.hb.y1 = 12;
  player.hb.x2 = 30;
  player.hb.y2 = 26;
  player.active = TRUE;
  player.frameSize = 16;
  player.tileIndex = 32;

  VDP_setSpriteFull(0, // sprite ID ( 0 to 79 )
      player.pos_x,   // X in screen coords
      player.pos_y,   // Y in screen coords
      SPRITE_SIZE(4,4), // 1x1 to up to 4x4
      TILE_ATTR_FULL(PAL2,    // PALette
        1,  // priority
        0,  // Flip Vertical
        0,  // Flip Horizontal
        shipsheet_ind + player.tileIndex  // index
        ) ,
      1
      );
  totalSprites = 1;
}


static void setupExplosions() {
  // offscreen
  s16 xpos =  -32;
  s16 ypos =  264;

  for( u16 i=0; i < MAX_EXPLOSIONS; ++i ) {
    explosions[i].pos_x = xpos;
    explosions[i].pos_y = ypos;
    explosions[i].vel_x = 0;
    explosions[i].vel_y = 0;
    explosions[i].active = FALSE;
    explosions[i].hb.x1 = 0;
    explosions[i].hb.y1 = 0;
    explosions[i].hb.x2 = 0;
    explosions[i].hb.y2 = 0;
    explosions[i].frameSize = 16;
    explosions[i].tileIndex = 0;
  }

  for( u16 i=0; i< MAX_EXPLOSIONS; ++i ) {
    VDP_setSpriteFull(totalSprites, // sprite ID ( 0 to 79 )
        24 * i,   // X in screen coords
        20 + (i<8? 0:32) ,   // Y in screen coords
        SPRITE_SIZE(4,4), // 1x1 to up to 4x4
        TILE_ATTR_FULL(PAL3,    // PALette
          1,  // priority
          0,  // Flip Vertical
          0,  // Flip Horizontal
          boomsheet_ind + 16  // index
          ) ,
        totalSprites +1 
        );
    totalSprites++;
  }
}

static void setupBossShots() {
  s16 xpos = -16;
  s16 ypos = 230;

  for( u16 i=0; i < MAX_BOSS_SHOTS; ++i ) {
    bossShots[i].pos_x = xpos;
    bossShots[i].pos_y = ypos;
    bossShots[i].vel_x = 0;
    bossShots[i].vel_y = 0;
    bossShots[i].active = FALSE;
    bossShots[i].hb.x1 = 0;
    bossShots[i].hb.y1 = 0;
    bossShots[i].hb.x2 = 8;
    bossShots[i].hb.y2 = 8;
    bossShots[i].frameSize = 1;
    bossShots[i].tileIndex = 0;
  }

  for( u16 i=0; i< MAX_BOSS_SHOTS; ++i ) {
    VDP_setSpriteFull(totalSprites, // sprite ID ( 0 to 79 )
        8 * i,   // X in screen coords
        10,   // Y in screen coords
        SPRITE_SIZE(1,1), // 1x1 to up to 4x4
        TILE_ATTR_FULL(PAL3,    // PALette
          1,  // priority
          0,  // Flip Vertical
          0,  // Flip Horizontal
          shots_ind + bossShots[i].tileIndex// index
          ),
        totalSprites +1 
        );
    totalSprites++;
  }
  
}

static void setupPlayerShots() {
  s16 xpos =  -16; // offline
  s16 ypos =  230;

  for( u16 i=0; i < MAX_PLAYER_SHOTS; ++i ) {
    playerShots[i].pos_x = xpos;
    playerShots[i].pos_y = ypos;
    playerShots[i].vel_x = 0;
    playerShots[i].vel_y = 0;
    playerShots[i].active = FALSE;
    playerShots[i].hb.x1 = 0;
    playerShots[i].hb.y1 = 0;
    playerShots[i].hb.x2 = 8;
    playerShots[i].hb.y2 = 8;
    playerShots[i].frameSize = 1;
    playerShots[i].tileIndex = 2;

  }

  for( u16 i=0; i< MAX_PLAYER_SHOTS; ++i ) {
    VDP_setSpriteFull(totalSprites, // sprite ID ( 0 to 79 )
        8 * i,   // X in screen coords
        160,  // Y in screen coords
        SPRITE_SIZE(1,1), // 1x1 to up to 4x4
        TILE_ATTR_FULL(PAL3,    // PALette
          1,  // priority
          0,  // Flip Vertical
          0,  // Flip Horizontal
          shots_ind + ( i%2==0 ? 1:2) // index
          ),
        totalSprites +1 
        );
    totalSprites++;
  }
}

void createSprites() {

  setupPlayer();

  setupExplosions();
  setupBossShots();
  setupPlayerShots();

  /*
  for( u16 i=35; i< 40; ++i ) {
    VDP_setSpriteFull(i, // sprite ID ( 0 to 79 )
        8 * (i-30),   // X in screen coords
        180,  // Y in screen coords
        SPRITE_SIZE(1,1), // 1x1 to up to 4x4
        TILE_ATTR_FULL(PAL3,    // PALette
          1,  // priority
          0,  // Flip Vertical
          0,  // Flip Horizontal
          shots_ind + 3 // index
          ),
        i+1 
        );
  }
        */



  VDP_updateSprites(totalSprites, DMA_QUEUE_COPY);
  SYS_doVBlankProcess();
}


int main(bool hard)
{

  // setup shots
  // head shot
  bossShotDeltaX[0] =  2;
  bossShotDeltaY[0] =  2;
  bossShotDeltaX[1] =  0;
  bossShotDeltaY[1] =  3;
  bossShotDeltaX[2] =  -2;
  bossShotDeltaY[2] =  2;

  bossShotDeltaX[3] =  0.223929;
  bossShotDeltaY[3] =  1.987424;


  // Setup Sound
  XGM_setPCM(  SND_LASER1, laser1, sizeof(laser1));
  XGM_setPCM(  SND_LASER2, laser2, sizeof(laser2));
  XGM_setPCM(  SND_LASERX_4, laserx_4, sizeof(laserx_4));
  XGM_setPCM(  SND_EXPLOSION, explosion6, sizeof(explosion6));


  VDP_setScreenWidth320();
  // set palettes
  PAL_setPalette( PAL0, planea_pal.data, CPU );
  PAL_setPalette( PAL1, planeb_pal.data, CPU );
  PAL_setPalette( PAL2, shipsheet_pal.data, CPU );
  PAL_setPalette( PAL3, shots_pal.data, CPU );

  memcpy(&palette[0], planea_pal.data, 32 );

  // set scrolling mode to LINE for horizontal and TILE for vertical
  VDP_setScrollingMode(HSCROLL_LINE, VSCROLL_2TILE);

  // Setup background 
  // get tile positions in VRAM.
  int ind = TILE_USER_INDEX;
  // Load the plane  into VRAM
  int indexA = ind;
  VDP_loadTileSet(planea.tileset, ind, CPU);
  int indexB = ind + planea.tileset->numTile; // AND get next position in VRAM ;
  VDP_loadTileSet(planeb.tileset, indexB, CPU);

  // Simple image for BG_B, so just draw it.
  VDP_drawImageEx(BG_B, &planeb, TILE_ATTR_FULL(PAL1, FALSE, FALSE, FALSE, indexB), 0, 0, FALSE, TRUE);
  // setup the tiles
  VDP_setTileMapEx(BG_A, planea.tilemap, TILE_ATTR_FULL(PAL0, FALSE, FALSE, FALSE, indexA),
      0,               // Plane X destination
      0,               // plane Y destination
      0,               // Region X start position
      0,               // Region Y start position
      PLANE_MAX_TILE, // width  (went with 64 becasue default width is 64.  Viewable screen is 40)
      28,             // height
      CPU);
  
  // Load sprite tilesest
  shipsheet_ind = indexB + planeb.tileset->numTile; 
  VDP_loadTileData( shipsheet_tileset.tiles, // tile data pointer
      shipsheet_ind,    // index
      80,               // number of tiles to load
      DMA_QUEUE         // transfer method
      );
  boomsheet_ind = shipsheet_ind + shipsheet_tileset.numTile; 
  VDP_loadTileData( boomsheet_tileset.tiles, // tile data pointer
      boomsheet_ind,    // index
      112,        // number of tiles to load
      DMA_QUEUE         // transfer method
      );
  shots_ind = boomsheet_ind + boomsheet_tileset.numTile; // AND get next position in VRAM ;
  VDP_loadTileData( shots_tileset.tiles, // tile data pointer
      shots_ind,    // index
      4,        // number of tiles to load
      DMA_QUEUE         // transfer method
      );




  // Setup interrupt handlers
  SYS_disableInts();
  {
    SYS_setVBlankCallback(VBlankHandler);
    SYS_setHIntCallback(HIntHandler);
    VDP_setHIntCounter(10);
    VDP_setHInterrupt(1);
  }
  SYS_enableInts();


  createSprites();
  
  // setup joystick
  JOY_init();
  JOY_setEventHandler( &myJoyHandler );

  boss_lgun_hb.hitpoints = 150;
  //boss_mgun_hb.hitpoints = 150;
  boss_rgun_hb.hitpoints = 150;
  boss_lvent_hb.hitpoints = 100;
  boss_rvent_hb.hitpoints = 100;

  while (TRUE)
  {
    // flashscreen

    if( flashScreen > 0 ) {
      if( flashScreen == 3 ) {
        PAL_setColors(1, palette_flash, 15, CPU); //using 1 to not mess with background color.
      } else if( flashScreen == 1 ) {
        PAL_setColors(1, palette +1, 15, CPU); // restore
      }
      --flashScreen;
    }


    // rotation 
    ++upperTicks;

    if( upperTicks == 4 || upperTicks == 2 ) {
        yUpperOffset += yUpperOffsetDir;
        if( yUpperOffset > 0) {
          yUpperOffsetDir = -1;
        }else if( yUpperOffset < -15 ) {
          yUpperOffsetDir = 1;
        }
        if( currUpperAngle < 4) {
          xUpperOffset+=2;
        } else if ( currUpperAngle > 6) {
          xUpperOffset-=2;
        }
    }
    if( upperTicks == 6 ) {
      upperTicks = 0; 
      currUpperAngle += upperStepDir;
      if( currUpperAngle >= lower_SCROLL_COUNT ) {
        upperStepDir = -1;
        currUpperAngle = 9;
      }else if (currUpperAngle <0 ) {
        upperStepDir = 1;
        currUpperAngle = 1;
      }
    }

    ++lowerTicks;
    if( lowerTicks == 8 ) {
      currLowerAngle += lowerStepDir;
      if( currLowerAngle >= upper_SCROLL_COUNT ) {
        lowerStepDir = -1;
        currLowerAngle = 9;
      }else if (currLowerAngle < 0 ) {
        lowerStepDir = 1;
        currLowerAngle = 1;
      }
    }

    if( lowerTicks == 12 ) {
      lowerTicks = 0;
      yLowerOffset += yLowerOffsetDir;
      if( yLowerOffset > -12) {
        yLowerOffsetDir = -1;
      }else if( yLowerOffset < -20 ) {
        yLowerOffsetDir = 1;
      }
      if( currLowerAngle < 4) {
        xLowerOffset+=1;
      } else if ( currLowerAngle > 6) {
        xLowerOffset-=1;
      }
    }


    s16 startUpperHScroll = upper_START_ROW_A - yUpperOffset;
    s16 stopUpperRows = upper_END_ROW_A - yUpperOffset;
    if( startUpperHScroll < 0 ) {
      stopUpperRows = upper_START_ROW_A + upper_ROWS_A + startUpperHScroll;
      startUpperHScroll = 0;
    }
    s16 tempAngle = currUpperAngle * upper_ROWS_A;
    for(s16 i=startUpperHScroll, offset=0; i < stopUpperRows; ++i, ++offset ) {
      hScrollA[ i ] = upper_hScroll[ tempAngle + offset] + xUpperOffset;
    }


    s16 startLowerHScroll = lower_START_ROW_A - yLowerOffset;
    s16 stopLowerHScroll = lower_END_ROW_A - yLowerOffset;
    if( stopLowerHScroll > 223 ) {
      stopLowerHScroll =223;
    }
    tempAngle = currLowerAngle * lower_ROWS_A;
    for(s16 i=startLowerHScroll, offset=0; i <= stopLowerHScroll; ++i, ++offset ) {
      hScrollA[ i ] = lower_hScroll[ tempAngle + offset] + xLowerOffset;
    }


    //
    tempAngle = currLowerAngle * lower_COLS_A;
    s16 tempAngle2 = currUpperAngle * upper_COLS_A;
    for (s16 i = 0; i < upper_COLS_A; ++i)
    {
      vScrollLowerA[i] = lower_vScroll[tempAngle + i] + yLowerOffset;
      vScrollUpperA[i] = upper_vScroll[tempAngle2 + i] + yUpperOffset;
    }



    // scroll the asteroids in BG_B
    vScrollB[0] -= planeBDeltas[0];
    vScrollB[1] -= planeBDeltas[1];
    vScrollB[2] -= planeBDeltas[2];
    vScrollB[3] -= planeBDeltas[3];
    vScrollB[4] -= planeBDeltas[4];
    vScrollB[5] -= planeBDeltas[5];
    vScrollB[6] -= planeBDeltas[6];
    vScrollB[7] -= planeBDeltas[7];
    vScrollB[8] -= planeBDeltas[8];
    vScrollB[9] -= planeBDeltas[9];
    vScrollB[10] -= planeBDeltas[10];
    vScrollB[11] -= planeBDeltas[11];
    vScrollB[12] -= planeBDeltas[12];
    vScrollB[13] -= planeBDeltas[13];
    vScrollB[14] -= planeBDeltas[14];
    vScrollB[15] -= planeBDeltas[15];
    vScrollB[16] -= planeBDeltas[16];
    vScrollB[17] -= planeBDeltas[17];
    vScrollB[18] -= planeBDeltas[18];
    vScrollB[19] -= planeBDeltas[19];

    boss_lgun_hb.x1 = lgun[currUpperAngle + currUpperAngle]-16 + xUpperOffset;
    boss_lgun_hb.y1 = lgun[currUpperAngle + currUpperAngle + 1]-16 - yUpperOffset;
    boss_lgun_hb.x2 = boss_lgun_hb.x1 + 32;
    boss_lgun_hb.y2 = boss_lgun_hb.y1 + 32;

    //    boss_mgun_hb.x1 = mgun[currUpperAngle + currUpperAngle]-16 + xUpperOffset;
    //    boss_mgun_hb.y1 = mgun[currUpperAngle + currUpperAngle + 1]-16 - yUpperOffset;
    //    boss_mgun_hb.x2 = boss_mgun_hb.x1 + 32;
    //    boss_mgun_hb.y2 = boss_mgun_hb.y1 + 32;

    boss_rgun_hb.x1 = rgun[currUpperAngle + currUpperAngle]-16 + xUpperOffset;
    boss_rgun_hb.y1 = rgun[currUpperAngle + currUpperAngle + 1]-16 - yUpperOffset;
    boss_rgun_hb.x2 = boss_rgun_hb.x1 + 32;
    boss_rgun_hb.y2 = boss_rgun_hb.y1 + 32;

    boss_lvent_hb.x1 = lvent[currUpperAngle + currUpperAngle]-16 + xUpperOffset;
    boss_lvent_hb.y1 = lvent[currUpperAngle + currUpperAngle + 1]-16 - yUpperOffset;
    boss_lvent_hb.x2 = boss_lvent_hb.x1 + 32;
    boss_lvent_hb.y2 = boss_lvent_hb.y1 + 32;

    boss_rvent_hb.x1 = rvent[currUpperAngle + currUpperAngle]-16 + xUpperOffset;
    boss_rvent_hb.y1 = rvent[currUpperAngle + currUpperAngle + 1]-16 - yUpperOffset;
    boss_rvent_hb.x2 = boss_rvent_hb.x1 + 32;
    boss_rvent_hb.y2 = boss_rvent_hb.y1 + 32;

    update();
    checkCollisions();

    // queu it up.
    SYS_disableInts();
    {
      VDP_setVerticalScrollTile(BG_A, 0, vScrollUpperA, 20, DMA_QUEUE);
      VDP_setHorizontalScrollLine(BG_A, 0, hScrollA, 224, DMA_QUEUE);
      //if( ticks % 2 == 0 ) {
      VDP_setVerticalScrollTile(BG_B, 0, vScrollB, 20, DMA_QUEUE); // use array to set plane offsets
                                                                   //} else {
                                                                   //}

      VDP_setSpritePosition(0, // sprite ID ( 0 to 79 )
          player.pos_x,   // X in screen coords
          player.pos_y   // Y in screen coords
          );
      VDP_updateSprites(totalSprites, DMA_QUEUE);
    }
    SYS_enableInts();
    SYS_doVBlankProcess();
  }
  return 0;
}
