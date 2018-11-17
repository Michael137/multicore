// To compile:
//
// csc Example.cs

using System;
using System.Threading;

public class Example {

  // Delay function waits a variable time controlled by "d".  NB: you
  // may need to modify the delay function if a different
  // implementation of the compiler and .NET runtime system optimize
  // away the computation being used to form the delay -- e.g., using
  // the return value in the caller.

  public static int delay(int d) {
    int i,k;
    int x = 0;
    for (i = 0; i<d; i++) {
      for (k = 0; k<1000000; k++) {
        x+=i+k;
      }
    }
    return x;
  }

  // Constructor for an "Example" object.  Fields in the object can be
  // used to pass values to/from the thread when it is started and
  // when it finishes.

  int Arg;
  public Example(int arg) {
    this.Arg = arg;
  }

  // Example thread function.  This just delays for a little while,
  // controlled by the parameter passed when the thread is started.

  public void ThreadProc() {
    Console.Out.WriteLine("Thread running, arg=" + this.Arg);
    delay(this.Arg);
    Console.Out.WriteLine("Thread done");
  }

  // Shared variable for use with example atomic compare and swap
  // operations (Interlocked.CompareExchange in this example).

  static int x = 0;

  // Main function

  public static void Main() {
 
    // Start a new thread, and then wait for it to complete:

    Console.Out.WriteLine("Start");
    Example e1 = new Example(1000);
    Thread t1 = new Thread(e1.ThreadProc);
    t1.Start();
    t1.Join();
    Console.Out.WriteLine("Joined with thread");

    // Example compare and swap operations

    int v;
    Console.Out.WriteLine("x=" + x);
    v = Interlocked.CompareExchange(ref x, 1, 0);
    Console.Out.WriteLine("x=" + x + " v=" + v);
    v = Interlocked.CompareExchange(ref x, 2, 0);
    Console.Out.WriteLine("x=" + x + " v=" + v);
    v = Interlocked.CompareExchange(ref x, 2, 1);
    Console.Out.WriteLine("x=" + x + " v=" + v);
  }
}
