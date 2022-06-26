#if defined(USE_TINYUSB)
#include <Adafruit_TinyUSB.h> // for Serial
#endif

#include <Arduino.h>

uint8_t interrupt_count = 0;

uint8_t button = 7; // https://learn.adafruit.com/assets/110598 - switch

void setup() 
{
  Serial.begin(115200);
  
  // register an interrupt handler for D7 (SW button)...let's just do rising edge for now
  // valid options are RISING, FALLING, CHANGE - https://circuitdigest.com/microcontroller-projects/arduino-interrupt-tutorial-with-examples

  pinMode( button, INPUT );
  
  // ISR_DEFERRED flag cause the callback to be deferred from ISR context
  // and invoked within a callback thread.
  // It is required to use ISR_DEFERRED if callback function take long time 
  // to run e.g Serial.print() or using any of Bluefruit API() which will
  // potentially call rtos API
  attachInterrupt( digitalPinToInterrupt(button), my_isr, ISR_DEFERRED | FALLING );

  // setup the blue LED to toggle from the ISR
  pinMode(LED_BLUE, OUTPUT);
  // let the RED LED be a heartbeat from main
  pinMode(LED_RED, OUTPUT);
}

void loop() 
{
  uint8_t toggle = digitalRead(LED_RED);

  digitalWrite(LED_RED, !toggle);   // invert the LED setting
  // sleep and say hello periodically...wait for interrupt to occur
  delay( 10000 );
  Serial.print( "Interrupt count is: ");
  Serial.println( interrupt_count );
}

void my_isr(void)
{
  uint8_t toggle = digitalRead(LED_BLUE);
  // increment the interrupt count and update the LED
  interrupt_count++;
  // invert the current setting
  digitalWrite(LED_BLUE, !toggle);

  // it's ok to print here since it's a deferred ISR
  Serial.println("Got an interrupt!");
}
