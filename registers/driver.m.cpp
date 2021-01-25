#include <registers.h>
#include <thread_id.h>

#include <cassert>
#include <iostream>
#include <thread>
#include <vector>

#ifdef __cpp_lib_barrier
#	include <barrier>
#	define BARRIER std::barrier
#else
//#include <compat/barrier.h>
//#define BARRIER amp::compat::barrier
#endif

constexpr int NUM_THREADS = 8;

int main()
{
	amp::SafeMRSWBoolReg<NUM_THREADS> reg;
	//	BARRIER b(NUM_THREADS);

	for( auto const& srsw : reg )
		assert( !srsw.read() );

	std::vector<std::thread> threads;
	for( int i = 0; i < NUM_THREADS; ++i )
		threads.emplace_back( [/*&b,*/ &reg]() {
			// b.wait();
			reg.write( true );
		} );

	for( auto& t : threads )
		t.join();

	for( auto const& srsw : reg )
		assert( srsw.read() );

	return 0;
}
