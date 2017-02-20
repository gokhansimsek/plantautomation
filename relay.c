#include <wiringPi.h>
#include <stdio.h>
#include <inttypes.h>
#include "relay.h"

#define RELAYPIN    15

void change_relay_state(int state)
{
	pinMode( RELAYPIN, OUTPUT );
	digitalWrite(RELAYPIN,state);
}
