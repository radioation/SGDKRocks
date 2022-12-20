# Mouse Example
I recently found out about the Sega Mega Mouse while reading about SGDK. 
So I bought one off ebay to try out with SGDK.  Luckily it works and
I was able to read it easily with SGDK's JOY functions. Unfortunately
the wiki is out of date (at the time I wrote this) and `JOY_setMouseEventHandler`
doesn't exist in SGDK 1.60

~~~bash
$ find . -type f -exec grep  JOY_setMouseEventHandler {} + | wc
     0       0       0
~~~

## Basic Usage
Sega Genesis and Master System controllers are handled by the `JOY_*`
functions in `joy.h.

1. Determine if a mouse is attached to the controller port
~~~c
u8 portType = JOY_getPortType(PORT_2);
if( portType == PORT_TYPE_MOUSE ) {
	// ... Do stuff here
}
~~~

2. Set mouse support 
if( portType == PORT_TYPE_MOUSE ) {
	JOY_setSupport(PORT_2, JOY_SUPPORT_MOUSE);
}
~~~

3. Read X and Y values for the mouse with `JOY_readJoypadX()` and `JOY_readJoypadY()`
These functions return signed X|Y values from -32767 to 32767.  The change in value
is proportional to the speed of mouse movement.    
* Positive values from `JOY_readJoypadX()` indicate movement to the right of the screen. Negative to the left.
* Positive values from `JOY_readJoypadY()` indicate movement to the top of the screen. Negative to the bottom.
~~~c
			x_mouse = JOY_readJoypadX(JOY_2);
			y_mouse = JOY_readJoypadY(JOY_2);
~~~
The values returned don't map to screen coordinates.  You can use the changes in values
over time to determine how to move a mouse pointer.

### Full Code
~~~c
int main(u32 hard)
{
	// check port 2 for a mouse
	bool mouseFound = FALSE;
	u8 portType = JOY_getPortType(PORT_2);
	if( portType == PORT_TYPE_MOUSE ) {
		JOY_setSupport(PORT_2, JOY_SUPPORT_MOUSE);
		mouseFound = TRUE;
		VDP_drawText("Mouse Found!", 14, 1 );
	} else {
		VDP_drawText("No Mouse", 32, 1 );
	}

	while(TRUE)
	{
		if( mouseFound ) {
			// mice return signed axis data. Change in value indicates movment.
			x_mouse = JOY_readJoypadX(JOY_2); // positive change indicates movement to the right
			y_mouse = JOY_readJoypadY(JOY_2); // positive change indicates movement to the top

			// Write mouse values to the screen
			VDP_setTextPalette(3);
			char mouseMessage[40];
			sprintf( mouseMessage, "Mouse Values x:%d, y:%d      ", x_mouse, y_mouse );
			VDP_drawText(mouseMessage, 8, 10 );
			VDP_setTextPalette(0);

		}

		SYS_doVBlankProcess();
	}

	return 0;
}
~~~

## Practical Use
`JOY_readJoypadX()` and `JOY_readJoypadY()` return signed X|Y values from -32767 to 32767.  
These values obviously don't map to the Genesis screen coordinates.  You should create 
X and Y values to store the screen coordinates for a sprite controlled by the mouse.  
Update the screen coordniates based on the changes from `JOY_readJoypadX()` and `JOY_readJoypadY()`

1. Setup values to store screen position
~~~c
	// Sprite position 
	fix32 posX = FIX32(154.0);
	fix32 posY = FIX32(108.0);
~~~

2. Create values to store old mouse X and Y values
~~~c
	s32 prev_x_mouse = JOY_readJoypadX(JOY_2);
	s32 prev_y_mouse = JOY_readJoypadY(JOY_2);
~~~

3. In the main loop read in new mouse values
~~~c
	s32 x_mouse = JOY_readJoypadX(JOY_2);
	s32 y_mouse = JOY_readJoypadY(JOY_2); 
~~~

4. Figure out how much the mouse values have changed

~~~c
	fix32 x_mouse_delta = fix32Sub(FIX32(x_mouse), FIX32(prev_x_mouse));
	fix32 y_mouse_delta = fix32Sub(FIX32(y_mouse), FIX32(prev_y_mouse));
~~~

5. Scale the change in mouse X|Y values to control how much the mouse 
affects the screen coordinates.   
~~~c
	fix32 x_delta = fix32Div( x_mouse_delta, sensitivity);
	fix32 y_delta = fix32Div( y_mouse_delta, sensitivity);
~~~

5. Update the screen position.
~~~c
	posX = fix32Add( posX, x_delta);
	posY = fix32Sub( posY, y_delta);
~~~
6. Save the current mouse values for the next iteration of your game loop
~~~c
	prev_x_mouse = x_mouse;
	prev_y_mouse = y_mouse;
~~~
     


