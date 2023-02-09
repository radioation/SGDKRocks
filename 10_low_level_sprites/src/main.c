#include <genesis.h>
#include "resources.h"

const u32 square_tile[8] =
{
  0x11111111,
  0x12222221,
  0x12333321,
  0x12344321,
  0x12344321,
  0x12333321,
  0x12222221,
  0x11111111
};


int main(bool hard)
{
  // set backgorund color.  
  VDP_setBackgroundColor(0);

  // load the palettes
  PAL_setPalette( PAL1, ship_pal.data, CPU );
  PAL_setPalette( PAL2, rock_pal.data, CPU );

  int ind = TILE_USER_INDEX;

  VDP_loadTileData( square_tile, // tile data pointer
      ind,    // index
      1,        // number of tiles to load 
      DMA_QUEUE         // transfer method
      );

  int ship_ind = ind + 1;
  VDP_loadTileData( ship_tileset.tiles, // tile data pointer
      ship_ind,    // index
      4,        // number of tiles to load 
      DMA_QUEUE         // transfer method
      );

  int rock_ind = ship_ind + 4; // 4 because we loaded 4 tiles for ship
  VDP_loadTileData( rock_tileset.tiles, // tile data pointer
      rock_ind,    // index
      4,        // number of tiles to load 
      DMA_QUEUE         // transfer method
      );

  VDP_setSpriteFull(0, // sprite ID ( 0 to 79 )
      40,   // X in screen coords
      40,   // Y in screen coords
      SPRITE_SIZE(1,1), // 1x1 to up to 4x4
      TILE_ATTR_FULL(PAL0,    // PALette 
        1,  // priority
        0,  // Flip Vertical
        0,  // Flip Horizontal
        ind // index
        ),
      1 // link to next sprite
      );

  VDP_setSpriteFull(1, // sprite ID ( 0 to 79 )
      60,   // X in screen coords
      40,   // Y in screen coords
      SPRITE_SIZE(1,1), // 1x1 to up to 4x4
      TILE_ATTR_FULL(PAL0,    // PALette 
        1,  // priority
        0,  // Flip Vertical
        0,  // Flip Horizontal
        ind // index
        ) ,
        2 // link to next sprite
      );


  VDP_setSpriteFull(2, // sprite ID ( 0 to 79 )
      80,   // X in screen coords
      40,   // Y in screen coords
      SPRITE_SIZE(2,2), // 1x1 to up to 4x4
      TILE_ATTR_FULL(PAL1,    // PALette 
        1,  // priority
        0,  // Flip Vertical
        0,  // Flip Horizontal
        ship_ind // index
        ) ,
      3
      );


  VDP_setSpriteFull(3, // sprite ID ( 0 to 79 )
      100,   // X in screen coords
      40,   // Y in screen coords
      SPRITE_SIZE(2,2), // 1x1 to up to 4x4
      TILE_ATTR_FULL(PAL2,    // PALette 
        1,  // priority
        0,  // Flip Vertical
        0,  // Flip Horizontal
        rock_ind // index
        ) ,
      4
      );


  // tell VDP to draw sprites
  VDP_updateSprites(4, DMA_QUEUE_COPY);
 
  //  VDP_updateSprites(80, // number of sprites to transfer starting at index 0
   //   DMA_QUEUE);


  while(TRUE)
  {
    // do VBLankd processes
    SYS_doVBlankProcess();
  }

  return 0;
}

