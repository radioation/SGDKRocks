#include <genesis.h>
#include "resources.h"




/////////////////////////////////////////////////////////////////////////////////
// define sound constants
#define SND_LASER      64
#define SND_EXPLOSION   65


/////////////////////////////////////////////////////////////////////////////////
// Define player constants
#define PLAYER_WIDTH        24
#define PLAYER_HEIGHT       24
#define PLAYER_SHOT_WIDTH   8 
#define PLAYER_SHOT_HEIGHT  8 
#define SHOT_OFFSET_X       8 
#define SHOT_OFFSET_Y       8 
#define MAX_PLAYER_SHOTS    4
#define PLAYER_SHOT_TIME    50  


/////////////////////////////////////////////////////////////////////////////////
// Define enemy constants
#define MAX_OBJECTS         45
// leave 7 of the objs for UFOs and player shots  ( 4 player shots, 1 UFOS, and 2 ufo shots )
#define MAX_ROCKS           38
#define MAX_EXPLOSIONS      5

#define UFO_SLOT            38

//#define UFO_SPAWN_TIME  0x02f8
#define UFO_SPAWN_TIME  300
#define UFO_SHOT_TICKS  60 


/////////////////////////////////////////////////////////////////////////////////
// Define UFO vals
static u16 ufoTick = 0; 
fix16 ufoShotX[64];
fix16 ufoShotY[64];


/////////////////////////////////////////////////////////////////////////////////
// Define map/world constants
#define MAP_WIDTH           640
#define MAP_HEIGHT          640
// remember fix16 is -512 to 511 so using 640 requires shifting the origin a bit
#define MAP_HALF_WIDTH     320 
#define MAP_HALF_HEIGHT    320

#define SCR_WIDTH           320
#define SCR_HEIGHT          224 

#define CAM_BOUNDARY        90

int tile_ind_map = TILE_USER_INDEX;
int tile_ind_title;

/////////////////////////////////////////////////////////////////////////////////
// World/Map variables
Map *map_b;
s16 camPosX; // relative to total world map
s16 camPosY; // relative to total world map

static u8 tick = 0; // just a commn tick for everyone to use
s16 delayTicks = -1; // count down times to delay events, like rocks at the start of a level.
u16 level = 1;
bool levelStarted = false;

enum GAME_MODE {
    attract_mode,
    play_mode,
    dead_mode,
    high_score_mode // someday
};

u8 game_mode = attract_mode;


/////////////////////////////////////////////////////////////////////////////////
// Moving Objs variables
Sprite *obj_sprites[MAX_OBJECTS];
fix16 obj_speed_x[MAX_OBJECTS];
fix16 obj_speed_y[MAX_OBJECTS];
fix16 obj_pos_x[MAX_OBJECTS];
fix16 obj_pos_y[MAX_OBJECTS];
fix16 obj_hit_w[MAX_OBJECTS];
s16 obj_ticks[MAX_OBJECTS];
bool obj_live[MAX_OBJECTS];
fix16 rockVelocity = FIX16(3.0);
u8 rockCount = 0;
#define NO_TYPE 0
#define PLAYER 1
#define PLAYER_SHOT 2

#define ROCK 3 
#define MID_ROCK 4
#define SMALL_ROCK 5
#define UFO 6
#define SMALL_UFO 7
#define STAR_GESTALT 8
#define STAR_SHARD 10

u8 obj_type[MAX_OBJECTS];

s16 score_by_type[] = { 0,0,0, 20, 50, 100, 150, 300, 150, 50 };

/////////////////////////////////////////////////////////////////////////////////
// player variables
// Asteroids used 8-bits for angles  0 though 255
// I have fewer sprite frames than 256, but can reuse frames for similar angles.
fix16 thrustX[256];
fix16 thrustY[256];
u8 shipDir = 0; // 0 is up
const u8 angleStep = 3; // turn rate.

Sprite *ship_sprite;
fix16 ship_accel_x = FIX16(0.0);
fix16 ship_accel_y = FIX16(0.0);
fix16 ship_speed_x = FIX16(0.0);
fix16 ship_speed_y = FIX16(0.0);
fix16 ship_pos_x = FIX16(0.0);
fix16 ship_pos_y = FIX16(0.0);
const fix16 max_speed = FIX16(4.0);
fix16 max_speed_x[256];
fix16 max_speed_y[256];
Sprite *warp_sprite;
//static u8 prevBlinkState = 0;
//static u8 currBlinkState = 0;
//static bool isWarpDown = FALSE;
enum SHIP_STATE {
    ship_dead = 0,
    ship_live = 1,
    ship_warping_in = 2,
    ship_warp_pressed = 4,
};
fix16 ship_warp_pos_x = FIX16(0.0);
fix16 ship_warp_pos_y = FIX16(0.0);
u8 ship_state = ship_dead;
static u16 score = 0;
static s16 lives = 3;
static s16 ship_ticks = 0;


/////////////////////////////////////////////////////////////////////////////////
// explosions

Sprite *explosion_sprites[MAX_EXPLOSIONS];
s16 explosion_ticks[MAX_EXPLOSIONS];
bool explosion_live[MAX_EXPLOSIONS];



// utils

void clear_enemy_objs() {

    for( u8 i=0; i < MAX_ROCKS + 1; ++i ) {
        if( obj_sprites[i] != NULL ) {
            SPR_releaseSprite( obj_sprites[i] );
            obj_sprites[i] = NULL;
        }
        obj_speed_x[i] = 0;
        obj_speed_y[i] = 0;
        obj_pos_x[i] = 0;
        obj_pos_y[i] = 0;
        obj_type[i] = 0;
        obj_hit_w[i] = 0;
        obj_live[i] = FALSE;
    }
}


void clear_objs() {


    memset( obj_sprites, 0, sizeof( obj_sprites ));
    memset( obj_speed_x, 0, sizeof( obj_speed_x ));
    memset( obj_speed_y, 0, sizeof( obj_speed_y ));
    memset( obj_pos_x, 0, sizeof( obj_pos_x ));
    memset( obj_pos_y, 0, sizeof( obj_pos_y ));
    memset( obj_type, 0, sizeof( obj_type ));
    memset( obj_hit_w, 0, sizeof( obj_hit_w ));

    memset( obj_ticks, 0, sizeof( obj_ticks ));

    for( u8 i=0; i < MAX_OBJECTS; ++i ) {
        obj_live[i] = FALSE;
    }
}



u8 currentExplosion = 0;

static void showExplosion(fix16 pos_x, fix16 pos_y)
{
    if (explosion_live[currentExplosion] == FALSE)
    {
        s16 x = F16_toInt(pos_x)- camPosX + MAP_HALF_WIDTH;
        s16 y = F16_toInt(pos_y)- camPosY + MAP_HALF_HEIGHT;
        // use it
        explosion_live[currentExplosion] = TRUE;
        explosion_ticks[currentExplosion] = 0;

        SPR_setVisibility(explosion_sprites[currentExplosion], VISIBLE);
        SPR_setPosition(explosion_sprites[currentExplosion], x, y );

        XGM_startPlayPCM(SND_EXPLOSION, 10, SOUND_PCM_CH3);

        // point to next explosion 
        ++currentExplosion;
        if (currentExplosion >= MAX_EXPLOSIONS)
        {
            currentExplosion = 0;
        }
    }
}

void spawnShip() {
    // put the ship in the center 
    //   px =   screenx + camPosX
    ship_pos_x = FIX16(camPosX + 147 - MAP_HALF_WIDTH);
    ship_pos_y = FIX16(camPosY + 100 - MAP_HALF_HEIGHT);
    SPR_setVisibility(ship_sprite, VISIBLE);
    ship_state = ship_live | ship_warping_in ;
    ship_ticks = 0 ; // reset for warp in timing.
}


void updateCameraPos()
{
    s16 px = F16_toInt(ship_pos_x);
    s16 py = F16_toInt(ship_pos_y);

    s16 playerScreenX = px - camPosX;
    s16 playerScreenY = py - camPosY;

    // adjust new camera position based on ship_position
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
    if (newCamX < -MAP_HALF_WIDTH)
    {
        newCamX = -MAP_HALF_WIDTH;
    }
    else if (newCamX > (MAP_HALF_WIDTH - SCR_WIDTH))
    {
        newCamX = MAP_HALF_WIDTH - SCR_WIDTH;
    }

    if (newCamY < -MAP_HALF_HEIGHT)
    {
        newCamY = -MAP_HALF_HEIGHT;
    }
    else if (newCamY > (MAP_HALF_HEIGHT - SCR_HEIGHT))
    {
        newCamY = MAP_HALF_HEIGHT - SCR_HEIGHT;
    }

    camPosX = newCamX + MAP_HALF_WIDTH;
    camPosY = newCamY + MAP_HALF_HEIGHT;
    MAP_scrollTo(map_b, camPosX, camPosY);
}

void inputCallback(u16 joy, u16 changed, u16 state)
{
    // create a shot if available
    if( game_mode == play_mode ) {
        if (changed & state & BUTTON_A && (ship_state & ship_live ) &&  !( ship_state & ship_warp_pressed) )
        {
            for (s16 i = MAX_OBJECTS - MAX_PLAYER_SHOTS; i < MAX_OBJECTS; ++i)
            {

                if (obj_live[i] == FALSE)
                {
                    // create a new one
                    XGM_startPlayPCM(SND_LASER, 1, SOUND_PCM_CH1);

                    obj_pos_x[i] = ship_pos_x+ FIX16(SHOT_OFFSET_X) ;// + F16_mul(thrustX[shipDir], FIX16(2.0));
                    obj_pos_y[i] = ship_pos_y+ FIX16(SHOT_OFFSET_Y) ;// + F16_mul(thrustY[shipDir], FIX16(2.0));
                    obj_speed_x[i] = ship_speed_x + (thrustX[shipDir] << 4 );
                    obj_speed_y[i] = ship_speed_y + (thrustY[shipDir] << 4 );
                    obj_live[i] = TRUE;
                    obj_ticks[i] = 0; // you do need this.
                    break;
                }
            }
        }
    } else {
        if (changed & state & BUTTON_START ) {
            clear_enemy_objs();
            level = 1;
            lives = 5;
            game_mode = play_mode;
            VDP_clearPlane( BG_A, FALSE);
            spawnShip();
            delayTicks = 120;
            levelStarted = false;
           // VDP_drawText("           ", 14, 20 );
            VDP_clearTextLine( 20 );

        }
    }
}

void handleInput()
{

    if( game_mode == play_mode && (ship_state & ship_live)  ) {
        u16 currState = JOY_readJoypad(JOY_1);
        // check rotation every frame.
        if (currState & BUTTON_LEFT)
        {
            shipDir -=angleStep;
            // ship rotation with 32 total frames and 256 total dirs. 1 frame has to cover
            //  8 dirs.   so frame 0 could cover dirs  252 253 254 255  0 1 2 3
            //        .   so frame 1 covers dirs  4 5 6 7 8 9 10 11
            u8 tmpDir = shipDir + 4;
            SPR_setAnim(ship_sprite, tmpDir >> 3);
        }
        else if (currState & BUTTON_RIGHT)
        {
            shipDir +=angleStep;
            u8 tmpDir = shipDir + 4;
            SPR_setAnim(ship_sprite, tmpDir >> 3);

        }

        if (currState & BUTTON_B) {
            if( !(ship_state & ship_warp_pressed) ) {
                // player just pressed warp button
                ship_state |= ship_warp_pressed;
                SPR_setVisibility(warp_sprite, VISIBLE);
            }
        } else {
            if( ship_state & ship_warp_pressed ) {
                // player just released warp
                ship_state ^= ship_warp_pressed;
                ship_pos_x = ship_pos_x + ( thrustX[shipDir] << 8 );
                ship_pos_y = ship_pos_y + ( thrustY[shipDir] << 8 );
                SPR_setVisibility(warp_sprite, HIDDEN);
            }
        }

        if( tick & 0x01 ) {
            // check thrust every other frame.
            if (currState & BUTTON_UP && !(ship_state & ship_warp_pressed))
            {
                // thrust
                //ship_accel_x += (thrustX[shipDir] >> 2);
                ship_accel_x += thrustX[shipDir];
                ship_speed_x += ship_accel_x;
                if( ship_speed_x > FIX16(0.0) && ship_accel_x > FIX16(0.0) && ship_speed_x >  max_speed_x[shipDir] ) {
                    ship_speed_x = max_speed_x[shipDir];
                    ship_accel_x = FIX16(0.0);
                } else if ( ship_speed_x < FIX16(0) && ship_accel_x < FIX16(0.0) && ship_speed_x <  max_speed_x[shipDir] ) {
                    ship_speed_x = max_speed_x[shipDir];
                    ship_accel_x = FIX16(0.0);
                }

                //ship_accel_y += (thrustY[shipDir] >> 2);
                ship_accel_y += thrustY[shipDir];
                ship_speed_y += ship_accel_y;
                if( ship_speed_y > FIX16(0.0) && ship_accel_y > FIX16(0.0) && ship_speed_y >  max_speed_y[shipDir] ) {
                    ship_speed_y = max_speed_y[shipDir];
                    ship_accel_y = FIX16(0.0);
                } else if ( ship_speed_y < FIX16(0.0) && ship_accel_y < FIX16(0.0) && ship_speed_y < max_speed_y[shipDir] ) {
                    ship_speed_y = max_speed_y[shipDir];
                    ship_accel_y = FIX16(0.0);
                }

            }
            else
            {
                // Decelerate without atan()/atan2()
                // not thrusting, check x and y movemtn components
                //  and turn orn acceleration to counter.
                if( ship_speed_x > FIX16(0.0) ) {
                    ship_accel_x = FIX16(-0.03);
                    ship_speed_x += ship_accel_x;
                    if( ship_speed_x <= FIX16(0.0) ) {
                        ship_speed_x = FIX16(0.0);
                        ship_accel_x = FIX16(0.0);
                        //ship_speed_y = FIX16(0.0);
                        //ship_accel_y = FIX16(0.0);
                    }
                } else if( ship_speed_x < FIX16(0.0) ) {
                    ship_accel_x = FIX16(0.03);
                    ship_speed_x += ship_accel_x;
                    if( ship_speed_x >= FIX16(0.0) ) {
                        ship_speed_x = FIX16(0.0);
                        ship_accel_x = FIX16(0.0);
                        //ship_speed_y = FIX16(0.0);
                        //ship_accel_y = FIX16(0.0);
                    }
                }

                if( ship_speed_y > FIX16(0.0) ) {
                    ship_accel_y = FIX16(-0.03);
                    ship_speed_y += ship_accel_y;
                    if( ship_speed_y <= FIX16(0.0) ) {
                        ship_speed_y = FIX16(0.0);
                        ship_accel_y = FIX16(0.0);
                        //ship_speed_x = FIX16(0.0);
                        //ship_accel_x = FIX16(0.0);
                    }
                } else if( ship_speed_y < FIX16(0.0) ) {
                    ship_accel_y = FIX16(0.03);
                    ship_speed_y += ship_accel_y;
                    if( ship_speed_y >= FIX16(0.0) ) {
                        ship_speed_y = FIX16(0.0);
                        ship_accel_y = FIX16(0.0);
                        //ship_speed_x = FIX16(0.0);
                        //ship_accel_x = FIX16(0.0);
                    }
                }

            }

        } 
    } else {

    }

}

void createUfo( )  {
    // start on one side
    obj_type[UFO_SLOT] = UFO;
    f16 vel = FIX16(0.5);

    obj_hit_w[UFO_SLOT] = FIX16(28);
    if( level > 12 ) {
        obj_type[UFO_SLOT] = SMALL_UFO; 
    }else if( level > 8 ) {
        if(  random() % 4 > 0 )  {
            obj_type[UFO_SLOT] = SMALL_UFO;
        }
    }else if( level > 5 ) {
        if(  random() % 4 > 1 )  {
            obj_type[UFO_SLOT] = SMALL_UFO;
        }
    }else if( level > 3 ) {
        if(  random() % 4 > 3 )  {
            obj_type[UFO_SLOT] = SMALL_UFO;
        }
    }


    if( obj_type[UFO_SLOT] == SMALL_UFO ) {
        vel = FIX16(1.0);
        obj_hit_w[UFO_SLOT] = FIX16(12);
        obj_sprites[UFO_SLOT] = SPR_addSprite(&small_ufo, -32, -32, TILE_ATTR(PAL3, 0, FALSE, FALSE));
    } else {
        obj_sprites[UFO_SLOT] = SPR_addSprite(&ufo, -32, -32, TILE_ATTR(PAL3, 0, FALSE, FALSE));
    }


    // start at same Y as ship
    obj_pos_y[UFO_SLOT]= ship_pos_y;


    if(  random() % 2 == 0 ) {
        // go left to right
        obj_speed_x[UFO_SLOT] = vel;
        obj_pos_x[UFO_SLOT]= ship_pos_x - FIX16(220);
    } else {
        // right to left
        obj_speed_x[UFO_SLOT] = -vel;
        obj_pos_x[UFO_SLOT]= ship_pos_x + FIX16(220);
    }

    obj_live[UFO_SLOT] = TRUE;

    SPR_setAnim(obj_sprites[UFO_SLOT], 0);
}


void fireUfoShot() {
    u8 shotDir = 0;
    if( obj_type[UFO_SLOT] == UFO ) {
        shotDir = random() & 0x2F;
    } else if ( obj_type[UFO_SLOT] == SMALL_UFO ) {
        // find relative position of ship to UFO
        fix16 deltaX = obj_pos_x[UFO_SLOT] - ship_pos_x;
        fix16 deltaY = obj_pos_y[UFO_SLOT] - ship_pos_y;

        // Handle special cases for pure vertical/horizontal
        if (deltaX == FIX16(0)) {
            // wi & arae
            shotDir = (deltaY > 0) ? 0 : 32; 
        }else if (deltaY == FIX16(0)) { 
            // Left or Right
            shotDir = (deltaX > 0) ? 48 : 16;  
        } else {

            // get absolute values
            s16 absX = F16_toInt(abs(deltaX));
            s16 absY = F16_toInt(abs(deltaY));


            if( absX == absY ) {
                // 45 degrees
                if (deltaX <= 0 && deltaY >= 0) {  // remember 0,0 is upper left, so a negative Y is aiming UP
                    shotDir = 8; // 360/64 -> 5.625  and 8 * 5.625 = 45 degrees
                } else if (deltaX <= 0 && deltaY <= 0) {
                    shotDir = 24; 
                } else if (deltaX > 0 && deltaY < 0) {
                    shotDir = 40; 
                } else {
                    shotDir = 56; 
                }

            } else {
                // Binary search approximation (4 bits for 16 angles per quadrant)
                bool swap = FALSE;
                // swap if we must
                if( absX > absY ) {
                    swap = TRUE;
                    s16 temp = absX;
                    absX = absY;
                    absY = temp;
                }

                // loop up to 4 times.
                u8 index = 16;
                for (u8 i = 0; i < 4; i++) {
                    if (absX < absY) {
                        index >>=1;
                    } else if ( absX == absY )  {
                        index >>=1;
                        break;
                    } else {
                        absX = absX - absY;
                        index |=1; // add current bit
                    }
                    absY >>=1; // she's gotta halve it
                }
                shotDir = index & 0x0F; // limit index to 16 values.

                if(swap) {
                    shotDir = 16 - shotDir;      
                }
                // Adjust for the quadrant
                /*if (deltaX <= 0 && deltaY >= 0) {  // remember 0,0 is upper left, so a negative Y is aiming UP
                  } else*/ if (deltaX <= 0 && deltaY <= 0) {
                      // LR Quadrant 
                      shotDir = 32 - shotDir;
                  } else if (deltaX > 0 && deltaY < 0) {
                      // LL Quadrant 
                      shotDir = 32 + shotDir;
                  } else if( deltaX > 0 && deltaY > 0){
                      // UL Quadrant 
                      shotDir = 48 + (15 - shotDir);
                  }
            }
        }

    }
    // spawn a shot if shot slot is available
    for (u16 i = UFO_SLOT + 1;  i <  MAX_OBJECTS - MAX_PLAYER_SHOTS; ++i)
    {
        if (obj_live[i] == FALSE)
        {
            // create a new one
            XGM_startPlayPCM(SND_LASER, 1, SOUND_PCM_CH2);

            obj_pos_x[i] = obj_pos_x[UFO_SLOT] + FIX16(SHOT_OFFSET_X) ;// + F16_mul(thrustX[shipDir], FIX16(2.0));
            obj_pos_y[i] = obj_pos_y[UFO_SLOT] + FIX16(SHOT_OFFSET_Y) ;// + F16_mul(thrustY[shipDir], FIX16(2.0));
            obj_speed_x[i] = obj_speed_x[UFO_SLOT] + (ufoShotX[shotDir] );
            obj_speed_y[i] = obj_speed_y[UFO_SLOT] + (ufoShotY[shotDir] );
            obj_live[i] = TRUE;
            obj_ticks[i] = 0; // you do need this.
            break;
        }
    }

}





void updateUfo() 
{
    ufoTick++; 
    if( obj_live[UFO_SLOT] == FALSE ) {
        if( ufoTick >= UFO_SPAWN_TIME ) {
            // create UFO
            createUfo();
            ufoTick = 0;
        }
        return;
    } else {
        // change Y veolcity every once in a while.
        if ( ufoTick % 60 == 0 ) {
            u8 rot = random();
            obj_speed_y[UFO_SLOT] = F16_mul(FIX16(4.0), thrustY[rot]);
            if(  (obj_pos_y[UFO_SLOT] - ship_pos_y ) < FIX16(50.0) ) {
                if( obj_speed_y[UFO_SLOT] < FIX16(0.0) ) {
                    obj_speed_y[UFO_SLOT] =  - obj_speed_y[UFO_SLOT];
                }
            } else if(  (obj_pos_y[UFO_SLOT] - ship_pos_y ) > FIX16(50.0) ) {
                if( obj_speed_y[UFO_SLOT] > FIX16(0.0) ) {
                    obj_speed_y[UFO_SLOT] =  - obj_speed_y[UFO_SLOT];
                }
            }
        }
        // fire when on scren.
        if ( ufoTick % UFO_SHOT_TICKS  == 0 && obj_live[UFO_SLOT] == TRUE && SPR_isVisible( obj_sprites[UFO_SLOT], true )) {
            fireUfoShot();
        }

    }

}


void createRock(u8 i, u16 rockType, fix16 x, fix16 y ) {

    obj_pos_x[i] = x;
    obj_pos_y[i] = y;
    u8 rot = random();
    obj_live[i] = TRUE;
    fix16 vel = rockVelocity;
    if( rockType == ROCK ) {
        obj_hit_w[i] = FIX16(30);
        obj_sprites[i] = SPR_addSprite(&rock, -32, -32, TILE_ATTR(PAL3, 0, FALSE, FALSE));
    } else if ( rockType == MID_ROCK ) {
        vel = rockVelocity + FIX16(2);
        obj_sprites[i] = SPR_addSprite(&mid_rock, -32, -32, TILE_ATTR(PAL3, 0, FALSE, FALSE));
        obj_hit_w[i] = FIX16(22);
    } else if ( rockType == SMALL_ROCK ) {
        vel = rockVelocity + FIX16(4);
        obj_sprites[i] = SPR_addSprite(&small_rock, -32, -32, TILE_ATTR(PAL3, 0, FALSE, FALSE));
        obj_hit_w[i] = FIX16(14);
    }
    obj_speed_x[i] = F16_mul( vel, thrustX[rot] );
    obj_speed_y[i] = F16_mul( vel, thrustY[rot] );
    SPR_setAnim(obj_sprites[i], i % 4);
    obj_type[i] = rockType;
}


void createRocks(u8 numRocks )
{
    for (u8 i = 0; i < MAX_ROCKS; ++i)
    {
        if( i < numRocks ) {
            fix16 x = FIX16(-MAP_HALF_WIDTH);
            fix16 y = FIX16(-MAP_HALF_HEIGHT);
            if( random() %2 == 0 ) {
                // x edges
                y = FIX16(random() % (MAP_HEIGHT - 32) - MAP_HALF_HEIGHT );
            } else {
                x = FIX16(random() % (MAP_WIDTH - 32) - MAP_HALF_WIDTH );
            }
            createRock( i, ROCK, x, y );
        } else {
            // clear out the rest  of the rocks
            obj_pos_x[i] = FIX16(-32);
            obj_pos_y[i] = FIX16(-32);

            obj_speed_x[i] = FIX16(0);
            obj_speed_y[i] = FIX16(0);
            obj_live[i] = FALSE;
            obj_hit_w[i] = FIX16(0);
            obj_sprites[i] = NULL;
            obj_type[i] = NO_TYPE;
        }
    }
}


void splitRock(u16 whichRock )
{
    u8 count = 0;
    fix16 x = obj_pos_x[whichRock];
    fix16 y = obj_pos_y[whichRock];
    u16 newType = MID_ROCK;
    if( obj_type[whichRock] == MID_ROCK ) {
        newType = SMALL_ROCK;
    }
    // u
    for (u8 i = 0; i < MAX_ROCKS; ++i)
    {
        if( obj_live[i] == FALSE ) {
            createRock( i, newType, x, y );
            count++;
            if ( count == 2 ) {
                return; 
            }
        }
    }
}




void startAttractMode() {
     game_mode = attract_mode;
     ship_state = ship_dead;
    // clear obs
    clear_enemy_objs();

    // reset map
    camPosX = 0 - SCR_WIDTH / 2;
    camPosY = 0 - SCR_HEIGHT / 2;
    MAP_scrollTo(map_b, camPosX, camPosY);
    VDP_clearTextLine( 3 );
    VDP_clearTextLine( 12 );
    VDP_clearTextLine( 20 );
    VDP_drawImageEx(BG_A, &title, TILE_ATTR_FULL(PAL3, TRUE, FALSE, FALSE, tile_ind_title), 0, 0, FALSE, TRUE);    

    // make all the rocks
    createRocks(MAX_ROCKS);
}

void startLevel() {
    // clear obs
    clear_enemy_objs();
           
    // figure out how many rocks for this level 
    u8 numRocks = 3 + level * 2;
    if( numRocks >= MAX_ROCKS ) {
        numRocks = MAX_ROCKS - 6;
    }
    // change base rock velocity
    fix16 flvl = FIX16( level );
    rockVelocity = FIX16(3.0) +  F16_mul( flvl, FIX16( 0.5 ) );
    if( rockVelocity > FIX16(10) ) {
        rockVelocity = FIX16(10);
    }
   
    // change UFO parameters
 
    // make all the rocks
    createRocks(numRocks);
    levelStarted = true;
}


void update()
{

    // update the player
    if( game_mode == play_mode ) {
        if( ship_state & ship_live ) {
            if( ship_state & ship_warping_in ) {
                ship_ticks++;
                if( ship_ticks < 150 ) {
                    if ( ship_ticks % 3 ) {
                        SPR_setVisibility(ship_sprite, VISIBLE);
                    } else {
                        SPR_setVisibility(ship_sprite, HIDDEN);
                    }
                } else {
                    SPR_setVisibility(ship_sprite, VISIBLE);
                    ship_state ^= ship_warping_in;
                }
            }
            ship_pos_x = ship_pos_x + ship_speed_x;
            ship_pos_y = ship_pos_y + ship_speed_y;

            if( ship_pos_x < FIX16(-6.0 - MAP_HALF_WIDTH ) ) {
                ship_pos_x = FIX16(-6.0 - MAP_HALF_WIDTH);
            } else if( ship_pos_x > FIX16( MAP_HALF_WIDTH - PLAYER_WIDTH + 6 ) ) {
                ship_pos_x = FIX16( MAP_HALF_WIDTH - PLAYER_WIDTH + 6 );
            }

            if( ship_pos_y < FIX16(-6.0 - MAP_HALF_HEIGHT) ) {
                ship_pos_y = FIX16(-6.0 - MAP_HALF_HEIGHT);
            } else if( ship_pos_y > FIX16( MAP_HALF_HEIGHT - PLAYER_HEIGHT + 6 ) ) {
                ship_pos_y = FIX16( MAP_HALF_HEIGHT - PLAYER_HEIGHT + 6 );
            }
        } else {
            ship_ticks++;
            if( ship_state == ship_dead ) {
                if( ship_ticks > 180 ) {
                    // spawn ship in 3 seconds.
                    if( lives > 0 ) {
                        spawnShip();
                    } else {
                        // game mode
                        startAttractMode();
                    }
                }
            }
        }
    }

    // deactiveate shots if they've been around too long.
    //for (u16 i = MAX_OBJECTS - MAX_PLAYER_SHOTS; i < MAX_OBJECTS; ++i)
    for (u16 i = UFO_SLOT+1; i < MAX_OBJECTS; ++i)
    {
        if (obj_live[i] == TRUE)
        {
            obj_ticks[i] += 1;
            if (obj_ticks[i] > PLAYER_SHOT_TIME )
            {
                obj_live[i] = FALSE;
            }
        }
    }

    // update non-players objects.
    for (u16 i = 0; i < MAX_OBJECTS; ++i)
    {
        if (obj_live[i] == TRUE)
        {
            obj_pos_x[i] += obj_speed_x[i];
            if (obj_pos_x[i] < FIX16(-32-MAP_HALF_WIDTH))
            {
                obj_pos_x[i] = FIX16(MAP_HALF_WIDTH);
            }
            else if (obj_pos_x[i] > FIX16(MAP_HALF_WIDTH))
            {
                obj_pos_x[i] = FIX16(-32-MAP_HALF_WIDTH);
            }

            obj_pos_y[i] += obj_speed_y[i];
            if (obj_pos_y[i] < FIX16(-32-MAP_HALF_HEIGHT))
            {
                obj_pos_y[i] = FIX16(MAP_HALF_HEIGHT);
            }
            else if (obj_pos_y[i] > FIX16(MAP_HALF_HEIGHT))
            {
                obj_pos_y[i] = FIX16(-32-MAP_HALF_HEIGHT);
            }

            s16 x = F16_toInt(obj_pos_x[i]) - camPosX + MAP_HALF_WIDTH;
            s16 y = F16_toInt(obj_pos_y[i]) - camPosY + MAP_HALF_HEIGHT;
            if (x >= -32 && x < SCR_WIDTH && y >= -32 && y < SCR_HEIGHT)
            {
                SPR_setVisibility(obj_sprites[i], VISIBLE);
                SPR_setPosition(obj_sprites[i], x, y );
            }
            else
            {
                SPR_setVisibility(obj_sprites[i], HIDDEN);
            }
        }
        else
        {
            SPR_setVisibility(obj_sprites[i], HIDDEN);
        }
    }


    for (u16 i = 0; i < MAX_EXPLOSIONS; ++i)
    {
        if (explosion_live[i] == TRUE)
        {
            explosion_ticks[i] += 1;
            if (explosion_ticks[i] < 9)
            {
                //  SPR_setFrame( explosions[i].sprite, explosions[i].ticks );
                SPR_setAnimAndFrame(explosion_sprites[i], i % 4, explosion_ticks[i]);
            }
            else
            {
                explosion_live[i] = FALSE;
                SPR_setVisibility(explosion_sprites[i], HIDDEN);
            }
        }
    }


    updateCameraPos();
    if( game_mode == play_mode ) {
        if( (ship_state & ship_warp_pressed) && ( tick & 0x04 ) ) {
            ship_warp_pos_x = ship_pos_x + ( thrustX[shipDir] << 8 );
            ship_warp_pos_y = ship_pos_y + ( thrustY[shipDir] << 8 );
            SPR_setPosition(ship_sprite, F16_toInt(ship_warp_pos_x) - camPosX + MAP_HALF_WIDTH, F16_toInt(ship_warp_pos_y) - camPosY + MAP_HALF_HEIGHT);
            SPR_setPosition(warp_sprite, F16_toInt(ship_warp_pos_x) - camPosX + MAP_HALF_WIDTH, F16_toInt(ship_warp_pos_y) - camPosY + MAP_HALF_HEIGHT);

        } else {
            SPR_setPosition(ship_sprite, F16_toInt(ship_pos_x) - camPosX + MAP_HALF_WIDTH, F16_toInt(ship_pos_y) - camPosY + MAP_HALF_HEIGHT);
            SPR_setPosition(warp_sprite, F16_toInt(ship_pos_x) - camPosX + MAP_HALF_WIDTH, F16_toInt(ship_pos_y) - camPosY + MAP_HALF_HEIGHT);
        }
    }

    // check if rocks all rocks are gone once in a while
    if( tick % 10 == 0 && levelStarted == true ) {
        //  
        rockCount = 0;
        for( u16 i=0; i < UFO_SLOT; ++i ) 
        {
            if( obj_live[i] == true ) {
                rockCount++;
            }
        }
        if( rockCount == 0 ) {
            level++;
            levelStarted = false;
            delayTicks = 120;
        }
    }

}


static void checkCollisions()
{

    for (u16 i = 0; i < MAX_OBJECTS - MAX_PLAYER_SHOTS; ++i)
    {
        if (obj_live[i] == TRUE && SPR_isVisible( obj_sprites[i], false ))
        {

            // check if ship has hit by anything that wasn't a player shot
            if ( game_mode == play_mode && obj_live[i] == TRUE && (ship_state & ship_live)      // ship is live
                    && ! ((ship_state & ship_warp_pressed) || ( ship_state & ship_warping_in) ) && // but not in a warp state
                    (obj_pos_x[i] + FIX16(2))      < (ship_pos_x + FIX16(21) ) &&
                    (obj_pos_x[i] + obj_hit_w[i] ) > (ship_pos_x + FIX16(3) )  &&
                    (obj_pos_y[i] + FIX16(2))      < (ship_pos_y + FIX16(21) ) &&
                    (obj_pos_y[i] + obj_hit_w[i])  > (ship_pos_y + FIX16(3) ))
            {

                // handle the killer object

                obj_live[i] = FALSE;
                if( i <= UFO_SLOT ) {   
                    // release rocks or UFO since a different sprite type may occupy 
                    // this slot later
                    SPR_releaseSprite( obj_sprites[i] );
                    obj_sprites[i] = NULL;
                } else {
                    // shots stay 
                    SPR_setVisibility(obj_sprites[i], HIDDEN);
                }

                XGM_startPlayPCM(SND_EXPLOSION, 10, SOUND_PCM_CH3);
                showExplosion(obj_pos_x[i], obj_pos_y[i]);
                // make more rocks
                if( obj_type[i] == ROCK || obj_type[i] == MID_ROCK ) {
                    splitRock(i);
                } else if (obj_type[i] == UFO || obj_type[i] == SMALL_UFO ) {
                    // or reset UFO.
                    ufoTick = 0;
                }
                --lives;    
                ship_state = ship_dead;
                ship_ticks = 0;
                XGM_startPlayPCM(SND_EXPLOSION, 10, SOUND_PCM_CH1);
                showExplosion(ship_pos_x, ship_pos_y );  
                ship_speed_x = FIX16(0);  
                ship_speed_y = FIX16(0);  
                SPR_setVisibility(ship_sprite, HIDDEN);
                if( lives == 0 ) {
                    VDP_drawText( "GAME OVER", 15, 12 );
                }
            }

            // if UFO is live check if a rock hit it.
            if(i < UFO_SLOT &&  obj_live[UFO_SLOT] == TRUE ) {
                if( SPR_isVisible(obj_sprites[UFO_SLOT], false) &&
                        (obj_pos_x[i] + FIX16(2))      <  (obj_pos_x[UFO_SLOT] + obj_hit_w[UFO_SLOT]) &&
                        (obj_pos_x[i] + obj_hit_w[i] ) >  (obj_pos_x[UFO_SLOT] + FIX16(2))      &&
                        (obj_pos_y[i] + FIX16(2))      <  (obj_pos_y[UFO_SLOT] + obj_hit_w[UFO_SLOT]) &&
                        (obj_pos_y[i] + obj_hit_w[i])  >  (obj_pos_y[UFO_SLOT] + FIX16(2))  )
                {
                    // destory both
                    obj_live[i] = FALSE;
                    SPR_releaseSprite( obj_sprites[i] );
                    obj_sprites[i] = NULL;
                    obj_live[UFO_SLOT] = FALSE;
                    SPR_releaseSprite( obj_sprites[UFO_SLOT] );
                    obj_sprites[UFO_SLOT] = NULL;
                    XGM_startPlayPCM(SND_EXPLOSION, 10, SOUND_PCM_CH3);
                    showExplosion(obj_pos_x[i], obj_pos_y[i]);
                    if( obj_type[i] == ROCK || obj_type[i] == MID_ROCK ) {
                        // make more rocks 
                        splitRock(i);
                    } 
                    ufoTick = 0;
                }
            }


            // check if object is hit by UFO shot
            if( i < UFO_SLOT  ) {
                for (u8 j = UFO_SLOT+1; j < MAX_OBJECTS - MAX_PLAYER_SHOTS; ++j)
                {
                    if ( obj_live[j] == TRUE &&
                            (obj_pos_x[i] + FIX16(2))     < (obj_pos_x[j] + FIX16(4)) &&
                            (obj_pos_x[i] + obj_hit_w[i]) > (obj_pos_x[j] + FIX16(4)) &&
                            (obj_pos_y[i] + FIX16(2))     < (obj_pos_y[j] + FIX16(4)) &&
                            (obj_pos_y[i] + obj_hit_w[i]) > (obj_pos_y[j] + FIX16(4)))
                    {
                        // deactivate the object
                        obj_live[i] = FALSE;
                        // and release it
                        SPR_releaseSprite( obj_sprites[i] );
                        obj_sprites[i] = NULL;
                        // deactivate the shot
                        obj_live[j] = FALSE;
                        SPR_setVisibility(obj_sprites[j], HIDDEN);
                        // play the sound
                        XGM_startPlayPCM(SND_EXPLOSION, 10, SOUND_PCM_CH3);
                        showExplosion(obj_pos_x[i], obj_pos_y[i]);

                        if( obj_type[i] == ROCK || obj_type[i] == MID_ROCK ) {
                            // make more rocks 
                            splitRock(i);
                        }
                    }
                }
            }

            // check if object is hit by player shot
            for (u8 j = MAX_OBJECTS - MAX_PLAYER_SHOTS; j < MAX_OBJECTS; ++j)
            {

                if ( obj_live[j] == TRUE &&
                        (obj_pos_x[i] + FIX16(2))     < (obj_pos_x[j] + FIX16(4)) &&
                        (obj_pos_x[i] + obj_hit_w[i]) > (obj_pos_x[j] + FIX16(4)) &&
                        (obj_pos_y[i] + FIX16(2))     < (obj_pos_y[j] + FIX16(4)) &&
                        (obj_pos_y[i] + obj_hit_w[i]) > (obj_pos_y[j] + FIX16(4)))
                {

                    // deactivate the object
                    obj_live[i] = FALSE;
                    // and release it
                    SPR_releaseSprite( obj_sprites[i] );
                    obj_sprites[i] = NULL;
                    //SPR_setVisibility(obj_sprites[i], HIDDEN);


                    // deactivate the shot
                    obj_live[j] = FALSE;
                    SPR_setVisibility(obj_sprites[j], HIDDEN);
                    // play the sound
                    XGM_startPlayPCM(SND_EXPLOSION, 10, SOUND_PCM_CH3);
                    showExplosion(obj_pos_x[i], obj_pos_y[i]);
                    score += score_by_type[obj_type[i]];
                    if( obj_type[i] == ROCK || obj_type[i] == MID_ROCK ) {
                        // make more rocks 
                        splitRock(i);
                    } else if (obj_type[i] == UFO || obj_type[i] == SMALL_UFO ) {
                        // or reset UFO.
                        ufoTick = 0;
                    }
                }
            }
        }
    }

}


void createShots()
{
    fix16 xpos = FIX16(-16);
    fix16 ypos = FIX16(-16);

    for (u16 i = MAX_OBJECTS - MAX_PLAYER_SHOTS; i < MAX_OBJECTS; ++i)
    {

        obj_pos_x[i] = xpos;
        obj_pos_y[i] = ypos;
        obj_speed_x[i] = FIX16(0.0); 
        obj_speed_y[i] = FIX16(0.0); 
        obj_live[i] = FALSE;
        obj_ticks[i] = 0;

        obj_sprites[i] = SPR_addSprite(&shot, xpos, ypos, TILE_ATTR(PAL0, 0, FALSE, FALSE));
        SPR_setAnim(obj_sprites[i],2);
    }
    for (u16 i = UFO_SLOT + 1;  i <  MAX_OBJECTS - MAX_PLAYER_SHOTS; ++i)
    {
        obj_pos_x[i] = xpos;
        obj_pos_y[i] = ypos;
        obj_speed_x[i] = FIX16(0.0); 
        obj_speed_y[i] = FIX16(0.0); 
        obj_live[i] = FALSE;
        obj_ticks[i] = 0;

        obj_sprites[i] = SPR_addSprite(&shot, xpos, ypos, TILE_ATTR(PAL0, 0, FALSE, FALSE));
        SPR_setAnim(obj_sprites[i],0); // blue shot

    }
}



static void createExplosions()
{
    for (u16 i = 0; i < MAX_EXPLOSIONS; ++i)
    {
        explosion_ticks[i] = 0;
        explosion_live[i] = FALSE;

        explosion_sprites[i] = SPR_addSprite(&explosion, -32, -32, TILE_ATTR(PAL0, 0, FALSE, FALSE));
        SPR_setAnim(explosion_sprites[i], i % 4);
        SPR_setVisibility(explosion_sprites[i], HIDDEN);
        SPR_setDepth(explosion_sprites[i], SPR_MIN_DEPTH);
    }
}



int main(bool hard)
{
    clear_objs();
    /////////////////////////////////////////////////////////////////////////////////
    // Setup Thrust Table
    u16 pos = 0;
    // Starting with UP just because I drew the ship that way.
    // start at PI/2 and procede clockwise
    //   in FIX16 90 degrees is at 256  ( 1/4 1024)
    for( s16 i = 64; i >= 0; i-- ) {
        thrustX[pos] =  F16_div( cosFix16(i * 4), FIX16(5));
        max_speed_x[pos] = F16_mul( max_speed, cosFix16(i*4));
        thrustY[pos] = -F16_div( sinFix16(i * 4), FIX16(5));  // flip the Y. We're not mathemagicians
        max_speed_y[pos] = -F16_mul( max_speed, sinFix16(i*4));
        pos++;
    }
    for( s16 i = 255; i > 64; i-- ) {
        //thrustX[pos] = cosFix16(i * 4);
        //max_speed_x[pos] = F16_mul( max_speed, thrustX[pos]);
        //thrustY[pos] = -sinFix16(i * 4);
        //max_speed_y[pos] = F16_mul( max_speed, thrustY[pos]);
        thrustX[pos] =  F16_div( cosFix16(i * 4), FIX16(5));
        max_speed_x[pos] = F16_mul( max_speed, cosFix16(i*4));
        thrustY[pos] = -F16_div( sinFix16(i * 4), FIX16(5));  // flip the Y. We're not mathemagicians
        max_speed_y[pos] = -F16_mul( max_speed, sinFix16(i*4));
        pos++;
    }

    /////////////////////////////////////////////////////////////////////////////////
    // Setup UFO Shot Table (64 slots)
    // starting with UP to match ship directions
    pos = 0;
    for( s16 i = 16; i >= 0; i-- ) {
        ufoShotX[pos] =  F16_mul( cosFix16(i * 16), FIX16(3));
        ufoShotY[pos] = -F16_mul( sinFix16(i * 16), FIX16(3)); 
        pos++;
    }
    for( s16 i = 63; i > 16; i-- ) {
        ufoShotX[pos] =  F16_mul( cosFix16(i * 16), FIX16(3));
        ufoShotY[pos] = -F16_mul( sinFix16(i * 16), FIX16(3));  
        pos++;
    }


    /////////////////////////////////////////////////////////////////////////////////
    // setup sounds
    XGM_setPCM(SND_LASER, laser_snd, sizeof(laser_snd));
    XGM_setPCM(SND_EXPLOSION, explosion_snd, sizeof(explosion_snd));

    /////////////////////////////////////////////////////////////////////////////////
    // Screen init
    VDP_setScreenWidth320();

    PAL_setPalette(PAL0, shot_pal.data, CPU);
    PAL_setPalette(PAL1, plane_b_pal.data, CPU);
    PAL_setPalette(PAL2, ship_pal.data, CPU);
    PAL_setPalette(PAL3, rock_pal.data, CPU);

    // Load the plane tiles into VRAM
    VDP_loadTileSet(&plane_b_tileset, tile_ind_map, DMA);
    tile_ind_title = tile_ind_map +  plane_b_tileset.numTile;
    VDP_loadTileSet(title.tileset, tile_ind_title, DMA);


    // init background map
    map_b = MAP_create(&plane_b_map, BG_B, TILE_ATTR_FULL(PAL1, FALSE, FALSE, FALSE, tile_ind_map));
    camPosX = 0 - SCR_WIDTH / 2;
    camPosY = 0 - SCR_HEIGHT / 2;
    MAP_scrollTo(map_b, camPosX, camPosY);


    /////////////////////////////////////////////////////////////////////////////////
    // Init sprite engine with defaults
    SPR_initEx(900);
    ship_pos_x = FIX16(0);
    ship_pos_y = FIX16(0);
    warp_sprite = SPR_addSprite(&blink2, F16_toInt(ship_pos_x) - camPosX, F16_toInt(ship_pos_y) - camPosY, TILE_ATTR(PAL3, 0, FALSE, FALSE));
    SPR_setVisibility(warp_sprite, HIDDEN);
    ship_sprite = SPR_addSprite(&ship, F16_toInt(ship_pos_x), F16_toInt(ship_pos_y), TILE_ATTR(PAL2, 0, FALSE, FALSE));
    SPR_setAnim(ship_sprite, 0);
    SPR_setVisibility(ship_sprite, HIDDEN);

    // create static sprites
    createShots();
    createExplosions();

    // LFG
    startAttractMode();

    JOY_setEventHandler(&inputCallback);
    u8 showStartMessage = 0;
    char message[40];
    while(TRUE)
    {
        tick++; // increment for decision making

        // read game pads and make initial calcs
        if( game_mode == play_mode ) {
            sprintf( message, "Ships: %d  Score: %d Rocks: %d ", lives, score, rockCount);
            VDP_drawText(message, 2,3 );
            handleInput();
            if( delayTicks > 0 ) {
                delayTicks--;
            } else if ( delayTicks == 0 ) {
                // start it up
                startLevel();
                delayTicks--;
            } 
        } else {
            showStartMessage++;
            if( showStartMessage < 40 ) {
                VDP_drawText("Press Start", 14, 20 );
            } else if (showStartMessage < 60 ) {
                //VDP_drawText("           ", 14, 20 );
                VDP_clearTextLine( 20 );
            } else {
                showStartMessage = 0;
            }
        }



        updateUfo();

        // move sprites
        update();

        checkCollisions();

        // SGDK stuff.
        SPR_update();
        SYS_doVBlankProcess();
    }
    return 0;
}
