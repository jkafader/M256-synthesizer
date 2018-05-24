/***************************************************
  This is a test example for the Adafruit Trellis w/HT16K33

  Designed specifically to work with the Adafruit Trellis
  ----> https://www.adafruit.com/products/1616
  ----> https://www.adafruit.com/products/1611

  These displays use I2C to communicate, 2 pins are required to
  interface
  Adafruit invests time and resources providing this open source code,
  please support Adafruit and open-source hardware by purchasing
  products from Adafruit!

  Written by Limor Fried/Ladyada for Adafruit Industries.
  MIT license, all text above must be included in any redistribution
 ****************************************************/

#include <Wire.h>                  // I2C Master lib for ATTinys which use USI
#include "Adafruit_Trellis.h"

/***************************************************
  This example shows reading buttons and setting/clearing buttons in a loop
  "momentary" mode has the LED light up only when a button is pressed
  "latching" mode lets you turn the LED on/off when pressed

  Up to 8 matrices can be used but this example will show 4 or 1
 ****************************************************/

#define MOMENTARY 0
#define LATCHING 1
// set the mode here
#define MODE LATCHING


Adafruit_Trellis matrix0 = Adafruit_Trellis();
Adafruit_Trellis matrix1 = Adafruit_Trellis();
Adafruit_Trellis matrix2 = Adafruit_Trellis();
Adafruit_Trellis matrix3 = Adafruit_Trellis();

Adafruit_Trellis matrix4 = Adafruit_Trellis();
Adafruit_Trellis matrix5 = Adafruit_Trellis();
Adafruit_Trellis matrix6 = Adafruit_Trellis();
Adafruit_Trellis matrix7 = Adafruit_Trellis();

Adafruit_Trellis matrix8 = Adafruit_Trellis();
Adafruit_Trellis matrix9 = Adafruit_Trellis();
Adafruit_Trellis matrixA = Adafruit_Trellis();
Adafruit_Trellis matrixB = Adafruit_Trellis();

Adafruit_Trellis matrixC = Adafruit_Trellis();
Adafruit_Trellis matrixD = Adafruit_Trellis();
Adafruit_Trellis matrixE = Adafruit_Trellis();
Adafruit_Trellis matrixF = Adafruit_Trellis();

// uncomment the below to add 3 more matrices
/*
  // you can add another 4, up to 8
*/

// Just one
//Adafruit_TrellisSet trellis =  Adafruit_TrellisSet(&matrix0);
// or use the below to select 4, up to 8 can be passed in
Adafruit_TrellisSet trellis[4] =  {Adafruit_TrellisSet(&matrix0, &matrix1, &matrix2, &matrix3),
  Adafruit_TrellisSet(&matrix4, &matrix5, &matrix6, &matrix7),
  Adafruit_TrellisSet(&matrix8, &matrix9, &matrixA, &matrixB),
  Adafruit_TrellisSet(&matrixC, &matrixD, &matrixE, &matrixF)};

// set to however many you're working with here, up to 8
#define NUMTRELLIS 16

#define numKeys (NUMTRELLIS * 16)

// Connect Trellis Vin to 5V and Ground to ground.
// Connect the INT wire to pin #A2 (can change later!)
#define INTPIN 8
// Connect I2C SDA pin to your Arduino SDA line
// Connect I2C SCL pin to your Arduino SCL line
// All Trellises share the SDA, SCL and INT pin!
// Even 8 tiles use only 3 wires max

#define MUXADDR 0x70
 
void muxselect(uint8_t k) {
  if (k > 7) return;
 
  Wire.beginTransmission(MUXADDR);
  Wire.write(1 << k);
  Wire.endTransmission();  
}

void setup() {
  // Serial.begin(9600);
  // Serial.println("Trellis Demo");

  // INT pin requires a pullup
  pinMode(INTPIN, INPUT);
  digitalWrite(INTPIN, HIGH);

  // begin() with the addresses of each panel in order
  // I find it easiest if the addresses are in order

  for(int j=0; j<4; j++){
    muxselect(j);
    trellis[j].begin(0x74, 0x75, 0x76, 0x77);  // or four!
    // light up all the LEDs in order
    for (uint8_t i = 0; i < 64; i++) {
      trellis[j].setLED(i);
      if((i + 1) % 16 == 0){
        trellis[j].writeDisplay();
        delay(30);
      }
    }
  }
  delay(1000);
  for(int j=0; j<4; j++){
    muxselect(j);
    trellis[j].begin(0x74, 0x75, 0x76, 0x77);  // or four!
    // then turn them off
    for (uint8_t i = 0; i < 64; i++) {
      trellis[j].clrLED(i);
      if((i + 1) % 16 == 0){
        trellis[j].writeDisplay();
        delay(30);
      }
    }
  }
}


void loop() {
  delay(20); // 30ms delay is required, dont remove me!

  for(int j=0; j<4; j++){
    muxselect(j);
    trellis[j].begin(0x74, 0x75, 0x76, 0x77);  // or four!
    // If a button was just pressed or released...
    if (trellis[j].readSwitches()) {
      // go through every button
      for (uint8_t i = 0; i < 64; i++) {
        // if it was pressed...
        if (trellis[j].justPressed(i)) {
          //Serial.print("v"); Serial.println(i);
          // Alternate the LED
          if (trellis[j].isLED(i))
            trellis[j].clrLED(i);
          else
            trellis[j].setLED(i);
        }
      }
      // tell the trellis to set the LEDs we requested
      trellis[j].writeDisplay();
    }
  }
}

