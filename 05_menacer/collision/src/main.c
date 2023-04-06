#include <genesis.h>
#include "resources.h"


#define MAX_VALS 10

// for simplicity just hardcoding some offsets instead of including calibration routines
//
static s16 xOffset = -20;
static s16 yOffset = 0;
s16 crosshairsPosX = 152;
s16 crosshairsPosY = 104;

#define MAX_SAMPLES 4
bool useAverage = FALSE;
s16 storedXs[MAX_SAMPLES];
s16 storedYs[MAX_SAMPLES];
s16 storedIndex = 0;

static s16 xLookup[ 256 ];  // full range for positive JOY_readJoypadX() vals

static void calculateXLookup() {
  s16 pos = 0;	
  for( int i=60; i < 183; ++i ) {
    xLookup[i] =  pos;
    pos += 2;
  }
  for( int i=228; i < 256; ++i ) {
    xLookup[i] =  pos;
    pos += 2;
  }
  for( int i=0; i < 60; ++i ) {
    xLookup[i] =  pos;
    pos += 2;
  }
}



// create UFO

struct CP_SPRITE {
  Sprite *sprite;
  s16 pos_x;
  s16 pos_y;
  s16 vel_x;
  s16 vel_y;

  s16 hitbox_x1;
  s16 hitbox_y1;
  s16 hitbox_x2;
  s16 hitbox_y2;
  bool active;
};

#define MAX_UFOS 10
struct CP_SPRITE ufos[MAX_UFOS];

void createUFOs() {
  s16 ufo_pos_x = 66;
  s16 ufo_pos_y = 28;

  s16 xpos = 66;
  s16 ypos = 22;
  for( u16 i=0; i < MAX_UFOS; ++i ) {
    if( i == 5 ) {
      ypos = ufo_pos_y + 40;
      xpos = ufo_pos_x + 16;
    }
    ufos[i].pos_x = xpos;
    ufos[i].pos_y = ypos;
    ufos[i].vel_x = 0;
    ufos[i].vel_y = 0;
    ufos[i].active = TRUE;
    ufos[i].hitbox_x1 = 0;
    ufos[i].hitbox_y1 = 0;
    ufos[i].hitbox_x2 = 32;
    ufos[i].hitbox_y2 = 32;

    ufos[i].sprite  = SPR_addSprite( &ufo, xpos, ypos, TILE_ATTR( PAL2, 0, FALSE, FALSE ));
    xpos += 40;

    SPR_setAnim( ufos[i].sprite, 0 );
  }
}

void checkCollision() {
  for( u16 i=0; i < MAX_UFOS; ++i ) {
    if( ufos[i].active == TRUE ) {
      // check if current shot is in 
      if( crosshairsPosX > (ufos[i].pos_x + ufos[i].hitbox_x1)  &&
          crosshairsPosX < (ufos[i].pos_x + ufos[i].hitbox_x2)  &&
          crosshairsPosY > (ufos[i].pos_y + ufos[i].hitbox_y1)  &&
          crosshairsPosY < (ufos[i].pos_y + ufos[i].hitbox_y2)   )
      {
        ufos[i].active = FALSE; // and hide it
        SPR_setVisibility(ufos[i].sprite, HIDDEN );
      }

    }
  }

}

// joypad handler
static void joypadHandler( u16 joypadId, u16 changed, u16 state ) {
  if( joypadId == JOY_2  ) {

    // A
    if( state & BUTTON_A ) { 
      // trigger pulled
      checkCollision(); 
    }

    if( state & BUTTON_B  ){
      // average out positions
      useAverage = !useAverage;
    }

    if( state & BUTTON_C ) {
      // reset vsibiliyt of all sprites.
      for( u16 i=0; i < MAX_UFOS; ++i ) {
        ufos[i].active = TRUE;
        SPR_setVisibility(ufos[i].sprite, VISIBLE );
      }

    }
  }
}

int main(bool hard)
{
  ///////////////////////////////////////////////////////////////////////////////////
  // palette Setup
  // Set background brighter than 0.	Dark backgrounds
  // prevents menacer from returning X, Y values.
  PAL_setPalette( PAL1, crosshairs_pal.data, CPU );
  PAL_setPalette( PAL2, ufo_pal.data, CPU );
  PAL_setColor(15, 0x0000);
  VDP_setTextPalette(0);
  PAL_setColor(0, 0x0844);

  ///////////////////////////////////////////////////////////////////////////////////
  // Sprite Setup
  SPR_init();

  // crosshair is 16x16
  Sprite *crosshairsSprite = NULL;
  crosshairsSprite = SPR_addSprite( &crosshairs,  // Sprite defined in resources
      crosshairsPosX,// starting X position
      crosshairsPosY,// starting Y position
      TILE_ATTR( PAL1,           // specify palette
        1,            // Tile priority ( with background)
        FALSE,        // flip the sprite vertically?
        FALSE         // flip the sprite horizontally
        ));

  createUFOs();

  ///////////////////////////////////////////////////////////////////////////////////
  // Menacer Setup
  // create lookup table
  calculateXLookup();

  // Asynchronous joystick handler.
  JOY_setEventHandler(joypadHandler);

  // check Port 2 for the Sega Menacer
  bool menacerFound = FALSE;
  u8 portType = JOY_getPortType(PORT_2);
  if (portType == PORT_TYPE_MENACER)
  {
    JOY_setSupport(PORT_2, JOY_SUPPORT_MENACER);
    menacerFound = TRUE;
  }
  for( s16 i=0; i < MAX_SAMPLES; ++i ) {
    storedXs[i] = -1;
    storedYs[i] = -1;
  }

  ///////////////////////////////////////////////////////////////////////////////////
  // Main Loop!

  while (TRUE)
  {

    if (menacerFound)
    {
      s16 xVal = JOY_readJoypadX(JOY_2);
      s16 yVal = JOY_readJoypadY(JOY_2);
      if( useAverage == FALSE ) {
        crosshairsPosX = xLookup[xVal] + xOffset;
        crosshairsPosY = yVal + yOffset;
      } else {
        if( xVal >= 0 ) {
          // non negative.
          storedXs[storedIndex] = xLookup[ xVal ];
          storedYs[storedIndex] = yVal;
          ++storedIndex;
          if( storedIndex >= MAX_SAMPLES ) {
            storedIndex = 0;
          }
          // compute average
          s16 avgX = 0;
          s16 avgY = 0;
          s16 nonZero = 0;
          for( s16 i=0; i < MAX_SAMPLES; ++i ) {
            if( storedXs[i] >= 0 ) {
              avgX += storedXs[i];
              avgY += storedYs[i];
              ++nonZero;
            }
            if( nonZero > 0 ) {
              crosshairsPosX = avgX / nonZero + xOffset;
              crosshairsPosY = avgY / nonZero + yOffset;
            }
          }
        } else {
          for( s16 i=0; i < MAX_SAMPLES; ++i ) {
            storedXs[i] = -1;
            storedYs[i] = -1;
          }
          storedIndex = 0;

        }

      }
    }

    // Set the Sprite Positions.
    SPR_setPosition(crosshairsSprite, crosshairsPosX - 8, crosshairsPosY - 8);

    SPR_update();

    SYS_doVBlankProcess();
  }
}
