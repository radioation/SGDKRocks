# Sega Menacer Light Gun
This code has been updated to work with SGDK 1.80.  It may not work with older versions of SGDK. 
I have had linking problems with 1.60 in the past.  You should use the most recent version 
of SGDK.
  ~~~cmd
  out/sega.o: In function `_EXTINT':
  (.text.keepboot+0x44e): undefined reference to `internalExtIntCB'
  out/sega.o: In function `_HINT':
  (.text.keepboot+0x460): undefined reference to `internalHIntCB'
  out/sega.o: In function `_VINT':
  (.text.keepboot+0x472): undefined reference to `internalVIntCB'
  ~~~
* Color and brightness matters. In my first version of this code, calls to `JOY_readJoypadX()` and `JOY_readJoypadY()` always returned -1 values.   Changing the background color fixed this.
* The X and Y values returned by `JOY_readJoypadX()` and `JOY_readJoypadY()` range from -1 to 255.  This range is smaller than the 320 pixels the Sega Genesis can display.
* The X values from `JOY_readJoypadX()` are discontinuous.  From left-to-right I'm seeing approximately values of 84-182:229-255:0-13 with my modified Radica Menacer.
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

## Basic Code 
1. Determine if a Menacer is attached to the controller port
   ~~~c
   if(portType == PORT_TYPE_MENACER )
   {
   	// ... do stuff here
   }		
   ~~~
2. Setup Menacer support
   ~~~c
   if(portType == PORT_TYPE_MENACER )
   {
   	JOY_setSupport(PORT_2, JOY_SUPPORT_MENACER);
   }
   ~~~
3. Read X and Y values for the menacer with `JOY_readJoypadX()` and `JOY_readJoypadY()`
These functions are 8-bit and return values ranging from 0 to 255.  The Y value is 
continuous.  The X value is discontinuous.   Neither X nor Y map exactly to the 
screen coordinates so you'll have to adjust them at runtime

### Basic Code
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
~~~

## Mapping JOY_readJoypadX Values to Pixel Coordinates
As mentioned above, the X values from `JOY_readJoypadX()` are disjoint.  As I move 
the light gun from the left side of my CRT to the right I see the following values (approximately):
* 84 through 182 
* 229 thorugh 255
* 0 to 13 
The values don't cover the entire screen.  84 is far from the left side of the screen.
Further, there's a discontinity from 182 to 229. Causing a jump on the right side of the 
screen. Even worse, the values reported by the gun on the far right side of the screen
correspond to pixels on the left.  This clearly isn't what we want in a game.

### Lookup Tables
Some searching brought me to [this](https://gendev.spritesmind.net/mirrors/eke/gen_lightgun.pdf) pdf.
It points out that :
* The Vertical values can be directly converted to a Y pixels.
* The Horizontal values reported are equivalent to two screen pixels
* Commercial games use a lookup table to map Horizontal values to screen pixels

So I made lookup table that can hold the full range of possible 
values reported by `JOY_readJoypadX()` including the gap.
I populate the table with screen coordinates starting at 0 (just a guess)
and incrementing by 2.
~~~c
static s16 xLookup[ 256 ];  // full rangeA for JOY_readJoypadX()

static void calculateXLookup() {
  // My own experience has 84 at left edge of the monitor, and 13 on the right.
  // I'll start populating the table left to right with a screen value of -40
  s16 pos = 0;
  for( int i=60; i < 183; ++i ) {
    xLookup[i] =  pos;
    pos += 2;
  }
	// handle the gap in X values 
  for( int i=229; i < 256; ++i ) {
    xLookup[i] =  pos;
    pos += 2;
  }
	// handle the wrap around to 0  
  for( int i=0; i < 60; ++i ) {
    xLookup[i] =  pos;
    pos += 2;
  }

}
~~~
Once the lookup table is ready, decent X and Y pixel values  can be found 
pretty easily
~~~c
  crosshairsPosX = xLookup[ xVal ];  // lookup the screen coordinate based on Horizontal Value
  crosshairsPosY = yVal;   // direct conversion of Vertical value
~~~

Depending on what you're displaying, you may want to factor in an offset.
In my case I want to center a 16 x 16 sprite.  So I want to shift it to the 
left and up by 8 pixels. I do this by subtracting 8 from the X and Y values.
~~~c
  crosshairsPosX = xLookup[ xVal ] - 8;
  crosshairsPosY = yVal - 8;
~~~

### Calibration
The lookup table gives pretty good results, but don't line up with the
sights of my Menacer.  Different Sega and Radica Menacers are unlikely to
have the same offsets as mine, so hardcoding an offset isn't a great idea. 
To compensate for this I added code that:
1. Has the user aim at a target at the center of the screen
2. Has the user fire 10 shots with the trigger.  The code saves the lookup
values for each shot
3. Calculates an average X and Y position from the shots fired
4. Calculate an offset from the screen center and the average X/Y values


To get the average I defined an array to store the X and Y positiions
~~~c
#define MAX_VALS 10
static s16 xValues[MAX_VALS];
static s16 yValues[MAX_VALS];
static u16 currentValue = 0;
~~~
The Joypad handler stores values when the trigger (BUTTON_A) is pulled
~~~c
    if( changed == BUTTON_A && joypadState == BUTTON_A) {
      if( calibrateMode ) {
        // get reading
        s16 xVal = JOY_readJoypadX(JOY_2);
        s16 yVal = JOY_readJoypadY(JOY_2);
        // store values for calculation
        if( currentValue < MAX_VALS ) {
          xValues[currentValue] = xLookup[xVal];
          yValues[currentValue] = s16(yVal);
          ++currentValue;
        }
        if( currentValue == MAX_VALS ){
          calculateOffset();
          currentValue = 0;
          calibrateMode = FALSE;
        }
      }
    }
~~~
And calculates the offsets using the average
~~~c
static void calculateOffset() {
  s16 xTemp = 0;
  s16 yTemp = 0;
  // get average X and Y
  for( int i=0; i < currentValue; ++i ) {
    xTemp = xTemp + xValues[i];
    yTemp = yTemp + yValues[i];
  }
  xTemp = xTemp / currentValue;
  yTemp = yTemp / currentValue;

  // center should be 160, 112
  xOffset = 160 - xTemp;
  yOffset = 112 - yTemp;

}
~~~

The crosshair positions are now calculated with the lookup table *and* offset values
~~~c
crosshairsPosX = xLookup[xVal] + xOffset - 8;
crosshairsPosY = s16( yVal ) + yOffset - 8;
~~~









