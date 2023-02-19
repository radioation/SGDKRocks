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

	// Load the tile
	int ind = TILE_USER_INDEX;

	VDP_loadTileData( (const u32 *)tile, // tile data pointer
			ind,   // index 
			1,   // number of tiles to load
			DMA_QUEUE    // transfer method
			); 

	//write our tile 1 on plane A at (5,5) with pal 0
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
