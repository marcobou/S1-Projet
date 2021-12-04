
// define des couleurs pour retourner une couleur avec une fonction
#define INVALID 0
#define RED     1
#define GREEN   2
#define PURPLE  3
#define YELLOW  4
#define ORANGE  5

// composantes acceptables pour considérer une couleur comme rouge
#define RED_MAX_RED   108
#define RED_MIN_RED   68
#define RED_MAX_GREEN 78
#define RED_MIN_GREEN 64
#define RED_MAX_BLUE  87 
#define RED_MIN_BLUE  67

// composantes acceptables pour considérer une couleur comme vert
#define GREEN_MAX_RED   69
#define GREEN_MIN_RED   58
#define GREEN_MAX_GREEN 116
#define GREEN_MIN_GREEN 85
#define GREEN_MAX_BLUE  87 
#define GREEN_MIN_BLUE  50

// composantes acceptables pour considérer une couleur comme purple
#define PURPLE_MAX_RED    74
#define PURPLE_MIN_RED    58
#define PURPLE_MAX_GREEN  83
#define PURPLE_MIN_GREEN  75
#define PURPLE_MAX_BLUE   93
#define PURPLE_MIN_BLUE   81

// composantes acceptables pour considérer une couleur comme jaune
#define YELLOW_MAX_RED    106   
#define YELLOW_MIN_RED    84
#define YELLOW_MAX_GREEN  89
#define YELLOW_MIN_GREEN  84
#define YELLOW_MAX_BLUE   75
#define YELLOW_MIN_BLUE   39

// composantes acceptables pour considérer une couleur comme orange
#define ORANGE_MAX_RED    132
#define ORANGE_MIN_RED    85
#define ORANGE_MAX_GREEN  77
#define ORANGE_MIN_GREEN  57
#define ORANGE_MAX_BLUE   70
#define ORANGE_MIN_BLUE   40


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
#define LINE_DETECTION_SPEED 0.4
#define LINE_CORRECTION_SPEED 0.4
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