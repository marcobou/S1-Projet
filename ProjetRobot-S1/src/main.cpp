#include <Arduino.h>
#include <LibRobus.h>
#include <math.h>

#define ENCODE_TOUR_ROUE 3200     // Nombre de ticks d'encodeur pour faire un tour
#define PULSE_PAR_ROND 7979*2
#define TAILLE_ROUE_PO 9.42477796 // Taille des roues en pouces
#define TAILLE_ROUE_ROBOTA 7.63
#define TAILLE_ROUE_ROBOTB 7.55
 
#define BASE_SPEED 0.2 
#define BASE_TURN_SPEED 0.3                 // Vitesse de base
#define MINUS_BASE_SPEED -0.4           // Vitesse de base negative
#define QUARTER_BASE_SPEED BASE_SPEED/4 // 1/4 de la vitesse de base pour départs et arrets plus doux
#define FACTEUR_CORRECTION 0.0004       // Facteur de correction pour le PID
#define CORRECTION_PAR_TOUR 8           // Nombre de fois que le PID ajuste les valeurs de vitesses par tour
#define SIFFLET A0
#define NOISE A1 

const float TAILLE_ROUE_CM = TAILLE_ROUE_ROBOTA * 3.141592;

void avance_tour_roue(float nbTour); 
void avance (float distance);
void turn (float angle);
bool detect5khz();
void setupAlex();

/* 
Fonction de setup pour initialiser le robot
*/
void setup()
{ 
    // put your setup code here, to run once: 
    Serial.begin(9600); 
    BoardInit(); 
    MOTOR_SetSpeed(LEFT, 0); 
    MOTOR_SetSpeed(RIGHT, 0); 
    pinMode(28, INPUT);
    Serial.println("Start"); 
    
} 

void loop() 
{
    
   /*
    bool sw = 0;
    while(sw == 0)
    {
        sw = digitalRead(28);
        delay(1);
    }
    */
   
    
/*
    turn(180);
    delay(1000);
    turn(90);
    delay(1000);
    turn(-90);
*/
    MOTOR_SetSpeed(LEFT, 0);
    MOTOR_SetSpeed(RIGHT, 0);

    //while(1);
}

void avance (float distance)
{
  avance_tour_roue(1.0f * distance / TAILLE_ROUE_CM); 
}

void turn (float angle){
    long nbPulses = 0;

    // Variables contenant les dernières valeurs d'encodeurs
    ENCODER_Reset(RIGHT);
    ENCODER_Reset(LEFT); 

    float convert = abs(angle /360 * PULSE_PAR_ROND);

    //Serial.println(convert);
    //Serial.println(angle);

    if (angle != 180)
    {
        // angle < 0 = tourner à gauche, angle > 0 = tourner à droite
        int turn_motor = angle < 0 ? RIGHT : LEFT;

        MOTOR_SetSpeed(turn_motor, BASE_TURN_SPEED);

        while(convert > nbPulses)
        {
            _delay_us(10);
            nbPulses = ENCODER_Read(turn_motor);
        }

        MOTOR_SetSpeed(turn_motor, 0);
    }
    else
    {
        // tourne de 180

        MOTOR_SetSpeed(LEFT, BASE_TURN_SPEED);
        MOTOR_SetSpeed(RIGHT, -BASE_TURN_SPEED);

        while(((convert - 200) / 2) > nbPulses)
        {
            _delay_us(10);
            nbPulses = ENCODER_Read(LEFT);
        }

        MOTOR_SetSpeed(LEFT, 0);
        MOTOR_SetSpeed(RIGHT, 0);
    }

    _delay_us(10);
 }

/*
Cette fonction prend en entrée un nombre entier et fait tourner les
roues un nombre de tour égal a ce nombre entier
*/
void avance_tour_roue(float nbTour) 
{ 
    long nbPulseRight = 0;
    long nbPulseLeft = 0;

    ENCODER_Reset(RIGHT); // Variables contenant les dernières valeures d'encodeurs
    ENCODER_Reset(LEFT); 
    
    float speedRatio = 0; // Ratio utilisé pour ajuster le rapport de vitesse entre les deux roues

    long pulse_to_do = nbTour * 3200;
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

        if(TAILLE_ROUE_CM == TAILLE_ROUE_ROBOTA)
        {
            speedRatio = (pulses_right-pulses_left)*FACTEUR_CORRECTION;
            MOTOR_SetSpeed(LEFT, current_speed + speedRatio); 
            MOTOR_SetSpeed(RIGHT, current_speed);
        }
        else
        {
            speedRatio = (pulses_left-pulses_right)*FACTEUR_CORRECTION;
            MOTOR_SetSpeed(LEFT, current_speed); 
            MOTOR_SetSpeed(RIGHT, current_speed + speedRatio);
        }

        _delay_us(100);
    }
 
    // Stop les moteurs
    MOTOR_SetSpeed(LEFT, 0); 
    MOTOR_SetSpeed(RIGHT, 0); 
    nbPulseLeft = ENCODER_ReadReset(LEFT);
    nbPulseRight = ENCODER_ReadReset(RIGHT);
    _delay_us(10);
}

bool detect5khz ()
{
      
    float tensionSifflet = analogRead(SIFFLET);
    float bruitAmbiant = analogRead(NOISE);
    float difference = tensionSifflet-bruitAmbiant;

    Serial.println("Voltage:");
    Serial.println(difference);

    if (difference < 220)
    {
       return true;
    }
    else
    {
        return false;
    }
    
}

void setupAlex()
{
    pinMode(SIFFLET, INPUT); //A0
    pinMode(NOISE, INPUT); //A1

}