#include <Stepper.h>
#include <IRremote.h>
//Pin Definitions
#define echoPinFront 0
#define echoPinRear 2
#define trigPinFront 1
#define trigPinRear 3

//Motor Objects
Stepper Stepper1(2048,7,8,9,10);

//IR Receiver Object
IRrecv  irrecv(6);
decode_results results;

//Data Fields & Constants
int speed;
int frontDist, rearDist;
int borderDist = 6;
int speedMultiplier = 3;

//Protothreading Fields
unsigned long previousMicros;
int maxMotorOffTime = 100;
int maxRemoteOffTime = 528;
int maxDistanceOffTime = 748;

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
    
    previousMicros = 0;
}

void loop()
{
    unsigned long currentMicros = micros();
    
    if (currentMicros - previousMicros >= maxMotorOffTime)
    {
        runMotor();
        previousMicros = currentMicros;
    }
    
    else if (currentMicros - previousMicros >= maxRemoteOffTime)
    {
        //Set motor speed with the remote readout
        speed += speedMultiplier * readRemote();
        previousMicros = currentMicros;
    }
    
    else if (currentMicros - previousMicros >= maxDistanceOffTime)
    {
        findDistance();
        
        //Find cart relation to motor borders and set speed speed caps
        if (frontDist > borderDist && rearDist > borderDist)
        {
            speed = setMaxMin(speed, 2 * speedMultiplier, -2 * speedMultiplier);
        }
        
        if (frontDist < borderDist)
        {
            speed = setMaxMin(speed, 0, -2 * speedMultiplier);
        }
        
        if (rearDist < borderDist)
        {
            speed = setMaxMin(speed, 2 * speedMultiplier, 0);
        }
        
        previousMicros = currentMicros;
    }
    
    else
    {
        runMotor();
    }
}

void findDistance()
{
    //Emit Output Pulse
    digitalWrite(trigPinFront, LOW);
    digitalWrite(trigPinRear, LOW);
    delayMicroseconds(2);
    
    digitalWrite(trigPinFront, HIGH);
    digitalWrite(trigPinRear, HIGH);
    delayMicroseconds(10);
    
    digitalWrite(trigPinFront, LOW);
    digitalWrite(trigPinRear, LOW);
    
    //Receive Output Pulse
    frontDist = int(pulseIn(0, HIGH) * 0.034 / 2);
    rearDist = int(pulseIn(1, HIGH) * 0.034 / 2);
}

void runMotor()
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
            output--;
            break;
            
            case 0xFF906:
            output++;
            break;
            
            default:
            break;
        }
        
        irrecv.resume();
    }
    return output;
}

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
