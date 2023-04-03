#include <genesis.h>

int main(bool hard)
{


  PAL_setColor(0, 0x0844);
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
    if (menacerFound)
    {
      // 3. Read X and Y values
      s16 xVal = JOY_readJoypadX(JOY_2);
      s16 yVal = JOY_readJoypadY(JOY_2);
      char message[40];
      sprintf(message, "Menacer Values x:%d, y:%d      ", xVal, yVal);
      VDP_drawText(message, 8, 7);

      // 4. Read the button states
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

      if (value & BUTTON_START)
      {
        VDP_drawText("C", 24, 9);
      }
      else {
        VDP_drawText(" ", 24, 9);
      }

    }
    SYS_doVBlankProcess();
  }
}
