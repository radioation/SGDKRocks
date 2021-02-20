#include <genesis.h>

int main()
{

	// Set background brighter than 0.  Black background
	// prevents menacer from returning X, Y values.
	VDP_setBackgroundColor( 4 );


	// check Port 2 for the Sega Menacer
	bool menacerFound = FALSE;
	u8 portType = JOY_getPortType(PORT_2);
	if(portType == PORT_TYPE_MENACER )
	{
		JOY_setSupport(PORT_2, JOY_SUPPORT_MENACER);
		menacerFound = TRUE;
		VDP_drawText("Menacer FOUND!", 14, 1);
	} else  {
		VDP_drawText("Menacer NOT found.", 11, 1);
	}

	while(TRUE)
	{
		if( menacerFound ) {	
			// get the button states		
			u16 value = JOY_readJoypad(JOY_2);
			if( value & BUTTON_A ) {
				VDP_drawText("A", 8, 8 );
			} else {
				VDP_drawText(" ", 8, 8 );
			}

			if( value & BUTTON_B ) {
				VDP_drawText("B", 10, 8 );
			} else {
				VDP_drawText(" ", 10, 8 );
			}

			if( value & BUTTON_C ) {
				VDP_drawText("C", 12, 8 );
			} else {
				VDP_drawText(" ", 12, 8 );
			}

			// THe menacer appears to return 8-bit values (0 to 255)
			//  255 is off screen.  
			u16 xVal = JOY_readJoypadX(JOY_2);
			u16 yVal = JOY_readJoypadY(JOY_2);
			char message[40];
			sprintf( message, "Menacer Values x:%d, y:%d      ", xVal, yVal );
			VDP_drawText(message, 8, 13 );
		}

		VDP_waitVSync();
		// SYS_doVBlankProcess(); 1.60 not linking

	}
}

