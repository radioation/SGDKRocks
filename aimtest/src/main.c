#include <genesis.h>


#define UFO_SLOT            0 
#define UFO 100
#define SMALL_UFO         200

static u16 ufoTick = 0; 
fix16 ufoShotX[64];
fix16 ufoShotY[64];

u8 obj_type[1];
fix16 obj_pos_x[1];
fix16 obj_pos_y[1];
fix16 ship_pos_x = FIX16(0.0);
fix16 ship_pos_y = FIX16(0.0);

u8 cursor_y = 0;

void fireUfoShot() {
    u8 shotDir = 0;
    if( obj_type[UFO_SLOT] == UFO ) {
        /*
           Big saucers shoot randomly
6c5c: 4a                           lsr     A                       ;If its a large saucer, prepare to shoot a random shot
6c5d: f0 06                        beq     GetScrShpDistance       ;If its a small saucer, prepare to shoot an aimed shot
6c5f: 20 b5 77                     jsr     GetRandNum              ;Get a random number
6c62: 4c c2 6c                     jmp     ScrShoot                ;Prepare to generate a saucer bullet
*/
        shotDir = random();

    } else if ( obj_type[UFO_SLOT] == SMALL_UFO ) {

        // find relative position of ship to UFO
        fix16 deltaX = obj_pos_x[UFO_SLOT] - ship_pos_x;
        fix16 deltaY = obj_pos_y[UFO_SLOT] - ship_pos_y;


        char msg[40];
        char objX[10];
        fix16ToStr( deltaX, objX, 4 );
        char objY[10];
        fix16ToStr( deltaY, objY, 4 );
        sprintf( msg, "dx: %s dy: %s ", objX, objY );
        VDP_drawText(msg, 0, cursor_y );
        cursor_y++;

        // Handle special cases for pure vertical/horizontal
        if (deltaX == FIX16(0)) {
            // wi & arae
            shotDir = (deltaY > 0) ? 0 : 32; 
        }else if (deltaY == FIX16(0)) { 
            // Left or Right
            shotDir = (deltaX > 0) ? 48 : 16;  
        } else {

            fix16 absX = abs(deltaX);
            fix16 absY = abs(deltaY);

            unsigned char index = 0;

            if( absX == absY ) {
                // 45 degrees
                shotDir = 8; // 360/64 -> 5.625  and 8 * 5.625 = 45 degrees
            } else {
                // Binary search approximation (4 bits for 16 angles per quadrant)
                index = 16;
                bool swap = FALSE;
                // swap if we must
                if( absX > absY ) {
                    swap = TRUE;
                    fix16 temp = absX;
                    absX = absY;
                    absY = temp;
                }
                // loop up to 4 times.
                for (u8 i = 0; i < 4; i++) {
                    if (absX < absY) {
                        index >>=1;
                    } else if ( absX == absY)  {
                        break;
                    } else {
                        absX = absX - absY;
                        index |=1; // add current bit
                    }
                }
                shotDir = index & 0x0F; // limit index to 16 values.
                if(swap) {
                    index = 15 - index;      
                }
            }
            // Adjust for the quadrant
            if (deltaX <= 0 && deltaY >= 0) {  // remember 0,0 is upper left, so a negative Y is aiming UP
                VDP_drawText("SHIP TO UPPER RIGHT", 0, cursor_y );
                cursor_y++;
            } else if (deltaX <= 0 && deltaY <= 0) {
                VDP_drawText("SHIP TO LOWER RIGHT", 0, cursor_y );
                cursor_y++;
                // LR Quadrant 
                if( absX == absY ) {
                    // 45 degrees
                    shotDir = 24; 
                } else {

                    shotDir = 16 + (15 - shotDir);
                }
            } else if (deltaX > 0 && deltaY < 0) {
                VDP_drawText("SHIP TO LOWER LEFT", 0, cursor_y );
                cursor_y++;
                // LL Quadrant 
                if( absX == absY ) {
                    // 45 degrees
                    shotDir = 40; 
                } else {

                    shotDir = 32 + shotDir;
                }
            } else {
                VDP_drawText("SHIP TO UPPER LEFT", 0, cursor_y );
                cursor_y++;
                // UL Quadrant 
                if( absX == absY ) {
                    // 45 degrees
                    shotDir = 56; 
                } else {

                    shotDir = 48 + (15 - shotDir);
                }
            }
        }



    }
    char msg[40];
    char objX[10];
    fix16ToStr( obj_pos_x[UFO_SLOT], objX, 4 );
    char objY[10];
    fix16ToStr( obj_pos_y[UFO_SLOT], objY, 4 );
    sprintf( msg, "x: %s y: %s i: %d", objX, objY, shotDir);
    VDP_drawText(msg, 0, cursor_y );
    cursor_y++;

    //int vx = shotDirections[shotDir].vx;
    //int vy = shotDirections[shotDir].vy;



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



int main(bool hard)
{
    /////////////////////////////////////////////////////////////////////////////////
    // Setup UFO Shot Table (64 slots)
    // starting with UP to match ship directions
    u16 pos = 0;
    for( s16 i = 16; i >= 0; i-- ) {
        ufoShotX[pos] =  fix16Div( cosFix16(i * 16), FIX16(5));
        ufoShotY[pos] = -fix16Div( sinFix16(i * 16), FIX16(5)); 
        pos++;
    }
    for( s16 i = 63; i > 16; i-- ) {
        ufoShotX[pos] =  fix16Div( cosFix16(i * 16), FIX16(5));
        ufoShotY[pos] = -fix16Div( sinFix16(i * 16), FIX16(5));  
        pos++;
    }

    obj_type[UFO_SLOT] = SMALL_UFO;
    ship_pos_x = FIX16(100.0);
    ship_pos_y = FIX16(100.0);

    // try different x/y's and print out the index
    /*
    obj_pos_x[UFO_SLOT] = FIX16(100);
    obj_pos_y[UFO_SLOT] = FIX16(110);
    fireUfoShot(); // expect 0

    obj_pos_x[UFO_SLOT] = FIX16(90);
    obj_pos_y[UFO_SLOT] = FIX16(100);
    fireUfoShot(); // expect 16

    obj_pos_x[UFO_SLOT] = FIX16(100);
    obj_pos_y[UFO_SLOT] = FIX16(90);
    fireUfoShot(); // exppect 32

    obj_pos_x[UFO_SLOT] = FIX16(110);
    obj_pos_y[UFO_SLOT] = FIX16(100);
    fireUfoShot(); // expect 48
    */

    // 45 degrres
   
    obj_pos_x[UFO_SLOT] = FIX16(90);
    obj_pos_y[UFO_SLOT] = FIX16(110);
    fireUfoShot(); // expet 8

    obj_pos_x[UFO_SLOT] = FIX16(90);
    obj_pos_y[UFO_SLOT] = FIX16(90);
    fireUfoShot(); // expect 24

    obj_pos_x[UFO_SLOT] = FIX16(110);
    obj_pos_y[UFO_SLOT] = FIX16(90);
    fireUfoShot(); // expect 40

    obj_pos_x[UFO_SLOT] = FIX16(110);
    obj_pos_y[UFO_SLOT] = FIX16(110);
    fireUfoShot(); // expect 56
  


    while(TRUE)
    {
        SYS_doVBlankProcess();
    }
    return 0;
}
