#include <thread_id.h>
#include <cstdint>

namespace amp
{
// Thread ID 0 is usually special
std::atomic<uint64_t> ThreadID::d_threadCounter{1};

uint64_t ThreadID::get() noexcept
{
	thread_local uint64_t tid = d_threadCounter++;
	return tid;
}

} // namespace amp
