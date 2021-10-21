#include <Arduino.h>
#include <Wire.h>
#include "Adafruit_TCS34725.h" 

#define RED 0
#define GREEN 1
#define BLUE 2

Adafruit_TCS34725 tcs = Adafruit_TCS34725(TCS34725_INTEGRATIONTIME_50MS, TCS34725_GAIN_4X);

class CustomColor
{
  private:
    /* data */
  public:
    uint16_t Red, Green, Blue, Clear;

    CustomColor(int r, int g, int b, int c)
    {
      Red = r;
      Green = g;
      Blue = b;
      Clear = c;
    }
    ~CustomColor()
    {
    }
};

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  Serial.println("CustomColor View Test!");

  if (tcs.begin()) 
  {
      Serial.println("Found sensor");
  } 
  else 
  {
      Serial.println("No TCS34725 found ... check your connections");
      while (1); // halt!
  }
}

void loop() 
{
  uint16_t clear, red, green, blue;

  tcs.setInterrupt(false);      // turn on LED

  delay(60);  // takes 50ms to read

  tcs.getRawData(&red, &green, &blue, &clear);

  tcs.setInterrupt(true);  // turn off LED

  Serial.print("C:\t"); Serial.print(clear);
  Serial.print("\tR:\t"); Serial.print(red);
  Serial.print("\tG:\t"); Serial.print(green);
  Serial.print("\tB:\t"); Serial.print(blue);
  // Figure out some basic hex code for visualization
    uint32_t sum = clear;
    float r, g, b;
    r = red; r /= sum;
    g = green; g /= sum;
    b = blue; b /= sum;
    r *= 256; g *= 256; b *= 256;
    Serial.print("\t");
    Serial.print((int)r, HEX); Serial.print((int)g, HEX); Serial.print((int)b, HEX);
    Serial.println();

    Serial.print((int)r ); Serial.print(" "); Serial.print((int)g);Serial.print(" ");  Serial.println((int)b );
}

CustomColor readColorOnce()
{
  CustomColor color = CustomColor(0,0,0,0);

  tcs.setInterrupt(false);      // turn on LED

  delay(60);  // takes 50ms to read

  tcs.getRawData(&color.Red, &color.Green, &color.Blue, &color.Clear);

  tcs.setInterrupt(true);  // turn off LED

  return color;
}




