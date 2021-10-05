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
const float LEFT_FORWARD_SPEED = 0.40f;
/** Speed for the right motor when going forward */
const float RIGHT_FORWARD_SPEED = 0.40f;
/** Ponderation factor for the PID */
const float KP = 0.0001f;
/** Number of pulses a motor does by cycle */
const int PULSES_BY_TURN = 3200;

void forward(long distance);

void setup()
{
    //Serial.begin(9600);
    BoardInit();
    forward(300);
}

void loop()
{

}

/**
 * Function that makes the robot go forward on a specified distance.
 * 
 * @param[in] distance The distance that the robot moves in cm.
 */
void forward(long distance)
{
    // Reset the encoders just in case there were not already reset.
    ENCODER_Reset(LEFT);
    ENCODER_Reset(RIGHT);

    // Number of turns the wheels must do to cover the distance.
    float nb_wheel_turns = 1.0f * distance / CIRCUMFERENCE;
    // Number of pulses that the motors will have to do.
    long nb_pulses_distance = nb_wheel_turns * PULSES_BY_TURN;

    // A counter that keeps track of the total number of pulses made with a motor
    // since we will reset the encoder after each reading
    long total_pulses_left = 0;
    long total_pulses_right = 0;

    float left_speed = LEFT_FORWARD_SPEED;
    float right_speed = RIGHT_FORWARD_SPEED;

    MOTOR_SetSpeed(LEFT, left_speed);
    MOTOR_SetSpeed(RIGHT, right_speed);

    // Will make the robot go forward until the motors exceed the needed number of pulses
    while (total_pulses_left < nb_pulses_distance && total_pulses_right < nb_pulses_distance)
    {
        delay(100);

        // Reset the encoders after the reading because we want the number of pulses between each readings,
        // not the number of pulses since the beginning.
        long pulses_right = ENCODER_ReadReset(RIGHT);
        long pulses_left = ENCODER_ReadReset(LEFT);

        total_pulses_left += pulses_left;
        total_pulses_right += pulses_right;

        // ===== PID =====

        // If there is a difference between the pulses of the right motor and the left one.
        if (pulses_left != pulses_right)
        {
            int speed_error = pulses_left - pulses_right;
            left_speed = left_speed - (speed_error * KP);
            MOTOR_SetSpeed(LEFT, left_speed);
        }

        // ===============
    }

    // Reset the encoders for the next actions.
    ENCODER_Reset(LEFT);
    ENCODER_Reset(RIGHT);

    MOTOR_SetSpeed(LEFT, 0.0f);
    MOTOR_SetSpeed(RIGHT, 0.0f);
}