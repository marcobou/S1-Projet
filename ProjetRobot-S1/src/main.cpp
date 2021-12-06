#include <Arduino.h>
#include <math.h>
#include <Wire.h>
#include "Adafruit_TCS34725.h"
#include "defines.h"
#include <LibRobus.h>
#include <Stepper.h>
#include <LiquidCrystal_I2C.h>
#include <SharpIR.h>

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

// ========================= FUNCTION SIGNATURE =========================

// ===== MOVE =====
void turn (float angle);
void forward (float distance);
void turn_to_central_sensor(int direction);
// ===== SETUP =====
void pin_setup();
void init_color_sensor();
void stop_action();
void stop_motors();
void reset_encoders();
void set_jar_detection_variables();
void reset_cpt_skittles();
void lcd_init();
// ===== COLOR =====
CustomColor read_color_once();
int find_color(CustomColor);
int get_color();
// ===== SKITTLES DROP =====
void detect_line();
bool jar_detection();
// ===== LCD =====
void show_menu(char title[], int cpt);
void switch_menu();
void on_click_btn_lcd();
void update_menu(int nb, int color);
void update_skittles_cpt(int color);
// ===== BUZZER =====
void play_buzzer();
// ===== GEAR =====
void turn_gear(int turn_degrees);
bool logic_gear(bool initial_turn = true);

// ========================= GLOBAL VARIABLES =========================

// Color sensor
Adafruit_TCS34725 tcs = Adafruit_TCS34725(TCS34725_INTEGRATIONTIME_50MS, TCS34725_GAIN_4X);
// Motor for the gear
Stepper steppermotor(STEPS_PER_REV, STEPPER_PIN1, STEPPER_PIN2, STEPPER_PIN3, STEPPER_PIN4);

const float WHEEL_SIZE_CM = WHEEL_SIZE_ROBOTA * 3.141592;

// ===== LCD =====

// The index of the current menu shown on the LCD. Goes from 0 to 4 (so 5 menus)
int menu_index;
// The count of green Skittles detected
int cpt_skittles_green;
// The count of red Skittles detected
int cpt_skittles_red;
// The count of yellow Skittles detected
int cpt_skittles_yellow;
// The count of orange Skittles detected
int cpt_skittles_orange;
// The count of purple Skittles detected
int cpt_skittles_purple;
// The LCD screen
LiquidCrystal_I2C lcd = LiquidCrystal_I2C(0x27, 16, 2);
// Array mapping a Skittles' color to a menu index 
int skittles_colors[5] = {ORANGE, GREEN, RED, YELLOW, PURPLE};

// ===== JAR DETECTION =====

// Infrared sensor for the distance, will detect the jars
SharpIR IR_sensor = SharpIR(1, IR_SENSOR_PIN);
// The color of the Skittles to drop
int current_skittle_color;
// The color associated to each jar
int jar_color[5] = {0, 0, 0, 0, 0};
// The index of the jar the robot is currently in front of. Goes from 1 to 5 (so 5 jars)
int jar_index;
// The number of times the robot detected the jar at the end of the line
int nb_jar_line_end;
// Tells us if the jar index should increase or decrease
bool is_jar_index_increasing;
// Number of times the robot detected nothing (so between each jar)
int nb_no_detection;
// The previous distance measured, will be used to verify that the current measured distance is valid
int last_distance;
// The second previous distance measured, will be used to verify that the current measured distance is valid
int previous_last_distance;

void setup()
{ 
    Wire.begin();
    Serial.begin(9600); 

    BoardInit(); 

    stop_motors();
    reset_encoders();

    pin_setup();

    reset_cpt_skittles();

    lcd_init();

    set_jar_detection_variables();

    init_color_sensor();
} 

void loop() 
{
    bool lcd_button_is_pressed = false;

    while(true)
    {
        // ===== START SEQUENCE BTN =====

        if(digitalRead(RESET_BTN_PIN) == LOW)
        {
            break;
        }

        // ===== ADJUST GEAR =====

        if(digitalRead(LEFT_BUMPER_PIN))
        {
            turn_gear(1);
            delay(10);
        }

        // ===== LCD BTN MENU =====

        if(digitalRead(LCD_MENU_BTN_PIN) == LOW && !lcd_button_is_pressed)
        {
            lcd_button_is_pressed = true;
            on_click_btn_lcd();
        }

        if (digitalRead(LCD_MENU_BTN_PIN) == HIGH && lcd_button_is_pressed)
        {
            lcd_button_is_pressed = false;
        }

        delay(1);
    }

    delay(10);

    if (logic_gear())
    {
        detect_line();
        stop_motors();
        play_buzzer();
    }
    else
    {
        play_buzzer();
    }
    
}

// ========================= MOVE =========================

/**
 * Function to make the robot turn.
 * 
 * @param[in] angle The angle at which the robot must turn. A negative value will make it turn to the left.
 */
void turn (float angle)
{
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
 * @param[in] distance The distance in cm for which the robot must go forward. A negative
 *      value will make the robot go backward
 */
void forward(float distance) 
{ 
    if (distance == 0)
    {
        return;
    }

    int go_forward = 1;

    if (distance < 0)
    {
        go_forward = -1;
        distance = -distance;
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
    float reduce_speed = 0.0001f;
    float current_speed = BASE_SPEED;

    while(pulses_left < pulse_to_do)
    {
        // Take the absolute value because, when going backward, the encoders return a negative value
        pulses_left = abs(ENCODER_Read(LEFT));
        pulses_right = abs(ENCODER_Read(RIGHT));

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

        speed_ratio = (pulses_left - pulses_right) * CORRECTION_FACTOR;
        MOTOR_SetSpeed(LEFT, current_speed * go_forward); 
        MOTOR_SetSpeed(RIGHT, (current_speed + speed_ratio) * go_forward);

        _delay_us(100);
    }
 
    stop_action();
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

    while(detect_value > MIDDLE_SENSOR_VALUE + 3 || detect_value < MIDDLE_SENSOR_VALUE - 3)
    {
        delay(1);
        detect_value = analogRead(A7);
    }

    MOTOR_SetSpeed(LEFT, LINE_DETECTION_SPEED); 
    MOTOR_SetSpeed(RIGHT, LINE_DETECTION_SPEED);
}

// ========================= SETUP =========================

/**
 * Function that configure the different pins.
 */
void pin_setup()
{
    pinMode(LCD_MENU_BTN_PIN, INPUT_PULLUP);
    pinMode(BUZZER_PIN_NO, OUTPUT);
    pinMode(LEFT_BUMPER_PIN,INPUT);
    pinMode(RESET_BTN_PIN, INPUT_PULLUP);
}

/**
 * Initialize the color sensor.
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
 * Set the initial values of the variables used for the jar detection.
 */
void set_jar_detection_variables()
{
    current_skittle_color = INVALID;
    jar_index = 0;
    nb_jar_line_end = 0;
    is_jar_index_increasing = true;
    nb_no_detection = 0;
    last_distance = 0;
    previous_last_distance = 0;

    for (int i = 0; i < 5; i++)
    {
        jar_color[i] = 0;
    }
}

/**
 * Reset the counters for the number of skittles and the menu index.
 */
void reset_cpt_skittles()
{
    cpt_skittles_orange = 0;
    cpt_skittles_green = 0;
    cpt_skittles_red = 0;
    cpt_skittles_yellow = 0;
    cpt_skittles_purple = 0;
    menu_index = 0;
}

/**
 * Initialize the LCD screen and show the first menu.
 */ 
void lcd_init()
{
    lcd.init();
    lcd.backlight();

    switch_menu();
}

// ========================= COLOR =========================

/**
 * Function to take a single reading of the color in front of the sensor.
 * 
 * @param[out] color A CustomColor object of what is in front of the sensor.
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

/**
 * Function to transform a CustomColor object to an int representing the color. Map the object
 * to a defined int of the defines.h file.
 * 
 * @param[in] customColor The CustomColor to transform.
 * @param[out] color The int color.
 */
int find_color(CustomColor color)
{
    int color_match = INVALID;
    
    // color is red
    if((color.Red >= RED_MIN_RED && color.Red <= RED_MAX_RED) &&
    (color.Green >= RED_MIN_GREEN && color.Green <= RED_MAX_GREEN) &&
    (color.Blue >= RED_MIN_BLUE && color.Blue <= RED_MAX_BLUE))
    {
        color_match = RED;
    }
    // color is green
    else if((color.Red >= GREEN_MIN_RED && color.Red <= GREEN_MAX_RED) &&
    (color.Green >= GREEN_MIN_GREEN && color.Green <= GREEN_MAX_GREEN) &&
    (color.Blue >= GREEN_MIN_BLUE && color.Blue <= GREEN_MAX_BLUE))
    {
        color_match = GREEN;
    }
    // color is purple
    else if((color.Red >= PURPLE_MIN_RED && color.Red <= PURPLE_MAX_RED) &&
    (color.Green >= PURPLE_MIN_GREEN && color.Green <= PURPLE_MAX_GREEN) &&
    (color.Blue >= PURPLE_MIN_BLUE && color.Blue <= PURPLE_MAX_BLUE))
    {
        color_match = PURPLE;
    }
    // color is yellow
    else if((color.Red >= YELLOW_MIN_RED && color.Red <= YELLOW_MAX_RED) &&
    (color.Green >= YELLOW_MIN_GREEN && color.Green <= YELLOW_MAX_GREEN) &&
    (color.Blue >= YELLOW_MIN_BLUE && color.Blue <= YELLOW_MAX_BLUE))
    {
        color_match = YELLOW;
    }
    // color is orange
    else if((color.Red >= ORANGE_MIN_RED && color.Red <= ORANGE_MAX_RED) &&
    (color.Green >= ORANGE_MIN_GREEN && color.Green <= ORANGE_MAX_GREEN) &&
    (color.Blue >= ORANGE_MIN_BLUE && color.Blue <= ORANGE_MAX_BLUE))
    {
        color_match = ORANGE;
    }
    // color is unknown
    else
    {
        Serial.println("Color is UNKNOWN");
    }
    return color_match;
}

/**
 * This function returns an int value associated to the color detected by the sensor. 
 * The color is read a number of times equal to NB_SCANS_COLOR set in the defines.h file.
 * If the same color is detected more than half of the readings. The returned value will
 * be a color. Else it will be INVALID (of value 0).
 * 
 * @param[out] color The read color of INVALID.
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

// ========================= SKITTLES DROP =========================

/** 
 * Function makes the robot follow the white line.
 */ 
void detect_line()
{
    reset_encoders();

    MOTOR_SetSpeed(LEFT, LINE_DETECTION_SPEED); 
    MOTOR_SetSpeed(RIGHT, LINE_DETECTION_SPEED);

    bool lcd_button_is_pressed = false;

    while (true)
    {
        delay(10);

        int detect_value = analogRead(A7);

        // === LCD btn menu ===
        if(digitalRead(LCD_MENU_BTN_PIN) == LOW && !lcd_button_is_pressed)
        {
            lcd_button_is_pressed = true;
            on_click_btn_lcd();
        }

        if (digitalRead(LCD_MENU_BTN_PIN) == HIGH && lcd_button_is_pressed)
        {
            lcd_button_is_pressed = false;
        }

        // === Corrections ===
        
        if(detect_value < RIGHT_SENSOR_VALUE + 3 && detect_value > RIGHT_SENSOR_VALUE - 3)
        {
            // Right sensor triggered

            turn_to_central_sensor(RIGHT);
        }
        else if(detect_value < LEFT_SENSOR_VALUE + 3 && detect_value > LEFT_SENSOR_VALUE - 3)
        {
            // Left sensor triggered

            turn_to_central_sensor(LEFT);
        }
        
        // --- Jar detection ---

        if (jar_detection())
        {
            // If the robot is in front of the jar of the same color as the Skittles inside the gear.

            // stop the robot
            stop_motors();
            reset_encoders();

            // Make the robot move forward to align the ramp to the jar
            forward(DISTANCE_IR_TO_RAMP);

            // Drop the Skittles into the jar by turning it once
            turn_gear(90);

            // Wait 3 seconds to give the Skittles time to drop
            delay(3000);

            // Move the robot back to its original location
            forward(-DISTANCE_IR_TO_RAMP);

            // Check the color of the Skittles in front of the sensor. No need to turn the gear
            // for the first read because of the turn for the drop. If no Skittles we break from
            // the loop so that the robot stops following the line
            if (!logic_gear(false))
            {
                break;
            }

            delay(2000);

            MOTOR_SetSpeed(LEFT, LINE_DETECTION_SPEED);
            MOTOR_SetSpeed(RIGHT, LINE_DETECTION_SPEED);
        }
    }

    stop_action();
}

/**
 * Function that must be called in a loop, it will detect the jar and index them with the appropriate color.
 * 
 * @param[out] is_color_jar True if the robot is in front of the jar associated to the current Skittles color.
 */
bool jar_detection()
{
    int obj_distance = IR_sensor.getDistance();

    if (obj_distance >= previous_last_distance - 1 && obj_distance <= previous_last_distance + 1 && 
        obj_distance >= last_distance - 1 && obj_distance <= last_distance + 1 && 
        obj_distance >= JAR_DISTANCE - 5 && obj_distance <= JAR_DISTANCE + 5)
    {
        // detected distance is basically the same as the previous 2 values (so 3 same values in a row) 
        // and it is in the expected range
        
        if (nb_no_detection > 15)
        {
            // if the robot is in front of a new jar (not detecting the same jar)
            nb_no_detection = 0;

            if ((jar_index == 5 || (jar_index == 1 && !is_jar_index_increasing)) && nb_jar_line_end < 2)
            {
                // first or fifth jar is detected for the second or third time (after the turns)
                nb_jar_line_end++;
            }
            else if (jar_index == 5 && nb_jar_line_end == 2)
            {
                // is in front of the fourth jar after passing the fifth one
                nb_jar_line_end = 0;
                is_jar_index_increasing = false;
            }
            else if (jar_index == 1 && nb_jar_line_end == 2)
            {
                // is in front of the second jar after passing the first one once a whole turn is finished
                nb_jar_line_end = 0;
                is_jar_index_increasing = true;
            }

            if (nb_jar_line_end == 0)
            {
                // if is not at an end of the jar line 

                if (is_jar_index_increasing)
                {
                    jar_index++;
                }
                else
                {
                    jar_index--;
                }

                if (jar_color[jar_index - 1] == 0)
                {
                    // if never passed in front of this jar before

                    jar_color[jar_index - 1] = skittles_colors[jar_index - 1];
                }
            }
        }

        if (jar_index != 0 && current_skittle_color == jar_color[jar_index - 1])
        {
            // If the color of the skittle inside of the gear correspond to the jar
            // the robot is currently in front of.
            
            return true;
        }
    }
    else
    {
        // If no jar was detected that means the robot is between 2 jars and he'll encounter a new one.
        
        nb_no_detection++;
    }

    previous_last_distance = last_distance;
    last_distance = obj_distance;

    return false;
}

// ========================= LCD =========================

/**
 * Show a menu (title on first line and number on second) on the LCD.
 * 
 * @param[in] title The title of the menu.
 * @param[in] cpt The number of the color that is shown by the menu.
 */
void show_menu(char title[], int cpt)
{
    lcd.setCursor(0,0);
    lcd.print(title);
    lcd.setCursor(0,1);
    lcd.print(cpt);
}

/**
 * Switch the shown menu to the next one.
 */ 
void switch_menu()
{
    lcd.clear();

    switch(menu_index)
    {
        case 0:
            show_menu("Skittles orange", cpt_skittles_orange);
            break;
        case 1:
            show_menu("Skittles vert", cpt_skittles_green);
            break;
        case 2:
            show_menu("Skittles rouge", cpt_skittles_red);
            break;
        case 3:
            show_menu("Skittles jaune", cpt_skittles_yellow);
            break;
        case 4:
            show_menu("Skittles mauve", cpt_skittles_purple);
            break;
        default:
            Serial.println("Erreur dans le switch de menu");
    }
}

/**
 * Function that is called when the button for changing the LCD menu is pressed.
 */ 
void on_click_btn_lcd()
{
    // if it's the last menu, we set it back to the first to loop.
    if(menu_index == 4)
    {
        menu_index = 0;
    }
    else
    {
        menu_index++;
    }

    switch_menu();
}

/**
 * Simply update the shown menu. Will be called if the sorted skittle is of the 
 * same color as the one presented on the LCD.
 * 
 * @param[in] nb The number of skittles to show on the second line of the LCD.
 * @param[in] color The color of the detected skittle.
 */ 
void update_menu(int nb, int color)
{
    if (skittles_colors[menu_index] == color)
    {
        lcd.setCursor(0, 1);
        lcd.print(" ");
        lcd.print(nb);
    }
}

/**
 * Update the appropriate skittles counter depending on the color of the detected skittle.
 * 
 * @param[in] color The color of the detected skittle.
 */ 
void update_skittles_cpt(int color)
{
    switch (color)
    {
        case ORANGE:
            cpt_skittles_orange++;
            update_menu(cpt_skittles_orange, color);
            break;
        case GREEN:
            cpt_skittles_green++;
            update_menu(cpt_skittles_green, color);
            break;
        case RED:
            cpt_skittles_red++;
            update_menu(cpt_skittles_red, color);
            break;
        case YELLOW:
            cpt_skittles_yellow++;
            update_menu(cpt_skittles_yellow, color);
            break;
        case PURPLE:
            cpt_skittles_purple++;
            update_menu(cpt_skittles_purple, color);
            break;
        default:
            Serial.println("Erreur dans l'update des compteurs de skittles");
    }
}

// ========================= BUZZER =========================

/**
 * Function to play the buzzer.
 */
void play_buzzer()
{
    digitalWrite(BUZZER_PIN_NO, HIGH);
    delay(1000);
    digitalWrite(BUZZER_PIN_NO, LOW);
}

// ========================= GEAR =========================

/**
 * Function to turn the gear.
 * 
 * @param[in] turn_degrees The number of degrees that the gear must turn.
 */
void turn_gear(int turn_degrees)
{
    steppermotor.setSpeed(700);    
    steppermotor.step(STEPS_PER_OUT_REV/(360/turn_degrees));
}

/**
 * Function that contains the logic arround the gear. It checks the color of a
 * Skittles and gets it to where it needs to be. If no Skittles is detected a few
 * times, the function ends.
 * 
 * @param[in] initial_turn True if the gear must time for the first read.
 * @param[out] skittle_detected True if a Skittles was detected.
 */
bool logic_gear(bool initial_turn = true)
{
    int attempts = 0;
    int color = INVALID;

    while(attempts < NB_TESTS)
    {
        if (initial_turn)
        {
            turn_gear(90);
            delay(500);
        }
        else
        {
            initial_turn = true;
        }
        
        color = get_color();
        delay(500);

        if(color == INVALID)
        {
            attempts++;
        }
        else
        {
            //case where the color is ok
            current_skittle_color = color;
            update_skittles_cpt(current_skittle_color);

            return true;
        }
    }

    return false;
}