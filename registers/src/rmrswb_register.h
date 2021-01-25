#ifndef RBMRSW_REGISTER_H_IN
#define RBMRSW_REGISTER_H_IN

#include <register.h>
#include <smrswb_register.h>
#include <thread_id.h>

namespace amp
{
template<std::size_t SIZE = 32>
struct RegularMRSWBoolReg : public Register<bool>
{
	/*
	 * Implementation of a Regular Single-Reader Single-Writer
	 * Boolean register. This container is *NOT* thread-safe
	 * but can be used as building block to construct one.
	 * A read call can return any value in the that is being
	 * written by some overlapping write or the last preceding
	 * write.
	 */
   public:
	// PUBLIC TYPES
	// NOTE: clang requires typename, gcc doesn't
	using const_iterator = typename SafeMRSWBoolReg<SIZE>::const_iterator;
	using iterator       = typename SafeMRSWBoolReg<SIZE>::iterator;

	// CREATORS
	explicit RegularMRSWBoolReg();

	// MANIPULATORS
	void write( bool v ) override;
	inline iterator begin() noexcept { return d_register.begin(); }
	inline iterator end() noexcept { return d_register.end(); }

	// ACCESSORS
	bool read() const noexcept override;

	inline const_iterator cbegin() const noexcept
	{
		return d_register.cbegin();
	}
	inline const_iterator cend() const noexcept { return d_register.cend(); }

   private:
	// PRIVATE DATA

	// TODO(137): making this static interferes with other instances
	// of this thread_local...is this OK for all use cases?
	static thread_local bool d_last;

	SafeMRSWBoolReg<SIZE> d_register;
};

// DEFINITIONS
template<std::size_t SIZE>
thread_local bool RegularMRSWBoolReg<SIZE>::d_last = false;

// CREATORS
template<std::size_t SIZE>
RegularMRSWBoolReg<SIZE>::RegularMRSWBoolReg()
{
}

// ACCESSORS
template<std::size_t SIZE>
bool RegularMRSWBoolReg<SIZE>::read() const noexcept
{
	return d_register.read();
}

// MANIPULATORS
template<std::size_t SIZE>
void RegularMRSWBoolReg<SIZE>::write( bool v )
{
	if( v != d_last )
	{
		d_last = v;
		d_register.write( v );
	}
}
} // namespace amp

#endif // RBMRSW_REGISTER_H_IN
