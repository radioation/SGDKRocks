#include <genesis.h>
#include "resources.h"

// define sound constants
#define SND_LASER      64
#define SND_EXPLOSION   65


// define player constants
#define PLAYER_FRAME_COUNT  16
#define MAX_ROTATION_INDEX  FIX32(15)
#define MIN_ROTATION_INDEX  FIX32(0)
#define PLAYER_WIDTH        16
#define PLAYER_HEIGHT       16
#define PLAYER_SHOT_WIDTH   8 
#define PLAYER_SHOT_HEIGHT  8 
#define MAX_PLAYER_SHOTS    4


// enemy constants
#define MAX_ENEMIES         2
#define MAX_ENEMY_SHOTS     3

#define MAX_ROCKS           5 
#define MAX_MID_ROCKS       10 
#define MAX_SMALL_ROCKS     20
#define MAX_XS_ROCKS        30

#define MAX_EXPLOSIONS      6

#define MAP_WIDTH           1280
#define MAP_HEIGHT          896 

#define SCR_WIDTH           320
#define SCR_HEIGHT          224 

#define CAM_BOUNDARY        90



typedef struct {
	Sprite *sprite;
	fix32 pos_x;
	fix32 pos_y;
	fix32 vel_x;
	fix32 vel_y;

	s16 hitbox_x1;
	s16 hitbox_y1;
	s16 hitbox_x2;
	s16 hitbox_y2;

	bool active;
	u16 ticks;
	u16 hitpoints;
} CP_SPRITE;


// map variables
Map *map_a;
s16   camPosX;  // relative to total world map
s16   camPosY;  // relative to total world map

// player variables
CP_SPRITE player;
fix32 playerRotation;
bool  doPlayerUpdate;
fix32 deltaX[PLAYER_FRAME_COUNT];
fix32 deltaY[PLAYER_FRAME_COUNT];
CP_SPRITE playerShots[ MAX_PLAYER_SHOTS ];


// enemies
CP_SPRITE enemies[MAX_ENEMIES];
CP_SPRITE enemyShots[MAX_ENEMY_SHOTS];
CP_SPRITE rocks[MAX_ROCKS];
CP_SPRITE midRocks[MAX_MID_ROCKS];
CP_SPRITE smallRocks[MAX_SMALL_ROCKS];
CP_SPRITE xsRocks[MAX_XS_ROCKS];

CP_SPRITE explosions[MAX_EXPLOSIONS];
u16 currentExplosion = 0;


static void addExplosion(  fix32 pos_x, fix32 pos_y ) {
  if( explosions[ currentExplosion ].active == FALSE ){
    // use it
    explosions[currentExplosion].pos_x  = pos_x;
    explosions[currentExplosion].pos_y  = pos_y;

    explosions[currentExplosion].active = TRUE;
    explosions[currentExplosion].ticks = 0;
    SPR_setVisibility( explosions[currentExplosion].sprite, VISIBLE);
    SPR_setPosition( explosions[currentExplosion].sprite,fix32ToInt(explosions[currentExplosion].pos_x),fix32ToInt(explosions[currentExplosion].pos_y));

		XGM_startPlayPCM(SND_EXPLOSION,10,SOUND_PCM_CH3);

    // point to next one
    ++currentExplosion;
    if( currentExplosion >= MAX_EXPLOSIONS ) {
      currentExplosion = 0;
    }
  }
}



void updateCameraPos() {
	s16 px = fix32ToInt( player.pos_x );
	s16 py = fix32ToInt( player.pos_y );

	s16 playerScreenX = px - camPosX;
	s16 playerScreenY = py - camPosY;

	// adjust new camera position based on player position
	s16 newCamX;
	s16 newCamY;
	if( playerScreenX > SCR_WIDTH - CAM_BOUNDARY - PLAYER_WIDTH ) {
		newCamX = px - ( SCR_WIDTH - CAM_BOUNDARY - PLAYER_WIDTH );
	} else if( playerScreenX < CAM_BOUNDARY ) {
		newCamX = px - CAM_BOUNDARY;	
	} else {
		newCamX = camPosX; // no change
	}

	if( playerScreenY > SCR_HEIGHT - CAM_BOUNDARY - PLAYER_HEIGHT ) {
		newCamY = py - ( SCR_HEIGHT - CAM_BOUNDARY - PLAYER_HEIGHT ) ;
	} else if( playerScreenY < CAM_BOUNDARY ) {
		newCamY = py - CAM_BOUNDARY;	
	} else {
		newCamY = camPosY; // no change
	}


	// handle edges
	if ( newCamX < 0  ) {
		newCamX = 0;
	} else if ( newCamX > (MAP_WIDTH - SCR_WIDTH )) {
		newCamX = MAP_WIDTH - SCR_WIDTH ;
	}

	if ( newCamY < 0  ) {
		newCamY = 0;
	} else if ( newCamY > (MAP_HEIGHT - SCR_HEIGHT )) {
		newCamY = MAP_HEIGHT - SCR_HEIGHT ;
	}

	camPosX = newCamX;
	camPosY = newCamY;
	MAP_scrollTo( map_a, camPosX, camPosY );
}

void inputCallback( u16 joy, u16 changed, u16 state ) {
	// create a shot if available
	if( changed & state & BUTTON_A ) {
		for( u16 i = 0; i < MAX_PLAYER_SHOTS; ++i ) {
			if( playerShots[i].active == FALSE ) {
				// create a new one
				XGM_startPlayPCM(SND_LASER,1,SOUND_PCM_CH2);

				u16 rot = fix32ToInt( playerRotation);
				playerShots[i].pos_x = fix32Add( player.pos_x, FIX32((PLAYER_WIDTH-PLAYER_SHOT_WIDTH)/2)) +  fix32Mul( deltaX[rot], FIX32(2.0));
				playerShots[i].pos_y = fix32Add( player.pos_y, FIX32((PLAYER_HEIGHT-PLAYER_SHOT_WIDTH)/2)) +  fix32Mul( deltaY[rot], FIX32(2.0));
				playerShots[i].vel_x = fix32Mul( deltaX[rot], FIX32(2.0));
				playerShots[i].vel_y = fix32Mul( deltaY[rot], FIX32(2.0));
				playerShots[i].active = TRUE;
				playerShots[i].ticks = 0;
				break;

			}
		}
	}
}


void handleInput() {
	u16 value = JOY_readJoypad(JOY_1);

	if( value & BUTTON_LEFT ) {
		playerRotation += FIX32( 0.25 );
		if( playerRotation > MAX_ROTATION_INDEX ) {
			playerRotation = MIN_ROTATION_INDEX;
		}
		int rot = fix32ToInt( playerRotation);
		SPR_setAnim( player.sprite, rot);
	} else if( value & BUTTON_RIGHT ) {
		playerRotation -= FIX32( 0.25 );
		if( playerRotation < MIN_ROTATION_INDEX ) {
			playerRotation = MAX_ROTATION_INDEX;
		}
		int rot = fix32ToInt( playerRotation);
		SPR_setAnim( player.sprite, rot);
	}



	if( value & BUTTON_UP ) {
		doPlayerUpdate = TRUE;
	} else {
		doPlayerUpdate = FALSE;
	}

}

void update() {
	if( doPlayerUpdate == TRUE ) {
		int rot = fix32ToInt( playerRotation);
		player.pos_x = fix32Add( player.pos_x, deltaX[rot] );
		player.pos_y = fix32Add( player.pos_y, deltaY[rot] );


		if( player.pos_x< FIX32( -6.0 ) ) {
			player.pos_x = 	FIX32( -6.0 );
		} else if( player.pos_x > FIX32( MAP_WIDTH - PLAYER_WIDTH + 6.0 )) {
			player.pos_x = 	FIX32( MAP_WIDTH - PLAYER_WIDTH + 6.0 );
		}

		if( player.pos_y< FIX32( -6.0) ) {
			player.pos_y = 	FIX32( -6.0 );
		} else if( player.pos_y > FIX32( MAP_HEIGHT - PLAYER_HEIGHT + 6.0 )) {
			player.pos_y = 	FIX32( MAP_HEIGHT - PLAYER_HEIGHT + 6.0 );
		}


	}


	// shots
	for( u16 i=0; i < MAX_PLAYER_SHOTS; ++i ) {
		if( playerShots[i].active == TRUE ) {
			playerShots[i].pos_x +=  playerShots[i].vel_x;
			playerShots[i].pos_y +=  playerShots[i].vel_y;
			s16 x = fix32ToInt(playerShots[i].pos_x) - camPosX;	
			s16 y = fix32ToInt(playerShots[i].pos_y) - camPosY;	
			if( x >= 0 && x < SCR_WIDTH && y >= 0 && y < SCR_HEIGHT ) {
				SPR_setVisibility( playerShots[i].sprite, VISIBLE);
				SPR_setPosition(playerShots[i].sprite, fix32ToInt(playerShots[i].pos_x) - camPosX, fix32ToInt(playerShots[i].pos_y) - camPosY );
			} else {
				playerShots[i].active = FALSE;
				SPR_setVisibility( playerShots[i].sprite, HIDDEN);
			}
		} else {
			SPR_setVisibility( playerShots[i].sprite, HIDDEN);
		}
	}




  // COPY PASTE IS THE WORST FORM OF REUSE:   MOVE TO A GENERIC GameEntity struct next 
	for( u16 i=0; i < MAX_ROCKS; ++i ) {
		if( rocks[i].active == TRUE ) {
			rocks[i].pos_x +=  rocks[i].vel_x;
			if( rocks[i].pos_x < FIX32(-32) ) { rocks[i].pos_x = FIX32(MAP_WIDTH);} 
			else if( rocks[i].pos_x >FIX32(MAP_WIDTH) ) { rocks[i].pos_x = FIX32(-32);} 

			rocks[i].pos_y +=  rocks[i].vel_y;
			if( rocks[i].pos_y < FIX32(-32) ) { rocks[i].pos_y = FIX32(MAP_HEIGHT);} 
			else if( rocks[i].pos_y >FIX32(MAP_HEIGHT)  ) { rocks[i].pos_y = FIX32(-32);} 

			s16 x = fix32ToInt(rocks[i].pos_x) - camPosX;	
			s16 y = fix32ToInt(rocks[i].pos_y) - camPosY;	
			if( x >= -32 && x < SCR_WIDTH && y >= -32 && y < SCR_HEIGHT ) {
				SPR_setVisibility( rocks[i].sprite, VISIBLE);
				SPR_setPosition(rocks[i].sprite, fix32ToInt(rocks[i].pos_x) - camPosX, fix32ToInt(rocks[i].pos_y) - camPosY );
			} else {
				SPR_setVisibility( rocks[i].sprite, HIDDEN);
			}
		} else {
			SPR_setVisibility( rocks[i].sprite, HIDDEN);
		}
	}

	for( u16 i=0; i < MAX_ROCKS; ++i ) {
		if( midRocks[i].active == TRUE ) {
			midRocks[i].pos_x +=  midRocks[i].vel_x;
			if( midRocks[i].pos_x < FIX32(-32) ) { midRocks[i].pos_x = FIX32(MAP_WIDTH);} 
			else if( midRocks[i].pos_x >FIX32(MAP_WIDTH) ) { midRocks[i].pos_x = FIX32(-32);} 

			midRocks[i].pos_y +=  midRocks[i].vel_y;
			if( midRocks[i].pos_y < FIX32(-32) ) { midRocks[i].pos_y = FIX32(MAP_HEIGHT);} 
			else if( midRocks[i].pos_y >FIX32(MAP_HEIGHT)  ) { midRocks[i].pos_y = FIX32(-32);} 

			s16 x = fix32ToInt(midRocks[i].pos_x) - camPosX;	
			s16 y = fix32ToInt(midRocks[i].pos_y) - camPosY;	
			if( x >= -32 && x < SCR_WIDTH && y >= -32 && y < SCR_HEIGHT ) {
				SPR_setVisibility( midRocks[i].sprite, VISIBLE);
				SPR_setPosition(midRocks[i].sprite, fix32ToInt(midRocks[i].pos_x) - camPosX, fix32ToInt(midRocks[i].pos_y) - camPosY );
			} else {
				SPR_setVisibility( midRocks[i].sprite, HIDDEN);
			}
		} else {
			SPR_setVisibility( midRocks[i].sprite, HIDDEN);
		}
	}

	for( u16 i=0; i < MAX_SMALL_ROCKS; ++i ) {
		if( smallRocks[i].active == TRUE ) {
			smallRocks[i].pos_x +=  smallRocks[i].vel_x;
			if( smallRocks[i].pos_x < FIX32(-32) ) { smallRocks[i].pos_x = FIX32(MAP_WIDTH);} 
			else if( smallRocks[i].pos_x >FIX32(MAP_WIDTH) ) { smallRocks[i].pos_x = FIX32(-32);} 

			smallRocks[i].pos_y +=  smallRocks[i].vel_y;
			if( smallRocks[i].pos_y < FIX32(-32) ) { smallRocks[i].pos_y = FIX32(MAP_HEIGHT);} 
			else if( smallRocks[i].pos_y >FIX32(MAP_HEIGHT)  ) { smallRocks[i].pos_y = FIX32(-32);} 

			s16 x = fix32ToInt(smallRocks[i].pos_x) - camPosX;	
			s16 y = fix32ToInt(smallRocks[i].pos_y) - camPosY;	
			if( x >= -32 && x < SCR_WIDTH && y >= -32 && y < SCR_HEIGHT ) {
				SPR_setVisibility( smallRocks[i].sprite, VISIBLE);
				SPR_setPosition(smallRocks[i].sprite, fix32ToInt(smallRocks[i].pos_x) - camPosX, fix32ToInt(smallRocks[i].pos_y) - camPosY );
			} else {
				SPR_setVisibility( smallRocks[i].sprite, HIDDEN);
			}
		} else {
			SPR_setVisibility( smallRocks[i].sprite, HIDDEN);
		}
	}

	for( u16 i=0; i < MAX_SMALL_ROCKS; ++i ) {
		if( xsRocks[i].active == TRUE ) {
			xsRocks[i].pos_x +=  xsRocks[i].vel_x;
			if( xsRocks[i].pos_x < FIX32(-32) ) { xsRocks[i].pos_x = FIX32(MAP_WIDTH);} 
			else if( xsRocks[i].pos_x >FIX32(MAP_WIDTH) ) { xsRocks[i].pos_x = FIX32(-32);} 

			xsRocks[i].pos_y +=  xsRocks[i].vel_y;
			if( xsRocks[i].pos_y < FIX32(-32) ) { xsRocks[i].pos_y = FIX32(MAP_HEIGHT);} 
			else if( xsRocks[i].pos_y >FIX32(MAP_HEIGHT)  ) { xsRocks[i].pos_y = FIX32(-32);} 

			s16 x = fix32ToInt(xsRocks[i].pos_x) - camPosX;	
			s16 y = fix32ToInt(xsRocks[i].pos_y) - camPosY;	
			if( x >= -32 && x < SCR_WIDTH && y >= -32 && y < SCR_HEIGHT ) {
				SPR_setVisibility( xsRocks[i].sprite, VISIBLE);
				SPR_setPosition(xsRocks[i].sprite, fix32ToInt(xsRocks[i].pos_x) - camPosX, fix32ToInt(xsRocks[i].pos_y) - camPosY );
			} else {
				SPR_setVisibility( xsRocks[i].sprite, HIDDEN);
			}
		} else {
			SPR_setVisibility( xsRocks[i].sprite, HIDDEN);
		}
	}

	// UFO physics
	for( u16 i=0; i < MAX_ENEMIES; ++i ) {
		if( enemies[i].active == TRUE ) {
			enemies[i].pos_x +=  enemies[i].vel_x;
			if( enemies[i].pos_x < FIX32(-32) ) { enemies[i].pos_x = FIX32(MAP_WIDTH);} 
			else if( enemies[i].pos_x >FIX32(MAP_WIDTH) ) { enemies[i].pos_x = FIX32(-32);} 

			enemies[i].pos_y +=  enemies[i].vel_y;
			if( enemies[i].pos_y < FIX32(-32) ) { enemies[i].pos_y = FIX32(MAP_HEIGHT);} 
			else if( enemies[i].pos_y >FIX32(MAP_HEIGHT)  ) { enemies[i].pos_y = FIX32(-32);} 

			s16 x = fix32ToInt(enemies[i].pos_x) - camPosX;	
			s16 y = fix32ToInt(enemies[i].pos_y) - camPosY;	
			if( x >= -32 && x < SCR_WIDTH && y >= -32 && y < SCR_HEIGHT ) {
				SPR_setVisibility( enemies[i].sprite, VISIBLE);
				SPR_setPosition(enemies[i].sprite, fix32ToInt(enemies[i].pos_x) - camPosX, fix32ToInt(enemies[i].pos_y) - camPosY );
			} else {
				SPR_setVisibility( enemies[i].sprite, HIDDEN);
			}
		} else {
			SPR_setVisibility( enemies[i].sprite, HIDDEN);
		}
	}





  for( u16 i=0; i < MAX_EXPLOSIONS; ++i ) {
    if( explosions[i].active == TRUE ) {
      explosions[i].ticks += 1;
      if( explosions[i].ticks < 9 ) {
      //  SPR_setFrame( explosions[i].sprite, explosions[i].ticks );
				SPR_setPosition(explosions[i].sprite, fix32ToInt(explosions[i].pos_x) - camPosX, fix32ToInt(explosions[i].pos_y) - camPosY );
        SPR_setAnimAndFrame( explosions[i].sprite, i%4, explosions[i].ticks );
      }
      else {
        explosions[i].active = FALSE;
        SPR_setVisibility( explosions[i].sprite, HIDDEN);
      }
    }

  }



	updateCameraPos();
	SPR_setPosition( player.sprite, fix32ToInt( player.pos_x) - camPosX, fix32ToInt( player.pos_y ) - camPosY );

}

static void checkCollisions() {
  for( u16 i=0; i < MAX_ENEMIES; ++i ) {
    if( enemies[i].active == TRUE ) {
      // check if ship has hit
      if( (enemies[i].pos_x + enemies[i].hitbox_x1) < (player.pos_x + player.hitbox_x2) &&
          (enemies[i].pos_x + enemies[i].hitbox_x2) > (player.pos_x + player.hitbox_x1) &&
					(enemies[i].pos_y + enemies[i].hitbox_y1) < (player.pos_y + player.hitbox_y2) &&
					(enemies[i].pos_y + enemies[i].hitbox_y2) > (player.pos_y + player.hitbox_y1)  )
			{
				enemies[i].hitpoints -=1;
				if( enemies[i].hitpoints == 0 ) {
					enemies[i].active = FALSE;
					SPR_setVisibility( enemies[i].sprite, HIDDEN);
					addExplosion(  enemies[i].pos_x, enemies[i].pos_y );
				}
				//SPR_setVisibility( player.sprite, HIDDEN );
			}

			for( u16 j=0; j < MAX_PLAYER_SHOTS; ++j ) {
				if(
						playerShots[j].active == TRUE &&
						(enemies[i].pos_x + enemies[i].hitbox_x1) < (playerShots[j].pos_x + FIX32(4)) &&
						(enemies[i].pos_x + enemies[i].hitbox_x2) > (playerShots[j].pos_x + FIX32(4)) &&
						(enemies[i].pos_y + enemies[i].hitbox_y1) < (playerShots[j].pos_y + FIX32(4)) &&
						(enemies[i].pos_y + enemies[i].hitbox_y2) > (playerShots[j].pos_y + FIX32(4))  )
				{
					enemies[i].hitpoints -=1;
					if( enemies[i].hitpoints == 0 ) {
						enemies[i].active = FALSE;
						SPR_setVisibility( enemies[i].sprite, HIDDEN);
					}
					playerShots[j].active = FALSE;
					SPR_setVisibility( playerShots[j].sprite, HIDDEN);
					addExplosion(  enemies[i].pos_x, enemies[i].pos_y );
				}
			}
		}
	}

	for( u16 i=0; i < MAX_ROCKS; ++i ) {
		if( rocks[i].active == TRUE ) {
			// check if ship has hit
			if( (rocks[i].pos_x + rocks[i].hitbox_x1) < (player.pos_x + player.hitbox_x2) &&
					(rocks[i].pos_x + rocks[i].hitbox_x2) > (player.pos_x + player.hitbox_x1) &&
					(rocks[i].pos_y + rocks[i].hitbox_y1) < (player.pos_y + player.hitbox_y2) &&
					(rocks[i].pos_y + rocks[i].hitbox_y2) > (player.pos_y + player.hitbox_y1)  )
			{
				rocks[i].hitpoints -=1;
				if( rocks[i].hitpoints == 0 ) {
					rocks[i].active = FALSE;
					SPR_setVisibility( rocks[i].sprite, HIDDEN);
					addExplosion(  rocks[i].pos_x, rocks[i].pos_y );
				}
				//SPR_setVisibility( player.sprite, HIDDEN );
			}

			for( u16 j=0; j < MAX_PLAYER_SHOTS; ++j ) {
				if(
						playerShots[j].active == TRUE &&
						(rocks[i].pos_x + rocks[i].hitbox_x1) < (playerShots[j].pos_x + FIX32(4)) &&
						(rocks[i].pos_x + rocks[i].hitbox_x2) > (playerShots[j].pos_x + FIX32(4)) &&
						(rocks[i].pos_y + rocks[i].hitbox_y1) < (playerShots[j].pos_y + FIX32(4)) &&
						(rocks[i].pos_y + rocks[i].hitbox_y2) > (playerShots[j].pos_y + FIX32(4))  )
				{
					rocks[i].hitpoints -=1;
					if( rocks[i].hitpoints == 0 ) {
						rocks[i].active = FALSE;
						SPR_setVisibility( rocks[i].sprite, HIDDEN);
					}
					playerShots[j].active = FALSE;
					SPR_setVisibility( playerShots[j].sprite, HIDDEN);
					addExplosion(  rocks[i].pos_x, rocks[i].pos_y );
				}
			}
		}
	}

	for( u16 i=0; i < MAX_ROCKS; ++i ) {
		if( midRocks[i].active == TRUE ) {
			// check if ship has hit
			if( (midRocks[i].pos_x + midRocks[i].hitbox_x1) < (player.pos_x + player.hitbox_x2) &&
					(midRocks[i].pos_x + midRocks[i].hitbox_x2) > (player.pos_x + player.hitbox_x1) &&
					(midRocks[i].pos_y + midRocks[i].hitbox_y1) < (player.pos_y + player.hitbox_y2) &&
					(midRocks[i].pos_y + midRocks[i].hitbox_y2) > (player.pos_y + player.hitbox_y1)  )
			{
				midRocks[i].hitpoints -=1;
				if( midRocks[i].hitpoints == 0 ) {
					midRocks[i].active = FALSE;
					SPR_setVisibility( midRocks[i].sprite, HIDDEN);
					addExplosion(  midRocks[i].pos_x, midRocks[i].pos_y );
				}
				//SPR_setVisibility( player.sprite, HIDDEN );
			}

			for( u16 j=0; j < MAX_PLAYER_SHOTS; ++j ) {
				if(
						playerShots[j].active == TRUE &&
						(midRocks[i].pos_x + midRocks[i].hitbox_x1) < (playerShots[j].pos_x + FIX32(4)) &&
						(midRocks[i].pos_x + midRocks[i].hitbox_x2) > (playerShots[j].pos_x + FIX32(4)) &&
						(midRocks[i].pos_y + midRocks[i].hitbox_y1) < (playerShots[j].pos_y + FIX32(4)) &&
						(midRocks[i].pos_y + midRocks[i].hitbox_y2) > (playerShots[j].pos_y + FIX32(4))  )
				{
					midRocks[i].hitpoints -=1;
					if( midRocks[i].hitpoints == 0 ) {
						midRocks[i].active = FALSE;
						SPR_setVisibility( midRocks[i].sprite, HIDDEN);
					}
					playerShots[j].active = FALSE;
					SPR_setVisibility( playerShots[j].sprite, HIDDEN);
					addExplosion(  midRocks[i].pos_x, midRocks[i].pos_y );
				}
			}
		}
	}

	for( u16 i=0; i < MAX_SMALL_ROCKS; ++i ) {
		if( smallRocks[i].active == TRUE ) {
			// check if ship has hit
			if( (smallRocks[i].pos_x + smallRocks[i].hitbox_x1) < (player.pos_x + player.hitbox_x2) &&
					(smallRocks[i].pos_x + smallRocks[i].hitbox_x2) > (player.pos_x + player.hitbox_x1) &&
					(smallRocks[i].pos_y + smallRocks[i].hitbox_y1) < (player.pos_y + player.hitbox_y2) &&
					(smallRocks[i].pos_y + smallRocks[i].hitbox_y2) > (player.pos_y + player.hitbox_y1)  )
			{
				smallRocks[i].hitpoints -=1;
				if( smallRocks[i].hitpoints == 0 ) {
					smallRocks[i].active = FALSE;
					SPR_setVisibility( smallRocks[i].sprite, HIDDEN);
					addExplosion(  smallRocks[i].pos_x, smallRocks[i].pos_y );
				}
				//SPR_setVisibility( player.sprite, HIDDEN );
			}

			for( u16 j=0; j < MAX_PLAYER_SHOTS; ++j ) {
				if(
						playerShots[j].active == TRUE &&
						(smallRocks[i].pos_x + smallRocks[i].hitbox_x1) < (playerShots[j].pos_x + FIX32(4)) &&
						(smallRocks[i].pos_x + smallRocks[i].hitbox_x2) > (playerShots[j].pos_x + FIX32(4)) &&
						(smallRocks[i].pos_y + smallRocks[i].hitbox_y1) < (playerShots[j].pos_y + FIX32(4)) &&
						(smallRocks[i].pos_y + smallRocks[i].hitbox_y2) > (playerShots[j].pos_y + FIX32(4))  )
				{
					smallRocks[i].hitpoints -=1;
					if( smallRocks[i].hitpoints == 0 ) {
						smallRocks[i].active = FALSE;
						SPR_setVisibility( smallRocks[i].sprite, HIDDEN);
					}
					playerShots[j].active = FALSE;
					SPR_setVisibility( playerShots[j].sprite, HIDDEN);
					addExplosion(  smallRocks[i].pos_x, smallRocks[i].pos_y );
				}
			}
		}
	}

	for( u16 i=0; i < MAX_SMALL_ROCKS; ++i ) {
		if( xsRocks[i].active == TRUE ) {
			// check if ship has hit
			if( (xsRocks[i].pos_x + xsRocks[i].hitbox_x1) < (player.pos_x + player.hitbox_x2) &&
					(xsRocks[i].pos_x + xsRocks[i].hitbox_x2) > (player.pos_x + player.hitbox_x1) &&
					(xsRocks[i].pos_y + xsRocks[i].hitbox_y1) < (player.pos_y + player.hitbox_y2) &&
					(xsRocks[i].pos_y + xsRocks[i].hitbox_y2) > (player.pos_y + player.hitbox_y1)  )
			{
				xsRocks[i].hitpoints -=1;
				if( xsRocks[i].hitpoints == 0 ) {
					xsRocks[i].active = FALSE;
					SPR_setVisibility( xsRocks[i].sprite, HIDDEN);
					addExplosion(  xsRocks[i].pos_x, xsRocks[i].pos_y );
				}
				//SPR_setVisibility( player.sprite, HIDDEN );
			}

			for( u16 j=0; j < MAX_PLAYER_SHOTS; ++j ) {
				if(
						playerShots[j].active == TRUE &&
						(xsRocks[i].pos_x + xsRocks[i].hitbox_x1) < (playerShots[j].pos_x + FIX32(4)) &&
						(xsRocks[i].pos_x + xsRocks[i].hitbox_x2) > (playerShots[j].pos_x + FIX32(4)) &&
						(xsRocks[i].pos_y + xsRocks[i].hitbox_y1) < (playerShots[j].pos_y + FIX32(4)) &&
						(xsRocks[i].pos_y + xsRocks[i].hitbox_y2) > (playerShots[j].pos_y + FIX32(4))  )
				{
					xsRocks[i].hitpoints -=1;
					if( xsRocks[i].hitpoints == 0 ) {
						xsRocks[i].active = FALSE;
						SPR_setVisibility( xsRocks[i].sprite, HIDDEN);
					}
					playerShots[j].active = FALSE;
					SPR_setVisibility( playerShots[j].sprite, HIDDEN);
					addExplosion(  xsRocks[i].pos_x, xsRocks[i].pos_y );
				}
			}
		}
	}


}





void createPlayerShots() {
	fix32 xpos = FIX32(-16);
	fix32 ypos = FIX32(-16);

	for( u16 i=0; i < MAX_PLAYER_SHOTS; ++i ) {
		playerShots[i].pos_x = xpos;
		playerShots[i].pos_y = ypos;
		playerShots[i].vel_x = FIX32(0.0);
		playerShots[i].vel_y = FIX32(0.0);
		playerShots[i].active = FALSE;
		playerShots[i].hitbox_x1 = FIX32(3);
		playerShots[i].hitbox_y1 = FIX32(3);
		playerShots[i].hitbox_x2 = FIX32(4);
		playerShots[i].hitbox_y2 = FIX32(4);

		playerShots[i].sprite = SPR_addSprite( &shot, xpos, ypos, TILE_ATTR( PAL0, 0, FALSE, FALSE ));
		SPR_setAnim( playerShots[i].sprite, 2 );
	}

}

void createRocks() {


	for( u16 i=0; i < MAX_ROCKS; ++i ) {
		rocks[i].pos_x = FIX32(random()%(MAP_WIDTH-32) + i );
		rocks[i].pos_y = FIX32(random()%(MAP_HEIGHT-32)+ i);
		u16 rot = random() % 16;
		fix32 vel = FIX32(0.3);
		rocks[i].vel_x = fix32Mul( vel, deltaX[rot]  );
		rocks[i].vel_y = fix32Mul( vel, deltaY[rot]  );
		rocks[i].active = TRUE;
		rocks[i].hitbox_x1 = FIX32(2);
		rocks[i].hitbox_y1 = FIX32(2);
		rocks[i].hitbox_x2 = FIX32(30);
		rocks[i].hitbox_y2 = FIX32(30);
		rocks[i].hitpoints = 3;

		rocks[i].sprite = SPR_addSprite( &rock, -32, -32, TILE_ATTR( PAL3, 0, FALSE, FALSE ));
		SPR_setAnim( rocks[i].sprite, i%4 );
	}
}

// COPY PASTE IS THE WORST FORM OF REUSE:   MOVE TO A GENERIC GameEntity struct next 
void createMidRocks() {
	for( u16 i=0; i < MAX_MID_ROCKS; ++i ) {
		midRocks[i].pos_x = FIX32(random()%(MAP_WIDTH-32) + i );
		midRocks[i].pos_y = FIX32(random()%(MAP_HEIGHT-32)+ i);
		u16 rot = random() % 16;
		fix32 vel = FIX32(0.5);
		midRocks[i].vel_x = fix32Mul( vel, deltaX[rot]  );
		midRocks[i].vel_y = fix32Mul( vel, deltaY[rot]  );
		midRocks[i].active = TRUE;
		midRocks[i].hitbox_x1 = FIX32(2);
		midRocks[i].hitbox_y1 = FIX32(2);
		midRocks[i].hitbox_x2 = FIX32(22);
		midRocks[i].hitbox_y2 = FIX32(22);
		midRocks[i].hitpoints = 2;

		midRocks[i].sprite = SPR_addSprite( &mid_rock, -32, -32, TILE_ATTR( PAL3, 0, FALSE, FALSE ));
		SPR_setAnim( midRocks[i].sprite, i%4 );
	}
}

void createSmallRocks() {
	for( u16 i=0; i < MAX_SMALL_ROCKS; ++i ) {
		smallRocks[i].pos_x = FIX32(random()%(MAP_WIDTH-32) + i );
		smallRocks[i].pos_y = FIX32(random()%(MAP_HEIGHT-32)+ i);
		u16 rot = random() % 16;
		fix32 vel = FIX32(0.7);
		smallRocks[i].vel_x = fix32Mul( vel, deltaX[rot]  );
		smallRocks[i].vel_y = fix32Mul( vel, deltaY[rot]  );
		smallRocks[i].active = TRUE;
		smallRocks[i].hitbox_x1 = FIX32(0);
		smallRocks[i].hitbox_y1 = FIX32(0);
		smallRocks[i].hitbox_x2 = FIX32(16);
		smallRocks[i].hitbox_y2 = FIX32(16);
		smallRocks[i].hitpoints = 1;

		smallRocks[i].sprite = SPR_addSprite( &small_rock, -32, -32, TILE_ATTR( PAL3, 0, FALSE, FALSE ));
		SPR_setAnim( smallRocks[i].sprite, i%4 );
	}
}

void createExtraSmallRocks() {
	for( u16 i=0; i < MAX_XS_ROCKS; ++i ) {
		xsRocks[i].pos_x = FIX32(random()%(MAP_WIDTH-32) + i );
		xsRocks[i].pos_y = FIX32(random()%(MAP_HEIGHT-32)+ i);
		u16 rot = random() % 16;
		fix32 vel = FIX32(0.9);
		xsRocks[i].vel_x = fix32Mul( vel, deltaX[rot]  );
		xsRocks[i].vel_y = fix32Mul( vel, deltaY[rot]  );
		xsRocks[i].active = TRUE;
		xsRocks[i].hitbox_x1 = FIX32(0);
		xsRocks[i].hitbox_y1 = FIX32(0);
		xsRocks[i].hitbox_x2 = FIX32(8);
		xsRocks[i].hitbox_y2 = FIX32(8);
		xsRocks[i].hitpoints = 1;

		xsRocks[i].sprite = SPR_addSprite( &xs_rock, -32, -32, TILE_ATTR( PAL3, 0, FALSE, FALSE ));
		SPR_setAnim( xsRocks[i].sprite, i%4 );
	}
}

void createEnemies() {


	for( u16 i=0; i < MAX_ENEMIES; ++i ) {
		enemies[i].pos_x = FIX32(random()%(MAP_WIDTH-32) + i );
		enemies[i].pos_y = FIX32(random()%(MAP_HEIGHT-32)+ i);
		u16 rot = random() % 16;
		fix32 vel = FIX32(1.0);
		enemies[i].vel_x = fix32Mul( vel, deltaX[rot]  );
		enemies[i].vel_y = fix32Mul( vel, deltaY[rot]  );
		enemies[i].active = TRUE;
		enemies[i].hitbox_x1 = FIX32(2); 
		enemies[i].hitbox_y1 = FIX32(2);
		enemies[i].hitbox_x2 = FIX32(30);
		enemies[i].hitbox_y2 = FIX32(30);
		enemies[i].hitpoints = 5;

		enemies[i].sprite = SPR_addSprite( &ufo, -32, -32, TILE_ATTR( PAL3, 0, FALSE, FALSE ));
		SPR_setAnim( enemies[i].sprite, 0 );
	}
}


static void createExplosions() {
	fix32 xpos =  FIX32(0);
	fix32 ypos =  FIX32(264);

	for( u16 i=0; i < MAX_EXPLOSIONS; ++i ) {
		explosions[i].pos_x = xpos;
		explosions[i].pos_y = ypos;
		explosions[i].vel_x = FIX32(0);
		explosions[i].vel_y = FIX32(0);
		explosions[i].active = FALSE;
		explosions[i].hitbox_x1 = FIX32(0);
		explosions[i].hitbox_y1 = FIX32(0);
		explosions[i].hitbox_x2 = FIX32(0);
		explosions[i].hitbox_y2 = FIX32(0);

		explosions[i].sprite = SPR_addSprite( &explosion, fix32ToInt(xpos), fix32ToInt(ypos), TILE_ATTR( PAL0, 0, FALSE, FALSE ));
		SPR_setAnim( explosions[i].sprite, i % 4 );

		SPR_setVisibility( explosions[i].sprite, HIDDEN );
		SPR_setDepth( explosions[i].sprite, SPR_MIN_DEPTH );
	}

}



int main(bool hard) {

	// clear 
	memset( playerShots, 0, sizeof(playerShots) );
	memset( enemies, 0, sizeof(enemies) );
	memset( enemyShots, 0, sizeof(enemyShots) );
	memset( rocks, 0, sizeof(rocks) );
	memset( smallRocks, 0, sizeof(smallRocks) );


	// setup sounds
	XGM_setPCM(  SND_LASER, laser_snd, sizeof(laser_snd));
	XGM_setPCM(  SND_EXPLOSION, explosion_snd, sizeof(explosion_snd));


	// Screen init
	VDP_setScreenWidth320();

	PAL_setPalette( PAL0, shot_pal.data, CPU);
	PAL_setPalette( PAL1, plane_pal.data, CPU);
	PAL_setPalette( PAL2, ship_pal.data, CPU);
	PAL_setPalette( PAL3, rock_pal.data, CPU);

	// Load the plane tiles into VRAM
	int ind = TILE_USER_INDEX;
	VDP_loadTileSet( &plane_a_tileset, ind, DMA );

	// init background map
	map_a = MAP_create( &plane_a_map, BG_A, TILE_ATTR_FULL( PAL1, FALSE, FALSE, FALSE, ind ) );
	camPosX = MAP_WIDTH/2 - SCR_WIDTH/2;
	camPosY = MAP_HEIGHT/2 - SCR_HEIGHT/2;
	MAP_scrollTo( map_a, camPosX, camPosY );


	// Init sprite engine with defaults
	SPR_initEx(900);
	player.pos_x = FIX32( MAP_WIDTH/2 - 12 );
	player.pos_y = FIX32( MAP_HEIGHT/2 - 12 );
	player.sprite = SPR_addSprite( &ship, fix32ToInt(player.pos_x) - camPosX, fix32ToInt(player.pos_y) - camPosY, TILE_ATTR(PAL2, 0, FALSE, FALSE ));
	player.hitbox_x1 = FIX32(5);
	player.hitbox_y1 = FIX32(5);
	player.hitbox_x2 = FIX32(19);
	player.hitbox_y2 = FIX32(19);
	playerRotation = MIN_ROTATION_INDEX;
	SPR_setAnim( player.sprite, fix32ToInt(playerRotation ) );

	// setup player motion offset.
	deltaX[0] = FIX32( 0.000000 );
	deltaY[0] = FIX32( -2.000000 );
	deltaX[1] = FIX32( -0.765367 );
	deltaY[1] = FIX32( -1.847759 );
	deltaX[2] = FIX32( -1.414214 );
	deltaY[2] = FIX32( -1.414214 );
	deltaX[3] = FIX32( -1.847759 );
	deltaY[3] = FIX32( -0.765367 );
	deltaX[4] = FIX32( -2.000000 );
	deltaY[4] = FIX32( -0.000000 );
	deltaX[5] = FIX32( -1.847759 );
	deltaY[5] = FIX32( 0.765367 );
	deltaX[6] = FIX32( -1.414214 );
	deltaY[6] = FIX32( 1.414214 );
	deltaX[7] = FIX32( -0.765367 );
	deltaY[7] = FIX32( 1.847759 );
	deltaX[8] = FIX32( -0.000000 );
	deltaY[8] = FIX32( 2.000000 );
	deltaX[9] = FIX32( 0.765367 );
	deltaY[9] = FIX32( 1.847759 );
	deltaX[10] = FIX32( 1.414214 );
	deltaY[10] = FIX32( 1.414214 );
	deltaX[11] = FIX32( 1.847759 );
	deltaY[11] = FIX32( 0.765367 );
	deltaX[12] = FIX32( 2.000000 );
	deltaY[12] = FIX32( 0.000000 );
	deltaX[13] = FIX32( 1.847759 );
	deltaY[13] = FIX32( -0.765367 );
	deltaX[14] = FIX32( 1.414214 );
	deltaY[14] = FIX32( -1.414214 );
	deltaX[15] = FIX32( 0.765367 );
	deltaY[15] = FIX32( -1.847759 );

	createExplosions();
	createPlayerShots();
	createRocks();
	createMidRocks();
	createSmallRocks();
	createExtraSmallRocks();
	createEnemies();

	JOY_setEventHandler( &inputCallback );

	while(TRUE) {

		handleInput();

		update();

		checkCollisions();

		SPR_update();

		SYS_doVBlankProcess();
	}	
	return 0;
}

