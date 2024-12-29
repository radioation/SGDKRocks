#include <genesis.h>
#include "resources.h"


/*
   Asteroids was 16bit fixed point for a 1024x1024 display. So we
   probaly don't need Fix32 for 320x224

   ```asm
   AstXSpeed       .eq     $0223  {addr/27}        ;Through $023D. Asteroid horiz speed. 255-192=Left, 1-63=Right.
   ShipXSpeed      .eq     $023e                   ;Ship horizontal speed.
   SaucerXSpeed    .eq     $023f                   ;Saucer horizontal speed.

   AstYSpeed       .eq     $0246  {addr/27}        ;Through $0260. Asteroid vert speed. 255-192=Down, 1-63=Up.
   ShipYSpeed      .eq     $0261                   ;Ship vertical speed.
   SaucerYSpeed    .eq     $0262                   ;Saucer vertical speed.

   AstXPosHi       .eq     $0269                   ;Through $0283. Asteroid horz position, high byte.
   ShipXPosHi      .eq     $0284                   ;Ship X position, high byte.
   ScrXPosHi       .eq     $0285                   ;Saucer X position, high byte.

   AstYPosHi       .eq     $028c                   ;Through $02A6. Asteroid vert position, high byte.
   ShipYPosHi      .eq     $02a7                   ;Ship Y position, high byte.
   ScrYPosHi       .eq     $02a8                   ;Saucer Y position, high byte.

   AstXPosLo       .eq     $02af                   ;Through $02C9. Asteroid horz position, low byte.
   ShipXPosLo      .eq     $02ca                   ;Ship X position, low byte.
   ScrXPosLo       .eq     $02cb                   ;Saucer X position, low byte.

   AstYPosLo       .eq     $02d2                   ;Through $02EC. Asteroid vert position, low byte.
   ShipYPosLo      .eq     $02ed                   ;Ship Y position, low byte.
   ScrYPosLo       .eq     $02ee                   ;Saucer Y position, low byte.
   ```

   It also had Accleration as X/Y in 8 bits
   ```asm
   ShipXAccel      .eq     $64                     ;Ship acceleration in the X direction.
   ShipYAccel      .eq     $65                     ;Ship acceleration in the Y direction.
   ```
   but we'll keep that 16 on sega
   */



// They used angles over 0 though 255
// I have fewer sprite frames than 256, but can reuse frames for similar angles.
fix16 thrustX[256];
fix16 thrustY[256];  
u8 shipDir = 0; // 0 is up
const u8 angleStep = 3; // turn rate.


// Just trying continuous memory instead of an array of structs in original implementation.
#define MAX_OBJECTS 35
Sprite *sprite[MAX_OBJECTS];
fix16 speed_x[MAX_OBJECTS];
fix16 speed_y[MAX_OBJECTS];
fix16 pos_x[MAX_OBJECTS];
fix16 pos_y[MAX_OBJECTS];

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
u8 tick = 0;

void handleInput() {

    u16 value = JOY_readJoypad(JOY_1);

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
            ship_accel_x += (thrustX[shipDir] >> 2);
            ship_speed_x += ship_accel_x; 
            if( ship_speed_x > FIX16(0.0) && ship_speed_x >  max_speed_x[shipDir] ) {
                ship_speed_x = max_speed_x[shipDir];
                ship_accel_x = FIX16(0.0);
            } else if ( ship_speed_x < FIX16(0) && ship_speed_x <  max_speed_x[shipDir] ) {
                ship_speed_x = max_speed_x[shipDir];
                ship_accel_x = FIX16(0.0);
            }

            ship_accel_y += (thrustY[shipDir] >> 2);
            ship_speed_y += ship_accel_y; 
            if( ship_speed_y > FIX16(0.0) && ship_speed_y >  max_speed_y[shipDir] ) {
                ship_speed_y = max_speed_y[shipDir];
                ship_accel_y = FIX16(0.0);
            } else if ( ship_speed_y < FIX16(0.0) && ship_speed_y < max_speed_y[shipDir] ) {
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
                ship_accel_x = -( ship_speed_x >> 3 );
                if ( ship_accel_x == FIX16(0.0 ) ){
                   ship_accel_x = FIX16(-0.05);
                }
                ship_speed_x += ship_accel_x; 
                if( ship_speed_x <= FIX16(0.0) ) {
                    ship_speed_x = FIX16(0.0);
                    ship_accel_x = FIX16(0.0);
                }
            } else if( ship_speed_x < FIX16(0.0) ) {
                ship_accel_x =  -(ship_speed_x >> 3);
                if ( ship_accel_x == FIX16(0.0 ) ){
                   ship_accel_x = FIX16(0.05);
                }
                ship_speed_x += ship_accel_x; 
                if( ship_speed_x >= FIX16(0.0) ) {
                    ship_speed_x = FIX16(0.0);
                    ship_accel_x = FIX16(0.0);
                }
            }

            if( ship_speed_y > FIX16(0.0) ) {
                ship_accel_y = -( ship_speed_y >> 3 );
                if ( ship_accel_y == FIX16(0.0 ) ){
                   ship_accel_y = FIX16(-0.05);
                }
                ship_speed_y += ship_accel_y; 
                if( ship_speed_y <= FIX16(0.0) ) {
                    ship_speed_y = FIX16(0.0);
                    ship_accel_y = FIX16(0.0);
                }
            } else if( ship_speed_y < FIX16(0.0) ) {
                ship_accel_y = -( ship_speed_y >> 3 );
                if ( ship_accel_y == FIX16(0.0 ) ){
                   ship_accel_y = FIX16(0.05);
                }
                ship_speed_y += ship_accel_y; 
                if( ship_speed_y >= FIX16(0.0) ) {
                    ship_speed_y = FIX16(0.0);
                    ship_accel_y = FIX16(0.0);
                }
            }

        } 

    }
}



void update() {

    ship_pos_x += ship_speed_x;
    if( ship_pos_x < FIX16(-24.0) ) {
        ship_pos_x = FIX16(344.0);
    } else if( ship_pos_x > FIX16(344.0) ) {
        ship_pos_x = FIX16(-24.0);
    }
    ship_pos_y += ship_speed_y;
    if( ship_pos_y < FIX16(-24.0) ) {
        ship_pos_y = FIX16(244.0);
    } else if( ship_pos_y > FIX16(248.0) ) {
        ship_pos_y = FIX16(-24.0);
    }
    SPR_setPosition( ship_sprite, fix16ToInt( ship_pos_x ), fix16ToInt( ship_pos_y ) );

}


int main(bool hard)
{
    // compute thrust
    /**
     *  \brief
     *      Compute sinus of specified value and return it as fix16.<br>
     *      The input value is an integer defined as [0..1024] range corresponding to radian [0..2PI] range.
     *
     *   
     *      sin_tab shows that 1023 is the max and it's not == 0  
     *     const fix16 sintab_f16[1024] = {
     *             FIX16(0.0000),      // 0
     *             FIX16(0.0061),      // 1      360/1024 is 0.3515625 degrees  360/256 is 1.40625
     *             FIX16(0.0123),      // 2
     *             FIX16(0.0184),      // 3
     *             FIX16(0.0245),      // 4   << 
     *             FIX16(0.0307),      // 5 
     *               . . . 
     *
     FIX16(0.9999),      // 254
     FIX16(1.0000),      // 255

     FIX16(1.0000),      // 256    // 90 degrees is at 256 (which is 1/4 1024 ) and SIN becomes 1 as expected
                         // 1/4 of 256 is what? 64.   ( so 64 * 4 is 256 we can just multipley each index by 4 to get sin table )

                         FIX16(1.0000),      // 257
                         FIX16(0.9999),      // 258
     *
     *
     FIX16(0.0000),      // 512    // and of course 180 is at 512 (1/2 of 1024) so SIN becomes 0 
                         // 2/4 of 256 is 128  
     *
     *
     *
     *
     *             FIX16(-0.0061),     // 1023
     */


    // fix16 sinFix16(u16 value);
    // since 12 o'clock is 0, we
    //   1) start at PI/2 ( 90 deg)
    //   2) drop down to 0 (whicih is also 2PI)
    //   3) from 2PI|360deg drop down to 270 deg
    //   4) 270 drop down to 180 deg
    //   5) 180 drop down to 90 deg
    //
    //   so we actually want to work from 
    //    64 down to 0
    //    255 down to 65
    //
    u16 pos = 0;
    for( s16 i = 64; i >= 0; i-- ) {
        thrustX[pos] = cosFix16(i * 4);
        max_speed_x[pos] = fix16Mul( max_speed, thrustX[pos]);
        thrustY[pos] = -sinFix16(i * 4);  // flip the Y. We're not mathemagicians
        max_speed_y[pos] = fix16Mul( max_speed, thrustY[pos]);
        pos++;
    }
    for( s16 i = 255; i > 64; i-- ) {
        thrustX[pos] = cosFix16(i * 4);
        max_speed_x[pos] = fix16Mul( max_speed, thrustX[pos]);
        thrustY[pos] = -sinFix16(i * 4);
        max_speed_y[pos] = fix16Mul( max_speed, thrustY[pos]);
        pos++;
    }

    // Screen init
    VDP_setScreenWidth320();

    PAL_setPalette(PAL0, ship_pal.data, CPU);
    PAL_setPalette(PAL1, shot_pal.data, CPU);

    SPR_initEx(900); // MOAR sprites.
    ship_pos_x = FIX16( playfield_width/2 - 8 );
    ship_pos_y = FIX16( playfield_height/2 - 8 );
    ship_sprite = SPR_addSprite(&redship, fix16ToInt(ship_pos_x), fix16ToInt(ship_pos_y), TILE_ATTR(PAL0, 0, FALSE, FALSE));
    SPR_setAnim(ship_sprite, 0);

    Sprite* bship = SPR_addSprite(&blueship, 48, 48, TILE_ATTR(PAL0, 0, FALSE, FALSE));
    Sprite* gship = SPR_addSprite(&greenship, 72, 48, TILE_ATTR(PAL0, 0, FALSE, FALSE));
    Sprite* yship = SPR_addSprite(&yellowship, 96, 48, TILE_ATTR(PAL0, 0, FALSE, FALSE));

    u8 frm = 0;
    char message[40];
    while(TRUE)
    {
        tick++; // increment for decision making

        // read game pads and make initial calcs
        handleInput();
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

        // move sprites
        update();

        // save it for the game. 
        //checkCollisions();

        SPR_setAnim(bship, frm);
        SPR_setAnim(gship, frm);
        SPR_setAnim(yship, frm);
        frm++;
        if( frm > 31 ) frm = 0;


        SPR_update();
        SYS_doVBlankProcess();
    }

    return 0;
}

