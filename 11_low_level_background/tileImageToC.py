from PIL import Image

img = Image.open("ground_tiles.png")

width, height = img.size

cols = int(width/8)
rows = int(height/8)
print("cols: %d, rows: %d" % (cols, rows ) )

px = img.load()

tileX = 0
print( "const u32 tileSet%d[%d] =\n{"%( tileX, 32 * rows ) ) 
for tileY in range( 0, rows ) :
  print(" // tile %d " % tileY )
  for y in range(0,8):
    print("0x", end="")
    for x in range(0,8):
      print( f'{px[x + tileX * 8,y + tileY * 8 ]:x}', end="") 
    if tileY < rows - 1:
      print(",")
    elif y < 7:
      print(",")
    else:
      print("\n};")
