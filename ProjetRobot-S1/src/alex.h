
// define des couleurs pour retourner une couleur avec une fonction
#define INVALID 0
#define RED     1
#define GREEN   2
#define PURPLE  3
#define YELLOW  4
#define ORANGE  5

// composantes acceptables pour considérer une couleur comme rouge
#define RED_MAX_RED   85
#define RED_MIN_RED   74
#define RED_MAX_GREEN 80
#define RED_MIN_GREEN 72
#define RED_MAX_BLUE  87  
#define RED_MIN_BLUE  83

// composantes acceptables pour considérer une couleur comme vert
#define GREEN_MAX_RED   60
#define GREEN_MIN_RED   57
#define GREEN_MAX_GREEN 102
#define GREEN_MIN_GREEN 91
#define GREEN_MAX_BLUE  85  
#define GREEN_MIN_BLUE  79

// composantes acceptables pour considérer une couleur comme bleu
#define BLUE_MAX_RED    65
#define BLUE_MIN_RED    59
#define BLUE_MAX_GREEN  88
#define BLUE_MIN_GREEN  84
#define BLUE_MAX_BLUE   94
#define BLUE_MIN_BLUE   90

// composantes acceptables pour considérer une couleur comme jaune
#define YELLOW_MAX_RED    86
#define YELLOW_MIN_RED    76
#define YELLOW_MAX_GREEN  90
#define YELLOW_MIN_GREEN  83
#define YELLOW_MAX_BLUE   77
#define YELLOW_MIN_BLUE   65

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

#define BASE 64 // BASE any number above 64
//Define the output pins of the PCF8574, which are directly connected to the LCD1602 pin.
#define RS BASE+0
#define RW BASE+1
#define EN BASE+2
#define LED BASE+3
#define D4 BASE+4
#define D5 BASE+5
#define D6 BASE+6
#define D7 BASE+7

// === LCD ===
#define LCD_MENU_BTN_PIN 38

// === IR SENSOR ===
#define IR_SENSOR_PIN A3
