#ifndef REGISTER_H_IN
#define REGISTER_H_IN

namespace amp
{
template<typename T>
struct Register
{
	virtual T read() const noexcept = 0;
	virtual void write( T v )       = 0;
};

} // namespace amp

#endif // REGISTER_H_IN
