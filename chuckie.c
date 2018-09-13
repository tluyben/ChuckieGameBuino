#include <SPI.h>
#include <Gamebuino.h>
Gamebuino gb;
#include <avr/pgmspace.h>
#include <EEPROM.h>

// Screen size: 84x48 => original game is 20x20 'tiles', eggs and seeds are 1x1 tile and
// the hens + harry are 1x3 blocks; in coordinates, 4x2 for a tile, 4x2 for eggs/seeds, 
// 4x6 for harry & hens. The caged duck is 2x3 tiles; 8x6. In short; we don't have a lot 
// of pixels to work with...

// Harry sprite is 4x6
const byte  PROGMEM harry_right_straight[] =  {
  8,6,
  B00000110,
  B00001101,
  B00000110,
  B00001111,
  B00000100,
  B00000110
};
const byte PROGMEM harry_right_step[] =  {
  8,6,
  B00000110,
  B00001101,
  B00000010,
  B00001111,
  B00000010,
  B00000011
};
const byte PROGMEM harry_stairs_straight[] =  {
  8,6,
  B00000110,
  B00001111,
  B00000110,
  B00001111,
  B00000110,
  B00001111
};
const byte PROGMEM harry_stairs_right_step[] =  {
  8,6,
  B00000110,
  B00001111,
  B00000110,
  B00001111,
  B00000111,
  B00001100
};

const byte PROGMEM hen_right_straight[] =  {
  4,6,
  B1100,
  B1011,
  B0100,
  B1111,
  B0100,
  B0110
};

const byte PROGMEM hen_right_step[] =  {
  4,6,
  B1100,
  B1011,
  B0100,
  B1111,
  B011,
  B1011
};


const byte PROGMEM stairs_down[] = {
  8,2,
  B00001111,
  B00001001
};

const byte PROGMEM base_tile[] = {
  8,2,
  B00001010,
  B00000101
};

const byte PROGMEM stairs_top[] = {
  8,2,
  B00001001,
  B00001001
};

const byte PROGMEM seed[] = {
  8,2,
  B00000110,
  B00001111
};

const byte PROGMEM egg[] = {
  8,2,
  B00001111,
  B00001111
};



/*
const byte PROGMEM harry_left[] =  {
  4,6,
  B0000,
  B0000,
  B0000,
  B0000,
  B0000,
  B0000
};*/


const char PROGMEM levels[]= \
 "                    " \
 "     EU      E S   E" \
 "    ==H=== ======= =" \
 "      H             " \
 "      H   ES        " \
 "      H   ===       " \
 "    E H         S   " \
 "   ===H         ==  " \
 "      H       ==    " \
 "      H   E ==      " \
 "  EU  H   ==   S E  " \
 "  =H==H==      ==== " \
 "   H  H             " \
 "   H  H             " \
 "  EH  H    U     U  " \
 " =====H====H=====H= " \
 "      H    H     H  " \
 "      H    H     H  " \
 "  SE  H    H  S  H  " \
 "====================" 

 
;


static unsigned char PROGMEM logo[]=
{
  8,8,
  B00000000,
  B00100100,
  B00011000,
  B01111110,
  B00111100,
  B01111110,
  B00111100,
  B00000000,
};

int fc = 0; 


void setup()
{
  gb.begin();
  gb.battery.show = false;
  
  gb.titleScreen(F("Buino Egg"), logo);
}

const byte frameTest = 1; 

byte x = 20; // harry his begin position; depends on the level; these are just testing defaults
byte y = 10; 
byte m = 5; // mode = 1 is straight/right, 2 is right step , 5 is straight/left, 6 is left step etc
byte j = 0; // are we jumping; because you can move during jumping, this is something different. if it's j=0 you are not jumping, anything else, you are
const byte *harry = harry_right_straight; // harry his current stance
byte flip = FLIPH; // we don't wan to waste memory, so all sprites are flipped for left/right
byte level = 1; 

int calcIdx(int l, int _x, int _y) 
{
  int sid = (l-1) * 20 * 20; 
  int idx = sid + (_x + _y * 20); 
  return idx; 
}

// find closest level index 
int findLevelIdx(int l, int _x, int _y) 
{
  
   return calcIdx(l, _x, _y);  
}

char findLevelValue(int l, int _x, int _y) 
{
  char c = (char)pgm_read_byte(levels + findLevelIdx(l,_x,_y)); 
  return c;  
}

void drawLevel(int l) 
{
   const byte *blck;
   for (int _y = 0; _y < 20; _y ++) {
     for (int _x = 19; _x >= 0; _x --) {
       int rx = _x*4; 
       int ry = 8+_y*2;
       int idx = calcIdx(l,_x, _y);
       char c = (char)pgm_read_byte(levels + idx);  
       
       blck = NULL; 
       if (c == '=') {
         blck = base_tile; 
       } else if (c == 'S') {
         blck = seed; 
       } else if (c == 'E') {
         blck = egg;
       } else if (c == 'H') {
         blck = stairs_down;
       }else if (c == 'U') {
         blck = stairs_down;
       }
       
       if (blck) 
         gb.display.drawBitmap(rx-4,ry,blck, NOROT,NOFLIP);
     }
   }
}


void loop()
{
  if (gb.update()) {
   
    drawLevel(1);
   
    if (gb.buttons.pressed(BTN_C)) {
      gb.titleScreen(F("Chuckie Egg"), logo);
      fc = 0; 
    }
  
    fc++; 
    
    if (gb.buttons.repeat(BTN_RIGHT,frameTest)){
      harry = harry_right_straight;
      if (m == 1|| m==2) x += 2;
      flip = NOFLIP; 
      if (m == 1) {
        harry = harry_right_step;
        m = 2; 
      } else {
        m = 1;  
      }
    } 
    if (gb.buttons.repeat(BTN_LEFT,frameTest)) {
      harry = harry_right_straight;
      if (m == 5 || m == 6) x -= 2;
      flip = FLIPH;
      if (m == 5) {
        harry = harry_right_step;
        m = 6; 
      } else {
        m = 5;  
      }
    } 
    if (gb.buttons.repeat(BTN_UP,frameTest)) {
      harry = harry_stairs_straight; 
      y -= 2;
      if (m == 0) {
        harry = harry_stairs_right_step;
        m = 7; 
      } else {
      }
    }
    
    if (gb.buttons.repeat(BTN_DOWN,frameTest)) {
      harry = harry_stairs_straight; 
      y += 2;
      if (m == 3) {
        harry = harry_stairs_right_step;
        m = 4; 
      } else {
        m = 3; 
      }
    }   
    

    
    // handle jumping
    /*if (j>0) {
      if (j > 5) {
       y --; 
      } else {
       y ++;  
      }
      j --; 
    }
    */
    if (gb.buttons.repeat(BTN_A, frameTest) && fc > 10) {
      if (j == 0) { // we can repeat jump but only when the previous jump was actually done
        j = 10; 
      } 
        
    }
    
    if (j > 5) {
      y--;j--; 
    }
    // handle falling
    else { // when not jumping
      int _x = ceil(x/4);
      int _y = ceil((y-8)/2)+3; 
      if (findLevelValue(level,_x,_y) == 0x20)
      {
        if (j > 5) {
          y--;
          j--;
        } else if (j > 0) {
          y++;
          j--;
        } else { 
          y += 2;
        } 
      } else {
        y = 8+(_y-3)*2;  
      }
    }
    
    // make sure we don't run off the screen
    if (x < 4) x = 4; 
    if (x > 76) x = 76;
    if (y < 2) y = 2; 
    if (y > 42) y = 42; 
    
    // x-4 because sprites have to be bytes, not nibbles, so to place them correctly the real coordinates are the virtual ones - 4 in x
    gb.display.drawBitmap(x-4,y,harry, NOROT,flip);
  

  }
  
   
}
