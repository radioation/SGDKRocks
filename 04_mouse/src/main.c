#include <genesis.h>
#include "resources.h"



int main(u32 hard)
{
	// Sprite
	Sprite *ufoSprite = NULL;
	VDP_setPalette( PAL1, ufo.palette->data );
	SPR_init();
	fix32 posX = FIX32(154.0);
	fix32 posY = FIX32(108.0);
	ufoSprite = SPR_addSprite( &ufo,      // Sprite defined in resource.res
                            fix32ToInt(posX),  // starting X position
                            fix32ToInt(posY),  // starting Y position
                            TILE_ATTR( PAL1,   // specify palette
                                        1,     // Tile priority ( with background)
                                        FALSE, // flip the sprite vertically?
                                        FALSE  // flip the sprite horizontally
                                        ));




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
    VDP_drawText("No Mouse", 32, 1 );
	}
	bool prev_set = FALSE;
	s32 x_mouse;
	s32 y_mouse;
	s32 prev_x_mouse;
	s32 prev_y_mouse;

	fix32 sensitivity = FIX32(10);
  while(TRUE)
  {
    if( mouseFound ) {
      // mice return signed axis data. Change in value
      // indicates movment.
      x_mouse = JOY_readJoypadX(JOY_2); // positive change indicates movement to the right
      y_mouse = JOY_readJoypadY(JOY_2); // positive change indicates movement to the left
      VDP_setTextPalette(3);
      char mouseMessage[40];
      sprintf( mouseMessage, "Mouse Values x:%d, y:%d      ", x_mouse, y_mouse );
      VDP_drawText(mouseMessage, 8, 10 );
      VDP_setTextPalette(0);

    }

		if( prev_set == FALSE ) {
			prev_x_mouse = x_mouse;
			prev_y_mouse = y_mouse;
			prev_set = TRUE;
		} else {
			fix32 x_delta = fix32Div( FIX32(x_mouse) - FIX32(prev_x_mouse), sensitivity);
			fix32 y_delta = fix32Div( FIX32(y_mouse) - FIX32(prev_y_mouse), sensitivity);
			posX = fix32Add( posX, x_delta);
			posY = fix32Sub( posY, y_delta);
			prev_x_mouse = x_mouse;
			prev_y_mouse = y_mouse;
		}

		SPR_setPosition( ufoSprite, fix32ToInt(posX), fix32ToInt(posY));

		SPR_update();


    SYS_doVBlankProcess();
  }

  return 0;
}


