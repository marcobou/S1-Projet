#include <Arduino.h>
#include <Wire.h>
#include "Adafruit_TCS34725.h" 
#include "defines.h"

using namespace defines;

// objet utilisé pour lire les valeurs du capteur de couleur
Adafruit_TCS34725 tcs = Adafruit_TCS34725(TCS34725_INTEGRATIONTIME_50MS, TCS34725_GAIN_4X);

// Classe qui permet de créer un objet de couleur avec ses 3 composantes (R, G, B)
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

// prototypes de fonctions
CustomColor readColorOnce();
int findColor(CustomColor);
void printRGBValues(CustomColor color);

// setup
void setup() 
{
    Serial.begin(9600);
    Serial.println("Color test begins :");

    // alimente le capteur de couleur sur la pin D13
    pinMode(13, OUTPUT);
    digitalWrite(13, 1);

    // essai de créer une connexion avec le capteur jusqu'a réussite
    while (!tcs.begin())
    {
        Serial.println("No TCS34725 found ... check your connections");
        // retire et redonne du courant au capteur à chaque échec de connexion
        digitalWrite(13, 0);
        delay(1000);
        digitalWrite(13, 1);
    } 

    Serial.println("Found sensor");
}

// main
void loop() 
{
    // objet de couleur
    CustomColor currentColor = CustomColor(0,0,0);

    // compteur pour s'assurer que le programme run encore
    int countNotDead=0;

    while(1)
    {
        currentColor = readColorOnce();

        printRGBValues(currentColor);
        
        findColor(currentColor);

        countNotDead++;
        Serial.println(countNotDead);
        Serial.println();

        delay(1000);
    }
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

/*
Cette fonction print sur le port série les composantes de l'objet couleur
donné en entrée.
 */
void printRGBValues(CustomColor color)
{
    Serial.println("Current color :");
    Serial.print("Red : "); Serial.println(color.Red);
    Serial.print("Green : "); Serial.println(color.Green);
    Serial.print("Blue : ");  Serial.println(color.Blue);
    Serial.println();
}
