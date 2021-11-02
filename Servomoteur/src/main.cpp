#include <Arduino.h>
#include <librobus.h>
int bidon = 0;
int otage();
int liberation();

void setup()
 {
  Serial.begin(9600); 
  SERVO_Enable(0);
  SERVO_Enable(1);
  delay(500);
  SERVO_SetAngle(0,0);
  SERVO_SetAngle(1,180);
  delay(500);
  SERVO_Disable(0);
  SERVO_Disable(1);
  delay(1000);
 
  BoardInit(); 
  MOTOR_SetSpeed(0,0); 
  MOTOR_SetSpeed(1,0); 
  Serial.println("Start");

  //fonction initaliation des servo 
 }

void loop() 
{
bidon = 0;
otage();
/*while(bidon != 1)
{
MOTOR_SetSpeed(0,-0.2); 
MOTOR_SetSpeed(1,-0.2);
delay(5000);
bidon = 1;
}
MOTOR_SetSpeed(0,0); 
MOTOR_SetSpeed(1,0);
*/

liberation();


}

//fonction prise d otage du ballon

int otage(void)
{
  SERVO_Enable(0);
   SERVO_Enable(1);
  delay(500);
  SERVO_SetAngle(0,90);
  SERVO_SetAngle(1,90);
  delay(500);
  SERVO_Disable(0);
  SERVO_Disable(1);
  delay(3000);
}

int liberation(void)
{
  SERVO_Enable(0);
  SERVO_Enable(1);
  delay(500);
  SERVO_SetAngle(0,0);
  SERVO_SetAngle(1,180);
  delay(500);
  SERVO_Disable(1);
  SERVO_Disable(0);
}