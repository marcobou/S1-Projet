#include <Arduino.h>
#include <LibRobus.h>

/**
 * Circumference of the wheels (Formula = 2*PI*r)
 * 
 * PI rounded to 3
 * r = 1.5 inches rounded to 4cm
 */
const int CIRCUMFERENCE = 2 * 3 * 4;
/** Speed for the left motor when going forward */
const float LEFT_FORWARD_SPEED = 0.45f;
/** Speed for the right motor when going forward */
const float RIGHT_FORWARD_SPEED = 0.471f;

void forward(int distance);

void setup()
{
    Serial.begin(9600);
    BoardInit();
}

void loop()
{
    forward(218);

    // Infinite loop to stop the execution of the program
    while(true) {
    
    }
}

/**
 * Function that makes the robot go forward on a specified distance.
 * 
 * @param[in] distance The distance that the robot moves in cm.
 */
void forward(int distance)
{
    // Number of turns the wheels must do to cover the distance.
    float nbWheelTurnsFast = 1.0f * distance / CIRCUMFERENCE;
    // Number of pulses that the motors will have to do.
    int nbPulsesFast = nbWheelTurnsFast * 3200;

    MOTOR_SetSpeed(0, LEFT_FORWARD_SPEED);
    MOTOR_SetSpeed(1, RIGHT_FORWARD_SPEED);

    // Will make the robot go forward until the motors exceed the number needed pulses
    while (ENCODER_Read(0) < nbPulsesFast)
    {
        // TODO read the encoder for the right motor too.
        delay(100);
    }

    ENCODER_ReadReset(0);

    MOTOR_SetSpeed(0, 0.0f);
    MOTOR_SetSpeed(1, 0.0f);
}