#include <assert.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct thread_info
{
	pthread_t thread_id;
	int thread_num;
	int thread_arg;
} thread_info;

// 3800 => 1s
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
	thread_info* ti = arg;
	delay( ti->thread_arg );
	return NULL;
}

int main( int argc, char** argv )
{
	int num_threads = atoi( argv[1] );

	thread_info* tinfo;
	tinfo = calloc( num_threads, sizeof( thread_info ) );
	int i, tret;
	void* res;

	for( i = 0; i < num_threads; ++i ) {
		tinfo[i].thread_num = i;
		tinfo[i].thread_arg = atoi( argv[2] );
		tret = pthread_create( &( tinfo[i].thread_id ), NULL, &thread_fn,
							   &tinfo[i] );
	}

	for( i = 0; i < num_threads; ++i ) {
		tret = pthread_join( tinfo[i].thread_id, &res );
	}

	return 0;
}
