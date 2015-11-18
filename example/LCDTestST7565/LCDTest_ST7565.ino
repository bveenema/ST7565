#include "backlight.h"
#include "ST7565.h"

#define BACKLIGHT_RED TX
#define BACKLIGHT_GREEN RX
#define BACKLIGHT_BLUE WKP
#define BACKLIGHT_LED 4		//Remove this after POC

#define LOGO16_GLCD_HEIGHT 16
#define LOGO16_GLCD_WIDTH  16


// a bitmap of a 16x16 fruit icon
const static unsigned char __attribute__ ((progmem)) logo16_glcd_bmp[]={
0x30, 0xf0, 0xf0, 0xf0, 0xf0, 0x30, 0xf8, 0xbe, 0x9f, 0xff, 0xf8, 0xc0, 0xc0, 0xc0, 0x80, 0x00,
0x20, 0x3c, 0x3f, 0x3f, 0x1f, 0x19, 0x1f, 0x7b, 0xfb, 0xfe, 0xfe, 0x07, 0x07, 0x07, 0x03, 0x00, };

// pin 2 - Serial data out (SID)
// pin 4 - Serial clock out (SCLK)
// pin A5 - Data/Command select (RS or A0)
// pin A4 - LCD reset (RST)
// pin 6 - LCD chip select (CS)
ST7565 glcd(2, 4, A5, A4, 6);
backlight backlight(BACKLIGHT_RED,BACKLIGHT_GREEN,BACKLIGHT_BLUE);

void setup(){
	RGB.control(true);

	backlight.setBrightness(255);
	backlight.setColor(255,255,255);

	// initialize and set the contrast to 0x18
	glcd.begin(0x18);
	glcd.display(); // show splashscreen
	delay(1000);
	glcd.clear();

	// draw a single pixel
	glcd.setpixel(10, 10, BLACK);
	glcd.display();        // show the changes to the buffer
	delay(2000);
	glcd.clear();

	// draw many lines
	testdrawline();
	glcd.display();       // show the lines
	delay(2000);
	glcd.clear();

	// draw rectangles
	testdrawrect();
	glcd.display();
	delay(2000);
	glcd.clear();

	// draw multiple rectangles
	testfillrect();
	glcd.display();
	delay(2000);
	glcd.clear();

	// draw mulitple circles
	testdrawcircle();
	glcd.display();
	delay(2000);
	glcd.clear();

	// draw a black circle, 10 pixel radius, at location (32,32)
	glcd.fillcircle(32, 32, 10, BLACK);
	glcd.display();
	delay(2000);
	glcd.clear();

	// draw the first ~120 characters in the font
	testdrawchar();
	glcd.display();
	delay(2000);
	glcd.clear();

	// draw a string at location (0,0)
	glcd.drawstring(0, 0, "Lorem ipsum dolor sit amet, consectetur adipisicing elit, sed do eiusmod tempor incididunt ut labore et dolore magna aliqua. Ut enim ad minim veniam, quis nostrud exercitation");
	glcd.display();
	delay(2000);
	glcd.clear();

	// draw a bitmap icon and 'animate' movement
	testdrawbitmap(logo16_glcd_bmp, LOGO16_GLCD_HEIGHT, LOGO16_GLCD_WIDTH);
}
void loop(){
	backlight.update();
	if(backlight.isFading()==0){
		static uint8_t i = 0;
		if(i==0){
			backlight.fadeTo(0,1000);
			i = 1;
		}else if(i==1){
			backlight.setColor(255,255,0);
			backlight.fadeTo(255,1000);
			i = 2;
		}else if(i==2){
			backlight.setColor(255,0,0);
			backlight.breathe(100,255,750);
			i = 3;
		}
	}
}


#define NUMFLAKES 10
#define XPOS 0
#define YPOS 1
#define DELTAY 2

void testdrawbitmap(const uint8_t *bitmap, uint8_t w, uint8_t h) {
  uint8_t icons[NUMFLAKES][3];
  randomSeed(666);     // whatever seed

  // initialize
  for (uint8_t f=0; f< NUMFLAKES; f++) {
    icons[f][XPOS] = random(128);
    icons[f][YPOS] = 0;
    icons[f][DELTAY] = random(5) + 1;
  }

	uint32_t initTime = millis();
	uint32_t drawBMPTime = 15000;
  while (millis()-initTime < drawBMPTime) {
    // draw each icon
    for (uint8_t f=0; f< NUMFLAKES; f++) {
      glcd.drawbitmap(icons[f][XPOS], icons[f][YPOS], logo16_glcd_bmp, w, h, BLACK);
    }
    glcd.display();
    delay(200);

    // then erase it + move it
    for (uint8_t f=0; f< NUMFLAKES; f++) {
      glcd.drawbitmap(icons[f][XPOS], icons[f][YPOS],  logo16_glcd_bmp, w, h, 0);
      // move it
      icons[f][YPOS] += icons[f][DELTAY];
      // if its gone, reinit
      if (icons[f][YPOS] > 64) {
				icons[f][XPOS] = random(128);
				icons[f][YPOS] = 0;
				icons[f][DELTAY] = random(5) + 1;
      }
    }
  }
}


void testdrawchar(void) {
  for (uint8_t i=0; i < 168; i++) {
    glcd.drawchar((i % 21) * 6, i/21, i);
  }
}

void testdrawcircle(void) {
  for (uint8_t i=0; i<64; i+=2) {
    glcd.drawcircle(63, 31, i, BLACK);
  }
}


void testdrawrect(void) {
  for (uint8_t i=0; i<64; i+=2) {
    glcd.drawrect(i, i, 128-i, 64-i, BLACK);
  }
}

void testfillrect(void) {
  for (uint8_t i=0; i<64; i++) {
      // alternate colors for moire effect
    glcd.fillrect(i, i, 128-i, 64-i, i%2);
  }
}

void testdrawline() {
  for (uint8_t i=0; i<128; i+=4) {
    glcd.drawline(0, 0, i, 63, BLACK);
  }
  for (uint8_t i=0; i<64; i+=4) {
    glcd.drawline(0, 0, 127, i, BLACK);
  }

  glcd.display();
  delay(1000);

  for (uint8_t i=0; i<128; i+=4) {
    glcd.drawline(i, 63, 0, 0, WHITE);
  }
  for (uint8_t i=0; i<64; i+=4) {
    glcd.drawline(127, i, 0, 0, WHITE);
  }
}
