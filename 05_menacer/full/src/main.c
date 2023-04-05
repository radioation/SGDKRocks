#include <genesis.h>
#include "resources.h"

// Setup palette to flash screen when trigger is pulled
static u16 palette[32];
const u16 palette_flash[32] =
{
  0x0EEE, 0x0EEE, 0x0EEE, 0x0EEE, 0x0EEE, 0x0EEE, 0x0EEE, 0x0EEE,

  0x0EEE, 0x0EEE, 0x0EEE, 0x0EEE, 0x0EEE, 0x0EEE, 0x0EEE, 0x0EEE,

  0x0EEE, 0x0EEE, 0x0EEE, 0x0EEE, 0x0EEE, 0x0EEE, 0x0EEE, 0x0EEE,

  0x0EEE, 0x0EEE, 0x0EEE, 0x0EEE, 0x0EEE, 0x0EEE, 0x0EEE, 0x0EEE
};


static u8 flashScreen = 0;
static bool useFlash = FALSE;
static bool useDark = FALSE;


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


static void joypadHandler( u16 joypadId, u16 changed, u16 state ) {
  if( joypadId == JOY_2  ) {

    // A
    if( state & BUTTON_A ) {
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

    // B
    if( state & BUTTON_B  ){
      currentValue = 0;
      calibrateMode = TRUE;
    }

    // C change render mode
    if( state & BUTTON_C ) {
      ++crosshairsMode;
      if( crosshairsMode > 2 ) {
        crosshairsMode = 0;
      }
    }
  } else if( joypadId == JOY_1  ) {
    // joypad 1 
    // Z - Toggle screen flash
    if( state & BUTTON_A ) {
      useFlash = !useFlash;
    }
    if( state & BUTTON_B ) {
      useDark = !useDark;
      if( useDark ) {
        palette[15] = 0x0888;
        palette[0] = 0x0004;
        PAL_setColor(15, 0x0888);
        PAL_setColor(0, 0x0004);
      } else {
        palette[15] = 0x0000;
        palette[0] = 0x844;
        PAL_setColor(15, 0x0000);
        PAL_setColor(0, 0x0844);
      }
    }

  }
}

static void calculateXLookup() {
  // T2
  // $70-$83   :  112-131   : Total hvals 20
  // $84-$B6; $E5-$FF; $00-$42  :  132-182; 229-255; 0-66   :  51; 27; 67  -> only 51+27+66 = 145 hvals 
  // $43-$6F   : 67-111  : 45 hvals
  //  there's only 210 havls total with many offscreen
  //  The active area is 290 < 320 pixels.
  //
  // since I'm going to bother with a calibration step  and offset I will use arbitray values
  //  in the lookup
  //
  //  My own experience has been
  //    Sega Menacer:  At the left start at 65 and runs to 182. Then skpps to 229  and contiues to 255
  //                   then finally 0 to 3 on the right side 
  //                    ( 182-65 ) + (255-229) + (3 - 0)  = 146
  //
  //    Radica Menacer:  At the left start at 79 and runs to 182. Then skpps to 229  and contiues to 255
  //                   then finally 0 to 28 on the right side 
  //                    ( 182-79 ) + (255-229) + (28 - 0) = 157  
  //
  //    Kega Fusion:  At the left start at 77 and runs to 182. Then skpps to 228  and contiues to 255
  //                   then finally 0 to 22 on the right side 
  //                    ( 182-77 ) + (255-228) + (22 - 0) =  154
  //
  //  
  s16 pos = 0;	
  for( int i=60; i < 183; ++i ) {
    xLookup[i] =  pos;
    pos += 2;
  }
  for( int i=229; i < 256; ++i ) {
    xLookup[i] =  pos;
    pos += 2;
  }
  for( int i=0; i < 60; ++i ) {
    xLookup[i] =  pos;
    pos += 2;
  }

}

int main(bool hard)
{
  

  ///////////////////////////////////////////////////////////////////////////////////
  // Sprite Setup
  Sprite *targetSprite = NULL;
  PAL_setPalette( PAL1, target_pal.data, CPU );
  SPR_init();

  // crosshair is 16x16
  Sprite *crosshairsSprite = NULL;
  s16 crosshairsPosX = 152;
  s16 crosshairsPosY = 104;
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
  // palette Setup
  // Set background brighter than 0.	Dark backgrounds
  // prevents menacer from returning X, Y values.
  PAL_setColor(15, 0x0000);
  VDP_setTextPalette(0);
  PAL_setColor(0, 0x0844);
  PAL_getColors( 0, &palette[0], 16);
  memcpy(&palette[16], target_pal.data, 16 * 2);

  ///////////////////////////////////////////////////////////////////////////////////
  // BG setup
  s16 indexB = TILE_USER_INDEX;
  VDP_loadTileSet(bgrect.tileset, indexB, CPU);
  VDP_drawImageEx(BG_B, &bgrect, TILE_ATTR_FULL(PAL1, FALSE, FALSE, FALSE, indexB), 0, 0, FALSE, TRUE);

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
    VDP_drawText("Menacer FOUND!", 13, 1);
  }
  else
  {
    VDP_drawText("Menacer NOT found.", 11, 1);
  }
  VDP_drawText("Press C to change drawing mode", 5, 5);

  VDP_drawText("Use Joypad A to toggle flash mode", 4, 17);
  VDP_drawText("Use Joypad B to toggle dark mode", 4, 19);
  ///////////////////////////////////////////////////////////////////////////////////
  // Main Loop!
  while (TRUE)
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

    if (menacerFound)
    {
      // get the button states
      u16 value = JOY_readJoypad(JOY_2);
      if (value & BUTTON_A)
      {
        VDP_drawText("A", 18, 9);
      }
      else
      {
        VDP_drawText(" ", 18, 9);
      }

      if (value & BUTTON_B)
      {
        VDP_drawText("B", 20, 9);
      }
      else
      {
        VDP_drawText(" ", 20, 9);
      }

      if (value & BUTTON_C)
      {
        VDP_drawText("C", 22, 9);
      }
      else
      {
        VDP_drawText(" ", 22, 9);
      }

      // The menacer appears to return 8-bit values (0 to 255)
      // if both values are -1, the gun is aiming off screen.
      s16 xVal = JOY_readJoypadX(JOY_2);
      s16 yVal = JOY_readJoypadY(JOY_2);
      char message[40];
      sprintf(message, "Menacer Values x:%d, y:%d      ", xVal, yVal);
      VDP_drawText(message, 8, 7);

      sprintf(message, "Offset Values x:%d, y:%d         ", xOffset,yOffset);
      VDP_drawText(message, 7, 10);

      if (calibrateMode)
      {
        VDP_drawText("Aim at target and pull trigger", 5, 3);
      }
      else
      {
        VDP_drawText("Press B to start calibration  ", 5, 3);
      }

      // set crosshairs position. Subtract 8 from each to compensate for 16x16 sprite
      switch (crosshairsMode)
      {
        case 0: // raw
          VDP_drawText("   Render raw joypad values   ", 5, 6);
          crosshairsPosX = xVal - 8;
          crosshairsPosY = yVal - 8;
          break;
        case 1: // with lookup
          VDP_drawText("   Render with lookup table   ", 5, 6);
          crosshairsPosX = xLookup[xVal] - 8;
          crosshairsPosY = yVal - 8;
          break;
        case 2: // with lookup + offset
          VDP_drawText("  Render with lookup + offset ", 5, 6);
          crosshairsPosX = xLookup[xVal] + xOffset - 8;
          crosshairsPosY = yVal + yOffset -8;
        default:
          break;
      }
    }

    // Set the Sprite Positions.
    SPR_setPosition(crosshairsSprite, crosshairsPosX, crosshairsPosY);
    SPR_update();

    SYS_doVBlankProcess();
  }
}
