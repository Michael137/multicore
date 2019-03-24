/*
 * Swap elements in array in a thread-safe manner. Includes benchmarks for
 * different approaches
 */

#include <assert.h>
#include <inttypes.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <xmmintrin.h> // _mm_pause

#define BILLION 1E9
#define SHARED_MEM_SIZE 16384
#define NUM_THREADS 16
#define NUM_SWAPS 100000

#define timespecsub( vvp, uvp )                                                \
	do                                                                         \
	{                                                                          \
		( vvp )->tv_sec -= ( uvp )->tv_sec;                                    \
		( vvp )->tv_nsec -= ( uvp )->tv_nsec;                                  \
		if( ( vvp )->tv_nsec < 0 )                                             \
		{                                                                      \
			( vvp )->tv_sec--;                                                 \
			( vvp )->tv_nsec += BILLION;                                       \
		}                                                                      \
	} while( 0 )

// For benchmark shared memory is simple integer array
static volatile int* g_shared = NULL;

// Core swap operation used by all lock implementations
static void raw_swap( int idx1, int idx2 )
{
	int tmp        = g_shared[idx1];
	g_shared[idx1] = g_shared[idx2];
	g_shared[idx2] = tmp;
}

/*
 * Locks tested:
 * 		Coarse Lock:	lock whole array
 * 		Fine Lock:		lock each individual array element separately
 * 		Region Lock:	lock regions of array
 * 		Lock-free:		no locks, just atomic operations
 * 		(TODO) Test-and-set: use TAS/TATAS on bit-vector instead of costlier (?) array of semaphores
 */

/*
 * Coarse Locking Implementation
 */
static pthread_mutex_t g_coarse_lck = PTHREAD_MUTEX_INITIALIZER;

static void coarse_swap( int idx1, int idx2 )
{
	pthread_mutex_lock( &g_coarse_lck );
	raw_swap( idx1, idx2 );
	pthread_mutex_unlock( &g_coarse_lck );
}

/*
 * Fine graind locking implementation
 */
static pthread_mutex_t* g_fine_lcks = NULL;
static void fine_swap( int idx1, int idx2 )
{
	pthread_mutex_lock( &g_fine_lcks[idx1] );
	pthread_mutex_lock( &g_fine_lcks[idx2] );
	raw_swap( idx1, idx2 );
	pthread_mutex_unlock( &g_fine_lcks[idx1] );
	pthread_mutex_unlock( &g_fine_lcks[idx2] );
}

/*
 * This implementation locks regions of shared memory
 */
#define G_REGION_LEN (SHARED_MEM_SIZE / ((SHARED_MEM_SIZE >= 100) ? 100 : ((SHARED_MEM_SIZE >= 10) ? 10 : 1)) )
static pthread_mutex_t* g_reg_lcks = NULL;
static void reg_swap( int idx1, int idx2 )
{
	int idx1_lck = idx1 % G_REGION_LEN;
	int idx2_lck = idx2 % G_REGION_LEN;

	pthread_mutex_lock( &g_reg_lcks[idx1_lck] );
	pthread_mutex_lock( &g_reg_lcks[idx2_lck] );
	raw_swap( idx1, idx2 );
	pthread_mutex_unlock( &g_reg_lcks[idx1_lck] );
	pthread_mutex_unlock( &g_reg_lcks[idx2_lck] );
}

/*
 * Lock-free implementation
 */
static const volatile int g_no_lck_sentinel = -1;
static void lock_free_swap( int idx1, int idx2 )
{
	int old1 = 0;
	int old2 = 0;
	do { _mm_pause(); } while( (old1 = __sync_lock_test_and_set(&g_shared[idx1], g_no_lck_sentinel)) == -1 );
	do { _mm_pause(); } while( (old2 = __sync_lock_test_and_set(&g_shared[idx2], g_no_lck_sentinel)) == -1 );
	do { if(__sync_bool_compare_and_swap(&g_shared[idx1], g_no_lck_sentinel, old2)) break; } while(1);
	do { if(__sync_bool_compare_and_swap(&g_shared[idx2], g_no_lck_sentinel, old1)) break; } while(1);
}

static void swap( int idx1, int idx2 )
{
#ifdef COARSE_LCK
	coarse_swap( idx1, idx2 );
#elif FINE_LCK
	fine_swap( idx1, idx2 );
#elif REG_LCK
	reg_swap( idx1, idx2 );
#elif NO_LCK
	lock_free_swap( idx1, idx2 );
#endif
}

/*
 * pthread benchmark utils
 */
typedef struct
{
	pthread_t tid;
	int idx1;
	int idx2;
} thread_arg;

void* thread_fn( void* arg )
{
	thread_arg* ta = arg;
	for( int i = 0; i < NUM_SWAPS; ++i )
		swap( ta->idx1, ta->idx2 );
	pthread_exit( NULL );
}

int main( int argc, char* argv[] )
{
	// Setup
	struct timespec start;
	struct timespec stop;
	thread_arg* threads = calloc( NUM_THREADS, sizeof( thread_arg ) );
	g_shared            = malloc( sizeof( int ) * SHARED_MEM_SIZE );
	srand( time( NULL ) );
	for( int i = 0; i < SHARED_MEM_SIZE; ++i )
		g_shared[i] = rand() % SHARED_MEM_SIZE + 1;

#ifdef FINE_LCK
	g_fine_lcks = malloc( SHARED_MEM_SIZE * sizeof( pthread_mutex_t ) );
	for( int i = 0; i < SHARED_MEM_SIZE; ++i )
		pthread_mutex_init( &g_fine_lcks[i], NULL );
#elif REG_LCK
	g_reg_lcks = malloc( G_REGION_LEN * sizeof( pthread_mutex_t ) );
	for( int i = 0; i < G_REGION_LEN; ++i )
		pthread_mutex_init( &g_reg_lcks[i], NULL );
#endif

	// Run the benchmark
	int idx1 = 0;
	int idx2 = 0;
	if( clock_gettime( CLOCK_REALTIME, &start ) == -1 )
	{
		fprintf( stderr, "clock_gettime failed to record start. Exiting...\n" );
		exit( EXIT_FAILURE );
	}
	for( int i = 0; i < NUM_THREADS; ++i )
	{
		idx1 = rand() % SHARED_MEM_SIZE;
		idx2 = rand() % SHARED_MEM_SIZE;
		// Make sure swapped indices are not the same (since XOR swap would
		// fail)
		while( idx1 == idx2 )
			idx2 = rand() % SHARED_MEM_SIZE;
		threads[i].idx1 = idx1;
		threads[i].idx2 = idx2;
		pthread_create( &( threads[i].tid ), NULL, thread_fn, &threads[i] );
	}

	for( int i = 0; i < NUM_THREADS; ++i )
		pthread_join( threads[i].tid, NULL );

	if( clock_gettime( CLOCK_REALTIME, &stop ) == -1 )
	{
		fprintf( stderr, "clock_gettime failed to record end. Exiting...\n" );
		exit( EXIT_FAILURE );
	}

	if( argc > 1 && !strcmp( argv[1], "-v" ) )
	{
		timespecsub( &stop, &start );
		printf( "  time: %jd.%09jd\n", (intmax_t)stop.tv_sec,
		        (intmax_t)stop.tv_nsec );
	}

	return EXIT_SUCCESS;
}
