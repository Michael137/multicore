#include <compat/barrier.h>

#include <chrono>
#include <cstddef>
#include <functional>
#include <thread>

// Bare bones implementation of:
// https://code.woboq.org/llvm/libcxx/include/barrier.html
// https://code.woboq.org/llvm/libcxx/src/barrier.cpp.html

namespace
{
// Based on:
// https://code.woboq.org/llvm/libcxx/include/__threading_support.html#std::__1::__libcpp_timed_backoff_policy
[[maybe_unused]] bool backoff( std::chrono::nanoseconds elapsed ) {
	if( elapsed > std::chrono::milliseconds( 128 ) )
		std::this_thread::sleep_for( std::chrono::milliseconds( 8 ) );
	else if( elapsed > std::chrono::microseconds( 64 ) )
		std::this_thread::sleep_for( elapsed / 2 );
	else if( elapsed > std::chrono::microseconds( 4 ) )
		std::this_thread::yield();
	else
	{
		// poll
	}

	return false;
}

static constexpr int polling_count = 64;

// Based on:
// https://code.woboq.org/llvm/libcxx/include/__threading_support.html#std::__1::__libcpp_polling_count
[[maybe_unused]] bool
poll_with_backoff( std::function<bool( void )> testFn,
                   std::function<bool( std::chrono::nanoseconds )> backoffFn ) {
	auto const start = std::chrono::high_resolution_clock::now();
	for( int count = 0;; )
	{
		if( testFn() )
			return true;

		if( count < polling_count )
		{
			count += 1;
			continue;
		}

		auto const elapsed = std::chrono::high_resolution_clock::now() - start;
		if( backoffFn( elapsed ) )
			return false;
	}
}

} // anonymous namespace

namespace amp
{
namespace compat
{
barrier::~barrier() {}

void barrier::arrive_and_wait()
{
	--d_phase;
	poll_with_backoff( [this]() -> bool { return d_phase.load() == 0; },
	                   backoff );
}

} // namespace compat
} // namespace amp
