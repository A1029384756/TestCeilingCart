#include <Stepper.h>
#define echoPin 2
#define trigPin 3
int stepsPerRevolution=2048;
Stepper Stepper1(stepsPerRevolution, 8,10,9,11);
int mtrSpeed=5;
long duration;
int distance;

void setup() {
  Stepper1.setSpeed(mtrSpeed);
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  Serial.begin(9600);
}

void loop() {
  findDistance();
  Serial.print("Distance: ");
  Serial.print(distance);
  Serial.println(" cm");
  if (distance > 3)
  {
    runMotor();
  }
}

void runMotor()
{
  Stepper1.step(10);
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
}

void findDistance()
{
  duration = pulseIn(echoPin, HIGH);
  distance = int(duration * 0.034 / 2);
}
