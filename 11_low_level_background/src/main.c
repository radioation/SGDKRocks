#include <genesis.h>

void moveTile(char* dest, char* source, s16 startPixel, s16 lastTilePixel, s16 tiles );

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

const u32 trunk[] =
{
  // tile 0
  0x4E000000,  
  0x4E000000,  
  0x4E000000,  
  0x4E000000,  
  0x4E000000,  
  0x4E000000,  
  0x4E000000,  
  0x4E000000
};

const u32 trunkSet0[8] =
{
  // tile 0
  0x00000000,  //  0    | 1    |  2|  3
  0x00200000,  //  8  9 |10 11 |12 13 |14 15 
  0x00000000,  // 16 17 |18 19 |20 21 |22 23
  0x20002000,
  0x00000000,
  0x00300000,
  0x00000000,
  0x20002000
};

const u32 trunkSet3[16] =
{
  // tile 0
  0x00000000,
  0x20202020,
  0x02000200,
  0x20205020,
  0x00000000,
  0x40305020,
  0x02000400,
  0x20205020,
  // tile 1
  0x00000000,
  0x70000000,
  0x04000000,
  0x60000000,
  0x00000000,
  0x70000000,
  0x04000000,
  0x20000000
};

const u32 trunkSet10[24] =
{
  // tile 0
  0x24254252,
  0x44222252,
  0x24232222,
  0x44232252,
  0x24222255,
  0x44222255,
  0x24254225,
  0x44224222,
  // tile 1
  0x42224244,
  0x22224242,
  0x42254222,
  0x22242242,
  0x42252242,
  0x42224242,
  0x42224246,
  0x22222544,
  // tile 2
  0x44000000,
  0x47000000,
  0x77000000,
  0x47000000,
  0x77000000,
  0x44000000,
  0x67000000,
  0x47000000
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
  0x11111111};

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
  0x00000000};

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
  u32 rowTiles[320];
  memset(rowTiles, 0, sizeof(rowTiles));
  memset(rowTiles, 0x0E, 32);             // set the first tile to 0x0E
  VDP_loadTileData((const u32 *)rowTiles, // tile data pointer
      rowIndex,              // index
      40,                    // number of tiles to load
      DMA_QUEUE              // transfer method
      );

  // set 20 rows to use the tiles we loaded into VRAM
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
  // Load the tileSet0 into VRAM with VDP_loadTileData
  // s16 tileSet0Index = rowIndex + 40;
  // VDP_loadTileData( (const u32 *)tileSet0, // tile data pointer
  //    tileSet0Index,   // index
  //    9,   // number of tiles to load
  //    DMA_QUEUE    // transfer method
  //    );
  s16 colIndex = rowIndex + 40;
  u32 colTiles[72];
  memset(colTiles, 0, sizeof(colTiles));
  memcpy(colTiles, tileSet0, sizeof(colTiles)); // copy the column data into ram
  VDP_loadTileData((const u32 *)colTiles,       // tile data pointer
      colIndex,                    // index
      9,                           // number of tiles to load
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
        9                         // height
        );
  }

  s16 offset = 1;
  s16 colSet = 0;
	s16 delay = 0;
  while (1)
  {
    // update the row data
    //memset(rowTiles + offset, 0x0, 32);





    /*
       offset++;
       if( offset > 322 ) {
       offset=0;
       }
       memset(rowTiles, 0x00, 354);
       memset((char*)rowTiles + offset, 0xFF, 1);

*/



    // u32 rowTiles[320]; << rowtiles is not 8 bits.
    /*
       memset(rowTiles, 0x00, 64);
       memset((char*)rowTiles + 0, 0xFF, 1);
       memset((char*)rowTiles + 1, 0x0F, 1);
       memset((char*)rowTiles + 2, 0x11, 1);
       memset((char*)rowTiles + 3, 0x01, 1);
       memset((char*)rowTiles + 4, 0x30, 1);
			 memset((char*)rowTiles + 5, 0x03, 1);
			 memset((char*)rowTiles + 6, 0x50, 1);
			 memset((char*)rowTiles + 7, 0x05, 1);
			 memset((char*)rowTiles + 8, 0x05, 1);
		//memset((char*)rowTiles + 16, 0x05, 1);
		//memset((char*)rowTiles + 160, 0x05, 1);
		*/

  	memset(rowTiles, 0, sizeof(rowTiles));
       ++offset	;
       if( offset > 290) {
	       offset = 0;
       }
       //s16 startPixel = 10;
       s16 startPixel = offset;

       s16 tiles = 3;
       s16 lastTilePixel = 1;

       moveTile( &rowTiles[0], &tile[0],  startPixel, lastTilePixel, tiles );



       /*
       // GABBA
       // 
       memset(rowTiles, 0x00, 128);
       s16 tiles = 3;
       s16 tileWidth = 1;
       s16 lastTilePixel = 1;
       ++delay;
       if ( delay ==1 ) {
       delay = 0;
       ++offset	;
       if( offset > 3) {
       offset = 0;
       }

       }
       if( offset == 0 ) {
       for( int t=0;t<tiles;++t) {
       char* dst = &rowTiles[0];
       dst = dst + (t * 32);
       char* src = &tile[0];
       src = src + (t * 32);
       // if last tile pixle is <=1, copy 1 column (byte)
       // if last tile pixle is <=3, copy 2 column (byte)
       // if last tile pixle is <=5, copy 4 column (byte)
       // last tile pixle <= 7

       if( t == tiles-1 && lastTilePixel <= 1 ) {
       // copy 1 set  for all 8
       char* dstPtr = dst;//&rowTiles[0];
       char* srcPtr = src;//&tile[0];
	*(dstPtr) =  *(srcPtr);
	*(dstPtr  + 4) =  *(srcPtr + 4);
	*(dstPtr  + 8) =  *(srcPtr + 8);
	*(dstPtr  + 12) = *(srcPtr + 12);
	*(dstPtr  + 16) = *(srcPtr + 16);
	*(dstPtr  + 20) = *(srcPtr + 20);
	*(dstPtr  + 24) = *(srcPtr + 24);
	*(dstPtr  + 28) = *(srcPtr + 28);
	} else if( t == tiles-1 && lastTilePixel <= 3 ) {
	short* dstPtr = dst;//&rowTiles[0];
	short* srcPtr = src;//&tile[0];
	*(dstPtr) =  *(srcPtr);
	*(dstPtr  + 2) =  *(srcPtr + 2);
	*(dstPtr  + 4) =  *(srcPtr + 4);
	*(dstPtr  + 6) =  *(srcPtr + 6);
	*(dstPtr  + 8) =  *(srcPtr + 8);
	*(dstPtr  + 10) = *(srcPtr + 10);
	*(dstPtr  + 12) = *(srcPtr + 12);
	*(dstPtr  + 14) = *(srcPtr + 14);
	} else if( t == tiles-1 && lastTilePixel <= 5 ) {

	short* dstPtr = dst;//&rowTiles[0];
	short* srcPtr = src;//&tile[0];
	*(dstPtr) =  *(srcPtr);
	*(dstPtr  + 2) =  *(srcPtr + 2);
	*(dstPtr  + 4) =  *(srcPtr + 4);
	*(dstPtr  + 6) =  *(srcPtr + 6);
	*(dstPtr  + 8) =  *(srcPtr + 8);
	*(dstPtr  + 10) = *(srcPtr + 10);
	*(dstPtr  + 12) = *(srcPtr + 12);
	*(dstPtr  + 14) = *(srcPtr + 14);

	char* dstPtrC = dst + 2;// ((char*)&rowTiles[0])+2;
	char* srcPtrC = src + 2;//((char*)&tile[0])+2;
	*(dstPtrC ) =     *(srcPtrC );
	*(dstPtrC + 4) =  *(srcPtrC + 4);
	*(dstPtrC + 8) =  *(srcPtrC + 8);
	*(dstPtrC + 12) = *(srcPtrC + 12);
	*(dstPtrC + 16) = *(srcPtrC + 16);
	*(dstPtrC + 20) = *(srcPtrC + 20);
       *(dstPtrC + 24) = *(srcPtrC + 24);
       *(dstPtrC + 28) = *(srcPtrC + 28);

  } else if ( lastTilePixel <= 7 ) {
	  //memcpy( rowTiles,  trunkSet0, 32 ); // copy the whole thing
	  memcpy( dst, src, 32 ); // copy the whole thing
  }
  }
  }else if( offset == 1 ) {

	  // if last tile pixle is <=1, copy 1 column (byte)
	  // if last tile pixle is <=3, copy 2 column (byte)
	  // if last tile pixle is <=5, copy 4 column (byte)
	  // last tile pixle <= 7
	  for( int t=0;t<tiles;++t) {
		  char* dst = &rowTiles[0];
		  dst = dst + (t * 32);
		  char* src = &tile[0];
		  src = src + (t * 32);
		  if(  t == tiles-1 && lastTilePixel <= 1 ) {
			  // copy 1 set  for all 8
			  char* dstPtr = dst+offset;//(char*)(&rowTiles[0])+offset;
			  char* srcPtr = src;//(char*)(&tile[0]);

			  *(dstPtr) =  *(srcPtr);
			  *(dstPtr  + 4) =  *(srcPtr + 4);
			  *(dstPtr  + 8) =  *(srcPtr + 8);
			  *(dstPtr  + 12) = *(srcPtr + 12);
			  *(dstPtr  + 16) = *(srcPtr + 16);
			  *(dstPtr  + 20) = *(srcPtr + 20);
			  *(dstPtr  + 24) = *(srcPtr + 24);
			  *(dstPtr  + 28) = *(srcPtr + 28);
		  } else if( t == tiles-1 &&  lastTilePixel <= 3 ) {
			  //offset of <=1 and last pixel <=3 has no real problem with destination 
			  //short* dstPtr = ((char*)(&rowTiles[0])+1);
			  //short* srcPtr = ((char*)(&tile[0])+1);
			  char* dstPtr = dst+offset;//(char*)(&rowTiles[0])+offset;
			  char* srcPtr = src;//(char*)(&tile[0]);
			  *(dstPtr) =  *(srcPtr);
			  *(dstPtr  + 4) =  *(srcPtr + 4);
			  *(dstPtr  + 8) =  *(srcPtr + 8);
			  *(dstPtr  + 12) = *(srcPtr + 12);
			  *(dstPtr  + 16) = *(srcPtr + 16);
			  *(dstPtr  + 20) = *(srcPtr + 20);
			  *(dstPtr  + 24) = *(srcPtr + 24);
			  *(dstPtr  + 28) = *(srcPtr + 28);
			  ++dstPtr;
			  ++srcPtr;
			  *(dstPtr) =  *(srcPtr);
			  *(dstPtr  + 4) =  *(srcPtr + 4);
			  *(dstPtr  + 8) =  *(srcPtr + 8);
			  *(dstPtr  + 12) = *(srcPtr + 12);
			  *(dstPtr  + 16) = *(srcPtr + 16);
			  *(dstPtr  + 20) = *(srcPtr + 20);
			  *(dstPtr  + 24) = *(srcPtr + 24);
			  *(dstPtr  + 28) = *(srcPtr + 28);
		  } else if( t == tiles-1 &&  lastTilePixel <= 5 ) {
			  //offset of <=1 and last pixel <=5 has no real problem with destination 
			  char* dstPtr = dst+offset;// (char*)(&rowTiles[0])+offset;
			  char* srcPtr = src;//(char*)(&tile[0]);
			  *(dstPtr) =  *(srcPtr);
			  *(dstPtr  + 4) =  *(srcPtr + 4);
			  *(dstPtr  + 8) =  *(srcPtr + 8);
			  *(dstPtr  + 12) = *(srcPtr + 12);
			  *(dstPtr  + 16) = *(srcPtr + 16);
			  *(dstPtr  + 20) = *(srcPtr + 20);
			  *(dstPtr  + 24) = *(srcPtr + 24);
			  *(dstPtr  + 28) = *(srcPtr + 28);
			  ++dstPtr;
			  ++srcPtr;
			  *(dstPtr) =  *(srcPtr);
			  *(dstPtr  + 4) =  *(srcPtr + 4);
			  *(dstPtr  + 8) =  *(srcPtr + 8);
			  *(dstPtr  + 12) = *(srcPtr + 12);
			  *(dstPtr  + 16) = *(srcPtr + 16);
			  *(dstPtr  + 20) = *(srcPtr + 20);
			  *(dstPtr  + 24) = *(srcPtr + 24);
			  *(dstPtr  + 28) = *(srcPtr + 28);
			  ++dstPtr;
			  ++srcPtr;
			  *(dstPtr) =  *(srcPtr);
			  *(dstPtr  + 4) =  *(srcPtr + 4);
			  *(dstPtr  + 8) =  *(srcPtr + 8);
			  *(dstPtr  + 12) = *(srcPtr + 12);
			  *(dstPtr  + 16) = *(srcPtr + 16);
			  *(dstPtr  + 20) = *(srcPtr + 20);
			  *(dstPtr  + 24) = *(srcPtr + 24);
			  *(dstPtr  + 28) = *(srcPtr + 28);

		  } else if ( lastTilePixel <= 7 ) {
			  //offset of <=1 and last pixel <=8 must blead over to rest of account 
			  char* dstPtr = dst+offset;//(char*)(&rowTiles[0])+offset;
			  char* srcPtr = src;//(char*)(&tile[0]);
			  *(dstPtr) =  *(srcPtr);
			  *(dstPtr  + 4) =  *(srcPtr + 4);
			  *(dstPtr  + 8) =  *(srcPtr + 8);
			  *(dstPtr  + 12) = *(srcPtr + 12);
			  *(dstPtr  + 16) = *(srcPtr + 16);
			  *(dstPtr  + 20) = *(srcPtr + 20);
			  *(dstPtr  + 24) = *(srcPtr + 24);
			  *(dstPtr  + 28) = *(srcPtr + 28);
			  ++dstPtr;
			  ++srcPtr;
			  *(dstPtr) =  *(srcPtr);
			  *(dstPtr  + 4) =  *(srcPtr + 4);
			  *(dstPtr  + 8) =  *(srcPtr + 8);
			  *(dstPtr  + 12) = *(srcPtr + 12);
			  *(dstPtr  + 16) = *(srcPtr + 16);
			  *(dstPtr  + 20) = *(srcPtr + 20);
			  *(dstPtr  + 24) = *(srcPtr + 24);
			  *(dstPtr  + 28) = *(srcPtr + 28);
			  ++dstPtr;
			  ++srcPtr;
			  *(dstPtr) =  *(srcPtr);
			  *(dstPtr  + 4) =  *(srcPtr + 4);
			  *(dstPtr  + 8) =  *(srcPtr + 8);
			  *(dstPtr  + 12) = *(srcPtr + 12);
			  *(dstPtr  + 16) = *(srcPtr + 16);
			  *(dstPtr  + 20) = *(srcPtr + 20);
			  *(dstPtr  + 24) = *(srcPtr + 24);
			  *(dstPtr  + 28) = *(srcPtr + 28);

			  // bleed over to next tile 
			  char* dstPtrC = dst + 32;
			  ++srcPtr;
			  *(dstPtrC ) =     *(srcPtr );
			  *(dstPtrC + 4) =  *(srcPtr + 4);
			  *(dstPtrC + 8) =  *(srcPtr + 8);
			  *(dstPtrC + 12) = *(srcPtr + 12);
			  *(dstPtrC + 16) = *(srcPtr + 16);
			  *(dstPtrC + 20) = *(srcPtr + 20);
			  *(dstPtrC + 24) = *(srcPtr + 24);
			  *(dstPtrC + 28) = *(srcPtr + 28);


		  }
	  }
  }else if( offset == 2 ) {
	  // if last tile pixle is <=1, copy 1 column (byte)
	  // if last tile pixle is <=3, copy 2 column (byte)
	  // if last tile pixle is <=5, copy 4 column (byte)
	  // last tile pixle <= 7
	  for( int t=0;t<tiles;++t) {
		  char* dst = &rowTiles[0];
		  dst = dst + (t * 32);
		  char* src = &tile[0];
		  src = src + (t * 32);
		  if(t == tiles -1 &&  lastTilePixel <= 1 ) {
			  // copy 1 set  for all 8
			  char* dstPtr = dst+offset;
			  char* srcPtr = src;

			  *(dstPtr) =  *(srcPtr);
			  *(dstPtr  + 4) =  *(srcPtr + 4);
			  *(dstPtr  + 8) =  *(srcPtr + 8);
			  *(dstPtr  + 12) = *(srcPtr + 12);
			  *(dstPtr  + 16) = *(srcPtr + 16);
			  *(dstPtr  + 20) = *(srcPtr + 20);
			  *(dstPtr  + 24) = *(srcPtr + 24);
			  *(dstPtr  + 28) = *(srcPtr + 28);
		  } else if(t == tiles -1 &&  lastTilePixel <= 3 ) {
			  //offset of <=1 and last pixel <=3 has no real problem with destination 
			  //short* dstPtr = ((char*)(&rowTiles[0])+1);
			  //short* srcPtr = ((char*)(&tile[0])+1);
			  char* dstPtr = dst+offset;
			  char* srcPtr = src;
			  *(dstPtr) =  *(srcPtr);
			  *(dstPtr  + 4) =  *(srcPtr + 4);
			  *(dstPtr  + 8) =  *(srcPtr + 8);
			  *(dstPtr  + 12) = *(srcPtr + 12);
			  *(dstPtr  + 16) = *(srcPtr + 16);
			  *(dstPtr  + 20) = *(srcPtr + 20);
			  *(dstPtr  + 24) = *(srcPtr + 24);
			  *(dstPtr  + 28) = *(srcPtr + 28);
			  ++dstPtr;
			  ++srcPtr;
			  *(dstPtr) =  *(srcPtr);
			  *(dstPtr  + 4) =  *(srcPtr + 4);
			  *(dstPtr  + 8) =  *(srcPtr + 8);
			  *(dstPtr  + 12) = *(srcPtr + 12);
			  *(dstPtr  + 16) = *(srcPtr + 16);
			  *(dstPtr  + 20) = *(srcPtr + 20);
			  *(dstPtr  + 24) = *(srcPtr + 24);
			  *(dstPtr  + 28) = *(srcPtr + 28);
		  } else if( t == tiles -1 && lastTilePixel <= 5 ) {
			  //offset of <=1 and last pixel <=5 has no real problem with destination 
			  char* dstPtr = dst+offset;
			  char* srcPtr = src;
			  *(dstPtr) =  *(srcPtr);
			  *(dstPtr  + 4) =  *(srcPtr + 4);
			  *(dstPtr  + 8) =  *(srcPtr + 8);
			  *(dstPtr  + 12) = *(srcPtr + 12);
			  *(dstPtr  + 16) = *(srcPtr + 16);
			  *(dstPtr  + 20) = *(srcPtr + 20);
			  *(dstPtr  + 24) = *(srcPtr + 24);
			  *(dstPtr  + 28) = *(srcPtr + 28);
			  ++dstPtr;
			  ++srcPtr;
			  *(dstPtr) =  *(srcPtr);
			  *(dstPtr  + 4) =  *(srcPtr + 4);
			  *(dstPtr  + 8) =  *(srcPtr + 8);
			  *(dstPtr  + 12) = *(srcPtr + 12);
			  *(dstPtr  + 16) = *(srcPtr + 16);
			  *(dstPtr  + 20) = *(srcPtr + 20);
			  *(dstPtr  + 24) = *(srcPtr + 24);
			  *(dstPtr  + 28) = *(srcPtr + 28);

			  // bleed over to next tile 
			  char* dstPtrC = dst + 32;
			  ++srcPtr;

			  *(dstPtrC) =  *(srcPtr);
			  *(dstPtrC  + 4) =  *(srcPtr + 4);
			  *(dstPtrC  + 8) =  *(srcPtr + 8);
			  *(dstPtrC  + 12) = *(srcPtr + 12);
			  *(dstPtrC  + 16) = *(srcPtr + 16);
			  *(dstPtrC  + 20) = *(srcPtr + 20);
			  *(dstPtrC  + 24) = *(srcPtr + 24);
			  *(dstPtrC  + 28) = *(srcPtr + 28);

		  } else if ( lastTilePixel <= 7 ) {
			  //offset of <=1 and last pixel <=8 must blead over to rest of account 
			  char* dstPtr = dst+offset;
			  char* srcPtr = src;
			  *(dstPtr) =  *(srcPtr);
			  *(dstPtr  + 4) =  *(srcPtr + 4);
			  *(dstPtr  + 8) =  *(srcPtr + 8);
			  *(dstPtr  + 12) = *(srcPtr + 12);
			  *(dstPtr  + 16) = *(srcPtr + 16);
			  *(dstPtr  + 20) = *(srcPtr + 20);
			  *(dstPtr  + 24) = *(srcPtr + 24);
			  *(dstPtr  + 28) = *(srcPtr + 28);
			  ++dstPtr;
			  ++srcPtr;
			  *(dstPtr) =  *(srcPtr);
			  *(dstPtr  + 4) =  *(srcPtr + 4);
			  *(dstPtr  + 8) =  *(srcPtr + 8);
			  *(dstPtr  + 12) = *(srcPtr + 12);
			  *(dstPtr  + 16) = *(srcPtr + 16);
			  *(dstPtr  + 20) = *(srcPtr + 20);
			  *(dstPtr  + 24) = *(srcPtr + 24);
			  *(dstPtr  + 28) = *(srcPtr + 28);
			  char* dstPtrC = dst + 32;
			  ++srcPtr;
			  *(dstPtrC) =  *(srcPtr);
			  *(dstPtrC  + 4) =  *(srcPtr + 4);
			  *(dstPtrC  + 8) =  *(srcPtr + 8);
			  *(dstPtrC  + 12) = *(srcPtr + 12);
			  *(dstPtrC  + 16) = *(srcPtr + 16);
			  *(dstPtrC  + 20) = *(srcPtr + 20);
			  *(dstPtrC  + 24) = *(srcPtr + 24);
			  *(dstPtrC  + 28) = *(srcPtr + 28);

			  // bleed over to next tile 
			  ++dstPtrC;
			  ++srcPtr;
			  *(dstPtrC ) =     *(srcPtr );
			  *(dstPtrC + 4) =  *(srcPtr + 4);
			  *(dstPtrC + 8) =  *(srcPtr + 8);
			  *(dstPtrC + 12) = *(srcPtr + 12);
			  *(dstPtrC + 16) = *(srcPtr + 16);
			  *(dstPtrC + 20) = *(srcPtr + 20);
			  *(dstPtrC + 24) = *(srcPtr + 24);
			  *(dstPtrC + 28) = *(srcPtr + 28);

		  }
	  }
  }else if( offset == 3 ) {
	  for( int t=0;t<tiles;++t) {
		  char* dst = &rowTiles[0];
		  dst = dst + (t * 32);
		  char* src = &tile[0];
		  src = src + (t * 32);
		  // if last tile pixle is <=1, copy 1 column (byte)
		  // if last tile pixle is <=3, copy 2 column (byte)
		  // if last tile pixle is <=5, copy 4 column (byte)
		  // last tile pixle <= 7
		  if( t == tiles-1 && lastTilePixel <= 1 ) {
			  // copy 1 set  for all 8
			  char* dstPtr = dst+offset;//&rowTiles[0];
			  char* srcPtr = src;//&tile[0];

			  *(dstPtr) =  *(srcPtr);
			  *(dstPtr  + 4) =  *(srcPtr + 4);
			  *(dstPtr  + 8) =  *(srcPtr + 8);
			  *(dstPtr  + 12) = *(srcPtr + 12);
			  *(dstPtr  + 16) = *(srcPtr + 16);
			  *(dstPtr  + 20) = *(srcPtr + 20);
			  *(dstPtr  + 24) = *(srcPtr + 24);
			  *(dstPtr  + 28) = *(srcPtr + 28);
		  } else if( t == tiles-1 && lastTilePixel <= 3 ) {
			  //offset of <=1 and last pixel <=3 has no real problem with destination 
			  char* dstPtr = dst+offset;
			  char* srcPtr = src;
			  *(dstPtr) =  *(srcPtr);
			  *(dstPtr  + 4) =  *(srcPtr + 4);
			  *(dstPtr  + 8) =  *(srcPtr + 8);
			  *(dstPtr  + 12) = *(srcPtr + 12);
			  *(dstPtr  + 16) = *(srcPtr + 16);
			  *(dstPtr  + 20) = *(srcPtr + 20);
			  *(dstPtr  + 24) = *(srcPtr + 24);
			  *(dstPtr  + 28) = *(srcPtr + 28);
			  char* dstPtrC = dst + 32;
			  ++srcPtr;
			  *(dstPtrC) =  *(srcPtr);
			  *(dstPtrC  + 4) =  *(srcPtr + 4);
			  *(dstPtrC  + 8) =  *(srcPtr + 8);
			  *(dstPtrC  + 12) = *(srcPtr + 12);
			  *(dstPtrC  + 16) = *(srcPtr + 16);
			  *(dstPtrC  + 20) = *(srcPtr + 20);
			  *(dstPtrC  + 24) = *(srcPtr + 24);
			  *(dstPtrC  + 28) = *(srcPtr + 28);
		  } else if( t == tiles -1 && lastTilePixel <= 5 ) {
			  //offset of <=1 and last pixel <=5 has no real problem with destination 
			  char* dstPtr = dst+offset;
			  char* srcPtr = src;
			  *(dstPtr) =  *(srcPtr);
			  *(dstPtr  + 4) =  *(srcPtr + 4);
			  *(dstPtr  + 8) =  *(srcPtr + 8);
			  *(dstPtr  + 12) = *(srcPtr + 12);
			  *(dstPtr  + 16) = *(srcPtr + 16);
			  *(dstPtr  + 20) = *(srcPtr + 20);
			  *(dstPtr  + 24) = *(srcPtr + 24);
			  *(dstPtr  + 28) = *(srcPtr + 28);
			  char* dstPtrC = dst + 32;
			  ++srcPtr;
			  *(dstPtrC) =  *(srcPtr);
			  *(dstPtrC  + 4) =  *(srcPtr + 4);
			  *(dstPtrC  + 8) =  *(srcPtr + 8);
			  *(dstPtrC  + 12) = *(srcPtr + 12);
			  *(dstPtrC  + 16) = *(srcPtr + 16);
			  *(dstPtrC  + 20) = *(srcPtr + 20);
			  *(dstPtrC  + 24) = *(srcPtr + 24);
			  *(dstPtrC  + 28) = *(srcPtr + 28);

			  // bleed over to next tile 
			  ++dstPtrC;
			  ++srcPtr;

			  *(dstPtrC) =  *(srcPtr);
			  *(dstPtrC  + 4) =  *(srcPtr + 4);
			  *(dstPtrC  + 8) =  *(srcPtr + 8);
			  *(dstPtrC  + 12) = *(srcPtr + 12);
			  *(dstPtrC  + 16) = *(srcPtr + 16);
			  *(dstPtrC  + 20) = *(srcPtr + 20);
			  *(dstPtrC  + 24) = *(srcPtr + 24);
			  *(dstPtrC  + 28) = *(srcPtr + 28);

		  } else if (  lastTilePixel <= 7 ) {
			  //offset of <=1 and last pixel <=8 must blead over to rest of account 
			  char* dstPtr = dst+offset;
			  char* srcPtr = src;
			  *(dstPtr) =  *(srcPtr);
			  *(dstPtr  + 4) =  *(srcPtr + 4);
			  *(dstPtr  + 8) =  *(srcPtr + 8);
			  *(dstPtr  + 12) = *(srcPtr + 12);
			  *(dstPtr  + 16) = *(srcPtr + 16);
			  *(dstPtr  + 20) = *(srcPtr + 20);
			  *(dstPtr  + 24) = *(srcPtr + 24);
			  *(dstPtr  + 28) = *(srcPtr + 28);
			  char* dstPtrC = dst + 32;
			  ++srcPtr;
			  *(dstPtrC) =  *(srcPtr);
			  *(dstPtrC  + 4) =  *(srcPtr + 4);
			  *(dstPtrC  + 8) =  *(srcPtr + 8);
			  *(dstPtrC  + 12) = *(srcPtr + 12);
			  *(dstPtrC  + 16) = *(srcPtr + 16);
			  *(dstPtrC  + 20) = *(srcPtr + 20);
			  *(dstPtrC  + 24) = *(srcPtr + 24);
			  *(dstPtrC  + 28) = *(srcPtr + 28);
			  ++dstPtrC;
			  ++srcPtr;
			  *(dstPtrC) =  *(srcPtr);
			  *(dstPtrC  + 4) =  *(srcPtr + 4);
			  *(dstPtrC  + 8) =  *(srcPtr + 8);
			  *(dstPtrC  + 12) = *(srcPtr + 12);
			  *(dstPtrC  + 16) = *(srcPtr + 16);
			  *(dstPtrC  + 20) = *(srcPtr + 20);
			  *(dstPtrC  + 24) = *(srcPtr + 24);
			  *(dstPtrC  + 28) = *(srcPtr + 28);

			  // bleed over to next tile 
			  ++dstPtrC;
			  ++srcPtr;
			  *(dstPtrC ) =     *(srcPtr );
			  *(dstPtrC + 4) =  *(srcPtr + 4);
			  *(dstPtrC + 8) =  *(srcPtr + 8);
			  *(dstPtrC + 12) = *(srcPtr + 12);
			  *(dstPtrC + 16) = *(srcPtr + 16);
			  *(dstPtrC + 20) = *(srcPtr + 20);
			  *(dstPtrC + 24) = *(srcPtr + 24);
			  *(dstPtrC + 28) = *(srcPtr + 28);
		  }

	  }
  }

  */
	  //memset((char*)rowTiles + 32, 0xFF, 32);

	  // load into VRAM
	  VDP_loadTileData((const u32 *)rowTiles, // tile data pointer
			  rowIndex,              // index
			  40,                    // number of tiles to load
			  DMA_QUEUE              // transfer method
			  );

  // update the col data
  if (colSet == 1)
  {
	  memcpy(colTiles, tileSet1, sizeof(colTiles));
  }
  else
  {
	  memcpy(colTiles, tileSet0, sizeof(colTiles));
  }
  colSet += 1;
  if (colSet > 1)
  {
	  colSet = 0;
  }
  VDP_loadTileData((const u32 *)colTiles, // tile data pointer
		  colIndex,              // index
		  9,                     // number of tiles to load
		  DMA_QUEUE              // transfer method
		  );

  // let SGDK do its thing.
  VDP_waitVSync();
  SYS_doVBlankProcess();
  }

  return 0;
}
