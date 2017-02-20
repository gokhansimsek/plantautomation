#include <wiringPi.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <pthread.h>
#include <time.h>
#include <errno.h>

#include "tsl2561.h"
#include "dht11.h"
#include "moisture.h"
#include "relay.h"
#include "sql.h"

#define READDELAY    5000
#define DAILYWRITEDELAY 60000//3600000 // One hour

void * transaction_thread()
{
	DHT11 data;
	int lux;
	int moisture_state;
	collectionData transaction_data;
	
	while ( 1 )
	{
		read_dht11_data(&data);
		lux = getLuxValue();
		moisture_state = read_moisture_data();
		change_relay_state(!moisture_state);

		transaction_data.light = lux;
		transaction_data.temperature = data.temperature;
		transaction_data.humidity = data.humidity;
		transaction_data.moisture = moisture_state;

		insertTransactionTableItem(transaction_data);

		delay( READDELAY ); /* wait 5sec to refresh */
	}
}

void * daily_thread()
{
	char date_buf[50] = {0};
    time_t curtime;
    struct tm *loc_time;
	
	while ( 1 )
	{
		//Getting current time of system
		curtime = time (NULL);

		// Converting current time to local time
		loc_time = localtime (&curtime);
		
		//if (loc_time->tm_hour = 14 )
		//{
			//insertDailyDataTableItem();
		//}
		//else
		//{
			//delay( DAILYWRITEDELAY );
		//}
		delay( DAILYWRITEDELAY );
		selectTransactionTableItem();
		delay (1000);
		insertDailyDataTableItem();
	}
}

int main( void )
{
	optimalData optimal_data;
	pthread_t transaction_tid, daily_tid;
	int err;
	
	if ( wiringPiSetup() == -1 )
	{
		exit( 1 );
	}

	createDatabase();
	createTransactionTable();
	createOptimalValueTable();
	createDailyDataTable();

	//char name[50] = {0};
	//int lightmax = 0;
	//int lightmin = 0;
	//int temperaturemax = 0;
	//int temperaturemin = 0;
	//int humiditymax = 0;
	//int humiditymin = 0;
	
	strncpy(optimal_data.name,"ORKIDE",50);
	optimal_data.lightmax = 1000;
	optimal_data.lightmin = 300;
	optimal_data.temperaturemax = 30;
	optimal_data.temperaturemin = 18;
	optimal_data.humiditymax = 50;
	optimal_data.humiditymin = 20;
	
	insertOptimalTableItem(optimal_data);
	
	selectOptimalTableItem("ORKIDE",&optimal_data);
	
	err = pthread_create(&transaction_tid, NULL, transaction_thread, NULL);
	
	if (err != 0)
	{
		printf("can't create transaction thread :[%s]", strerror(err));
	}
	else
	{
		printf("Transaction thread created successfully\n");
    }       

	err = pthread_create(&daily_tid, NULL, daily_thread, NULL);
	
	if (err != 0)
	{
		printf("can't create daily thread :[%s]", strerror(err));
	}
	else
	{
		printf("Daily thread created successfully\n");
    }
    
	pthread_join( transaction_tid, NULL);
	pthread_join( daily_tid, NULL);
	
	return(0);
}
