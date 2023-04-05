#include <genesis.h>
#include "resources.h"


static u16 palette[32];
const u16 palette_flash[32] =
{
  0x0888, 0x0888, 0x0888, 0x0888, 0x0888, 0x0888, 0x0888, 0x0888,

  0x0888, 0x0888, 0x0888, 0x0888, 0x0888, 0x0888, 0x0888, 0x0888,

  0x0888, 0x0888, 0x0888, 0x0888, 0x0888, 0x0888, 0x0888, 0x0888,

  0x0888, 0x0888, 0x0888, 0x0888, 0x0888, 0x0888, 0x0888, 0x0888
};
static u8 flashScreen = 0;
static bool useFlash = FALSE;



u16 crosshairsMode = 0;  // 0 raw values, 1 with lookup,  2 with lookup and calibration offset
static bool calibrateMode = FALSE;
#define MAX_VALS 10
static s16 xValues[MAX_VALS];
static s16 yValues[MAX_VALS];
static u16 currentValue = 0;


static s16 xOffset = 0;
static s16 yOffset = 0;

static s16 xLookup[ 256 ];  // full range for JOY_readJoypadX()


static void calculateOffset() {
  s16 xTemp = 0;
  s16 yTemp = 0;
  // get average X and Y 
  for( int i=0; i < currentValue; ++i ) {
    xTemp = xTemp + xValues[i];
    yTemp = yTemp + yValues[i];
  }
  xTemp = xTemp / currentValue;
  yTemp = yTemp / currentValue;

  // center should be 160, 112 
  xOffset = 160 - xTemp;		
  yOffset = 112 - yTemp;		

}


static void joypadHandler( u16 joypadId, u16 changed, u16 joypadState ) {

  // standard controller handles modes
  if( joypadId == JOY_1  ) {
    if( joypadState & BUTTON_B ) {
      currentValue = 0;
      calibrateMode = TRUE;
    } else if( joypadState & BUTTON_C ) {

      ++crosshairsMode;
      if( crosshairsMode > 2 ) {
        crosshairsMode = 0;
      }

    } else if( joypadState & BUTTON_Z ) {
      // Z - Toggle screen flash
      useFlash = !useFlash;
    }

  }else if( joypadId == JOY_2  ) {

    // A
    if( joypadState & BUTTON_A) {
      flashScreen = 3;
      if( calibrateMode ) {
        // get reading
        s16 xVal = JOY_readJoypadX(JOY_2);
        s16 yVal = JOY_readJoypadY(JOY_2);
        // store values for calculation
        if( currentValue < MAX_VALS ) {
          xValues[currentValue] = xLookup[xVal];
          yValues[currentValue] = yVal;
          ++currentValue;
        }
        if( currentValue == MAX_VALS ){
          calculateOffset();
          currentValue = 0;
          calibrateMode = FALSE;
        }
      }
    }

  }
}


static void calculateXLookup() {
  // My blue justifiers return approximately 
  //  * 31 through 180 ( 149 values ) when I pan from left to right
  //  * 33 through 176 ( 143 values ) when I pan from left to right
  // 
  //  Kega fusion
  //  35 through 182, 228 to 235 from left to right
  //      ( 182-35) + (235- 228)  = 154 
  s16 pos = 0;
  for( int i=20; i < 183; ++i ) {
    xLookup[i] = pos;
    pos += 2;
  }
  for( int i=229; i < 255; ++i ) {
    xLookup[i] = pos;
    pos += 2;
  }

}



int main(bool hard)
{
  ///////////////////////////////////////////////////////////////////////////////////
  // Sprite Setup
  Sprite *targetSprite = NULL;
  PAL_setPalette( PAL1, tgt_palette.data, CPU);

  SPR_init();

  // crosshair is 16x16
  Sprite *crosshairsSprite = NULL;
  s16 crosshairsPosX = 152.0;
  s16 crosshairsPosY = 104.0;
  crosshairsSprite = SPR_addSprite( &crosshairs,  // Sprite defined in resources
      crosshairsPosX,// starting X position
      crosshairsPosY,// starting Y position
      TILE_ATTR( PAL1,           // specify palette
        1,            // Tile priority ( with background)
        FALSE,        // flip the sprite vertically?
        FALSE         // flip the sprite horizontally
        ));

  // set target in the center position (target is 32x32 ) 
  s16 targetPosX = 144; // 160 - 16 = 144
  s16 targetPosY = 96;  // 112 - 16 = 96

  targetSprite = SPR_addSprite( &target,              // Sprite defined in resource.res
      targetPosX, // starting X position
      targetPosY, // starting Y position
      TILE_ATTR( PAL1,        // specify palette
        1,          // Tile priority ( with background)
        FALSE,      // flip the sprite vertically?
        FALSE       // flip the sprite horizontally
        ));

  ///////////////////////////////////////////////////////////////////////////////////
  // Color setup
  // Set background brighter than 0.	darker backgrounds
  // prevent Justifier from returning X, Y values.
  PAL_setColor(15, 0x0000);
  VDP_setTextPalette(0);
  PAL_setColor(0, RGB24_TO_VDPCOLOR(0x4488ff)); // seems to work OK
  PAL_getColors( 0, &palette[0], 16);
  memcpy(&palette[16], tgt_palette.data, 16 * 2);


  ///////////////////////////////////////////////////////////////////////////////////
  // BG setup
  s16 indexB = TILE_USER_INDEX;
  VDP_loadTileSet(bgrect.tileset, indexB, CPU);
  VDP_drawImageEx(BG_B, &bgrect, TILE_ATTR_FULL(PAL1, FALSE, FALSE, FALSE, indexB), 0, 0, FALSE, TRUE);


  ///////////////////////////////////////////////////////////////////////////////////
  // Justifier Setup
  //
  calculateXLookup();

  // Asynchronous joystick handler. 
  JOY_setEventHandler (joypadHandler );


  // check Port 2 for the Konami Justifier
  bool justifierFound = FALSE;
  u8 portType = JOY_getPortType(PORT_2);
  if(portType == PORT_TYPE_JUSTIFIER )
  {
    JOY_setSupport(PORT_2, JOY_SUPPORT_JUSTIFIER_BLUE );
    justifierFound = TRUE;
    VDP_drawText("Justifier FOUND!", 11, 1);
  } else {
    VDP_drawText("Justifier NOT found.", 10, 1);
  }

  VDP_drawText("Press C to change drawing mode", 5, 5);
  ///////////////////////////////////////////////////////////////////////////////////
  // Main Loop!
  while(TRUE)
  {

    if ( useFlash && flashScreen > 0)
    {
      if (flashScreen == 3)
      {
        PAL_setColors(0, palette_flash, 32, CPU);
      }
      else if (flashScreen == 1)
      {
        PAL_setColors(0, palette, 32, CPU);
      }
      --flashScreen;
    }

    if( justifierFound ) {	
      // get the button states		
      u16 value = JOY_readJoypad(JOY_2);
      if( value & BUTTON_A ) {
        VDP_drawText("A", 18, 9);
      } else {
        VDP_drawText(" ", 18, 9);
      }


      // My blue justifier appears to return 34 through 176 when I use it on 
      // H32 mode.  
      // 
      // if both values are -1, the gun is aiming off screen.	
      s16 xVal = JOY_readJoypadX(JOY_2);
      s16 yVal = JOY_readJoypadY(JOY_2);
      char message[40];
      sprintf( message, "Justifier Values x:%d, y:%d      ", xVal, yVal );
      VDP_drawText(message, 7, 7 );

      sprintf( message, "Offset Values x:%d, y:%d         ", xOffset, yOffset );
      VDP_drawText(message, 7, 10 );

      if( calibrateMode ) {
        VDP_drawText("Aim at target and pull trigger", 5, 3);
      } else {
        VDP_drawText("Press B to start calibration  ", 5, 3);
      }

      // set crosshairs position. Subtract 8 from each to compensate for 16x16 sprite
      switch( crosshairsMode ) { 
        case 0: // raw
          VDP_drawText("   Render raw joypad values   ", 5, 6);
          crosshairsPosX = xVal - 8;
          crosshairsPosY = yVal - 8;
          break;
        case 1: // with lookup
          VDP_drawText("   Render with lookup table   ", 5, 6);
          crosshairsPosX = xLookup[ xVal ] - 8;
          crosshairsPosY = yVal - 8;
          break;
        case 2: // with lookup + offset
          VDP_drawText("  Render with lookup + offset ", 5, 6);
          crosshairsPosX = xLookup[xVal] + xOffset - 8;
          crosshairsPosY = yVal + yOffset - 8;
        default:
          break;
      }
    }

    // Set the Sprite Positions.
    SPR_setPosition( crosshairsSprite, crosshairsPosX, crosshairsPosY );
    SPR_update();

    SYS_doVBlankProcess();

  }
}

