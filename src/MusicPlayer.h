#ifndef MUSIC_PLAYER_H
#define MUSIC_PLAYER_H

#include <Arduino.h>
#include <Adafruit_SSD1306.h>
#include <BleKeyboard.h>
#include <MusicIcon.h>


extern Adafruit_SSD1306 display;   // "this exists elsewhere, I just want access"
extern BleKeyboard Player; //Bluetooth Name
extern bool DisplayState; 
extern bool ButtonState; 
extern bool LastButtonState; 
extern bool PauseState;

void MusicPlayer_Init();
void MusicPlayer_HandleButton(int tapCount);
void MusicPlayer_Draw();

#endif