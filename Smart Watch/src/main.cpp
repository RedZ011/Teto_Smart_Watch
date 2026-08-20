#include <Arduino.h>
#include <Wire.h>
#include <SPI.h>
#include <Adafruit_SSD1306.h>
#include <WiFi.h>
#include <Minecraft10pt7b.h>
#include <Minecraft7pt7b.h>
#include <WiFiMulti.h>
#include <Teto.h>
#include <MusicPlayer.h>
//Screen
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1

//Pins
#define SCK  4
#define MOSI  6
#define RES  7
#define DC  8
#define CS  10
const int Button = 3;

//Wifi
const char* SSID = "Folk";
const char* PASS = "abdo0100";

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &SPI, DC, RES, CS);

struct tm timeinfo;

 WiFiMulti wifiMulti;

//+++++++++++++++++++++++++++++++++++++++++ Standard Variables ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
int hours = 12 ;
int minutes = 59;
int seconds = 50;
int day = 18;
int month = 8;
int year = 2026;
bool ButtonState = 0;
bool isPM = 0;
bool quote = 1;
bool TetoFrame = 0;
bool LastButtonState = 1;
bool DisplayState = 0;
unsigned long LastClock = 0;
unsigned long LastTeto = 0;
unsigned long ClearDisplayClock = 0;
unsigned long ButtonClock = 0;
unsigned long pressStartTime = 0;
char PMStr[3] = "PM";
char timeStr[8] = "12:00PM";
; 



//+++++++++++++++++++++++++++++++++++++++++ Functions ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void UpdateDate(int& day, int& month, int& year) {
  // Increment the day
  day++;

  // Check if the day exceeds the number of days in the current month
  if ((month == 1 || month == 3 || month == 5 || month == 7 || month == 8 || month == 10 || month == 12) && day > 31) {
    day = 1;
    month++;
  } else if ((month == 4 || month == 6 || month == 9 || month == 11) && day > 30) {
    day = 1;
    month++;
  } else if (month == 2) {
    // Check for leap year
    bool isLeapYear = (year % 4 == 0 && year % 100 != 0) || (year % 400 == 0);
    int daysInFebruary = isLeapYear ? 29 : 28;
    if (day > daysInFebruary) {
      day = 1;
      month++;
    }
  }

  // Check if the month exceeds December
  if (month > 12) {
    month = 1;
    year++;
  }
}


void UpdateAMPM(bool& isPM, int& hours) {
  static int LastHour = 0; 
  if (hours == 12 && LastHour != 12) { // Check if the hour is 12 and it wasn't 12 in the last update
    isPM = !isPM; // Toggle AM/PM
  }
  LastHour = hours; // Update LastHour to the current hour
}

void UpdateDay(int hours , char* PMStr){
  static int LastHour = 0;
  if (hours == 12 && LastHour != 12) { // Check if the hour is 12 and it wasn't 12 in the last update
    if (isPM) {
      strcpy(PMStr, "PM");
    } else {
      strcpy(PMStr, "AM");
      UpdateDate(day, month, year); // Increment the date when transitioning from 11:59 PM to 12:00 AM
    }
    LastHour = hours; // Update LastHour to the current hour
  }
}

void UpdateTime(int& seconds , int& minutes, int& hours) {
 
  seconds++;
  
  if (seconds >= 60) {
    seconds = 0;
    minutes++;
    if (minutes >= 60) {
      minutes = 0;
      hours++;
      if (hours > 12) {
        hours = 1; // Reset hours to 1 after 12
      }
    }
  }
  
  UpdateAMPM(isPM, hours);

  UpdateDay(hours, PMStr);

}

void DisplayDate(int day, int month, int year) {
  display.setFont(&Minecraft7pt7b);
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0,10);
  display.printf("%02d/%02d/%04d", day, month, year);
}


void DisplayTime(char* timeStr) {

  display.setFont(&Minecraft10pt7b);
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(30, 30);
  display.println(timeStr);

}

void DisplayTeto(bool TetoFrame) {



    const unsigned char* current_frame = (const unsigned char*)pgm_read_ptr(&(MAIN_ARR[(int)TetoFrame]));
   
    if(TetoFrame == 0)
    display.drawBitmap(54, 36, current_frame, 30, 31, SSD1306_WHITE);
    else
    display.drawBitmap(54, 38, current_frame, 30, 31, SSD1306_WHITE);
  display.display();
  }


  void DisplayOn(bool ButtonPressed) {
      if (ButtonPressed) 
          display.ssd1306_command(SSD1306_DISPLAYON); // Turn on the display
       else 
         display.ssd1306_command(SSD1306_DISPLAYOFF); // Turn off the display
  }

  void ButtonPress(bool ButtonState , bool& LastButtonState) {

   if(ButtonState == HIGH && LastButtonState == LOW){
   pressStartTime = millis(); // Record the time when the button is pressed  
  }
  
  if(ButtonState == LOW && LastButtonState == HIGH) {
    unsigned long pressDuration = millis() - pressStartTime; // Calculate the duration of the button press
    if (pressDuration >= 2000) { // Check if the button was pressed for 2 seconds or more
     MusicPlayer_Draw();
    } 
    else if(pressDuration >= 500) { // Short press
     DisplayOn(DisplayState); // Short press, turn on the display
      DisplayState = !DisplayState; // Toggle the display state for the next press
    }

  }
  LastButtonState = ButtonState; // Update the last button state for the next loop iteration
  }


//++++++++++++++++++++++++++++++++++++++++++ Setup and Loop ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
  void setup() {
  Serial.begin(115200);

  
 if (!display.begin(SSD1306_SWITCHCAPVCC)) {
    Serial.println("SSD1306 allocation failed");
    while (1);
  }
  /*
  //Wifi Setup.

  WiFi.begin(SSID, PASS);
  
  display.clearDisplay();
  display.setTextSize(1);           
  display.setTextColor(SSD1306_WHITE);      
  display.setCursor(0,0); 
  display.println("Connecting to WiFi...");
  display.display();



  Serial.print("Connected to WiFi network\n");
  

  display.println("Connected to:");
  display.println(SSID);
  display.display();
*/

 pinMode(Button, INPUT_PULLUP); // Set the button pin as input with internal pull-up resistor



display.clearDisplay();

  
// Format the time as a string
  sprintf(timeStr, "%02d:%02d%s", hours, minutes, PMStr);

// display code
DisplayTime(timeStr);

DisplayDate(day, month, year);

display.display();

}



void loop() {

    unsigned long now = millis();

    ButtonState = digitalRead(Button); // Read the button state

    
    ButtonPress(ButtonState, LastButtonState); // Handle button press logic


    if(now - ClearDisplayClock >= 1000){ // Clear the display every 1 second
      ClearDisplayClock = now;
    display.clearDisplay();
    }

// Update the time every second

 if(now - LastClock >= 1000){
    LastClock = now;
    //Update the time variables
    UpdateTime(seconds, minutes, hours);

// print the time Variables
if(quote){
   sprintf(timeStr, "%02d:%02d%s", hours, minutes, PMStr);
quote = 0;
}
else{
   sprintf(timeStr, "%02d %02d%s", hours, minutes, PMStr);
  quote = 1;
}
  }





//Display Teto Function
if(now - LastTeto >= 1000){ // Update Teto every 1 seconds
  LastTeto = now;
  TetoFrame = !TetoFrame; // Toggle Teto frame for next update
}

DisplayTime(timeStr);

DisplayTeto(TetoFrame);

DisplayDate(day, month, year);

display.display();

}