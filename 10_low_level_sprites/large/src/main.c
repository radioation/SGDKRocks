#include <genesis.h>
#include "resources.h"
#include "dino.h"



int main(bool hard)
{
  //////////////////////////////////////////////////////////
  // Setup colors
  VDP_setBackgroundColor(0);
  PAL_setPalette( PAL1, dino_pal.data, CPU );


  //////////////////////////////////////////////////////////
  // Load the tilesets
  int dino_ind = TILE_USER_INDEX;
  VDP_loadTileData( dino_frame_0, // tile data pointer
      dino_ind,    // index
      dino_frame_0_tiles,        // number of tiles to load 
      DMA_QUEUE         // transfer method
      );

  // Add Sprites
  s16 offset = 0;
  s16 maxSprites = 0;
  s16 numSprites = 0;
  for( s16 i=0; i < dino_frame_0_sprite_count; ++ i ){
    if( i > 0 ) {
      offset += dino_frame_0_sprite_info[i-1].tiles;
    }
    VDP_setSpriteFull(numSprites, // sprite ID ( 0 to 79 )
        100 + dino_frame_0_sprite_info[i].startX,   // X in screen coords
        20 + dino_frame_0_sprite_info[i].startY,   // Y in screen coords
        SPRITE_SIZE(dino_frame_0_sprite_info[i].width,4), // 1x1 to up to 4x4
        TILE_ATTR_FULL(PAL1,    // PALette 
          1,  // priority
          0,  // Flip Vertical
          0,  // Flip Horizontal
          dino_ind + offset // index
          ) ,
        numSprites +1 
        );
    ++numSprites;
  }
  maxSprites = numSprites;
  VDP_updateSprites(numSprites, DMA_QUEUE_COPY);
  s16 dinoFrame = 0;
  s16 tick = 0;
  while(TRUE)
  {
    ++tick; 
    s16 dinoSprite = 6; 
    if ( tick % 10 == 0) {
      ++dinoFrame;
      if(dinoFrame> 6) {
        dinoFrame = 0;
      }
      offset = 0;
      numSprites = 0;
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
            VDP_setSpriteFull(numSprites, // sprite ID ( 0 to 79 )
                100 + dino_frame_0_sprite_info[i].startX,   // X in screen coords
                20 + dino_frame_0_sprite_info[i].startY,   // Y in screen coords
                SPRITE_SIZE(dino_frame_0_sprite_info[i].width,4), // 1x1 to up to 4x4
                TILE_ATTR_FULL(PAL1,    // PALette 
                  1,  // priority
                  0,  // Flip Vertical
                  0,  // Flip Horizontal
                  dino_ind + offset // index
                  ) ,
                ( i < dino_frame_0_sprite_count -1 ) ? numSprites  + 1 : 0
                );
            ++numSprites;
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
            VDP_setSpriteFull(numSprites, // sprite ID ( 0 to 79 )
                100 + dino_frame_1_sprite_info[i].startX,   // X in screen coords
                20 + dino_frame_1_sprite_info[i].startY,   // Y in screen coords
                SPRITE_SIZE(dino_frame_1_sprite_info[i].width,4), // 1x1 to up to 4x4
                TILE_ATTR_FULL(PAL1,    // PALette 
                  1,  // priority
                  0,  // Flip Vertical
                  0,  // Flip Horizontal
                  dino_ind + offset // index
                  ) ,
                ( i < dino_frame_0_sprite_count -1 ) ? numSprites  +1 : 0
                );
            ++numSprites;
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
            VDP_setSpriteFull(numSprites, // sprite ID ( 0 to 79 )
                100 + dino_frame_2_sprite_info[i].startX,   // X in screen coords
                20 + dino_frame_2_sprite_info[i].startY,   // Y in screen coords
                SPRITE_SIZE(dino_frame_2_sprite_info[i].width,4), // 1x1 to up to 4x4
                TILE_ATTR_FULL(PAL1,    // PALette 
                  1,  // priority
                  0,  // Flip Vertical
                  0,  // Flip Horizontal
                  dino_ind + offset // index
                  ) ,
                ( i < dino_frame_0_sprite_count -1 ) ? numSprites  +1 : 0
                );
            ++numSprites;
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
            VDP_setSpriteFull(numSprites, // sprite ID ( 0 to 79 )
                100 + dino_frame_3_sprite_info[i].startX,   // X in screen coords
                20 + dino_frame_3_sprite_info[i].startY,   // Y in screen coords
                SPRITE_SIZE(dino_frame_3_sprite_info[i].width,4), // 1x1 to up to 4x4
                TILE_ATTR_FULL(PAL1,    // PALette 
                  1,  // priority
                  0,  // Flip Vertical
                  0,  // Flip Horizontal
                  dino_ind + offset // index
                  ) ,
                ( i < dino_frame_0_sprite_count -1 ) ? numSprites  +1 : 0
                );
            ++numSprites;
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
            VDP_setSpriteFull(numSprites, // sprite ID ( 0 to 79 )
                100 + dino_frame_4_sprite_info[i].startX,   // X in screen coords
                20 + dino_frame_4_sprite_info[i].startY,   // Y in screen coords
                SPRITE_SIZE(dino_frame_4_sprite_info[i].width,4), // 1x1 to up to 4x4
                TILE_ATTR_FULL(PAL1,    // PALette 
                  1,  // priority
                  0,  // Flip Vertical
                  0,  // Flip Horizontal
                  dino_ind + offset // index
                  ) ,
                ( i < dino_frame_0_sprite_count -1 ) ? numSprites  +1 : 0
                );
            ++numSprites;
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
            VDP_setSpriteFull(numSprites, // sprite ID ( 0 to 79 )
                100 + dino_frame_5_sprite_info[i].startX,   // X in screen coords
                20 + dino_frame_5_sprite_info[i].startY,   // Y in screen coords
                SPRITE_SIZE(dino_frame_5_sprite_info[i].width,4), // 1x1 to up to 4x4
                TILE_ATTR_FULL(PAL1,    // PALette 
                  1,  // priority
                  0,  // Flip Vertical
                  0,  // Flip Horizontal
                  dino_ind + offset // index
                  ) ,
                ( i < dino_frame_0_sprite_count -1 ) ? numSprites  +1 : 0
                );
            ++numSprites;
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
            VDP_setSpriteFull(numSprites, // sprite ID ( 0 to 79 )
                100 + dino_frame_6_sprite_info[i].startX,   // X in screen coords
                20 + dino_frame_6_sprite_info[i].startY,   // Y in screen coords
                SPRITE_SIZE(dino_frame_6_sprite_info[i].width,4), // 1x1 to up to 4x4
                TILE_ATTR_FULL(PAL1,    // PALette 
                  1,  // priority
                  0,  // Flip Vertical
                  0,  // Flip Horizontal
                  dino_ind + offset // index
                  ) ,
                ( i < dino_frame_0_sprite_count -1 ) ? numSprites  +1 : 0
                );
            ++numSprites;
          }
      }
      if( numSprites < maxSprites ) {
        for( s16 i = numSprites; i < maxSprites; ++i ) {
           VDP_setSpritePosition( i , -32, -32 );
        }
      }else if( numSprites > maxSprites ) {
        maxSprites = numSprites;
      }
      // tell VDP to draw  the 6 sprites we've defined.
      VDP_updateSprites(maxSprites, DMA_QUEUE_COPY);
    }

    // do VBLankd processes
    SYS_doVBlankProcess();
  }

  return 0;
}

