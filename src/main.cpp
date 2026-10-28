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
#include <BleKeyboard.h>
#include <Bluetooth.h>
#include <Bye.h>
#include <NimBLEDevice.h>

//Screen
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1


const int Button = 2;

//Wifi
const char* SSID = "Folk";
const char* PASS = "abdo0100";

BleKeyboard Player("Teto Watch"); //Bluetooth Name
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

struct tm timeinfo;

 WiFiMulti wifiMulti;

//+++++++++++++++++++++++++++++++++++++++++ Standard Variables ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
int hours = 12 ;
int minutes = 59;
int seconds = 50;
int day = 18;
int month = 8;
int year = 2026;
int TetoWalkFrame = 0;
bool ButtonState = 0;
bool isPM = 0; // Change this if you want to change the PM or AM not the actual string
bool quote = 1;
bool TetoFrame = 0;
bool LastButtonState = 1;
bool DisplayState = 1;
bool DisplayOnState = 1;
bool byeFrame = 0;
bool LongPressFlag = 0;
unsigned long LastClock = 0;
unsigned long LastTeto = 0;
unsigned long LastByeFrame = 0;
unsigned long ClearDisplayClock = 0;
unsigned long ButtonClock = 0;
unsigned long pressStartTime = 0;
unsigned long now ;
char PMStr[3] = "PM";
char timeStr[8] = "12:00PM";




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
  }
      LastHour = hours; // Update LastHour to the current hour

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
  display.setFont();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0,1);
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


  void DisplayOn(bool State) {
      if (State){
             while (now - LastClock >= 1000) {

          LastClock += 1000;      // we restore the lost 3 seconds on the animation

          UpdateTime(seconds, minutes, hours);

        }

        display.ssd1306_command(SSD1306_DISPLAYON); // Turn on the display

        unsigned long startTime = millis(); // Record the start time
        
        while (millis() - startTime <= 3000) {
         
          unsigned long currentTime = millis();
          if (currentTime - LastByeFrame >= 500) { // Update every 500 milliseconds
            LastByeFrame = currentTime;
            byeFrame = !byeFrame; // Toggle the frame
            const unsigned char* current_frame = (const unsigned char*)pgm_read_ptr(&(Main_Bye[(int)byeFrame]));
            display.clearDisplay();
            display.drawBitmap(20, 0, current_frame, 64, 64, SSD1306_WHITE);
            display.setFont();
            display.setCursor(90, 20);
            display.println("Hi!!");
            display.display();
          }

        }



      } 
       else{

        unsigned long startTime = millis(); // Record the start time
        
        while (millis() - startTime <= 3000) {
         
          unsigned long currentTime = millis();
          if (currentTime - LastByeFrame >= 500) { // Update every 500 milliseconds
            LastByeFrame = currentTime;
            byeFrame = !byeFrame; // Toggle the frame
            const unsigned char* current_frame = (const unsigned char*)pgm_read_ptr(&(Main_Bye[(int)byeFrame]));
            display.clearDisplay();
            display.drawBitmap(20, 0, current_frame, 64, 64, SSD1306_WHITE);
            display.setFont();
            display.setCursor(90, 20);
            display.println("Bye!!");
            display.display();
          }

        }

        display.ssd1306_command(SSD1306_DISPLAYOFF); // Turn off the display
        while (now - LastClock >= 1000) {

          LastClock += 1000;      // we restore the lost 3 seconds on the animation

          UpdateTime(seconds, minutes, hours);

        }

       }
      
      
      }
        void BluetoothState(bool State){
      if(!State){ // if the bluetooth is disconnected we will disconnect all the connected devices and stop advertising
        NimBLEServer* server = NimBLEDevice::getServer();
if (server && server->getConnectedCount() > 0) {

  std::vector<uint16_t> peers = server->getPeerDevices();
  for (auto id : peers) server->disconnect(id);   // kick the phone off

}
NimBLEDevice::getAdvertising()->stop();           // stop being discoverable
      }
      else
      NimBLEDevice::getAdvertising()->start(); // start being discoverable
    }

  
    void SleepMode(){
      if(!DisplayOnState){
        //Turn off the bluetooth
        BluetoothState(false);
        //Clock Speed 80Mhz
        setCpuFrequencyMhz(80);
      }
      else 
      {
        //Turn on the bluetooth
        BluetoothState(true);
        //Clock Speed 160Mhz
        setCpuFrequencyMhz(160);
        }
    }
  

  void ButtonPress(bool ButtonState , bool& LastButtonState) {

   if(ButtonState == LOW && LastButtonState == HIGH){

    pressStartTime = millis(); // Record the time when the button is pressed  
    LongPressFlag = false;  // Reset the long press flag when the button is pressed

  }

   // LongPressFlag is used to turn the display on or off for once 

   if (ButtonState == LOW && !LongPressFlag && millis() - pressStartTime >= 4000) { // we will see if the button is still held and if the flag is still false and if the button is held for 5 seconds
    LongPressFlag = true;      
    DisplayOnState = !DisplayOnState;
    DisplayOn(DisplayOnState);

    if (DisplayOnState)  SleepMode();

    if (!DisplayOnState) SleepMode();   
  }
    
  if(ButtonState == HIGH && LastButtonState == LOW) {
    unsigned long pressDuration = millis() - pressStartTime; // Calculate the duration of the button press
   
  if (!LongPressFlag) { // Only process short presses if the long press flag is not set

   if (pressDuration >= 1000) { // Check if the button was pressed for 1.5 seconds or more
      
      DisplayState = !DisplayState; // Toggle the display state

      display.clearDisplay(); // Clear the display when toggling
    } 


  }
  }
    LastButtonState = ButtonState; // Update the last button state for the next loop iteration

}



//++++++++++++++++++++++++++++++++++++++++++ Setup and Loop ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
  void setup() {
    Serial.begin(115200);
    Player.begin();
    Wire.begin(8,9);
    Wire.setClock(400000);   


  
 if (!display.begin(SSD1306_SWITCHCAPVCC,0x3C)) {
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

     now = millis();

    ButtonState = digitalRead(Button); // Read the button state

    if(DisplayState)
    ButtonPress(ButtonState, LastButtonState); // Handle button press logic for the Clock Display Only
    

    if(now - ClearDisplayClock >= 1000 && DisplayOnState){  // Clear the display every 1 second
      ClearDisplayClock = now;
    display.clearDisplay();
    }

// Update the time every second

 if(now - LastClock >= 1000){
    LastClock = now;
    //Update the time variables
    UpdateTime(seconds, minutes, hours);

// print the time Variables
if(quote && DisplayOnState){
   sprintf(timeStr, "%02d:%02d%s", hours, minutes, PMStr);
quote = 0;
}
else{
   sprintf(timeStr, "%02d %02d%s", hours, minutes, PMStr);
  quote = 1;
}
  }





//Display Teto Function
if(now - LastTeto >= 1000 && DisplayOnState){ // Update Teto every 1 seconds
  LastTeto = now;
  TetoFrame = !TetoFrame; // Toggle Teto frame for next update
}

if(DisplayState){
  if(DisplayOnState){
DisplayTime(timeStr);
DisplayTeto(TetoFrame);
DisplayDate(day, month, year);
// notifacation
if(Player.isConnected())
display.drawBitmap(115,0,bluetooth,10,10,SSD1306_WHITE);
if(PauseState && Player.isConnected())
display.drawBitmap(100,0,M_icon,10,10,SSD1306_WHITE); // display the music playing icon 
}
}


else{
  if(DisplayOnState)
     MusicPlayer_Draw();
}

if(DisplayOnState)
display.display();

if(!DisplayOnState)
delay(10); // Delay for 10 milliseconds to reduce CPU usage when the display is off
}