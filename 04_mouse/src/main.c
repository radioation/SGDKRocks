#include <genesis.h>
#include "resources.h"



int main(bool hard)
{
	///////////////////////////////////////////////////////////////////////////////////
	// Sprite Setup
	Sprite *ufoSprite = NULL;
	PAL_setPalette( PAL1, ufo_pal.data, CPU );
	SPR_init();
	// Sprite position 
	fix32 posX = FIX32(154.0);
	fix32 posY = FIX32(108.0);

	ufoSprite = SPR_addSprite( &ufo,               // Sprite defined in resource.res
															fix32ToInt(posX),  // starting X position
															fix32ToInt(posY),  // starting Y position
															TILE_ATTR( PAL1,   // specify palette
																					1,     // Tile priority ( with background)
																					FALSE, // flip the sprite vertically?
																					FALSE  // flip the sprite horizontally
																					));
            


	///////////////////////////////////////////////////////////////////////////////////
	// Mouse support. Unfortunately the wiki is a bit out of date in some places.
	// I'm not seeing JOY_setMouseEventHandler anywhere in SGDK's source code.
	//
	// $ find . -type f -exec grep  JOY_setMouseEventHandler {} + | wc
	//      0       0       0
	//
	// OTOH, the SGDK examples have calls to JOY_setSupport() and JOY_readJoystick[X|Y]()
	// these seem to give me X|Y values from -32767 to 32767.


	// check port 2 for a mouse
	bool mouseFound = FALSE;
	u8 portType = JOY_getPortType(PORT_2);
	if( portType == PORT_TYPE_MOUSE ) {
		JOY_setSupport(PORT_2, JOY_SUPPORT_MOUSE);
		mouseFound = TRUE;
		VDP_drawText("Mouse Found!", 14, 1 );
	} else {
		VDP_drawText("No Mouse", 14, 1 );
	}
	
	// Previous mouse values. To be used to figure out
	// how much the X & Y values have changed since the
	// last read
	s32 prev_x_mouse = JOY_readJoypadX(JOY_2);
	s32 prev_y_mouse = JOY_readJoypadY(JOY_2);

	fix32 sensitivity = FIX32(10);
	while(TRUE)
	{
		if( mouseFound ) {

			// Read mouse values from JOY_readJoypadX[X|Y]().
			// mice return signed axis data. Change in value indicates movment.
			s32 x_mouse = JOY_readJoypadX(JOY_2); // positive change indicates movement to the right of the screen
			s32 y_mouse = JOY_readJoypadY(JOY_2); // positive change indicates movement to the top of the screen

			// Write mouse values to the screen
			VDP_setTextPalette(3);
			char mouseMessage[40];
			sprintf( mouseMessage, "Mouse Values x:%ld, y:%ld      ", x_mouse, y_mouse );
			VDP_drawText(mouseMessage, 8, 10 );
			VDP_setTextPalette(0);


			// Figure out how much the mice have moved.  
			fix32 x_delta = fix32Div( FIX32(x_mouse) - FIX32(prev_x_mouse), sensitivity);
			fix32 y_delta = fix32Div( FIX32(prev_y_mouse) - FIX32(y_mouse), sensitivity);

			// change the mouse position.
			posX = posX + x_delta;
			posY = posY - y_delta;
			// save mouse values for next calculation
			prev_x_mouse = x_mouse;
			prev_y_mouse = y_mouse;
     
		 	// Move the sprite
			SPR_setPosition( ufoSprite, fix32ToInt(posX), fix32ToInt(posY));
			SPR_update();
		}

    // get the button states
    s16 value = JOY_readJoypad(JOY_2);
    if( value & BUTTON_A ) {
      VDP_drawText("A", 8, 8 );
    } else {
      VDP_drawText(" ", 8, 8 );
    }

    if( value & BUTTON_B ) {
      VDP_drawText("B", 10, 8 );
    } else {
      VDP_drawText(" ", 10, 8 );
    }

    if( value & BUTTON_C ) {
      VDP_drawText("C", 12, 8 );
    } else {
      VDP_drawText(" ", 12, 8 );
    }



		SYS_doVBlankProcess();
	}

	return 0;
}


