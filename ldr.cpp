#include "ldr.h"
#include "Arduino.h"
#include "main.h"

void init_ldr(void)
{
   pinMode(GARDEN_LIGHT, OUTPUT);
   pinMode(LDR, INPUT);
   
}
unsigned int adc_val;
void brightness_control(void)
{
  adc_val  = analogRead(LDR);
  adc_val = 255 - (adc_val/4);
  analogWrite(GARDEN_LIGHT, adc_val);
  delay(100);
}
