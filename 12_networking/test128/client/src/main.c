#include <genesis.h>
#include "network.h"

int cursor_x, cursor_y;
u8 buttons, buttons_prev;

int main()
{
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

	VDP_drawText("IP Address:", cursor_x, cursor_y); 
	NET_printIP(cursor_x+12, cursor_y); cursor_y++;

	VDP_drawText("MAC:", cursor_x, cursor_y); 
	NET_printMAC(cursor_x+5, cursor_y); cursor_y+=2;

	waitMs(1000);

	NET_pingIP(cursor_x, cursor_y, 4, "10.201.138.174"); cursor_y+=6;

	waitMs(2000);

	VDP_drawText("Rebooting adapter...", cursor_x ,cursor_y); cursor_y+=2;
	NET_resetAdapter();

	NET_connect(cursor_x, cursor_y, "10.201.138.174:8080"); cursor_x=0; cursor_y++;

	s16 recCount = 0;
	while(1) // Loop forever and print out any data we receive in the hardware receive fifo
	{ 
		buttons = JOY_readJoypad(JOY_1);
		u8 c = 0;
		if(buttons & BUTTON_START && buttons_prev == 0x00)
		{ 
			c = 128;
		}

		if( c > 0 ) {
			NET_sendByte(c); 
			c = 0;
			recCount = 0;
			VDP_drawText("sent", cursor_x, cursor_y); cursor_x+=4;

			startTimer(0);

			while(NET_RXReady() )
			{
				u8 byte = NET_readByte();
				if (cursor_x >= 40) { cursor_x=0; cursor_y++; }
				if (cursor_y >= 25) { cursor_x=0; cursor_y=1; }
				sprintf(str, "%d", byte); // Convert to readable number
				VDP_drawText(str, cursor_x, cursor_y); cursor_x+=4;
			}

			// get number of subticks sine startTimer(0) call
			//   where SGDK timer.h defines  SUBTICKPERSECOND    76800
			u32 elapsedTime = getTimer(0, FALSE);
			sprintf(str, "time: %ld. ", elapsedTime); // Convert to readable number
			cursor_x = 0;
			cursor_y++;
			VDP_drawText(str, cursor_x, cursor_y); cursor_y++;
			// been getting about 8960 on my Sega Genesis 1 and 9040 on MegaSG. 
			// So lets say reading 128 bytes takes about 9200 subticks 
			//      9200 subticks / ( 76800 (subticks/second) )
			//      ~ 0.12 seconds ~ 120 ms to read 128 bytes.  
			//  I shold still consider splitting thigns up into sub parts



		}		


		buttons_prev = buttons;



		SYS_doVBlankProcess(); 
	}
	//------------------------------------------------------------------
	return(0);
}




