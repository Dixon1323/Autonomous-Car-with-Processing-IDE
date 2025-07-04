#include <HCSR04.h>
#include <Servo.h>

UltraSonicDistanceSensor distanceSensor(D5, D6);     //d       
UltraSonicDistanceSensor distanceSensor2(D7, D8);    //d2          
Servo servoMotor;            

int delayTime = 5;           
long d;                 
long d2;                

void setup() {
  Serial.begin(9600);           

  servoMotor.attach(D2);         
  servoMotor.write(180);        
  delay(1000);             
  servoMotor.write(0);        
  delay(1000);              

}

void loop() {
  for (int i = 1; i < 180; i++) { 
    readSensors();           
    Serial.print(i);          
    Serial.print(",");          
    Serial.print(d);        
    Serial.print(",");        
    Serial.println(d2);        
    servoMotor.write(i); 
    delay(delayTime);   
  }
  for (int i = 180; i > 1; i--) {   
    readSensors();           
    Serial.print(i);
    Serial.print(","); 
    Serial.print(d);
    Serial.print(",");
    Serial.println(d2);
    servoMotor.write(i);
    delay(delayTime);
  }
}

void readSensors() {
  d = distanceSensor.measureDistanceCm();
  d2 = distanceSensor2.measureDistanceCm();
  if(d>=400){
    d=0;
  }
  if(d2>=400){
    d2=0;
  }
}