#include "MusicPlayer.h"
#include <Arduino.h>
#include <Next.h>
#include <Back.h>
# include <Play.h>
#include <Pause.h>
#include <Bluetooth.h>

int tapCount = 0; // Variable to count the number of taps
bool PauseState = false; 
bool NextPressed = false;
bool BackPressed = false;
bool LastPauseState = false;
bool LastNextState = false;
bool LastBackState = false;
unsigned long Duration = 0;
unsigned long PauseDuration = 0;
unsigned long BackDuration = 0;
unsigned long NextDuration = 0;
unsigned long PressStart = 0;
unsigned long now1 = 0;





  void M_ButtonPress(bool ButtonState , bool& LastButtonState) {

   if(ButtonState == LOW && LastButtonState == HIGH){
  
    PressStart = now1; // Record the time when the button is pressed  
  
  }
  
  if(ButtonState == HIGH && LastButtonState == LOW) {

    unsigned long pressDuration = now1 - PressStart; // Calculate the duration of the button press
    
    if (pressDuration >= 1000) { // Check if the button was pressed for 1 second or more
      

      DisplayState = !DisplayState; // Toggle the display state
      
      display.clearDisplay(); // Clear the display when toggling


    } 
    
    else if(pressDuration >= 60) { // Short press
    
      tapCount++;

      if (tapCount > 3) 

        tapCount = 0; // Reset tap count if it exceeds the number of available actions
      
    
    }

 

  }

     if(tapCount > 1 &&  ( now1 - PressStart ) > 1000) { // If the counter is more than 1 and 1 second has passed since the last tap Reset the counter 
      PressStart = now1;
      tapCount = 0; 
    }

  LastButtonState = ButtonState; // Update the last button state for the next loop iteration

}



void PlayOrPause(){  

  if(PauseState)
  {

    if(PauseState != LastPauseState){
       PauseDuration = now1;     
    }
     
    if(now1 - PauseDuration <= 500)   
    
      display.drawBitmap(55, 44, Pause_Pressed, 19, 21, SSD1306_WHITE);
    
     else
      display.drawBitmap(55, 44, Pause, 19, 21, SSD1306_WHITE);
 

  }
    
    
     else  
  {

   if(PauseState != LastPauseState)
    PauseDuration = now1;
   

   if(now1 - PauseDuration <= 500 )
  display.drawBitmap(55, 44, Play_Pressed, 19, 21, SSD1306_WHITE);


  else
  display.drawBitmap(55, 44, Play, 19, 21, SSD1306_WHITE);

  }
  
     LastPauseState = PauseState;




}
void NextButton(){


  
if(LastNextState != NextPressed)
    NextDuration = now1;

if(now1 - NextDuration <= 500)
  display.drawBitmap(80, 44, Next_Pressed, 19, 21, SSD1306_WHITE);
   
else

  display.drawBitmap(80, 44, Next, 19, 21, SSD1306_WHITE);

  
  
  LastNextState = NextPressed;
  
}

void BackButton(){


  if(LastBackState != BackPressed)
   BackDuration = now1;

if(now1 - BackDuration <= 500)
  display.drawBitmap(30, 44, Back_Pressed, 19, 21, SSD1306_WHITE);
   
else

  display.drawBitmap(30, 44, Back, 19, 21, SSD1306_WHITE);

  
  
  LastBackState = BackPressed;
}


void MusicPlayer_Init() {
// actual setup code here

  
}

void MusicPlayer_HandleButton(int tapCount) {
  // actual logic here, using tapCount (1=play, 2=next, 3=back)

  switch (tapCount)
  {
  case 1:

      if (Player.isConnected()) {

    if(!PauseState)
    Player.write(KEY_MEDIA_PLAY);
    else
    Player.write(KEY_MEDIA_STOP);
    
    PauseState = !PauseState;

  }
  tapCount = 0;
    break;

    case 2:
   NextPressed = !NextPressed;

   if (Player.isConnected()) {
    Player.write(KEY_MEDIA_NEXT_TRACK);
  }
  tapCount = 0;
  break;


  case 3:
    BackPressed = !BackPressed;
      if (Player.isConnected()) {
    Player.write(KEY_MEDIA_PREVIOUS_TRACK);
  }
  tapCount = 0;
  break;

  
  default:
    break;
  }



  }



void DisplayButtons(){
  PlayOrPause();
  NextButton();
  BackButton();
}

void MusicPlayer_Draw() {
  now1 = millis();
  // actual drawing code here
display.clearDisplay();

display.drawBitmap(40,0,Teto3,50,50,SSD1306_WHITE);

if(Player.isConnected())
display.drawBitmap(115,0,bluetooth,10,10,SSD1306_WHITE);

if (now1 - Duration >= 1000 )
{
  
Duration = now1;




MusicPlayer_HandleButton(tapCount);     

tapCount = 0;


}

DisplayButtons();

if(!DisplayState)
M_ButtonPress(ButtonState, LastButtonState); // Handle button press logic for the Music Player Display Only

display.display();

}