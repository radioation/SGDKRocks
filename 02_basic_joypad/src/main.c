#include <genesis.h>

static void printJoy( u16 joypadId, u16 joypadState, u16 x, u16 y ) {
  u16 ypos = y;
  char message[40];
  // JOYX,UP,DOWN,LEFT,RIGHT,A,B,C,X,Y,Z,STRT
  if( joypadId == JOY_1 ) {
    sprintf(message,"JOY1 ");
  } else {
    sprintf(message,"JOY2 ");
    ++ypos;
  }

  // check for the joystick direction
  if( joypadState & BUTTON_UP ) {
    strcat(message,"UP ");
  }else {
    strcat(message,"   ");
  }
  if( joypadState & BUTTON_DOWN ) {
    strcat(message,"DOWN ");
  }else {
    strcat(message,"     ");
  }
  if( joypadState & BUTTON_LEFT ) {
    strcat(message,"LEFT ");
  }else {
    strcat(message,"     ");
  }
  if( joypadState & BUTTON_RIGHT ) {
    strcat(message,"RIGHT ");
  }else {
    strcat(message,"      ");
  }
  if( joypadState & BUTTON_A ) {
    strcat(message,"A ");
  }
  else  {
    strcat(message,"  ");
  }

  if( joypadState & BUTTON_B ) {
    strcat(message,"B ");
  }
  else  {
    strcat(message,"  ");
  }

  if( joypadState & BUTTON_C ) {
    strcat(message,"C ");
  }
  else  {
    strcat(message,"  ");
  }

  if( joypadState & BUTTON_X ) {
    strcat(message,"X ");
  }
  else  {
    strcat(message,"  ");
  }

  if( joypadState & BUTTON_Y ) {
    strcat(message,"Y ");
  }
  else  {
    strcat(message,"  ");
  }

  if( joypadState & BUTTON_Z ) {
    strcat(message,"Z ");
  }
  else  {
    strcat(message,"  ");
  }

  if( joypadState & BUTTON_START ) {
    strcat(message,"STRT");
  }
  else  {
    strcat(message,"    ");
  }
  VDP_drawText(message, 0, ypos );

}



// Synchronous Joypad function. You periodically call this function in the
// main loop to read the current state of the controllers
static void readJoypad( u16 joypadId ) {

  // You pick the joypad you want to read by passing
  // JOY_1, JOY_2, ... JOY_8 to JOY_readJoypad()
  // The value it returns holds the current joypad state
  u16 joypadState = JOY_readJoypad( joypadId );
  printJoy(joypadId, joypadState, 0, 2 );
}


// joypad event handler.  This gets called automatically by SGDK when the joypad
// state changes
static void joypadHandler( u16 joypadId, u16 changed, u16 joypadState ) {
  VDP_setTextPalette(1);
  printJoy( joypadId, joypadState, 0, 6 );
  VDP_setTextPalette(0);
}



int main(bool hard)
{
  VDP_drawText("SYNCH: readJoypad()", 0, 1 );
  VDP_drawText("ASYNCH: joypadHandler()", 0, 5 );


  // Asynchronous joystick handler.
  JOY_setEventHandler (joypadHandler );


  while(TRUE)
  {
    // 1) read input Syncrhonously
    readJoypad(JOY_1);
    readJoypad(JOY_2);

		// Wait for VBlank
    SYS_doVBlankProcess();
  }

  return 0;
}


