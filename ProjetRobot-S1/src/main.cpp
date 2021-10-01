#include <Arduino.h>  // Librairie pour utiliser les fonctions internes d'arduino
#include <LibRobus.h> // Librairie 
 
#define ENCODE_TOUR_ROUE 3200     // Nombre de ticks d'encodeur pour faire un tour
 
#define TAILLE_ROUE_PO 9.42477796 // Taille des roues en pouces
#define TAILLE_ROUE_CM 23.938936  // Taille des roues en cm
 
#define BASE_SPEED 0.4                  // Vitesse de base
#define QUARTER_BASE_SPEED BASE_SPEED/4 // 1/4 de la vitesse de base pour départs et arrets plus doux
#define FACTEUR_CORRECTION 0.0004       // Facteur de correction pour le PID
#define CORRECTION_PAR_TOUR 4           // Nombre de fois que le PID ajuste les valeurs de vitesses par tour
 
void avance_CM(int nbCM); 
void avance_tour_roue(int nbTour); 

/* 
Fonction de setup pour initialiser le robot
*/
void setup() { 
  // put your setup code here, to run once: 
  Serial.begin(9600); 
  BoardInit(); 
  MOTOR_SetSpeed(LEFT,0); 
  MOTOR_SetSpeed(RIGHT,0); 
  Serial.println("Start"); 
} 
 
/*
Boucle principale du programme
*/
void loop() { 
  for(;;) // Boucle infinie pour tester la ligne droite
  { 
    delay(1000); 
    avance_tour_roue(100); 
  } 
  // put your main code here, to run repeatedly: 
} 
/*
Cette fonction prend en entrée un nombre entier et fait tourner les
roues un nombre de tour égal a ce nombre entier
*/
void avance_tour_roue(int nbTour) 
{ 
  long nbPulseRight = ENCODER_ReadReset(RIGHT); // Variables contenant les dernières valeures d'encodeurs
  long nbPulseLeft = ENCODER_ReadReset(LEFT); 
 
  float speedRatio = 0; // Ratio utilisé pour ajuster le rapport de vitesse entre les deux roues
 
  for(int i = 1; i <= 4; i++) // Utilise le 1er tour pour accélérer
  {
    // Change la vitesse de chaque moteurs à la valeur de base, applique le PID
    MOTOR_SetSpeed(LEFT, ((QUARTER_BASE_SPEED*i)+speedRatio)); 
    MOTOR_SetSpeed(RIGHT, (QUARTER_BASE_SPEED*i)); 

    // Laisse les moteurs tourner jusqu'a la valeur voulue avant de mettre a jour le PID
    while(nbPulseRight <= ENCODE_TOUR_ROUE/CORRECTION_PAR_TOUR*i) 
    { 
      // Vérifie si la condition est remplie toutes les 100us
      nbPulseRight = ENCODER_Read(RIGHT); 
      nbPulseLeft = ENCODER_Read(LEFT); 
      _delay_us(100); 
    } 

    Serial.print("\nPulse right : "); 
    Serial.println(nbPulseRight); 
    Serial.print("Pulse left : "); 
    Serial.println(nbPulseLeft); 
 
    speedRatio = (nbPulseRight-nbPulseLeft)*FACTEUR_CORRECTION; 
    Serial.print("Speed Ratio : ");
    Serial.println(speedRatio); 
  }

  for(int i = 2; i <= nbTour*CORRECTION_PAR_TOUR; i++) // Tours restants
  { 
    MOTOR_SetSpeed(LEFT, (BASE_SPEED+speedRatio)); 
    MOTOR_SetSpeed(RIGHT, BASE_SPEED);

    while(nbPulseRight <= ENCODE_TOUR_ROUE/CORRECTION_PAR_TOUR*i) 
    { 
      nbPulseRight = ENCODER_Read(RIGHT); 
      nbPulseLeft = ENCODER_Read(LEFT); 
      _delay_us(100); 
    } 
    
    Serial.print("\nPulse right : "); 
    Serial.println(nbPulseRight); 
    Serial.print("Pulse left : "); 
    Serial.println(nbPulseLeft); 
    
    speedRatio = (nbPulseRight-nbPulseLeft)*FACTEUR_CORRECTION; 
    Serial.print("Speed Ratio : ");
    Serial.println(speedRatio);  
  } 
 
  MOTOR_SetSpeed(0,0); 
  MOTOR_SetSpeed(1,0); 
} 
 
void avance_CM(int nbCM) 
{ 
   
} 
