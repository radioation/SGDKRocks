#include <genesis.h>
#include "resources.h"

int main(bool hard)
{
	// set backgorund color.  
	VDP_setBackgroundColor(0);

	// Sprite Pointer
	Sprite *shipSprite = NULL;
	Sprite *rockSprite = NULL;

	// load the palette of the ship resource
	PAL_setPalette( PAL1, ship_pal.data, CPU );
	PAL_setPalette( PAL2, rock_pal.data, CPU );

	// Initialize the Sprite engine
	SPR_init();

	// create the ship sprite from the resource object
	// and center it in the screen.  The Genesis display
	// is 320x224 and the sprites in this example are 16
	// pixels wide.  So subtract half of the sprite width
	// and height from the horizontal and vertical centers
	int ship_pos_x = 152; // 320/2 - 8  
	int ship_pos_y = 102; // 224/2 - 8
	shipSprite = SPR_addSprite( &ship, ship_pos_x, ship_pos_y, TILE_ATTR( PAL1, 0, FALSE, FALSE ));	
	
	// place a single rock in the upper left portion of the screen
	int rock_pos_x = 0;
	int rock_pos_y = 0;
	rockSprite = SPR_addSprite( &rock, rock_pos_x, rock_pos_y, TILE_ATTR( PAL2, 0, FALSE, FALSE ));	

	while(TRUE)
	{
		// move the ship
		/*ship_pos_y -= 2;
		if( ship_pos_y <= 0  ) {
			ship_pos_y = 240;
		}*/

		SPR_setPosition( shipSprite, ship_pos_x, ship_pos_y );
		SPR_setPosition( rockSprite, rock_pos_x, rock_pos_y );

		// draw current screen 
		SPR_update();

		// Wait for VBlank start
		SYS_doVBlankProcess();
	}

	// Release the sprites. 
	SPR_releaseSprite( shipSprite );
	SPR_releaseSprite( rockSprite );

	// stop the sprite engine
	SPR_end();

	return 0;
}

