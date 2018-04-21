#define SIN_PIN 8
#define SCLK_PIN  9
#define PCLK_PIN 10
#define OUTPUT_PINS 16

uint8_t update_required = 0;
uint16_t switches[OUTPUT_PINS]; 

void setup()
{
  pinMode(SCLK_PIN, OUTPUT);
  pinMode(SIN_PIN, OUTPUT);
  digitalWrite(PCLK_PIN, HIGH);
  pinMode(PCLK_PIN, OUTPUT);

}
void loop()
{
  connect (12, 0);    // Matrix (Input,Output)

  update ();
  update_required = 0;
}

void connect(uint8_t signal, uint8_t pin)           // function writes a single bit to connect any of the 16 Y pins to any of the 48 X pins.
{
  uint8_t chip = 0;
  if (signal >= 16) return;
  switches[chip + (15 - signal)] |= (1 << (pin & 15));
  update_required = 1;
}

void disconnectAll(void)                            // function clears the entire in-memory array
{
  memset(switches, 0, sizeof(switches));
  update_required = 1;
}

void update(uint16_t * switches)           // writes all the bits to all 3 chips and pulses the latch clock, so all the switches change at the same moment.
{
  uint8_t i;
  uint16_t n, mask;

  for (i = 0; i < OUTPUT_PINS; i++) {
    n = switches[i];
    // begin with 1000 0000 0000 0000
    // bit shift right until the bit "falls off" pins.
    // this will loop 16 times before stopping.
    for (mask = 0x8000; mask; mask >>= 1) {
      digitalWrite(SIN_PIN, (n & mask) ? HIGH : LOW);  // SDI_PIN
      // 20ns setup required
      asm("nop"); // Each "nop" yields a 62.5 ns (nanosecond) delay
      digitalWrite(SCLK_PIN, HIGH);
      asm("nop"); // sclk pulse width, 100 ns minimum
      asm("nop");
      digitalWrite(SCLK_PIN, LOW);
      asm("nop");
      // 40ns hold time required
    }
  }
  asm("nop"); // 65ns setup required
  asm("nop");
  digitalWrite(PCLK_PIN, LOW);
  asm("nop"); // pclk pulse width 65ns minimum
  asm("nop");
  digitalWrite(PCLK_PIN, HIGH);
  update_required = 0;
}
