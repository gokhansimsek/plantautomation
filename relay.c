#include <wiringPi.h>
#include <stdio.h>
#include <inttypes.h>
#include "relay.h"

#define WATERRELAYPIN      4
#define LIGHTRELAYPIN      5
#define FANRELAYPIN       16
#define HUMIDITYRELAYPIN   1

void change_water_relay_state(int state)
{
    pinMode( WATERRELAYPIN, OUTPUT );
    printf("Change water    relay state to = %s\n",state==0?"Opened":"Closed");
    digitalWrite(WATERRELAYPIN,state);
}

void change_light_relay_state(int state)
{
    pinMode( LIGHTRELAYPIN, OUTPUT );
    printf("Change light    relay state to = %s\n",state==0?"Opened":"Closed");
    digitalWrite(LIGHTRELAYPIN,state);
}

void change_fan_relay_state(int state)
{
    pinMode( FANRELAYPIN, OUTPUT );
    printf("Change fan      relay state to = %s\n",state==0?"Opened":"Closed");
    digitalWrite(FANRELAYPIN,state);
}

void change_humidity_relay_state(int state)
{
    pinMode( HUMIDITYRELAYPIN, OUTPUT );
    printf("Change humidity relay state to = %s\n",state==0?"Opened":"Closed");
    digitalWrite(HUMIDITYRELAYPIN,state);
}
