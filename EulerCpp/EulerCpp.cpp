//
// Euler's sum of powers conjecture counterexample test without recursion
// Wikipedia: Euler's conjecture was disproven by L. J. Lander and T. R. Parkin
// in 1966 when, through a direct computer search on a CDC 6600, they found
// a counterexample for k = 5. A total of three primitive (that is, in which
// the summands do not all have a common factor) counterexamples are known:
// 27^5 + 84^5 + 110^5 + 133^5 = 144^5 (Lander & Parkin, 1966).
//
#include "stdafx.h"

typedef unsigned __int32 uint32;
typedef unsigned __int64 uint64;

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

// search type (select one)
// MAPUSE = unordered_map
//#define SEARCHMAPUSE
// BITSETVECTOR = bitmask + hash + vector
//#define SEARCHBITSETVECTOR
// RANGE SEARCH
#define SEARCHRANGE

// SEARCHBITSETVECTOR size in bytes. Must be power of 2
const uint32 SEARCHBITSETSIZE = 0x20000;
// Do not modify next values
const uint32 SEARCHBITSETSIZEARRAY = SEARCHBITSETSIZE / 4;
const uint32 SEARCHBITSETSIZEMASK = ((SEARCHBITSETSIZE * 8) - 1);

// Total memory consumption
// for SEARCHMAPUSE
// N in unordered_map + N array of values to hold powers
//
// for SEARCHBITSETVECTOR
// ( SEARCHBITSETSIZE * 193) + N array of values to hold powers + N of CompValue

// number of ^5 powers to check
const int N = 86000; // max number is ~7131 for 64 bit values ( 18 446 744 073 709 551 616 )
// Min N=150 for 27 ^ 5 + 84 ^ 5 + 110 ^ 5 + 133 ^ 5 = 144 ^ 5 (Lander & Parkin, 1966).
// const int N = 86000; // 128 bit variables are required to operate with these values, uncomment define UINT128USE or DECIUSE at start of this file
// Min N=86000 for 55^5 + 3183^5 + 28969^5 + 85282^5 = 85359^5 (Frye, 2004). ( 85359^5 is 4 531 548 087 264 753 520 490 799 )


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

#ifdef UINT128USE
typedef uint128 mainType;
#endif
#ifdef BOOST128USE
typedef boost::multiprecision::uint128_t mainType;
#endif
#ifdef UINT64USE
typedef unsigned long long mainType;
#endif

//####################################
// Array of powers
mainType powers[N];
//
//####################################


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
	CompValue(mainType f, uint32 n)
	{
		fivePower = f;
		number = n;
	}
	mainType fivePower;
	uint32 number;
};

#ifdef BOOSTUSE
boost::container::vector<uint32> foundItems;
	#ifdef SEARCHMAPUSE
	boost::unordered_map<mainType, uint32> all;
	#endif
	#ifdef SEARCHBITSETVECTOR
	boost::container::vector<CompValue*> setMap[ SEARCHBITSETSIZE * 8 ];
	#endif
#else
std::vector<uint32> foundItems;
	#ifdef SEARCHMAPUSE
	std::unordered_map<mainType, uint32> all;
	#endif
	#ifdef SEARCHBITSETVECTOR
	std::vector<CompValue*> setMap[ SEARCHBITSETSIZE * 8 ];
	#endif
#endif

#ifdef SEARCHBITSETVECTOR

uint32 bitseta[SEARCHBITSETSIZEARRAY];

inline uint32 findBit(mainType fivePower)
{
#ifdef UINT128USE
	//uint32 bitval = fivePower.GetHash16();
	uint32 bitval = fivePower.GetHash32() & SEARCHBITSETSIZEMASK;
#else
	uint32 bitval = (((uint32)((fivePower >> 32) ^ fivePower)));
	bitval = (((bitval >> 16) ^ bitval) & SEARCHBITSETSIZEMASK);
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

inline void setBit(mainType fivePower, uint32 number)
{
#ifdef UINT128USE
	//uint32 bitval = fivePower.GetHash16();
	uint32 bitval = fivePower.GetHash32() & SEARCHBITSETSIZEMASK;
#else
	uint32 bitval = (((uint32)((fivePower >> 32) ^ fivePower)));
	bitval = (((bitval >> 16) ^ bitval) & SEARCHBITSETSIZEMASK);
#endif

	setMap[bitval].push_back(new CompValue(fivePower, number));

	uint32 b = 1 << (bitval & 0x1F);
	uint32 index = bitval >> 5;
	bitseta[index] = bitseta[index] | b;
}
#endif

#ifdef SEARCHRANGE

uint32 lastRangeIndex = 0;

inline uint32 findInRange(mainType fivePower, uint32 startIndex)
{
	while (startIndex < N)
	{
		lastRangeIndex = startIndex;
		if (powers[startIndex] > fivePower)
		{
			return 0;
		}
		if (powers[startIndex] == fivePower)
		{
			return startIndex;
		}
		++startIndex;
	}
	return 0;
}
#endif

mainType p5(mainType x)
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
	std::cout << "\r";
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
	uint32 foundVal;
	mainType sum = 0U, baseSum = 0U;

	uint32 ind0 = 0x02;
	uint32 ind1 = 0x02;
	uint32 ind2 = 0x02;
	uint32 ind3 = 0x02;

#ifdef SEARCHRANGE
	lastRangeIndex = 0x02;
#endif

	// base sum without ind0 for performance
	baseSum = powers[ind1] + powers[ind2] + powers[ind3];

	while (true)
	{
		sum = baseSum + powers[ind0];

#ifdef SEARCHRANGE
		foundVal = findInRange( sum, lastRangeIndex);
		if (foundVal > 0)
#endif

#ifdef SEARCHBITSETVECTOR
		foundVal = findBit(sum);
		if (foundVal > 0)
#endif
		{
#ifdef SEARCHMAPUSE
			if (all.find(sum) != all.end())
			{
				foundVal = all[sum];
#endif
				// clear line
				clearLine();
				for (auto ind : foundItems)
				{
					if ((foundVal % ind ) == 0 )
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
#ifdef SEARCHMAPUSE
			}
#endif
		}

		// displaying some progress
		counter++;
		if ((counter & 0x3FFFFFF) == 0)
		{
			clearLine();
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
			//lastRangeIndex = 0x02;
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
#ifdef SEARCHRANGE
		lastRangeIndex = 0x02;
		if (ind1 > lastRangeIndex)
		{
			lastRangeIndex = ind1;
		}
		if (ind2 > lastRangeIndex)
		{
			lastRangeIndex = ind2;
		}
		if (ind3 > lastRangeIndex)
		{
			lastRangeIndex = ind3;
		}
#endif
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

