
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



// defines pour les mouvements
#define ENCODE_TOUR_ROUE 3200     // Nombre de ticks d'encodeur pour faire un tour
#define PULSE_PAR_ROND 7979*2
#define TAILLE_ROUE_PO 9.42477796 // Taille des roues en pouces
#define TAILLE_ROUE 23.97035195

#define BASE_SPEED 0.2 
#define BASE_TURN_SPEED 0.3                 // Vitesse de base
#define MINUS_BASE_SPEED -0.4           // Vitesse de base negative
#define QUARTER_BASE_SPEED BASE_SPEED/4 // 1/4 de la vitesse de base pour départs et arrets plus doux
#define FACTEUR_CORRECTION 0.0004       // Facteur de correction pour le PID
#define CORRECTION_PAR_TOUR 8           // Nombre de fois que le PID ajuste les valeurs de vitesses par tour

#define PIN_COLOR_SENSOR 13