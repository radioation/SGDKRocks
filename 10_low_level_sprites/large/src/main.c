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
  s16 currFrame = 1;
  VDP_loadTileData( dino_0 + dino_frame_info[currFrame].start, // tile data pointer
      dino_ind,    // index
      dino_frame_info[currFrame].tileCount,        // number of tiles to load 
      DMA_QUEUE         // transfer method
      );

  // Add Sprites
  s16 offset = 0;
  s16 maxSprites = 0;
  s16 numSprites = 0;
  for( s16 i=0; i < dino_frame_info[currFrame].spriteCount; ++ i ){
    if( i > 0 ) {
      offset += dino_sprite_info[ dino_frame_info[currFrame].spriteInfoIndex + i-1].tiles;
    }
    VDP_setSpriteFull(numSprites, // sprite ID ( 0 to 79 )
        100 + dino_sprite_info[ dino_frame_info[currFrame].spriteInfoIndex + i].startX,   // X in screen coords
        20 + dino_sprite_info[ dino_frame_info[currFrame].spriteInfoIndex + i].startY,   // Y in screen coords
        SPRITE_SIZE(dino_sprite_info[ dino_frame_info[currFrame].spriteInfoIndex + i].width,4), // 1x1 to up to 4x4
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
  currFrame = 0;
  s16 tick = 0;
  while(TRUE)
  {
    ++tick; 
    s16 dinoSprite = 6; 

    if ( tick % 10 == 0) {
      ++currFrame;
      if(currFrame> 6) {
        currFrame = 0;
      }
      offset = 0;
      numSprites = 0;


      VDP_loadTileData( dino_0 + dino_frame_info[currFrame].start, // tile data pointer
              dino_ind,    // index
              dino_frame_info[currFrame].tileCount,        // number of tiles to load 
              DMA_QUEUE         // transfer method
              );

      // Add Sprites
      for( s16 i=0; i < dino_frame_info[currFrame].spriteCount; ++ i ){
          if( i > 0 ) {
              offset += dino_sprite_info[ dino_frame_info[currFrame].spriteInfoIndex + i-1].tiles;
          }
          VDP_setSpriteFull(numSprites, // sprite ID ( 0 to 79 )
                  100 + dino_sprite_info[ dino_frame_info[currFrame].spriteInfoIndex + i].startX,   // X in screen coords
                  20 + dino_sprite_info[ dino_frame_info[currFrame].spriteInfoIndex + i].startY,   // Y in screen coords
                  SPRITE_SIZE(dino_sprite_info[ dino_frame_info[currFrame].spriteInfoIndex + i].width,4), // 1x1 to up to 4x4
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

