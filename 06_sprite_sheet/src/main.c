#include <genesis.h>
#include "resources.h"

int main(bool i)
{
      
	// set backgorund color.  
	// index can be 0-15 (colors from PAL0) and 16-31 (colors from PAL0), 32-47 (PAL1), 48-63(PAL3)
	VDP_setBackgroundColor(0);

	// load the palette from the ship resource
	PAL_setPalette( PAL0, ship_pal.data, CPU );
	PAL_setPalette( PAL1, rocks_pal.data, CPU );

	// setup the engine
	SPR_init();

	// pos16ers to the sprite objects
	Sprite *shipSprite = NULL;
	Sprite *rockSprite1 = NULL;
	Sprite *rockSprite2 = NULL;
	Sprite *rockSprite3 = NULL;
	Sprite *rockSprite4 = NULL;


	// create the sprite from the resource
	s16 ship_pos_x = 147;
	s16 ship_pos_y = 110;
	shipSprite = SPR_addSprite( &ship, 
			ship_pos_x, 
			ship_pos_y, 
			TILE_ATTR( PAL0, 
				0, 
				FALSE, 
				FALSE ));	

	s16 rock_pos_x_1 = 50;
	s16 rock_pos_y_1 = 50;
	rockSprite1 = SPR_addSprite( &rocks, rock_pos_x_1, rock_pos_y_1, TILE_ATTR( PAL1, 0, FALSE, FALSE ));	
	SPR_setAnim( rockSprite1, 0 );
	s16 rock_pos_x_2 = 100;
	s16 rock_pos_y_2 = 50;
	rockSprite2 = SPR_addSprite( &rocks, rock_pos_x_2, rock_pos_y_2, TILE_ATTR( PAL1, 0, FALSE, FALSE ));	
	SPR_setAnim( rockSprite2, 1 );
	s16 rock_pos_x_3 = 150;
	s16 rock_pos_y_3 = 50;
	rockSprite3 = SPR_addSprite( &rocks, rock_pos_x_3, rock_pos_y_3, TILE_ATTR( PAL1, 0, FALSE, FALSE ));	
	SPR_setAnim( rockSprite3, 2 );
	s16 rock_pos_x_4 = 200;
	s16 rock_pos_y_4 = 50;
	rockSprite4 = SPR_addSprite( &rocks, rock_pos_x_4, rock_pos_y_4, TILE_ATTR( PAL1, 0, FALSE, FALSE ));	
	SPR_setAnim( rockSprite4, 3 );
		

	while(TRUE)
	{
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

		// wait for VBlank start
		SYS_doVBlankProcess();
	}

	SPR_end();

	return 0;
}

