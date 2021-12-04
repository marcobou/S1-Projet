#include <Arduino.h>
#include <math.h>
#include <Wire.h>
#include "Adafruit_TCS34725.h"
#include "alex.h"
#include <LibRobus.h>
#include <Stepper.h>

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
Stepper steppermotor(STEPS_PER_REV, STEPPER_PIN1, STEPPER_PIN2, STEPPER_PIN3, STEPPER_PIN4);


const float WHEEL_SIZE_CM = WHEEL_SIZE_ROBOTA * 3.141592;

void forward (float distance);
void turn (float angle);
void pin_setup();
void turn_on_del(int del_pin);
void turn_off_del(int del_pin);
void init_color_sensor();
CustomColor read_color_once();
int find_color(CustomColor);
bool object_detection(int *nb_detection, float last_distances[]);
void stop_action();
void stop_motors();
void reset_encoders();
void detect_line(float distance);
float get_average(float arr[], int size);
void turn_to_central_sensor(int direction);
void logic_gear();
void turn_gear(int turn_degrees);
int get_color();
void printRGBValues(CustomColor color);

void setup()
{ 
    Serial.begin(9600); 

    BoardInit(); 

    stop_motors();
    reset_encoders();

    pin_setup();
    pinMode(27,INPUT);

    init_color_sensor();

    delay(500);

    Serial.println("Start"); 
} 

void loop() 
{
    bool sw = 0;
    CustomColor color(0,0,0);
    while(sw == 0)
    {
        sw = digitalRead(FRONT_BUMPER_PIN);
        if(digitalRead(27))
        {
            turn_gear(1);
            delay(10);
        }
        delay(1);
    }
    delay(10);
    //turn_gear(1);
    /*turn_gear(90);
    color = read_color_once();
    printRGBValues(color);*/
    logic_gear(); 
}

/**
 * Function to make the robot turn.
 * 
 * @param[in] angle The angle at which the robot must turn. A negative value will make it turn to the left.
 */
void turn (float angle){
    long nb_pulses = 0;

    reset_encoders();

    float convert = abs(angle /360 * PULSES_BY_CIRCLE);

    if (angle != 180)
    {
        // angle < 0 = turn left, angle > 0 = turn right
        int turn_motor = angle < 0 ? RIGHT : LEFT;

        MOTOR_SetSpeed(turn_motor, BASE_TURN_SPEED);

        while(convert > nb_pulses)
        {
            _delay_us(10);
            nb_pulses = ENCODER_Read(turn_motor);
        }
    }
    else
    {
        // make a 180

        MOTOR_SetSpeed(LEFT, BASE_TURN_SPEED);
        MOTOR_SetSpeed(RIGHT, -BASE_TURN_SPEED);

        while(((convert - 100) / 2) > nb_pulses)
        {
            _delay_us(10);
            nb_pulses = ENCODER_Read(LEFT);
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

    reset_encoders(); 
    
    // Ratio used to adjust the speed of the two wheels.
    float speed_ratio = 0;

    long pulse_to_do = nb_wheel_turn * PULSES_BY_TURN;
    long pulses_left = 0;
    long pulses_right;

    MOTOR_SetSpeed(LEFT, BASE_SPEED); 
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

/*
Init anything related to the color sensor
*/
void init_color_sensor()
{
    // try to connect to the color sensor
    while (!tcs.begin())
    {
        Serial.println("No TCS34725 found ... check your connections");
        delay(1000);
    } 
    Serial.println("TCS34725 found");
}
/*
This function returns a CustomColor objet with the Red, Green and Blue relative
values of what is under the sensor.
*/
CustomColor read_color_once()
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

/**
 * Function that configure the different pins.
 */
void pin_setup()
{
    pinMode(FRONT_BUMPER_PIN, INPUT);

    //Vout SENSOR LIGNE
    pinMode(LINE_PIN, INPUT);
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
 * Function that makes the robot detect an object while it moves and/or follows the track.
 * 
 * @param[in] nb_detection The number of times the robot detected the same object. Must pass a pointer.
 * @param[in] last_distances An array containing all the last mesured distances between the robot and the object.
 * @param[out] object_is_detected True if the object was detected.
 */
bool object_detection(int *nb_detection, float last_distances[])
{
    delay(100);
    float obj_distance = SONAR_GetRange(SONAR_ID);
    float last_distances_average = get_average(last_distances, MIN_DETECTION + 1);

    if (obj_distance <= MAX_DISTANCE && obj_distance != 0.0)
    {
        float max_distance_range = last_distances_average * (1.0 + MARGIN_ERROR_DISTANCE);
        float min_distance_range = last_distances_average * (1.0 - MARGIN_ERROR_DISTANCE);

        if ((obj_distance <= max_distance_range && obj_distance >= min_distance_range) ||
            last_distances_average == 0.0)
        {
            /*
                First condition block: checks that the max detection number is not exceeded and that the distance
                    between the object and the robot is between a certain range of the previous distances.
                
                OR

                Second condition block: checks that the max detection number is not exceeded and that the average
                    of the previous distances is 0 (will happen if there is no distance in the array)
            */
            last_distances[*nb_detection] = obj_distance;
            (*nb_detection)++;

            Serial.println(*nb_detection);

            if (*nb_detection >= MIN_DETECTION)
            {             
                return true;
            }
        }
    }
    else
    {
        *nb_detection = 0;

        // "Clear" the array by replacing all of its value by 0.
        for (int i = 0; i < MIN_DETECTION + 1; i++)
        {
            last_distances[i] = 0;
        }
    }

    return false;
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
 * Function that must be called at the end of each action to stop the motors and reset the encoders.
 */ 
void stop_action()
{
    stop_motors();
    reset_encoders(); 
    _delay_us(10);
}

/**
 * Function that only stops the motors.
 */ 
void stop_motors()
{
    MOTOR_SetSpeed(LEFT, 0); 
    MOTOR_SetSpeed(RIGHT, 0); 
}

/**
 * Function that resets both encoders.
 */ 
void reset_encoders()
{
    ENCODER_Reset(LEFT);
    ENCODER_Reset(RIGHT);
}

/**
 * Function that makes the robot turn until the central sensor meets the track's line.
 * 
 * @param[in] direction The direction in which the robot must turn. 0 for left and 1 for right.
 */ 
void turn_to_central_sensor(int direction)
{
    bool turn_right = direction == RIGHT;

    MOTOR_SetSpeed(LEFT, turn_right ? LINE_CORRECTION_SPEED : COUNTER_LINE_CORRECTION); 
    MOTOR_SetSpeed(RIGHT, turn_right ? COUNTER_LINE_CORRECTION : LINE_CORRECTION_SPEED);

    int detect_value = 0;

    while(detect_value > 148 + 3 || detect_value < 148 - 3)
    {
        delay(1);
        detect_value = analogRead(A7);
    }

    MOTOR_SetSpeed(LEFT, LINE_DETECTION_SPEED); 
    MOTOR_SetSpeed(RIGHT, LINE_DETECTION_SPEED);
}

/** 
 * Function makes the robot follow the white line.
 * 
 * @param[in] distance The distance for which the robot must follow the line. 
 *      If distance is equal to 0 the robot will follow the line indefinetly.
 */ 
void detect_line(float distance)
{
    reset_encoders();

    MOTOR_SetSpeed(LEFT, LINE_DETECTION_SPEED); 
    MOTOR_SetSpeed(RIGHT, LINE_DETECTION_SPEED);

    float nb_wheel_turn = distance / WHEEL_SIZE_CM;
    long nb_pulses = nb_wheel_turn * PULSES_BY_TURN;

    while (true)
    {
        delay(10);

        // If the robot must follow the line only for a specific distance.
        if (distance != 0.0 && ENCODER_Read(LEFT) >= nb_pulses)
        {
            break;
        }

        int detect_value = analogRead(A7);

        // === Corrections ===

        if (detect_value == 733 || detect_value == 441 || detect_value == 1021)
        {
            continue;
        }

        if(detect_value < 291 + 3 && detect_value > 291 - 3) //detection par SENSOR_DROITE
        {
            turn_to_central_sensor(RIGHT);
        }
        else if(detect_value < 583 + 3 && detect_value > 583 - 3) //detection par SENSOR_GAUCHE 
        {
            turn_to_central_sensor(LEFT);
        }
    }

    stop_action();
}
/*
    Function that contains the logic arround the gear. It checks the color of a
    skittle and gets it to where it needs to be. If no skittle is detected a few
    times, the function ends.
*/
void logic_gear()
{
    int attempts = 0;
    int color = INVALID;

    while(attempts < NB_TESTS)
    {
        turn_gear(90);
        delay(500);
        color = get_color();
        delay(500);
        if(color == INVALID)
        {
            attempts++;
        }
        else
        {
            //case where the color is ok
        }
    }
}

void turn_gear(int turn_degrees)
{
    steppermotor.setSpeed(700);    
    steppermotor.step(STEPS_PER_OUT_REV/(360/turn_degrees));
}



/*
    This function returns an int value associated to the color detected by the sensor. 
    The color is read a number of times equal to NB_SCANS_COLOR set in the alex.h file.
    If the same color is detected more than half of the readings. The returned value will
    be a color. Else it will be INVALID (of value 0).
*/
int get_color()
{
    int color[NB_SCANS_COLOR]={INVALID};    // an array to add the detected colors to
    int color_count[NB_COLOR+1] = {0};      // an array to add up the number of times a color was detected

    // loop to detect the colors and count how many of each was detected
    for(int i = 0; i < NB_SCANS_COLOR; i++)
    {
        color[i]=find_color(read_color_once());
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