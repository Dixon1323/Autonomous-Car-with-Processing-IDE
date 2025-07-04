#include <HCSR04.h>
#include <Servo.h>

#define IN1 D0 
#define IN2 D1  
#define IN3 D3  
#define IN4 D4  

// Ultrasonic Sensors
UltraSonicDistanceSensor frontSensor(D7, D8);
UltraSonicDistanceSensor backSensor(D5, D6);

// Servo Motor for Scanning
Servo servoMotor;

// Configuration Parameters
int threshold = 50;   // Distance threshold to detect obstacles (cm)
int angleStep = 20;    // Servo movement step size
int delayTime = 50;   // Delay per servo step (ms)

// Movement Tracking
bool movingForward = true;  
unsigned long lastMoveTime = 0;   
unsigned long lastForwardCheck = 0;  
const int stuckThreshold = 3000; // Time to consider the vehicle stuck (ms)
long lastFrontDist = 0; 

void setup() {
  Serial.begin(9600);
  
  // Attach Servo
  servoMotor.attach(D2); 
  servoMotor.write(90);  // Start at center position

  // Motor Pins
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);

  moveForward();  // Start moving forward
  lastMoveTime = millis();  
  lastForwardCheck = millis();
}

void loop() {
  static int angle = 0;
  static bool sweepingRight = true;

  // Move Servo to Scan Environment
  servoMotor.write(angle);
  delay(delayTime);
  
  // Measure Distances
  long frontDist = frontSensor.measureDistanceCm();
  long backDist = backSensor.measureDistanceCm();

  // Handle sensor errors (-1 means out of range)
  if (frontDist == -1 || frontDist > 400) frontDist = 500;  // Use a large valid value
  if (backDist == -1 || backDist > 400) backDist = 500;  // Use a large valid value

  // Print Scanning Data
  Serial.print(angle);
  Serial.print(",");
  Serial.print(frontDist);
  Serial.print(",");
  Serial.println(backDist);

  // Obstacle Avoidance
  if (frontDist <= threshold) {
    avoidObstacle(angle);
    lastMoveTime = millis();  
  }

  // Check if the car is stuck only if the front distance is within obstacle range
  if (millis() - lastForwardCheck > 1000 && frontDist <= threshold) {  
    if (abs(frontDist - lastFrontDist) < 10) {  // Increase tolerance for movement
      if (millis() - lastMoveTime > stuckThreshold) { 
        // Serial.println("🚨 Car is stuck! Checking for obstacles before reversing...");
        reverseAndFindPath();
        lastMoveTime = millis();
      }
    } else {
      lastMoveTime = millis();  // Reset if car is moving
    }
    lastFrontDist = frontDist;
    lastForwardCheck = millis();
  }

  // Servo Sweep Logic
  if (sweepingRight) {
    angle += angleStep;
    if (angle >= 180) sweepingRight = false;
  } else {
    angle -= angleStep;
    if (angle <= 0) sweepingRight = true;
  }
}

// Function to Avoid Obstacles Based on Angle
void avoidObstacle(int obstacleAngle) {
  stopMotors();
  
  int turnTime = map(obstacleAngle, 0, 180, 1000, 4000); // Adjust turn based on angle
  int forwardTime = 500; 

  if (obstacleAngle > 90) {
    // Serial.println("↩️ Turning Left");
    turnLeft();
  } else {
    // Serial.println("↪️ Turning Right");
    turnRight();
  }
  delay(turnTime);

  // Serial.println("⬆️ Moving Forward");
  moveForward(); 
  delay(forwardTime);
}

// Function to Reverse and Find a Clear Path
void reverseAndFindPath() {
  stopMotors();
  delay(500);

  long backDist = backSensor.measureDistanceCm();
  if (backDist <= threshold) {  // Check if there's an obstacle behind
    // Serial.println("⚠️ Obstacle detected behind! Can't reverse.");
    return;
  }

  // Serial.println("🔄 Reversing...");
  moveBackward();
  delay(2000); 
  stopMotors();

  int bestAngle = findBestPath();
  // Serial.print("✅ Best path found at angle: ");
  // Serial.println(bestAngle);

  if (bestAngle > 90) {
    // Serial.println("↩️ Turning Left to New Path");
    turnLeft();
  } else {
    // Serial.println("↪️ Turning Right to New Path");
    turnRight();
  }
  delay(500);

  // Serial.println("⬆️ Moving Forward to New Path");
  moveForward();
}

// Function to Scan and Find the Best Path
int findBestPath() {
  int maxDistance = 0;
  int bestAngle = 90;

  for (int scanAngle = 0; scanAngle <= 180; scanAngle += 10) {
    servoMotor.write(scanAngle);
    delay(100);
    long dist = frontSensor.measureDistanceCm();
    if (dist == -1 || dist > 400) dist = 0;  

    if (dist > maxDistance) {
      maxDistance = dist;
      bestAngle = scanAngle;
    }
  }
  return bestAngle;
}

// Movement Functions
void moveForward() {
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
}

void moveBackward() {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);
}

void stopMotors() {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, LOW);
}

void turnLeft() {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
}

void turnRight() {
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);
}
