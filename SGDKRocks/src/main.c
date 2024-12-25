#include <genesis.h>
#include "resources.h"




/////////////////////////////////////////////////////////////////////////////////
// define sound constants
#define SND_LASER      64
#define SND_EXPLOSION   65


/////////////////////////////////////////////////////////////////////////////////
// define player constants
#define PLAYER_FRAME_COUNT  32
#define PLAYER_WIDTH        24
#define PLAYER_HEIGHT       24
#define PLAYER_SHOT_WIDTH   8 
#define PLAYER_SHOT_HEIGHT  8 
#define MAX_PLAYER_SHOTS    4

/////////////////////////////////////////////////////////////////////////////////
// enemy constants

#define MAX_OBJS            65
#define MAX_EXPLOSIONS      6

/////////////////////////////////////////////////////////////////////////////////
// map/world constants
#define MAP_WIDTH           640
#define MAP_HEIGHT          640

#define SCR_WIDTH           320
#define SCR_HEIGHT          224 

#define CAM_BOUNDARY        90

#define PI_1 FIX16(3.142)
#define PI_2 FIX16(1.571)  

// map variables
Map *map_a;
s16 camPosX; // relative to total world map
s16 camPosY; // relative to total world map

/*
typedef struct
{
		Sprite *sprite;
		fix16 pos_x;
		fix16 pos_y;
		fix16 vel_x;
		fix16 vel_y;

		s16 hitbox_x1;
		s16 hitbox_y1;
		s16 hitbox_x2;
		s16 hitbox_y2;

		bool active;
		u16 ticks;
		u16 hitpoints;
} CP_SPRITE;
*/


/////////////////////////////////////////////////////////////////////////////////
// player variables
/*
CP_SPRITE player;
fix16 playerRotation;
bool doPlayerUpdate;
fix16 acc = FIX16(0.10);
u8 doDec = 0;
fix16 deltaX[PLAYER_FRAME_COUNT];
fix16 deltaY[PLAYER_FRAME_COUNT];
CP_SPRITE playerShots[MAX_PLAYER_SHOTS];
*/



/////////////////////////////////////////////////////////////////////////////////
// Moving Objs
/*
CP_SPRITE enemies[MAX_ENEMIES];
CP_SPRITE enemyShots[MAX_ENEMY_SHOTS];
CP_SPRITE rocks[MAX_ROCKS];
CP_SPRITE midRocks[MAX_MID_ROCKS];
CP_SPRITE smallRocks[MAX_SMALL_ROCKS];
CP_SPRITE xsRocks[MAX_XS_ROCKS];
CP_SPRITE explosions[MAX_EXPLOSIONS];
*/






u8 currentExplosion = 0;

static void addExplosion(fix16 pos_x, fix16 pos_y)
{
		if (explosions[currentExplosion].active == FALSE)
		{
				// use it
				explosions[currentExplosion].pos_x = pos_x;
				explosions[currentExplosion].pos_y = pos_y;

				explosions[currentExplosion].active = TRUE;
				explosions[currentExplosion].ticks = 0;
				SPR_setVisibility(explosions[currentExplosion].sprite, VISIBLE);
				SPR_setPosition(explosions[currentExplosion].sprite, fix16ToInt(explosions[currentExplosion].pos_x), fix16ToInt(explosions[currentExplosion].pos_y));

				XGM_startPlayPCM(SND_EXPLOSION, 10, SOUND_PCM_CH3);

				// point to next one
				++currentExplosion;
				if (currentExplosion >= MAX_EXPLOSIONS)
				{
						currentExplosion = 0;
				}
		}
}

void updateCameraPos()
{
		s16 px = fix16ToInt(player.pos_x);
		s16 py = fix16ToInt(player.pos_y);

		s16 playerScreenX = px - camPosX;
		s16 playerScreenY = py - camPosY;

		// adjust new camera position based on player position
		s16 newCamX;
		s16 newCamY;
		if (playerScreenX > SCR_WIDTH - CAM_BOUNDARY - PLAYER_WIDTH)
		{
				newCamX = px - (SCR_WIDTH - CAM_BOUNDARY - PLAYER_WIDTH);
		}
		else if (playerScreenX < CAM_BOUNDARY)
		{
				newCamX = px - CAM_BOUNDARY;
		}
		else
		{
				newCamX = camPosX; // no change
		}

		if (playerScreenY > SCR_HEIGHT - CAM_BOUNDARY - PLAYER_HEIGHT)
		{
				newCamY = py - (SCR_HEIGHT - CAM_BOUNDARY - PLAYER_HEIGHT);
		}
		else if (playerScreenY < CAM_BOUNDARY)
		{
				newCamY = py - CAM_BOUNDARY;
		}
		else
		{
				newCamY = camPosY; // no change
		}

		// handle edges
		if (newCamX < 0)
		{
				newCamX = 0;
		}
		else if (newCamX > (MAP_WIDTH - SCR_WIDTH))
		{
				newCamX = MAP_WIDTH - SCR_WIDTH;
		}

		if (newCamY < 0)
		{
				newCamY = 0;
		}
		else if (newCamY > (MAP_HEIGHT - SCR_HEIGHT))
		{
				newCamY = MAP_HEIGHT - SCR_HEIGHT;
		}

		camPosX = newCamX;
		camPosY = newCamY;
		MAP_scrollTo(map_a, camPosX, camPosY);
}

void inputCallback(u16 joy, u16 changed, u16 state)
{
		// create a shot if available
		if (changed & state & BUTTON_A)
		{
				for (u16 i = 0; i < MAX_PLAYER_SHOTS; ++i)
				{
						if (playerShots[i].active == FALSE)
						{
							// create a new one
							XGM_startPlayPCM(SND_LASER, 1, SOUND_PCM_CH2);

							u16 rot = fix16ToInt(playerRotation);
							playerShots[i].pos_x = (player.pos_x+ FIX16((PLAYER_WIDTH - PLAYER_SHOT_WIDTH) / 2)) + fix16Mul(deltaX[rot], FIX16(2.0));
							playerShots[i].pos_y = (player.pos_y+ FIX16((PLAYER_HEIGHT - PLAYER_SHOT_WIDTH) / 2)) + fix16Mul(deltaY[rot], FIX16(2.0));
							playerShots[i].vel_x = fix16Mul(deltaX[rot], FIX16(2.0));
							playerShots[i].vel_y = fix16Mul(deltaY[rot], FIX16(2.0));
							playerShots[i].active = TRUE;
							playerShots[i].ticks = 0;
							break;
						}
				}
		}
}

void handleInput()
{
		u16 value = JOY_readJoypad(JOY_1);

		if (value & BUTTON_LEFT)
		{
				playerRotation += FIX16(0.25);
				if (playerRotation > MAX_ROTATION_INDEX)
				{
						playerRotation = MIN_ROTATION_INDEX;
				}
				int rot = fix16ToInt(playerRotation);
				SPR_setAnim(player.sprite, rot);
		}
		else if (value & BUTTON_RIGHT)
		{
				playerRotation -= FIX16(0.25);
				if (playerRotation < MIN_ROTATION_INDEX)
				{
						playerRotation = MAX_ROTATION_INDEX;
				}
				int rot = fix16ToInt(playerRotation);
				SPR_setAnim(player.sprite, rot);
		}

		if (value & BUTTON_UP)
		{
				// doPlayerUpdate = TRUE;
				int rot = fix16ToInt(playerRotation);
				fix16 temp_x = (player.vel_x+ fix16Mul(acc, deltaX[rot]));
				fix16 temp_y = (player.vel_y+ fix16Mul(acc, deltaY[rot]));
				fix16 speedsqr =(fix16Mul(temp_x, temp_x)+ fix16Mul(temp_y, temp_y));

				if (speedsqr < FIX16(4))
				{ // square of 1.2 == 1.44
						player.vel_x = temp_x;
						player.vel_y = temp_y;
				}
				else
				{
						// get direction and rescale speed for it.
						u16 dir = ApproxAtan2(temp_y, temp_x);
						fix16 s = sinFix32(dir);
						fix16 c = cosFix32(dir);
						player.vel_x = fix16Mul(FIX16(2), c);
						player.vel_y = fix16Mul(FIX16(2), s);
				}
		}
		else
		{
			++doDec;
			if( doDec == 10) {
				doDec = 0;
				fix16 speedsqr = (fix16Mul(player.vel_x, player.vel_x)+ fix16Mul(player.vel_y, player.vel_y));
				u16 dir = ApproxAtan2(player.vel_y, player.vel_x);
				fix16 s = sinFix32(dir);
				fix16 c = cosFix32(dir);
				fix16 speed = fix16Sqrt( fix16ToFix16(speedsqr));
        speed =  speed- FIX16(0.1);
				if( speed < FIX16(0.1)){
					player.vel_x = FIX16(0);
					player.vel_y = FIX16(0);
				} else {
					player.vel_x = fix16Mul(fix16ToFix32(speed), c);
					player.vel_y = fix16Mul(fix16ToFix32(speed), s);
				}
			}
		}
}

void update()
{
		// if( doPlayerUpdate == TRUE ) {
		player.pos_x = (player.pos_x+ player.vel_x);
		player.pos_y = (player.pos_y+ player.vel_y);

		if (player.pos_x < FIX16(-6.0))
		{
				player.pos_x = FIX16(-6.0);
		}
		else if (player.pos_x > FIX16(MAP_WIDTH - PLAYER_WIDTH + 6))
		{
				player.pos_x = FIX16(MAP_WIDTH - PLAYER_WIDTH + 6);
		}

		if (player.pos_y < FIX16(-6))
		{
				player.pos_y = FIX16(-6);
		}
		else if (player.pos_y > FIX16(MAP_HEIGHT - PLAYER_HEIGHT + 6))
		{
				player.pos_y = FIX16(MAP_HEIGHT - PLAYER_HEIGHT + 6);
		}

		//}

		// shots
		for (u16 i = 0; i < MAX_PLAYER_SHOTS; ++i)
		{
				if (playerShots[i].active == TRUE)
				{
						playerShots[i].pos_x += playerShots[i].vel_x;
						playerShots[i].pos_y += playerShots[i].vel_y;
						s16 x = fix16ToInt(playerShots[i].pos_x) - camPosX;
						s16 y = fix16ToInt(playerShots[i].pos_y) - camPosY;
						if (x >= 0 && x < SCR_WIDTH && y >= 0 && y < SCR_HEIGHT)
						{
							SPR_setVisibility(playerShots[i].sprite, VISIBLE);
							SPR_setPosition(playerShots[i].sprite, fix16ToInt(playerShots[i].pos_x) - camPosX, fix16ToInt(playerShots[i].pos_y) - camPosY);
						}
						else
						{
							playerShots[i].active = FALSE;
							SPR_setVisibility(playerShots[i].sprite, HIDDEN);
						}
				}
				else
				{
						SPR_setVisibility(playerShots[i].sprite, HIDDEN);
				}
		}

		// COPY PASTE IS THE WORST FORM OF REUSE:   MOVE TO A GENERIC GameEntity struct next
		for (u16 i = 0; i < MAX_ROCKS; ++i)
		{
				if (rocks[i].active == TRUE)
				{
						rocks[i].pos_x += rocks[i].vel_x;
						if (rocks[i].pos_x < FIX16(-32))
						{
							rocks[i].pos_x = FIX16(MAP_WIDTH);
						}
						else if (rocks[i].pos_x > FIX16(MAP_WIDTH))
						{
							rocks[i].pos_x = FIX16(-32);
						}

						rocks[i].pos_y += rocks[i].vel_y;
						if (rocks[i].pos_y < FIX16(-32))
						{
							rocks[i].pos_y = FIX16(MAP_HEIGHT);
						}
						else if (rocks[i].pos_y > FIX16(MAP_HEIGHT))
						{
							rocks[i].pos_y = FIX16(-32);
						}

						s16 x = fix16ToInt(rocks[i].pos_x) - camPosX;
						s16 y = fix16ToInt(rocks[i].pos_y) - camPosY;
						if (x >= -32 && x < SCR_WIDTH && y >= -32 && y < SCR_HEIGHT)
						{
							SPR_setVisibility(rocks[i].sprite, VISIBLE);
							SPR_setPosition(rocks[i].sprite, fix16ToInt(rocks[i].pos_x) - camPosX, fix16ToInt(rocks[i].pos_y) - camPosY);
						}
						else
						{
							SPR_setVisibility(rocks[i].sprite, HIDDEN);
						}
				}
				else
				{
						SPR_setVisibility(rocks[i].sprite, HIDDEN);
				}
		}

		for (u16 i = 0; i < MAX_ROCKS; ++i)
		{
				if (midRocks[i].active == TRUE)
				{
						midRocks[i].pos_x += midRocks[i].vel_x;
						if (midRocks[i].pos_x < FIX16(-32))
						{
							midRocks[i].pos_x = FIX16(MAP_WIDTH);
						}
						else if (midRocks[i].pos_x > FIX16(MAP_WIDTH))
						{
							midRocks[i].pos_x = FIX16(-32);
						}

						midRocks[i].pos_y += midRocks[i].vel_y;
						if (midRocks[i].pos_y < FIX16(-32))
						{
							midRocks[i].pos_y = FIX16(MAP_HEIGHT);
						}
						else if (midRocks[i].pos_y > FIX16(MAP_HEIGHT))
						{
							midRocks[i].pos_y = FIX16(-32);
						}

						s16 x = fix16ToInt(midRocks[i].pos_x) - camPosX;
						s16 y = fix16ToInt(midRocks[i].pos_y) - camPosY;
						if (x >= -32 && x < SCR_WIDTH && y >= -32 && y < SCR_HEIGHT)
						{
							SPR_setVisibility(midRocks[i].sprite, VISIBLE);
							SPR_setPosition(midRocks[i].sprite, fix16ToInt(midRocks[i].pos_x) - camPosX, fix16ToInt(midRocks[i].pos_y) - camPosY);
						}
						else
						{
							SPR_setVisibility(midRocks[i].sprite, HIDDEN);
						}
				}
				else
				{
						SPR_setVisibility(midRocks[i].sprite, HIDDEN);
				}
		}

		for (u16 i = 0; i < MAX_SMALL_ROCKS; ++i)
		{
				if (smallRocks[i].active == TRUE)
				{
						smallRocks[i].pos_x += smallRocks[i].vel_x;
						if (smallRocks[i].pos_x < FIX16(-32))
						{
							smallRocks[i].pos_x = FIX16(MAP_WIDTH);
						}
						else if (smallRocks[i].pos_x > FIX16(MAP_WIDTH))
						{
							smallRocks[i].pos_x = FIX16(-32);
						}

						smallRocks[i].pos_y += smallRocks[i].vel_y;
						if (smallRocks[i].pos_y < FIX16(-32))
						{
							smallRocks[i].pos_y = FIX16(MAP_HEIGHT);
						}
						else if (smallRocks[i].pos_y > FIX16(MAP_HEIGHT))
						{
							smallRocks[i].pos_y = FIX16(-32);
						}

						s16 x = fix16ToInt(smallRocks[i].pos_x) - camPosX;
						s16 y = fix16ToInt(smallRocks[i].pos_y) - camPosY;
						if (x >= -32 && x < SCR_WIDTH && y >= -32 && y < SCR_HEIGHT)
						{
							SPR_setVisibility(smallRocks[i].sprite, VISIBLE);
							SPR_setPosition(smallRocks[i].sprite, fix16ToInt(smallRocks[i].pos_x) - camPosX, fix16ToInt(smallRocks[i].pos_y) - camPosY);
						}
						else
						{
							SPR_setVisibility(smallRocks[i].sprite, HIDDEN);
						}
				}
				else
				{
						SPR_setVisibility(smallRocks[i].sprite, HIDDEN);
				}
		}

		for (u16 i = 0; i < MAX_SMALL_ROCKS; ++i)
		{
				if (xsRocks[i].active == TRUE)
				{
						xsRocks[i].pos_x += xsRocks[i].vel_x;
						if (xsRocks[i].pos_x < FIX16(-32))
						{
							xsRocks[i].pos_x = FIX16(MAP_WIDTH);
						}
						else if (xsRocks[i].pos_x > FIX16(MAP_WIDTH))
						{
							xsRocks[i].pos_x = FIX16(-32);
						}

						xsRocks[i].pos_y += xsRocks[i].vel_y;
						if (xsRocks[i].pos_y < FIX16(-32))
						{
							xsRocks[i].pos_y = FIX16(MAP_HEIGHT);
						}
						else if (xsRocks[i].pos_y > FIX16(MAP_HEIGHT))
						{
							xsRocks[i].pos_y = FIX16(-32);
						}

						s16 x = fix16ToInt(xsRocks[i].pos_x) - camPosX;
						s16 y = fix16ToInt(xsRocks[i].pos_y) - camPosY;
						if (x >= -32 && x < SCR_WIDTH && y >= -32 && y < SCR_HEIGHT)
						{
							SPR_setVisibility(xsRocks[i].sprite, VISIBLE);
							SPR_setPosition(xsRocks[i].sprite, fix16ToInt(xsRocks[i].pos_x) - camPosX, fix16ToInt(xsRocks[i].pos_y) - camPosY);
						}
						else
						{
							SPR_setVisibility(xsRocks[i].sprite, HIDDEN);
						}
				}
				else
				{
						SPR_setVisibility(xsRocks[i].sprite, HIDDEN);
				}
		}

		// UFO physics
		for (u16 i = 0; i < MAX_ENEMIES; ++i)
		{
				if (enemies[i].active == TRUE)
				{
						enemies[i].pos_x += enemies[i].vel_x;
						if (enemies[i].pos_x < FIX16(-32))
						{
							enemies[i].pos_x = FIX16(MAP_WIDTH);
						}
						else if (enemies[i].pos_x > FIX16(MAP_WIDTH))
						{
							enemies[i].pos_x = FIX16(-32);
						}

						enemies[i].pos_y += enemies[i].vel_y;
						if (enemies[i].pos_y < FIX16(-32))
						{
							enemies[i].pos_y = FIX16(MAP_HEIGHT);
						}
						else if (enemies[i].pos_y > FIX16(MAP_HEIGHT))
						{
							enemies[i].pos_y = FIX16(-32);
						}

						s16 x = fix16ToInt(enemies[i].pos_x) - camPosX;
						s16 y = fix16ToInt(enemies[i].pos_y) - camPosY;
						if (x >= -32 && x < SCR_WIDTH && y >= -32 && y < SCR_HEIGHT)
						{
							SPR_setVisibility(enemies[i].sprite, VISIBLE);
							SPR_setPosition(enemies[i].sprite, fix16ToInt(enemies[i].pos_x) - camPosX, fix16ToInt(enemies[i].pos_y) - camPosY);
						}
						else
						{
							SPR_setVisibility(enemies[i].sprite, HIDDEN);
						}
				}
				else
				{
						SPR_setVisibility(enemies[i].sprite, HIDDEN);
				}
		}

		for (u16 i = 0; i < MAX_EXPLOSIONS; ++i)
		{
				if (explosions[i].active == TRUE)
				{
						explosions[i].ticks += 1;
						if (explosions[i].ticks < 9)
						{
							//  SPR_setFrame( explosions[i].sprite, explosions[i].ticks );
							SPR_setPosition(explosions[i].sprite, fix16ToInt(explosions[i].pos_x) - camPosX, fix16ToInt(explosions[i].pos_y) - camPosY);
							SPR_setAnimAndFrame(explosions[i].sprite, i % 4, explosions[i].ticks);
						}
						else
						{
							explosions[i].active = FALSE;
							SPR_setVisibility(explosions[i].sprite, HIDDEN);
						}
				}
		}

		updateCameraPos();
		SPR_setPosition(player.sprite, fix16ToInt(player.pos_x) - camPosX, fix16ToInt(player.pos_y) - camPosY);
}

static void checkCollisions()
{
		for (u16 i = 0; i < MAX_ENEMIES; ++i)
		{
				if (enemies[i].active == TRUE)
				{
						// check if ship has hit
						if ((enemies[i].pos_x + enemies[i].hitbox_x1) < (player.pos_x + player.hitbox_x2) &&
								(enemies[i].pos_x + enemies[i].hitbox_x2) > (player.pos_x + player.hitbox_x1) &&
								(enemies[i].pos_y + enemies[i].hitbox_y1) < (player.pos_y + player.hitbox_y2) &&
								(enemies[i].pos_y + enemies[i].hitbox_y2) > (player.pos_y + player.hitbox_y1))
						{
							enemies[i].hitpoints -= 1;
							if (enemies[i].hitpoints == 0)
							{
								enemies[i].active = FALSE;
								SPR_setVisibility(enemies[i].sprite, HIDDEN);
								addExplosion(enemies[i].pos_x, enemies[i].pos_y);
							}
							// SPR_setVisibility( player.sprite, HIDDEN );
						}

						for (u16 j = 0; j < MAX_PLAYER_SHOTS; ++j)
						{
							if (
									playerShots[j].active == TRUE &&
									(enemies[i].pos_x + enemies[i].hitbox_x1) < (playerShots[j].pos_x + FIX16(4)) &&
									(enemies[i].pos_x + enemies[i].hitbox_x2) > (playerShots[j].pos_x + FIX16(4)) &&
									(enemies[i].pos_y + enemies[i].hitbox_y1) < (playerShots[j].pos_y + FIX16(4)) &&
									(enemies[i].pos_y + enemies[i].hitbox_y2) > (playerShots[j].pos_y + FIX16(4)))
							{
								enemies[i].hitpoints -= 1;
								if (enemies[i].hitpoints == 0)
								{
									enemies[i].active = FALSE;
									SPR_setVisibility(enemies[i].sprite, HIDDEN);
								}
								playerShots[j].active = FALSE;
								SPR_setVisibility(playerShots[j].sprite, HIDDEN);
								addExplosion(enemies[i].pos_x, enemies[i].pos_y);
							}
						}
				}
		}

		for (u16 i = 0; i < MAX_ROCKS; ++i)
		{
				if (rocks[i].active == TRUE)
				{
						// check if ship has hit
						if ((rocks[i].pos_x + rocks[i].hitbox_x1) < (player.pos_x + player.hitbox_x2) &&
								(rocks[i].pos_x + rocks[i].hitbox_x2) > (player.pos_x + player.hitbox_x1) &&
								(rocks[i].pos_y + rocks[i].hitbox_y1) < (player.pos_y + player.hitbox_y2) &&
								(rocks[i].pos_y + rocks[i].hitbox_y2) > (player.pos_y + player.hitbox_y1))
						{
							rocks[i].hitpoints -= 1;
							if (rocks[i].hitpoints == 0)
							{
								rocks[i].active = FALSE;
								SPR_setVisibility(rocks[i].sprite, HIDDEN);
								addExplosion(rocks[i].pos_x, rocks[i].pos_y);
							}
							// SPR_setVisibility( player.sprite, HIDDEN );
						}

						for (u16 j = 0; j < MAX_PLAYER_SHOTS; ++j)
						{
							if (
									playerShots[j].active == TRUE &&
									(rocks[i].pos_x + rocks[i].hitbox_x1) < (playerShots[j].pos_x + FIX16(4)) &&
									(rocks[i].pos_x + rocks[i].hitbox_x2) > (playerShots[j].pos_x + FIX16(4)) &&
									(rocks[i].pos_y + rocks[i].hitbox_y1) < (playerShots[j].pos_y + FIX16(4)) &&
									(rocks[i].pos_y + rocks[i].hitbox_y2) > (playerShots[j].pos_y + FIX16(4)))
							{
								rocks[i].hitpoints -= 1;
								if (rocks[i].hitpoints == 0)
								{
									rocks[i].active = FALSE;
									SPR_setVisibility(rocks[i].sprite, HIDDEN);
								}
								playerShots[j].active = FALSE;
								SPR_setVisibility(playerShots[j].sprite, HIDDEN);
								addExplosion(rocks[i].pos_x, rocks[i].pos_y);
							}
						}
				}
		}

		for (u16 i = 0; i < MAX_ROCKS; ++i)
		{
				if (midRocks[i].active == TRUE)
				{
						// check if ship has hit
						if ((midRocks[i].pos_x + midRocks[i].hitbox_x1) < (player.pos_x + player.hitbox_x2) &&
								(midRocks[i].pos_x + midRocks[i].hitbox_x2) > (player.pos_x + player.hitbox_x1) &&
								(midRocks[i].pos_y + midRocks[i].hitbox_y1) < (player.pos_y + player.hitbox_y2) &&
								(midRocks[i].pos_y + midRocks[i].hitbox_y2) > (player.pos_y + player.hitbox_y1))
						{
							midRocks[i].hitpoints -= 1;
							if (midRocks[i].hitpoints == 0)
							{
								midRocks[i].active = FALSE;
								SPR_setVisibility(midRocks[i].sprite, HIDDEN);
								addExplosion(midRocks[i].pos_x, midRocks[i].pos_y);
							}
							// SPR_setVisibility( player.sprite, HIDDEN );
						}

						for (u16 j = 0; j < MAX_PLAYER_SHOTS; ++j)
						{
							if (
									playerShots[j].active == TRUE &&
									(midRocks[i].pos_x + midRocks[i].hitbox_x1) < (playerShots[j].pos_x + FIX16(4)) &&
									(midRocks[i].pos_x + midRocks[i].hitbox_x2) > (playerShots[j].pos_x + FIX16(4)) &&
									(midRocks[i].pos_y + midRocks[i].hitbox_y1) < (playerShots[j].pos_y + FIX16(4)) &&
									(midRocks[i].pos_y + midRocks[i].hitbox_y2) > (playerShots[j].pos_y + FIX16(4)))
							{
								midRocks[i].hitpoints -= 1;
								if (midRocks[i].hitpoints == 0)
								{
									midRocks[i].active = FALSE;
									SPR_setVisibility(midRocks[i].sprite, HIDDEN);
								}
								playerShots[j].active = FALSE;
								SPR_setVisibility(playerShots[j].sprite, HIDDEN);
								addExplosion(midRocks[i].pos_x, midRocks[i].pos_y);
							}
						}
				}
		}

		for (u16 i = 0; i < MAX_SMALL_ROCKS; ++i)
		{
				if (smallRocks[i].active == TRUE)
				{
						// check if ship has hit
						if ((smallRocks[i].pos_x + smallRocks[i].hitbox_x1) < (player.pos_x + player.hitbox_x2) &&
								(smallRocks[i].pos_x + smallRocks[i].hitbox_x2) > (player.pos_x + player.hitbox_x1) &&
								(smallRocks[i].pos_y + smallRocks[i].hitbox_y1) < (player.pos_y + player.hitbox_y2) &&
								(smallRocks[i].pos_y + smallRocks[i].hitbox_y2) > (player.pos_y + player.hitbox_y1))
						{
							smallRocks[i].hitpoints -= 1;
							if (smallRocks[i].hitpoints == 0)
							{
								smallRocks[i].active = FALSE;
								SPR_setVisibility(smallRocks[i].sprite, HIDDEN);
								addExplosion(smallRocks[i].pos_x, smallRocks[i].pos_y);
							}
							// SPR_setVisibility( player.sprite, HIDDEN );
						}

						for (u16 j = 0; j < MAX_PLAYER_SHOTS; ++j)
						{
							if (
									playerShots[j].active == TRUE &&
									(smallRocks[i].pos_x + smallRocks[i].hitbox_x1) < (playerShots[j].pos_x + FIX16(4)) &&
									(smallRocks[i].pos_x + smallRocks[i].hitbox_x2) > (playerShots[j].pos_x + FIX16(4)) &&
									(smallRocks[i].pos_y + smallRocks[i].hitbox_y1) < (playerShots[j].pos_y + FIX16(4)) &&
									(smallRocks[i].pos_y + smallRocks[i].hitbox_y2) > (playerShots[j].pos_y + FIX16(4)))
							{
								smallRocks[i].hitpoints -= 1;
								if (smallRocks[i].hitpoints == 0)
								{
									smallRocks[i].active = FALSE;
									SPR_setVisibility(smallRocks[i].sprite, HIDDEN);
								}
								playerShots[j].active = FALSE;
								SPR_setVisibility(playerShots[j].sprite, HIDDEN);
								addExplosion(smallRocks[i].pos_x, smallRocks[i].pos_y);
							}
						}
				}
		}

		for (u16 i = 0; i < MAX_SMALL_ROCKS; ++i)
		{
				if (xsRocks[i].active == TRUE)
				{
						// check if ship has hit
						if ((xsRocks[i].pos_x + xsRocks[i].hitbox_x1) < (player.pos_x + player.hitbox_x2) &&
								(xsRocks[i].pos_x + xsRocks[i].hitbox_x2) > (player.pos_x + player.hitbox_x1) &&
								(xsRocks[i].pos_y + xsRocks[i].hitbox_y1) < (player.pos_y + player.hitbox_y2) &&
								(xsRocks[i].pos_y + xsRocks[i].hitbox_y2) > (player.pos_y + player.hitbox_y1))
						{
							xsRocks[i].hitpoints -= 1;
							if (xsRocks[i].hitpoints == 0)
							{
								xsRocks[i].active = FALSE;
								SPR_setVisibility(xsRocks[i].sprite, HIDDEN);
								addExplosion(xsRocks[i].pos_x, xsRocks[i].pos_y);
							}
							// SPR_setVisibility( player.sprite, HIDDEN );
						}

						for (u16 j = 0; j < MAX_PLAYER_SHOTS; ++j)
						{
							if (
									playerShots[j].active == TRUE &&
									(xsRocks[i].pos_x + xsRocks[i].hitbox_x1) < (playerShots[j].pos_x + FIX16(4)) &&
									(xsRocks[i].pos_x + xsRocks[i].hitbox_x2) > (playerShots[j].pos_x + FIX16(4)) &&
									(xsRocks[i].pos_y + xsRocks[i].hitbox_y1) < (playerShots[j].pos_y + FIX16(4)) &&
									(xsRocks[i].pos_y + xsRocks[i].hitbox_y2) > (playerShots[j].pos_y + FIX16(4)))
							{
								xsRocks[i].hitpoints -= 1;
								if (xsRocks[i].hitpoints == 0)
								{
									xsRocks[i].active = FALSE;
									SPR_setVisibility(xsRocks[i].sprite, HIDDEN);
								}
								playerShots[j].active = FALSE;
								SPR_setVisibility(playerShots[j].sprite, HIDDEN);
								addExplosion(xsRocks[i].pos_x, xsRocks[i].pos_y);
							}
						}
				}
		}
}

void createPlayerShots()
{
		fix16 xpos = FIX16(-16);
		fix16 ypos = FIX16(-16);

		for (u16 i = 0; i < MAX_PLAYER_SHOTS; ++i)
		{
				playerShots[i].pos_x = xpos;
				playerShots[i].pos_y = ypos;
				playerShots[i].vel_x = FIX16(0.0);
				playerShots[i].vel_y = FIX16(0.0);
				playerShots[i].active = FALSE;
				playerShots[i].hitbox_x1 = FIX16(3);
				playerShots[i].hitbox_y1 = FIX16(3);
				playerShots[i].hitbox_x2 = FIX16(4);
				playerShots[i].hitbox_y2 = FIX16(4);

				playerShots[i].sprite = SPR_addSprite(&shot, xpos, ypos, TILE_ATTR(PAL0, 0, FALSE, FALSE));
				SPR_setAnim(playerShots[i].sprite, 2);
		}
}

void createRocks()
{

		for (u16 i = 0; i < MAX_ROCKS; ++i)
		{
				rocks[i].pos_x = FIX16(random() % (MAP_WIDTH - 32) + i);
				rocks[i].pos_y = FIX16(random() % (MAP_HEIGHT - 32) + i);
				u16 rot = random() % 16;
				fix16 vel = FIX16(0.3);
				rocks[i].vel_x = fix16Mul(vel, deltaX[rot]);
				rocks[i].vel_y = fix16Mul(vel, deltaY[rot]);
				rocks[i].active = TRUE;
				rocks[i].hitbox_x1 = FIX16(2);
				rocks[i].hitbox_y1 = FIX16(2);
				rocks[i].hitbox_x2 = FIX16(30);
				rocks[i].hitbox_y2 = FIX16(30);
				rocks[i].hitpoints = 3;

				rocks[i].sprite = SPR_addSprite(&rock, -32, -32, TILE_ATTR(PAL3, 0, FALSE, FALSE));
				SPR_setAnim(rocks[i].sprite, i % 4);
		}
}

// COPY PASTE IS THE WORST FORM OF REUSE:   MOVE TO A GENERIC GameEntity struct next
void createMidRocks()
{
		for (u16 i = 0; i < MAX_MID_ROCKS; ++i)
		{
				midRocks[i].pos_x = FIX16(random() % (MAP_WIDTH - 32) + i);
				midRocks[i].pos_y = FIX16(random() % (MAP_HEIGHT - 32) + i);
				u16 rot = random() % 16;
				fix16 vel = FIX16(0.5);
				midRocks[i].vel_x = fix16Mul(vel, deltaX[rot]);
				midRocks[i].vel_y = fix16Mul(vel, deltaY[rot]);
				midRocks[i].active = TRUE;
				midRocks[i].hitbox_x1 = FIX16(2);
				midRocks[i].hitbox_y1 = FIX16(2);
				midRocks[i].hitbox_x2 = FIX16(22);
				midRocks[i].hitbox_y2 = FIX16(22);
				midRocks[i].hitpoints = 2;

				midRocks[i].sprite = SPR_addSprite(&mid_rock, -32, -32, TILE_ATTR(PAL3, 0, FALSE, FALSE));
				SPR_setAnim(midRocks[i].sprite, i % 4);
		}
}

void createSmallRocks()
{
		for (u16 i = 0; i < MAX_SMALL_ROCKS; ++i)
		{
				smallRocks[i].pos_x = FIX16(random() % (MAP_WIDTH - 32) + i);
				smallRocks[i].pos_y = FIX16(random() % (MAP_HEIGHT - 32) + i);
				u16 rot = random() % 16;
				fix16 vel = FIX16(0.7);
				smallRocks[i].vel_x = fix16Mul(vel, deltaX[rot]);
				smallRocks[i].vel_y = fix16Mul(vel, deltaY[rot]);
				smallRocks[i].active = TRUE;
				smallRocks[i].hitbox_x1 = FIX16(0);
				smallRocks[i].hitbox_y1 = FIX16(0);
				smallRocks[i].hitbox_x2 = FIX16(16);
				smallRocks[i].hitbox_y2 = FIX16(16);
				smallRocks[i].hitpoints = 1;

				smallRocks[i].sprite = SPR_addSprite(&small_rock, -32, -32, TILE_ATTR(PAL3, 0, FALSE, FALSE));
				SPR_setAnim(smallRocks[i].sprite, i % 4);
		}
}

void createExtraSmallRocks()
{
		for (u16 i = 0; i < MAX_XS_ROCKS; ++i)
		{
				xsRocks[i].pos_x = FIX16(random() % (MAP_WIDTH - 32) + i);
				xsRocks[i].pos_y = FIX16(random() % (MAP_HEIGHT - 32) + i);
				u16 rot = random() % 16;
				fix16 vel = FIX16(0.9);
				xsRocks[i].vel_x = fix16Mul(vel, deltaX[rot]);
				xsRocks[i].vel_y = fix16Mul(vel, deltaY[rot]);
				xsRocks[i].active = TRUE;
				xsRocks[i].hitbox_x1 = FIX16(0);
				xsRocks[i].hitbox_y1 = FIX16(0);
				xsRocks[i].hitbox_x2 = FIX16(8);
				xsRocks[i].hitbox_y2 = FIX16(8);
				xsRocks[i].hitpoints = 1;

				xsRocks[i].sprite = SPR_addSprite(&xs_rock, -32, -32, TILE_ATTR(PAL3, 0, FALSE, FALSE));
				SPR_setAnim(xsRocks[i].sprite, i % 4);
		}
}

void createEnemies()
{

		for (u16 i = 0; i < MAX_ENEMIES; ++i)
		{
				enemies[i].pos_x = FIX16(random() % (MAP_WIDTH - 32) + i);
				enemies[i].pos_y = FIX16(random() % (MAP_HEIGHT - 32) + i);
				u16 rot = random() % 16;
				fix16 vel = FIX16(1.0);
				enemies[i].vel_x = fix16Mul(vel, deltaX[rot]);
				enemies[i].vel_y = fix16Mul(vel, deltaY[rot]);
				enemies[i].active = TRUE;
				enemies[i].hitbox_x1 = FIX16(2);
				enemies[i].hitbox_y1 = FIX16(2);
				enemies[i].hitbox_x2 = FIX16(30);
				enemies[i].hitbox_y2 = FIX16(30);
				enemies[i].hitpoints = 5;

				enemies[i].sprite = SPR_addSprite(&ufo, -32, -32, TILE_ATTR(PAL3, 0, FALSE, FALSE));
				SPR_setAnim(enemies[i].sprite, 0);
		}
}

static void createExplosions()
{
		fix16 xpos = FIX16(0);
		fix16 ypos = FIX16(264);

		for (u16 i = 0; i < MAX_EXPLOSIONS; ++i)
		{
				explosions[i].pos_x = xpos;
				explosions[i].pos_y = ypos;
				explosions[i].vel_x = FIX16(0);
				explosions[i].vel_y = FIX16(0);
				explosions[i].active = FALSE;
				explosions[i].hitbox_x1 = FIX16(0);
				explosions[i].hitbox_y1 = FIX16(0);
				explosions[i].hitbox_x2 = FIX16(0);
				explosions[i].hitbox_y2 = FIX16(0);

				explosions[i].sprite = SPR_addSprite(&explosion, fix16ToInt(xpos), fix16ToInt(ypos), TILE_ATTR(PAL0, 0, FALSE, FALSE));
				SPR_setAnim(explosions[i].sprite, i % 4);

				SPR_setVisibility(explosions[i].sprite, HIDDEN);
				SPR_setDepth(explosions[i].sprite, SPR_MIN_DEPTH);
		}
}

int main(bool hard)
{

		// fix16 x = FIX16(2);
		// fix16 y = FIX16(1);
		// fix16 z = fix16Div(y, x);
		// fix16 temp = ApproxAtan(z);
		// KLog_F3x(4, "     y : ", y, " x: ", x, " atan: ", temp);
		// x = FIX16(2);
		// y = FIX16(-1);
		// z = fix16Div(y, x);
		//  temp = ApproxAtan(z);
		// KLog_F3x(4, "     y : ", y, " x: ", x, " atan: ", temp);
		// x = FIX16(-2);
		// y = FIX16(1);
		// z = fix16Div(y, x);
		//  temp = ApproxAtan(z);
		// KLog_F3x(4, "     y : ", y, " x: ", x, " atan: ", temp);
		// x = FIX16(-2);
		// y = FIX16(-1);
		// z = fix16Div(y, x);
		//  temp = ApproxAtan(z);
		// KLog_F3x(4, "     y : ", y, " x: ", x, " atan: ", temp);

		//  x = FIX16(1);
		//  y = FIX16(2);
		// z = fix16Div(y, x);
		//  temp = ApproxAtan(z);
		// KLog_F3x(4, "     y : ", y, " x: ", x, " atan: ", temp);
		// x = FIX16(-1);
		// y = FIX16(2);
		// z = fix16Div(y, x);
		//  temp = ApproxAtan(z);
		// KLog_F3x(4, "     y : ", y, " x: ", x, " atan: ", temp);
		// x = FIX16(1);
		// y = FIX16(-2);
		// z = fix16Div(y, x);
		//  temp = ApproxAtan(z);
		// KLog_F3x(4, "     y : ", y, " x: ", x, " atan: ", temp);
		// x = FIX16(-1);
		// y = FIX16(-2);
		// z = fix16Div(y, x);
		// temp = ApproxAtan(z);
		// KLog_F3x(4, "     y : ", y, " x: ", x, " atan: ", temp);

		// KLog("?---------------------");
		// x = FIX16(1);
		// y = FIX16(0);
		// temp = ApproxAtan2(y,x);
		// KLog_F3x(4, "     y : ", y, " x: ", x, " atan: ", temp);
		// x = FIX16(1);
		// y = FIX16(1);
		// temp = ApproxAtan2(y,x);
		// KLog_F3x(4, "     y : ", y, " x: ", x, " atan: ", temp);

		// x = FIX16(0);
		// y = FIX16(1);
		// temp = ApproxAtan2(y,x);
		// KLog_F3x(4, "     y : ", y, " x: ", x, " atan: ", temp);
		// x = FIX16(-1);
		// y = FIX16(1);
		// temp = ApproxAtan2(y,x);
		// KLog_F3x(4, "     y : ", y, " x: ", x, " atan: ", temp);
		// x = FIX16(-1);
		// y = FIX16(0);
		// temp = ApproxAtan2(y,x);
		// KLog_F3x(4, "     y : ", y, " x: ", x, " atan: ", temp);
		// x = FIX16(-1);
		// y = FIX16(-1);
		// temp = ApproxAtan2(y,x);
		// KLog_F3x(4, "     y : ", y, " x: ", x, " atan: ", temp);
		// x = FIX16(0);
		// y = FIX16(-1);
		// temp = ApproxAtan2(y,x);
		// KLog_F3x(4, "     y : ", y, " x: ", x, " atan: ", temp);
		// x = FIX16(1);
		// y = FIX16(-1);
		// temp = ApproxAtan2(y,x);
		// KLog_F3x(4, "     y : ", y, " x: ", x, " atan: ", temp);

		// clear
		memset(playerShots, 0, sizeof(playerShots));
		memset(enemies, 0, sizeof(enemies));
		memset(enemyShots, 0, sizeof(enemyShots));
		memset(rocks, 0, sizeof(rocks));
		memset(smallRocks, 0, sizeof(smallRocks));

		// setup sounds
		XGM_setPCM(SND_LASER, laser_snd, sizeof(laser_snd));
		XGM_setPCM(SND_EXPLOSION, explosion_snd, sizeof(explosion_snd));

		// Screen init
		VDP_setScreenWidth320();

		PAL_setPalette(PAL0, shot_pal.data, CPU);
		PAL_setPalette(PAL1, plane_pal.data, CPU);
		PAL_setPalette(PAL2, ship_pal.data, CPU);
		PAL_setPalette(PAL3, rock_pal.data, CPU);

		// Load the plane tiles into VRAM
		int ind = TILE_USER_INDEX;
		VDP_loadTileSet(&plane_a_tileset, ind, DMA);

		// init background map
		map_a = MAP_create(&plane_a_map, BG_A, TILE_ATTR_FULL(PAL1, FALSE, FALSE, FALSE, ind));
		camPosX = MAP_WIDTH / 2 - SCR_WIDTH / 2;
		camPosY = MAP_HEIGHT / 2 - SCR_HEIGHT / 2;
		MAP_scrollTo(map_a, camPosX, camPosY);

		// Init sprite engine with defaults
		SPR_initEx(900);
		player.pos_x = FIX16(MAP_WIDTH / 2 - 8);
		player.pos_y = FIX16(MAP_HEIGHT / 2 - 8);
		player.sprite = SPR_addSprite(&ship, fix16ToInt(player.pos_x) - camPosX, fix16ToInt(player.pos_y) - camPosY, TILE_ATTR(PAL2, 0, FALSE, FALSE));
		player.hitbox_x1 = FIX16(3);
		player.hitbox_y1 = FIX16(3);
		player.hitbox_x2 = FIX16(13);
		player.hitbox_y2 = FIX16(13);
		playerRotation = MIN_ROTATION_INDEX;
		// playerRotation = FIX16(12);
		SPR_setAnim(player.sprite, fix16ToInt(playerRotation));

		// setup player motion offset, with up being at rotation 0
		deltaX[0] = FIX16(0.000000);
		deltaY[0] = FIX16(-2.000000);
		deltaX[1] = FIX16(-0.765367);
		deltaY[1] = FIX16(-1.847759);
		deltaX[2] = FIX16(-1.414214);
		deltaY[2] = FIX16(-1.414214);
		deltaX[3] = FIX16(-1.847759);
		deltaY[3] = FIX16(-0.765367);
		// left
		deltaX[4] = FIX16(-2.000000);
		deltaY[4] = FIX16(-0.000000);
		deltaX[5] = FIX16(-1.847759);
		deltaY[5] = FIX16(0.765367);
		deltaX[6] = FIX16(-1.414214);
		deltaY[6] = FIX16(1.414214);
		deltaX[7] = FIX16(-0.765367);
		deltaY[7] = FIX16(1.847759);
		// down
		deltaX[8] = FIX16(-0.000000);
		deltaY[8] = FIX16(2.000000);
		deltaX[9] = FIX16(0.765367);
		deltaY[9] = FIX16(1.847759);
		deltaX[10] = FIX16(1.414214);
		deltaY[10] = FIX16(1.414214);
		deltaX[11] = FIX16(1.847759);
		deltaY[11] = FIX16(0.765367);
		// right
		deltaX[12] = FIX16(2.000000);
		deltaY[12] = FIX16(0.000000);
		deltaX[13] = FIX16(1.847759);
		deltaY[13] = FIX16(-0.765367);
		deltaX[14] = FIX16(1.414214);
		deltaY[14] = FIX16(-1.414214);
		deltaX[15] = FIX16(0.765367);
		deltaY[15] = FIX16(-1.847759);

		createExplosions();
		createPlayerShots();
		createRocks();
		createMidRocks();
		createSmallRocks();
		createExtraSmallRocks();
		createEnemies();

		JOY_setEventHandler(&inputCallback);

		while (TRUE)
		{

				handleInput();

				update();

				checkCollisions();

				SPR_update();

				SYS_doVBlankProcess();
		}
		return 0;
}
