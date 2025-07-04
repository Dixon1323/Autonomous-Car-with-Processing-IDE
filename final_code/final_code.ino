#include <HCSR04.h>
#include <Servo.h>

#define IN1 D1  // Motor driver input 1
#define IN2 D3  
#define IN3 D4  
#define IN4 D5  

UltraSonicDistanceSensor frontSensor(D7, D8); // Front-facing sensor
UltraSonicDistanceSensor backSensor(D5, D6);  // Back-facing sensor
Servo servoMotor;

int threshold = 20; // Safe distance in cm
int angleStep = 1; // Angle step for scanning
long frontDistances[181]; // Stores front sensor readings (0° to 180°)
long backDistances[181];  // Stores back sensor readings (180° to 0°)

void setup() {
  Serial.begin(9600);
  Serial.println("ESP8266 Reset Reason:");
  Serial.println(ESP.getResetReason()); // Get crash reason
  
  servoMotor.attach(D2); 
  servoMotor.write(90);   

  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);
}

void loop() {
  scanSurroundings();
  navigate();
}

void scanSurroundings() {
  int index = 0;

  // Sweep from 0° to 180°
  for (int angle = 0; angle <= 180; angle += angleStep) {
    servoMotor.write(angle);
    delay(20);
    frontDistances[index] = frontSensor.measureDistanceCm();
    backDistances[index] = backSensor.measureDistanceCm();
    if (frontDistances[index] >= 400) frontDistances[index] = 0;
    if (backDistances[index] >= 400) backDistances[index] = 0;
    Serial.print(angle);
    Serial.print(",");
    Serial.print(frontDistances[index]);
    Serial.print(","); 
    Serial.println(backDistances[index]);
    index++;
  }

  // Sweep back from 180° to 0°
  for (int angle = 180; angle >= 0; angle -= angleStep) {
    servoMotor.write(angle);
    delay(20);
    frontDistances[index] = frontSensor.measureDistanceCm();
    backDistances[index] = backSensor.measureDistanceCm();
    if (frontDistances[index] >= 400) frontDistances[index] = 0;
    if (backDistances[index] >= 400) backDistances[index] = 0;
    Serial.print("Angle: ");
    Serial.print(angle);
    Serial.print(" | Front: ");
    Serial.print(frontDistances[index]);
    Serial.print(" cm, Back: ");
    Serial.println(backDistances[index]);
    index++;
  }
}

void navigate() {
  int minFrontDist = 400, minFrontAngle = -1;

  // Find the closest obstacle in the front
  for (int i = 0; i <= 180; i++) {
    if (frontDistances[i] > 0 && frontDistances[i] < minFrontDist) {
      minFrontDist = frontDistances[i];
      minFrontAngle = i;
    }
  }

  if (minFrontDist > threshold) {
    moveForward();
  } else {
    stopMotors();
    delay(500);

    // Calculate turn intensity (shorter for slight turns, longer for sharp turns)
    int turnIntensity = map(abs(minFrontAngle - 90), 0, 90, 500, 2000);

    if (minFrontAngle < 90) {
      Serial.print("Obstacle at ");
      Serial.print(minFrontAngle);
      Serial.println("° (Left) -> Turning Right");
      turnRight(turnIntensity);
      delay(200);
      Serial.println("Correcting by Turning Left");
      turnLeft(turnIntensity);
    } else {
      Serial.print("Obstacle at ");
      Serial.print(minFrontAngle);
      Serial.println("° (Right) -> Turning Left");
      turnLeft(turnIntensity);
      delay(200);
      Serial.println("Correcting by Turning Right");
      turnRight(turnIntensity);
    }

    moveForward();
  }
}

void turnLeft(int duration) {
  Serial.print("Turning Left for ");
  Serial.print(duration);
  Serial.println(" ms");
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
  delay(duration);
}

void turnRight(int duration) {
  Serial.print("Turning Right for ");
  Serial.print(duration);
  Serial.println(" ms");
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);
  delay(duration);
}

void moveForward() {
  Serial.println("Moving forward");
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
}

void moveBackward() {
  Serial.println("Moving backward");
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);
  delay(1000);
}

void stopMotors() {
  Serial.println("Stopping");
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, LOW);
}
