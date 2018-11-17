#include <assert.h>
#include <pthread.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct thread_info
{
	pthread_t thread_id;
	int thread_num;
	int thread_arg; // delay passed as argument
} thread_info;

bool check_input( char const* str )
{
	if( str[0] == '\0' ) return false;

	while( *str ) {
		if( *str < '0' || *str > '9' ) return false;
		*str++;
	}

	return true;
}

static __attribute__( ( noinline ) ) __attribute__( ( optimize( "O0" ) ) ) int
delay( int d )
{
	int i, k;
	int x = 0;
	for( i = 0; i < d; i++ ) {
		for( k = 0; k < 100000; k++ ) {
			x += i + k;
		}
	}
	return x;
}

static void* thread_fn( void* arg )
{
	int arg_val = *(int*)arg;
	printf( "Thread running, arg=%d\n", arg_val );
	delay( arg_val );
	printf( "Thread done\n" );
	return NULL;
}

int main( int argc, char** argv )
{
	// TODO: use getopt
	if( argc != 2 ) {
		printf( "Expected 1 argument (number of threads). Found %d\n", argc );
		return 1;
	} else {
		if( !( check_input( argv[1] ) ) ) {
			printf( "Wrong input format. Expected int got: %s", argv[1] );
			return 1;
		}

		int num_threads = atoi( argv[1] );
		printf( "Spawning %d threads\n", num_threads );

		thread_info* tinfo;
		tinfo = calloc( num_threads, sizeof( thread_info ) );

		int i, tret;
		void* res;
		for( i = 0; i < num_threads; ++i ) {
			tinfo[i].thread_num = i;
			tinfo[i].thread_arg = 1000000;
			tret = pthread_create( &tinfo[i].thread_id, NULL, &thread_fn,
								   &tinfo[i].thread_num );
		}

		for( i = 0; i < num_threads; ++i ) {
			tret = pthread_join( tinfo[i].thread_id, &res );
			printf( "Joined with thread %d; returned value was: %s\n",
					tinfo[i].thread_num, (char*)res );
			free( res );
		}

		free( tinfo );
	}

	return 0;
}
