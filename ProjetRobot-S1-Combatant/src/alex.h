
// defines pour le capteur de couleur

// define des couleurs pour retourner une couleur avec une fonction
#define UNKNOWN 0
#define RED     1
#define GREEN   2
#define BLUE    3
#define YELLOW  4

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
#define BASE_TURN_SPEED 0.3             // Base speed of the robot when turning
#define CORRECTION_FACTOR 0.0004        // Correction factor for the PID

// PIN numbers for the DEL, 50-51-52 are already taken by ROBUS
#define RED_DEL_PIN 53
#define BLUE_DEL_PIN 48
#define YELLOW_DEL_PIN 47
#define GREEN_DEL_PIN 49

// Pin for sensors
#define PIN_COLOR_SENSOR 13