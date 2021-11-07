#include <Arduino.h>
#include <math.h>
#include <Wire.h>
#include "Adafruit_TCS34725.h"
#include "alex.h"

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

Adafruit_TCS34725 tcs = Adafruit_TCS34725(TCS34725_INTEGRATIONTIME_50MS, TCS34725_GAIN_4X);

const float WHEEL_SIZE_CM = WHEEL_SIZE_ROBOTA * 3.141592;

void forward (float distance);
void turn (float angle);
void pin_setup();
void turn_on_del(int del_pin);
void turn_off_del(int del_pin);
void turn_off_del_all();
void initColorSensor();
CustomColor readColorOnce();
int findColor(CustomColor);

void setup() 
{
    Serial.begin(9600); 

    BoardInit(); 

    MOTOR_SetSpeed(LEFT, 0); 
    MOTOR_SetSpeed(RIGHT, 0);

    pin_setup();
    turn_off_del_all();

    initColorSensor();

    Serial.println("Start");
}

void loop() {
    bool sw = 0;
    while(sw == 0)
    {
        sw = digitalRead(28);
        delay(1);
    }

    MOTOR_SetSpeed(LEFT, 0);
    MOTOR_SetSpeed(RIGHT, 0);

    while(1);
}

/**
 * Function to make the robot turn.
 * 
 * @param[in] angle The angle at which the robot must turn. A negative value will make it turn to the left.
 */
void turn (float angle){
    long nbPulses = 0;

    // Variables contenant les dernières valeurs d'encodeurs
    ENCODER_Reset(RIGHT);
    ENCODER_Reset(LEFT); 

    float convert = abs(angle /360 * PULSES_BY_CIRCLE);

    Serial.println(convert);
    Serial.println(angle);

    if (angle != 180)
    {
        // angle < 0 = tourner à gauche, angle > 0 = tourner à droite
        int turn_motor = angle < 0 ? RIGHT : LEFT;

        MOTOR_SetSpeed(turn_motor, BASE_TURN_SPEED);

        while(convert > nbPulses)
        {
            _delay_us(10);
            nbPulses = ENCODER_Read(turn_motor);
        }

        MOTOR_SetSpeed(turn_motor, 0);
    }
    else
    {
        // tourne de 180

        MOTOR_SetSpeed(LEFT, BASE_TURN_SPEED);
        MOTOR_SetSpeed(RIGHT, -BASE_TURN_SPEED);

        while(((convert - 200) / 2) > nbPulses)
        {
            _delay_us(10);
            nbPulses = ENCODER_Read(LEFT);
        }

        MOTOR_SetSpeed(LEFT, 0);
        MOTOR_SetSpeed(RIGHT, 0);
    }

    _delay_us(10);
 }

/**
 * Function that makes the robot go forward.
 * 
 * @param[in] distance The distance for which the robot must go forward.
 */
void forward(float distance) 
{ 
    float nb_wheel_turn = 1.0f * distance / WHEEL_SIZE_CM;

    ENCODER_Reset(RIGHT);
    ENCODER_Reset(LEFT); 
    
    // Ratio used to adjust the speed of the two wheels.
    float speed_ratio = 0;

    long pulse_to_do = nb_wheel_turn * PULSES_BY_TURN;
    long pulses_left = 0;
    long pulses_right;

    MOTOR_SetSpeed(LEFT, (BASE_SPEED)); 
    MOTOR_SetSpeed(RIGHT, BASE_SPEED);

    float added_speed = 0.0001f;
    float reduce_speed = added_speed * 1.0f;
    float current_speed = BASE_SPEED;
    float max_speed = 0.9f;
    float min_speed = 0.2f;

    while(pulses_left < pulse_to_do)
    {
        pulses_left = ENCODER_Read(LEFT);
        pulses_right = ENCODER_Read(RIGHT);

        if (pulses_left * 1.0f / pulse_to_do < 0.5f)
        {
            if (current_speed < max_speed)
            {
                current_speed += added_speed;
            }
        }
        else
        {
            if (current_speed > min_speed)
            {
                current_speed -= reduce_speed;
            }
        }

        if(WHEEL_SIZE_CM == WHEEL_SIZE_ROBOTA)
        {
            speed_ratio = (pulses_right - pulses_left) * CORRECTION_FACTOR;
            MOTOR_SetSpeed(LEFT, current_speed + speed_ratio); 
            MOTOR_SetSpeed(RIGHT, current_speed);
        }
        else
        {
            speed_ratio = (pulses_left - pulses_right) * CORRECTION_FACTOR;
            MOTOR_SetSpeed(LEFT, current_speed); 
            MOTOR_SetSpeed(RIGHT, current_speed + speed_ratio);
        }

        _delay_us(100);
    }
 
    // Stop the motors
    MOTOR_SetSpeed(LEFT, 0); 
    MOTOR_SetSpeed(RIGHT, 0); 
    ENCODER_Reset(LEFT);
    ENCODER_Reset(RIGHT);
    _delay_us(10);
}

/**
 * Function that configure the different pins.
 */
void pin_setup()
{
    pinMode(28, INPUT);

    // DEL
    pinMode(RED_DEL_PIN, OUTPUT);
    pinMode(BLUE_DEL_PIN, OUTPUT);
    pinMode(YELLOW_DEL_PIN, OUTPUT);
    pinMode(GREEN_DEL_PIN, OUTPUT);
}

/**
 * Function to turn on a DEL.
 * 
 * @param[in] del_pin The pin of the DEL that we wish to turn on.
 */
void turn_on_del(int del_pin)
{
    digitalWrite(del_pin, HIGH);
}

/**
 * Function to turn off a DEL.
 * 
 * @param[in] del_pin The pin of the DEL that we wish to turn off.
 */
void turn_off_del(int del_pin)
{
    digitalWrite(del_pin, LOW);
}

/**
 * Function to turn off all the DEL.
 */
void turn_off_del_all()
{
    digitalWrite(RED_DEL_PIN, LOW);
    digitalWrite(BLUE_DEL_PIN, LOW);
    digitalWrite(YELLOW_DEL_PIN, LOW);
    digitalWrite(GREEN_DEL_PIN, LOW);
}

/*
Init anything related to the color sensor
*/
void initColorSensor()
{
    pinMode(PIN_COLOR_SENSOR, OUTPUT);
    digitalWrite(13, 1);

    // try to connect to the color sensor
    while (!tcs.begin())
    {
        Serial.println("No TCS34725 found ... check your connections");
        // cut power to the sensor and turn it back on until it works
        digitalWrite(PIN_COLOR_SENSOR, 0);
        delay(1000);
        digitalWrite(PIN_COLOR_SENSOR, 1);
    } 
}
/*
This function returns a CustomColor objet with the Red, Green and Blue relative
values of what is under the sensor.
*/
CustomColor readColorOnce()
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