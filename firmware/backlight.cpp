/*	backlight.h
/*	Written by Veen - Veenema Design Works
/*
/*	This library mirrors the photon RGB LED to any other RGB but was written to
/*  work with the backlight of the Adafruit ST7565 w/ RGB backlight LCD
/*
/*  Influenced by Particle Docs and LEDFader library by jeremy (jgillick)
/*-------------------------------------------------------------------------*/

#include "backlight.h"

backlight::backlight(pin_t redpin, pin_t greenpin, pin_t bluepin){
  setup(redpin, greenpin, bluepin);
}
void backlight::setup(pin_t redpin, pin_t greenpin, pin_t bluepin){
  _redpin = redpin;
  _greenpin = greenpin;
  _bluepin = bluepin;

  pinMode(_redpin, OUTPUT);
  pinMode(_greenpin, OUTPUT);
  pinMode(_bluepin, OUTPUT);

  RGB.onChange(&backlight::handler, this);

}
bool backlight::setColor(uint8_t red, uint8_t green, uint8_t blue){
  //RGB is not under User control
  if(!RGB.controlled()){
    return 1;
  }
  RGB.color(red,green,blue);
  return 0;
}

bool backlight::setBrightness(uint8_t brightness){
  //RGB is not under User control
  if(!RGB.controlled()){
    return 1;
  }
  RGB.brightness(brightness);
  _currentBrightness = brightness;
  return 0;
}

bool backlight::isFading(){
  //RGB is not under User control
  if(!RGB.controlled()){
    return 0;
  }
  if(_duration > 0){
    return 1;
  }
  return 0;
}

bool backlight::fadeTo(uint8_t brightness, uint16_t t){
  //RGB is not under User control
  if(!RGB.controlled()){
    return 1;
  }
  if(t<MIN_INTERVAL){
    RGB.brightness(brightness);
    return 0;
  }
  _newBrightness = brightness;
  _duration = t;
  _breathe = 0;
  //Compute interval
  uint16_t brightnessDiff = abs(_currentBrightness - _newBrightness);
  _interval = _duration/brightnessDiff;
  if(_interval < MIN_INTERVAL){
    _interval = MIN_INTERVAL;
  }
  _lastfadeTime = millis();
  return 0;
}

bool backlight::breathe(uint8_t minBrightness, uint8_t maxBrightness, uint16_t t){
  //RGB is not under User control
  if(!RGB.controlled()){
    return 1;
  }
  _minBrightness = minBrightness;
  _maxBrightness = maxBrightness;
  _breatheDuration = t;
  _duration = t;
  _breathe = 1;

  //set Brightness within MIN/Max Boundary
  constrain(_currentBrightness,_minBrightness,_maxBrightness);
  setBrightness(_currentBrightness);

  //Fade towards furthest boundary
  uint8_t minDistance = abs(_currentBrightness - _minBrightness);
  uint8_t maxDistance = abs(_currentBrightness - _maxBrightness);
  if(maxDistance>minDistance){
    _breatheUp = 1;
    _newBrightness = _maxBrightness;
  }else{
    _breatheUp = 0;
    _newBrightness = _minBrightness;
  }

  //Compute interval
  uint16_t brightnessDiff = abs(_currentBrightness - _newBrightness);
  _interval = _breatheDuration/brightnessDiff;
  if(_interval < MIN_INTERVAL){
    _interval = MIN_INTERVAL;
  }
  _lastfadeTime = millis();

  return 0;
}

void  backlight::update(){
  //RGB is not under User control
  if(!RGB.controlled()){
    return;
  }

  //No Fade or breathe
  if(_duration == 0){
    return;
  }

  uint16_t timeElapsed = millis()-_lastfadeTime;

  //Interval hasn't passed yet
  if(timeElapsed < _interval){
    return;
  }

  // How far along have we gone since last update
  float percent = (float)timeElapsed / (float)_duration;
  _percentDone += percent;

 // We've hit 100%, set LED to the final color or swith breath direction
 if (percent >= 1) {
   if(_breathe){
     _duration = _breatheDuration;
     _percentDone = 0;
     if(_breatheUp){
       _newBrightness = _minBrightness;
       _breatheUp = 0;
     }else{
       _newBrightness = _maxBrightness;
       _breatheUp = 1;
     }
     return;
   }else{
     _duration = 0;
     _percentDone = 100;
     setBrightness(_newBrightness);
     return;
   }
  }
  // Move color to where it should be
  int fade_diff = _newBrightness - _currentBrightness;
  int increment = round(fade_diff * percent);

  setBrightness(_currentBrightness + increment);

  // Update time and finish
  _duration -= timeElapsed;
  _lastfadeTime = millis();
  return;
}

void backlight::handler(uint8_t r, uint8_t g, uint8_t b){
  analogWrite(_redpin, 255 - r);
  analogWrite(_greenpin, 255 - g);
  analogWrite(_bluepin, 255 - b);
}
