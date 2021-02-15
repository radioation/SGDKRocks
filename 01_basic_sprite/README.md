# Basic Sprite Example

SGDK makes it easy to load and display sprites from images.
At a minimum you want to 
1. Define a sprite resource  in res/resources.res

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


## Pro Motion NG
1. Create a new project using *"File|New Project|Create ..."*
2. In the "Create a new Project" dialog box set the Type to "Sega Megadrive/Genesis (Generic)" and the Color Palette to "rainbow333.pal"
![New Project](https://github.com/radioation/SGDKRocks/blob/main/01_basic_sprite/imgs/pmng_new_project.png)





