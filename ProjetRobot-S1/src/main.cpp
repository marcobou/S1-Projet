#include <Arduino.h>
#include <LibRobus.h>
#include <math.h>

#define ENCODER_WHEEL_TURN 3200     // Nombre de ticks d'encodeur pour faire un tour
#define PULSE_PER_360 7979*2     // Nombre de ticks d'encodeur pour faire tourner le robot de 360o
#define SIZE_WHEEL_IN 9.42477796 // Taille des roues en pouces
#define SIZE_WHEEL_CM 23.938936  // Taille des roues en cm
 
#define BASE_SPEED 0.3                  // Vitesse de base
#define MINUS_BASE_SPEED -0.3           // Vitesse de base negative
#define QUARTER_BASE_SPEED BASE_SPEED/4 // 1/4 de la vitesse de base pour départs et arrets plus doux
#define CORRECTION_FACTOR 0.0004       // Facteur de correction pour le PID
#define CORRECTION_PER_TURN 4           // Nombre de fois que le PID ajuste les valeurs de vitesses par tour


const int steps_forward[] = {220, -90, 20, 90, 20, 90, 20, };


void forward_nb_turns(float nbTurns); 
void forward_cm (float nbCM);
void turn (float angle);

/* 
Fonction de setup pour initialiser le robot
*/
void setup() 
{ 
  Serial.begin(9600); 
  BoardInit(); 
  MOTOR_SetSpeed(LEFT,0); 
  MOTOR_SetSpeed(RIGHT,0); 
  Serial.println("Start"); 
} 

void loop() 
{
  forward_cm (220);
  turn(-90.0);
  forward_cm (20);
  turn(90);
  forward_cm(20);
  turn(90);
  forward_cm(20);
  turn(-90);
  forward_cm(10);
  turn(45);
  forward_cm(34);
  turn(-90);
  forward_cm(50);
  turn(45);
  forward_cm(27);
  turn(30);
  forward_cm(77.5);
  turn(-30);

  turn(180);

  turn(30);
  forward_cm(77.5);
  turn(-30);
  forward_cm(27);
  turn(-45);
  forward_cm(50);
  turn(90);
  forward_cm(34);
  turn(-45);
  forward_cm(10);
  turn(90);
  forward_cm(20);
  turn(-90);
  forward_cm(20);
  turn(-90);
  forward_cm (20);
  turn(90);
  forward_cm (220);

  MOTOR_SetSpeed(LEFT, 0);
  MOTOR_SetSpeed(RIGHT, 0);

  while(1);
}

void forward_cm (float nbCM)
{
  nbCM = (1.0*nbCM/SIZE_WHEEL_CM);
  forward_nb_turns(nbCM);
  
}
void turn (float angle)
{
  long nbPulseRight = 0;// Variables contenant les dernières valeures d'encodeurs
  long nbPulseLeft = 0;
  ENCODER_Reset(LEFT); 
  ENCODER_Reset(RIGHT); 

  float convert = abs(angle/360*PULSE_PER_360);
  Serial.println(convert);
  Serial.println(angle);

  //tourne a gauche
  if (angle<0)
  {
    MOTOR_SetSpeed(RIGHT, BASE_SPEED);

    while(convert>nbPulseRight)
    {
      _delay_us(10);
      nbPulseRight = ENCODER_Read(RIGHT);
    }
    MOTOR_SetSpeed(RIGHT, 0);
  }
   //tourne a droite
  else if (angle>0 && angle!=180)
  {
    MOTOR_SetSpeed(LEFT, BASE_SPEED);
    while(convert>nbPulseLeft)
    {
      _delay_us(10);
      nbPulseLeft = ENCODER_Read(LEFT);
    }
      MOTOR_SetSpeed(LEFT, 0);
  }
  // 180 degrés
  else if (angle == 180)
  {
    MOTOR_SetSpeed(LEFT, BASE_SPEED);
    MOTOR_SetSpeed(RIGHT, MINUS_BASE_SPEED);

    while((convert/2)>nbPulseLeft)
    {
      _delay_us(10);
      nbPulseLeft = ENCODER_Read(LEFT);    
    }
    MOTOR_SetSpeed(LEFT, 0);
    MOTOR_SetSpeed(RIGHT, 0);
  }
  _delay_us(10);
}

/*
Cette fonction prend en entrée un nombre et fait tourner les
roues un nombre de tour égal a ce nombre
*/
void forward_nb_turns(float nbTurns) 
{ 
  long nbPulseRight = 0;
  ENCODER_Reset(RIGHT); // Variables contenant les dernières valeures d'encodeurs
  long nbPulseLeft = 0;
  ENCODER_Reset(LEFT); 
 
  float speedRatio = 0; // Ratio utilisé pour ajuster le rapport de vitesse entre les deux roues
 
  for(int i = 1; i <= 4; i++) // Utilise le 1er tour pour accélérer
  {
    // Change la vitesse de chaque moteurs à la valeur de base, applique le PID
    MOTOR_SetSpeed(LEFT, ((QUARTER_BASE_SPEED*i)+speedRatio)); 
    MOTOR_SetSpeed(RIGHT, (QUARTER_BASE_SPEED*i)); 

    // Laisse les moteurs tourner jusqu'a la valeur voulue avant de mettre a jour le PID
    while(nbPulseRight <= ENCODER_WHEEL_TURN/CORRECTION_PER_TURN*i) 
    { 
      // Vérifie si la condition est remplie toutes les 100us
      nbPulseRight = ENCODER_Read(RIGHT); 
      nbPulseLeft = ENCODER_Read(LEFT); 
      _delay_us(100); 
    } 

    // Ajuste la commande de vitesse du moteur gauche pour compenser les erreurs materielles
    speedRatio = (nbPulseRight-nbPulseLeft)*CORRECTION_FACTOR; 
    
    // Prints pour tester le PID
    Serial.print("\nPulse right : "); 
    Serial.println(nbPulseRight); 
    Serial.print("Pulse left : "); 
    Serial.println(nbPulseLeft); 
    Serial.print("Speed Ratio : ");
    Serial.println(speedRatio); 
  }

  for(int i = 2; i <= nbTurns*CORRECTION_PER_TURN; i++) // Tours restants
  { 
    // Change la vitesse de chaque moteurs à la valeur de base, applique le PID
    MOTOR_SetSpeed(LEFT, (BASE_SPEED+speedRatio)); 
    MOTOR_SetSpeed(RIGHT, BASE_SPEED);

    // Laisse les moteurs tourner jusqu'a la valeur voulue avant de mettre a jour le PID
    while(nbPulseRight <= ENCODER_WHEEL_TURN/CORRECTION_PER_TURN*i) 
    { 
      // Vérifie si la condition est remplie toutes les 100us
      nbPulseRight = ENCODER_Read(RIGHT); 
      nbPulseLeft = ENCODER_Read(LEFT); 
      _delay_us(100); 
    } 
    
    // Ajuste la commande de vitesse du moteur gauche pour compenser les erreurs materielles
    speedRatio = (nbPulseRight-nbPulseLeft)*CORRECTION_FACTOR; 
    
    // Prints pour tester le PID
    Serial.print("\nPulse right : "); 
    Serial.println(nbPulseRight); 
    Serial.print("Pulse left : "); 
    Serial.println(nbPulseLeft); 
    Serial.print("Speed Ratio : ");
    Serial.println(speedRatio); 
  } 
 
  // Stop les moteurs
  MOTOR_SetSpeed(0,0); 
  MOTOR_SetSpeed(1,0); 
  nbPulseLeft = ENCODER_ReadReset(LEFT);
  nbPulseRight = ENCODER_ReadReset(RIGHT);
  _delay_us(10);
  delay(1000);
}