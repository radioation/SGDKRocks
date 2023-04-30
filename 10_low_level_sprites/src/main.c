#include <genesis.h>
#include "resources.h"
#include "dino.h"
#include "explosion.h"

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
  PAL_setPalette( PAL0, ship_pal.data, CPU );
  PAL_setPalette( PAL1, rock_pal.data, CPU );
  PAL_setPalette( PAL2, dino_pal.data, CPU );
  PAL_setPalette( PAL3, shipsheet_pal.data, CPU );


  // Load the tilesets
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

  int numbers_ind = rock_ind + 4; // 4 because we loaded 4 tiles for rock
  VDP_loadTileData( numbers_tileset.tiles, // tile data pointer
      numbers_ind,    // index
      32,        // number of tiles to load 
      DMA_QUEUE         // transfer method
      );

  int shipsheet_ind = numbers_ind + 32; 
  VDP_loadTileData( shipsheet2_tileset.tiles, // tile data pointer
      shipsheet_ind,    // index
      160,        // number of tiles to load 
      DMA_QUEUE         // transfer method
      );

  int dino_ind = shipsheet_ind + 160; 
  VDP_loadTileData( dino_frame_0, // tile data pointer
      dino_ind,    // index
      dino_frame_0_tiles,        // number of tiles to load 
      DMA_QUEUE         // transfer method
      );


  // Add Sprites
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
      TILE_ATTR_FULL(PAL0,    // PALette 
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
      TILE_ATTR_FULL(PAL1,    // PALette 
        1,  // priority
        0,  // Flip Vertical
        0,  // Flip Horizontal
        rock_ind // index
        ) ,
      4
      );


  VDP_setSpriteFull(4, // sprite ID ( 0 to 79 )
      120,   // X in screen coords
      40,   // Y in screen coords
      SPRITE_SIZE(2,2), // 1x1 to up to 4x4
      TILE_ATTR_FULL(PAL3,    // PALette 
        1,  // priority
        0,  // Flip Vertical
        0,  // Flip Horizontal
        numbers_ind + 28 // index
        ) ,
      5
      );

  // tell VDP to draw sprites
  s16 x = 160;
  s16 y = 100;
  s16 shipframe_offset = 0;
  s16 shipframe_dir = 1;
  VDP_setSpriteFull(5, // sprite ID ( 0 to 79 )
      x,   // X in screen coords
      y,   // Y in screen coords
      SPRITE_SIZE(4,4), // 1x1 to up to 4x4
      TILE_ATTR_FULL(PAL3,    // PALette 
        1,  // priority
        0,  // Flip Vertical
        0,  // Flip Horizontal
        shipsheet_ind + shipframe_offset  // index
        ) ,
      6  // next sprite ID
      );

  VDP_setSpriteFull(6, // sprite ID ( 0 to 79 )
      x,   // X in screen coords
      y,   // Y in screen coords
      SPRITE_SIZE(4,4), // 1x1 to up to 4x4
      TILE_ATTR_FULL(PAL3,    // PALette 
        1,  // priority
        0,  // Flip Vertical
        0,  // Flip Horizontal
        shipsheet_ind + shipframe_offset  // index
        ) ,
      7  // next sprite ID
      );


  // tell VDP to draw  the 6 sprites we've defined.
  s16 currSprite = 7 ;
  //for( int i=0; i < 

  /*
  VDP_setSpriteFull(currSprite, // sprite ID ( 0 to 79 )
      160 + dino_frame_0_sprite_info[0].startX,   // X in screen coords
      20 + dino_frame_0_sprite_info[0].startY,   // Y in screen coords
      SPRITE_SIZE(dino_frame_0_sprite_info[0].width,4), // 1x1 to up to 4x4
      TILE_ATTR_FULL(PAL2,    // PALette 
        1,  // priority
        0,  // Flip Vertical
        0,  // Flip Horizontal
        dino_ind // index
        ) ,
      currSprite +1 
      );
  VDP_setSpriteFull(7, // sprite ID ( 0 to 79 )
      160 + dino_frame_0_sprite_info[1].startX,   // X in screen coords
      20 + dino_frame_0_sprite_info[1].startY,   // Y in screen coords
      SPRITE_SIZE(dino_frame_0_sprite_info[1].width,4), // 1x1 to up to 4x4
      TILE_ATTR_FULL(PAL2,    // PALette 
        1,  // priority
        0,  // Flip Vertical
        0,  // Flip Horizontal
        dino_ind + 8 // index
        ) ,
      0
      );

*/
  s16 dinoSprite = 6; 
  s16 offset = 0;
  for( s16 i=0; i < dino_frame_0_sprite_count; ++ i ){
    if( i > 0 ) {
      offset += dino_frame_0_sprite_info[i-1].tiles;
    }
    VDP_setSpriteFull(currSprite, // sprite ID ( 0 to 79 )
        160 + dino_frame_0_sprite_info[i].startX,   // X in screen coords
        20 + dino_frame_0_sprite_info[i].startY,   // Y in screen coords
        SPRITE_SIZE(dino_frame_0_sprite_info[i].width,4), // 1x1 to up to 4x4
        TILE_ATTR_FULL(PAL2,    // PALette 
          1,  // priority
          0,  // Flip Vertical
          0,  // Flip Horizontal
          dino_ind + offset // index
          ) ,
        currSprite +1 
        );
    ++currSprite;
  }

  VDP_updateSprites(currSprite, DMA_QUEUE_COPY);
  s16 dinoFrame = 0;
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

    // update position of srpite 5
    VDP_setSpritePosition(5, // sprite ID ( 0 to 79 )
        x,   // X in screen coords
        y   // Y in screen coords
        );

    // update animation frame of sprite 5 by setting tile index.
    VDP_setSpriteTile(5, // sprite ID ( 0 to 79 )
        shipsheet_ind + shipframe_offset  // tile index
        );

    // update position of srpite 5
    VDP_setSpritePosition(6, // sprite ID ( 0 to 79 )
        x,   // X in screen coords
        y + 40  // Y in screen coords
        );

    // update animation frame of sprite 5 by setting tile index.
    VDP_setSpriteTile(6, // sprite ID ( 0 to 79 )
        shipsheet_ind + shipframe_offset + 80  // tile index for BLUE ship
        );




    s16 currSprite = 7 ;
    s16 totalSprites = 6;
    s16 dinoSprite = 6; 
    s16 offset = 0;
    if ( x % 10 == 0) {
    ++dinoFrame;
    if(dinoFrame> 6) {
      dinoFrame = 0;
    }
    //dinoFrame = 2;
    switch(dinoFrame) {
      case 0:
        VDP_loadTileData( dino_frame_0, // tile data pointer
            dino_ind,    // index
            dino_frame_0_tiles,        // number of tiles to load 
            DMA_QUEUE         // transfer method
            );
        for( s16 i=0; i < dino_frame_0_sprite_count; ++ i ){
          if( i > 0 ) {
            offset += dino_frame_0_sprite_info[i-1].tiles;
          }
          VDP_setSpriteFull(currSprite, // sprite ID ( 0 to 79 )
              160 + dino_frame_0_sprite_info[i].startX,   // X in screen coords
              20 + dino_frame_0_sprite_info[i].startY,   // Y in screen coords
              SPRITE_SIZE(dino_frame_0_sprite_info[i].width,4), // 1x1 to up to 4x4
              TILE_ATTR_FULL(PAL2,    // PALette 
                1,  // priority
                0,  // Flip Vertical
                0,  // Flip Horizontal
                dino_ind + offset // index
                ) ,
               ( i < dino_frame_0_sprite_count -1 ) ? currSprite  +1 : 0
              );
          ++currSprite;
        }
        break;
      case 1:
        VDP_loadTileData( dino_frame_1, // tile data pointer
            dino_ind,    // index
            dino_frame_1_tiles,        // number of tiles to load 
            DMA_QUEUE         // transfer method
            );
        for( s16 i=0; i < dino_frame_1_sprite_count; ++ i ){
          if( i > 0 ) {
            offset += dino_frame_1_sprite_info[i-1].tiles;
          }
          VDP_setSpriteFull(currSprite, // sprite ID ( 0 to 79 )
              160 + dino_frame_1_sprite_info[i].startX,   // X in screen coords
              20 + dino_frame_1_sprite_info[i].startY,   // Y in screen coords
              SPRITE_SIZE(dino_frame_1_sprite_info[i].width,4), // 1x1 to up to 4x4
              TILE_ATTR_FULL(PAL2,    // PALette 
                1,  // priority
                0,  // Flip Vertical
                0,  // Flip Horizontal
                dino_ind + offset // index
                ) ,
               ( i < dino_frame_0_sprite_count -1 ) ? currSprite  +1 : 0
              );
          ++currSprite;
        }
        break;
      case 2:
        VDP_loadTileData( dino_frame_2, // tile data pointer
            dino_ind,    // index
            dino_frame_2_tiles,        // number of tiles to load 
            DMA_QUEUE         // transfer method
            );
        for( s16 i=0; i < dino_frame_2_sprite_count; ++ i ){
          if( i > 0 ) {
            offset += dino_frame_2_sprite_info[i-1].tiles;
          }
          VDP_setSpriteFull(currSprite, // sprite ID ( 0 to 79 )
              160 + dino_frame_2_sprite_info[i].startX,   // X in screen coords
              20 + dino_frame_2_sprite_info[i].startY,   // Y in screen coords
              SPRITE_SIZE(dino_frame_2_sprite_info[i].width,4), // 1x1 to up to 4x4
              TILE_ATTR_FULL(PAL2,    // PALette 
                1,  // priority
                0,  // Flip Vertical
                0,  // Flip Horizontal
                dino_ind + offset // index
                ) ,
               ( i < dino_frame_0_sprite_count -1 ) ? currSprite  +1 : 0
              );
          ++currSprite;
        }
        break;
      case 3:
        VDP_loadTileData( dino_frame_3, // tile data pointer
            dino_ind,    // index
            dino_frame_3_tiles,        // number of tiles to load 
            DMA_QUEUE         // transfer method
            );
        for( s16 i=0; i < dino_frame_3_sprite_count; ++ i ){
          if( i > 0 ) {
            offset += dino_frame_3_sprite_info[i-1].tiles;
          }
          VDP_setSpriteFull(currSprite, // sprite ID ( 0 to 79 )
              160 + dino_frame_3_sprite_info[i].startX,   // X in screen coords
              20 + dino_frame_3_sprite_info[i].startY,   // Y in screen coords
              SPRITE_SIZE(dino_frame_3_sprite_info[i].width,4), // 1x1 to up to 4x4
              TILE_ATTR_FULL(PAL2,    // PALette 
                1,  // priority
                0,  // Flip Vertical
                0,  // Flip Horizontal
                dino_ind + offset // index
                ) ,
               ( i < dino_frame_0_sprite_count -1 ) ? currSprite  +1 : 0
              );
          ++currSprite;
        }
        break;
      case 4:
        VDP_loadTileData( dino_frame_4, // tile data pointer
            dino_ind,    // index
            dino_frame_4_tiles,        // number of tiles to load 
            DMA_QUEUE         // transfer method
            );
        for( s16 i=0; i < dino_frame_4_sprite_count; ++ i ){
          if( i > 0 ) {
            offset += dino_frame_4_sprite_info[i-1].tiles;
          }
          VDP_setSpriteFull(currSprite, // sprite ID ( 0 to 79 )
              160 + dino_frame_4_sprite_info[i].startX,   // X in screen coords
              20 + dino_frame_4_sprite_info[i].startY,   // Y in screen coords
              SPRITE_SIZE(dino_frame_4_sprite_info[i].width,4), // 1x1 to up to 4x4
              TILE_ATTR_FULL(PAL2,    // PALette 
                1,  // priority
                0,  // Flip Vertical
                0,  // Flip Horizontal
                dino_ind + offset // index
                ) ,
               ( i < dino_frame_0_sprite_count -1 ) ? currSprite  +1 : 0
              );
          ++currSprite;
        }
        break;
      case 5:
        VDP_loadTileData( dino_frame_5, // tile data pointer
            dino_ind,    // index
            dino_frame_5_tiles,        // number of tiles to load 
            DMA_QUEUE         // transfer method
            );
        for( s16 i=0; i < dino_frame_5_sprite_count; ++ i ){
          if( i > 0 ) {
            offset += dino_frame_5_sprite_info[i-1].tiles;
          }
          VDP_setSpriteFull(currSprite, // sprite ID ( 0 to 79 )
              160 + dino_frame_5_sprite_info[i].startX,   // X in screen coords
              20 + dino_frame_5_sprite_info[i].startY,   // Y in screen coords
              SPRITE_SIZE(dino_frame_5_sprite_info[i].width,4), // 1x1 to up to 4x4
              TILE_ATTR_FULL(PAL2,    // PALette 
                1,  // priority
                0,  // Flip Vertical
                0,  // Flip Horizontal
                dino_ind + offset // index
                ) ,
               ( i < dino_frame_0_sprite_count -1 ) ? currSprite  +1 : 0
              );
          ++currSprite;
        }
        break;
      default:
        VDP_loadTileData( dino_frame_6, // tile data pointer
            dino_ind,    // index
            dino_frame_6_tiles,        // number of tiles to load 
            DMA_QUEUE         // transfer method
            );
        for( s16 i=0; i < dino_frame_6_sprite_count; ++ i ){
          if( i > 0 ) {
            offset += dino_frame_6_sprite_info[i-1].tiles;
          }
          VDP_setSpriteFull(currSprite, // sprite ID ( 0 to 79 )
              160 + dino_frame_6_sprite_info[i].startX,   // X in screen coords
              20 + dino_frame_6_sprite_info[i].startY,   // Y in screen coords
              SPRITE_SIZE(dino_frame_6_sprite_info[i].width,4), // 1x1 to up to 4x4
              TILE_ATTR_FULL(PAL2,    // PALette 
                1,  // priority
                0,  // Flip Vertical
                0,  // Flip Horizontal
                dino_ind + offset // index
                ) ,
               ( i < dino_frame_0_sprite_count -1 ) ? currSprite  +1 : 0
              );
          ++currSprite;
        }
    }
    }
    // tell VDP to draw  the 6 sprites we've defined.
    VDP_updateSprites(currSprite, DMA_QUEUE_COPY);

    // do VBLankd processes
    SYS_doVBlankProcess();
  }

  return 0;
}

