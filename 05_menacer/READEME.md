# Sega Menacer Light Gun
So, I don't actually yave a Sega Menacer, they're a bit more expensive 
than I'm willing to pay on ebay. OTOH, I found [this](https://www.youtube.com/watch?v=a9mCa0__SPQ)
video  on youtube

Things to keep in mind
* Brightness Matters
* Guns aren't perfect.  My Modified Radica has the left-most part of the
  screen at 84 and the right-most at 20.  
* I'm unable to link using SGDK 1.60
~~~cmd
out/sega.o: In function `_EXTINT':
(.text.keepboot+0x44e): undefined reference to `internalExtIntCB'
out/sega.o: In function `_HINT':
(.text.keepboot+0x460): undefined reference to `internalHIntCB'
out/sega.o: In function `_VINT':
(.text.keepboot+0x472): undefined reference to `internalVIntCB'
~~~

## Basic Code 
~~~c
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

			// The menacer returns 8-bit values (0 to 255)
			//  255 is off screen.  
			s16 xVal = JOY_readJoypadX(JOY_2);
			s16 yVal = JOY_readJoypadY(JOY_2);
			char message[40];
			sprintf( message, "Menacer Values x:%d, y:%d      ", xVal, yVal );
			VDP_drawText(message, 8, 13 );
		}

		VDP_waitVSync();
		// SYS_doVBlankProcess(); 1.60 not linking

	}
}
~~~
