/*************************************************** 
  Designed specifically to work with the Adafruit Trellis 
  ----> https://www.adafruit.com/products/1616
  ----> https://www.adafruit.com/products/1611
 ****************************************************/
#include <Wire.h>
#include "Adafruit_Trellis.h"
#include <SPI.h>

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

// uncomment the below to add 3 more matrices
/*
// you can add another 4, up to 8
*/


// set to however many you're working with here, up to 8
#define NUMTRELLIS 8
#define numKeys (NUMTRELLIS * 16)
// Connect Trellis Vin to 5V and Ground to ground.
// Connect the INT wire to pin #A2 (can change later!)

#define PCLK_PIN 9
#define OUTPUT_PINS 16
int update_required = 0;

void updateXpntArray(uint16_t * switches){           // writes all the bits to chip and pulses the latch clock, so all the switches change at the same moment.
    uint8_t row;
    uint16_t last_byte = 0;
    uint16_t mask = 0xFFFF;
    Serial.print("Crosspoint Matrix Data:\n");
    SPI.beginTransaction(SPISettings(1000000, LSBFIRST, SPI_MODE0));
    for(int row=0; row<16; row++){
       Serial.print("row: ");
       Serial.print(switches[row] & mask, BIN);
       last_byte = SPI.transfer16(0);
       Serial.print(" last value: ");
       Serial.print(last_byte, BIN);
       Serial.print("\n");
    }
    digitalWrite(PCLK_PIN, LOW);
    asm("nop"); // pclk pulse width 65ns minimum
    asm("nop");
    digitalWrite(PCLK_PIN, HIGH);
    asm("nop");
    asm("nop");
    SPI.endTransaction();
    update_required = 0;
}
/*
void updateXpntArray(uint16_t * switches)           // writes all the bits to all 3 chips and pulses the latch clock, so all the switches change at the same moment.
{
  uint8_t row;
  uint16_t rowBytes, mask;
  Serial.print("Crosspoint Matrix Data:\n");
  for (row = 0; row < OUTPUT_PINS; row++) {
    rowBytes = switches[row];
    // begin with 1000 0000 0000 0000
    // bit shift right until the bit "falls off" pins.
    // this will loop 16 times before stopping.
    for (mask = 0x8000; mask; mask >>= 1) {
      Serial.print((rowBytes & mask) ? 1 : 0, BIN);
      Serial.print(" ");
      digitalWrite(SIN_PIN, (rowBytes & mask) ? HIGH : LOW);  // SDI_PIN
      // 20ns setup required
      asm("nop"); // Each "nop" yields a 62.5 ns (nanosecond) delay
      asm("nop");
      digitalWrite(SCLK_PIN, HIGH);
      asm("nop"); // sclk pulse width, 100 ns minimum
      asm("nop");
      asm("nop");
      digitalWrite(SCLK_PIN, LOW);
      asm("nop"); // 40ns hold time required
    }
    Serial.print("\n");
  }
  asm("nop"); // 65ns setup required
  asm("nop");
  digitalWrite(PCLK_PIN, LOW);
  asm("nop"); // pclk pulse width 65ns minimum
  asm("nop");
  asm("nop");
  asm("nop");
  digitalWrite(PCLK_PIN, HIGH);
  asm("nop");
  asm("nop");
  asm("nop");
  update_required = 0;
  // Serial.print("--------");
}*/


int * getPinArray(){
  static int trellisButtons[256] = 
{ 0,   1,   2,   3,  16,  17,  18,  19,  32,  33,  34,  35,  48,  49,  50,  51,
  4,   5,   6,   7,  20,  21,  22,  23,  36,  37,  38,  39,  52,  53,  54,  55, 
  8,   9,  10,  11,  24,  25,  26,  27,  40,  41,  42,  43,  56,  57,  58,  59, 
 12,  13,  14,  15,  28,  29,  30,  31,  44,  45,  46,  47,  60,  61,  62,  63, 
 64,  65,  66,  67,  80,  81,  82,  83,  96,  97,  98,  99, 112, 113, 114, 115,
 68,  69,  70,  71,  84,  85,  86,  87, 100, 101, 102, 103, 116, 117, 118, 119,
 72,  73,  74,  75,  88,  89,  90,  91, 104, 105, 106, 107, 120, 121, 122, 123, 
 76,  77,  78,  79,  92,  93,  94,  95, 108, 109, 110, 111, 124, 125, 126, 127 };

  return trellisButtons;
}

// initialize an array of button numbers so we don't have to calculate them.
int * trellisButtons = getPinArray();

// or use the below to select 4, up to 8 can be passed in
Adafruit_TrellisSet trellis =  Adafruit_TrellisSet(&matrix0, &matrix1, &matrix2, &matrix3, &matrix4, &matrix5, &matrix6, &matrix7);

void disconnectAll(){
  static uint16_t switches[16] = { 0, 0, 0, 0,   0, 0, 0, 0,   0, 0, 0, 0,   0, 0, 0, 0 };
  updateXpntArray(switches);
}

void setup() {
  Serial.begin(9600);

  // INT pin requires a pullup
  //pinMode(INTPIN, INPUT);
  pinMode(PCLK_PIN, OUTPUT);
  //digitalWrite(INTPIN, HIGH);
  digitalWrite(PCLK_PIN, HIGH);
  
  // begin() with the addresses of each panel in order
  // I find it easiest if the addresses are in order
  trellis.begin(0x70, 0x71, 0x72, 0x73, 0x74, 0x75, 0x76, 0x77);  // eight

  // light up all the LEDs in order
  for (uint8_t i=0; i<numKeys; i++) {
    trellis.setLED(trellisButtons[i]);
    trellis.writeDisplay();
    delay(30);
  }
  // then turn them off
  for (uint8_t i=0; i<numKeys; i++) {
    trellis.clrLED(trellisButtons[i]);
    trellis.writeDisplay();
    delay(30);
  }
  for(int i=0; i<numKeys; i++){
     Serial.print(trellisButtons[i]);
     Serial.print(" ");
  }
  disconnectAll();
}


int * updateDisplay(int * trellisButtons){
    // go through every button
    static uint16_t rows[16] = { 0, 0, 0, 0,   0, 0, 0, 0,   0, 0, 0, 0,   0, 0, 0, 0 };
    for (int j=0; j<numKeys; j++){
        int row = j / 16;
        int col = j % 16;
        // set bit `col` of uint `rows[row]`
        uint16_t switches = rows[row];
        // Alternate the LED
        if (trellis.justPressed(trellisButtons[j])){
          update_required = 1;
          if(trellis.isLED(trellisButtons[j])){
            switches = switches & ~(1 << (15 - col));
            trellis.clrLED(trellisButtons[j]);
          } else {
            switches = switches | (1 << (15 - col));
            trellis.setLED(trellisButtons[j]);
          }
        }
        rows[row] = switches;
    }
    return rows;
}

void loop() {
  // send data only when you receive data:
  int incomingByte = 0;
  if (Serial.available() > 0) {
    // read the incoming byte:
    incomingByte = Serial.read();

    // say what you got:
    Serial.print("I received: ");
    Serial.println(incomingByte, DEC);
  }
  delay(30); // 30ms delay is required, dont remove me!

  // If a button was just pressed or released...
  if (trellis.readSwitches()) {
      // tell the trellis to set the LEDs we requested and assemble a bit matrix
      int * serialData = updateDisplay(trellisButtons);
      trellis.writeDisplay();
      // Serial.print("Switch Data:\n");
      for(int i=0; i<16; i++){
        for(uint16_t mask = 0x8000; mask; mask >>= 1){
//          Serial.print((*(serialData + i) & mask) && 1 || 0, BIN);
//          Serial.print(" ");
        }
//        Serial.print("\n");
      }
//      Serial.print("\n---\n");
      if(update_required){
//      Serial.print("updating Xpnt Array:\n");
        updateXpntArray(serialData);
      }
      // do i2c write to master here    
  }
}

