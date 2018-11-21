#define _GNU_SOURCE
#include <assert.h>
#include <pthread.h>
#include <sched.h>
#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define SUM_LOOP_NUM 10000
void sum_array( int* arr, size_t sz );
static pthread_mutex_t shared_arr_mutex = PTHREAD_MUTEX_INITIALIZER;
volatile sig_atomic_t global_tatas_lock = 0;

volatile int* global_rw_flags = NULL;
volatile sig_atomic_t global_rw_tail = 0;
volatile sig_atomic_t first_thread_complete = 0;
volatile int operation_ctr = 0; // TODO: might need to move to thread_local or add synchronization
								// TODO: make sure it approximately causes correct number of writes

typedef struct thread_info
{
	pthread_t thread_id;
	int thread_num;
	int thread_arg;
	int shared_data_sz;
	int** shared_data;
	int run_on_single_core;
	int rw_slot;
	int number_of_threads;
	int ops_until_write;
} thread_info;

void log_thread( thread_info* ti )
{
#ifdef DEBUG
	printf( "Called from: %d\n", ti->thread_num );
#endif
}

void cond_lock_ro( thread_info** ti )
{
#ifdef USE_MUTEX
	pthread_mutex_lock( &shared_arr_mutex );
#elif USE_TATAS
	do {
		while( global_tatas_lock ) {
		}
	} while( __sync_lock_test_and_set( &global_tatas_lock, 1 ) );
#elif USE_RW_TATAS
	do {
		int old = global_tatas_lock;
		if( ( old >= 0 ) && ( __sync_bool_compare_and_swap( &global_tatas_lock,
															old, old + 1 ) ) )
			break;
	} while( 1 );
#elif USE_FLAG_RW // flag-based reader-writer lock (array-based lock)
	// Based on:
	// https://geidav.wordpress.com/2016/12/03/scalable-spinlocks-1-array-based/
	// https://www.csd.uoc.gr/~hy586/material/lectures/cs586-Section3.pdf
	// Art of Multiprocessor Programming Ch. 7.5.1
	int n = ( *ti )->number_of_threads;
	int old_tail = __sync_fetch_and_add( &global_rw_tail, 1 * 4 );
	int slot = ( *ti )->rw_slot = ( ( old_tail + 1 * 4 ) ) % n;
	while( !global_rw_flags[slot] ) {
		// spin
		//		printf( "Thread %d: Waiting in slot %d tail %d\n", ( *ti
		//)->thread_num, 		slot, global_rw_tail );
	}
#endif
}

void cond_unlock_ro( thread_info** ti )
{
#ifdef USE_MUTEX
	pthread_mutex_unlock( &shared_arr_mutex );
#elif USE_TATAS
	global_tatas_lock = 0;
#elif USE_RW_TATAS
	__sync_fetch_and_add( &global_tatas_lock, -1 );
#elif USE_FLAG_RW // flag-based reader-writer lock (array-based lock)
	int n = ( *ti )->number_of_threads;
	int slot = ( *ti )->rw_slot;
	global_rw_flags[slot] = 0;
	global_rw_flags[( slot + 1 * 4 ) % n] = 1;
#ifdef DEBUG
	printf( "Unlocking %d\n", ( *ti )->thread_num );
#endif
#endif
}

void cond_lock_rw( thread_info** ti)
{
#ifdef USE_MUTEX
	pthread_mutex_lock( &shared_arr_mutex );
#elif USE_TATAS
	do {
		while( global_tatas_lock ) {
		}
	} while( __sync_lock_test_and_set( &global_tatas_lock, 1 ) );
#elif USE_RW_TATAS
	do {
		if(( global_tatas_lock == 0 ) &&
			__sync_bool_compare_and_swap( &global_tatas_lock, 0, -1 ))
			break;
	} while( 1 );
#elif USE_FLAG_RW // flag-based reader-writer lock (array-based lock)
#endif
}

void cond_unlock_rw( thread_info** ti)
{
#ifdef USE_MUTEX
	pthread_mutex_unlock( &shared_arr_mutex );
#elif USE_TATAS
	global_tatas_lock = 0;
#elif USE_RW_TATAS
	global_tatas_lock = 0;
#elif USE_FLAG_RW // flag-based reader-writer lock (array-based lock)

#endif
}

void lock( thread_info** ti)
{
	if(operation_ctr >= (*ti)->ops_until_write)
		cond_lock_rw( ti );
	else
		cond_lock_ro( ti );
}

void unlock( thread_info** ti)
{
	if(operation_ctr >= (*ti)->ops_until_write) {
		cond_unlock_rw( ti );
		operation_ctr = 0;
	} else
		cond_unlock_ro( ti );
}

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

static void* __attribute__( ( noinline ) ) __attribute__( ( optimize( "O0" ) ) )
thread_sum_unlim_fn( void* arg )
{

	thread_info* t_info = arg;
	if( t_info->run_on_single_core ) schedule_on_core();

	// printf( "Called from: INTHREAD%ld\n", t_info->thread_id );
	while( 1/*!first_thread_complete*/ ) {
		lock( &t_info );
		log_thread( t_info );
		sum_array( *( t_info->shared_data ), t_info->shared_data_sz );
		unlock( &t_info );
	}
	return NULL;
}

static void* __attribute__( ( noinline ) ) __attribute__( ( optimize( "O0" ) ) )
thread_sum_fn( void* arg )
{
	pthread_setcanceltype( PTHREAD_CANCEL_ASYNCHRONOUS, NULL );
	thread_info* t_info = arg;
	if( t_info->run_on_single_core ) schedule_on_core();

	// printf( "Called from: INTHREAD%ld\n", t_info->thread_id );
	int i;
	for( i = 0; i < t_info->thread_arg; ++i ) {
		lock( &t_info );
		log_thread( t_info );
		sum_array( *( t_info->shared_data ), t_info->shared_data_sz );
		unlock( &t_info );
	}
	// printf( "Called from: DONE%ld\n", t_info->thread_id );

	return NULL;
}

void __attribute__( ( noinline ) ) __attribute__( ( optimize( "O0" ) ) )
sum_array( int* arr, size_t sz )
{
	int sum = 0;
	int i;
	for( i = 0; i < sz; ++i ) {
		sum += arr[i];
	}

	operation_ctr++;
}

// 1: number of threads
// 2: shared array size
// 3: run on single core
int main( int argc, char** argv )
{
	srand( time( NULL ) );
	// TODO: use getopt

	int num_threads = atoi( argv[1] );
	printf( "Spawning %d threads\n", num_threads );

	thread_info* tinfo;
	tinfo = calloc( num_threads, sizeof( thread_info ) );

	int shared_data_sz = atoi( argv[2] );
	int* shared_arr = malloc( shared_data_sz * sizeof( int ) );
	int j;
	for( j = 0; j < shared_data_sz; ++j )
		shared_arr[j] = rand() % ( shared_data_sz * 5 );

	/* Initialize global reader-writer flags */
	global_rw_flags =
		calloc( 4 * num_threads, sizeof( int ) ); // * 4 for padding
	global_rw_flags[0] = 1;						  // First flag available

	int i, tret;
	void* res;
	tinfo[0].thread_num = 0;
	tinfo[0].thread_arg = SUM_LOOP_NUM;
	tinfo[0].run_on_single_core = atoi( argv[3] );
	tinfo[0].shared_data = &shared_arr;
	tinfo[0].shared_data_sz = shared_data_sz;
	tinfo[0].rw_slot = 0;
	tinfo[0].number_of_threads = num_threads;
	tinfo[0].ops_until_write = atoi( argv[4] );
	tret = pthread_create( &( tinfo[0].thread_id ), NULL, &thread_sum_fn,
						   &tinfo[0] );

	// printf( "Functions: %p\n", &thread_sum_fn, &thread_sum_unlim_fn );

	for( i = 1; i < num_threads; ++i ) {
		tinfo[i].thread_num = i;
		tinfo[i].thread_arg = SUM_LOOP_NUM;
		tinfo[i].run_on_single_core = atoi( argv[3] );
		tinfo[i].shared_data = &shared_arr;
		tinfo[i].shared_data_sz = shared_data_sz;
		tinfo[i].rw_slot = 0;
		tinfo[i].ops_until_write = atoi( argv[4] );
		tret = pthread_create( &( tinfo[i].thread_id ), NULL,
							   &thread_sum_unlim_fn, &tinfo[i] );
	}

	tret = pthread_join( tinfo[0].thread_id, &res );
	//__sync_fetch_and_add( &first_thread_complete, 1 );

	// printf( "Thread %d finished. Cancelling other threads now...\n",
	//		tinfo[0].thread_num );
	// TODO: add cleanup handler to avoid leaks (flagged by tsan)
	for( i = 1; i < num_threads; ++i ) {
		pthread_cancel( tinfo[i].thread_id );
	}

	//for( i = 1; i < num_threads; ++i )
	//{
	//	tret = pthread_join( tinfo[i].thread_id, &res );
	//}

	free( res );
	free( shared_arr );
	free( (int*)global_rw_flags );
	free( tinfo );

	return 0;
}
