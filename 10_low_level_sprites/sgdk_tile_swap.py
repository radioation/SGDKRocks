#!/usr/bin/env python

import os,  argparse
import numpy as np
import math
from PIL import Image, ImageDraw
import shutil
from pathlib import Path


def main(args):
  
  spriteWidthTiles = args.width
  spriteHeightTiles = args.height
  if spriteWidthTiles == 0 or spriteHeightTiles == 0:
    print("Width and height must be at least 1 tile.")
    return
  if spriteWidthTiles > 4 or spriteHeightTiles > 4:
    print("Width and height must be at most 4 tile.")
    return

  imageFilename = args.input_filename
  outputFilename = args.output_filename


  with Image.open( imageFilename ) as im:
    inputImg = im.convert('RGB')
    imageWidthPixels, imageHeightPixels = im.size
    # check width and height are multiples of 8
    if imageWidthPixels % 8 != 0 or  imageHeightPixels % 8 != 0 :
      print("Image width and height must be multiples of 8.")
      return
    imageWidthTiles = imageWidthPixels / 8
    imageHeightTiles = imageHeightPixels / 8
    spriteWidthPixels = spriteWidthTiles * 8
    spriteHeightPixels = spriteHeightTiles * 8
    if imageWidthPixels % spriteWidthPixels != 0 or imageHeightPixels % spriteHeightPixels != 0:
      print("Image width and height must be multiples of sprite width and height.")
      return


    # create a work image
    workImg = Image.new('RGB', (imageWidthPixels, imageHeightPixels))
   
    totalAnim = int(imageHeightTiles / spriteHeightTiles)
    totalFrames = int(imageWidthTiles / spriteWidthTiles)
    currOutColPixel = 0
    currOutRowPixel = 0

    # copy tiles in Sega order.
    for anim in range( 0,totalAnim ):
      startRowPixel = anim * spriteHeightPixels
      for frame in range( 0,totalFrames ):
        startColPixel = frame * spriteWidthPixels
        for tileX in range( 0, spriteWidthTiles ):
          for tileY in range( 0, spriteHeightTiles ):
            # copy it
            inputStartX = startColPixel + tileX * 8
            inputStartY = startRowPixel + tileY * 8
            inputEndX = inputStartX + 8
            inputEndY = inputStartY + 8
            #print("source %d %d %d %d " % (inputStartX, inputStartY, inputEndX, inputEndY))
            region = inputImg.crop((inputStartX, inputStartY, inputEndX, inputEndY))

            outputStartX =  currOutColPixel
            outputStartY =  currOutRowPixel
            outputEndX = outputStartX + 8
            outputEndY = outputStartY + 8

            #print("  dest %d %d %d %d " %  (outputStartX, outputStartY, outputEndX, outputEndY))
            workImg.paste(region, (outputStartX, outputStartY, outputEndX, outputEndY))
            currOutColPixel += 8
            if currOutColPixel >= imageWidthPixels:
              currOutRowPixel += 8
              currOutColPixel = 0



    outImg = workImg.quantize( palette = im )

    outImg.save( outputFilename )



# the program.
if __name__ == '__main__':
  parser = argparse.ArgumentParser( 
      description = "Swap tile positions in sprite sheets for use with `VDP_loadTileData()`",
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
      default = 'out.png',
      help = "Output filename",
      metavar = "ARG")


  args = parser.parse_args()


  main(args)
