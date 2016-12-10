//
// Euler's sum of powers conjecture counterexample test without recursion
// Wikipedia: Euler's conjecture was disproven by L. J. Lander and T. R. Parkin
// in 1966 when, through a direct computer search on a CDC 6600, they found
// a counterexample for k = 5. A total of three primitive (that is, in which
// the summands do not all have a common factor) counterexamples are known:
// 27^5 + 84^5 + 110^5 + 133^5 = 144^5 (Lander & Parkin, 1966).
//
#include "stdafx.h"

// uncomment to use 128bit values (25 times slower)
// #define DECIUSE

#include <boost/config.hpp> /* keep it first to prevent nasty warns in MSVC */
#include <iostream>
#include <boost/unordered_map.hpp>
#include <boost/chrono.hpp>
#include <boost/multiprecision/cpp_int.hpp>
#include <stdlib.h>

typedef boost::chrono::high_resolution_clock high_resolution_clock;
typedef boost::chrono::milliseconds milliseconds;

// number of ^5 powers to check
const int N = 150; // max number is ~7131 for 64 bit values ( 18 446 744 073 709 551 616 )
// N=150 27 ^ 5 + 84 ^ 5 + 110 ^ 5 + 133 ^ 5 = 144 ^ 5 (Lander & Parkin, 1966).
// const int N = 86000; // 128 bit variables are required to operate with these values, uncomment define DECIUSE at start of this file
// N=86000 for 55^5 + 3183^5 + 28969^5 + 85282^5 = 85359^5 (Frye, 2004). ( 85359^5 is 4 531 548 087 264 753 520 490 799 )

#ifdef DECIUSE
typedef boost::multiprecision::uint128_t ullong;
#else
typedef unsigned long long ullong;
#endif

typedef unsigned int uint;

ullong counter = 0;
ullong powers[N];
boost::unordered_map<ullong, uint> all;
uint bitseta[256 * 256 * 8];

ullong p5(ullong x)
{
	return x * x * x * x * x;
}

inline bool findBit64(ullong v)
{
	uint bitval = (((uint)((v >> 32) ^ v)) & 0x00FFFFFF);
	uint b = 1 << (bitval & 0x1F);
	uint index = bitval >> 5;
	return ((bitseta[index] & b) > 0);
}

inline void setBit64(ullong v)
{
	uint bitval = (((uint)((v >> 32) ^ v)) & 0x00FFFFFF);
	uint b = 1 << (bitval & 0x1F);
	uint index = bitval >> 5;
	bitseta[index] = bitseta[index] | b;
}

int _tmain(int argc, _TCHAR* argv[])
{
	high_resolution_clock::time_point _start = high_resolution_clock::now();

	ullong hashHit = 0;
	ullong sum = 0;
	counter = 0;
	boost::chrono::milliseconds speedTime;
	ullong speed, hitsspeed;

	uint ind0 = 0x02;
	uint ind1 = 0x02;
	uint ind2 = 0x02;
	uint ind3 = 0x02;

	std::cout << "Building table of powers 1 - " << N << "^5\n";

	all.reserve( N +1 );
	memset(bitseta, 0, sizeof(bitseta));

	for (uint i = 0; i < N; ++i)
	{
		powers[i] = p5(i);
		all[powers[i]] = i;
		setBit64(powers[i]);
	}

	speedTime = boost::chrono::duration_cast<milliseconds>(high_resolution_clock::now() - _start);
	std::cout << "Table ready. Building time: " << ((double)speedTime.count() / 1000 ) << "s, starting search...\n\n";

	while (true)
	{
		sum = powers[ind0] + powers[ind1] + powers[ind2] + powers[ind3];
		if (findBit64(sum))
		{
			hashHit++;
			if (all.find(sum) != all.end())
			{
				for (int i = 0; i < 78; ++i)
				{
					std::cout << " ";
				}
				std::cout << "\r";
				std::cout << "\n\nOk, found: ";
				std::cout << ind3 << "^5 + ";
				std::cout << ind2 << "^5 + ";
				std::cout << ind1 << "^5 + ";
				std::cout << ind0 << "^5 = ";
				std::cout << all[sum] << "^5  ";

				speedTime = boost::chrono::duration_cast<milliseconds>(high_resolution_clock::now() - _start);
				std::cout << "s: " << (double(speedTime.count()) / 1000) << " s. itr: " << counter << "\n\n";
				//break;
			}
		}

		// displaying some progress
		counter++;
		if ((counter & 0xFFFFFF) == 0)
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

			speedTime = boost::chrono::duration_cast<milliseconds>(high_resolution_clock::now() - _start);
			speed = counter / (ullong)speedTime.count();
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
	}

	boost::chrono::milliseconds time = boost::chrono::duration_cast<milliseconds>(high_resolution_clock::now() - _start);
	std::cout << "\nDone in: " << ( double(time.count()) / 1000 ) << "s\n" << "Total iterations: " << counter << "\n";
	speed = counter / (ullong)time.count();
	std::cout << "Speed: " << speed << " itr/ms";
	hitsspeed = counter / (ullong)hashHit;
	std::cout << "\nHash filter ratio: " << hitsspeed << "\n\nPress Enter...";

	getchar();

	return 0;
}

