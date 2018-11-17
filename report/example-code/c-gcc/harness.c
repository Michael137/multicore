#define _GNU_SOURCE
#include <assert.h>
#include <pthread.h>
#include <sched.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define SUM_LOOP_NUM 100000
void sum_array( int* arr, size_t sz );

typedef struct thread_info
{
	pthread_t thread_id;
	int thread_num;
	int thread_arg;
	int shared_data_sz;
	int** shared_data;
	int run_on_single_core;
} thread_info;

// 1s = 3800
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
	thread_info* t_info = arg;
	//	printf( "Thread running, id %ld, num %d, arg=%d\n", t_info->thread_id,
	// t_info->thread_num, t_info->thread_arg);
	delay( t_info->thread_arg );
	//	printf( "Thread done\n" );
	return NULL;
}

void schedule_on_core()
{
	cpu_set_t my_set;	  /* Define your cpu_set bit mask. */
	CPU_ZERO( &my_set );   /* Initialize it all to 0, i.e. no CPUs selected. */
	CPU_SET( 3, &my_set ); /* set the bit that represents core 7. */
	sched_setaffinity( 0, sizeof( cpu_set_t ),
					   &my_set ); /* Set affinity of tihs process to */
								  /* the defined mask, i.e. only 7. */
}

static void* thread_sum_unlim_fn( void* arg )
{

	thread_info* t_info = arg;
	if( t_info->run_on_single_core ) schedule_on_core();

	while( 1 ) {
		sum_array( *( t_info->shared_data ), t_info->shared_data_sz );
		//		printf( "Called from: %d\n", ((thread_info*)arg)->thread_num );
	}
	return NULL;
}

static void* thread_sum_fn( void* arg )
{
	thread_info* t_info = arg;
	if( t_info->run_on_single_core ) schedule_on_core();

	int i;
	for( i = 0; i < t_info->thread_arg; ++i )
		sum_array( *( t_info->shared_data ), t_info->shared_data_sz );
	return NULL;
}

void __attribute__( ( optimize( "O0" ) ) ) sum_array( int* arr, size_t sz )
{
	int sum = 0;
	int i;
	for( i = 0; i < sz; ++i ) {
		sum += arr[i];
	}
}

// 1: number of threads
// 2: shared array size
// 3: run on single core
int main( int argc, char** argv )
{
	// TODO: use getopt

	int num_threads = atoi( argv[1] );
	printf( "Spawning %d threads\n", num_threads );

	thread_info* tinfo;
	tinfo = calloc( num_threads, sizeof( thread_info ) );

	pthread_setcanceltype( PTHREAD_CANCEL_ASYNCHRONOUS, NULL );

	int shared_data_sz = atoi( argv[2] );
	int* shared_arr = malloc( sizeof( int ) );
	int j;
	for( j = 0; j < shared_data_sz; ++j )
		shared_arr[j] = j;

	int i, tret;
	void* res;
	tinfo[0].thread_num = 0;
	tinfo[0].thread_arg = SUM_LOOP_NUM;
	tinfo[0].run_on_single_core = atoi( argv[3] );
	tinfo[0].shared_data = &shared_arr;
	tinfo[0].shared_data_sz = shared_data_sz;
	tret =
		pthread_create( &tinfo[0].thread_id, NULL, &thread_sum_fn, &tinfo[0] );

	for( i = 1; i < num_threads; ++i ) {
		tinfo[i].thread_num = i;
		tinfo[i].thread_arg = SUM_LOOP_NUM;
		tinfo[i].run_on_single_core = atoi( argv[3] );
		tinfo[i].shared_data = &shared_arr;
		tinfo[i].shared_data_sz = shared_data_sz;
		tret = pthread_create( &tinfo[i].thread_id, NULL, &thread_sum_unlim_fn,
							   &tinfo[i] );
	}

	tret = pthread_join( tinfo[0].thread_id, &res );
	for( i = 1; i < num_threads; ++i ) {
		pthread_cancel( tinfo[i].thread_id );
	}
	free( res );

	//		for( i = 0; i < num_threads; ++i ) {
	//			tret = pthread_join( tinfo[i].thread_id, &res );
	//			// printf( "Joined with thread %d; returned value was:
	//%s\n",
	//			// tinfo[i].thread_num, (char*)res );
	//			free( res );
	//		}

	free( tinfo );
	free( shared_arr );

	return 0;
}
