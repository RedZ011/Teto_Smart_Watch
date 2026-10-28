#include "MusicPlayer.h"
#include <Arduino.h>
#include <Next.h>
#include <Back.h>
#include <Play.h>
#include <Pause.h>
#include <Bluetooth.h>
#include <BigTeto.h>
#include <MusicPlayerIcons.h>



int tapCount = 0; // Variable to count the number of taps
bool PauseState = false; 
bool NextPressed = false;
bool BackPressed = false;
bool LastPauseState = false;
bool LastNextState = false;
bool LastBackState = false;
bool BigTetoFrame = false;
unsigned long Duration = 0;
unsigned long TetoDuration = 0;
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

void DisplayTeto2(){
    const unsigned char* current_frame = (const unsigned char*)pgm_read_ptr(&(TETO_ARR[(int)BigTetoFrame]));

  if(!BigTetoFrame){
  display.drawBitmap(40,-5,current_frame,50,50,SSD1306_WHITE);
}
else{
  display.drawBitmap(40,0,current_frame,50,50,SSD1306_WHITE);
}
}




void Display_M_icons(){
  if(BigTetoFrame){
    //left
  display.drawBitmap(35,10,musicnote1,10,10,SSD1306_WHITE); //top
  display.drawBitmap(30,25,musicnote2,10,10,SSD1306_WHITE);//bottom
  display.drawBitmap(22,18,musicnote1,10,10,SSD1306_WHITE);//middle
  //right
  display.drawBitmap(90,10,musicnote1,10,10,SSD1306_WHITE);//top
  display.drawBitmap(90,25,musicnote2,10,10,SSD1306_WHITE);//bottom
  display.drawBitmap(98,15,musicnote1,10,10,SSD1306_WHITE);//middle
  }
  else{
    //left
  display.drawBitmap(35,15,musicnote1,10,10,SSD1306_WHITE); //top
  display.drawBitmap(30,30,musicnote2,10,10,SSD1306_WHITE);//bottom
  display.drawBitmap(22,23,musicnote1,10,10,SSD1306_WHITE);//middle
  //right
  display.drawBitmap(90,15,musicnote1,10,10,SSD1306_WHITE);//top
  display.drawBitmap(90,30,musicnote2,10,10,SSD1306_WHITE);//bottom
  display.drawBitmap(98,20,musicnote1,10,10,SSD1306_WHITE);//middle
  }

  
}


void MusicPlayer_Draw() {
  now1 = millis();
  // actual drawing code here
display.clearDisplay();


if(Player.isConnected() && PauseState){  // move teto if there is connection and the music is playing

if(now1 - TetoDuration >= 1000){
  TetoDuration = now1;
  BigTetoFrame = !BigTetoFrame;


}

display.drawBitmap(100,0,M_icon,10,10,SSD1306_WHITE); // display the music playing icon

Display_M_icons();

}
else
  BigTetoFrame = false; // don't move teto if there is no connection




if(Player.isConnected())
display.drawBitmap(115,0,bluetooth,10,10,SSD1306_WHITE); // show the bluetooth icon if connected to bluetooth

if (now1 - Duration >= 1000 )
{
  
Duration = now1;




MusicPlayer_HandleButton(tapCount);   // Handle the Taps

tapCount = 0;


}
DisplayButtons(); //display the player buttons
DisplayTeto2(); // display the teto frames



if(!DisplayState)
M_ButtonPress(ButtonState, LastButtonState); // Handle button press logic for the Music Player Display Only


display.display();

}