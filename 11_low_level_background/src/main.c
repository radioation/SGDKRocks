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
      5); // Y


  while(1)
  {
    // ... code
    SYS_doVBlankProcess();
  }


  return 0;
}
