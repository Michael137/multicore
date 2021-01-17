#ifndef THREAD_ID_H_IN
#define THREAD_ID_H_IN

#include <atomic>
#include <cstdint>

namespace amp
{
struct ThreadID
{
	static uint64_t get() noexcept;

   private:
	static std::atomic<uint64_t> d_threadCounter;
};

} // namespace amp

#endif // THREAD_ID_H_IN
