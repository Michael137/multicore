#include <registers.h>

#include <cassert>
#include <climits>
#include <iostream>
#include <thread>
#include <vector>

#ifdef __cpp_lib_barrier
#	include <barrier>
#	define BARRIER std::barrier
#else
#	include <compat/barrier.h>
#	define BARRIER amp::compat::barrier
#endif

constexpr int NUM_THREADS = 6;

int main()
{
	amp::RegularMRSWReg<NUM_THREADS> reg;
	BARRIER b( NUM_THREADS );

	std::vector<std::thread> threads;
	for( int i = 0; i < NUM_THREADS; ++i )
		threads.emplace_back( [&b, &reg]() {
			b.arrive_and_wait();

			reg.write( 100 );
		} );

	for( auto& t : threads )
		t.join();

	int res = static_cast<int>(reg.read());
	std::cout << "reg.read() => " << res << std::endl;
	assert(reg.read());
}
