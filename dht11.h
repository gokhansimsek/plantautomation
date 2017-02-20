/*
 *	@file        DHT11.h
 *	@date:       01/01/2016
 *  
 *  DHT11 temperature sensor data read function definitions
 *  
 * */
 
#include <stdint.h>
#include <strings.h>

typedef struct DHT11
{
	int temperature;
	int humidity;
} DHT11;

/**
 * Read data from sensor
 */
int read_dht11_data(struct DHT11 *data);
