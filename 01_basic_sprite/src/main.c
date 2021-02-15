#include <genesis.h>
#include "resources.h"

int main(u16 hard)
{
	// set backgorund color.  
	VDP_setBackgroundColor(0);

	// Sprite Pointer
	Sprite *shipSprite = NULL;
	Sprite *rockSprite = NULL;

	// load the palette of the ship resource
	VDP_setPalette( PAL1, ship.palette->data );
	VDP_setPalette( PAL2, rock.palette->data );

	// Initialize the Sprite engine
	SPR_init(0,0,0);

	// create the sprite from the resource
	int ship_pos_x = 147;
	int ship_pos_y = 150;
	shipSprite = SPR_addSprite( &ship, ship_pos_x, ship_pos_y, TILE_ATTR( PAL1, 0, FALSE, FALSE ));	
		
	int rock_pos_x = 147;
	int rock_pos_y =  60;
	rockSprite = SPR_addSprite( &rock, rock_pos_x, rock_pos_y, TILE_ATTR( PAL2, 0, FALSE, FALSE ));	

	while(TRUE)
	{
		// read input
		/*ship_pos_y -= 2;
		if( ship_pos_y <= 0  ) {
			ship_pos_y = 240;
		}*/

		SPR_setPosition( shipSprite, ship_pos_x, ship_pos_y );
		SPR_setPosition( rockSprite, rock_pos_x, rock_pos_y );

		// draw current screen (logo, start screen, settings, game, gameover, credits...)
		SPR_update();

		// wait for screen refresh
		SYS_doVBlankProcess();
	}

	SPR_end();

	return 0;
}

