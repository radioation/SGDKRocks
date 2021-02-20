# Sega Menacer Light Gun
So, I don't actually have a Sega Menacer, they're a bit more expensive 
than I'm willing to pay on ebay. OTOH, I found [this](https://www.youtube.com/watch?v=a9mCa0__SPQ)
video  on YouTube.   I was able to get a relatively cheap Radica Menacer
and modify it to work with my Sega Genesis and SGDK.


Things to keep in mind when using SGDK with the Menacer.
* Brightness Matters. I thought something was wrong with my code initially because it was always returning -1 values.   Changing the background color fixed this.
* Light guns aren't perfect.  My Modified Radica has the left-most part of the screen at 84 and the right-most at 20.  This needs to be handled in software.
* The are some comments about calibration [here](http://gendev.spritesmind.net/forum/viewtopic.php?t=14&start=660)
> So you have two things that any light gun game MUST do: first, convert the 
> counter values into a pixel; and second, have some kind of calibration screen.
>  To calibrate the gun, you put up a target on the screen with a message 
>  telling the user to fire at the center of the target. A simple bulls-eye is 
>  fine. You then look at where the gun SAYS the user is aiming, and adjust it 
>  to be the center. It's best to have the user fire several times so as to take 
>  into account the user's ability to hold the gun steady. Maybe show the user 
>  where the gun thinks it's aimed based on the current average offset. The user 
>  can just keep firing until the spot matches the center of the target.
* I'm unable to link using SGDK 1.60.  It does work with 1.51
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
