#include <genesis.h>
#include "resources.h"


/*
   Ateroids was 16bit fixed point for a 1024x1024 display. So we
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


/*
// will this be slower than making arrays for speedX, speedY,  positionX, positionY
typedef struct
{
Sprite *sprite;
fix16 spd_x;
fix16 spd_y;

fix16 pos_x;
fix16 pos_y;


u8   ship_dir;  //  increment/decrement by 3 though

} CP_SPRITE;
fix16 acc_x;
fix16 acc_y;
*/



/* other thigns tonote
   1. They appear to use 8 bits for ShipDir for angles but step by 3.
   2. Movement doesn't care about the overall vector. It handles
   X and Y independantly. While not techincally correct, it seems
   convincing enough when playing
   3. Acceleration
   a. They lookup thrust for X and Y indepednely (they use quadrants and shifting to save space
   but we can make the whole range for lookup
   b. They double the thrust for ship with a left shift (probably not necessary here)
   then add thrust to accelerationX and Accel Y (independlty)
   c. Add Accel to ShipSpeed.
   d. Check max velocity. 

   E. elshewere, in the general object updates they just add the speed
   to the position (x and y independntly as usual) 

   4. Max velocity. They just check for max X and max Y separately 
   and turn accleration to 0 if they have ot.

   5. Deceleration?
   if not thrusting but IS MOVING, the game changes accleartion in that direction to -1
   ( so presumably speed is eventually 0 and accleration becomes 0) 
   */

// assume angle are 0 though 255
// there are fewer frames, but we just reuse frames for similar angles.
// we could actually just pre-compute 1 quadrant to save space at the cost of speed, but
// I'm betting for a simple asteroids game the memory will be just fine.
fix16 thrustX[256];
fix16 thrustY[256];  // Could eliminate this by using an offset 
u8 shipDir = 0; // 0 is up
const u8 angleStep = 3;

// Just trying continuous memory instead of an array of structs
//const u8 max_objects = 55;
#define MAX_OBJECTS 35
Sprite *sprite[MAX_OBJECTS];
fix16 speed_x[MAX_OBJECTS];
fix16 speed_y[MAX_OBJECTS];
fix16 pos_x[MAX_OBJECTS];
fix16 pos_y[MAX_OBJECTS];

Sprite *ship_sprite;
fix16 ship_accel_x;
fix16 ship_accel_y;
fix16 ship_speed_x;
fix16 ship_speed_y;
fix16 ship_pos_x;
fix16 ship_pos_y;

const s16 playfield_width = 320;
const s16 playfield_height = 224;


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
    if (value & BUTTON_UP)
    {
        /*
        // doPlayerUpdate = TRUE;
        int rot = fix32ToInt(playerRotation);
        fix32 temp_x = (player.vel_x+ fix32Mul(acc, deltaX[rot]));
        fix32 temp_y = (player.vel_y+ fix32Mul(acc, deltaY[rot]));
        fix32 speedsqr =(fix32Mul(temp_x, temp_x)+ fix32Mul(temp_y, temp_y));

        if (speedsqr < FIX32(4))
        { // square of 1.2 == 1.44
        player.vel_x = temp_x;
        player.vel_y = temp_y;
        }
        else
        {
        // get direction and rescale speed for it.
        u16 dir = ApproxAtan2(temp_y, temp_x);
        fix32 s = sinFix32(dir);
        fix32 c = cosFix32(dir);
        player.vel_x = fix32Mul(FIX32(2), c);
        player.vel_y = fix32Mul(FIX32(2), s);
        }
        */
    }  
    else
    {
        /*
           ++doDec;
           if( doDec == 10) {
           doDec = 0;
           fix32 speedsqr = (fix32Mul(player.vel_x, player.vel_x)+ fix32Mul(player.vel_y, player.vel_y));
           u16 dir = ApproxAtan2(player.vel_y, player.vel_x);
           fix32 s = sinFix32(dir);
           fix32 c = cosFix32(dir);
           fix16 speed = fix16Sqrt( fix32ToFix16(speedsqr));
           speed =  speed- FIX16(0.1);
           if( speed < FIX16(0.1)){
           player.vel_x = FIX32(0);
           player.vel_y = FIX32(0);
           } else {
           player.vel_x = fix32Mul(fix16ToFix32(speed), c);
           player.vel_y = fix32Mul(fix16ToFix32(speed), s);
           }
           }
           */
    }
}



void update() {
    /*
    // if( doPlayerUpdate == TRUE ) {
    player.pos_x = (player.pos_x+ player.vel_x);
    player.pos_y = (player.pos_y+ player.vel_y);

    if (player.pos_x < FIX32(-6.0))
    {
    player.pos_x = FIX32(-6.0);
    }
    else if (player.pos_x > FIX32(MAP_WIDTH - PLAYER_WIDTH + 6))
    {
    player.pos_x = FIX32(MAP_WIDTH - PLAYER_WIDTH + 6);
    }

    if (player.pos_y < FIX32(-6))
    {
    player.pos_y = FIX32(-6);
    }
    else if (player.pos_y > FIX32(MAP_HEIGHT - PLAYER_HEIGHT + 6))
    {
    player.pos_y = FIX32(MAP_HEIGHT - PLAYER_HEIGHT + 6);
    }
    */

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
    //
    //   so we actually want to work from 
    //    64 down to 0
    //    255 down to 65
    //


    u16 pos = 0;
    for( s16 i = 64; i >= 0; i-- ) {
        thrustX[pos] = cosFix16(i * 4);
        thrustY[pos] = sinFix16(i * 4);
        pos++;
    }
    for( s16 i = 255; i > 65; i-- ) {
        thrustX[pos] = cosFix16(i * 4);
        thrustY[pos] = sinFix16(i * 4);
        pos++;
    }

    // Screen init
    VDP_setScreenWidth320();

    PAL_setPalette(PAL0, ship_pal.data, CPU);
    PAL_setPalette(PAL1, shot_pal.data, CPU);

    SPR_initEx(900);
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
        handleInput();
        char thrX[10];
        fix16ToStr( thrustX[shipDir], thrX, 4 );
        char thrY[10];
        fix16ToStr( thrustY[shipDir], thrY, 4 );
        sprintf( message, " shipDir: %d tx: %s ty %s ", shipDir, thrX, thrY   );
        VDP_drawText(message, 1,1 );
        update();
        //checkCollisions();

        //SPR_setAnim(bship, frm);
        //SPR_setAnim(gship, frm);
        //SPR_setAnim(yship, frm);
        //frm++;
        //if( frm > 31 ) frm = 0;


        SPR_update();
        SYS_doVBlankProcess();
    }

    return 0;
}

