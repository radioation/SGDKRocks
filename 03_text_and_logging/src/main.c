#include <genesis.h>

int main(bool hard) {

	// set background color to a palette color
	// SGDK automatically sets up 4 color palettes 
	// when it starts up.  You can pick a number
	// from 0 to 63 to set the background color
	VDP_setBackgroundColor(49);  

	// Draw text in the middle of the screen
	// Genesis can show 40 columns and 28 rows
	// of text
	u16 x = 14; // Text starts atcolumn 14
	u16 y = 10; // Text starts at row 10
	VDP_drawText("SGDK ROCKS!!", x, y);

	char message[40];
	// use intToStr() to print row numbers
	for( s32 i=0; i < 28; ++i ) {
		intToStr( i, message, 1 );
		VDP_drawText( message, 0, i );
	}

	// use uintToStr() to print column numbers
	for( u32 i=0; i < 40; ++i ) {
		u32 tmp = i%10;
		uintToStr( tmp, message, 1 );
		VDP_drawText( message, i, 0 );
		if( i > 0 ) {
			if( tmp == 0 ) {
				uintToStr( i/10, message, 1 );
				VDP_drawText( message, i, 1 );
			}
		}
	}

	while(TRUE) {

		// Basic text output
		fix32 fFps = SYS_getFPSAsFloat(); // there also exists a SYS_getFPSAsFloat() function to get it as a fixed point number
		char cFps[5];
		fix32ToStr( fFps, cFps, 1 );
		sprintf( message, "FPS: %s", cFps );
		VDP_drawText(message, 25, 26); // place on bottom of screen

		u16 uCpu = SYS_getCPULoad();
		strclr(message);
		sprintf( message, "CPU: %d", uCpu );
		VDP_drawText(message, 3, 26);

		//  GENS KLog() examples.  SGDK's KLog commands
		//  can send messages to Gens KMod's message window.
		//  
		//  ( use CPU->Debug->Messages to open the message window )
		//
		//  This can be helpful when you want to debug 
		//  a program and don't want to write code out ot your
		//  screen.
		KLog("KLog() test example!");

		// The KLog_U* functions write out messages and unsigned values
		u32 u1 = 100;
		KLog_U1("KLog_U1() test: ", u1);
		KLog_U1x( 6, "Klog_U1x() test:" , u1 );  // The 'x' variant appends leading zeros

		u32 u2 = 200;
		KLog_U2("KLog_U2() test: ", u1, " part 2: ", u2);

		u32 u3 = 300;
		u32 u4 = 400;
		KLog_U4("KLog_U4() test: ", u1, " part 2: ", u2, " part 3: ", u3, " part 4: ", u4);

		KLog_U1_("append a number ", u1, " and another string"); // Places unsigned number between two strings
		KLog_U1x_(6, "append a number ", u1, " and another string"); // 'x' variant appends leading zeroes


		// Similar functions exist for Signed and Float numbers too
		s32 s1 = -100;
		s32 s2 = -200;
		s32 s3 = -300;
		KLog_S3_("KLog_S3() test: ", s1, " part 2: ", s2, " part 3: ", s3, " and another string " );

		fix32 f1 =  1.3;
		KLog_f1x( 4, "Fixed point with leading zeroes : ", f1 );

		// Timing
		u32 start = getTick();// Returns elapsed time from console reset (1/256 second based).
		// if you want to hit 60fps, you need to be under 16.6666 secs
		// some time-consuming operation
		float c = 0.0;
		for( u32 i=0; i < 601; ++i ) {
			float a = 2.0;
			float b = 555.0;
			c = a * b + random()%10;
		}
		u32 end = getTick();
		u32 dummy = c;
		KLog_U1("c: ", dummy);
		// Log the timing 
		u32 elapsed = end - start;
		KLog_U1("Start tick: ", start);
		KLog_U1("End tick: ", end);
		KLog_U1("Elapsed Time: ", elapsed);


    bool didFrameMiss = VDP_waitVSync(); // wait for next VSync
		if( didFrameMiss ) {
			KLog("FRAME MISSED!");
		}

		SYS_doVBlankProcess();
	}
	return 0;
}
