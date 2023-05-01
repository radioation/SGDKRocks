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

const u32 tileSet0[288] =
{
  // tile 0 
  0x00000000,
  0x00000000,
  0x00000000,
  0x77777777,
  0x66666666,
  0x55555555,
  0x44444444,
  0x44444444,
  // tile 1 
  0x33333333,
  0x33333333,
  0x22222222,
  0x22222222,
  0x11111111,
  0x11111111,
  0x88888888,
  0x88888888,
  // tile 2 
  0x88888888,
  0x77777777,
  0x77777777,
  0x77777777,
  0x77777777,
  0x66666666,
  0x66666666,
  0x66666666,
  // tile 3 
  0x66666666,
  0x55555555,
  0x55555555,
  0x55555555,
  0x55555555,
  0x55555555,
  0x55555555,
  0x44444444,
  // tile 4 
  0x44444444,
  0x44444444,
  0x44444444,
  0x44444444,
  0x44444444,
  0x44444444,
  0x33333333,
  0x33333333,
  // tile 5 
  0x33333333,
  0x33333333,
  0x33333333,
  0x33333333,
  0x33333333,
  0x33333333,
  0x33333333,
  0x33333333,
  // tile 6 
  0x22222222,
  0x22222222,
  0x22222222,
  0x22222222,
  0x22222222,
  0x22222222,
  0x22222222,
  0x22222222,
  // tile 7 
  0x22222222,
  0x22222222,
  0x22222222,
  0x22222222,
  0x22222222,
  0x11111111,
  0x11111111,
  0x11111111,
  // tile 8 
  0x11111111,
  0x11111111,
  0x11111111,
  0x11111111,
  0x11111111,
  0x11111111,
  0x11111111,
  0x11111111
};

const u32 tileSet1[72] =
{
  // tile 0 
  0x00000000,
  0x00000000,
  0x00000000,
  0x77770000,
  0x66667777,
  0x55556666,
  0x44445555,
  0x44444444,
  // tile 1 
  0x33334444,
  0x33333333,
  0x22223333,
  0x22222222,
  0x11112222,
  0x11111111,
  0x88881111,
  0x88888888,
  // tile 2 
  0x88888888,
  0x77778888,
  0x77777777,
  0x77777777,
  0x77777777,
  0x66667777,
  0x66666666,
  0x66666666,
  // tile 3 
  0x66666666,
  0x55556666,
  0x55555555,
  0x55555555,
  0x55555555,
  0x55555555,
  0x55555555,
  0x44445555,
  // tile 4 
  0x44444444,
  0x44444444,
  0x44444444,
  0x44444444,
  0x44444444,
  0x44444444,
  0x33334444,
  0x33333333,
  // tile 5 
  0x33333333,
  0x33333333,
  0x33333333,
  0x33333333,
  0x33333333,
  0x33333333,
  0x33333333,
  0x33333333,
  // tile 6 
  0x22223333,
  0x22222222,
  0x22222222,
  0x22222222,
  0x22222222,
  0x22222222,
  0x22222222,
  0x22222222,
  // tile 7 
  0x22222222,
  0x22222222,
  0x22222222,
  0x22222222,
  0x22222222,
  0x11112222,
  0x11111111,
  0x11111111,
  // tile 8 
  0x00000000,
  0x00000000,
  0x00000000,
  0x00000000,
  0x00000000,
  0x00000000,
  0x00000000,
  0x00000000
};

int main(bool arg) 
{
  //////////////////////////////////////////////////////////////////////
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


  //////////////////////////////////////////////////////////////////////
  // make a row of 40 tiles and load int VRAM
  s16 rowIndex = ind + 1;
  u32 rowTiles[320];
  memset( rowTiles,0, sizeof(rowTiles) );
  memset( rowTiles, 0x0E, 32 ); // set the first tile to 0x0E
  VDP_loadTileData( (const u32 *)rowTiles, // tile data pointer
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


  //////////////////////////////////////////////////////////////////////
  // Load the tileSet0 into VRAM with VDP_loadTileData
  //s16 tileSet0Index = rowIndex + 40;
  //VDP_loadTileData( (const u32 *)tileSet0, // tile data pointer
  //    tileSet0Index,   // index 
  //    9,   // number of tiles to load
  //    DMA_QUEUE    // transfer method
  //    ); 
  s16 colIndex = rowIndex + 40;
  u32 colTiles[72];
  memset( colTiles,0, sizeof(colTiles) );
  memcpy( colTiles, tileSet0, sizeof( colTiles ) ); // copy the column data into ram
  VDP_loadTileData( (const u32 *)colTiles, // tile data pointer
      colIndex,   // index 
      9,   // number of tiles to load
      DMA_QUEUE    // transfer method
      ); 

  for( u16 x=1; x< 40; ++x ) {
    // make a column out of it.
    VDP_fillTileMapRectInc( BG_B,
        TILE_ATTR_FULL( PAL2,// Palette
          1,                // Priority
          0,                // Flip Vertical
          0,                // FLip Horizontal
          colIndex),             // tile index
        x,  // x
        20,  // y
        1,  // width
        9  // height
        ); 
  }

  s16 offset=0;  
  s16 colSet = 0;
  while(1)
  {
    // update the row data
    memset( rowTiles+offset, 0x0,32);
    offset+=8;
    if( offset > 312 ) {
      offset=0;
    }
    memset( rowTiles+offset, 0x0E, 32 );
    // load into VRAM
    VDP_loadTileData( (const u32 *)rowTiles, // tile data pointer
        rowIndex,   // index 
        40,   // number of tiles to load
        DMA_QUEUE    // transfer method
        ); 


    // update the col data
    if( colSet == 1 )  {
      memcpy( colTiles, tileSet1, sizeof( colTiles ) ); 
    } else {
      memcpy( colTiles, tileSet0, sizeof( colTiles ) );
    }
    colSet  += 1;
    if( colSet > 1 ) {
      colSet = 0;
    }
    VDP_loadTileData( (const u32 *)colTiles, // tile data pointer
        colIndex,   // index 
        9,   // number of tiles to load
        DMA_QUEUE    // transfer method
        ); 


    // let SGDK do its thing.
    SYS_doVBlankProcess();
  }

  return 0;
}

