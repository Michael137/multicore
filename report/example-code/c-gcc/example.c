// To compile:
//
// gcc -O2 -lpthread example.c 

#include <assert.h>
#include <stdio.h>
#include <pthread.h>

// Delay function waits a variable time controlled by "d".  Note that
// it is set to be non-inlined so that the computation is not
// completely optimized away.  (Without inlining, gcc does not realize
// that the return value is not actually needed).

static __attribute__ ((noinline)) int delay(int d) {
  int i,k;
  int x = 0;
  for (i = 0; i<d; i++) {
    for (k = 0; k<100000; k++) {
      x+=i+k;
    }
  }
  return x;
}

// Example thread function.  This just delays for a little while,
// controlled by the parameter passed when the thread is started.

static void *thread_fn(void *arg) {
  int arg_val = *(int*)arg;
  printf("Thread running, arg=%d\n", arg_val);
  delay(arg_val);
  printf("Thread done\n");
  return NULL;
}

// Shared variable for use with example atomic compare and swap
// operations (__sync_val_compare_and_swap in this example).

static volatile int x = 0;

// Main function

int main(int argc, char **argv) {
 
  // Start a new thread, and then wait for it to complete:

  int thread_arg;
  pthread_t new_thread;
  thread_arg = 100000;
  int r = pthread_create(&new_thread, 
			 NULL,         // Attributes
			 thread_fn,
			 &thread_arg); // Parameter for thread_fn
  assert(r==0 && "pthread_create failed");
  printf("Waiting for thread\n");
  void *ret;
  r = pthread_join(new_thread, &ret);
  assert(r==0 && "pthread_join failed");
  printf("Joined with thread ret=%p\n", ret);

  // Example compare and swap operations

  int v = 0;
  printf("x=%d\n", x);
  v = __sync_val_compare_and_swap(&x, 0, 1);
  printf("x=%d v=%d\n", x, v);
  v = __sync_val_compare_and_swap(&x, 0, 2);
  printf("x=%d v=%d\n", x, v);
  v = __sync_val_compare_and_swap(&x, 1, 2);
  printf("x=%d v=%d\n", x, v);
  
  return 0;
}
