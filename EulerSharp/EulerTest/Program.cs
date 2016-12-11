//
// Euler's sum of powers conjecture counterexample test without recursion
// Wikipedia: Euler's conjecture was disproven by L. J. Lander and T. R. Parkin
// in 1966 when, through a direct computer search on a CDC 6600, they found
// a counterexample for k = 5. A total of three primitive (that is, in which
// the summands do not all have a common factor) counterexamples are known:
// 27^5 + 84^5 + 110^5 + 133^5 = 144^5 (Lander & Parkin, 1966).
//
// This is bruteforce algorithm to find a such counterexamples.
//
// Uncomment to use decimal
#define DECIUSE

using System;
using System.Collections.Generic;
using System.Linq;


namespace EulerTest
{
	class Program
	{
		// number of ^5 powers to check
		const int N = 250; // max number is ~7131 for 64 bit values ( 18 446 744 073 709 551 616 )
		// N=150 27 ^ 5 + 84 ^ 5 + 110 ^ 5 + 133 ^ 5 = 144 ^ 5 (Lander & Parkin, 1966).
		// const int N = 86000
		// N=86000 for 55^5 + 3183^5 + 28969^5 + 85282^5 = 85359^5 (Frye, 2004). ( 85359^5 is 4 531 548 087 264 753 520 490 799 )
		// Average speed with decimal type about 10000000 tests per second.
		// Average speed with Uint64 type about 100000000 tests per second.


#if ( DECIUSE)
		static decimal p5( decimal x )
		{
			return x * x * x * x * x;
		}
		static decimal[] pa = new decimal[ N ];
		static Dictionary<decimal, uint> all = new Dictionary<decimal, uint>();
#else
		static UInt64 p5( UInt64 x )
		{
			return x * x * x * x * x;
		}
		static UInt64[] pa = new UInt64[ N ];
		static Dictionary<UInt64, uint> all = new Dictionary<UInt64, uint>();
#endif

		static uint[] bitseta = new uint[ 2048 ];

#if ( DECIUSE)
		static bool findBit64(decimal vd )
		{
			UInt64 v = (UInt64)vd;
#else
		static bool findBit64(UInt64 v )
		{
#endif
			uint bitval = (((uint)((v >> 32) ^ v) ^ ((uint)((v >> 48)))) & 0x0000FFFF);
			uint b = ( uint )1 << (int)( bitval & ( uint )0x1F );
			uint index = bitval >> 5;
			return ((bitseta[index] & b) > 0);
		}

#if ( DECIUSE)
		static void setBit64( decimal vd )
		{
			UInt64 v = ( UInt64 )vd;
#else
		static void setBit64( UInt64 v )
		{
#endif
			uint bitval = (((uint)((v >> 32) ^ v) ^ ((uint)((v >> 48)))) & 0x0000FFFF);
			uint b = ( uint )1 << ( int )( bitval & ( uint )0x1F );
			uint index = bitval >> 5;
			bitseta[index] = bitseta[index] | b;
		}

		static void Main( string[] args )
		{
			var watch = System.Diagnostics.Stopwatch.StartNew();

			Console.WriteLine( "Building table of powers 1 - " + N + "^5\n");

			for ( uint i = 0; i < N; ++i )
			{
#if ( DECIUSE)
				pa[ i ] = p5( ( decimal )i );
#else
				pa[ i ] = p5( ( UInt64 )i );
#endif
				all[ pa[ i ] ] = ( uint )i;
				setBit64( (UInt64)pa[ i ] );
			}
			Console.WriteLine( "Table ready. Building time: " + ( watch.Elapsed.TotalMilliseconds / 1000 ) + "sec. Starting search...\n\n");

#if ( DECIUSE)
			decimal iterations = 0, hashHit = 0;
			decimal sum = 0, preSum = 0;
#else
			UInt64 iterations = 0, hashHit = 0;
			UInt64 sum = 0, preSum = 0;
#endif

			UInt64 u64 = 0;

			uint ind0 = 0x02;
			uint ind1 = 0x02;
			uint ind2 = 0x02;
			uint ind3 = 0x02;


			preSum = pa[ ind1 ] + pa[ ind2 ] + pa[ ind3 ];

			while ( true )
			{
				iterations++;

				sum = preSum + pa[ ind0 ];

				if ( findBit64( ( UInt64 )sum ) )
				{
					hashHit++;
					if ( all.ContainsKey( sum ) )
					{
						Console.SetCursorPosition( 0, Console.CursorTop - 1 );
						Console.Write( Enumerable.Repeat<char>( ' ', Console.BufferWidth ).ToArray() );
						Console.SetCursorPosition( 0, Console.CursorTop - 1 );
						Console.WriteLine( "Found: " + ind3 + " " + ind2 + " " + ind1 + " " + ind0 + " = " + all[ sum ] + " t: " + ( watch.Elapsed.TotalMilliseconds / 1000 ) + " sec. itr: " + iterations + "\n\n" );
					}
				}


				// displaying some progress
				u64 = ( UInt64 )iterations;
				if ( ( u64 & 0x3FFFFF ) == 0 )
				{
					Console.SetCursorPosition( 0, Console.CursorTop - 1 );
					Console.Write( Enumerable.Repeat<char>( ' ', Console.BufferWidth ).ToArray() );
					Console.SetCursorPosition( 0, Console.CursorTop - 1 );

					Console.WriteLine( ind3 + "^5 " + ind2 + "^5 " + ind1 + "^5 " + ind0 + "  t: " + ( watch.Elapsed.TotalMilliseconds / 1000 ) + " sec. itr: " + iterations + " sp: " + ( ( UInt64 )iterations / ( UInt64 )watch.Elapsed.TotalMilliseconds ) + " hh: " + ( (UInt64)iterations / ( UInt64 )hashHit ));
				}
				// displaying some progress

				++ind0;
				if ( ind0 < N )
				{
					continue;
				}
				else
				{
					ind0 = ++ind1;
				}
				if ( ind1 >= N )
				{
					ind0 = ind1 = ++ind2;
				}
				if ( ind2 >= N )
				{
					ind0 = ind1 = ind2 = ++ind3;
				}
				if ( ind3 >= N )
				{
					break;
				}
				preSum = pa[ ind1 ] + pa[ ind2 ] + pa[ ind3 ];
			}

			watch.Stop();
			Console.WriteLine( "\nDone in: " + ( watch.Elapsed.TotalMilliseconds / 1000 ) + " sec. itr: " + iterations + " sp: " + ( ( UInt64 )iterations / ( UInt64 )watch.Elapsed.TotalMilliseconds ) + " hh: " + ( ( UInt64 )iterations / ( UInt64 )hashHit ) );

			//Console.WriteLine( "Press Enter to close\n" );
			//Console.Read();
		}
	}
}
