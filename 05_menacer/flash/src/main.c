#include <genesis.h>
s16 flashScreen = 0;

static void joypadHandler( u16 joypadId, u16 changed, u16 state ) {
  if( joypadId == JOY_2  ) {
    // A
    if(state & BUTTON_A) {
      flashScreen = 5;
    }
  }
}


int main(bool hard)
{
  ///////////////////////////////////////////////////////////////////////////////////
  // Flash Setup
  PAL_setColor(0, 0x0004);
  u16 palette_orig[16];
  PAL_getColors( 0, &palette_orig[0], 16);

  const u16 palette_flash[16] =
  {
    0x0888, 0x0888, 0x0888, 0x0888, 0x0888, 0x0888, 0x0888, 0x0888,
    0x0888, 0x0888, 0x0888, 0x0888, 0x0888, 0x0888, 0x0888, 0x0888
  };

  JOY_setEventHandler(joypadHandler);



  ///////////////////////////////////////////////////////////////////////////////////
  // Menacer Setup
  //  1. check Port 2 for the Sega Menacer
  bool menacerFound = FALSE;
  u8 portType = JOY_getPortType(PORT_2);
  if (portType == PORT_TYPE_MENACER)
  {
    // 2. Turn on Menacer support
    JOY_setSupport(PORT_2, JOY_SUPPORT_MENACER);
    menacerFound = TRUE;
    VDP_drawText("Menacer FOUND!", 13, 1);
  }
  else
  {
    VDP_drawText("Menacer NOT found.", 11, 1);
  }




  ///////////////////////////////////////////////////////////////////////////////////
  // Main Loop!
  while (TRUE)
  {

    if( flashScreen == 5 ) {
      PAL_setColors(0, palette_flash, 16, CPU);
    } else if( flashScreen <= 0) {
      PAL_setColors(0, palette_orig, 16, CPU);
    }
    if( flashScreen > 0 ) {
      --flashScreen;
    }

    if (menacerFound)
    {
      // 3. Read X and Y values
      s16 xVal = JOY_readJoypadX(JOY_2);
      s16 yVal = JOY_readJoypadY(JOY_2);
      char message[40];
      if( xVal >= 0 ) {
        sprintf(message, "Menacer Values x:%d, y:%d      ", xVal, yVal);
        VDP_drawText(message, 8, 7);
      }

    }

    SYS_doVBlankProcess();
  }
}
