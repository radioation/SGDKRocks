#include <genesis.h>
#include "resources.h"


u16 crosshairsMode = 0;  // 0 raw values, 1 with lookup,  2 with lookup and calibration offset
static bool calibrateMode = FALSE;
#define MAX_VALS 10
static fix32 xValues[MAX_VALS];
static fix32 yValues[MAX_VALS];
static u16 currentValue = 0;


static fix32 xOffset = FIX32(0);
static fix32 yOffset = FIX32(0);

//static fix32 scaleFactorX = FIX32(1.0);  // according to gen_lightgun.pdf each H counter unit is equivalent to two pixels.
//static fix32 scaleFactorY = FIX32(1.0);  // according to gen_lightgun.pdf each V counter is directly convertable to a Y coordinate
static fix32 xLookup[ 256 ];  // full range for JOY_readJoypadX()



static void calculateOffset() {
	fix32 xTemp = FIX32(0);
	fix32 yTemp = FIX32(0);
	// get average X and Y 
	for( int i=0; i < currentValue; ++i ) {
		xTemp = fix32Add( xTemp, xValues[i] );
		yTemp = fix32Add( yTemp, yValues[i] );
	}
	xTemp = fix32Div( xTemp, FIX32( currentValue ) );
	yTemp = fix32Div( yTemp, FIX32( currentValue ) );

	// center should be 160, 112 
	xOffset = FIX32(160) - xTemp;		
	yOffset = FIX32(112) - yTemp;		

}


static void joypadHandler( u16 joypadId, u16 changed, u16 joypadState ) {
  if( joypadId == JOY_2  ) {

		// A
		if( changed == BUTTON_A && joypadState == BUTTON_A) {
			if( calibrateMode ) {
				// get reading
				s16 xVal = JOY_readJoypadX(JOY_2);
				s16 yVal = JOY_readJoypadY(JOY_2);
				// store values for calculation
				if( currentValue < MAX_VALS ) {
					xValues[currentValue] = xLookup[xVal];
					yValues[currentValue] = FIX32(yVal);
					++currentValue;
				}
				if( currentValue == MAX_VALS ){
					calculateOffset();
					currentValue = 0;
					calibrateMode = FALSE;
				}
			}
		}


		// B
		if( changed == BUTTON_B && joypadState == BUTTON_B ) {
			currentValue = 0;
			calibrateMode = TRUE;
		}

		// C change render mode
		if( changed == BUTTON_C && joypadState == BUTTON_C ) {
			++crosshairsMode;
			if( crosshairsMode > 2 ) {
				crosshairsMode = 0;
			}
		}


	}
}

static void calculateXLookup() {
	// T2
	// $70-$83   :  112-131   : Total hvals 20
	// $84-$B6; $E5-$FF; $00-$42  :  132-182; 229-255; 0-66   :  51; 27; 67  -> only 51+27+66 = 145 hvals 
	// $43-$6F   : 67-111  : 45 hvals
	//  there's only 210 havls total with many offscreen
	//  The active area is 290 < 320 pixels.
	// since I'm going to bother with a calibration step  and offset I will use arbitray values
	//  in the lookup
	//
	//  My own experience has puts 84  the left edge of the monitor, so I'll start with 60
	fix32 pos = FIX32(-40);	
	for( int i=60; i < 183; ++i ) {
		xLookup[i] =  pos;
		pos = fix32Add( pos, FIX32(2) );
	}
	for( int i=229; i < 256; ++i ) {
		xLookup[i] =  pos;
		pos = fix32Add( pos, FIX32(2) );
	}
	for( int i=0; i < 60; ++i ) {
		xLookup[i] =  pos;
		pos = fix32Add( pos, FIX32(2) );
	}

}


int main(bool hard)
{

	///////////////////////////////////////////////////////////////////////////////////
	// Sprite Setup
	Sprite *targetSprite = NULL;
	VDP_setPalette( PAL1, target.palette->data );
	SPR_init();

	// crosshair is 16x16
	Sprite *crosshairsSprite = NULL;
	fix32 crosshairsPosX = FIX32(152.0);
	fix32 crosshairsPosY = FIX32(104.0);
	crosshairsSprite = SPR_addSprite( &crosshairs,  // Sprite defined in resources
			fix32ToInt(crosshairsPosX),// starting X position
			fix32ToInt(crosshairsPosY),// starting Y position
			TILE_ATTR( PAL1,           // specify palette
				1,            // Tile priority ( with background)
				FALSE,        // flip the sprite vertically?
				FALSE         // flip the sprite horizontally
				));

	// set target in the center position (target is 32x32 ) 
	fix32 targetPosX = FIX32(144.0); // 160 - 16 = 144
	fix32 targetPosY = FIX32(96.0);  // 112 - 16 = 96

	targetSprite = SPR_addSprite( &target,              // Sprite defined in resource.res
			fix32ToInt(targetPosX), // starting X position
			fix32ToInt(targetPosY), // starting Y position
			TILE_ATTR( PAL1,        // specify palette
				1,          // Tile priority ( with background)
				FALSE,      // flip the sprite vertically?
				FALSE       // flip the sprite horizontally
				));

	///////////////////////////////////////////////////////////////////////////////////
	// Menacer Setup
	// create lookup table
	calculateXLookup();

	// Set background brighter than 0.	Black background
	// prevents menacer from returning X, Y values.
	//VDP_setBackgroundColor( 4 );

    VDP_setPaletteColor(15, 0x0000);
    VDP_setTextPalette(0);
    VDP_setPaletteColor(0, 0x0844);

	// Asynchronous joystick handler. 
	JOY_setEventHandler (joypadHandler );


	// check Port 2 for the Sega Menacer
	bool menacerFound = FALSE;
	u8 portType = JOY_getPortType(PORT_2);
	if(portType == PORT_TYPE_MENACER )
	{
		JOY_setSupport(PORT_2, JOY_SUPPORT_MENACER);
		menacerFound = TRUE;
		VDP_drawText("Menacer FOUND!", 13, 1);
	} else {
		VDP_drawText("Menacer NOT found.", 11, 1);
	}
	 VDP_drawText("Press C to change drawing mode", 5, 5);

	///////////////////////////////////////////////////////////////////////////////////
	// Main Loop!
	while(TRUE)
	{
		if( menacerFound ) {	
			// get the button states		
			u16 value = JOY_readJoypad(JOY_2);
			if( value & BUTTON_A ) {
				VDP_drawText("A", 18, 9);
			} else {
				VDP_drawText(" ", 18, 9);
			}

			if( value & BUTTON_B ) {
				VDP_drawText("B", 20, 9);
			} else {
				VDP_drawText(" ", 20, 9);
			}

			if( value & BUTTON_C ) {
				VDP_drawText("C", 22, 9);
			} else {
				VDP_drawText(" ", 22, 9);
			}

			// The menacer appears to return 8-bit values (0 to 255)
			// if both values are -1, the gun is aiming off screen.	
			s16 xVal = JOY_readJoypadX(JOY_2);
			s16 yVal = JOY_readJoypadY(JOY_2);
			char message[40];
			sprintf( message, "Menacer Values x:%d, y:%d      ", xVal, yVal );
			VDP_drawText(message, 8, 7 );

			sprintf( message, "Offset Values x:%d, y:%d         ", fix32ToInt(xOffset),fix32ToInt( yOffset) );
			VDP_drawText(message, 7, 10 )

			if( calibrateMode ) {
				VDP_drawText("Aim at target and pull trigger", 5, 3);
			} else {
				VDP_drawText("Press B to start calibration  ", 5, 3);
			}

			// set crosshairs position. Subtract 8 from each to compensate for 16x16 sprite
			switch( crosshairsMode ) { 
			case 0: // raw
				VDP_drawText("   Render raw joypad values   ", 5, 6);
				crosshairsPosX = FIX32( xVal - 8 );
				crosshairsPosY = FIX32( yVal - 8 );
				break;
			case 1: // with lookup
				VDP_drawText("   Render with lookup table   ", 5, 6);
				crosshairsPosX = fix32Sub(xLookup[ xVal ], FIX32(8));
				crosshairsPosY = fix32Sub(FIX32( yVal  ),  FIX32(8));
				break;
			case 2: // with lookup + offset
				VDP_drawText("  Render with lookup + offset ", 5, 6);
				crosshairsPosX = fix32Sub(fix32Add(xLookup[xVal], xOffset), FIX32(8));
				crosshairsPosY = fix32Sub(fix32Add(FIX32( yVal ), yOffset), FIX32(8));
			default:
				break;
			}
		}

		// Set the Sprite Positions.
		// SPR_setPosition( targetSprite, fix32ToInt( targetPosX ), fix32ToInt( targetPosY ) );
		SPR_setPosition( crosshairsSprite, fix32ToInt( crosshairsPosX ), fix32ToInt( crosshairsPosY ) );
		SPR_update();

		SYS_doVBlankProcess(); 

	}
}

