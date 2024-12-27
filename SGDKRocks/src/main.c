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
#define PLAYER_SHOT_TIME    42  


/////////////////////////////////////////////////////////////////////////////////
// Define enemy constants

#define MAX_OBJECTS         45
// leave 8 of the objs for ufos and player shots
#define MAX_ROCKS           37
#define MAX_EXPLOSIONS      6


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


/////////////////////////////////////////////////////////////////////////////////
// World/Map variables
Map *map_b;
s16 camPosX; // relative to total world map
s16 camPosY; // relative to total world map

static u8 tick = 0; // just a commn tick for everyone to use

/////////////////////////////////////////////////////////////////////////////////
// Moving Objs variables
Sprite *obj_sprites[MAX_OBJECTS];
fix16 obj_speed_x[MAX_OBJECTS];
fix16 obj_speed_y[MAX_OBJECTS];
fix16 obj_pos_x[MAX_OBJECTS];
fix16 obj_pos_y[MAX_OBJECTS];
u8 obj_width[MAX_OBJECTS];
fix16 obj_hit_w[MAX_OBJECTS];
s16 obj_ticks[MAX_OBJECTS];
bool obj_live[MAX_OBJECTS];


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
const fix16 max_speed = FIX16(5.5);
fix16 max_speed_x[256];
fix16 max_speed_y[256];
const s16 playfield_width = 320;
const s16 playfield_height = 224;



void clear_objs() {
    memset( obj_sprites, 0, sizeof( obj_sprites ));
    memset( obj_speed_x, 0, sizeof( obj_speed_x ));
    memset( obj_speed_y, 0, sizeof( obj_speed_y ));
    memset( obj_pos_x, 0, sizeof( obj_pos_x ));
    memset( obj_pos_y, 0, sizeof( obj_pos_y ));
    memset( obj_width, 0, sizeof( obj_width ));
    memset( obj_hit_w, 0, sizeof( obj_hit_w ));

    memset( obj_ticks, 0, sizeof( obj_ticks ));

    for( u8 i=0; i < MAX_OBJECTS; ++i ) {
        obj_live[i] = FALSE;
    }
}



u8 currentExplosion = 0;

static void addExplosion(fix16 pos_x, fix16 pos_y)
{
/*
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
*/
}



void updateCameraPos()
{
    s16 px = fix16ToInt(ship_pos_x);
    s16 py = fix16ToInt(ship_pos_y);

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
    if (changed & state & BUTTON_A)
    {
        for (s16 i = MAX_OBJECTS - MAX_PLAYER_SHOTS; i < MAX_OBJECTS; ++i)
        {

            if (obj_live[i] == FALSE)
            {
                // create a new one
                XGM_startPlayPCM(SND_LASER, 1, SOUND_PCM_CH2);

                obj_pos_x[i] = ship_pos_x+ FIX16(SHOT_OFFSET_X) ;// + fix16Mul(thrustX[shipDir], FIX16(2.0));
                obj_pos_y[i] = ship_pos_y+ FIX16(SHOT_OFFSET_Y) ;// + fix16Mul(thrustY[shipDir], FIX16(2.0));
                obj_speed_x[i] = ship_speed_x + (thrustX[shipDir] << 4 );
                obj_speed_y[i] = ship_speed_y + (thrustY[shipDir] << 4 );
                obj_live[i] = TRUE;
                obj_ticks[i] = 0; // you do need this.
                break;
            }
        }
    }
}

void handleInput()
{

    u16 value = JOY_readJoypad(JOY_1);
    // check rotation every frame.
    if (value & BUTTON_LEFT)
    {
        shipDir -=angleStep;
        // ship rotation with 32 total frames and 256 total dirs. 1 frame has to cover
        //  8 dirs.   so frame 0 could cover dirs  252 253 254 255  0 1 2 3
        //        .   so frame 1 covers dirs  4 5 6 7 8 9 10 11
        u8 tmpDir = shipDir + 4;
        SPR_setAnim(ship_sprite, tmpDir >> 3);
    }
    else if (value & BUTTON_RIGHT)
    {
        shipDir +=angleStep;
        u8 tmpDir = shipDir + 4;
        SPR_setAnim(ship_sprite, tmpDir >> 3);

    }

    if( tick & 0x01 ) {
        // check thrust every other frame.
        if (value & BUTTON_UP)
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
                ship_accel_x = FIX16(-0.07);
/*
                ship_accel_x = -( ship_speed_x >> 3 );
                if ( ship_accel_x == FIX16(0.0 ) ){
                    ship_accel_x = FIX16(-0.05);
                }
*/
                ship_speed_x += ship_accel_x;
                if( ship_speed_x <= FIX16(0.0) ) {
                    ship_speed_x = FIX16(0.0);
                    ship_accel_x = FIX16(0.0);
                }
            } else if( ship_speed_x < FIX16(0.0) ) {
                ship_accel_x = FIX16(0.07);
/*
                ship_accel_x =  -(ship_speed_x >> 3);
                if ( ship_accel_x == FIX16(0.0 ) ){
                    ship_accel_x = FIX16(0.05);
                }*/
                ship_speed_x += ship_accel_x;
                if( ship_speed_x >= FIX16(0.0) ) {
                    ship_speed_x = FIX16(0.0);
                    ship_accel_x = FIX16(0.0);
                }
            }

            if( ship_speed_y > FIX16(0.0) ) {
                ship_accel_y = FIX16(-0.07);
/*
                ship_accel_y = -( ship_speed_y >> 3 );
                if ( ship_accel_y == FIX16(0.0 ) ){
                    ship_accel_y = FIX16(-0.05);
                }
*/
                ship_speed_y += ship_accel_y;
                if( ship_speed_y <= FIX16(0.0) ) {
                    ship_speed_y = FIX16(0.0);
                    ship_accel_y = FIX16(0.0);
                }
            } else if( ship_speed_y < FIX16(0.0) ) {
                ship_accel_y = FIX16(0.07);
/*
                ship_accel_y = -( ship_speed_y >> 3 );
                if ( ship_accel_y == FIX16(0.0 ) ){
                    ship_accel_y = FIX16(0.05);
                }
*/
                ship_speed_y += ship_accel_y;
                if( ship_speed_y >= FIX16(0.0) ) {
                    ship_speed_y = FIX16(0.0);
                    ship_accel_y = FIX16(0.0);
                }
            }

        }

    } 


}

void update()
{

    // update the player
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


    // deactiveate shots if they've been around too long.
    for (u16 i = MAX_OBJECTS - MAX_PLAYER_SHOTS; i < MAX_OBJECTS; ++i)
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

            s16 x = fix16ToInt(obj_pos_x[i]) - camPosX + MAP_HALF_WIDTH;
            s16 y = fix16ToInt(obj_pos_y[i]) - camPosY + MAP_HALF_HEIGHT;
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


/*
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
*/

    updateCameraPos();
    SPR_setPosition(ship_sprite, fix16ToInt(ship_pos_x) - camPosX + MAP_HALF_WIDTH, fix16ToInt(ship_pos_y) - camPosY + MAP_HALF_HEIGHT);

    //SPR_setPosition( ship_sprite, fix16ToInt( ship_pos_x ), fix16ToInt( ship_pos_y ) );
}

static void checkCollisions()
{

/*
    for (u16 i = 0; i < MAX_ENEMIES; ++i)
    {
        if (enemies[i].active == TRUE)
        {
            // check if ship has hit
            if ((enemies[i].pos_x + enemies[i].hitbox_x1) < (ship_pos_x + player.hitbox_x2) &&
                    (enemies[i].pos_x + enemies[i].hitbox_x2) > (ship_pos_x + player.hitbox_x1) &&
                    (enemies[i].pos_y + enemies[i].hitbox_y1) < (ship_pos_y + player.hitbox_y2) &&
                    (enemies[i].pos_y + enemies[i].hitbox_y2) > (ship_pos_y + player.hitbox_y1))
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
*/


    for (u16 i = 0; i < MAX_OBJECTS - MAX_PLAYER_SHOTS; ++i)
    {
        if (obj_live[i] == TRUE)
        {
/*
            // check if ship has hit
            if ((obj_pos_x[i] + FIX16(2)) < (ship_pos_x + FIX16(3) ) &&
                (obj_pos_x[i] + obj_hit_w[i] ) > (ship_pos_x + FIX16(21) )  &&
                (obj_pos_y[i] + FIX16(2)) < (ship_pos_y + FIX16(3) ) &&
                (obj_pos_y[i] + obj_hit_w[i]) > (ship_pos_y + FIX16(21) ))
            {
                obj_live[i] = FALSE;
                SPR_setVisibility(obj_sprites[i], HIDDEN);

                addExplosion(obj_pos_x[i], obj_pos_y[i]);
            }
*/
            for (u8 j = MAX_OBJECTS - MAX_PLAYER_SHOTS; j < MAX_OBJECTS; ++j)
            {

/*
        char message[40];
        sprintf( message, "  check i: %d j: %d  ", i, j   );
        VDP_drawText(message, 1,1 );
        char objX[10];
        fix16ToStr( obj_pos_x[i], objX, 4 );
        char objY[10];
        fix16ToStr( obj_pos_y[i], objY, 4 );
        sprintf( message, "obj x: %s y: %s ", objX, objY);
        VDP_drawText(message, 1,2 );

        fix16ToStr( obj_pos_x[j], objX, 4 );
        fix16ToStr( obj_pos_y[j], objY, 4 );
        sprintf( message, "shot x: %s y: %s ", objX, objY);
        VDP_drawText(message, 1,3 );
*/

                if ( obj_live[j] == TRUE &&
                     (obj_pos_x[i] + FIX16(2))     < (obj_pos_x[j] + FIX16(4)) &&
                     (obj_pos_x[i] + obj_hit_w[i]) > (obj_pos_x[j] + FIX16(4)) &&
                     (obj_pos_y[i] + FIX16(2))     < (obj_pos_y[j] + FIX16(4)) &&
                     (obj_pos_y[i] + obj_hit_w[i]) > (obj_pos_y[j] + FIX16(4)))
                {



                    obj_live[i] = FALSE;
                    SPR_setVisibility(obj_sprites[i], HIDDEN);
                    obj_live[j] = FALSE;
                    SPR_setVisibility(obj_sprites[j], HIDDEN);

                    XGM_startPlayPCM(SND_EXPLOSION, 10, SOUND_PCM_CH3);
                    addExplosion(obj_pos_x[i], obj_pos_y[i]);
                }
            }
        }
    }

}


void createPlayerShots()
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
        /*
           playerShots[i].hitbox_x1 = FIX16(3);
           playerShots[i].hitbox_y1 = FIX16(3);
           playerShots[i].hitbox_x2 = FIX16(4);
           playerShots[i].hitbox_y2 = FIX16(4);
           */
        obj_sprites[i] = SPR_addSprite(&shot, xpos, ypos, TILE_ATTR(PAL0, 0, FALSE, FALSE));
        SPR_setAnim(obj_sprites[i],2);
    }
}

void createRocks(u8 rockCount )
{

    for (u8 i = 0; i < MAX_ROCKS; ++i)
    {
        if( i < rockCount ) {
            obj_pos_x[i] = FIX16(random() % (MAP_WIDTH - 32) - MAP_HALF_WIDTH );
            obj_pos_y[i] = FIX16(random() % (MAP_HEIGHT - 32) - MAP_HALF_HEIGHT );
            u8 rot = random();
            fix16 vel = FIX16(5.0);
            obj_speed_x[i] = fix16Mul( vel, thrustX[rot] );
            obj_speed_y[i] = fix16Mul( vel, thrustY[rot] );
            obj_live[i] = TRUE;
            obj_hit_w[i] = FIX16(30);
            obj_sprites[i] = SPR_addSprite(&rock, -32, -32, TILE_ATTR(PAL3, 0, FALSE, FALSE));
            SPR_setAnim(obj_sprites[i], i % 4);
        } else {
            obj_pos_x[i] = FIX16(-32);
            obj_pos_y[i] = FIX16(-32);

            obj_speed_x[i] = FIX16(0);
            obj_speed_y[i] = FIX16(0);
            obj_live[i] = FALSE;
            obj_hit_w[i] = FIX16(0);
            obj_sprites[i] = NULL;
        }

    }

}

void createEnemies()
{
    /*
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
       */
}

static void createExplosions()
{
    fix16 xpos = FIX16(0);
    fix16 ypos = FIX16(264);

    for (u16 i = 0; i < MAX_EXPLOSIONS; ++i)
    {
        /*
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
           */
    }
}

int main(bool hard)
{
    clear_objs();
    /////////////////////////////////////////////////////////////////////////////////
    // Setup Thrust Table
    u16 pos = 0;
    for( s16 i = 64; i >= 0; i-- ) {
        thrustX[pos] =  fix16Div( cosFix16(i * 4), FIX16(5));
        max_speed_x[pos] = fix16Mul( max_speed, cosFix16(i*4));
        thrustY[pos] = -fix16Div( sinFix16(i * 4), FIX16(5));  // flip the Y. We're not mathemagicians
        max_speed_y[pos] = -fix16Mul( max_speed, sinFix16(i*4));
        pos++;
    }
    for( s16 i = 255; i > 64; i-- ) {
        //thrustX[pos] = cosFix16(i * 4);
        //max_speed_x[pos] = fix16Mul( max_speed, thrustX[pos]);
        //thrustY[pos] = -sinFix16(i * 4);
        //max_speed_y[pos] = fix16Mul( max_speed, thrustY[pos]);
        thrustX[pos] =  fix16Div( cosFix16(i * 4), FIX16(5));
        max_speed_x[pos] = fix16Mul( max_speed, cosFix16(i*4));
        thrustY[pos] = -fix16Div( sinFix16(i * 4), FIX16(5));  // flip the Y. We're not mathemagicians
        max_speed_y[pos] = -fix16Mul( max_speed, sinFix16(i*4));
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
    int ind = TILE_USER_INDEX;
    VDP_loadTileSet(&plane_b_tileset, ind, DMA);

    // init background map
    map_b = MAP_create(&plane_b_map, BG_B, TILE_ATTR_FULL(PAL1, FALSE, FALSE, FALSE, ind));
    camPosX = 0 - SCR_WIDTH / 2;
    camPosY = 0 - SCR_HEIGHT / 2;
    MAP_scrollTo(map_b, camPosX, camPosY);

    /////////////////////////////////////////////////////////////////////////////////
    // Init sprite engine with defaults
    SPR_initEx(900);
    ship_pos_x = FIX16(- 12);
    ship_pos_y = FIX16(- 12);
    //ship_sprite = SPR_addSprite(&ship, fix16ToInt(ship_pos_x) - camPosX, fix16ToInt(ship_pos_y) - camPosY, TILE_ATTR(PAL2, 0, FALSE, FALSE));
    ship_sprite = SPR_addSprite(&ship, fix16ToInt(ship_pos_x) - camPosX, fix16ToInt(ship_pos_y) - camPosY, TILE_ATTR(PAL2, 0, FALSE, FALSE));

    SPR_setAnim(ship_sprite, 0);

    //player.hitbox_x1 = FIX16(3);
    //player.hitbox_y1 = FIX16(3);
    //player.hitbox_x2 = FIX16(13);
    //player.hitbox_y2 = FIX16(13);
    //playerRotation = MIN_ROTATION_INDEX;
    // playerRotation = FIX16(12);



    createPlayerShots();
    //createExplosions();
    createRocks(MAX_ROCKS);
    //createEnemies();

    JOY_setEventHandler(&inputCallback);

    char message[40];
    while(TRUE)
    {
        tick++; // increment for decision making

        // read game pads and make initial calcs
        handleInput();
        /*
        // output calcs
        char thrX[10];
        fix16ToStr( thrustX[shipDir], thrX, 4 );
        char thrY[10];
        fix16ToStr( thrustY[shipDir], thrY, 4 );
        sprintf( message, " shipDir: %d tx: %s ty %s ", shipDir, thrX, thrY   );
        VDP_drawText(message, 1,1 );
        char spdX[10];
        fix16ToStr( ship_speed_x, spdX, 4 );
        char spdY[10];
        fix16ToStr( ship_speed_y, spdY, 4 );
        sprintf( message, " sx: %s sy: %s ", spdX, spdY);
        VDP_drawText(message, 1,2 );
        char accX[10];
        fix16ToStr( ship_accel_x, accX, 4 );
        char accY[10];
        fix16ToStr( ship_accel_y, accY, 4 );
        sprintf( message, " ax: %s ay: %s ",  accX, accY);

        VDP_drawText(message, 1,3 );
        char maxX[10];
        fix16ToStr( max_speed_x[shipDir], maxX, 4 );
        char maxY[10];
        fix16ToStr( max_speed_y[shipDir], maxY, 4 );
        sprintf( message, " shipDir: %d mx: %s my %s ", shipDir, maxX, maxY   );
        VDP_drawText(message, 1,4 );

        char posX[10];
        fix16ToStr( ship_pos_x, posX, 4 );
        char posY[10];
        fix16ToStr( ship_pos_y, posY, 4 );
        sprintf( message, " px: %s py: %s ",  posX, posY);
        VDP_drawText(message, 1,5 );

        sprintf( message, " cam x: %d cam y: %d   ", camPosX, camPosY );
        VDP_drawText(message, 1,6 );


        char shtX[10];
        fix16ToStr( obj_pos_x[MAX_OBJECTS-MAX_PLAYER_SHOTS], shtX, 4 );
        char shtY[10];
        fix16ToStr( obj_pos_y[MAX_OBJECTS-MAX_PLAYER_SHOTS], shtY, 4 );
        sprintf( message, "SHOT x: %s y: %s t: %d ",  shtX, shtY, obj_ticks[MAX_OBJECTS-MAX_PLAYER_SHOTS]);
        VDP_drawText(message, 1,7 );
        */



        // move sprites
        update();

        checkCollisions();

        // SGDK stuff.
        SPR_update();
        SYS_doVBlankProcess();
    }
    return 0;
}
