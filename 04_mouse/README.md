# Mouse Example

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
      // mice return signed axis data. Change in value
      // indicates movment.
      x_mouse = JOY_readJoypadX(JOY_2); // positive change indicates movement to the right
      y_mouse = JOY_readJoypadY(JOY_2); // positive change indicates movement to the left
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

