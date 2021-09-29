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
int speed;
long frontDist, rearDist;
int borderDist = 6;
int maxSpeed = 3;

void setup()
{
    PT_INIT(&ptFindDistance);
    PT_INIT(&ptRunMotor);
    PT_INIT(&ptReadRemote);
    
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
    speed = setMaxMin(speed, 2 * speedMultiplier * (frontDist > 3), -2 * speedMultiplier * (rearDist > 3));
    PT_SCHEDULE(runMotor(&ptRunMotor));
    PT_SCHEDULE(readRemote(&ptReadRemote));
    PT_SCHEDULE(findDistance(&ptFindDistance));
}

pt ptFindDistance;
int findDistance(struct pt* pt)
{
    PT_BEGIN(pt);
    for(;;)
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
        
        PT_SLEEP(100);
    }
    PT_END(pt);
}

pt ptRunMotor;
int runMotor(struct pt* pt)
{
    PT_begin(pt);
    for(;;)
    {
        //Set motor speed and run steppers
        Stepper1.setSpeed(speed);
        if (speed != 0)
        {
            Stepper1.step(1);
        }
        PT_YIELD(pt);
    }
    
    PT_END(pt);
}

pt ptReadRemote;
int readRemote(struct pt* pt)
{
    PT_BEGIN(pt);
    for(;;)
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
        
        PT_SLEEP(pt, 100);
    }
    
    PT_END(pt);
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
