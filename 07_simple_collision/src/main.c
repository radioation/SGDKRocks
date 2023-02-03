#include <genesis.h>
#include "resources.h"


// constants
#define MAX_SHOTS 15
#define MAX_UFOS 9 
#define MAX_UFO_SHOTS  18

#define LEFT_EDGE  0
#define RIGHT_EDGE  320
#define TOP_EDGE 0
#define BOTTOM_EDGE 224


// the play 
struct CP_SPRITE {
  Sprite *sprite;
  s16 pos_x;
  s16 pos_y;
  s16 vel_x;
  s16 vel_y;

  s16 hitbox_x1;
  s16 hitbox_y1;
  s16 hitbox_x2;
  s16 hitbox_y2;

  bool active;

};


struct CP_SPRITE shipSprite;
struct CP_SPRITE shipShots[MAX_SHOTS];


struct CP_SPRITE ufos[MAX_UFOS];
struct CP_SPRITE ufoShots[MAX_UFO_SHOTS];


void myJoyHandler( u16 joy, u16 changed, u16 state)
{
  if (joy == JOY_1)
  {
    if (state & BUTTON_A) {

      int addedShot = 0;
      for( int i=0; i < MAX_SHOTS; ++i ) {
        if( shipShots[i].active == FALSE ) {
          shipShots[i].active = TRUE;
          // set its starting position
          shipShots[i].pos_x = shipSprite.pos_x+12;
          shipShots[i].pos_y = shipSprite.pos_y;
          switch( addedShot ) {            
            case 0:
              shipShots[i].vel_x =  0;
              shipShots[i].vel_y = -6;
              break;

            case 1:
              shipShots[i].vel_x = -2;
              shipShots[i].vel_y = -5;
              break;

            case 2:
              shipShots[i].vel_x =  2;
              shipShots[i].vel_y = -5;
              break;

            case 3:
              shipShots[i].vel_x = -3;
              shipShots[i].vel_y = -3;
              break;

            case 4:
              shipShots[i].vel_x =  3;
              shipShots[i].vel_y = -3;
              break;


          }
          ++addedShot;
          if( addedShot >= 5 ) {
            break;
          }
        }
      }

    }
    if (state & BUTTON_B) {
      for( u16 i=0; i < MAX_UFOS; ++i ) {
        if( ufos[i].active == TRUE ) {
          // create 3 shots each
          int cap = MAX_UFO_SHOTS/MAX_UFOS;
          for(u16 j=0; j < cap; ++j ) {
            if( ufoShots[i*cap + j].active == FALSE ) {

              ufoShots[i*cap+j].pos_x = ufos[i].pos_x + 16;
              ufoShots[i*cap+j].pos_y = ufos[i].pos_y + 16;
              ufoShots[i*cap+j].active = TRUE;
              if( j == 0 ) {
                ufoShots[i*cap].vel_x =  0;
                ufoShots[i*cap].vel_y = +3;
              } else if (j == 1 ) {
                ufoShots[i*cap+1].vel_x = -1;
                ufoShots[i*cap+1].vel_y = +2;
              } else if (j == 2 ) {
                ufoShots[i*cap+2].vel_x =  1;
                ufoShots[i*cap+2].vel_y = +2;
              }
            }
          }

        }
      }
    }

    if (state & BUTTON_C) {
      for( u16 i=0; i < MAX_UFOS; ++i ) {
        ufos[i].active = TRUE;
      }
    }
    /*Set player velocity if left or right are pressed;
     *set velocity to 0 if no direction is pressed */
    if (state & BUTTON_RIGHT)
    {
      shipSprite.vel_x = 2;
    }
    else if (state & BUTTON_LEFT)
    {
      shipSprite.vel_x = -2;
    } else{
      if( (changed & BUTTON_RIGHT) | (changed & BUTTON_LEFT) ){
        shipSprite.vel_x = 0;
      }
    }

    if (state & BUTTON_UP)
    {
      shipSprite.vel_y = -2;
    }
    else if (state & BUTTON_DOWN)
    {
      shipSprite.vel_y = 2;
    } else{
      if( (changed & BUTTON_UP) | (changed & BUTTON_DOWN) ){
        shipSprite.vel_y = 0;
      }
    }
  }
}


void update() {
  //Check horizontal bounds
  if(shipSprite.pos_x < LEFT_EDGE){
    shipSprite.pos_x = LEFT_EDGE;
    shipSprite.vel_x = -shipSprite.vel_x;
  } else if(shipSprite.pos_x + (shipSprite.hitbox_x2 - shipSprite.hitbox_x1 ) > RIGHT_EDGE){
    shipSprite.pos_x = RIGHT_EDGE - (shipSprite.hitbox_x2 - shipSprite.hitbox_x1) ;
    shipSprite.vel_x = -shipSprite.vel_x;
  }


  //Check vertical bounds
  if(shipSprite.pos_y < TOP_EDGE){
    shipSprite.pos_y = TOP_EDGE;
    shipSprite.vel_y = -shipSprite.vel_y;
  } else if(shipSprite.pos_y + (shipSprite.hitbox_y2 - shipSprite.hitbox_y1 ) > BOTTOM_EDGE){
    shipSprite.pos_y = BOTTOM_EDGE - ( shipSprite.hitbox_y2 - shipSprite.hitbox_y1 );
    shipSprite.vel_y = -shipSprite.vel_y;
  }

  //Position the ship
  shipSprite.pos_x += shipSprite.vel_x;
  shipSprite.pos_y += shipSprite.vel_y;


  // shots
  for( u16 i=0; i < MAX_SHOTS; ++i ) {
    if( shipShots[i].active == TRUE ) {
      shipShots[i].pos_x +=  shipShots[i].vel_x;
      shipShots[i].pos_y +=  shipShots[i].vel_y;
      if(shipShots[i].pos_y  < 0 ) {
        shipShots[i].pos_x = -16;
        shipShots[i].pos_y = 230;
        shipShots[i].vel_x = 0;
        shipShots[i].vel_y = 0;
        shipShots[i].active = FALSE;
      }
      SPR_setPosition(shipShots[i].sprite,shipShots[i].pos_x,shipShots[i].pos_y);
    } else {
      SPR_setPosition( shipShots[i].sprite, -32, 230 );
    }
  }

  for( u16 i=0; i < MAX_UFO_SHOTS; ++i ) {
    if( ufoShots[i].active == TRUE ) {
      ufoShots[i].pos_x +=  ufoShots[i].vel_x;
      ufoShots[i].pos_y +=  ufoShots[i].vel_y;
      if(ufoShots[i].pos_y  > 224 ) {
        ufoShots[i].pos_x = -16;
        ufoShots[i].pos_y = 230;
        ufoShots[i].vel_x = 0;
        ufoShots[i].vel_y = 0;
        ufoShots[i].active = FALSE;
      }
      SPR_setPosition(ufoShots[i].sprite,ufoShots[i].pos_x,ufoShots[i].pos_y);
    } else {
      SPR_setPosition( ufoShots[i].sprite, -32, 230 );
    }
  }

  SPR_setPosition( shipSprite.sprite, shipSprite.pos_x, shipSprite.pos_y );

  for( u16 i=0; i < MAX_UFOS; ++i ) {
    if( ufos[i].active == TRUE ) {
      SPR_setPosition( ufos[i].sprite, ufos[i].pos_x, ufos[i].pos_y);
    } else {
      SPR_setPosition( ufos[i].sprite, -32, 230 );
    }
  }


}

void checkCollisions() {
  /*
     function CheckCollisions(){

     for( var i = 0; i < rocks.length  ;  i++ ){

  // check if shot hits the rock
  for( var j = 0;  j< shots.length; j++ ){
  //alert( shots[j].shape.length )
  if( rocks[i].CheckCollision( shots[j] ) ){
  shots[j].alive = 0;
  rocks[i].alive = 0;
  }
  }// for( var j = 0;  j< shots.length; j++ ){

  // check if ship hits the rock
  if( ship != null ){
  if( rocks[i].CheckCollision( ship ) ){
  ship.alive = 0;
  rocks[i].alive = 0;
  }
  }// if( ship != null ){

  }// for( var i = 0; i < rocks.length  ;  i++ ){
  }
  */
  for( u16 i=0; i < MAX_UFOS; ++i ) {
    if( ufos[i].active == TRUE ) {
      // check if ship has hit
      if( (ufos[i].pos_x + ufos[i].hitbox_x1) < (shipSprite.pos_x + shipSprite.hitbox_x2) &&
          (ufos[i].pos_x + ufos[i].hitbox_x2) > (shipSprite.pos_x + shipSprite.hitbox_x1) &&
          (ufos[i].pos_y + ufos[i].hitbox_y1) < (shipSprite.pos_y + shipSprite.hitbox_y2) &&
          (ufos[i].pos_y + ufos[i].hitbox_y2) > (shipSprite.pos_y + shipSprite.hitbox_y1)  ) 
      {
        ufos[i].active = FALSE;
      }

      for( u16 j=0; j < MAX_SHOTS; ++j ) {
        if(
            shipShots[j].active == TRUE &&
            (ufos[i].pos_x + ufos[i].hitbox_x1) < (shipShots[j].pos_x + shipShots[j].hitbox_x2) &&
            (ufos[i].pos_x + ufos[i].hitbox_x2) > (shipShots[j].pos_x + shipShots[j].hitbox_x1) &&
            (ufos[i].pos_y + ufos[i].hitbox_y1) < (shipShots[j].pos_y + shipShots[j].hitbox_y2) &&
            (ufos[i].pos_y + ufos[i].hitbox_y2) > (shipShots[j].pos_y + shipShots[j].hitbox_y1)  ) 
        {
          ufos[i].active = FALSE;
          shipShots[j].active = FALSE;
        }
      }
    }
  }
  for( u16 i=0; i < MAX_UFO_SHOTS; ++i ) {
    if( ufoShots[i].active == TRUE ) {
      if( (ufoShots[i].pos_x + ufoShots[i].hitbox_x1) < (shipSprite.pos_x + shipSprite.hitbox_x2) &&
          (ufoShots[i].pos_x + ufoShots[i].hitbox_x2) > (shipSprite.pos_x + shipSprite.hitbox_x1) &&
          (ufoShots[i].pos_y + ufoShots[i].hitbox_y1) < (shipSprite.pos_y + shipSprite.hitbox_y2) &&
          (ufoShots[i].pos_y + ufoShots[i].hitbox_y2) > (shipSprite.pos_y + shipSprite.hitbox_y1)  ) 
      {
        ufoShots[i].active = FALSE;
      }
    }
  }

}

void createUFOs() {
  s16 ufo_pos_x = 66;
  s16 ufo_pos_y = 28;

  s16 xpos = 66;
  s16 ypos = 22;
  for( u16 i=0; i < MAX_UFOS; ++i ) {
    if( i == 5 ) {
      ypos = ufo_pos_y + 40;
      xpos = ufo_pos_x + 16;
    } 
    ufos[i].pos_x = xpos;
    ufos[i].pos_y = ypos;
    ufos[i].vel_x = 0;
    ufos[i].vel_y = 0;
    ufos[i].active = TRUE;
    ufos[i].hitbox_x1 = 0;
    ufos[i].hitbox_y1 = 0;
    ufos[i].hitbox_x2 = 32;
    ufos[i].hitbox_y2 = 32;

    ufos[i].sprite  = SPR_addSprite( &ufo, xpos, ypos, TILE_ATTR( PAL2, 0, FALSE, FALSE ));
    xpos += 40;

    SPR_setAnim( ufos[i].sprite, 0 );
  }

}

void createUFOShots() {
  s16 xpos = -16;
  s16 ypos = 230;

  for( u16 i=0; i < MAX_UFO_SHOTS; ++i ) {
    ufoShots[i].pos_x = xpos;
    ufoShots[i].pos_y = ypos;
    ufoShots[i].vel_x = 0;
    ufoShots[i].vel_y = 0;
    ufoShots[i].active = FALSE;
    ufoShots[i].hitbox_x1 = 0;
    ufoShots[i].hitbox_y1 = 0;
    ufoShots[i].hitbox_x2 = 8;
    ufoShots[i].hitbox_y2 = 8;

    ufoShots[i].sprite = SPR_addSprite( &shots, xpos, ypos, TILE_ATTR( PAL0, 0, FALSE, FALSE ));
    SPR_setAnim( ufoShots[i].sprite, 1 );
  }

}

void createShipShots() {
  s16 xpos = -16;
  s16 ypos = 230;

  for( u16 i=0; i < MAX_SHOTS; ++i ) {
    shipShots[i].pos_x = xpos;
    shipShots[i].pos_y = ypos;
    shipShots[i].vel_x = 0;
    shipShots[i].vel_y = 0;
    shipShots[i].active = FALSE;
    shipShots[i].hitbox_x1 = 0;
    shipShots[i].hitbox_y1 = 0;
    shipShots[i].hitbox_x2 = 8;
    shipShots[i].hitbox_y2 = 8;

    shipShots[i].sprite = SPR_addSprite( &shots, xpos, ypos, TILE_ATTR( PAL0, 0, FALSE, FALSE ));
    SPR_setAnim( shipShots[i].sprite, 2 );
  }

}

int main( bool h ) {
  // SETUP backgroupd
  VDP_setBackgroundColor(0);
  VDP_setScreenWidth320();


  PAL_setPalette( PAL0, shots_pal.data, CPU);
  PAL_setPalette( PAL1, ship_pal.data, CPU );
  PAL_setPalette( PAL2, ufo_pal.data, CPU);


  // Sprite initializatoin
  SPR_init();

  // set intiial position for player
  shipSprite.pos_x = 144;
  shipSprite.pos_y = 160;
  shipSprite.vel_x = 0;
  shipSprite.vel_y = 0;
  shipSprite.active = TRUE;
  shipSprite.hitbox_x1 = 0;
  shipSprite.hitbox_y1 = 0;
  shipSprite.hitbox_x2 = 32;
  shipSprite.hitbox_y2 = 32;

  shipSprite.sprite  = SPR_addSprite( &ship, shipSprite.pos_x, shipSprite.pos_y, TILE_ATTR( PAL1, 0, FALSE,FALSE ));

  createShipShots();
  createUFOs();
  createUFOShots();


  SPR_update();


  //////////////////////////////////////////////////////////////
  // setup joystick
  JOY_init();
  JOY_setEventHandler( &myJoyHandler );

  while(TRUE) {
    VDP_showFPS(TRUE);

    // 1) update objects in game
    update();

    // 2) detect collisions
    checkCollisions();


    // 3) Update sprites
    SPR_update();


    SYS_doVBlankProcess();


  }

}



