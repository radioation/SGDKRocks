#include <genesis.h>
#include "network.h"
#include "resources.h"

int cursor_x, cursor_y;
u8 buttons, buttons_prev;

int main()
{
/*
    // setup networking
    SYS_disableInts();                      // Disable interrupts
    VDP_setScreenWidth320();                // Set screen Width
    VDP_setScreenHeight224();               // Set screen Height
    VDP_setBackgroundColor(0);              // Set background black
    VDP_setTextPlane(BG_B);                 // Use PLANE B for text rendering
    VDP_setTextPalette(0);                  // Use palette 0 for text color
    SYS_enableInts();                       // Enable interrupts (allows our callback routine to print data)

    PAL_fadeOutPalette(PAL0,1,FALSE);
    VDP_setBackgroundColor(51); // Blue background
    PAL_fadeInPalette(PAL0, palette_grey, 16, FALSE);

    cursor_x = 0;
    cursor_y = 1;

    VDP_drawText("Detecting adapter...[  ]", cursor_x, cursor_y); cursor_x+=21; 
    NET_initialize(); // Detect cartridge and set boolean variable

    if(cart_present)
    {
        VDP_setTextPalette(2); // Green text
        VDP_drawText("Ok", cursor_x, cursor_y); cursor_x=0; cursor_y+=2;
        VDP_setTextPalette(0); // White text
    }
    else
    {
        VDP_setTextPalette(1); // Red text
        VDP_drawText("XX", cursor_x, cursor_y); cursor_x=0; cursor_y+=2;
        VDP_setTextPalette(0); // White text
        VDP_drawText("Adapter not present", cursor_x, cursor_y);
        while(1) { SYS_doVBlankProcess(); }
    }

//------------------------------------------------------------------
// MAIN LOOP
//------------------------------------------------------------------

    waitMs(2000);

    VDP_drawText("Rebooting adapter...", cursor_x ,cursor_y); cursor_y+=2;
    NET_resetAdapter();

    NET_connect(cursor_x, cursor_y, "10.201.138.151:8080"); cursor_x=0; cursor_y++;


    waitMs(2000);
*/

    // setup graphics
    PAL_setPalette( PAL0, starfield_pal.data, CPU);
    PAL_setPalette( PAL1, ships_pal.data, CPU);
    PAL_setPalette( PAL2, rocks_pal.data, CPU);
    PAL_setPalette( PAL3, ufos_pal.data, CPU);

    int starfieldInd = TILE_USER_INDEX;
    VDP_drawImageEx(BG_A, &starfield, TILE_ATTR_FULL(PAL0, FALSE, FALSE, FALSE, starfieldInd), 0, 0, FALSE, TRUE);

    
    // setup sprites
    SPR_init();
    s16 playerX = 152;
    s16 playerY = 102;
    Sprite* playerSprite = SPR_addSprite( &ships, playerX, playerY, TILE_ATTR( PAL1, 0, FALSE, FALSE ));
    

    // attach and get ID
    // from now on send ID

    // expecting 4 player chunks of 3 * 2 =6 bytes each ( 24 bytes )
    // expecting 20 rock chunks of 2 * 2 = 4 bytes each ( 60 bytes )
    // 84 bytes total?
    while(1) // Loop forever and print out any data we receive in the hardware receive fifo
    { 
        //NET_sendMessage("r");
/*
        NET_sendByte('r');
        while(NET_RXReady()) // while data in hardware receive FIFO
        {   
            u8 byte = NET_readByte(); // Retrieve byte from RX hardware Fifo directly
            switch(byte)
            {
                case 0x0A: // a line feed?
                    cursor_y++;
                    cursor_x=1;
                    break;              
                case 0x0D: // a carridge Return?
                    cursor_x=1;
                    break; 
                default:   // print
                    if (cursor_x >= 40) { cursor_x=0; cursor_y++; }
                    if (cursor_y >= 28) { cursor_x=0; cursor_y=0; }
                    sprintf(str, "%c", byte); // Convert
                    VDP_drawText(str, cursor_x, cursor_y); cursor_x++;
                    break;
            }
        }
*/

        buttons_prev = buttons;
        SPR_update();
        SYS_doVBlankProcess(); 
    }

//------------------------------------------------------------------
    return(0);
}




