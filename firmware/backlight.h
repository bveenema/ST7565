/*	backlight.h
/*	Written by Veen - Veenema Design Works
/*
/*	This library mirrors the photon RGB LED to any other RGB but was written to
/*  work with the backlight of the Adafruit ST7565 w/ RGB backlight LCD
/*
/*  Influenced by Particle Docs and LEDFader library by jeremy (jgillick)
/*-------------------------------------------------------------------------*/

#ifndef backlight_H
#define backlight_H

//What is the resolution of the PWM
#ifndef PWMBITS
#define PWMBITS 255
#endif

//Is the LED common annode (RGBONDIR = 0) or common cathode (RGBONDIR = 1)
#ifndef RGBONDIR
#define RGBONDIR 0
#endif

// The minimum time (milliseconds) the program will wait between LED adjustments
// adjust this to modify performance.
#ifndef MIN_INTERVAL
#define MIN_INTERVAL 20
#endif

#include "application.h"

class backlight
{
public:
	backlight(pin_t redpin, pin_t greenpin, pin_t bluepin);
	void setup(pin_t redpin, pin_t greenpin, pin_t bluepin);
	bool setColor(uint8_t red, uint8_t green, uint8_t blue);
  bool setBrightness(uint8_t brightness);
	bool fadeTo(uint8_t brightness, uint16_t t);
  bool breathe(uint8_t minBrightness, uint8_t maxBrightness, uint16_t t);
  bool isFading();
  void update();
  void handler(uint8_t r, uint8_t g, uint8_t b);

private:
	pin_t _redpin;
	pin_t _greenpin;
	pin_t _bluepin;
	uint8_t _red = 0;
	uint8_t _green = 0;
	uint8_t _blue = 0;
  uint8_t _currentBrightness = 0;
  uint8_t _newBrightness = 0;
  uint8_t _minBrightness = 0;
  uint8_t _maxBrightness = 0;
  uint16_t _interval = 0;
  int32_t _duration = 0;
  int32_t _breatheDuration = 0;
  bool _breathe = 0;
  bool _breatheUp = 0;
  float _percentDone = 0;
	uint32_t _lastfadeTime = 0;
};
#endif
