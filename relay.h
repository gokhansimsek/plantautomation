/*
 *	@file        relay.h
 *	@date:       01/01/2016
 *  
 *  Change relay state function definitions
 *  
 * */
 
#include <stdint.h>
#include <strings.h>

/**
 * Change relay state
 */
void change_water_relay_state(int state);

void change_light_relay_state(int state);

void change_fan_relay_state(int state);

void change_humidity_relay_state(int state);
