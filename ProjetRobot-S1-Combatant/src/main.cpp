#include <Arduino.h>
#include <math.h>
#include <Wire.h>
#include "Adafruit_TCS34725.h"
#include "defines.h"

using namespace defines;

class CustomColor
{
    private:
    /* data */
    public:
        uint16_t Red, Green, Blue; // Valeurs relatives des couleurs

        // constructeur
        CustomColor(int r, int g, int b)
        {
            Red = r;
            Green = g;
            Blue = b;
        }
        ~CustomColor()
        {
        }
};

CustomColor readColorOnce();
int findColor(CustomColor);

void setup() {
  // put your setup code here, to run once:
}

void loop() {
  // put your main code here, to run repeatedly:
}

/*
Cette fonction fait la lecture du capteur de couleur et retourne un objet avec
les 3 composantes de couleur
*/
CustomColor readColorOnce()
{
    CustomColor color = CustomColor(0,0,0);
    uint32_t sum;
    uint16_t clear;
    float r, g, b;

    tcs.setInterrupt(false);      // turn on LED

    delay(70);  // takes 50ms to read

    tcs.getRawData(&color.Red, &color.Green, &color.Blue, &clear);

    tcs.setInterrupt(true);  // turn off LED

    sum = clear;

    r = color.Red; r /= sum;
    g = color.Green; g /= sum;
    b = color.Blue; b /= sum;
    r *= 256; g *= 256; b *= 256;

    color.Red = (int)r;
    color.Green = (int)g;
    color.Blue = (int)b;

    return color;
}

/*
Cette fonction prend en entrée un objet de couleur et le compare à des valeurs
pré-déterminées pour savoir si l'objet de couleur est l'une des couleurs recherchées.
La fonction retourne la couleur trouvée selon un index défini
*/
int findColor(CustomColor color)
{
    int colorMatch = UNKNOWN;
    
    // color is red
    if((color.Red >= RED_MIN_RED && color.Red <= RED_MAX_RED) &&
    (color.Green >= RED_MIN_GREEN && color.Green <= RED_MAX_GREEN) &&
    (color.Blue >= RED_MIN_BLUE && color.Blue <= RED_MAX_BLUE))
    {
        Serial.println("Color is RED");
        colorMatch = RED;
    }
    // color is green
    else if((color.Red >= GREEN_MIN_RED && color.Red <= GREEN_MAX_RED) &&
    (color.Green >= GREEN_MIN_GREEN && color.Green <= GREEN_MAX_GREEN) &&
    (color.Blue >= GREEN_MIN_BLUE && color.Blue <= GREEN_MAX_BLUE))
    {
        Serial.println("Color is GREEN");
        colorMatch = GREEN;
    }
    // color is blue
    else if((color.Red >= BLUE_MIN_RED && color.Red <= BLUE_MAX_RED) &&
    (color.Green >= BLUE_MIN_GREEN && color.Green <= BLUE_MAX_GREEN) &&
    (color.Blue >= BLUE_MIN_BLUE && color.Blue <= BLUE_MAX_BLUE))
    {
        Serial.println("Color is BLUE");
        colorMatch = BLUE;
    }
    // color is yellow
    else if((color.Red >= YELLOW_MIN_RED && color.Red <= YELLOW_MAX_RED) &&
    (color.Green >= YELLOW_MIN_GREEN && color.Green <= YELLOW_MAX_GREEN) &&
    (color.Blue >= YELLOW_MIN_BLUE && color.Blue <= YELLOW_MAX_BLUE))
    {
        Serial.println("Color is YELLOW");
        colorMatch = YELLOW;
    }
    // color is unknown
    else
    {
        Serial.println("Color is UNKNOWN");
    }
    Serial.println();
    return colorMatch;
}   