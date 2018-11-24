#define _GNU_SOURCE
#include <assert.h>
#include <pthread.h>
#include <sched.h>
#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <xmmintrin.h> // _mm_pause

#define SUM_LOOP_NUM 10000
static pthread_mutex_t shared_arr_mutex = PTHREAD_MUTEX_INITIALIZER;
volatile sig_atomic_t global_tatas_lock = 0;

volatile int* global_anderson_flags = NULL;

volatile sig_atomic_t* g_ro_flags = NULL; // TODO: wrap flags in padded structs
volatile sig_atomic_t g_rw_flag = 0;	  // Single writer

volatile sig_atomic_t global_anderson_tail = 0;
volatile sig_atomic_t first_thread_complete = 0;
volatile int operation_ctr =
	0; // TODO: might need to move to thread_local or add synchronization
	   // TODO: make sure it approximately causes correct number of writes

typedef struct thread_info
{
	pthread_t thread_id;
	int thread_num;
	int thread_arg;
	int shared_data_sz;
	int** shared_data;
	int run_on_single_core;
	int anderson_slot;
	int number_of_threads;
	int ops_until_write;
	int operation_ctr;
} thread_info;

void sum_array( int* arr, size_t sz );
void cond_unlock_rw( thread_info** ti );
void cond_lock_rw( thread_info** ti );
void cond_unlock_ro( thread_info** ti );
void cond_lock_ro( thread_info** ti );
static int delay( int d );

void log_thread( thread_info* ti )
{
#ifdef DEBUG
	printf( "Called from: %d\n", ti->thread_num );
#endif
}

void eb_spin( int* ctr )
{
	if( (*ctr)++ < 32 ) {
		struct timespec ts;
		int milliseconds = 2 << *ctr;
		ts.tv_sec = milliseconds / 1000;
		ts.tv_nsec = ( milliseconds % 1000 ) * 1000000;
		nanosleep( &ts, NULL );
	} else
		pthread_yield();
}

void cond_lock_ro( thread_info** ti )
{
#ifdef USE_MUTEX
	pthread_mutex_lock( &shared_arr_mutex );
#elif USE_TATAS
	do {
		while( global_tatas_lock ) {
			// spin
			_mm_pause();
		}
	} while( __sync_lock_test_and_set( &global_tatas_lock, 1 ) );
#elif USE_RW_TATAS
	do {
		int old = global_tatas_lock;
		if( ( old >= 0 ) && ( __sync_bool_compare_and_swap( &global_tatas_lock,
															old, old + 1 ) ) )
			break;
	} while( 1 );
#elif USE_ANDERSON // flag-based reader-writer lock (array-based lock)
	cond_lock_rw( ti );
#elif USE_RW_ARRAY // http://joeduffyblog.com/2009/02/20/a-more-scalable-readerwriter-lock-and-a-bit-less-harsh-consideration-of-the-idea/
	int backoff_ctr = 0;
	while( 1 ) {
		while( g_rw_flag == 1 ) {
			eb_spin( &backoff_ctr );
		}

		__sync_fetch_and_add( &( g_ro_flags[( *ti )->thread_num] ), 1 );
		if( g_rw_flag == 0 ) break;

		__sync_fetch_and_add( &( g_ro_flags[( *ti )->thread_num] ), -1 );
	}
#endif
}

void cond_unlock_ro( thread_info** ti )
{
#ifdef USE_MUTEX
	pthread_mutex_unlock( &shared_arr_mutex );
#elif USE_TATAS
	__sync_lock_release( &global_tatas_lock );
#elif USE_RW_TATAS
	__sync_fetch_and_add( &global_tatas_lock, -1 );
#elif USE_ANDERSON // flag-based reader-writer lock (array-based lock)
	cond_unlock_rw( ti );
#elif USE_RW_ARRAY
	__sync_fetch_and_add( &( g_ro_flags[( *ti )->thread_num] ), -1 );
#endif
}

void cond_lock_rw( thread_info** ti )
{
#ifdef USE_MUTEX
	pthread_mutex_lock( &shared_arr_mutex );
#elif USE_TATAS
	do {
		while( global_tatas_lock ) {
			// spin
			_mm_pause();
		}
	} while( __sync_lock_test_and_set( &global_tatas_lock, 1 ) );
#elif USE_RW_TATAS
	do {
		if( ( global_tatas_lock == 0 ) &&
			__sync_bool_compare_and_swap( &global_tatas_lock, 0, -1 ) )
			break;
	} while( 1 );
#elif USE_ANDERSON // flag-based reader-writer lock (array-based lock)
	// Based on:
	// https://geidav.wordpress.com/2016/12/03/scalable-spinlocks-1-array-based/
	// https://www.csd.uoc.gr/~hy586/material/lectures/cs586-Section3.pdf
	// Art of Multiprocessor Programming Ch. 7.5.1
	int n = ( *ti )->number_of_threads;
	int old_tail = __sync_fetch_and_add( &global_anderson_tail, 1 * 4 );
	int slot = ( *ti )->anderson_slot = ( ( old_tail + 1 * 4 ) ) % n;
	while( !global_anderson_flags[slot] ) {
		// spin
		_mm_pause();
	}
#elif USE_RW_ARRAY
	int ctr = 0;
	while( true ) {
		if( g_rw_flag == 0 && __sync_lock_test_and_set( &g_rw_flag, 1 ) == 0 ) {
			for( int i = 0; i < ( *ti )->number_of_threads; ++i )
				while( g_ro_flags[i] != 0 )
					eb_spin( &ctr );
			break;
		}
		eb_spin( &ctr );
	}
#endif
}

void cond_unlock_rw( thread_info** ti )
{
#ifdef USE_MUTEX
	pthread_mutex_unlock( &shared_arr_mutex );
#elif USE_TATAS
	__sync_lock_release( &global_tatas_lock );
#elif USE_RW_TATAS
	global_tatas_lock = 0;
#elif USE_ANDERSON // flag-based reader-writer lock (array-based lock)
	int n = ( *ti )->number_of_threads;
	int slot = ( *ti )->anderson_slot;
	global_anderson_flags[slot] = 0;
	global_anderson_flags[( slot + 1 * 4 ) % n] = 1;
#ifdef DEBUG
	printf( "Unlocking %d\n", ( *ti )->thread_num );
#endif
#elif USE_RW_ARRAY
	__sync_lock_release( &g_rw_flag );
#endif
}

void lock( thread_info** ti )
{
	if( ( *ti )->ops_until_write != 0 &&
		( *ti )->operation_ctr >= ( *ti )->ops_until_write )
		cond_lock_rw( ti );
	else
		cond_lock_ro( ti );
}

void unlock( thread_info** ti )
{
	if( ( *ti )->ops_until_write != 0 &&
		( *ti )->operation_ctr >= ( *ti )->ops_until_write ) {
		cond_unlock_rw( ti );
		( *ti )->operation_ctr = 0;
#ifdef DEBUG
		puts( "unlocking write" );
#endif
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
	delay( t_info->thread_arg );
	return NULL;
}

void schedule_on_core()
{
	cpu_set_t my_set;
	CPU_ZERO( &my_set );
	CPU_SET( 3, &my_set );
	sched_setaffinity( 0, sizeof( cpu_set_t ), &my_set );
}

static void* __attribute__( ( noinline ) ) __attribute__( ( optimize( "O0" ) ) )
thread_sum_unlim_fn( void* arg )
{
	thread_info* t_info = arg;
	if( t_info->run_on_single_core ) schedule_on_core();

	while( 1 /*!first_thread_complete*/ ) {
		lock( &t_info );
		log_thread( t_info );
		sum_array( *( t_info->shared_data ), t_info->shared_data_sz );
		unlock( &t_info );
		t_info->operation_ctr++;
	}
	return NULL;
}

static void* __attribute__( ( noinline ) ) __attribute__( ( optimize( "O0" ) ) )
thread_sum_fn( void* arg )
{
	pthread_setcanceltype( PTHREAD_CANCEL_ASYNCHRONOUS, NULL );
	thread_info* t_info = arg;
	if( t_info->run_on_single_core ) schedule_on_core();

	int i;
	for( i = 0; i < t_info->thread_arg; ++i ) {
		lock( &t_info );
		log_thread( t_info );
		sum_array( *( t_info->shared_data ), t_info->shared_data_sz );
		unlock( &t_info );
		t_info->operation_ctr++;
	}

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
}

// 1: number of threads
// 2: shared array size
// 3: run on single core
// 4: number of operations until a write is to be performed
int main( int argc, char** argv )
{
	srand( time( NULL ) );

	int num_threads = atoi( argv[1] );
	printf( "Spawning %d threads\n", num_threads );

	thread_info* tinfo;
	tinfo = calloc( num_threads, sizeof( thread_info ) );

	int shared_data_sz = atoi( argv[2] );
	int* shared_arr = malloc( shared_data_sz * sizeof( int ) );
	int j;
	for( j = 0; j < shared_data_sz; ++j )
		shared_arr[j] = rand() % ( shared_data_sz * 5 );

	/* Initialize global anderson lock flags */
	global_anderson_flags =
		calloc( 4 * num_threads, sizeof( int ) ); // * 4 for padding
	global_anderson_flags[0] = 1;				  // First flag available

	/* Initialize rw queue lock flags */
	g_ro_flags = calloc( num_threads, sizeof( int ) ); // Multiple readers

	int i, tret;
	void* res;
	tinfo[0].thread_num = 0;
	tinfo[0].thread_arg = SUM_LOOP_NUM;
	tinfo[0].run_on_single_core = atoi( argv[3] );
	tinfo[0].shared_data = &shared_arr;
	tinfo[0].shared_data_sz = shared_data_sz;
	tinfo[0].anderson_slot = 0;
	tinfo[0].number_of_threads = num_threads;
	tinfo[0].ops_until_write = atoi( argv[4] );
	tinfo[0].operation_ctr = 0;
	tret = pthread_create( &( tinfo[0].thread_id ), NULL, &thread_sum_fn,
						   &tinfo[0] );

	for( i = 1; i < num_threads; ++i ) {
		tinfo[i].thread_num = i;
		tinfo[i].thread_arg = SUM_LOOP_NUM;
		tinfo[i].run_on_single_core = atoi( argv[3] );
		tinfo[i].shared_data = &shared_arr;
		tinfo[i].number_of_threads = num_threads;
		tinfo[i].shared_data_sz = shared_data_sz;
		tinfo[i].anderson_slot = 0;
		tinfo[i].ops_until_write = atoi( argv[4] );
		tinfo[i].operation_ctr = 0;
		tret = pthread_create( &( tinfo[i].thread_id ), NULL,
							   &thread_sum_unlim_fn, &tinfo[i] );
	}

	tret = pthread_join( tinfo[0].thread_id, &res );

	// TODO: add cleanup handler to avoid leaks (flagged by tsan)
	for( i = 1; i < num_threads; ++i ) {
		pthread_cancel( tinfo[i].thread_id );
	}

	free( res );
	free( shared_arr );
	free( (int*)global_anderson_flags );
	free( tinfo );

	return 0;
}
