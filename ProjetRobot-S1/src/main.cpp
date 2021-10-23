#include <Arduino.h>
#include <LibRobus.h>
#include <math.h>

#define PULSES_BY_TURN 3200             // Number of ticks/pulses that the encoders read each time the wheels finish a turn
#define PULSES_BY_CIRCLE 7979 * 2
#define WHEEL_SIZE_ROBOTA 7.63
#define WHEEL_SIZE_ROBOTB 7.55
 
#define BASE_SPEED 0.2                  // Base speed of the robot when going forward
#define BASE_TURN_SPEED 0.3             // Base speed of the robot when turning
#define CORRECTION_FACTOR 0.0004        // Correction factor for the PID

// PIN numbers for the DEL, 50-51-52 are already taken by ROBUS
#define RED_DEL_PIN 53
#define BLUE_DEL_PIN 48
#define YELLOW_DEL_PIN 47
#define GREEN_DEL_PIN 49

const float WHEEL_SIZE_CM = WHEEL_SIZE_ROBOTA * 3.141592;

void forward (float distance);
void turn (float angle);
void pin_setup();
void turn_on_del(int del_pin);
void turn_off_del(int del_pin);
void turn_off_del_all();

void setup()
{ 
    Serial.begin(9600); 

    BoardInit(); 

    MOTOR_SetSpeed(LEFT, 0); 
    MOTOR_SetSpeed(RIGHT, 0);

    pin_setup();
    turn_off_del_all();

    Serial.println("Start"); 
} 

void loop() 
{
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
        // angle < 0 = turn left, angle > 0 = turn right
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
        // make a 180

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
 * @param[in] distance The distance in cm for which the robot must go forward.
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