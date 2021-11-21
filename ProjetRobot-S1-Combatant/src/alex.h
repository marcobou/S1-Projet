
// defines pour le capteur de couleur

// define des couleurs pour retourner une couleur avec une fonction
#define INVALID 0
#define RED     1
#define GREEN   2
#define PURPLE  3
#define YELLOW  4
#define ORANGE  5

// composantes acceptables pour considérer une couleur comme rouge
#define RED_MAX_RED   0
#define RED_MIN_RED   0
#define RED_MAX_GREEN 0
#define RED_MIN_GREEN 0
#define RED_MAX_BLUE  0 
#define RED_MIN_BLUE  0

// composantes acceptables pour considérer une couleur comme vert
#define GREEN_MAX_RED   0
#define GREEN_MIN_RED   0
#define GREEN_MAX_GREEN 0
#define GREEN_MIN_GREEN 0
#define GREEN_MAX_BLUE  0 
#define GREEN_MIN_BLUE  0

// composantes acceptables pour considérer une couleur comme purple
#define PURPLE_MAX_RED    0
#define PURPLE_MIN_RED    0
#define PURPLE_MAX_GREEN  0
#define PURPLE_MIN_GREEN  0
#define PURPLE_MAX_BLUE   0
#define PURPLE_MIN_BLUE   0

// composantes acceptables pour considérer une couleur comme jaune
#define YELLOW_MAX_RED    0
#define YELLOW_MIN_RED    0
#define YELLOW_MAX_GREEN  0
#define YELLOW_MIN_GREEN  0
#define YELLOW_MAX_BLUE   0
#define YELLOW_MIN_BLUE   0

// composantes acceptables pour considérer une couleur comme orange
#define ORANGE_MAX_RED    0
#define ORANGE_MIN_RED    0
#define ORANGE_MAX_GREEN  0
#define ORANGE_MIN_GREEN  0
#define ORANGE_MAX_BLUE   0
#define ORANGE_MIN_BLUE   0

// defines for movements
#define PULSES_BY_TURN 3200             // Number of ticks/pulses that the encoders read each time the wheels finish a turn
#define PULSES_BY_CIRCLE 7979 * 2
#define WHEEL_SIZE_ROBOTA 7.63
#define WHEEL_SIZE_ROBOTB 7.55
 
#define BASE_SPEED 0.2                  // Base speed of the robot when going forward
#define BASE_TURN_SPEED 0.3             // Base speed of the robot when turning
#define CORRECTION_FACTOR 0.0006        // Correction factor for the PID


// PIN numbers for the DEL, 50-51-52 are already taken by ROBUS
#define RED_DEL_PIN 53
#define BLUE_DEL_PIN 48
#define YELLOW_DEL_PIN 47
#define GREEN_DEL_PIN 49

// Pin for sensors
#define COLOR_SENSOR_PIN    13
#define NB_SCANS_COLOR      10
#define NB_COLOR            5

// pins for the whistle
#define WHISTLE_PIN A0
#define NOISE_PIN   A1 
#define WHISTLE_DIFFERENCE 200

// === LINE DETECTION ===
#define LINE_PIN A7
#define LINE_DETECTION_SPEED 0.4
#define LINE_CORRECTION_SPEED 0.4
#define COUNTER_LINE_CORRECTION 0.1

// === OBJECT DETECTION ===
#define SONAR_ID 0
#define MARGIN_ERROR_DISTANCE 0.05
#define MIN_DETECTION 2                 // Mininum number of detection that it takes for the robot to decide that he detected the object.
#define MAX_DETECTION 12                // Maximum number of detection that it takes for the robot to decide that he did NOT detect the object.
#define DETECTION_TURN_SPEED 0.2
#define WALL_DISTANCE 88
#define MAX_DISTANCE 50
#define DISTANCE_WHEEL_TO_SONAR 13

// ServoMotor
#define SERVO_MOTOR_ID 0
#define ARM_LENGTH 15

// Distances
#define TRACK_TO_COLOR 40
#define COLOR_TO_BALL 37
#define BALL_TO_ZONE 200
#define BALL_TO_TURNING_POINT 25
#define CORRECTION_FOR_BACKWARDS 40

// Engrenage
#define NB_ATTEMPTS 3
