#ifndef MUSIC_PLAYER_H
#define MUSIC_PLAYER_H

#include <Arduino.h>

void MusicPlayer_Init();
void MusicPlayer_HandleButton(int tapCount);
void MusicPlayer_Draw();

#endif