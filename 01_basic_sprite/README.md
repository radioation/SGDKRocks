# Basic Sprite Example

SGDK makes it easy to load and display sprites from images.
At a minimum you want to 
1. Define a sprite resource  in res/resources.res
~~~
SPRITE ship "sprites/ship.png" 2 2 NONE
~~~

2. Load the color palette of the sprite resource
~~~c
	VDP_setPalette( PAL1, ship.palette->data );
~~~
3. Initialize the sprite engine
~~~c
  SPR_init()
~~~
4. Create a Sprite with SPR_addSprite()
~~~c
	Sprite *shipSprite = NULL;
	shipSprite = SPR_addSprite( &ship,  	// Sprite name from resources.res
				152, 	 	// starting X position
				102, 	 	// starting Y position
				TILE_ATTR( 
					PAL1,   // Palette index
					0,      // Palette index
					FALSE,  // Vertical Flip
					FALSE   // Horizontal Flip
					));	
~~~

5. Update the sprite engine in the main game loop
~~~c
  SPR_update();
~~~


# Sprite Image Creation
Sega Genesis sprites can be created from 16 color indexed images.


## GIMP
1. Create an image using *"File|New..."*.   Set the image width and height to a size supported by SGDK.

2. Convert the image to an Indexed Image using *"Image|Mode|Indexed..."*.  Choose a palette with 16 colors or fewer.

![Convert Image](https://github.com/radioation/SGDKRocks/blob/main/01_basic_sprite/imgs/gimp_indexed_image.jpg)

3. You can use the "Colormap" Dockable Dialog to edit colors if needed.

![Color Map](https://github.com/radioation/SGDKRocks/blob/main/01_basic_sprite/imgs/gimp_drawing_with_colormap.jpg)

4. Draw your sprite.   Take care to use only 15 color or fewer.   The first color is the background color and will be transparent when rendered by SGDK.

5. Once you're doing editing your image, export it as a PNG with *"File|Export As..."*
 

## Pro Motion NG
1. Create a new project using *"File|New Project|Create ..."*.   Set the image width and height to a size supported by SGDK.
2. In the "Create a new Project" dialog box set the size of your image, the Type to "Sega Megadrive/Genesis (Generic)" and the Color Palette to "rainbow333.pal"

![New Project](https://github.com/radioation/SGDKRocks/blob/main/01_basic_sprite/imgs/pmng_new_project.png)

3. Draw your sprite.  Take care to use only 15 colors or fewer.  The first color is the background color and will be transparent when rendered by SGDK.

4. Reduce the colors in your image to 16.  Use *"Colors|Reduce Colors..."*

![Reduce Colors](https://github.com/radioation/SGDKRocks/blob/main/01_basic_sprite/imgs/pmng_reduce_colors_to_16.png)

5. Once you're doing editing your image, save it as a PNG with *"File|Save Image As..."*

![Save as PNG](https://github.com/radioation/SGDKRocks/blob/main/01_basic_sprite/imgs/pmng_export_png.png)





