
// define des couleurs pour retourner une couleur avec une fonction
#define INVALID 0
#define RED     1
#define GREEN   2
#define PURPLE  3
#define YELLOW  4
#define ORANGE  5

// composantes acceptables pour considérer une couleur comme rouge
#define RED_MAX_RED   75
#define RED_MIN_RED   66
#define RED_MAX_GREEN 80
#define RED_MIN_GREEN 74
#define RED_MAX_BLUE  89
#define RED_MIN_BLUE  83

// composantes acceptables pour considérer une couleur comme vert
#define GREEN_MAX_RED   62
#define GREEN_MIN_RED   56
#define GREEN_MAX_GREEN 96
#define GREEN_MIN_GREEN 88
#define GREEN_MAX_BLUE  85
#define GREEN_MIN_BLUE  76

// composantes acceptables pour considérer une couleur comme purple
#define PURPLE_MAX_RED    61
#define PURPLE_MIN_RED    57
#define PURPLE_MAX_GREEN  82
#define PURPLE_MIN_GREEN  80
#define PURPLE_MAX_BLUE   91
#define PURPLE_MIN_BLUE   89

// composantes acceptables pour considérer une couleur comme jaune
#define YELLOW_MAX_RED    90   
#define YELLOW_MIN_RED    78
#define YELLOW_MAX_GREEN  88
#define YELLOW_MIN_GREEN  82
#define YELLOW_MAX_BLUE   71
#define YELLOW_MIN_BLUE   59

// composantes acceptables pour considérer une couleur comme orange
#define ORANGE_MAX_RED    94
#define ORANGE_MIN_RED    84
#define ORANGE_MAX_GREEN  75
#define ORANGE_MIN_GREEN  69
#define ORANGE_MAX_BLUE   78
#define ORANGE_MIN_BLUE   69


// defines for movements
#define PULSES_BY_TURN 3200             // Number of ticks/pulses that the encoders read each time the wheels finish a turn
#define PULSES_BY_CIRCLE 7979 * 2
#define WHEEL_SIZE_ROBOTA 7.63
#define WHEEL_SIZE_ROBOTB 7.55
 
#define BASE_SPEED 0.2                  // Base speed of the robot when going forward
#define MIN_SPEED 0.2
#define MAX_SPEED 0.9
#define BASE_TURN_SPEED 0.3             // Base speed of the robot when turning
#define CORRECTION_FACTOR 0.0006        // Correction factor for the PID


// Pin for sensors
#define COLOR_SENSOR_PIN 13

// === LINE DETECTION ===
#define LINE_PIN A7
#define LINE_DETECTION_SPEED 0.3
#define LINE_CORRECTION_SPEED 0.3
#define COUNTER_LINE_CORRECTION 0.1

// === OBJECT DETECTION ===
#define SONAR_ID 0
#define MARGIN_ERROR_DISTANCE 0.05
#define MIN_DETECTION 2                 // Mininum number of detection that it takes for the robot to decide that he detected the object.
#define MAX_DISTANCE 50

#define FRONT_BUMPER_PIN 28

// Number of steps per internal motor revolution 
#define STEPS_PER_REV   32
 
//  Amount of Gear Reduction
#define GEAR_RED        64
 
// Number of steps per geared output rotation
#define STEPS_PER_OUT_REV (STEPS_PER_REV * GEAR_RED)

// Number of times the gear rotates to see if skittles are left before stopping
#define NB_TESTS    3

// Number of times the color is scanned each time and total number of colors
#define NB_SCANS_COLOR      5
#define NB_COLOR            5

// Pins for the stepper motor
#define STEPPER_PIN1    8
#define STEPPER_PIN2    10
#define STEPPER_PIN3    9
#define STEPPER_PIN4    11

// === LCD ===
#define LCD_MENU_BTN_PIN 38

// === IR SENSOR ===
#define IR_SENSOR_PIN A3
#define JAR_DISTANCE 17
#define DISTANCE_IR_TO_RAMP 22

// === BUZZER ===
#define BUZZER_PIN_NO 48

// === Reset ===
#define RESET_BTN_PIN 39
