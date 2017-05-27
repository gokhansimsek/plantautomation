#include "sql.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sqlite3.h> 
#include <time.h>

char g_name[50] = {0};
int g_light_max = 0;
int g_light_min = 0;
int g_temperature_max = 0;
int g_temperature_min = 0;
int g_humidity_max = 0;
int g_humidity_min = 0;

static int print_callback(void *data, int argc, char **argv, char **azColName)
{
   int i;
   for(i=0; i<argc; i++)
   {
      //printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
   }
   //printf("\n");
   return 0;
}

static int optimal_callback(void *data, int argc, char **argv, char **azColName)
{
   int i;
   fprintf(stderr, "%s: ", (const char*)data);
   for(i=0; i<argc; i++)
   {
      //printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
      if (strcmp(azColName[i],"NAME") == 0)
      {
          strcpy(g_name,argv[i]);
      }
      if (strcmp(azColName[i],"LIGHTMAX") == 0)
      {
          g_light_max = atoi(argv[i]);
      }
      if (strcmp(azColName[i],"LIGHTMIN") == 0)
      {
          g_light_min = atoi(argv[i]);
      }
      if (strcmp(azColName[i],"TEMPERATUREMAX") == 0)
      {
          g_temperature_max = atoi(argv[i]);
      }
      if (strcmp(azColName[i],"TEMPERATUREMIN") == 0)
      {
          g_temperature_min = atoi(argv[i]);
      }
      if (strcmp(azColName[i],"HUMIDITYMAX") == 0)
      {
          g_humidity_max = atoi(argv[i]);
      }
      if (strcmp(azColName[i],"HUMIDITYMIN") == 0)
      {
          g_humidity_min = atoi(argv[i]);
      }
   }
   return 0;
}

int createDatabase()
{
    sqlite3 *db;
    int rc;

    rc = sqlite3_open("automation.db", &db);

    if( rc )
    {
        fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
        return 0;
    }
    else
    {
        fprintf(stderr, "Opened database successfully\n");
    }
    sqlite3_close(db);
    return 1;
}

int createTransactionTable()
{
    sqlite3 *db;
    char *zErrMsg = 0;
    int rc;
    char *sql;

    rc = sqlite3_open("automation.db", &db);

    if( rc )
    {
        fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
        return 0;
    }

    /* Create SQL statement */
    sql = "CREATE TABLE DATACOLLECTION("  \
             "DATE                   TEXT    NOT NULL," \
             "HOUR                  TEXT    NOT NULL," \
             "LIGHT                   INT     NOT NULL," \
             "TEMPERATURE     INT     NOT NULL," \
             "HUMIDITY             INT     NOT NULL," \
             "MOISTURE            INT     NOT NULL);";

    /* Execute SQL statement */
    rc = sqlite3_exec(db, sql, print_callback, 0, &zErrMsg);
    if( rc != SQLITE_OK )
    {
        fprintf(stderr, "SQL error: %s\n", zErrMsg);
        sqlite3_free(zErrMsg);
    }
    else
    {
        fprintf(stdout, "Transaction table created successfully\n");
    }
    sqlite3_close(db);
    return 1;
}

int createOptimalValueTable()
{
    sqlite3 *db;
    char *zErrMsg = 0;
    int rc;
    char *sql;

    rc = sqlite3_open("automation.db", &db);

    if( rc )
    {
        fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
        return 0;
    }

    /* Create SQL statement */
    sql = "CREATE TABLE OPTIMALVALUES("  \
             "NAME                         TEXT    NOT NULL," \
             "LIGHTMAX                  INT     NOT NULL," \
             "LIGHTMIN                   INT     NOT NULL," \
             "TEMPERATUREMAX    INT     NOT NULL," \
             "TEMPERATUREMIN     INT     NOT NULL," \
             "HUMIDITYMAX            INT     NOT NULL," \
             "HUMIDITYMIN             INT     NOT NULL)";

    /* Execute SQL statement */
    rc = sqlite3_exec(db, sql, print_callback, 0, &zErrMsg);
    if( rc != SQLITE_OK )
    {
        fprintf(stderr, "SQL error: %s\n", zErrMsg);
        sqlite3_free(zErrMsg);
    }
    else
    {
        fprintf(stdout, "Optimal value table created successfully\n");
    }
    sqlite3_close(db);
    return 1;
}

int createDailyFeedbackStatusTable()
{
    sqlite3 *db;
    char *zErrMsg = 0;
    int rc;
    char *sql;

    rc = sqlite3_open("automation.db", &db);

    if( rc )
    {
        fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
        return 0;
    }

    /* Create SQL statement */
    sql = "CREATE TABLE DAILYSTATUS("  \
             "DATE                    TEXT    NOT NULL," \
             "FEEDBACK            INT      NOT NULL);";

    /* Execute SQL statement */
    rc = sqlite3_exec(db, sql, print_callback, 0, &zErrMsg);
    if( rc != SQLITE_OK )
    {
        fprintf(stderr, "SQL error: %s\n", zErrMsg);
        sqlite3_free(zErrMsg);
    }
    else
    {
        fprintf(stdout, "Daily feedback value table created successfully\n");
    }
    sqlite3_close(db);
    return 1;
}

int insertTransactionTableItem(struct collectionData insert_data)
{
    sqlite3 *db;
    char *zErrMsg = 0;
    int rc;
    char *sql;
    char date_buf[50] = {0};
    char hour_buf[50] = {0};
    time_t curtime;
    struct tm *loc_time;

    //Getting current time of system
    curtime = time (NULL);

    // Converting current time to local time
    loc_time = localtime (&curtime);

    snprintf(date_buf, 50,"%d-%d-%d",loc_time->tm_mday,loc_time->tm_mon + 1,loc_time->tm_year + 1900);
    snprintf(hour_buf, 50,"%02d:%02d",loc_time->tm_hour,loc_time->tm_min);
    
    /* Open database */
    rc = sqlite3_open("automation.db", &db);
    if( rc )
    {
        fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
        return 0;
    }

    printf("Light = %d, Temp = %d, Humidity = %d\n",insert_data.light,insert_data.temperature,insert_data.humidity);
    
    /* Create SQL statement */
    sql = sqlite3_mprintf("INSERT INTO DATACOLLECTION VALUES ('%q', '%q', %d, %d, %d, %d );",date_buf,hour_buf,
                insert_data.light,insert_data.temperature,insert_data.humidity,insert_data.moisture);
    
    /* Execute SQL statement */
    rc = sqlite3_exec(db, sql, print_callback, 0, &zErrMsg);
    if( rc != SQLITE_OK )
    {
        fprintf(stderr, "SQL error: %s\n", zErrMsg);
        sqlite3_free(zErrMsg);
    }
    else
    {
        fprintf(stdout, "Transaction records added successfully\n");
    }
    sqlite3_close(db);
    return 1;    
}

int selectTransactionTableItem()
{
    sqlite3 *db;
    char *zErrMsg = 0;
    int rc;
    char *sql;
    const char* data = "Transaction callback function called";
    char date_buf[50] = {0};
    time_t curtime;
    struct tm *loc_time;

    //Getting current time of system
    curtime = time (NULL);

    // Converting current time to local time
    loc_time = localtime (&curtime);

    snprintf(date_buf, 50,"%d-%d-%d",loc_time->tm_mday,loc_time->tm_mon + 1,loc_time->tm_year + 1900);
    
    /* Open database */
    rc = sqlite3_open("automation.db", &db);
    if( rc )
    {
        fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
        return 0;
    }

    /* Create SQL statement */
    sql = sqlite3_mprintf("SELECT * FROM DATACOLLECTION WHERE DATE='%q'",date_buf);
    
    /* Execute SQL statement */
    rc = sqlite3_exec(db, sql, print_callback, (void*)data, &zErrMsg);
    if( rc != SQLITE_OK )
    {
        fprintf(stderr, "SQL error: %s\n", zErrMsg);
        sqlite3_free(zErrMsg);
    }
    else
    {
        fprintf(stdout, "Select Transaction Data operation done successfully\n");
    }
    sqlite3_close(db);
    return 1;
}

int insertOptimalTableItem(struct optimalData insert_data)
{
    sqlite3 *db;
    char *zErrMsg = 0;
    int rc;
    char *sql;

    /* Open database */
    rc = sqlite3_open("automation.db", &db);
    if( rc )
    {
        fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
        return 0;
    }
    
    /* Create SQL statement */
    sql = sqlite3_mprintf("INSERT INTO OPTIMALVALUES VALUES ('%q', %d, %d, %d, %d, %d, %d );",
                insert_data.name,insert_data.lightmax,insert_data.lightmin,insert_data.temperaturemax,insert_data.temperaturemin,insert_data.humiditymax,insert_data.humiditymin);
    
    /* Execute SQL statement */
    rc = sqlite3_exec(db, sql, print_callback, 0, &zErrMsg);
    if( rc != SQLITE_OK )
    {
        fprintf(stderr, "SQL error: %s\n", zErrMsg);
        sqlite3_free(zErrMsg);
    }
    else
    {
        fprintf(stdout, "Optimal Value records added successfully\n");
    }
    sqlite3_close(db);
    return 1;    
}

int selectOptimalTableItem(char *name, struct optimalData *selected_data)
{
    sqlite3 *db;
    char *zErrMsg = 0;
    int rc;
    char *sql;
    const char* data = "Optimal callback function called";

    /* Open database */
    rc = sqlite3_open("automation.db", &db);
    if( rc )
    {
        fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
        return 0;
    }

    /* Create SQL statement */
    sql = sqlite3_mprintf("SELECT * FROM OPTIMALVALUES WHERE NAME='%q'",name);
    
    /* Execute SQL statement */
    rc = sqlite3_exec(db, sql, optimal_callback, (void*)data, &zErrMsg);
    if( rc != SQLITE_OK )
    {
        fprintf(stderr, "SQL error: %s\n", zErrMsg);
        sqlite3_free(zErrMsg);
    }
    else
    {
        fprintf(stdout, "Select Optimal Data operation done successfully\n");
    }
    sqlite3_close(db);
    
    strcpy(selected_data->name,g_name);
    selected_data->lightmax = g_light_max;
    selected_data->lightmin = g_light_min;
    selected_data->temperaturemax = g_temperature_max;
    selected_data->temperaturemin = g_temperature_min;
    selected_data->humiditymax = g_humidity_max;
    selected_data->humiditymin = g_humidity_min;
    
    printf("Selected Data Items LightMax = %d, LightMin = %d, TemperatureMax = %d, TemperatureMin = %d, HumidityMax = %d, HumidityMin = %d\n",
            selected_data->lightmax,selected_data->lightmin,selected_data->temperaturemax,selected_data->temperaturemin,selected_data->humiditymax,selected_data->humiditymin);
    return 1;
}

int insertDailyFeedbackStatusTableItem(PlantStatus status)
{
    sqlite3 *db;
    char *zErrMsg = 0;
    int rc;
    char *sql;
    char date_buf[50] = {0};
    time_t curtime;
    struct tm *loc_time;

    //Getting current time of system
    curtime = time (NULL);

    // Converting current time to local time
    loc_time = localtime (&curtime);

    snprintf(date_buf, 50,"%d-%d-%d",loc_time->tm_mday,loc_time->tm_mon + 1,loc_time->tm_year + 1900);
    
    /* Open database */
    rc = sqlite3_open("automation.db", &db);
    if( rc )
    {
        fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
        return 0;
    }
    
    /* Create SQL statement */
    sql = sqlite3_mprintf("INSERT INTO DAILYSTATUS VALUES ('%q', %d );",date_buf,status);
    
    /* Execute SQL statement */
    rc = sqlite3_exec(db, sql, print_callback, 0, &zErrMsg);
    if( rc != SQLITE_OK )
    {
        fprintf(stderr, "SQL error: %s\n", zErrMsg);
        sqlite3_free(zErrMsg);
    }
    else
    {
        fprintf(stdout, "Daily Feedback status records added successfully\n");
    }
    sqlite3_close(db);

    return 1;    
}

