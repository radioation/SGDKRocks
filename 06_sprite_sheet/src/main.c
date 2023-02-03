#include <genesis.h>
#include "resources.h"

int main(u16 hard)
{

	// set backgorund color.  
	// index can be 0-15 (colors from PAL0) and 16-31 (colors from PAL1), 32-47 (PAL2), 48-63(PAL3)

	VDP_setBackgroundColor(0);

	// pointer to the sprite object
	Sprite *shipSprite = NULL;
	Sprite *rockSprite1 = NULL;
	Sprite *rockSprite2 = NULL;
	Sprite *rockSprite3 = NULL;
	Sprite *rockSprite4 = NULL;

	// load the palette from the ship resource
	VDP_setPalette( PAL1, ship.palette->data );
	VDP_setPalette( PAL2, rocks.palette->data );

	// setup the engine
	SPR_init(0,0,0);

	// create the sprite from the resource
	int ship_pos_x = 147;
	int ship_pos_y = 110;
	shipSprite = SPR_addSprite( &ship, ship_pos_x, ship_pos_y, TILE_ATTR( PAL1, 0, FALSE, FALSE ));	
	int rock_pos_x_1 = 50;
	int rock_pos_y_1 = 50;
	rockSprite1 = SPR_addSprite( &rocks, rock_pos_x_1, rock_pos_y_1, TILE_ATTR( PAL2, 0, FALSE, FALSE ));	
	SPR_setAnim( rockSprite1, 0 );
	int rock_pos_x_2 = 100;
	int rock_pos_y_2 = 50;
	rockSprite2 = SPR_addSprite( &rocks, rock_pos_x_2, rock_pos_y_2, TILE_ATTR( PAL2, 0, FALSE, FALSE ));	
	SPR_setAnim( rockSprite2, 1 );
	int rock_pos_x_3 = 150;
	int rock_pos_y_3 = 50;
	rockSprite3 = SPR_addSprite( &rocks, rock_pos_x_3, rock_pos_y_3, TILE_ATTR( PAL2, 0, FALSE, FALSE ));	
	SPR_setAnim( rockSprite3, 2 );
	int rock_pos_x_4 = 200;
	int rock_pos_y_4 = 50;
	rockSprite4 = SPR_addSprite( &rocks, rock_pos_x_4, rock_pos_y_4, TILE_ATTR( PAL2, 0, FALSE, FALSE ));	
	SPR_setAnim( rockSprite4, 3 );
		

	while(TRUE)
	{
		// read input
		// move sprite
	/*	ship_pos_y -= 2;
		if( ship_pos_y <= 0  ) {
			ship_pos_y = 512;
		}*/

		SPR_setPosition( shipSprite, ship_pos_x, ship_pos_y );

		
		// update text
		char msg[32];
		sprintf( msg, "x: %3d y: %3d", ship_pos_x, ship_pos_y );
		VDP_drawText( msg, 15, 2 );

		// draw current screen (logo, start screen, settings, game, gameover, credits...)
		SPR_update();

		// wait for screen refresh
		VDP_waitVSync();
	}

	SPR_end();

	return 0;
}

