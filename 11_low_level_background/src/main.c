#include <genesis.h>

const u32 tile[8]=
{
  0xEEEEEEEE,
  0x44444444,
  0xEEEEEEEE,
  0x44444444,
  0xEEEEEEEE,
  0x44444444,
  0xEEEEEEEE,
  0x44444444
};


int main(bool arg) 
{

  // Load the tile into VRAM with VDP_loadTileData
  s16 ind = TILE_USER_INDEX; 
  VDP_loadTileData( (const u32 *)tile, // tile data pointer
      ind,   // index 
      1,   // number of tiles to load
      DMA_QUEUE    // transfer method
      ); 

  // Copy our tile from VRAM to BG_B at (5,5) with pal 1
  VDP_setTileMapXY(BG_B,
      TILE_ATTR_FULL( PAL1,// Palette
        1,                // Priority
        0,                // Flip Vertical
        0,                // FLip Horizontal
        ind),             // tile index
      5,  // X
      0); // Y

  // make a row of 40 tiles and load int VRAM
  s16 rowIndex = ind + 1;
  u32 myTiles[320];
  memset( myTiles,0, sizeof(myTiles) );
  memset( myTiles, 0x0E, 32 ); // set the first tile to 0x0E
  VDP_loadTileData( (const u32 *)myTiles, // tile data pointer
      rowIndex,   // index 
      40,   // number of tiles to load
      DMA_QUEUE    // transfer method
      ); 

  // set 20 rows to use the tiles we loaded into VRAM
  for( u16 y=1; y< 20; ++y ) {
    VDP_fillTileMapRectInc( BG_B,
        TILE_ATTR_FULL( PAL3,// Palette
          1,                // Priority
          0,                // Flip Vertical
          0,                // FLip Horizontal
          rowIndex),             // tile index
        0,  // x
        y,  // y
        40,  // width
        1  // height
        ); 
  }

  s16 offset=0;  

  while(1)
  {
    // update the buffer
    memset( myTiles+offset, 0x0,32);
    offset+=8;
    if( offset > 312 ) {
      offset=0;
    }
    memset( myTiles+offset, 0x0E, 32 );
    // load into VRAM
    VDP_loadTileData( (const u32 *)myTiles, // tile data pointer
        rowIndex,   // index 
        40,   // number of tiles to load
        DMA_QUEUE    // transfer method
        ); 

    // let SGDK do its thing.
    SYS_doVBlankProcess();
  }

  return 0;
}
