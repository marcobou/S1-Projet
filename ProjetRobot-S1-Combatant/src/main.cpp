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
void turn_off_del_all();
void initColorSensor();
CustomColor read_color();
int find_color(CustomColor);
void hit_object(float obj_distance);
bool object_detection(int *nb_detection, float last_distances[]);
void stop_action();
void stop_motors();
void reset_encoders();
void detect_line(float distance, bool get_ball, bool detect_whistle, bool search_object);
bool detect5khz();
void bring_ball(int color_no);
void move_arm(int angle);
void turn_on_del_depending_color(int color);
void turn_to_central_sensor(int direction);
void logic_gear();
void turn_gear(int turn_degrees);
int get_color();

void setup() 
{
    Serial.begin(9600); 

    BoardInit(); 

    stop_motors();
    reset_encoders();

    pin_setup();

    SERVO_Enable(SERVO_MOTOR_ID);
    delay(100);
    move_arm(180);
    move_arm(150);
    move_arm(180);
    SERVO_Disable(SERVO_MOTOR_ID);

    turn_off_del_all();

    initColorSensor();

    delay(500);

    Serial.println("Start");
}

void loop() {
    bool sw = 0;

    while(1)
    {
        while(sw == 0)
        {
            sw = digitalRead(28);
            delay(1);
        }
        sw = 0;

        logic_gear();
    }
    
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
        // angle < 0 = tourner ?? gauche, angle > 0 = tourner ?? droite
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
        // tourne de 180

        MOTOR_SetSpeed(LEFT, BASE_TURN_SPEED);
        MOTOR_SetSpeed(RIGHT, -BASE_TURN_SPEED);

        while(((convert - 200) / 2) > nb_pulses)
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
 * @param[in] distance The distance for which the robot must go forward.
 */
void forward(float distance) 
{ 
    float nb_wheel_turn = 1.0f * distance / WHEEL_SIZE_CM;

    reset_encoders(); 
    
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
 
    stop_action();
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

    // Whistle
    pinMode(WHISTLE_PIN, INPUT); //A0
    pinMode(NOISE_PIN, INPUT); //A1

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
 * Turn on a del depending on a detected color.
 * 
 * @param[in] color The detected color.
 */ 
void turn_on_del_depending_color(int color)
{
    if (color == RED)
    {
        turn_on_del(RED_DEL_PIN);
    }
    else if (color == YELLOW)
    {
        turn_on_del(YELLOW_DEL_PIN);
    }
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
    pinMode(COLOR_SENSOR_PIN, OUTPUT);
    digitalWrite(13, 1);

    // try to connect to the color sensor
    while (!tcs.begin())
    {
        Serial.println("No TCS34725 found ... check your connections");
        // cut power to the sensor and turn it back on until it works
        digitalWrite(COLOR_SENSOR_PIN, 0);
        delay(1000);
        digitalWrite(COLOR_SENSOR_PIN, 1);
    } 
}
/*
This function returns a CustomColor objet with the Red, Green and Blue relative
values of what is under the sensor.
*/
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
/**
* Function that detects the whistle baby whistle baby here we go!
 * 
 *@param[out] whistleIsDetected True if whislte is detected, false if not.
*/
bool detect5khz()
{
      
    float whistle_tension = analogRead(WHISTLE_PIN);
    float ambiant_noise = analogRead(NOISE_PIN);
    float difference = whistle_tension - ambiant_noise;

    if (difference < WHISTLE_DIFFERENCE)
    {
        turn_on_del(RED_DEL_PIN);
        turn_on_del(YELLOW_DEL_PIN);
        turn_on_del(BLUE_DEL_PIN);

        return true;
    }
    else
    {
        return false;
    }    
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
 * Function that makes the robot hit an object.
 * 
 * @param[in] obj_distance The distance between the robot and the object.
 */
void hit_object(float obj_distance)
{
    stop_action();

    turn(90);
    forward(obj_distance);

    turn(180);
    turn(180);
    turn(180);

    forward(obj_distance - 5);
    
    turn_to_central_sensor(RIGHT);

    stop_action();
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
    delay(90);
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
 * Function that must be called at the end of each action to stop the motors and reset the encoders.
 */ 
void stop_action()
{
    stop_motors(); 
    ENCODER_Reset(LEFT);
    ENCODER_Reset(RIGHT);
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
 * @param[in] get_ball True if the robot must exit the track to get the ball.
 * @param[in] detect_whistle True if the robot must detect the whistle while following the line.
 * @param[in] search_object True if the robot must search for the object while following the line.
 */ 
void detect_line(float distance, bool get_ball, bool detect_whistle, bool search_object)
{
    reset_encoders();

    MOTOR_SetSpeed(LEFT, LINE_DETECTION_SPEED); 
    MOTOR_SetSpeed(RIGHT, LINE_DETECTION_SPEED);

    float nb_wheel_turn = distance / WHEEL_SIZE_CM;
    long nb_pulses = nb_wheel_turn * PULSES_BY_TURN;

    int nb_detection = 0;
    float last_distances [MIN_DETECTION + 1] = {};

    while (true)
    {
        delay(10);

        // If the robot must follow the line only for a specific distance.
        if (distance != 0.0 && !get_ball && !detect_whistle &&
            !search_object && ENCODER_Read(LEFT) >= nb_pulses)
        {
            break;
        }

        // If the robot must detect the whistle while following the line.
        if (detect_whistle && detect5khz())
        {
            stop_motors();
            delay(500);
            detect_line(0.0, false, false, true);

            break;
        }

        // If the robot must search for the object while following the line.
        if (search_object && object_detection(&nb_detection, last_distances))
        {
            turn_on_del(GREEN_DEL_PIN);

            hit_object(get_average(last_distances, MIN_DETECTION + 1) + 10);

            turn_off_del_all();

            break;
        }

        int detect_value = analogRead(A7);

        // If the robot must go toward the ball.
        if (get_ball && detect_value == 733)
        {
            stop_motors();

            turn(-90);
            detect_line(TRACK_TO_COLOR - 15, false, false, false);
            forward(15);

            break;
        }

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

/**
 * Function that makes the robot move from the color zone to the ball and then brings
 * the ball to the correct area and come back to the track.
 * 
 * @param[in] color The color of the area to which he must bring the ball.
 */
void bring_ball(int color)
{
    forward(COLOR_TO_BALL);

    move_arm(80);

    /*if (color == BLUE)
    {
        turn(-90);
        forward(BALL_TO_TURNING_POINT);
        turn(93);
    }
    else if (color == YELLOW)
    {
        turn(92);
        forward(BALL_TO_TURNING_POINT);
        turn(-91);
    }
    else if (color == RED)
    {
        turn(3);
    }*/

    float distance_forward = color == RED ? BALL_TO_ZONE + WHEEL_SIZE_CM : BALL_TO_ZONE;

    forward(distance_forward);

    delay(1000);
    move_arm(180);

//
    MOTOR_SetSpeed(LEFT,-0.3);
    MOTOR_SetSpeed(RIGHT,-0.3);
    delay(1000);
    MOTOR_SetSpeed(LEFT,0);
    MOTOR_SetSpeed(RIGHT,0);
//

    turn(180);
    forward(BALL_TO_ZONE);
/*
    if (color == BLUE)
    {
        turn(-90);
        forward(BALL_TO_TURNING_POINT + 30);
        turn(90);
    }
    else if (color == YELLOW)
    {
        turn(90);
        forward(BALL_TO_TURNING_POINT - 10);
        turn(-92);
    }
    else if (color == RED)
    {
        turn(-45);
        forward(WHEEL_SIZE_CM);
        turn(45);
    }*/

    forward(COLOR_TO_BALL + TRACK_TO_COLOR - CORRECTION_FOR_BACKWARDS);
    turn(-40);
}

/**
 * Function to make the arm move to the specified angle.
 * 
 * @param[in] angle The angle of the arm at the end. 180 puts the arm in the air and 0 is 
 *      straight toward the ground.
 */ 
void move_arm(int angle)
{
    SERVO_Enable(SERVO_MOTOR_ID);
    delay(100);
    SERVO_SetAngle(SERVO_MOTOR_ID, angle);
    delay(500);
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
        color = get_color();

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
    int steps_to_do = STEPS_PER_OUT_REV * turn_degrees / 360;
    steppermotor.setSpeed(1023);    
    steppermotor.step(-steps_to_do);
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
