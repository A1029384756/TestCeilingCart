#include <Stepper.h>
#include "IRremote.h"
//Pin Definitions
#define echoPinFront 0
#define echoPinRear 2
#define trigPinFront 1
#define trigPinRear 3

//Motor Objects
Stepper Stepper1(2048, 11, 12, 13, 14);
Stepper Stepper2(2048, 7, 8, 9, 10);


//IR Receiver Object
IRrecv  irrecv(6);
decode_results results;

//Data Fields & Constants
int movement;
int frontDist;
int rearDist;
int borderDist = 6;
int speedMultiplier = 3;

void setup()
{
    Stepper1.setSpeed(0);
    Stepper2.setSpeed(0);
    
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
    //Run distance sensor code
    findDistance();
    
    //Set motor speed with the remote readout
    movement += speedMultiplier * readRemote();
    
    //Find cart relation to motor borders and set movement speed caps
    if (frontDist > borderDist && rearDist > borderDist)
    {
        movement = setMaxMin(movement, 2 * speedMultiplier, -2 * speedMultiplier);
    }
    
    if (frontDist < borderDist)
    {
        movement = setMaxMin(movement, 0, -2 * speedMultiplier);
    }
    
    if (rearDist < borderDist)
    {
        movement = setMaxMin(movement, 2 * speedMultiplier, 0);
    }
    
    runMotor(movement);
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
    
    //Recieve Output Pulse
    frontDist = int(pulseIn(0, HIGH) * 0.034 / 2);
    rearDist = int(pulseIn(1, HIGH) * 0.034 / 2);
}

void runMotor(int speed)
{
    //Set motor speed and run steppers
    Stepper1.setSpeed(speed);
    Stepper2.setSpeed(speed);
    if (speed != 0)
    {
        for (int i = 0; i < 10; i++)
        {
            Stepper1.step(1);
            Stepper2.step(1);
        }
    }
}

int readRemote()
{
    //Run decode operation on ir reciever
    if(irrecv.decode(&results))
    {
        //Return results based on hex code
        if(results.value == 0xFD10EF)
        {
            return -1;
        }
        
        else if (results.value == 0xFD50AF)
        {
            return 1;
        }
        
        else
        {
            return 0;
        }
    }
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
