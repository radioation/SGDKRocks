#include <genesis.h>

// 1x1
const u32 square_tile[] =
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


// Numbers
const u32 numbers_tiles[] =
{
  //
  0x00011000,
  0x00111000,
  0x00011000,
  0x00011000,
  0x00011000,
  0x00011000,
  0x01111110,
  0x00000000,

  0x00222200,
  0x02200220,
  0x00000220,
  0x00002200,
  0x00022000,
  0x00220000,
  0x02222220,
  0x00000000,

  0x00333300,
  0x03300330,
  0x00000330,
  0x00033300,
  0x00000330,
  0x03300330,
  0x00333300,
  0x00000000,

  0x00004440,
  0x00044440,
  0x00440440,
  0x04400440,
  0x04444444,
  0x00000440,
  0x00000440,
  0x00000000,

  0x05555550,
  0x05500000,
  0x05555500,
  0x05500550,
  0x00000550,
  0x05500550,
  0x00555500,
  0x00000000,

  0x00666600,
  0x06600660,
  0x06600000,
  0x06666600,
  0x06600660,
  0x06600660,
  0x00666600,
  0x00000000,

  0x07777770,
  0x00000770,
  0x00007700,
  0x00007700,
  0x00077000,
  0x00077000,
  0x00077000,
  0x00000000,

  0x00888800,
  0x08800880,
  0x08800880,
  0x00888800,
  0x08800880,
  0x08800880,
  0x00888800,
  0x00000000,


  0x00999900,
  0x09900960,
  0x09900990,
  0x00099990,
  0x00000990,
  0x09900990,
  0x00999900,
  0x00000000,




};




int main(bool hard)
{
  //////////////////////////////////////////////////////////////////
  // set colors.  
  VDP_setBackgroundColor(4);

  //////////////////////////////////////////////////////////////////
  // Load the tilesets
  int square_ind = TILE_USER_INDEX;

  VDP_loadTileData( square_tile, // tile data pointer
      square_ind,                // index
      1,                         // number of tiles to load 
      DMA_QUEUE                  // transfer method
      );

  int numbers_ind = numbers_ind + 1;     // add 1 for previous tilset 
  VDP_loadTileData( numbers_tiles, 
      numbers_ind,    
      9,                          // load 4 tiles 
      DMA_QUEUE         
      );

  //////////////////////////////////////////////////////////////////
  // Add Sprites
  VDP_setSpriteFull(0, // sprite ID ( Can be 0 to 79, start with 0 )
      40,   // X in screen coords
      40,   // Y in screen coords
      SPRITE_SIZE(1,1), // 1x1 to up to 4x4
      TILE_ATTR_FULL(PAL2,    // PALette 
        1,  // priority
        0,  // Flip Vertical
        0,  // Flip Horizontal
        square_ind // index
        ),
      1 // link to next sprite  (which will be 1)
      );

  VDP_setSpriteFull(1, // sprite ID ( 1 is the next sprite id )
      80,   // X in screen coords
      40,   // Y in screen coords
      SPRITE_SIZE(1,2), // 1x1 to up to 4x4
      TILE_ATTR_FULL(PAL2,    // PALette 
        1,  // priority
        0,  // Flip Vertical
        0,  // Flip Horizontal
        numbers_ind // index
        ),
      2 // link to next sprite
      );

  VDP_setSpriteFull(2, // sprite ID 
      120,  // X in screen coords
      40,   // Y in screen coords
      SPRITE_SIZE(2,1), // 1x1 to up to 4x4
      TILE_ATTR_FULL(PAL2,    // PALette 
        1,  // priority
        0,  // Flip Vertical
        0,  // Flip Horizontal
        numbers_ind // index
        ),
      3 // link to next sprite
      );


  VDP_setSpriteFull(3, // sprite ID ( 1 is the next psrite id
      160,  // X in screen coords
      40,   // Y in screen coords
      SPRITE_SIZE(2,2), // 1x1 to up to 4x4
      TILE_ATTR_FULL(PAL2,    // PALette 
        1,  // priority
        0,  // Flip Vertical
        0,  // Flip Horizontal
        numbers_ind // index
        ),
      4 //  last sprite.
      );

  VDP_setSpriteFull(4, // sprite ID ( 1 is the next psrite id
      200,  // X in screen coords
      40,   // Y in screen coords
      SPRITE_SIZE(2,3), // 1x1 to up to 4x4
      TILE_ATTR_FULL(PAL2,    // PALette 
        1,  // priority
        0,  // Flip Vertical
        0,  // Flip Horizontal
        numbers_ind // index
        ),
      5 //  last sprite.
      );

  VDP_setSpriteFull(5, // sprite ID ( 1 is the next psrite id
      240,  // X in screen coords
      40,   // Y in screen coords
      SPRITE_SIZE(2,4), // 1x1 to up to 4x4
      TILE_ATTR_FULL(PAL2,    // PALette 
        1,  // priority
        0,  // Flip Vertical
        0,  // Flip Horizontal
        numbers_ind // index
        ),
      6 //  last sprite.
      );

  VDP_setSpriteFull(6, // sprite ID 
      80,   // X in screen coords
      90,   // Y in screen coords
      SPRITE_SIZE(1,3), // 1x1 to up to 4x4
      TILE_ATTR_FULL(PAL2,    // PALette 
        1,  // priority
        0,  // Flip Vertical
        0,  // Flip Horizontal
        numbers_ind // index
        ),
      7 // link to next sprite
      );

  VDP_setSpriteFull(7, // sprite ID 
      120,  // X in screen coords
      90,   // Y in screen coords
      SPRITE_SIZE(3,1), // 1x1 to up to 4x4
      TILE_ATTR_FULL(PAL2,    // PALette 
        1,  // priority
        0,  // Flip Vertical
        0,  // Flip Horizontal
        numbers_ind // index
        ),
      8 // link to next sprite
      );

  VDP_setSpriteFull(8, // sprite ID 
      160,  // X in screen coords
      90,   // Y in screen coords
      SPRITE_SIZE(3,2), // 1x1 to up to 4x4
      TILE_ATTR_FULL(PAL2,    // PALette 
        1,  // priority
        0,  // Flip Vertical
        0,  // Flip Horizontal
        numbers_ind // index
        ),
      9 // link to next sprite
      );

  VDP_setSpriteFull(9, // sprite ID 
      200,  // X in screen coords
      90,   // Y in screen coords
      SPRITE_SIZE(3,3), // 1x1 to up to 4x4
      TILE_ATTR_FULL(PAL2,    // PALette 
        1,  // priority
        0,  // Flip Vertical
        0,  // Flip Horizontal
        numbers_ind // index
        ),
      0 // link to next sprite
      );



  // Update 
  VDP_updateSprites(10, DMA_QUEUE_COPY);

  VDP_drawText("1x1  1x2  2x1  2x2  2x3  2x4", 5, 3);
  VDP_drawText("     1x3  3x1  3x3  3x3" , 5, 10);
  while(TRUE)
  {
    // do VBLankd processes
    SYS_doVBlankProcess();
  }

  return 0;


};




