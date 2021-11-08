#include <Arduino.h>
#include <LibRobus.h>
#include <math.h>

// === MOTORS ===
#define PULSES_BY_TURN 3200             // Number of ticks/pulses that the encoders read each time the wheels finish a turn
#define PULSES_BY_CIRCLE 7979 * 2
#define WHEEL_SIZE_ROBOTA 7.63
#define WHEEL_SIZE_ROBOTB 7.55

// === FORWARD ===
#define BASE_SPEED 0.2                  // Base speed of the robot when going forward
#define MIN_SPEED 0.2
#define MAX_SPEED 0.9
#define CORRECTION_FACTOR 0.0004        // Correction factor for the PID

// === TURN ===
#define BASE_TURN_SPEED 0.3             // Base speed of the robot when turning

// === BUMPERS ===
#define FRONT_BUMPER_PIN 28

// === DEL ===
// PIN numbers for the DEL, 50-51-52 are already taken by ROBUS
#define RED_DEL_PIN 53
#define BLUE_DEL_PIN 48
#define YELLOW_DEL_PIN 47
#define GREEN_DEL_PIN 49

// === DETECTION ===
#define LINE_PIN A7
#define DETECTION_SPEED 0.3

// === OBJECT DETECTION ===
#define SONAR_ID 0
#define MARGIN_ERROR_DISTANCE 0.05
#define MIN_DETECTION 4                 // Mininum number of detection that it takes for the robot to decide that he detected the object.
#define MAX_DETECTION 12                // Maximum number of detection that it takes for the robot to decide that he did NOT detect the object.
#define DETECTION_TURN_SPEED 0.2

const float WHEEL_SIZE_CM = WHEEL_SIZE_ROBOTA * 3.141592;

void forward (float distance);
void turn (float angle);
void pin_setup();
void turn_on_del(int del_pin);
void turn_off_del(int del_pin);
void turn_off_del_all();
float detect_object(float max_distance);
void seek_object(float max_distance);
void stop_action();
void detect_line(float distance);

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
        sw = digitalRead(FRONT_BUMPER_PIN);
        delay(1);
    }

    seek_object(30.0);
}

/**
 * Function to make the robot turn.
 * 
 * @param[in] angle The angle at which the robot must turn. A negative value will make it turn to the left.
 */
void turn (float angle){
    long nbPulses = 0;

    ENCODER_Reset(RIGHT);
    ENCODER_Reset(LEFT); 

    float convert = abs(angle /360 * PULSES_BY_CIRCLE);

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
    }
    else
    {
        // make a 180

        MOTOR_SetSpeed(LEFT, BASE_TURN_SPEED);
        MOTOR_SetSpeed(RIGHT, -BASE_TURN_SPEED);

        while(((convert - 100) / 2) > nbPulses)
        {
            _delay_us(10);
            nbPulses = ENCODER_Read(LEFT);
        }
    }

    stop_action();
 }

/**
 * Function that makes the robot go forward.
 * 
 * @param[in] distance The distance in cm for which the robot must go forward.
 */
void forward(float distance) 
{ 
    if (distance == 0)
    {
        return;
    }

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

    while(pulses_left < pulse_to_do)
    {
        pulses_left = ENCODER_Read(LEFT);
        pulses_right = ENCODER_Read(RIGHT);

        if (pulses_left * 1.0f / pulse_to_do < 0.5f)
        {
            if (current_speed < MAX_SPEED)
            {
                current_speed += added_speed;
            }
        }
        else
        {
            if (current_speed > MIN_SPEED)
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
 
    stop_action();
}

/**
 * Function that configure the different pins.
 */
void pin_setup()
{
    pinMode(FRONT_BUMPER_PIN, INPUT);

    //Vout SENSOR LIGNE
    pinMode(LINE_PIN, INPUT);

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

/**
 * Get the average of an array of float.
 * 
 * @param[in] arr The array to get the average from
 * @param[in] size The size of the array
 * @param[out] average The average
 */
float get_average(float arr[], int size)
{
    float sum = 0.0;
    // Will keep track of how many non-zero values there are in the array
    int non_zero_values = 0;

    for (int i = 0; i < size; i++)
    {
        float val = arr[i];

        if (val != 0)
        {
            non_zero_values++;
            sum += val;
        }
    }

    // If there is 0 non-zero values, that means that the array is filled with 0. To dodge a
    // divided by 0 exception, we simply return 0.
    if (non_zero_values == 0)
    {
        return 0;
    }

    // We want to divide by the numder of non-zero values to get a more meaningful average.
    return sum / non_zero_values;
}

/**
 * Function that makes the robot seek an object. It will try to detect an object and go towards it and then return
 * to its initial position. If it fails to detect the object, it will continue to move while following the path
 * until it finds it.
 * 
 * @param[in] max_distance The maximum distance, in cm, at which we expect to find the object.
 */
void seek_object(float max_distance)
{
    while(true)
    {
        float obj_distance = detect_object(max_distance);

        if (obj_distance == 0.0)
        {
            turn(180);

            detect_line(40.0);
        }
        else
        {
            forward(obj_distance);
            delay(500);
            turn(180);
            delay(500);
            forward(obj_distance);

            break;
        }
    }
}

/**
 * Function to detect an object and get how far away it is.
 * 
 * @param[in] max_distance The maximum distance, in cm, at which we expect to find the object.
 * @param[out] obj_distance The distance between the robot and the object, in cm.
 */
float detect_object(float max_distance)
{
    int nb_detection = 0;
    // Array containing the previous detected values that we want to consider.
    float last_distances [MAX_DETECTION] = {};
    long nbPulses = 0;
    float detection_distance = 0.0f;

    ENCODER_Reset(RIGHT);
    ENCODER_Reset(LEFT); 

    float convert = abs(180.0f / 360 * PULSES_BY_CIRCLE);

    MOTOR_SetSpeed(LEFT, DETECTION_TURN_SPEED);
    MOTOR_SetSpeed(RIGHT, -DETECTION_TURN_SPEED);

    while (true)
    {
        // Delay of 100ms for the sonar, this is recommended by the doc.
        delay(100);
        nbPulses = ENCODER_Read(LEFT);

        if (((convert - 100) / 2) < nbPulses)
        {
            // Turn until the robot does a 180, then stops. Will happen if the robot didn't detect any
            // object, will then return 0 for the distance.

            stop_action();
            return 0.0f;
        }

        float obj_distance = SONAR_GetRange(SONAR_ID);
        // The average of the previous detections.
        float last_distances_average = get_average(last_distances, MAX_DETECTION);

        if (obj_distance <= max_distance)
        {
            // If we are detecting something inside of the maximum distance range

            // Calculate the range in which the object's distance must be. The range is the average of the
            // previous distances with a margin of error.
            float max_distance_range = last_distances_average * (1.0 + MARGIN_ERROR_DISTANCE);
            float min_distance_range = last_distances_average * (1.0 - MARGIN_ERROR_DISTANCE);

            if ((nb_detection < MAX_DETECTION && obj_distance <= max_distance_range && obj_distance >= min_distance_range) ||
                (nb_detection < MAX_DETECTION && last_distances_average == 0.0))
            {
                /*
                    First condition block: checks that the max detection number is not exceeded and that the distance
                        between the object and the robot is between a certain range of the previous distances.
                    
                    OR

                    Second condition block: checks that the max detection number is not exceeded and that the average
                        of the previous distances is 0 (will happen if there is no distance in the array)
                */
                last_distances[nb_detection] = obj_distance;
                nb_detection++;
            }
        }
        else
        {
            if (nb_detection >= MIN_DETECTION && nb_detection <= MAX_DETECTION)
            {
                /*If the first detected object was detected long enough, but not too long, we will return the distance
                from the robot to the object and apply a turn to the left to the robot to correct the fact that he
                overshot the object.*/

                stop_action();

                turn(-30);

                // The distance returned by the sonar is not enough to get to the object, so we add a certain distance to it.
                // Might be a good idea to find another way than to hardcode the value.
                return last_distances_average + 20;
            }

            nb_detection = 0;

            // "Clear" the array by replacing all of its value by 0.
            for (int i = 0; i < MAX_DETECTION; i++)
            {
                last_distances[i] = 0;
            }
        }
    }
}

void stop_action()
{
    // Stop the motors
    MOTOR_SetSpeed(LEFT, 0); 
    MOTOR_SetSpeed(RIGHT, 0); 
    ENCODER_Reset(LEFT);
    ENCODER_Reset(RIGHT);
    _delay_us(10);
}

/** 
 * Function makes the robot follow the white line.
 * 
 * @param[in] distance The distance for which the robot must follow the line. 
 *      If distance is equal to 0 the robot will follow the line indefinetly.
 */ 
void detect_line(float distance)
{
    ENCODER_Reset(LEFT);
    ENCODER_Reset(RIGHT);

    MOTOR_SetSpeed(LEFT, BASE_SPEED); 
    MOTOR_SetSpeed(RIGHT, BASE_SPEED);

    float nb_wheel_turn = distance / WHEEL_SIZE_CM;
    long nb_pulses = nb_wheel_turn * PULSES_BY_TURN;

    while (true)
    {
        delay(10);

        if (distance != 0.0 && ENCODER_Read(LEFT) >= nb_pulses)
        {
            break;
        }

        int detect_value = analogRead(A7);

        if (detect_value == 733 || detect_value == 441 || detect_value == 1021)
        {
            continue;
        }

        if(detect_value < 291+3 && detect_value > 291-3) //detection par SENSOR_DROITE
        {
            MOTOR_SetSpeed(LEFT, BASE_TURN_SPEED); 
            MOTOR_SetSpeed(RIGHT, 0);

            while(detect_value > 148 + 3 || detect_value < 148 - 3)
            {
                delay(1);
                detect_value = analogRead(A7);
            }
            MOTOR_SetSpeed(LEFT, BASE_SPEED); 
            MOTOR_SetSpeed(RIGHT, BASE_SPEED);
        }
        else if(detect_value < 583+3 && detect_value > 583-3) //detection par SENSOR_GAUCHE 
        {
            MOTOR_SetSpeed(LEFT, 0); 
            MOTOR_SetSpeed(RIGHT, BASE_TURN_SPEED);

            while(detect_value > 148 + 3 || detect_value < 148 - 3)
            {
                delay(1);
                detect_value = analogRead(A7);
            }

            MOTOR_SetSpeed(LEFT, BASE_SPEED); 
            MOTOR_SetSpeed(RIGHT, BASE_SPEED);
        }
    }

    MOTOR_SetSpeed(LEFT, 0); 
    MOTOR_SetSpeed(RIGHT, 0);

    ENCODER_Reset(LEFT);
    ENCODER_Reset(RIGHT);
}
