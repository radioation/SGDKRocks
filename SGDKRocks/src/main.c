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
// leave 8 of the objs for UFOs and player shots  ( 4 player shots, 2 UFOS, and 2 ufo shots )
#define MAX_ROCKS           37
#define MAX_EXPLOSIONS      5

#define UFO_SLOT            37

#define UFO_SPAWN_TIME  0x02f8
static u16 ufoTick = 0; 


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
fix16 obj_hit_w[MAX_OBJECTS];
s16 obj_ticks[MAX_OBJECTS];
bool obj_live[MAX_OBJECTS];

#define NO_TYPE 0
#define PLAYER 1
#define PLAYER_SHOT 3

#define ROCK 20
#define MID_ROCK 50
#define SMALL_ROCK 75
#define UFO 200
#define SMALL_UFO 400
#define STAR 150 
#define SHARD 350 

u8 obj_type[MAX_OBJECTS];

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
Sprite *blink_sprite;
static u8 prevBlinkState = 0;
static u8 currBlinkState = 0;
static bool isBlinkDown = FALSE;
fix16 ship_blink_pos_x = FIX16(0.0);
fix16 ship_blink_pos_y = FIX16(0.0);


/////////////////////////////////////////////////////////////////////////////////
// explosions

Sprite *explosion_sprites[MAX_EXPLOSIONS];
fix16 explosion_pos_x[MAX_EXPLOSIONS];
fix16 explosion_pos_y[MAX_EXPLOSIONS];
s16 explosion_ticks[MAX_EXPLOSIONS];
bool explosion_live[MAX_EXPLOSIONS];



// utils

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
            s16 x = fix16ToInt(pos_x)- camPosX + MAP_HALF_WIDTH;
            s16 y = fix16ToInt(pos_y)- camPosY + MAP_HALF_HEIGHT;
    if (explosion_live[currentExplosion] == FALSE)
    {
        // use it
        //explosion_pos_x[currentExplosion] = x;
        //explosion_pos_y[currentExplosion] = y;

        explosion_live[currentExplosion] = TRUE;
        explosion_ticks[currentExplosion] = 0;

        SPR_setVisibility(explosion_sprites[currentExplosion], VISIBLE);
        SPR_setPosition(explosion_sprites[currentExplosion], x, y );

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
        if( !isBlinkDown ) {
            isBlinkDown = TRUE;
            SPR_setVisibility(blink_sprite, VISIBLE);
        }
    } else {
        if( isBlinkDown ) {
            // just released blink
            isBlinkDown = FALSE;
            ship_pos_x = ship_pos_x + ( thrustX[shipDir] << 8 );
            ship_pos_y = ship_pos_y + ( thrustY[shipDir] << 8 );
            SPR_setVisibility(blink_sprite, HIDDEN);
        }
    }

    if( tick & 0x01 ) {
        // check thrust every other frame.
        if (currState & BUTTON_UP)
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

void createUfo( )  {
}

void aimUfoShot()  {
}

void updateUfo() 
{
    // some pieces of real asteroids

    // 


    /*  They only updated every 4 frames
6b93: a5 5c        UpdateScr       lda     FrameTimer              ;Update saucers only every 4th frame
6b95: 29 03                        and     #$03                    ;Is this the 4th frame?
6b97: f0 01                        beq     ChkScrExplode           ;If so, branch to continue processing 
*/

    /*
       They used a timer, duh
6bb7: ce f7 02     UpdateScrTimer  dec     ScrTimer                ;Is it time to re-spawn a saucer?
6bba: d0 dd                        bne     EndUpdateScr            ;If not, branch to exit

*/
    ufoTick++; 
    if( obj_live[UFO_SLOT] == FALSE ) {
        if( ufoTick >= UFO_SPAWN_TIME ) {
            // create UFO
            createUfo();
            ufoTick = 0;
        }
        return;
    } else {
        // do motion
        // randomly change Y veolcity every 128 frames
        if (ufoTick % 0x03 ) {
        }
    }


    /*
       Big saucers shoot randomly


6c5c: 4a                           lsr     A                       ;If its a large saucer, prepare to shoot a random shot
6c5d: f0 06                        beq     GetScrShpDistance       ;If its a small saucer, prepare to shoot an aimed shot
6c5f: 20 b5 77                     jsr     GetRandNum              ;Get a random number
6c62: 4c c2 6c                     jmp     ScrShoot                ;Prepare to generate a saucer bullet
*/




    /*
       They randomly vary Y velocity 
6c34: a5 5c        ScrYVelocity    lda     FrameTimer              ;Randomly change saucer Y velocity every 128 frames
6c36: 0a                           asl     A                       ;Is it time to change the saucer's Y velocity?
6c37: d0 0c                        bne     ChkScrUpdate            ;If not, branch
6c39: 20 b5 77                     jsr     GetRandNum              ;Get a random number
6c3c: 29 03                        and     #%00000011              ;Keep the lower 2 bits for index into table below

6c3f: bd d1 6c                     lda     ScrYSpeedTbl,x          ;Load new Y velocity value for the saucer
6c42: 8d 62 02                     sta     SaucerYSpeed
*/

    /* 
       small ufo aim

6c92: ad ed 02                     lda     ShipYPosLo
6c95: 38                           sec                             ;Get difference between saucer and ship X position low byte
6c96: ed ee 02                     sbc     ScrYPosLo
6c99: 85 0b                        sta     GenByte0B               ;Save result
6c9b: ad a7 02                     lda     ShipYPosHi              ;Get difference between saucer and ship X position high byte
6c9e: ed a8 02                     sbc     ScrYPosHi
6ca1: 20 ec 77                     jsr     NextScrShpDist          ;Calculate next frame saucer/ship Y distance
6ca4: a8                           tay                             ;Save Y distance data for bullet
; 
6ca5: 20 f0 76                     jsr     CalcScrShotDir          ;Calculate the small saucer's shot direction
*/


    /*
       They anticipate next distance for small saucer shots

77ec: 06 0b        NextScrShpDist  asl     GenByte0B
77ee: 2a                           rol     A                       ;Get the signed difference between
77ef: 06 0b                        asl     GenByte0B               ; the ship and saucer upper 4 bits
77f1: 2a                           rol     A
77f2: 38                           sec                             ;Predict next location of saucer with respect to the ship
77f3: e5 0c                        sbc     GenByte0C               ; by subtracting the current saucer XY velocity from the
77f5: 60                           rts                             ; from the saucer/ship distance
*/


    /* 
     * actual shot angle is based on x and y distances

     ; 
     ; Calculate small saucer shot velocity.
     ; 
76f0: 98           CalcScrShotDir  tya                             ;Load the Y distance between the saucer and the ship
76f1: 10 09                        bpl     ScrShotXDir             ;Is Y direction positive? If so, branch to do X direction
76f3: 20 08 77                     jsr     TwosCompliment          ;Calculate the 2's compliment of the Y distance
76f6: 20 fc 76                     jsr     ScrShotXDir             ;Calculate the X direction of the saucer shot
76f9: 4c 08 77                     jmp     TwosCompliment          ;Calculate the 2's compliment of the value in A

76fc: a8           ScrShotXDir     tay                             ;Save the modified Y shot distance
76fd: 8a                           txa                             ;Get the the raw X shot distance
76fe: 10 0e                        bpl     CalcScrShotAngle        ;Is X direction positive? If so, branch to calculate shot angle
7700: 20 08 77                     jsr     TwosCompliment          ;Calculate the 2's compliment of the value in A
7703: 20 0e 77                     jsr     CalcScrShotAngle        ;Calculate the small saucer's shot angle
7706: 49 80                        eor     #$80                    ;Set the appropriate quadrant for the bullet
; 
; 2's compliment.
; 
7708: 49 ff        TwosCompliment  eor     #$ff
770a: 18                           clc                             ;Calculate the 2's compliment of the value in A
770b: 69 01                        adc     #$01
770d: 60                           rts

; 
; Calculate small saucer shot angle.
; 
• Clear variables
]ShotXYDistance .var    $0c    {addr/1}

CalcScrShotAngle
770e: 85 0c                        sta     ]ShotXYDistance         ;Store shot modified X distance
7710: 98                           tya
7711: c5 0c                        cmp     ]ShotXYDistance         ;Is X and Y distance the same?
7713: f0 10                        beq     ShotAngle45             ;If so, angle is 45 degrees.  Branch to set and exit
7715: 90 11                        bcc     LookUpAngle             ;Is angle in lower 45 degrees of quadrant? if so, branch
7717: a4 0c                        ldy     ]ShotXYDistance         ;Swap X and Y components as the shot is
7719: 85 0c                        sta     ]ShotXYDistance         ; in the upper 45 degrees of the quadrant
771b: 98                           tya
771c: 20 28 77                     jsr     LookUpAngle             ;Look up angle but return to find proper quadrant
771f: 38                           sec                             ;Set the appropriate quadrant for the bullet
7720: e9 40                        sbc     #$40
7722: 4c 08 77                     jmp     TwosCompliment          ;Calculate the 2's compliment of the value in A

7725: a9 20        ShotAngle45     lda     #$20                    ;Player's ship is at a 45 degree angle to the saucer
7727: 60                           rts

7728: 20 6c 77     LookUpAngle     jsr     FindScrAngleIndex       ;Find the index in the table below for the shot angle
772b: bd 2f 77                     lda     ShotAngleTbl,x
772e: 60                           rts                             ;Look up the proper angle and exit

; 
; The following table divides 45 degrees of a circle into 16 pieces.  Its used
; to calculate the direction of a bullet from a small saucer to the player's
; ship.  The other angles in the circle are derived from this table.
; 
772f: 00 02 05 07+ ShotAngleTbl    .bulk   $00,$02,$05,$07,$0a,$0c,$0f,$11,$13,$15,$17,$19,$1a,$1c,$1d,$1f 
*/



    /*
       if score is lower than 35000 they add a small inaccuracy to the saucer shot

6ca8: 85 62                        sta     ScrBulletDir            ;Saucer shot direction is the same type of data as ship direction
6caa: 20 b5 77                     jsr     GetRandNum              ;Get a random number
6cad: a6 19                        ldx     ScoreIndex
6caf: b4 53                        ldy     PlayerScores+1,x        ;Is the player's score less than 35,000?
6cb1: c0 35                        cpy     #$35                    ;If so, add inaccuracy to small saucer's bullet
6cb3: a2 00                        ldx     #$00
*/





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


    for (u16 i = 0; i < MAX_EXPLOSIONS; ++i)
    {
        if (explosion_live[i] == TRUE)
        {
            explosion_ticks[i] += 1;
            if (explosion_ticks[i] < 9)
            {
                //  SPR_setFrame( explosions[i].sprite, explosions[i].ticks );
                //SPR_setPosition(explosion_sprites[i], fix16ToInt(explosions[i].pos_x) - camPosX, fix16ToInt(explosions[i].pos_y) - camPosY);
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
    if( isBlinkDown && ( tick & 0x04 ) ) {
        ship_blink_pos_x = ship_pos_x + ( thrustX[shipDir] << 8 );
        ship_blink_pos_y = ship_pos_y + ( thrustY[shipDir] << 8 );
        SPR_setPosition(ship_sprite, fix16ToInt(ship_blink_pos_x) - camPosX + MAP_HALF_WIDTH, fix16ToInt(ship_blink_pos_y) - camPosY + MAP_HALF_HEIGHT);
        SPR_setPosition(blink_sprite, fix16ToInt(ship_blink_pos_x) - camPosX + MAP_HALF_WIDTH, fix16ToInt(ship_blink_pos_y) - camPosY + MAP_HALF_HEIGHT);

    } else {
        SPR_setPosition(ship_sprite, fix16ToInt(ship_pos_x) - camPosX + MAP_HALF_WIDTH, fix16ToInt(ship_pos_y) - camPosY + MAP_HALF_HEIGHT);
        SPR_setPosition(blink_sprite, fix16ToInt(ship_pos_x) - camPosX + MAP_HALF_WIDTH, fix16ToInt(ship_pos_y) - camPosY + MAP_HALF_HEIGHT);
    }

    //SPR_setPosition( ship_sprite, fix16ToInt( ship_pos_x ), fix16ToInt( ship_pos_y ) );
}

void createRock(u8 i, u16 rockType, fix16 x, fix16 y ) {

    obj_pos_x[i] = x;
    obj_pos_y[i] = y;
    u8 rot = random();
    obj_live[i] = TRUE;
    fix16 vel = FIX16(7.0);
    if( rockType == ROCK ) {
        obj_hit_w[i] = FIX16(30);
        obj_sprites[i] = SPR_addSprite(&rock, -32, -32, TILE_ATTR(PAL3, 0, FALSE, FALSE));
    } else if ( rockType == MID_ROCK ) {
        vel = FIX16(10.0);
        obj_sprites[i] = SPR_addSprite(&mid_rock, -32, -32, TILE_ATTR(PAL3, 0, FALSE, FALSE));
        obj_hit_w[i] = FIX16(22);
    } else if ( rockType == SMALL_ROCK ) {
        vel = FIX16(13.0);
        obj_sprites[i] = SPR_addSprite(&small_rock, -32, -32, TILE_ATTR(PAL3, 0, FALSE, FALSE));
        obj_hit_w[i] = FIX16(14);
    }
    obj_speed_x[i] = fix16Mul( vel, thrustX[rot] );
    obj_speed_y[i] = fix16Mul( vel, thrustY[rot] );
    SPR_setAnim(obj_sprites[i], i % 4);
    obj_type[i] = rockType;
}


void createRocks(u8 rockCount )
{
    for (u8 i = 0; i < MAX_ROCKS; ++i)
    {
        if( i < rockCount ) {
            fix16 x = FIX16(random() % (MAP_WIDTH - 32) - MAP_HALF_WIDTH );
            fix16 y = FIX16(random() % (MAP_HEIGHT - 32) - MAP_HALF_HEIGHT );
            createRock( i, ROCK, x, y );
            /*
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
               obj_type[i] = ROCK;
               */
        } else {
            // clear out the rest 
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


                    // deactivate the rock
                    SPR_releaseSprite( obj_sprites[i] );
                    obj_live[i] = FALSE;
                    //SPR_setVisibility(obj_sprites[i], HIDDEN);
                    // deactivate the shot
                    obj_live[j] = FALSE;
                    SPR_setVisibility(obj_sprites[j], HIDDEN);
                    // play the sound
                    XGM_startPlayPCM(SND_EXPLOSION, 10, SOUND_PCM_CH3);
                    showExplosion(obj_pos_x[i], obj_pos_y[i]);
                    // make more rocks
                    if( obj_type[i] == ROCK || obj_type[i] == MID_ROCK ) {
                        splitRock(i);
                    }
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
    fix16 xpos = FIX16(-16);
    fix16 ypos = FIX16(264);
    for (u16 i = 0; i < MAX_EXPLOSIONS; ++i)
    {
        explosion_pos_x[i] = xpos;
        explosion_pos_y[i] = ypos;
        explosion_ticks[i] = 0;
        explosion_live[i] = FALSE;

        explosion_sprites[i] = SPR_addSprite(&explosion, fix16ToInt(xpos), fix16ToInt(ypos), TILE_ATTR(PAL0, 0, FALSE, FALSE));
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
    blink_sprite = SPR_addSprite(&blink2, fix16ToInt(ship_pos_x) - camPosX, fix16ToInt(ship_pos_y) - camPosY, TILE_ATTR(PAL3, 0, FALSE, FALSE));
    SPR_setVisibility(blink_sprite, HIDDEN);
    ship_sprite = SPR_addSprite(&ship, fix16ToInt(ship_pos_x) - camPosX, fix16ToInt(ship_pos_y) - camPosY, TILE_ATTR(PAL2, 0, FALSE, FALSE));
    SPR_setAnim(ship_sprite, 0);

    //Sprite* bsprite = SPR_addSprite(&blink, fix16ToInt(ship_pos_x) - camPosX, fix16ToInt(ship_pos_y) - camPosY, TILE_ATTR(PAL3, 0, FALSE, FALSE));

    //player.hitbox_x1 = FIX16(3);
    //player.hitbox_y1 = FIX16(3);
    //player.hitbox_x2 = FIX16(13);
    //player.hitbox_y2 = FIX16(13);
    //playerRotation = MIN_ROTATION_INDEX;
    // playerRotation = FIX16(12);



    createPlayerShots();
    createExplosions();
    //createRocks(MAX_ROCKS);
    createRocks(10);
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

        // check UFO every 4th frame (b00000011)
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
