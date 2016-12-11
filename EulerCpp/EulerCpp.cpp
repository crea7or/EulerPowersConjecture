//
// Euler's sum of powers conjecture counterexample test without recursion
// Wikipedia: Euler's conjecture was disproven by L. J. Lander and T. R. Parkin
// in 1966 when, through a direct computer search on a CDC 6600, they found
// a counterexample for k = 5. A total of three primitive (that is, in which
// the summands do not all have a common factor) counterexamples are known:
// 27^5 + 84^5 + 110^5 + 133^5 = 144^5 (Lander & Parkin, 1966).
//
#include "stdafx.h"
#include "uint128.h"

// uncomment to use unsigned int 64 bits  ~310 000 its/ms
#define UINT64USE
// uncomment to use 128bit values by me ( 40% slower than uint64 )  ~180 000 its/ms
// #define UINT128USE
// uncomment to use 128bit values by boost ( 25 times slower than uint64 )   ~17 000 its/ms
// #define BOOST128USE


// use boost or stl
#define BOOSTUSE // stl otherwise

// boost:413608 its/ms
// std : 367906 its/ms
// boost map is 10% faster


#ifdef BOOSTUSE
#include <boost/config.hpp> /* keep it first to prevent nasty warns in MSVC */
#include <boost/unordered_map.hpp>
#include <boost/chrono.hpp>
#include <boost/multiprecision/cpp_int.hpp>
#include <boost/container/vector.hpp>
#else
#include <unordered_map>
#include <chrono>
#include <vector>
#endif

#include <iostream>
#include <stdlib.h>

#ifdef BOOSTUSE
typedef boost::chrono::high_resolution_clock high_resolution_clock;
typedef boost::chrono::milliseconds milliseconds;
#else
typedef std::chrono::high_resolution_clock high_resolution_clock;
typedef std::chrono::milliseconds milliseconds;
#endif

// number of ^5 powers to check
const int N = 500; // max number is ~7131 for 64 bit values ( 18 446 744 073 709 551 616 )
// N=150 27 ^ 5 + 84 ^ 5 + 110 ^ 5 + 133 ^ 5 = 144 ^ 5 (Lander & Parkin, 1966).
// const int N = 86000; // 128 bit variables are required to operate with these values, uncomment define UINT128USE or DECIUSE at start of this file
// N=86000 for 55^5 + 3183^5 + 28969^5 + 85282^5 = 85359^5 (Frye, 2004). ( 85359^5 is 4 531 548 087 264 753 520 490 799 )

#ifdef UINT128USE
typedef uint128 ullong;
#endif
#ifdef BOOST128USE
typedef boost::multiprecision::uint128_t ullong;
#endif
#ifdef UINT64USE
typedef unsigned long long ullong;
#endif

typedef unsigned __int32 uint32;
typedef unsigned __int64 uint64;


ullong powers[N];

#ifdef BOOSTUSE
boost::unordered_map<ullong, uint32> all;
boost::container::vector<ullong> foundItems;
#else
std::unordered_map<ullong, uint32> all;
std::vector<ullong> foundItems;
#endif

uint32 bitseta[2048];

inline bool findBit64(ullong v)
{
#ifdef UINT128USE
	uint32 bitval = v.GetHash16();
#else
	uint32 bitval = (((uint32)((v >> 32) ^ v)));
	bitval = (((bitval >> 16) ^ bitval) & 0xFFFF);
#endif

	uint32 b = 1 << (bitval & 0x1F);
	uint32 index = bitval >> 5;
	return ((bitseta[index] & b) > 0);
}

inline void setBit64(ullong v)
{
#ifdef UINT128USE
	uint32 bitval = v.GetHash16();
#else
	uint32 bitval = (((uint32)((v >> 32) ^ v)));
	bitval = (((bitval >> 16) ^ bitval) & 0xFFFF);
#endif
	uint32 b = 1 << (bitval & 0x1F);
	uint32 index = bitval >> 5;
	bitseta[index] = bitseta[index] | b;
}

ullong p5(ullong x)
{
	return x * x * x * x * x;
}

#ifdef BOOSTUSE
// Hash function to make int128 work with boost::hash.
std::size_t hash_value(uint128 const &input)
{
	boost::hash<unsigned long long> hasher;
	return hasher(input.GetHash64());
}
#else
namespace std
{
	template <>
	struct hash<uint128>
	{
		std::size_t operator()(const uint128& input) const
		{
			using std::size_t;
			using std::hash;
			return hash<unsigned long long>()(input.GetHash64());
		}
	};
}
#endif

milliseconds duration_cast_ms(high_resolution_clock::time_point minus)
{
#ifdef BOOSTUSE
	return boost::chrono::duration_cast<milliseconds>(high_resolution_clock::now() - minus);
#else
	return std::chrono::duration_cast<milliseconds>(high_resolution_clock::now() - minus);
#endif
}


int _tmain(int argc, _TCHAR* argv[])
{

	high_resolution_clock::time_point _start = high_resolution_clock::now();

	milliseconds speedTime;

	std::cout << "Building table of powers 1 - " << N << "^5\n";

	all.reserve( N +1 );
	memset(bitseta, 0, sizeof(bitseta));

	for (uint32 i = 0; i < N; ++i)
	{
		powers[i] = p5(i);
		all[powers[i]] = i;
		setBit64(powers[i]);
	}

	speedTime = duration_cast_ms( _start );
	std::cout << "Table ready. Building time: " << ((double)speedTime.count() / 1000 ) << "s, starting search...\n\n";


	uint64 hashHit = 0, counter = 0, speed = 0, hitsspeed = 0;
	ullong sum = 0U, baseSum = 0U;

	uint32 ind0 = 0x02;
	uint32 ind1 = 0x02;
	uint32 ind2 = 0x02;
	uint32 ind3 = 0x02;

	// base sum without ind0 for performance
	baseSum = powers[ind1] + powers[ind2] + powers[ind3];

	while (true)
	{
		sum = baseSum + powers[ind0];

		if (findBit64(sum))
		{
			hashHit++;
			if (all.find(sum) != all.end())
			{
				//foundItems
				for (int i = 0; i < 78; ++i)
				{
					std::cout << " ";
				}

				// res
				uint32 res = all[sum];

				std::cout << "\r";
				std::cout << "\n\nOk, found: ";
				std::cout << ind3 << "^5 + ";
				std::cout << ind2 << "^5 + ";
				std::cout << ind1 << "^5 + ";
				std::cout << ind0 << "^5 = ";
				std::cout << res << "^5  ";

				foundItems.push_back(res);

				speedTime = duration_cast_ms( _start );
				std::cout << "s: " << (double(speedTime.count()) / 1000) << " s. itr: " << counter << "\n\n";
			}
		}

		// displaying some progress
		counter++;
		if ((counter & 0x3FFFFFF) == 0)
		{
			for (int i = 0; i < 78; ++i)
			{
				std::cout << " ";
			}
			std::cout << "\r";
			std::cout << ind3 << "^5 ";
			std::cout << ind2 << "^5 ";
			std::cout << ind1 << "^5 ";
			std::cout << ind0 << "^5 ";

			speedTime = duration_cast_ms( _start);
			speed = counter / (uint64)speedTime.count();
			hitsspeed = counter / hashHit;
			// speed: iterations per millisecond, counter: total iterations, hitspeed: hash filter performance
			std::cout << "s: " << speed << " i/ms itrs: " << counter << " hh: " << hitsspeed << "\r";
		}

		// displaying some progress

		++ind0;
		if (ind0 < N)
		{
			continue;
		}
		else
		{
			ind0 = ++ind1;
		}
		if (ind1 >= N)
		{
			ind0 = ind1 = ++ind2;
		}
		if (ind2 >= N)
		{
			ind0 = ind1 = ind2 = ++ind3;
		}
		if (ind3 >= N)
		{
			for (int i = 0; i < 78; ++i)
			{
				std::cout << " ";
			}
			std::cout << "\r";
			std::cout << "\nEnded\n";
			break;
		}

		// refresh without ind0
		baseSum = powers[ind1] + powers[ind2] + powers[ind3];
	}

	milliseconds time = duration_cast_ms(_start);
	std::cout << "\nDone in: " << ( double(time.count()) / 1000 ) << "s\n" << "Total iterations: " << counter << "\n";
	speed = counter / (uint64)time.count();
	std::cout << "Speed: " << speed << " itr/ms";
	hitsspeed = counter / hashHit;
	std::cout << "\nHash filter ratio: " << hitsspeed << "\n\nPress Enter...";

	//getchar();

	return 0;
}

