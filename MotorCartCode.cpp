#include <Stepper.h>
#include <IRremote.h>
#include <protothreads.h>
//Pin Definitions
#define echoPinFront 0
#define trigPinFront 1
#define echoPinRear 2
#define trigPinRear 3

//Motor Objects
Stepper Stepper1(2048,7,8,9,10);

//IR Receiver Object
IRrecv  irrecv(6);
decode_results results;

//Data Fields & Constants
int speed = 0;
int borderDist = 6;
long frontDist, rearDist;

unsigned long currentTime = 0;
unsigned long previousDistanceTime, previousRemoteTime = 0;
long distanceTimeInterval = 300;
long remoteTimeInterval = 10;

int setMaxMin(int x, int max, int min)
{
    //Set motor max and min speed based on input
    if (x > max)
    {
        return max;
    }
    
    else if (x < min)
    {
        return min;
    }
    
    else
    {
        return x;
    }
}

int findDistance()
{
    digitalWrite(trigPinFront, LOW);
    digitalWrite(trigPinRear, LOW);
    delayMicroseconds(2);
    
    digitalWrite(trigPinFront, HIGH);
    
    digitalWrite(trigPinRear, HIGH);
    delayMicroseconds(10);
    
    digitalWrite(trigPinFront, LOW);
    digitalWrite(trigPinRear, LOW);
    
    frontDist = pulseIn(echoPinFront, HIGH)/29/2;
    rearDist = pulseIn(echoPinRear, HIGH)/29/2;
}

int runMotor()
{
    //Set motor speed and run steppers
    Stepper1.setSpeed(speed);
    if (speed != 0)
    {
        Stepper1.step(1);
    }
}

int readRemote()
{
    int output = 0;
    //Run decode operation on ir reciever
    if(irrecv.decode(&results))
    {
        int value = results.value;
        Serial.println(value);
        //Return results based on hex code
        switch(value)
        {
            case 0xFFE01:
            speed--;
            break;
            
            case 0xFF906:
            speed++;
            break;
            
            default:
            break;
        }
        
        irrecv.resume();
    }
}

void setup()
{
    Stepper1.setSpeed(0);
    
    //Set Pin Modes
    pinMode(trigPinFront, OUTPUT);
    pinMode(trigPinRear, OUTPUT);
    pinMode(echoPinFront, INPUT);
    pinMode(echoPinRear, INPUT);
    
    //Open serial monitor and start ir receiver
    Serial.begin(9600);
    
    irrecv.enableIRIn();
}

void loop()
{
    currentTime = millis();
    
    speed = setMaxMin(speed, 6*(frontDist > 3), -6*(rearDist > 3));
    runMotor();
    
    if (distanceTimeInterval < (currentTime - previousDistanceTime))
    {
        previousDistanceTime = currentTime;
        findDistance();
    }
    
    if (remoteTimeInterval < (currentTime - previousRemoteTime))
    {
        previousRemoteTime = currentTime;
        readRemote();
    }
}
