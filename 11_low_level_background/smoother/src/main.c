#include <genesis.h>
#include "ground.h"


extern void drawAt(char* dest, char* source, s16 startPixel, s16 lastTilePixel, s16 tiles );

// tiles for row example
const u32 tile[] =
{
  0x80808080,
  0x07080808,
  0x87608080,
  0x07650808,
  0x87654080,
  0x07654308,
  0x87654320,
  0x07654321,

  0x00000000,
  0xEEEEEEEE,
  0x00000000,
  0xEEEEEEEE,
  0x00000000,
  0xEEEEEEEE,
  0x00000000,
  0xEEEEEEEE,

  0x07654321,
  0x87654320,
  0x07654308,
  0x87654080,
  0x07650808,
  0x87608080,
  0x07080808,
  0x80808080
};


int main(bool arg)
{
  PAL_setColor(0, 0x0844);
  //////////////////////////////////////////////////////////////////////
  // Load the tile into VRAM with VDP_loadTileData
  s16 ind = TILE_USER_INDEX;
  VDP_loadTileData((const u32 *)tile, // tile data pointer
      ind,               // index
      1,                 // number of tiles to load
      DMA_QUEUE          // transfer method
      );

  // Copy our tile from VRAM to BG_B at (5,5) with pal 1
  VDP_setTileMapXY(BG_B,
      TILE_ATTR_FULL(PAL1, // Palette
        1,    // Priority
        0,    // Flip Vertical
        0,    // FLip Horizontal
        ind), // tile index
      5,                   // X
      0);                  // Y

  //////////////////////////////////////////////////////////////////////
  // make a row of 40 tiles and load int VRAM
  s16 rowIndex = ind + 1;
  u32 rowTiles[320]; // 40 tiles in a row * 8 rows per tile = 320 elements
  memset(rowTiles, 0, sizeof(rowTiles));
  memset(rowTiles, 0x0E, 32);             // set the first tile to 0x0E
  VDP_loadTileData((const u32 *)rowTiles, // tile data pointer
      rowIndex,              // index
      40,                    // number of tiles to load
      DMA_QUEUE              // transfer method
      );

  // set 23 rows to use the tiles we loaded into VRAM
  for (u16 y = 1; y < 20; ++y)
  {
    VDP_fillTileMapRectInc(BG_B,
        TILE_ATTR_FULL(PAL3,      // Palette
          1,         // Priority
          0,         // Flip Vertical
          0,         // FLip Horizontal
          rowIndex), // tile index
        0,                        // x
        y,                        // y
        40,                       // width
        1                         // height
        );
  }

  //////////////////////////////////////////////////////////////////////
  // Load the ground into VRAM with VDP_loadTileData
  s16 colIndex = rowIndex + 40; // rowTiles is 40 actual tiles.
  u32 colTiles[80]; // 10 rows in the column * 8 rows per tile is 80 elements.
  memset(colTiles, 0, sizeof(colTiles));
  memcpy(colTiles, ground, sizeof(colTiles) ); //sizeof(colTiles)); // copy the column data into ram
  VDP_loadTileData((const u32 *)colTiles,       // tile data pointer
      colIndex,                    // index
      10,                          // number of tiles to load
      DMA_QUEUE                    // transfer method
      );

  for (u16 x = 0; x < 40; ++x)
  {
    // make a column out of it.
    VDP_fillTileMapRectInc(BG_B,
        TILE_ATTR_FULL(PAL2,      // Palette
          1,         // Priority
          0,         // Flip Vertical
          0,         // FLip Horizontal
          colIndex), // tile index
        x,                        // x
        20,                       // y
        1,                        // width
        10                         // height (10 tiles)
        );
  }

  s16 offset = 1;
  s16 groundFrame = 0;
  s16 delay = 0;
  while (1)
  {

    ///////////////////////////////////////////////////////
    // update the row data
    memset(rowTiles, 0, sizeof(rowTiles));
    ++offset	;
    if( offset > 290) {
      offset = 0;
    }

    s16 startPixel = offset;

    s16 lastTilePixel = 7;
    s16 tiles = 3;

    drawAt( &rowTiles[0], &tile[0],  startPixel, lastTilePixel, tiles );
    // load row into VRAM
    VDP_loadTileData((const u32 *)rowTiles, // tile data pointer
        rowIndex,              // index
        40,                    // number of tiles to load
        DMA_QUEUE              // transfer method
        );

    ///////////////////////////////////////////////////////
    // update the col data

    memcpy(colTiles, ground + (groundFrame * 80),  sizeof(colTiles ));
    VDP_loadTileData((const u32 *)colTiles, // tile data pointer
        colIndex,              // index
        10,                     // number of tiles to load
        DMA_QUEUE              // transfer method
        );
    delay +=1;
    if( delay > 3 ) {
        delay = 0;
        groundFrame += 1;
        if (groundFrame > 5)
        {
            groundFrame = 0;
        }
    }

    // let SGDK do its thing.
    //VDP_waitVSync();

    SYS_doVBlankProcess();
  }

  return 0;
}
