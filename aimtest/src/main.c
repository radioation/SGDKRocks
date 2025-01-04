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
        shotDir = random();
    } else if ( obj_type[UFO_SLOT] == SMALL_UFO ) {

        // find relative position of ship to UFO
        fix16 deltaX = obj_pos_x[UFO_SLOT] - ship_pos_x;
        fix16 deltaY = obj_pos_y[UFO_SLOT] - ship_pos_y;

        char msg[40];

        // Handle special cases for pure vertical/horizontal
        if (deltaX == FIX16(0)) {
            // wi & arae
            shotDir = (deltaY > 0) ? 0 : 32; 
        }else if (deltaY == FIX16(0)) { 
            // Left or Right
            shotDir = (deltaX > 0) ? 48 : 16;  
        } else {

            // get absolute values
            s16 absX = fix16ToInt(abs(deltaX));
            s16 absY = fix16ToInt(abs(deltaY));

            sprintf( msg, "ax: %d ay: %d ", absX, absY );
            VDP_drawText(msg, 0, cursor_y );
            cursor_y++;

            unsigned char index = 0;

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
                index = 16;
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
                if (deltaX <= 0 && deltaY >= 0) {  // remember 0,0 is upper left, so a negative Y is aiming UP
                    VDP_drawText("SHIP TO UPPER RIGHT", 0, cursor_y );
                    cursor_y++;
                } else if (deltaX <= 0 && deltaY <= 0) {
                    VDP_drawText("SHIP TO LOWER RIGHT", 0, cursor_y );
                    cursor_y++;
                    // LR Quadrant 
                    shotDir = 32 - shotDir;
                } else if (deltaX > 0 && deltaY < 0) {
                    VDP_drawText("SHIP TO LOWER LEFT", 0, cursor_y );
                    cursor_y++;
                    // LL Quadrant 
                    shotDir = 32 + shotDir;
                } else {
                    VDP_drawText("SHIP TO UPPER LEFT", 0, cursor_y );
                    cursor_y++;
                    // UL Quadrant 
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

/*
    // 45 degrees
    obj_pos_x[UFO_SLOT] = FIX16(90);
    obj_pos_y[UFO_SLOT] = FIX16(110);
    fireUfoShot(); // expect 8

    obj_pos_x[UFO_SLOT] = FIX16(90);
    obj_pos_y[UFO_SLOT] = FIX16(90);
    fireUfoShot(); // expect 24

    obj_pos_x[UFO_SLOT] = FIX16(110);
    obj_pos_y[UFO_SLOT] = FIX16(90);
    fireUfoShot(); // expect 40

    obj_pos_x[UFO_SLOT] = FIX16(110);
    obj_pos_y[UFO_SLOT] = FIX16(110);
    fireUfoShot(); // expect 56
*/

/*
    // upper right
    obj_pos_x[UFO_SLOT] = FIX16(99);
    obj_pos_y[UFO_SLOT] = FIX16(110);
    fireUfoShot(); 

    obj_pos_x[UFO_SLOT] = FIX16(98);
    obj_pos_y[UFO_SLOT] = FIX16(110);
    fireUfoShot(); 

    obj_pos_x[UFO_SLOT] = FIX16(98);
    obj_pos_y[UFO_SLOT] = FIX16(108);
    fireUfoShot();

    obj_pos_x[UFO_SLOT] = FIX16(98);
    obj_pos_y[UFO_SLOT] = FIX16(106);
    fireUfoShot();

    obj_pos_x[UFO_SLOT] = FIX16(97);
    obj_pos_y[UFO_SLOT] = FIX16(106);
    fireUfoShot();

    obj_pos_x[UFO_SLOT] = FIX16(96);
    obj_pos_y[UFO_SLOT] = FIX16(107);
    fireUfoShot(); 

    obj_pos_x[UFO_SLOT] = FIX16(90);
    obj_pos_y[UFO_SLOT] = FIX16(101);
    fireUfoShot(); 

    obj_pos_x[UFO_SLOT] = FIX16(92);
    obj_pos_y[UFO_SLOT] = FIX16(102);
    fireUfoShot(); 

    obj_pos_x[UFO_SLOT] = FIX16(94);
    obj_pos_y[UFO_SLOT] = FIX16(102);
    fireUfoShot(); 
*/

/* 
    // lower right
    obj_pos_x[UFO_SLOT] = FIX16(99);
    obj_pos_y[UFO_SLOT] = FIX16(90);
    fireUfoShot(); 

    obj_pos_x[UFO_SLOT] = FIX16(98);
    obj_pos_y[UFO_SLOT] = FIX16(90);
    fireUfoShot(); 

    obj_pos_x[UFO_SLOT] = FIX16(98);
    obj_pos_y[UFO_SLOT] = FIX16(92);
    fireUfoShot();

    obj_pos_x[UFO_SLOT] = FIX16(98);
    obj_pos_y[UFO_SLOT] = FIX16(94);
    fireUfoShot();

    obj_pos_x[UFO_SLOT] = FIX16(90);
    obj_pos_y[UFO_SLOT] = FIX16(99);
    fireUfoShot(); 

    obj_pos_x[UFO_SLOT] = FIX16(92);
    obj_pos_y[UFO_SLOT] = FIX16(98);
    fireUfoShot(); 

    obj_pos_x[UFO_SLOT] = FIX16(94);
    obj_pos_y[UFO_SLOT] = FIX16(98);
    fireUfoShot(); 
*/


/*
    // to lower left 
    obj_pos_x[UFO_SLOT] = FIX16(101);
    obj_pos_y[UFO_SLOT] = FIX16(90);
    fireUfoShot(); 

    obj_pos_x[UFO_SLOT] = FIX16(102);
    obj_pos_y[UFO_SLOT] = FIX16(90);
    fireUfoShot(); 

    obj_pos_x[UFO_SLOT] = FIX16(102);
    obj_pos_y[UFO_SLOT] = FIX16(92);
    fireUfoShot();

    obj_pos_x[UFO_SLOT] = FIX16(102);
    obj_pos_y[UFO_SLOT] = FIX16(94);
    fireUfoShot();

    obj_pos_x[UFO_SLOT] = FIX16(110);
    obj_pos_y[UFO_SLOT] = FIX16(99);
    fireUfoShot(); 

    obj_pos_x[UFO_SLOT] = FIX16(108);
    obj_pos_y[UFO_SLOT] = FIX16(98);
    fireUfoShot(); 

    obj_pos_x[UFO_SLOT] = FIX16(106);
    obj_pos_y[UFO_SLOT] = FIX16(98);
    fireUfoShot(); 
*/
    // to upper left
    obj_pos_x[UFO_SLOT] = FIX16(101);
    obj_pos_y[UFO_SLOT] = FIX16(110);
    fireUfoShot(); 

    obj_pos_x[UFO_SLOT] = FIX16(102);
    obj_pos_y[UFO_SLOT] = FIX16(110);
    fireUfoShot(); 

    obj_pos_x[UFO_SLOT] = FIX16(102);
    obj_pos_y[UFO_SLOT] = FIX16(108);
    fireUfoShot();

    obj_pos_x[UFO_SLOT] = FIX16(102);
    obj_pos_y[UFO_SLOT] = FIX16(106);
    fireUfoShot();

    obj_pos_x[UFO_SLOT] = FIX16(110);
    obj_pos_y[UFO_SLOT] = FIX16(101);
    fireUfoShot(); 

    obj_pos_x[UFO_SLOT] = FIX16(108);
    obj_pos_y[UFO_SLOT] = FIX16(102);
    fireUfoShot(); 

    obj_pos_x[UFO_SLOT] = FIX16(106);
    obj_pos_y[UFO_SLOT] = FIX16(102);


    while(TRUE)
    {
        SYS_doVBlankProcess();
    }
    return 0;
}
