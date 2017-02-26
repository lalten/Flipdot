/////////////////////////////////////////////////////////////////
//  Testenvironment and base for displaying Fonts on the 
//  Flipdot display. 
//  CC-BY SA NC 2016 c-hack.de    ralf@surasto.de
/////////////////////////////////////////////////////////////////

#include "font6x8.h"
#include "font8x8.h"
#include "font8x12.h"

//================== Constants ===============================

#include "Flipdot.h"
#define OFF 0
#define ON 1

//================ global Variables ==========================
// frameBuffer stores the content of the Flipdotmatrix
// Each pixel is one bit (I.e only an eigth of storage is required 
unsigned char frameBuffer[X_SIZE][Y_SIZE];    


//#################### Public Functions ####################################

//====================================================
// Clears the entire framebuffer and flips every pixel
// of the display
// color = BLACK   all pixels set to black
// color = YELLOW  all pixels set to yellow
//====================================================
void clearAll(int color) {
   int i,j;

   for (i=0; i<X_SIZE; i++) {
//   for (i=0; i<168; i++) {
     Serial.println(i);
     for (j=0; j<Y_PIXELS; j++) {
       setFrameBuffer(i,j,color);
       pixel(i,j,color);
 //      delay(100);     // Slow down for debug
     }
   }
}

//=====================================================
// Clears the entire framebuffer but only those pixels
// get changed which have to be changed.
// This can be much faster but requires that the 
// frambuffer is really up to date
// color = BLACK   all pixels set to black
// color = YELLOW  all pixels set to yellow
//====================================================
void quickClear(int color) {
   int i,j;
   int old;

   for (i=0; i<X_SIZE; i++) 
     for (j=0; j<Y_PIXELS; j++) {
       old = getFrameBuffer(i,j);
       setFrameBuffer(i,j,color);
       if (old!=color) pixel(i,j,color);   // Write to Hardware only if changed
     }
}

//====================================================
// Sets a Pixel at row Y column X
// color = BLACK   all pixels set to black
// color = YELLOW  all pixels set to yellow
//====================================================
void setPixel(int x, int y, int color) {
  
   pixel(x,y,color);
   
}


//====================================================
// Draws a horizotal line at row Y
// color = BLACK   all pixels set to black
// color = YELLOW  all pixels set to yellow
//===================================================
void hLine(int y, int color) {
  int i;

  for (i=0; i<X_SIZE; i++) {
     setFrameBuffer(i, y, color);
     pixel(i,y,color);
     //delay(200);
  }
}

//====================================================
// Draws a vertical line at column X
// color = BLACK   all pixels set to black
// color = YELLOW  all pixels set to yellow
//====================================================
void vLine(int x, int color) {
  int i;

  for (i=0; i<Y_PIXELS; i++) {
     setFrameBuffer(x, i, color);
     pixel(x,i,color);
     //delay(200);
  }
}


//====================================================
// This funtion flips the pixels in one range 100 time
// in order to get rid of remanence effects or other
// reasons for stuck pixels.
// x1 = start column
// x2 = stop column
//====================================================
void crossTrainer(int x1, int x2) {
  int i,j;

  for (i=x1; i<=x2; i++) {
    for (j=0; j<10; j++) {
      vLine(i,0);  // Yellow line
      vLine(i,1);  // Black line
    }
  }
}



//============================================
// printString(int xOffs, int yOffs, int color, int size char s) 
// xOffs = position of the left side of the string
// yOffs = position of the top of the string
// color = ON means yellow, OFF means black
// size = SMALL | MEDIUM | LARGE
// s = string
//============================================
int printString(int xOffs, int yOffs, int color, int size, String s) {
  int i,x,y;
  char c;
  unsigned char uc;
    
  i=0;
  x=xOffs;
  y=yOffs;

  if (size==SMALL) x=xOffs-2;   // Somehow I need to shift the text to the left

  while ((i<s.length()) && (i<100)) {
    c = s.charAt(i);
    //Serial.print((int) c); 
    
    if (c == -61) {
      if (i < s.length()-1) i++;
      c = s.charAt(i); 
      switch(c) {
        case -92:  uc=132; break;   // ä
        case -74:  uc=148; break;   // ö
        case -68:  uc=129; break;   // ü
        case -124: uc=142; break;   // Ä
        case -106: uc=153; break;   // Ö
        case -100: uc=154; break;   // Ü
        case -97:  uc=225; break;   // ß
        default: break;
      }
    } else uc=c;

    //Serial.print(" --> ");
    //Serial.println(uc); // debug only
    
    switch(size) {
      case SMALL: x = printChar6x8(x, y, color, uc); break;
      case MEDIUM: x = printChar8x8(x, y, color, uc); break;
      case LARGE: x = printChar8x12(x, y, color, uc); break;
      default: x = printChar6x8(x, y, color, uc);
    }
    //Serial.print(s.charAt(i));  
    i++;
  }
  return(x);
}


//###################### Internal Functions ##########################################


//===========================================
// setFrameBuffer(int x, int y, int value)
// Set one Pixel at x,y-Position
// value can be ON or OFF
//=========================================== 
void setFrameBuffer(int x, int y, int value) {
    unsigned char w, wNot;
    int yByteNo, yBitNo;

    w=1;
    if ((y < 8 * Y_SIZE)&&(x < X_SIZE)&&(x>=0)&&(y>=0)) {
		yByteNo = y/8;    // integer division to select the byte
		yBitNo = y%8;     // modulo division (residual) to select the bit in that byte
    	w = w<<yBitNo;
    	if (value == ON) { 
	       frameBuffer[x][yByteNo] = frameBuffer[x][yByteNo]|w;  // Logical OR adds one bit to the existing byte
	    } else {
   		   wNot = 0xFF - w;	
	       frameBuffer[x][yByteNo] = frameBuffer[x][yByteNo]&wNot;  // Logical AND set one bit to zero in the existing byte
 	    }
	}
}

//===========================================
// int getFrameBuffer(int x, int y)
// Gets color of one Pixel at x,y-Position
// Value can be BLACK or YELLOW
//=========================================== 
int getFrameBuffer(int x, int y) {
    unsigned char w, wNot;
    int yByteNo, yBitNo;

    w=1;
    if ((y < 8 * Y_SIZE)&&(x < X_SIZE)&&(x>=0)&&(y>=0)) {
    yByteNo = y/8;    // integer division to select the byte
    yBitNo = y%8;     // modulo division (residual) to select the bit in that byte
    w = w<<yBitNo;
    if (frameBuffer[x][yByteNo]&w > 0) return(1); else return(0);
  }
}


//============================================
// printChar6x8(int xOffs, int yOffs, char c) 
// printChar8x8(int xOffs, int yOffs, char c) 
// printChar8x12(int xOffs, int yOffs, char c) 
//
// xOffs = position of the left side of the character
// yOffs = position of the top of the character
// color = ON means yellow, OFF means black
// c = ASCII Character
// returns new x position
//============================================
int printChar6x8(int xOffs, int yOffs, int color, unsigned char c) {
	unsigned char x,y,w;
	
	for (y=0; y<8; y++) {   
		w=pgm_read_byte(&(font6x8[c][y]));    // Important: pgm_read_byte reads from the array in the flash memory
    for (x=0; x<8; x++) {   
		if (w&1) {
		  setFrameBuffer(x+xOffs,y+yOffs,color);
      pixel(x+xOffs, y+yOffs, color);
		}
       w=w>>1;
    } 
  }
  return(xOffs+6);
}

int printChar8x8(int xOffs, int yOffs, int color, unsigned char c) {
	unsigned char x,y,w;
	
	for (y=0; y<8; y++) {   
		w=pgm_read_byte(&(font8x8[c][y]));   // Important: pgm_read_byte reads from the array in the flash memory
    for (x=0; x<8; x++) {   
 			if (w&1) {
 			  setFrameBuffer(x+xOffs,y+yOffs,color);
        pixel(x+xOffs, y+yOffs, color);
 			}
      w=w>>1;
    } 
  }
  return(xOffs+8);
}


int printChar8x12(int xOffs, int yOffs, int color, unsigned char c) {
	unsigned int x,y,w;
	
	for (y=0; y<12; y++) {   
		w=pgm_read_byte(&(font8x12[c][y]));   // Important: pgm_read_byte reads from the array in the flash memory
    for (x=0; x<12; x++) {   
			if (w&1) {
			  setFrameBuffer(x+xOffs,y+yOffs,color);
        pixel(x+xOffs, y+yOffs, color);
			}
      w=w>>1;
    } 
  }
  return(xOffs+7);
}


//============================================
// DEBUG ONLY
// printFrameBuffer is only used to see the 
// content on the screen for debug
//============================================
void printFrameBuffer() {
	int x,y,bitNo,maxBits;
	unsigned char w;
	
	maxBits=8;
    for (y=0; y<Y_SIZE; y++) {
	   w = 1;   // most right bit set
	   if (y == Y_SIZE-1) {
		   maxBits = 8 - Y_SIZE*8 % Y_PIXELS;
	   } 
	   for (bitNo=0; bitNo<maxBits; bitNo++) {
		   for (x=0; x<X_SIZE; x++) {
			   if (frameBuffer[x][y] & w) Serial.print("#"); else Serial.print(".");
		   }  
		   w = w<<1; 
		   Serial.println("");
	   }
    }
}

//===========================================
// DEBUG ONLY
// This is to check if reading from Flash 
// memory works
// Using: pgm_read_byte
//===========================================
void printFont() {
  int i;

  for (i=0; i<256; i++) Serial.println(pgm_read_byte(&(font6x8[i][0])) );
}
