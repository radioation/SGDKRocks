#include <genesis.h>

/*
 * dest is start of output buffer in bytes
 * source is start of input buffer in bytes
 *
 * tiles = 1, is the number of 8-pixel width tiles to move
 *
 * lastTilePixel : columns are two pixes wide, so 0,1 will copy first two pixles
 *    of tile
 *    2,3 will copy first 4 pixles
 *    5,6 will copy first 6 pixles
 *    7,8 will copy all 8 pixels of a tile
 *
 *
 * */

void drawAt(char* dest, char* source, s16 startPixel, s16 lastTilePixel, s16 tiles )
{ 
  s16 startTile = startPixel >> 3; // divide by 8
  s16 startByte = startTile << 5;  
  s16 offsetInTile = (startPixel - (startTile << 3 )) >> 1;// we can only do 2 columns at at time anyway
  KLog_S3( "startTile:", startTile, " startByte:", startByte, " offsetInTile:", offsetInTile ); 
  
  if( offsetInTile == 0 ) {
    for( int t=0;t<tiles;++t) {
      char* dst = dest + startByte; //&rowTiles[0];
      dst = dst + (t * 32); // go to next tile
      char* src = source;
      src = src + (t * 32);
      // if last tile pixle is <=1, copy 1 column (byte)
      // if last tile pixle is <=3, copy 2 column (byte)
      // if last tile pixle is <=5, copy 4 column (byte)
      // last tile pixle <= 7
      if( t == tiles-1 && lastTilePixel <= 1 ) {
        // copy 1 set  for all 8
        char* dstPtr = dst;
        char* srcPtr = src;
        *(dstPtr) =  *(srcPtr);
        *(dstPtr  + 4) =  *(srcPtr + 4);
        *(dstPtr  + 8) =  *(srcPtr + 8);
        *(dstPtr  + 12) = *(srcPtr + 12);
        *(dstPtr  + 16) = *(srcPtr + 16);
        *(dstPtr  + 20) = *(srcPtr + 20);
        *(dstPtr  + 24) = *(srcPtr + 24);
        *(dstPtr  + 28) = *(srcPtr + 28);
      } else if( t == tiles-1 && lastTilePixel <= 3 ) {
        short* dstPtr = dst;
        short* srcPtr = src;
        *(dstPtr) =  *(srcPtr);
        *(dstPtr  + 2) =  *(srcPtr + 2);
        *(dstPtr  + 4) =  *(srcPtr + 4);
        *(dstPtr  + 6) =  *(srcPtr + 6);
        *(dstPtr  + 8) =  *(srcPtr + 8);
        *(dstPtr  + 10) = *(srcPtr + 10);
        *(dstPtr  + 12) = *(srcPtr + 12);
        *(dstPtr  + 14) = *(srcPtr + 14);
      } else if( t == tiles-1 && lastTilePixel <= 5 ) {
        short* dstPtr = dst;
        short* srcPtr = src;
        *(dstPtr) =  *(srcPtr);
        *(dstPtr  + 2) =  *(srcPtr + 2);
        *(dstPtr  + 4) =  *(srcPtr + 4);
        *(dstPtr  + 6) =  *(srcPtr + 6);
        *(dstPtr  + 8) =  *(srcPtr + 8);
        *(dstPtr  + 10) = *(srcPtr + 10);
        *(dstPtr  + 12) = *(srcPtr + 12);
        *(dstPtr  + 14) = *(srcPtr + 14);

        char* dstPtrC = dst + 2;
        char* srcPtrC = src + 2;
        *(dstPtrC ) =     *(srcPtrC );
        *(dstPtrC + 4) =  *(srcPtrC + 4);
        *(dstPtrC + 8) =  *(srcPtrC + 8);
        *(dstPtrC + 12) = *(srcPtrC + 12);
        *(dstPtrC + 16) = *(srcPtrC + 16);
        *(dstPtrC + 20) = *(srcPtrC + 20);
        *(dstPtrC + 24) = *(srcPtrC + 24);
        *(dstPtrC + 28) = *(srcPtrC + 28);

      } else if ( lastTilePixel <= 7 ) {
        memcpy( dst, src, 32 ); // copy the whole thing
      }
    }
  }else if( offsetInTile == 1 ) {
    // if last tile pixle is <=1, copy 1 column (byte)
    // if last tile pixle is <=3, copy 2 column (byte)
    // if last tile pixle is <=5, copy 4 column (byte)
    // last tile pixle <= 7
    for( int t=0;t<tiles;++t) {
      char* dst = dest + startByte;
      dst = dst + (t * 32);
      char* src = source;
      src = src + (t * 32);
      if(  t == tiles-1 && lastTilePixel <= 1 ) {
        // copy 1 set  for all 8
        char* dstPtr = dst+offsetInTile;
        char* srcPtr = src;

        *(dstPtr) =  *(srcPtr);
        *(dstPtr  + 4) =  *(srcPtr + 4);
        *(dstPtr  + 8) =  *(srcPtr + 8);
        *(dstPtr  + 12) = *(srcPtr + 12);
        *(dstPtr  + 16) = *(srcPtr + 16);
        *(dstPtr  + 20) = *(srcPtr + 20);
        *(dstPtr  + 24) = *(srcPtr + 24);
        *(dstPtr  + 28) = *(srcPtr + 28);
      } else if( t == tiles-1 &&  lastTilePixel <= 3 ) {
        //offsetInTile of <=1 and last pixel <=3 has no real problem with destination 
        char* dstPtr = dst+offsetInTile;
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
      } else if( t == tiles-1 &&  lastTilePixel <= 5 ) {
        //offsetInTile of <=1 and last pixel <=5 has no real problem with destination 
        char* dstPtr = dst+offsetInTile;
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
        //offsetInTile of <=1 and last pixel <=8 must blead over to rest of account 
        char* dstPtr = dst+offsetInTile;
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
  }else if( offsetInTile == 2 ) {
    // if last tile pixle is <=1, copy 1 column (byte)
    // if last tile pixle is <=3, copy 2 column (byte)
    // if last tile pixle is <=5, copy 4 column (byte)
    // last tile pixle <= 7
    for( int t=0;t<tiles;++t) {
      char* dst = dest + startByte;
      dst = dst + (t * 32);
      char* src = source;
      src = src + (t * 32);
      if(t == tiles -1 &&  lastTilePixel <= 1 ) {
        // copy 1 set  for all 8
        char* dstPtr = dst+offsetInTile;
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
        //offsetInTile of <=1 and last pixel <=3 has no real problem with destination 
        char* dstPtr = dst+offsetInTile;
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
        //offsetInTile of <=1 and last pixel <=5 has no real problem with destination 
        char* dstPtr = dst+offsetInTile;
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
        //offsetInTile of <=1 and last pixel <=8 must blead over to rest of account 
        char* dstPtr = dst+offsetInTile;
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
  }else if( offsetInTile == 3 ) {
    for( int t=0;t<tiles;++t) {
      char* dst = dest + startByte;
      dst = dst + (t * 32);
      char* src = source;
      src = src + (t * 32);
      // if last tile pixle is <=1, copy 1 column (byte)
      // if last tile pixle is <=3, copy 2 column (byte)
      // if last tile pixle is <=5, copy 4 column (byte)
      // last tile pixle <= 7
      if( t == tiles-1 && lastTilePixel <= 1 ) {
        // copy 1 set  for all 8
        char* dstPtr = dst+offsetInTile;
        char* srcPtr = src;

        *(dstPtr) =  *(srcPtr);
        *(dstPtr  + 4) =  *(srcPtr + 4);
        *(dstPtr  + 8) =  *(srcPtr + 8);
        *(dstPtr  + 12) = *(srcPtr + 12);
        *(dstPtr  + 16) = *(srcPtr + 16);
        *(dstPtr  + 20) = *(srcPtr + 20);
        *(dstPtr  + 24) = *(srcPtr + 24);
        *(dstPtr  + 28) = *(srcPtr + 28);
      } else if( t == tiles-1 && lastTilePixel <= 3 ) {
        //offsetInTile of <=1 and last pixel <=3 has no real problem with destination 
        char* dstPtr = dst+offsetInTile;
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
        //offsetInTile of <=1 and last pixel <=5 has no real problem with destination 
        char* dstPtr = dst+offsetInTile;
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
        //offsetInTile of <=1 and last pixel <=8 must blead over to rest of account 
        char* dstPtr = dst+offsetInTile;
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

}
