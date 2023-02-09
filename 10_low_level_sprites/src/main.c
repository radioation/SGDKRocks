#include <genesis.h>

const u32 tile[8] =
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

  VDP_loadTileData( tile, // tile data pointer
      ind,    // index
      1,        // number of tiles to load 
      DMA_QUEUE         // transfer method
      );

  VDP_setSprite(0, // sprite ID ( 0 to 79 )
      40,   // X in screen coords
      40,   // Y in screen coords
      SPRITE_SIZE(1,1), // 1x1 to up to 4x4
      TILE_ATTR_FULL(PAL0,    // PALette 
        1,  // priority
        0,  // Flip Vertical
        0,  // Flip Horizontal
        ind // index
        ) 
      );
  // tell VDP to draw sprites
  VDP_updateSprites(1, // number of sprites to transfer starting at index 0
      DMA_QUEUE);


  while(TRUE)
  {
    // do VBLankd processes
    SYS_doVBlankProcess();
  }

  return 0;
}

