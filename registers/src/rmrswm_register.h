#ifndef RMRSWM_REGISTER_H_IN
#define RMRSWM_REGISTER_H_IN

#include <register.h>
#include <rmrswb_register.h>

#include <array>
#include <cassert>
#include <cstdint>
#include <limits>

namespace amp
{
template<std::size_t SIZE = 32, typename SIZE_T = uint8_t>
struct RegularMRSWReg : public Register<SIZE_T>
{
	/*
	 * Implementation of a Regular Single-Reader Single-Writer
	 * M-Valued register. This container is *NOT* thread-safe
	 * but can be used as building block to construct one.
	 * A read call can return any value in the that is being
	 * written by some overlapping write or the last preceding
	 * write. Each register can hold a value in the range of
	 * [std::numeric_limits<SIZE_T>::min(),
	 *  std::numeric_limits<SIZE_T>::max()]
	 */
   private:
	// PRIVATE DATA
	static constexpr uintmax_t range_d = std::numeric_limits<SIZE_T>::max()
	                                     - std::numeric_limits<SIZE_T>::min();

	// PRIVATE TYPES
	using table_t = std::array<RegularMRSWBoolReg<SIZE>, range_d + 1>;

   public:
	// PUBLIC TYPES
	// NOTE: clang requires typename, gcc doesn't
	using const_iterator = typename table_t::const_iterator;
	using iterator       = typename table_t::iterator;

	// CREATORS
	explicit RegularMRSWReg();

	// MANIPULATORS
	void write( SIZE_T v ) override;
	inline iterator begin() noexcept { return d_table.begin(); }
	inline iterator end() noexcept { return d_table.end(); }

	// ACCESSORS
	SIZE_T read() const noexcept override;

	inline const_iterator cbegin() const noexcept
	{
		return d_table.cbegin();
	}
	inline const_iterator cend() const noexcept { return d_table.cend(); }

   private:
	// PRIVATE DATA
	table_t d_table;
};

// DEFINITIONS

// CREATORS
template<std::size_t SIZE, typename SIZE_T>
RegularMRSWReg<SIZE, SIZE_T>::RegularMRSWReg()
{
	d_table[0].write(true);
}

// ACCESSORS
template<std::size_t SIZE, typename SIZE_T>
SIZE_T RegularMRSWReg<SIZE, SIZE_T>::read() const noexcept
{
	for(SIZE_T i = 0; i < d_table.size(); ++i)
	{
		if(d_table[i].read())
			return i;
	}

	return -1;
}

// MANIPULATORS
template<std::size_t SIZE, typename SIZE_T>
void RegularMRSWReg<SIZE, SIZE_T>::write( SIZE_T v )
{
	assert(v >= 0 && v < d_table.size());
	d_table[v].write(true);
	for(SIZE_T i = v; i > 0; --i)
	{
		d_table[i - 1].write(false);
	}
}
} // namespace amp

#endif // RMRSWM_REGISTER_H_IN
