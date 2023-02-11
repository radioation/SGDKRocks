# NOTES:
* As far as I can tell, there is no `VDP_setSpriteP()` function or `_spritedef` in v1.80 of SGDK.  Searching the source and header files yield no matches:
```bash
ggallard@BOOMSTICK:/mnt/c/Dev/SGDK/sgdk180$ find . -type f -name "*.[c|h]" -exec grep "\b_spritedef\b" {} +
ggallard@BOOMSTICK:/mnt/c/Dev/SGDK/sgdk180$ find . -type f -name "*.[c|h]" -exec grep "\bVDP_setSpriteP\b" --color {} +
ggallard@BOOMSTICK:/mnt/c/Dev/SGDK/sgdk180$
```
* OTOH, a bunch of functions starting with `VDP_setSprite` are available.  With names like `VDP_setSpritePosition()` and `VDP_setSpriteTile()`, it should be obvious what they do.
```bash
ggallard@BOOMSTICK:/mnt/c/Dev/SGDK/sgdk180$ find . -type f -name "*.[c|h]" -exec grep "\bvoid VDP_setSprite" --color {} +
./inc/vdp.h:void VDP_setSpriteListAddress(u16 value);
./inc/vdp_spr.h:void VDP_setSpriteFull(u16 index, s16 x, s16 y, u8 size, u16 attribut, u8 link);
./inc/vdp_spr.h:void VDP_setSprite(u16 index, s16 x, s16 y, u8 size, u16 attribut);
./inc/vdp_spr.h:void VDP_setSpritePosition(u16 index, s16 x, s16 y);
./inc/vdp_spr.h:void VDP_setSpriteSize(u16 index, u8 size);
./inc/vdp_spr.h:void VDP_setSpriteAttribut(u16 index, u16 attribut);
./inc/vdp_spr.h:void VDP_setSpriteLink(u16 index, u8 link);
./inc/vdp_spr.h:void VDP_setSpritePriority(u16 index, bool priority);
./inc/vdp_spr.h:void VDP_setSpritePalette(u16 index, u16 palette);
./inc/vdp_spr.h:void VDP_setSpriteFlip(u16 index, bool flipH, bool flipV);
./inc/vdp_spr.h:void VDP_setSpriteFlipH(u16 index, bool flipH);
./inc/vdp_spr.h:void VDP_setSpriteFlipV(u16 index, bool flipV);
./inc/vdp_spr.h:void VDP_setSpriteTile(u16 index, u16 tile);
./src/vdp.c:void VDP_setSpriteListAddress(u16 value)
./src/vdp_spr.c:void VDP_setSpriteFull(u16 index, s16 x, s16 y, u8 size, u16 attribut, u8 link)
./src/vdp_spr.c:void VDP_setSprite(u16 index, s16 x, s16 y, u8 size, u16 attribut)
./src/vdp_spr.c:void VDP_setSpritePosition(u16 index, s16 x, s16 y)
./src/vdp_spr.c:void VDP_setSpriteSize(u16 index, u8 size)
./src/vdp_spr.c:void VDP_setSpriteAttribut(u16 index, u16 attribut)
./src/vdp_spr.c:void VDP_setSpriteLink(u16 index, u8 link)
./src/vdp_spr.c:void VDP_setSpritePriority(u16 index, bool priority)
./src/vdp_spr.c:void VDP_setSpritePalette(u16 index, u16 palette)
./src/vdp_spr.c:void VDP_setSpriteFlip(u16 index, bool flipH, bool flipV)
./src/vdp_spr.c:void VDP_setSpriteFlipH(u16 index, bool flipH)
./src/vdp_spr.c:void VDP_setSpriteFlipV(u16 index, bool flipV)
./src/vdp_spr.c:void VDP_setSpriteTile(u16 index, u16 tile)
```
Instead of manually setting all sprite parameters everytime you want to update a sprite, you can just set the ones that are changing. Its less coding, but I haven't looked into how fast these run.

* `sgdk_tile_swap.py`: is a *simple* Python script I wrote to convert some my .png files into a format I can use with `VDP_loadTileData`. 

* `-W` sets the width of the sprite in tiles 
* `-H` sets the height of the sprite in tiles 
* `-i` specifies the name of file to process
* `-o` specifies the output file name 

For a 16x16 sprite:
```bash
python3 sgdk_tile_swap.py -W 2 -H 2 -i numbers.png -o swapnum.png
```
For a 32x32 sprite:
```bash
python3 sgdk_tile_swap.py -W 4 -H 4 -i shipsheet.png -o swapship.png
```
I have not yet tried it with 24x24 or other sizes




