#!/usr/bin/env python




import os,  argparse
import numpy as np
import math
from PIL import Image
import shutil
from pathlib import Path


def main(args):
    
  spriteWidthTiles = args.width
  spriteHeightTiles = args.height
  if spriteWidthTiles == 0 or spriteHeightTiles == 0:
    print("Width and height must be at least 1 tile.")
    return

  chunkWidthTiles = spriteWidthTiles
  if chunkWidthTiles > 4:
      chunkWidthTiles = 4

  chunkHeightTiles = spriteHeightTiles
  if chunkHeightTiles > 4:
      chunkHeightTiles = 4

  imageFilename = args.input_filename
  outputFilename = args.output_filename


  with Image.open( imageFilename ) as im:
    #inputImg = im.convert('RGB')
    imageWidthPixels, imageHeightPixels = im.size
    px = im.load()
    #print( im.mode )
    #print( im.getcolors() )

    # check width and height are multiples of 8
    if imageWidthPixels % 8 != 0 or  imageHeightPixels % 8 != 0 :
      print("Image width and height must be multiples of 8.")
      return

    imageWidthTiles = imageWidthPixels / 8
    imageHeightTiles = imageHeightPixels / 8

    spriteWidthPixels = spriteWidthTiles * 8
    spriteHeightPixels = spriteHeightTiles * 8

    chunkWidthPixels = chunkWidthTiles * 8
    chunkHeightPixels = chunkHeightTiles * 8

    totalChunksX =  int(spriteWidthTiles / chunkWidthTiles)
    totalChunksY =  int(spriteHeightTiles / chunkHeightTiles)
    if imageWidthPixels % spriteWidthPixels != 0 or imageHeightPixels % spriteHeightPixels != 0:
      print("Image width and height must be multiples of sprite width and height.")
      return


    print ("struct sprite_info {")
    print ("        u16 width;")
    print ("        u16 tiles;")
    print ("        u16 startX;")
    print ("        u16 startY;")
    print ("};\n")

    print ("struct frame_info {")
    print ("        u16 start;")
    print ("        u16 spriteCount;")
    print ("        u16 tileCount;")
    print ("        u16 spriteInfoIndex;")
    print ("};\n")



    # create a work image
    # workImg = Image.new('RGB', (imageWidthPixels, imageHeightPixels))
    #print(imageHeightPixels, imageHeightTiles,  spriteHeightTiles)
    #print(imageWidthPixels, imageWidthTiles, spriteWidthTiles)
      
    totalAnim = int(imageHeightTiles / spriteHeightTiles)
    totalFrames = int(imageWidthTiles / spriteWidthTiles)
    #print("found %d animations with %d frames" % ( totalAnim, totalFrames ) )
    #print(" working with %d x %d chunks" % ( totalChunksX, totalChunksY ) )
    currOutColPixel = 0
    currOutRowPixel = 0
    # for each frame,
    # check tiles for each chunk, if not populated don't write it out
    # if populated write out the chunks as const u32  and 
    # keep track of x,y pixel offset of each

    # the sprite draws the tiles from top to bottom THEN left to right (ie column order)
    frameOffsets = []
    frameStart = 0
    arrayCounter = 0
    frameSpriteCounts =  []
    frameSpriteCount = 0
    frameTileCounts =  []
    frameTileCount = 0

    for anim in range( 0,totalAnim ):
      print("const u32 dino_%d [] = { " % anim )
      frameStarts = []
      spriteWidths = []
      spriteTiles = []
      spriteStartsX = []
      spriteStartsY = []
      startRowPixel = anim * spriteHeightPixels
      #print("ANIM %d -----------------" % anim )
      for frame in range( 0,totalFrames ):
        #print("const u32 dino_frame_%d [] = { " % frame )
        print("  // Frame %d -----------------" % frame )
        startColPixel = frame * spriteWidthPixels
        inputStartX = startColPixel
        inputStartY = startRowPixel
        # check every chunk in the frame
        chunkOffset = 0
        frameSpriteCount = 0
        frameTileCount = 0
        for chunkX in range( 0, totalChunksX ):
          for chunkY in range( 0, totalChunksY ): 
            #print("    chunk X %d  Y % d  -----------------" % ( chunkX, chunkY) )
            # check every pixel in the chunk.
            hasPixel = False
            chunkPixelX = inputStartX + chunkX * chunkWidthPixels
            chunkPixelY = inputStartY + chunkY * chunkHeightPixels
            #print( "chunkPixelX", chunkPixelX, "chunkPixelY", chunkPixelY)
            for x in range( chunkPixelX, chunkPixelX + chunkWidthPixels ):
              for y in range( chunkPixelY, chunkPixelY + chunkHeightPixels ):
                currentPixel  = px[ x, y ]
                if currentPixel > 0:
                  #print(x,y, currentPixel )
                  hasPixel = True
              
            if hasPixel:
              frameSpriteCount += 1
              # i can has pixel.  go through each chunk in column 
              # order and define the output
              print("// USE sprite X %d sprite Y %d  offset %d" % ( chunkX, chunkY, chunkOffset ) )
              chunkOffset += 32 * chunkWidthTiles * chunkHeightTiles
              vals = ""
              lvals = ""
              rvals = ""
              nonZero = False
              lNonZero = False
              rNonZero = False

              for currentTileX in range( 0,  chunkWidthTiles ):
                for currentTileY  in range( 0, chunkHeightTiles ):
                  if currentTileX < 2:
                    lvals += "// tileX " + str(currentTileX) +  "  tileY " + str(currentTileY) +  "\n"
                  else:
                    rvals += "// tileX " + str(currentTileX) +  "  tileY " + str(currentTileY) +  "\n"
                  for y in range(chunkPixelY + currentTileY * 8, chunkPixelY + currentTileY * 8 + 8 ):
                    #print("0x", end="")
                    if currentTileX < 2:
                      lvals += "  0x"
                    else:
                      rvals += "  0x"
                    for x in range(chunkPixelX + currentTileX * 8, chunkPixelX + currentTileX * 8 + 8 ):
                      if px[x,y] > 0 :
                        if currentTileX < 2:
                          nonZero = True
                          lNonZero = True
                        else:
                          nonZero = True
                          rNonZero = True
                      if currentTileX < 2:
                        lvals = lvals + f'{px[x ,y ]:x}'
                      else:
                        rvals = rvals + f'{px[x ,y ]:x}'

                    if currentTileX < 2:
                      lvals += ",\n"
                    else:
                      rvals += ",\n"

              if nonZero:
                print(vals)

              if lNonZero:
                print(lvals)
              if rNonZero:
                print(rvals)
                
              if lNonZero and rNonZero:
                spriteWidths.append(4)  
                spriteTiles.append(16)  
                arrayCounter+=(16*8)
                frameTileCount += 16
                spriteStartsX.append(chunkX * 32)
                spriteStartsY.append(chunkY * 32)
              elif lNonZero:
                spriteWidths.append(2)  
                spriteTiles.append(8)  
                arrayCounter+=(8*8)
                frameTileCount +=  8
                spriteStartsX.append(chunkX * 32)
                spriteStartsY.append(chunkY * 32)
              elif rNonZero:
                spriteWidths.append(2)  
                spriteTiles.append(8)  
                arrayCounter+=(8*8)
                frameTileCount +=  8
                spriteStartsX.append(chunkX * 32 + 16)
                spriteStartsY.append(chunkY * 32)
            else:
              print("// SKIPPED sprite X %d sprite Y %d  offset %d" % ( chunkX, chunkY, chunkOffset ) )


        #print( "\n};\n")
        frameStarts.append(frameStart)
        frameStart = arrayCounter 
        frameSpriteCounts.append(frameSpriteCount)
        frameTileCounts.append(frameTileCount)

      print( "\n};\n")

      print( "const struct sprite_info dino_sprite_info[] = {" )
      c = 0
      totalTiles = 0
      for s in range(0, len(spriteWidths) ):
        print( "  { %d, %d, %d, %d }" % (  spriteWidths[s], spriteTiles[s], spriteStartsX[s], spriteStartsY[s]), end="")
        totalTiles += spriteTiles[s]
        if c < len(spriteWidths) - 1:
          print(",")
        c = c + 1
      print( "\n};\n")

      print( "const struct frame_info dino_frame_info[] = {" )
      c = 0
      totalTiles = 0
      spriteIndex = 0
      for s in range(0, len(frameStarts) ):
        print( "  { %d, %d, %d, %d }" % (  frameStarts[s], frameSpriteCounts[s], frameTileCounts[s], spriteIndex  ) , end="")
        spriteIndex += frameSpriteCounts[s]
        if c < len(frameStarts) - 1:
          print(",")
        c = c + 1
      print( "\n};\n")


      #print("u16 dino_frame_%d_tiles = %d;\n\n" % (frame, totalTiles ))
      #print("u16 dino_frame_%d_sprite_count = %d;\n\n" % (frame, len(spriteWidths) ))





# the program.
if __name__ == '__main__':
  parser = argparse.ArgumentParser( 
      description = "Swap tile positions in sprite sheets for use with `VDP_loadTileData()`.  Example:  python3 sgdk_tile_c2.py   -W 12 -H 20 -i dino2.png",
      epilog = "As an alternative to the commandline, params can be placed in a file, one per line, and specified on the commandline like '%(prog)s @params.conf'.",
      fromfile_prefix_chars = '@' )

  # parameter list
  parser.add_argument( "-W",
      "--width",
      default = 2,
      type=int,
      help = "Sprite Width in tiles",
      metavar = "ARG")

  parser.add_argument( "-H",
      "--height",
      default = 2,
      type=int,
      help = "Sprite height in tiles",
      metavar = "ARG")

  parser.add_argument( "-i",
      "--input_filename",
      default = 'image.png',
      help = "input image filename",
      metavar = "ARG")


  parser.add_argument( "-o",
      "--output_filename",
      default = 'out.h',
      help = "Output filename",
      metavar = "ARG")


  args = parser.parse_args()


  main(args)
