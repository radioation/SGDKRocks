#include <genesis.h>
#include "resources.h"
#include "explosion.h"


int main(bool hard)
{
  // set backgorund color.  
  VDP_setBackgroundColor(0);

  // load the palettes
  PAL_setPalette( PAL1, shipsheet_pal.data, CPU );


  // Load the tilesets
  int shipsheet_ind = TILE_USER_INDEX;
  VDP_loadTileData( shipsheet_tileset.tiles, // tile data pointer
      shipsheet_ind,    // index
      160,        // number of tiles to load 
      DMA_QUEUE         // transfer method
      );


  s16 x = 160;
  s16 y = 120;
  s16 shipframe_offset = 0;
  s16 shipframe_dir = 1;
  VDP_setSpriteFull(0, // sprite ID ( 0 to 79 )
      x,   // X in screen coords
      y,   // Y in screen coords
      SPRITE_SIZE(4,4), // 1x1 to up to 4x4
      TILE_ATTR_FULL(PAL1,    // PALette 
        1,  // priority
        0,  // Flip Vertical
        0,  // Flip Horizontal
        shipsheet_ind + shipframe_offset  // index
        ) ,
      1  // next sprite ID
      );

  VDP_setSpriteFull(1, // sprite ID ( 0 to 79 )
      x,   // X in screen coords
      y - 60,   // Y in screen coords
      SPRITE_SIZE(4,4), // 1x1 to up to 4x4
      TILE_ATTR_FULL(PAL1,    // PALette 
        1,  // priority
        1,  // Flip Vertical
        0,  // Flip Horizontal
        shipsheet_ind + shipframe_offset  + 80  // index
        ) ,
      0  // next sprite ID
      );


  // tell VDP to draw the sprites we've defined.
  s16 numSprites = 2;
  VDP_updateSprites(numSprites, DMA_QUEUE_COPY);
  while(TRUE)
  {
    // move sprite position
    ++x; 

    // chagne current animiation frame by computing offset in tile memory
    if ( x % 10 == 0) {
      if( shipframe_dir > 0 ) {
        shipframe_offset += 16;
      } else {
        shipframe_offset -= 16;
      }
      if( shipframe_offset > 64 ) {
        shipframe_dir = -1;
        shipframe_offset = 48;
      }
      if( shipframe_offset < 0  ) {
        shipframe_dir = 1;
        shipframe_offset = 16;
      }
    }

    // update position of srpite 0
    VDP_setSpritePosition(0, // sprite ID ( 0 to 79 )
        x,   // X in screen coords
        y   // Y in screen coords
        );

    // update animation frame of sprite 5 by setting tile index.
    VDP_setSpriteTile(0, // sprite ID ( 0 to 79 )
        shipsheet_ind + shipframe_offset  // tile index
        );

    // update position of srpite 5
    VDP_setSpritePosition(1, // sprite ID ( 0 to 79 )
        x,   // X in screen coords
        y - 80  // Y in screen coords
        );

    // update animation frame of sprite 5 by setting tile index.
    VDP_setSpriteTile(1, // sprite ID ( 0 to 79 )
        shipsheet_ind + shipframe_offset + 80  // tile index for BLUE ship
        );

    s16 offset = 0;

    // tell VDP to draw  the 6 sprites we've defined.
    VDP_updateSprites(numSprites, DMA_QUEUE_COPY);

    // do VBLankd processes
    SYS_doVBlankProcess();
  }

  return 0;
}

