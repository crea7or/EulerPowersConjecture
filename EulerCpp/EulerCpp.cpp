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

// Value define
// uncomment to use unsigned int 64 bits. For N=500: ~440 000 000 its/s ( Max N=7100 )
//#define UINT64USE
// uncomment to use 128bit values by me ( 30% slower than uint64 ). For N=500: ~300 000 000 its/s(N=500)  ( Max N=50859008 )
#define UINT128USE
// uncomment to use 128bit values by boost ( 17 times slower than uint64 ). For N=500: ~26 000 000 its/s(N=500) ( Max N=50859008 )
//#define BOOST128USE


// Lib define
// use boost as library or stl
#define BOOSTUSE // stl otherwise

// boost map: 413608 its/ms
// std   mpa: 367906 its/ms
// boost map is 10% faster

#ifdef BOOSTUSE
#include <boost/config.hpp> /* keep it first to prevent nasty warns in MSVC */
#include <boost/unordered_map.hpp>
#include <boost/chrono.hpp>
#include <boost/multiprecision/cpp_int.hpp>
#include <boost/container/vector.hpp>
#else // stl
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

// N=1 000 000 - 12600
// number of ^5 powers to check
const int N = 86000; // max number is ~7131 for 64 bit values ( 18 446 744 073 709 551 616 )
// Min N=150 for 27 ^ 5 + 84 ^ 5 + 110 ^ 5 + 133 ^ 5 = 144 ^ 5 (Lander & Parkin, 1966).
// const int N = 86000; // 128 bit variables are required to operate with these values, uncomment define UINT128USE or DECIUSE at start of this file
// Min N=86000 for 55^5 + 3183^5 + 28969^5 + 85282^5 = 85359^5 (Frye, 2004). ( 85359^5 is 4 531 548 087 264 753 520 490 799 )

#ifdef UINT128USE
typedef uint128 ullong;
#endif
#ifdef BOOST128USE
typedef boost::multiprecision::uint128_t ullong;
#endif
#ifdef UINT64USE
typedef unsigned long long ullong;
#endif

// search type
// MAPUSE = unordered_map
//#define SEARCHMAPUSE
// BITSETVECTOR = bitmask + hash + vector
#define SEARCHBITSETVECTOR

typedef unsigned __int32 uint32;
typedef unsigned __int64 uint64;

ullong powers[N];

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


struct CompValue
{
	CompValue(ullong f, uint32 n)
	{
		fivePower = f;
		number = n;
	}
	ullong fivePower;
	uint32 number;
};


#ifdef BOOSTUSE
boost::container::vector<uint32> foundItems;
	#ifdef SEARCHMAPUSE
	boost::unordered_map<ullong, uint32> all;
	#endif
	#ifdef SEARCHBITSETVECTOR
	boost::container::vector<CompValue*> setMap[256 * 256 * 16];
	#endif
#else
std::vector<uint32> foundItems;
	#ifdef SEARCHMAPUSE
	std::unordered_map<ullong, uint32> all;
	#endif
	#ifdef SEARCHBITSETVECTOR
	std::vector<CompValue*> setMap[256 * 256 * 16];
	#endif
#endif

#ifdef SEARCHBITSETVECTOR
// buffer size is effective up to N=1000000 (over unordered_map performance)
uint32 bitseta[32768];

inline uint32 findBit(ullong fivePower)
{
#ifdef UINT128USE
	//uint32 bitval = fivePower.GetHash16();
	uint32 bitval = fivePower.GetHash32() & 0xFFFFF;
#else
	uint32 bitval = (((uint32)((fivePower >> 32) ^ fivePower)));
	bitval = (((bitval >> 16) ^ bitval) & 0xFFFF);
#endif

	uint32 b = 1 << (bitval & 0x1F);
	uint32 index = bitval >> 5;

	if((bitseta[index] & b) > 0)
	{
		for (auto itm : setMap[bitval])
		{
			if (itm->fivePower == fivePower)
			{
				return itm->number;
			}
		}
	}
	return 0;
}

inline void setBit(ullong fivePower, uint32 number)
{
#ifdef UINT128USE
	//uint32 bitval = fivePower.GetHash16();
	uint32 bitval = fivePower.GetHash32() & 0xFFFFF;
#else
	uint32 bitval = (((uint32)((fivePower >> 32) ^ fivePower)));
	bitval = (((bitval >> 16) ^ bitval) & 0xFFFF);
#endif

	setMap[bitval].push_back(new CompValue(fivePower, number));

	uint32 b = 1 << (bitval & 0x1F);
	uint32 index = bitval >> 5;
	bitseta[index] = bitseta[index] | b;
}
#endif

ullong p5(ullong x)
{
	return x * x * x * x * x;
}


milliseconds duration_cast_ms(high_resolution_clock::time_point minus)
{
#ifdef BOOSTUSE
	return boost::chrono::duration_cast<milliseconds>(high_resolution_clock::now() - minus);
#else
	return std::chrono::duration_cast<milliseconds>(high_resolution_clock::now() - minus);
#endif
}

void clearLine()
{
	for (int i = 0; i < 77; ++i)
	{
		std::cout << " ";
	}
}

int _tmain(int argc, _TCHAR* argv[])
{
	high_resolution_clock::time_point _start = high_resolution_clock::now();

	milliseconds speedTime;

	std::cout << "Building table of powers 1 - " << N << "^5\n";
#ifdef SEARCHBITSETVECTOR
	memset(bitseta, 0, sizeof(bitseta));
#endif

	for (uint32 i = 0; i < N; ++i)
	{
		powers[i] = p5(i);
#ifdef SEARCHMAPUSE
		all[powers[i]] = i;
#endif
#ifdef SEARCHBITSETVECTOR
		setBit(powers[i], i);
#endif
	}

	speedTime = duration_cast_ms( _start );
	std::cout << "Table ready. Building time: " << ((double)speedTime.count() / 1000 ) << "s, starting search...\n\n";

	uint64 counter = 1, speed = 0, hitsspeed = 0;
	uint32 foundTest, foundVal;
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

#ifdef SEARCHBITSETVECTOR
		foundVal = findBit(sum); // comment when using with map
		if (foundVal > 0) // comment when using with map
#endif
		{
#ifdef SEARCHMAPUSE
			if (all.find(sum) != all.end())
#endif
			{
#ifdef SEARCHMAPUSE
				//foundItems
				//only with map
				foundVal = all[sum];
#endif

				// clear line
				clearLine();
				std::cout << "\r";

				for (auto ind : foundItems)
				{
					foundTest = foundVal / ind;
					if ((foundTest * ind) == foundVal)
					{
						// duplicate
						foundVal = 0;
						break;
					}
				}
				if (foundVal != 0)
				{
					std::cout << "\n\nOk, found: ";
					std::cout << ind3 << "^5 + ";
					std::cout << ind2 << "^5 + ";
					std::cout << ind1 << "^5 + ";
					std::cout << ind0 << "^5 = ";
					std::cout << foundVal << "^5  ";
					// store to check it later
					foundItems.push_back(foundVal);
					speedTime = duration_cast_ms(_start);
					std::cout << "s: " << (double(speedTime.count()) / 1000) << " s. itr: " << counter << "\n\n";
				}
				else
				{
					std::cout << "duplicate";
				}
			}
		}

		// displaying some progress
		counter++;
		if ((counter & 0x3FFFFFF) == 0)
		{
			clearLine();
			std::cout << "\r";
			std::cout << ind3 << "^5 ";
			std::cout << ind2 << "^5 ";
			std::cout << ind1 << "^5 ";
			std::cout << ind0 << "^5 ";

			speedTime = duration_cast_ms( _start);
			speed = counter / (uint64)speedTime.count();
			// speed: iterations per millisecond, counter: total iterations
			std::cout << "s: " << speed << " i/ms itrs: " << counter << "\r";
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
			break;
		}
		// refresh without ind0
		baseSum = powers[ind1] + powers[ind2] + powers[ind3];
	}

	clearLine();

	milliseconds time = duration_cast_ms(_start);
	std::cout << "\nDone in: " << ( double(time.count()) / 1000 ) << "s\n" << "Total iterations: " << counter << "\n";
	speed = counter / (uint64)time.count();
	std::cout << "Speed: " << speed << " itr/ms\n";

	//getchar();

#ifdef SEARCHBITSETVECTOR
	for (auto itm : setMap)
	{
		for (auto comp : itm)
		{
			delete comp;
		}
	}
#endif

	return 0;
}

