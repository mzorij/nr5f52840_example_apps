#if defined(USE_TINYUSB)
#include <Adafruit_TinyUSB.h> // for Serial
#endif

#include <Arduino.h>

uint8_t led = LED_BLUE;//LED_RED;//LED_BLUE;

void setup() 
{
  pinMode(led, OUTPUT);
}

void loop() 
{
  digitalWrite(led, HIGH);   // turn the LED on (HIGH is the voltage level)
  delay(1000);                       // wait for a second
  digitalWrite(led, LOW);    // turn the LED off by making the voltage LOW
  delay(1000);                       // wait for a second
}
