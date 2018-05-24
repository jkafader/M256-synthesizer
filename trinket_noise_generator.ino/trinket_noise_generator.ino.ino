#define SPK_PIN 1

void setup() {
  // put your setup code here, to run once:
  pinMode(SPK_PIN, OUTPUT);
  randomSeed(analogRead(2));
}

void loop() {
   int output = random(0,2);
   digitalWrite (SPK_PIN, output & 1);
   //delay(1);
  // put your main code here, to run repeatedly:
}
