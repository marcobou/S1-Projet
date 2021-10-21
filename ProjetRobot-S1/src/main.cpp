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
 
const float TAILLE_ROUE_CM = TAILLE_ROUE_ROBOTA * 3.141592;

void avance_tour_roue(float nbTour); 
void avance (float x);
void turn (float angle);

/* 
Fonction de setup pour initialiser le robot
*/
void setup() { 
  // put your setup code here, to run once: 
  Serial.begin(9600); 
  BoardInit(); 
  MOTOR_SetSpeed(LEFT,0); 
  MOTOR_SetSpeed(RIGHT,0); 
  pinMode(28, INPUT);
  Serial.println("Start"); 
} 

void loop() 
{
  bool sw = 0;
  while(sw == 0)
  {
    sw = digitalRead(28);
    delay(1);
  }
  avance(115);
  turn(-45);
  avance(65);
  turn(44);
  avance(335);
  turn(180);
  avance(125);
  turn(-47);
  avance(65);
  turn(47);
  avance(340);

  turn(180);
  turn(180);
  turn(180);
  

  MOTOR_SetSpeed(LEFT, 0);
  MOTOR_SetSpeed(RIGHT, 0);

  while(1);

}

void avance (float x){
  x = (1.0*x/TAILLE_ROUE_CM);
  avance_tour_roue(x);
  
}
void turn (float angle){
  
  long nbPulseRight = 0;
  long nbPulseLeft = 0;
  ENCODER_Reset(RIGHT); // Variables contenant les dernières valeures d'encodeurs
  ENCODER_Reset(LEFT); 
  float convert = abs(angle/360*PULSE_PAR_ROND);
  Serial.println(convert);
  Serial.println(angle);
  //tourne a gauche
  if (angle<0)
  {
    MOTOR_SetSpeed(RIGHT, BASE_TURN_SPEED);
    //MOTOR_SetSpeed(LEFT, MINUS_BASE_SPEED);
    while(convert>nbPulseRight)
    {
      _delay_us(10);
      nbPulseRight = ENCODER_Read(RIGHT);

      }
    MOTOR_SetSpeed(RIGHT, 0);
   // MOTOR_SetSpeed(LEFT, 0);

  }
   //tourne a droite
   else if (angle>0 && angle!=180){
     MOTOR_SetSpeed(LEFT, BASE_TURN_SPEED);
     while(convert>nbPulseLeft)
     {
      _delay_us(10);
      nbPulseLeft = ENCODER_Read(LEFT);
        }
      MOTOR_SetSpeed(LEFT, 0);
   }
   else if (angle == 180){
     MOTOR_SetSpeed(LEFT, BASE_TURN_SPEED);
     MOTOR_SetSpeed(RIGHT, -BASE_TURN_SPEED);
     while(((convert-200)/2)>nbPulseLeft)
     {
      _delay_us(10);
      nbPulseLeft = ENCODER_Read(LEFT);
        }
      MOTOR_SetSpeed(LEFT, 0);
      MOTOR_SetSpeed(RIGHT, 0);

   }


_delay_us(10);
//delay(1000);
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
  MOTOR_SetSpeed(0,0); 
  MOTOR_SetSpeed(1,0); 
  nbPulseLeft = ENCODER_ReadReset(LEFT);
  nbPulseRight = ENCODER_ReadReset(RIGHT);
  _delay_us(10);
}