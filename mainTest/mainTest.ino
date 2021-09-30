#include <libRobus.h>
void setup() {
  // put your setup code here, to run once:
BoardInit();
}

void loop() {
  // put your main code here, to run repeatedly:
  
  MOTOR_SetSpeed(0, 0.5)(1, 0.5);
  delay (2000);
  MOTOR_SetSpeed(0, 0)(1, 0);
  x++;
  
  while (1){
    
  }
 }
