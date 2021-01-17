#include <registers.h>
#include <thread_id.h>

#include <iostream>
#include <vector>
#include <thread>

constexpr int NUM_THREADS = 8;

int main()
{
	amp::SafeMRSWBoolReg<NUM_THREADS> reg;

	std::vector<std::thread> threads;
	for( int i = 0; i < NUM_THREADS; ++i )
		threads.emplace_back(
		    []() { std::cout << amp::ThreadID::get() << std::endl; } );

	for( auto& t : threads )
		t.join();

	return 0;
}
