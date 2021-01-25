#ifndef COMPAT_BARRIER_H_IN
#define COMPAT_BARRIER_H_IN

#include <atomic>
#include <cstddef>

namespace amp
{
namespace compat
{
class barrier
{
   private:
	// PRIVATE TYPES
	using barrier_phase_t = uint8_t;

   public:
	// CREATORS
	constexpr explicit barrier( std::ptrdiff_t phase_count )
	    : d_phase( phase_count )
	{
	}

	~barrier();

	barrier( const barrier& ) = delete;
	barrier& operator=( const barrier& ) = delete;

	// PUBLIC MANIPULATOR
	void arrive_and_wait();

   private:
	std::atomic<barrier_phase_t> d_phase;
};
} // namespace compat
} // namespace amp

#endif // COMPAT_BARRIER_H_IN
