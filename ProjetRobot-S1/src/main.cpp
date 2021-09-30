#include <Arduino.h>
#include <LibRobus.h>

/**
 * Circumference of the wheels
 * 
 * Furmula of circumferece = 2*PI*r
 * PI rounded to 3
 * r = 1.5 inches rounded to 4cm
 */
const int CIRCUMFERENCE = 2 * 3 * 4;
const float LEFT_FORWARD_SPEED = 0.45f;
const float RIGHT_FORWARD_SPEED = 0.471f;
const int SLOW_DISTANCE = 30;

void forward(int distance);

void setup()
{
  // put your setup code here, to run once:
  Serial.begin(9600);
  Serial.println("I am born");

  BoardInit();
}

void loop()
{
  // put your main code here, to run repeatedly:
  forward(218);

  while(true) {
    
  }
}

/**
 * Fonction pour avancer le robot en ligne droite
 * 
 * @param[in] distance Distance que le robot doit parcourir en cm.
 */
void forward(int distance)
{
  float nbWheelTurnsFast = 1.0f * distance / CIRCUMFERENCE;

  int nbPulsesFast = nbWheelTurnsFast * 3200;

  MOTOR_SetSpeed(0, LEFT_FORWARD_SPEED);
  MOTOR_SetSpeed(1, RIGHT_FORWARD_SPEED);

  while (ENCODER_Read(0) < nbPulsesFast)
  {
    delay(100);
  }

  ENCODER_ReadReset(0);
  MOTOR_SetSpeed(0, 0.0f);
  MOTOR_SetSpeed(1, 0.0f);
}