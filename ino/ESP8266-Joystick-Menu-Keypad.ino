/*
    Dev: Hudson Miranda
    30/08/2020
*/

#include "Keypad.h"

// Define matrix keypad
const byte ROWS = 4; //four rows
const byte COLS = 3; //four columns

char keys[ROWS][COLS] = {
  {'1','2','3'},
  {'4','5','6'},
  {'7','8','9'},
  {'*','0','#'}
};

// For ESP8266 Microcontroller
//byte rowPins[ROWS] = {D1, D2, D3, D4}; 
//byte colPins[COLS] = {D5, D6, D7, D8}; 

// For ESP32 Microcontroller
byte rowPins[ROWS] = {23, 22, 3, 21}; 
byte colPins[COLS] = {19, 18, 5};

Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

void setup() {
  Serial.begin(115200);
}

void loop() {

  char key = keypad.getKey();

  if (key){
    Serial.println(key);
  }
    
  if(analogRead(12)>2000){
      Serial.println("TOP");
  }
  if(analogRead(12)<1000){
      Serial.println("BOTTOM");
  }
  if(analogRead(13)>2000){
      Serial.println("RIGHT");
  }
  if(analogRead(13)>1000){
      Serial.println("LEFT");
  }
  if(analogRead(14)==0){
      Serial.println("BUTTON PRESSED");
  }
    
//   Serial.println(analogRead(12));
//   Serial.println(analogRead(13));
//   Serial.println(analogRead(14));
//   Serial.println("- - - - - - - - - - - - - -");
  delay(1000);
}
