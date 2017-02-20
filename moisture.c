#include <wiringPi.h>
#include <stdio.h>
#include <inttypes.h>
#include "moisture.h"

#define MOISTUREPIN    28

int read_moisture_data()
{
	pinMode( MOISTUREPIN, INPUT );
	uint8_t state = digitalRead( MOISTUREPIN );
	return state;
}
