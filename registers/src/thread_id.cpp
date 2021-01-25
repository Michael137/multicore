#include <thread_id.h>
#include <cstdint>

namespace amp
{
std::atomic<uint64_t> ThreadID::d_threadCounter{0};

uint64_t ThreadID::get() noexcept
{
	thread_local uint64_t tid = d_threadCounter++;
	return tid;
}

} // namespace amp
