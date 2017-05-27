#include <wiringPi.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <pthread.h>
#include <time.h>
#include <errno.h>
#include <curl/curl.h>
#include <math.h>

#include "tsl2561.h"
#include "dht11.h"
#include "moisture.h"
#include "relay.h"
#include "sql.h"
#include "mail.h"

#define READDELAY     900000 //15 minutes
#define MAILDELAY     3600000 // 1 hour

#define BETTERBUTTONPIN         22
#define NORMALBUTTONPIN         23
#define WORSEBUTTONPIN          24

#define RELAYSTATECLOSE         1
#define RELAYSTATEOPEN          0

optimalData optimal_data;
bool is_feedback_given = false;

void handle_state(struct collectionData data)
{
    time_t curtime;
    struct tm *loc_time;
    
    //Getting current time of system
    curtime = time (NULL);
    
    // Converting current time to local time
    loc_time = localtime (&curtime);

    if ( loc_time->tm_hour > 6 && loc_time->tm_hour < 18)
    {
        //Light sensor should be stay outside away from lights.
        //Otherwise lights stays open always. Relay pin needs to low state to close.
        if (data.light < optimal_data.lightmin)
        {
            change_light_relay_state(RELAYSTATEOPEN);
        }
        else
        {
            change_light_relay_state(RELAYSTATECLOSE);
        }
    }
    
    change_water_relay_state(!data.moisture);

    if ( data.temperature > optimal_data.temperaturemax )
    {
        change_fan_relay_state(RELAYSTATEOPEN);
        if ( data.humidity > optimal_data.humiditymax )
        { 
			//open windows
		}   
    }
    else if ( data.temperature < optimal_data.temperaturemin )
    {
        //close windows
        change_fan_relay_state(RELAYSTATECLOSE);
    }
    else if (data.temperature > optimal_data.temperaturemin && data.temperature < optimal_data.temperaturemax)
    {
		//keep windows open
		change_fan_relay_state(RELAYSTATECLOSE);
	}
    
    if ( data.humidity < optimal_data.humiditymin )
    {
        change_fan_relay_state(RELAYSTATECLOSE);
        change_humidity_relay_state(RELAYSTATEOPEN);
        //close windows
    }
    else if ( data.humidity > optimal_data.humiditymax )
    {
        //open windows
        change_fan_relay_state(RELAYSTATEOPEN);
        change_humidity_relay_state(RELAYSTATECLOSE);
    }
    else if (data.humidity > optimal_data.humiditymin && data.humidity < optimal_data.humiditymax)
    {
        change_humidity_relay_state(RELAYSTATECLOSE);
    }    
}

void * transaction_thread()
{
    DHT11 data;
    int lux;
    int moisture_state;
    collectionData transaction_data;
    
    while ( 1 )
    {
        printf("Reading data...\n");
        read_dht11_data(&data);
        
        lux = getLuxValue();
        if (lux < 0)
        {
            lux = 0;            
        }
        
        moisture_state = read_moisture_data();
        
        transaction_data.light = lux;
        transaction_data.temperature = data.temperature;
        transaction_data.humidity = data.humidity;
        transaction_data.moisture = moisture_state;

        //Sometimes sensor reads 0 value. We shouldn't add this data to DB.
        if (data.temperature != 0)
        {
            insertTransactionTableItem(transaction_data);
        }

        handle_state(transaction_data);

        delay( READDELAY ); /* wait 15min to refresh */
    }
}

void * feedback_thread()
{
    pinMode( BETTERBUTTONPIN, INPUT );
    pinMode( NORMALBUTTONPIN, INPUT );
    pinMode( WORSEBUTTONPIN, INPUT );
    
    uint8_t better_state   = 0;
    uint8_t normal_state   = 0;
    uint8_t worse_state   = 0;
    
    time_t curtime;
    struct tm *loc_time;
    
    while ( 1 ) 
    {
		//Getting current time of system
        curtime = time (NULL);
        
        // Converting current time to local time
        loc_time = localtime (&curtime);
        
        if (is_feedback_given == false)
        { 
			better_state = digitalRead( BETTERBUTTONPIN );
			normal_state = digitalRead( NORMALBUTTONPIN );
			worse_state  = digitalRead( WORSEBUTTONPIN );
			
			//printf("Better = %d, Normal = %d, Worse = %d\n",better_state,normal_state,worse_state);
			delay ( 1000 );
			
			if (better_state)
			{
				insertDailyFeedbackStatusTableItem(Better);
				is_feedback_given = true;
                printf("Feedback Status = Better\n");
				delay ( 10000 );
			}
			else if (normal_state)
			{
				insertDailyFeedbackStatusTableItem(Normal);
				is_feedback_given = true;
                printf("Feedback Status = Normal\n");
				delay ( 10000 );
			}
			else if (worse_state)
			{
				insertDailyFeedbackStatusTableItem(Worse);
				is_feedback_given = true;
                printf("Feedback Status = Worse\n");
				delay ( 10000 );
			}
        }
        
        if (loc_time->tm_hour == 2 && is_feedback_given == true) 
        {
			printf("Feedback flag has been reset\n");
			is_feedback_given = false;
		}
    }
}

void * mail_thread()
{
    time_t curtime;
    struct tm *loc_time;
    
    while ( 1 ) 
    { 
        //Getting current time of system
        curtime = time (NULL);
        
        // Converting current time to local time
        loc_time = localtime (&curtime);

        if (loc_time->tm_hour == 9 ) 
        { 
            if (send_feedback_email() == CURLE_OK)
           {
				printf("Mail has been sent\n");
                delay( MAILDELAY ); /* wait 1 hour to pass 8 o'clock */
           }
           else
           {
                //Wait 10 seconds and try again.
                printf("Mail couldn't be sent\n");
                delay ( 10000 );
           }
        }
        else
        {
            delay( READDELAY ); /* wait 15min to refresh */
        }
    }
}

int main( void )
{
    pthread_t transaction_tid, feedback_tid, mail_tid;
    int err;
    
    if ( wiringPiSetup() == -1 )
    {
        exit( 1 );
    }

    system("rm automation.db");

    createDatabase();
    createTransactionTable();
    createOptimalValueTable();
    createDailyFeedbackStatusTable();
    
    strncpy(optimal_data.name,"ORKIDE",50);
    optimal_data.lightmax = 5000;
    optimal_data.lightmin = 3000;
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


    err = pthread_create(&feedback_tid, NULL, feedback_thread, NULL);
    
    if (err != 0)
    {
        printf("can't create daily feedback thread :[%s]", strerror(err));
    }
    else
    {
        printf("Daily feedback thread created successfully\n");
    }

    err = pthread_create(&mail_tid, NULL, mail_thread, NULL);
    
    if (err != 0)
    {
        printf("can't create daily mail sending thread :[%s]", strerror(err));
    }
    else
    {
        printf("Daily mail sending thread created successfully\n");
    }

    pthread_join( transaction_tid, NULL);
    pthread_join( feedback_tid, NULL);
    pthread_join( mail_tid, NULL);
    
    return(0);
}
