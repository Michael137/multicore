#include <array>
#include <atomic>
#include <chrono>
#include <iostream>
#include <thread>
#include <vector>

#ifndef TH_NUM
#	define TH_NUM 4
#endif

#ifndef DT_SIZE
#	define DT_SIZE 8
#endif

struct [[gnu::packed]] State8
{
	alignas( 8 ) int8_t c0;
	alignas( 8 ) int8_t c1;
	alignas( 8 ) int8_t c2;
	alignas( 8 ) int8_t c3;
	alignas( 8 ) int8_t c4;
	alignas( 8 ) int8_t c5;
	alignas( 8 ) int8_t c6;
	alignas( 8 ) int8_t c7;
}
s8;

struct State64
{
	alignas( 64 ) int64_t c0;
	alignas( 64 ) int64_t c1;
	alignas( 64 ) int64_t c2;
	alignas( 64 ) int64_t c3;
	alignas( 64 ) int64_t c4;
	alignas( 64 ) int64_t c5;
	alignas( 64 ) int64_t c6;
	alignas( 64 ) int64_t c7;
} s64;

std::atomic<unsigned> total8{0};
std::atomic<unsigned> total64{0};

#define ACCESS_FN( is, ia )                                                    \
	void access_##is##_##ia()                                                  \
	{                                                                          \
		auto start = std::chrono::system_clock::now();                         \
		for( int i = 0; i < 1e6; ++i )                                         \
			s##is.c##ia *= s##is.c##ia;                                        \
		total##is += ( std::chrono::system_clock::now() - start ).count();     \
	}

ACCESS_FN( 8, 0 );
ACCESS_FN( 8, 1 );
ACCESS_FN( 8, 2 );
ACCESS_FN( 8, 3 );
ACCESS_FN( 8, 4 );
ACCESS_FN( 8, 5 );
ACCESS_FN( 8, 6 );
ACCESS_FN( 8, 7 );

ACCESS_FN( 64, 0 );
ACCESS_FN( 64, 1 );
ACCESS_FN( 64, 2 );
ACCESS_FN( 64, 3 );
ACCESS_FN( 64, 4 );
ACCESS_FN( 64, 5 );
ACCESS_FN( 64, 6 );
ACCESS_FN( 64, 7 );

#define ACCESS8( idx ) access_8_##idx()
#define ACCESS64( idx ) access_64_##idx()

#if DT_SIZE == 8
#	define ACCESS( idx ) ACCESS8( idx )
#else
#	define ACCESS( idx ) ACCESS64( idx )
#endif

int main()
{
	std::vector<std::thread> threads;

	threads.emplace_back( [] { ACCESS( 0 ); } );
#if TH_NUM > 1
	threads.emplace_back( [] { ACCESS( 1 ); } );
#endif
#if TH_NUM > 2
	threads.emplace_back( [] { ACCESS( 2 ); } );
#endif
#if TH_NUM > 3
	threads.emplace_back( [] { ACCESS( 3 ); } );
#endif
#if TH_NUM > 4
	threads.emplace_back( [] { ACCESS( 4 ); } );
#endif
#if TH_NUM > 5
	threads.emplace_back( [] { ACCESS( 5 ); } );
#endif
#if TH_NUM > 6
	threads.emplace_back( [] { ACCESS( 6 ); } );
#endif
#if TH_NUM > 7
	threads.emplace_back( [] { ACCESS( 7 ); } );
#endif

	for( auto&& th: threads )
		th.join();

#if DT_SIZE == 8
	std::cout << total8 / ( 1.0e3 * TH_NUM ) << std::endl;
#else
	std::cout << total64 / ( 1.0e3 * TH_NUM ) << std::endl;
#endif

	return 0;
}
