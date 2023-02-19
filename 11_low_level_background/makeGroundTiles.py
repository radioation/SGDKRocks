from PIL import Image, ImageDraw
import math


#  Jurasic seems to use 9 colors for in cycling
width = 48 
height = 200 # height of image
tileWidth = 8

img = Image.new( mode="P", size = (width,height))
img.putpalette([
  0,0,0,  # 0
  63,0,0, # red 1
  127,0,0,
  190,0,0,
  255,0,0,
  0,0,63,   # blue 5
  0,0,127,
  0,0,190,
  0,0,255,
  0,63,0,   # green 9
  0,127,0,
  0,190,0,
  0,255,0,
  0,63,63,   #  13
  0,127,127,
  0,190,190

  ])
dImage = ImageDraw.Draw( img )

worldY =  -250

# 96 pixel high ground

# using Z estimate from Lou's page draw initial ground patter
start = 0 
end = 80 
lowZ = worldY/ (start - height/2)
highZ = worldY/ (end - height/2)

sections = 15
sectionZ = ( highZ - lowZ ) / sections

lastZ = lowZ

lastImgY = height - 1

color = 1
for y in range( start, end, 1 ):
  #z = int( (1+(y-159)/15.9)  * 47/ (y - 160))
  z = worldY/ (y - height/2)
  if z - lastZ >= sectionZ:
    imgY = height - y - 1
    if imgY != lastImgY:
      print( imgY, lastImgY )
      dImage.rectangle( ((0,lastImgY), ( tileWidth-1, imgY+1)), fill=color) 
    lastZ += sectionZ
    lastImgY = imgY +1
    color += 1
    if color > 8:
      color = 1

dImage.line( ((0,lastImgY), ( tileWidth-1, lastImgY)), fill=color) 


# make shifted patterns
strip = img.crop( (0,0, 1, height) )

# vshift of 1 or two
offsetX = tileWidth
offsetY = 0
for i in range(0, tileWidth,1 ):
  if i == 4:
    offsetY +=1
  img.paste( strip, ( offsetX +i, offsetY, offsetX+i+1, height + offsetY ) ); 


# vshift 3 or 4 
offsetX += tileWidth
offsetY = -1
for i in range(0, tileWidth,1 ):
  if i == 1 or i == 5:
    offsetY +=1
  img.paste( strip, ( offsetX +i, offsetY, offsetX+i+1, height + offsetY ) ); 

# vshift 4 or 5 
offsetX += tileWidth
offsetY = -2
for i in range(0, tileWidth,1 ):
  if i == 2 or i == 4 or i == 6:
    offsetY +=1
  img.paste( strip, ( offsetX +i, offsetY, offsetX+i+1, height + offsetY ) ); 

# vshift 5 or 6 
offsetX += tileWidth
offsetY = -3
for i in range(0, tileWidth,1 ):
  if i == 2 or i == 3 or i == 5 or i == 6:
    offsetY +=1
  img.paste( strip, ( offsetX +i, offsetY, offsetX+i+1, height + offsetY ) ); 

# vshift 7 or 8 
offsetX += tileWidth
offsetY = -4
for i in range(0, tileWidth,1 ):
  offsetY +=1
  img.paste( strip, ( offsetX +i, offsetY, offsetX+i+1, height + offsetY ) ); 


img.save("starter.png")






