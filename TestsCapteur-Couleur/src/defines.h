namespace defines
{
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

// Number of steps per internal motor revolution 
#define STEPS_PER_REV   32
 
//  Amount of Gear Reduction
#define GEAR_RED        64
 
// Number of steps per geared output rotation
#define STEPS_PER_OUT_REV (STEPS_PER_REV * GEAR_RED)

#define NB_TESTS    3

#define NB_SCANS_COLOR      5
#define NB_COLOR            5
}
