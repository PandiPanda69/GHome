/***********************************
* Driver de test
* Pulse chaque seconde un nouveau message
* Author : Sébastien M.
***********************************/

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <string.h>

#include "../../drv_api.h"

#define MAX_SENSOR 32
#define SEED	   5
#define MIN_PULSE_INTERVAL 500
#define MAX_PULSE_INTERVAL 2000

static int the_msgq;
static pthread_t the_thread;
static pthread_mutex_t the_mutex = PTHREAD_MUTEX_INITIALIZER;
static short must_continue;

int added_sensors[MAX_SENSOR];
unsigned int sensor_count;

void add_sensor( unsigned int id )
{
	int i = 0;

	pthread_mutex_lock( &the_mutex );
	for( i = 0; i < MAX_SENSOR; i++ )
	{
		if( added_sensors[i] == 0 )
		{
			added_sensors[i] = id;
			sensor_count++;

			break;
		}
	}
	pthread_mutex_unlock( &the_mutex );

	if( i == MAX_SENSOR )
		printf( "Cannot add new sensor : limit reached.\n" );
	else
		printf( "\tAdded at #%d\n", i );
}

unsigned int sensor_exists( unsigned int id )
{
	int i;

	pthread_mutex_lock( &the_mutex );
        for( i = 0; i < MAX_SENSOR; i++ )
        {
                if( added_sensors[i] == id )
                        break;
        }
        pthread_mutex_unlock( &the_mutex );

	return i;
}

void remove_sensor( unsigned int id )
{
	int res = sensor_exists( id );

	if( res >= 0 && res < MAX_SENSOR )
	{
		pthread_mutex_lock( &the_mutex );
		added_sensors[res] = 0;
		sensor_count--;
	        pthread_mutex_unlock( &the_mutex );
	}
	else
		printf( "No sensor found.\n" );
}


/**
* La callback qui pulse
*/
void* callback( void* ptr )
{
	int buffer;

	srand( SEED );

	pthread_mutex_lock( &the_mutex );
	must_continue = 1;
	buffer = 1;
	pthread_mutex_unlock( &the_mutex );

	while( buffer == 1 )
	{
		int index;
		int id = (rand() % MAX_SENSOR);

		if( id == 0 ) continue;

		/* On verifie que l'id est bien dans la liste des capteurs surveillés */
		index = sensor_exists( id );

		if( index >= 0 && index < MAX_SENSOR )
		{
			int res;
			struct msg_drv_notify buf;

			buf.msg_type = DRV_MSG_TYPE;
			buf.id_sensor = id;
			buf.flag_value = (rand() % DRV_LAST_VALUE);
			buf.value = rand()%255;

			res = msgsnd( the_msgq, (const void*) &buf, sizeof(struct msg_drv_notify) - sizeof(long), 0 );
		}

		usleep( rand()%(MAX_PULSE_INTERVAL-MIN_PULSE_INTERVAL) + MIN_PULSE_INTERVAL );

		pthread_mutex_lock( &the_mutex );
		buffer = must_continue;
		pthread_mutex_unlock( &the_mutex );
	}
}

/**
* Initialisation, ne fait quasiment rien
*/
int drv_init( const char* addr, int port )
{
	int i;

	printf( "Connexion à %s:%d\n", addr, port );

	/* Initialise la liste des capteurs ajoutés */
	for( i = 0; i < MAX_SENSOR; i++ )
		added_sensors[i]  = 0;

	sensor_count = 0;

	return 0;
}

int drv_run( int msgq_id )
{
	int ret;

	the_msgq = msgq_id;
	ret = pthread_create( &the_thread, NULL, callback, NULL );

	return 0;
}

void drv_stop( void )
{
	printf( "Wait\n" );

	pthread_mutex_lock( &the_mutex );
	must_continue = 0;
	pthread_mutex_unlock( &the_mutex );

	pthread_join(the_thread, NULL);

	printf( "Stop\n" );
}

int drv_add_sensor( unsigned int id_sensor )
{
	printf( "Adding sensor #%d\n", id_sensor );

	add_sensor( id_sensor );

	return 0;
}

void drv_remove_sensor( unsigned int id_sensor )
{
	printf( "Removing sensor #%d\n", id_sensor );

	remove_sensor( id_sensor );
}

int drv_fetch_data( unsigned int id_sensor, unsigned int id_trame, char* buffer, int max_length )
{
	return 0;
}


int drv_send_data( unsigned int id_sensor, unsigned int id_trame )
{
	return 0;
}

void drv_get_info( char* buffer, int max_length )
{
	strcpy( buffer, "Driver de test, v1.0" );	
}
