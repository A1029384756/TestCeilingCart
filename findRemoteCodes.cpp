#inlcude "IRremote.h"

IRrecv irrecv(6);
decode_results results;

void setup()
{
    Serial.begin(9600);
    irrecv.enableIRIn();
}

void loop
{
    readRemote();
}

void readRemote()
{
    if(irrecv.decode(&results))
    {
        if (results.value != 0x000000)
        {
            Serial.println(results.value);
        }
    }
    