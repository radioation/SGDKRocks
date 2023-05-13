#include <genesis.h>
#include "network.h"
#include "resources.h"

int cursor_x, cursor_y;
u8 buttons, buttons_prev;

#define NUM_ROCKS 10


int main()
{

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

	NET_connect(cursor_x, cursor_y, "10.201.138.174:8080"); cursor_x=0; cursor_y++;


	waitMs(2000);


	VDP_clearTextArea(0,0,40,24);
	// setup GAME  graphics
	VDP_setScreenWidth256();
	//PAL_setPalette( PAL0, starfield_pal.data, CPU);
	PAL_setPalette( PAL1, ships_pal.data, CPU);
	PAL_setPalette( PAL2, rocks_pal.data, CPU);
	PAL_setPalette( PAL3, ufos_pal.data, CPU);

	//VDP_setBackgroundColor(51); // Blue background

	int starfieldInd = TILE_USER_INDEX;
	//VDP_drawImageEx(BG_A, &starfield, TILE_ATTR_FULL(PAL0, FALSE, FALSE, FALSE, starfieldInd), 0, 0, FALSE, TRUE);


	// setup sprites
	SPR_init();
	s16 playerX1 = 152;
	s16 playerY1 = 102;

	s16 playerX2= 152;
	s16 playerY2= 102;

	s16 playerX3= 152;
	s16 playerY3= 102;

	s16 playerX4= 152;
	s16 playerY4= 102;
	Sprite* playerSprite1 = SPR_addSprite( &ships, playerX1, playerY1, TILE_ATTR( PAL1, 0, FALSE, FALSE ));
	SPR_setAnim(playerSprite1, 0 );
	Sprite* playerSprite2 = SPR_addSprite( &ships, playerX2, playerY2, TILE_ATTR( PAL1, 0, FALSE, FALSE ));
	SPR_setAnim(playerSprite2, 1 );
	Sprite* playerSprite3 = SPR_addSprite( &ships, playerX3, playerY3, TILE_ATTR( PAL1, 0, FALSE, FALSE ));
	SPR_setAnim(playerSprite3, 2 );
	Sprite* playerSprite4 = SPR_addSprite( &ships, playerX4, playerY4, TILE_ATTR( PAL1, 0, FALSE, FALSE ));
	SPR_setAnim(playerSprite4, 3 );

	Sprite *rockSprites[NUM_ROCKS];
	for( s16 i=0; i < NUM_ROCKS; ++i ) {
		rockSprites[i] = SPR_addSprite( &rocks, 10, 10, TILE_ATTR( PAL2, 0, FALSE, FALSE ));
	}

	// attach and get ID
	// from now on send ID

	// expecting 4 player chunks of 3 * 2 =6 bytes each ( 24 bytes )
	// expecting 20 rock chunks of 2 * 2 = 4 bytes each ( 60 bytes )
	// 84 bytes total?
	u16 tick = 0;
	u8 networkData[36]; // 4 players have 3 x 16 bit (2 byte) vas)
	while(1) // Loop forever and print out any data we receive in the hardware receive fifo
	{ 
		startTimer(0);
		// always send player control
		// '0x0100_XXXX' sends player control
		u16 joypadState = JOY_readJoypad( JOY_1 );
		u8 command = 64;
		if( joypadState & BUTTON_LEFT ) {
			command += 0b00000001 ;
		}
		if( joypadState & BUTTON_RIGHT ) {
			command += 0b00000010 ;
		}
		if( joypadState & BUTTON_UP ) {
			command += 0b00000100 ;
		}
		if( joypadState & BUTTON_DOWN ) {
			command += 0b00001000 ;
		}
		NET_sendByte(command);
		++tick;


		/*
# MAKE FIRST BYTE A TAG FOR THE TYPE OF DATA SENT 
# 0 player pos
# 1 player shots
# 2 ROCKS
*/
		// sometimes
		/*
		// '0' request the player positions
		NET_sendByte(0);
		u16 bytesPos = 0;
		while(NET_RXReady()) // while data in hardware receive FIFO
		{   
		networkData[bytesPos] = NET_readByte(); // Retrieve byte from RX hardware Fifo directly
		bytesPos++;
		//sprintf(str, "%d", byte); // Convert
		//u16 bytesAvailable = NET_bytesAvailable(); // Retrieve byte from RX hardware Fifo directly
		//sprintf(str, "%d", bytesAvailable); // Convert
		//VDP_drawText(str, cursor_x, cursor_y);
		}

		//sprintf(str, "bp: %d", bytesPos); // Convert
		//VDP_drawText(str, cursor_x+5, cursor_y);
		if( bytesPos == 12 ) {

		// Convert the first 2 bytes to a short int
		//s16 a = (networkData[0] << 8) | networkData[1];
		s16 dir = networkData[0] & 0x0f;
		playerX1 = networkData[1];
		playerY1 = networkData[2];
		SPR_setPosition( playerSprite1, playerX1, playerY1 );
		SPR_setFrame(playerSprite1, dir);

		dir = networkData[3] & 0x0f;
		playerX2 = networkData[4];
		playerY2 = networkData[5];
		SPR_setPosition( playerSprite2, playerX2, playerY2 );
		SPR_setFrame(playerSprite2, dir);

		dir = networkData[6] & 0x0f;
		playerX3 = networkData[7];
		playerY3 = networkData[8];
		SPR_setPosition( playerSprite3, playerX3, playerY3 );
		SPR_setFrame(playerSprite3, dir);

		dir = networkData[9] & 0x0f;
		playerX4 = networkData[10];
		playerY4 = networkData[11];
		SPR_setPosition( playerSprite4, playerX4, playerY4 );
		SPR_setFrame(playerSprite4, dir);


		}



		NET_sendByte(128);


		bytesPos = 0;
		while( bytesPos < 2 * NUM_ROCKS ) {
		while(NET_RXReady()) // while data in hardware receive FIFO
		{   
		networkData[bytesPos] = NET_readByte(); // Retrieve byte from RX hardware Fifo directly
		bytesPos++;
		//sprintf(str, "%d", byte); // Convert
		//u16 bytesAvailable = NET_bytesAvailable(); // Retrieve byte from RX hardware Fifo directly
		//sprintf(str, "%d", bytesAvailable); // Convert
		//VDP_drawText(str, cursor_x, cursor_y);
		}
		}
		sprintf(str, "bp2: %d", bytesPos); // Convert
		VDP_drawText(str, cursor_x+5, cursor_y+1);

		if( bytesPos >= 6 ) {
		for( u16 r = 0; r < NUM_ROCKS; ++r ) {
		SPR_setPosition( rockSprites[r], networkData[r*2], networkData[r*2+1]);
		}
	}
	*/


		//   where SGDK timer.h defines  SUBTICKPERSECOND    76800
		u32 elapsedTime = getTimer(0, FALSE);
	sprintf(str, "time: %ld. ", elapsedTime); // Convert to readable number
	cursor_x = 0;
	cursor_y++;
	VDP_drawText(str, 10, 3); cursor_y++;


	SPR_update();
	SYS_doVBlankProcess(); 
	}

	//------------------------------------------------------------------
	return(0);
}




