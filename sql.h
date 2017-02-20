/*
 *	@file        sql.h
 *	@date:       01/01/2016
 *  
 *  sqlite3 database access functions
 *  
 * */
 
#include <stdint.h>
#include <strings.h>

typedef struct collectionData
{
	int light;
	int temperature;
	int humidity;
	int moisture;
} collectionData;

typedef struct optimalData
{
	char name[50];
	int lightmax;
	int lightmin;
	int temperaturemax;
	int temperaturemin;
	int humiditymax;
	int humiditymin;
} optimalData;

typedef struct dailyData
{
	int light;
	int temperature;
	int humidity;
} dailyData;

/**
 * Database and table initialization functions
 */
int createDatabase();
int createTransactionTable();
int createOptimalValueTable();
int createDailyDataTable();

/**
 * Transaction table related functions
 */
int insertTransactionTableItem(struct collectionData insert_data);
int selectTransactionTableItem();

/**
 * Optimal Values table related functions
 */
int insertOptimalTableItem(struct optimalData insert_data);
int selectOptimalTableItem(char *search_data, struct optimalData *selected_data);

/**
 * Daily Values table related functions
 */
int insertDailyDataTableItem();
