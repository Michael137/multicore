#ifndef SMRSWB_REGISTER_H_IN
#define SMRSWB_REGISTER_H_IN

#include <register.h>
#include <thread_id.h>
#include <array>

namespace amp
{
struct SafeSRSWBoolReg : public Register<bool>
{
	/*
	 * Implementation of a Single-Reader Single-Writer
	 * Boolean register. Despite its name this is *NOT*
	 * a thread-safe container but can be used as a
	 * building block to construct one. Overlapping
	 * read/write calls can return any value in the
	 * range allowed by 'bool' and non-overlapping
	 * read/write will return the value of the last
	 * preceding write.
	 */

	// CREATORS
	explicit SafeSRSWBoolReg(bool v) : d_value(v) {}
	explicit SafeSRSWBoolReg() {}

	// MANIPULATORS
	void write( bool v ) override { d_value = v; }

	// ACCESSORS
	bool read() const noexcept override { return d_value; }

   private:
	// PRIVATE DATA
	bool d_value = false;
};

template<std::size_t SIZE = 32>
struct SafeMRSWBoolReg : public Register<bool>
{
	/*
	 * Implementation of a Single-Reader Single-Writer
	 * Boolean register. Despite its name this is *NOT*
	 * a thread-safe container but can be used as a
	 * building block to construct one. Overlapping
	 * read/write calls can return any value written
	 * by overlapping writes OR the last preceding write.
	 * Overlapping reads will return the value of the last write.
	 * Non-overlapping read/write calls return the value of
	 * the last preceding write.
	 */

	// CREATORS
	explicit SafeMRSWBoolReg();

	// MANIPULATORS
	void write( bool v ) override;

	// ACCESSORS
	bool read() const noexcept override;

   private:
	// PRIVATE DATA
	std::array<SafeSRSWBoolReg, SIZE> d_table;
};

// DEFINITIONS

// CREATORS
template<std::size_t SIZE>
SafeMRSWBoolReg<SIZE>::SafeMRSWBoolReg()
{
}

// ACCESSORS
template<std::size_t SIZE>
bool SafeMRSWBoolReg<SIZE>::read() const noexcept
{
	return d_table[ThreadID::get()].read();
}

// MANIPULATORS
template<std::size_t SIZE>
void SafeMRSWBoolReg<SIZE>::write( bool v )
{
	d_table[ThreadID::get()].write(v);
}

} // namespace amp

#endif // SMRSWB_REGISTER_H_IN
