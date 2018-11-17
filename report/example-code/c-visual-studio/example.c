// To compile:
//
// cl /O2 example.c

#include <windows.h>
#include <assert.h>

// Delay function waits a variable time controlled by "d".  Note that
// optimization is disabled for this function so that the (useless)
// computation is left to form a delay.

#pragma optimize("", off)
static int delay(int d) {
  int i,k;
  int x = 0;
  for (i = 0; i<d; i++) {
    for (k = 0; k<1000000; k++) {
      x+=i+k;
    }
  }
  return x;
}
#pragma optimize("", on)


// Example thread function.  This just delays for a little while,
// controlled by the parameter passed when the thread is started.

static DWORD WINAPI thread_fn(LPVOID lpParam) {
  int arg_val = *(int*)lpParam;
  printf("Thread running, arg=%d\n", arg_val);
  delay(arg_val);
  printf("Thread done\n");
  return 17;
}

// Shared variable for use with example atomic compare and swap
// operations (InterlockedCompareExchange in this example).

static volatile LONG x = 0;

// Main function

int main(int argc, char **argv) {
 
  // Start a new thread, and then wait for it to complete:

  int thread_arg;
  DWORD r;
  LONG v;
  HANDLE new_thread;
  thread_arg = 1000;
  new_thread = CreateThread(NULL,
                            0,           // Default stack size
                            thread_fn,
                            &thread_arg, // Parameter for thread_fn
                            0,           // 0 => Run immediately
                            NULL);

  assert(new_thread != NULL && "CreateThread failed");
  printf("Waiting for thread\n");
  r = WaitForSingleObject(new_thread, INFINITE);
  assert(r == WAIT_OBJECT_0 && "WaitForSingleObject failed");
  printf("Joined with thread\n");

  // Example compare and swap operations

  printf("x=%d\n", x);
  v = InterlockedCompareExchange(&x, 1, 0);
  printf("x=%d v=%d\n", x, v);
  v = InterlockedCompareExchange(&x, 2, 0);
  printf("x=%d v=%d\n", x, v);
  v = InterlockedCompareExchange(&x, 2, 1);
  printf("x=%d v=%d\n", x, v);
  
  return 0;
}
