#include <Arduino.h>
#include <Wire.h>
#include "Adafruit_TCS34725.h" 
#include "defines.h"
#include <Stepper.h>

using namespace defines;

// objet utilisé pour lire les valeurs du capteur de couleur
Adafruit_TCS34725 tcs = Adafruit_TCS34725(TCS34725_INTEGRATIONTIME_50MS, TCS34725_GAIN_4X);
Stepper steppermotor(STEPS_PER_REV, 4, 6, 5, 7);

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
CustomColor read_color();
int find_color(CustomColor);
void printRGBValues(CustomColor color);
int get_color();
void turnGear(int turn_degrees);

// setup
void setup() 
{
    Serial.begin(9600);
    //Serial.println("Color test begins :");

    // alimente le capteur de couleur sur la pin D13
    //pinMode(13, OUTPUT);
    //digitalWrite(13, 1);

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
        // test only color sensor
        currentColor = read_color();

        printRGBValues(currentColor);
        
        find_color(currentColor);

        countNotDead++;
        Serial.println(countNotDead);
        Serial.println();

        delay(1000);

        /*int skittlesLeft = NB_TESTS;
        int color = INVALID;

        while(skittlesLeft != 0)
        {

            //turnGear(90);
            //STEPS_PER_OUT_REV * turn_degrees / 360;
            steppermotor.setSpeed(1023);    
            steppermotor.step(STEPS_PER_OUT_REV /4);
            delay(1000);
            //color = find_color(read_color());
            currentColor=read_color();
            printRGBValues(currentColor);
            find_color(currentColor);
            if(color == INVALID)
            {
                //skittlesLeft--;
            }
            else
            {
                
                skittlesLeft = NB_TESTS;
            }
        }
        while(1);*/
    }
}

CustomColor read_color()
{
    CustomColor color = CustomColor(0,0,0);

    // variables used in the calculations and readings
    uint32_t sum;
    uint16_t clear;
    float r, g, b;

    tcs.setInterrupt(false);      // turn on LED

    delay(70);  // takes 50ms to read

    tcs.getRawData(&color.Red, &color.Green, &color.Blue, &clear); // get data from sensor

    tcs.setInterrupt(true);  // turn off LED

    sum = clear;

    // put values as relatives for easier comparing
    r = color.Red; r /= sum; 
    g = color.Green; g /= sum;
    b = color.Blue; b /= sum;
    r *= 256; g *= 256; b *= 256;

    // put values in int format
    color.Red = (int)r;
    color.Green = (int)g;
    color.Blue = (int)b;

    return color;
}

/*
This function takes in a color object and compairs the RGB values to presets
for each possible colors. An int with a value relative to the color that fit the
RGB values is returned.
*/
int find_color(CustomColor color)
{
    int color_match = INVALID;
    
    // color is red
    if((color.Red >= RED_MIN_RED && color.Red <= RED_MAX_RED) &&
    (color.Green >= RED_MIN_GREEN && color.Green <= RED_MAX_GREEN) &&
    (color.Blue >= RED_MIN_BLUE && color.Blue <= RED_MAX_BLUE))
    {
        Serial.println("Color is RED");
        color_match = RED;
    }
    // color is green
    else if((color.Red >= GREEN_MIN_RED && color.Red <= GREEN_MAX_RED) &&
    (color.Green >= GREEN_MIN_GREEN && color.Green <= GREEN_MAX_GREEN) &&
    (color.Blue >= GREEN_MIN_BLUE && color.Blue <= GREEN_MAX_BLUE))
    {
        Serial.println("Color is GREEN");
        color_match = GREEN;
    }
    // color is purple
    else if((color.Red >= PURPLE_MIN_RED && color.Red <= PURPLE_MAX_RED) &&
    (color.Green >= PURPLE_MIN_GREEN && color.Green <= PURPLE_MAX_GREEN) &&
    (color.Blue >= PURPLE_MIN_BLUE && color.Blue <= PURPLE_MAX_BLUE))
    {
        Serial.println("Color is PURPLE");
        color_match = PURPLE;
    }
    // color is yellow
    else if((color.Red >= YELLOW_MIN_RED && color.Red <= YELLOW_MAX_RED) &&
    (color.Green >= YELLOW_MIN_GREEN && color.Green <= YELLOW_MAX_GREEN) &&
    (color.Blue >= YELLOW_MIN_BLUE && color.Blue <= YELLOW_MAX_BLUE))
    {
        Serial.println("Color is YELLOW");
        color_match = YELLOW;
    }
    // color is orange
    else if((color.Red >= ORANGE_MIN_RED && color.Red <= ORANGE_MAX_RED) &&
    (color.Green >= ORANGE_MIN_GREEN && color.Green <= ORANGE_MAX_GREEN) &&
    (color.Blue >= ORANGE_MIN_BLUE && color.Blue <= ORANGE_MAX_BLUE))
    {
        Serial.println("Color is ORANGE");
        color_match = ORANGE;
    }
    // color is unknown
    else
    {
        Serial.println("Color is UNKNOWN");
    }
    Serial.println();
    return color_match;
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

void turnGear(int turn_degrees)
{
    int steps_to_do = STEPS_PER_OUT_REV * turn_degrees / 360;
    steppermotor.setSpeed(1023);    
    steppermotor.step(-steps_to_do);
}

int get_color()
{
    int color[NB_SCANS_COLOR]={INVALID};    // an array to add the detected colors to
    int color_count[NB_COLOR+1] = {0};      // an array to add up the number of times a color was detected

    // loop to detect the colors and count how many of each was detected
    for(int i = 0; i < NB_SCANS_COLOR; i++)
    {
        color[i]=find_color(read_color());
        color_count[color[i]]++;
    }
    
    // returns a color if detected more than half the times
    for(int i = 0; i<=NB_COLOR; i++)
    {
        if(color_count[i]>NB_SCANS_COLOR/2)
        {
            return i;
        }
    }

    // if no main color, return invalid
    return INVALID;
}