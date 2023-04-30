#include <genesis.h>
#include "resources.h"
#include "explosion.h"


int main(bool hard)
{
  ////////////////////////////////////////////////////////
  //  Setup colors
  VDP_setBackgroundColor(0);

  // palette from RESCOMP
  PAL_setPalette( PAL1, shipsheet_pal.data, CPU );

  // palette from C array.
  PAL_setColors( 32, explosion_pal, 15, CPU );



  ////////////////////////////////////////////////////////
  // Load the tilesets
  int shipsheet_ind = TILE_USER_INDEX;
  VDP_loadTileData( shipsheet_tileset.tiles, // tile data pointer
      shipsheet_ind,    // index
      160,        // number of tiles to load 
      DMA_QUEUE         // transfer method
      );

  int explosion_ind = shipsheet_ind + 160;
  VDP_loadTileData( explosion_tiles, // tile data pointer
      explosion_ind,    // index
      336,        // number of tiles to load 
      DMA_QUEUE         // transfer method
      );


  ////////////////////////////////////////////////////////
  // Create Sprites
  s16 x = 160;
  s16 y = 140;

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
        shipsheet_ind + shipframe_offset  + 80  // index of second row
        ) ,
      2  // next sprite ID
      );

  s16 explosion_offset = 0;
  VDP_setSpriteFull(2, // sprite ID ( 0 to 79 )
      144,   // X in screen coords
      96,   // Y in screen coords
      SPRITE_SIZE(4,4), // 1x1 to up to 4x4
      TILE_ATTR_FULL(PAL2,    // PALette 
        1,  // priority
        0,  // Flip Vertical
        0,  // Flip Horizontal
        explosion_ind // first row
        ) ,
      3  // next sprite ID
      );


  VDP_setSpriteFull(3, // sprite ID ( 0 to 79 )
      100,   // X in screen coords
      96,   // Y in screen coords
      SPRITE_SIZE(4,4), // 1x1 to up to 4x4
      TILE_ATTR_FULL(PAL2,    // PALette 
        1,  // priority
        0,  // Flip Vertical
        0,  // Flip Horizontal
        explosion_ind // first row
        ) ,
      4  // next sprite ID
      );

  VDP_setSpriteFull(4, // sprite ID ( 0 to 79 )
      188,   // X in screen coords
      96,   // Y in screen coords
      SPRITE_SIZE(4,4), // 1x1 to up to 4x4
      TILE_ATTR_FULL(PAL2,    // PALette 
        1,  // priority
        0,  // Flip Vertical
        0,  // Flip Horizontal
        explosion_ind // first row
        ) ,
      0  // next sprite ID
      );



  // tell VDP to draw the sprites we've defined.
  s16 numSprites = 5;
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
      explosion_offset += 16;
      if( explosion_offset > 96 ) {
        explosion_offset = 0;
      }
    }

    // update position of srpite 0
    VDP_setSpritePosition(0, // sprite ID ( 0 to 79 )
        x,   // X in screen coords
        y   // Y in screen coords
        );

    // update animation frame of sprite 0 by setting tile index.
    VDP_setSpriteTile(0, // sprite ID ( 0 to 79 )
        shipsheet_ind + shipframe_offset  // tile index
        );

    // update position of srpite 1
    VDP_setSpritePosition(1, // sprite ID ( 0 to 79 )
        x,   // X in screen coords
        y - 100  // Y in screen coords
        );

    // update animation frame of sprite 1 by setting tile index.
    VDP_setSpriteTile(1, // sprite ID ( 0 to 79 )
        shipsheet_ind + shipframe_offset + 80  // tile index for BLUE ship
        );

    VDP_setSpriteTile(2, 
        explosion_ind + explosion_offset 
        );
    VDP_setSpriteTile(3,
        explosion_ind + explosion_offset + 112
        );
    VDP_setSpriteTile(4,
        explosion_ind + explosion_offset + 224
        );


    // tell VDP to draw  the 6 sprites we've defined.
    VDP_updateSprites(numSprites, DMA_QUEUE_COPY);

    // do VBLankd processes
    SYS_doVBlankProcess();
  }

  return 0;
}

