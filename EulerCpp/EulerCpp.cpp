//
// Euler's sum of powers conjecture counterexample test without recursion
// Wikipedia: Euler's conjecture was disproven by L. J. Lander and T. R. Parkin
// in 1966 when, through a direct computer search on a CDC 6600, they found
// a counterexample for k = 5. A total of three primitive (that is, in which
// the summands do not all have a common factor) counterexamples are known:
// 27^5 + 84^5 + 110^5 + 133^5 = 144^5 (Lander & Parkin, 1966).
//
#include "stdafx.h"
#include <iostream>
#include <algorithm>
#include <stdlib.h>
#include <vector>
#include <unordered_map>
#include <map>
#include <chrono>

using namespace std;

typedef std::chrono::high_resolution_clock high_resolution_clock;
typedef std::chrono::milliseconds milliseconds;

// number of ^5 powers to check

 const int N = 150;
// N=150 27 ^ 5 + 84 ^ 5 + 110 ^ 5 + 133 ^ 5 = 144 ^ 5 (Lander & Parkin, 1966).

// const int N = 86000;
// N=86000 for 55^5 + 3183^5 + 28969^5 + 85282^5 = 85359^5 (Frye, 2004).

typedef unsigned long long ullong;

ullong counter = 0;
ullong powers[N];

ullong p5(ullong x)
{
	return x * x * x * x * x;
}

std::unordered_map<ullong, unsigned int> all;

int _tmain(int argc, _TCHAR* argv[])
{
	high_resolution_clock::time_point _start = high_resolution_clock::now();


	cout << "Building table of powers 1 - " << N << "^5\n";
	for (unsigned int i = 0; i < N; ++i)
	{
		powers[i] = p5(i);
		all[powers[i]] = i;
	}

	cout << "Table ready, starting search...\n\n";

	ullong sum = 0;

	unsigned int ind0 = 0x02;
	unsigned int ind1 = 0x02;
	unsigned int ind2 = 0x02;
	unsigned int ind3 = 0x02;

	counter = 0;
	std::chrono::milliseconds speedTime;
	double speed;

	while (true)
	{
		sum = powers[ind0] + powers[ind1] + powers[ind2] + powers[ind3];

		counter++;
		if ((counter & 0xFFFFFF ) == 0 )
		{
			for (int i = 0; i < 78; ++i)
			{
				cout << " ";
			}
			cout << "\r";
			cout << ind3 << "^5 ";
			cout << ind2 << "^5 ";
			cout << ind1 << "^5 ";
			cout << ind0 << "^5 ";

			speedTime = std::chrono::duration_cast<milliseconds>(high_resolution_clock::now() - _start);
			speed = ((double)counter / (double)speedTime.count());
			cout << " speed: " << speed << " itr/ms  total itr: " << counter << "   \r";
		}

		if (all.find(sum) != all.end())
		{
			for (int i = 0; i < 78; ++i)
			{
				cout << " ";
			}
			cout << "\r";
			cout << "\n\nOk, found: ";
			cout << ind3 << "^5 + ";
			cout << ind2 << "^5 + ";
			cout << ind1 << "^5 + ";
			cout << ind0 << "^5 = ";
			cout << all[sum] << "^5\n\n";
			//break;
		}

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
				cout << " ";
			}
			cout << "\r";
			cout << "\nEnded\n";
			break;
		}
	}

	std::chrono::milliseconds time = std::chrono::duration_cast<milliseconds>(high_resolution_clock::now() - _start);
	//speed = double(time.count()) / 1000;

	cout << "\nDone in: " << ( double(time.count()) / 1000 ) << "s\n" << "Total iterations: " << counter << "\n";
	speed = ((double)counter / (double)time.count());
	cout << "Speed: " << speed << " itr/ms";

	getchar();

	return 0;
}

