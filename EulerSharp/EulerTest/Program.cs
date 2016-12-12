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
// Uncomment to use UInt64
//#define UINT64USE
// Uncomment to use UInt128
#define UINT128USE
// Uncomment to use decimal - slow and only for test reasons
//#define DECIUSE


using System;
using System.Collections.Generic;
using System.Linq;
using Dirichlet.Numerics;


namespace EulerTest
{
	class Program
	{
		// number of ^5 powers to check
		const int N = 290; // max number is ~7131 for 64 bit values ( 18 446 744 073 709 551 616 )
		// N=150 27 ^ 5 + 84 ^ 5 + 110 ^ 5 + 133 ^ 5 = 144 ^ 5 (Lander & Parkin, 1966).
		// const int N = 86000
		// N=86000 for 55^5 + 3183^5 + 28969^5 + 85282^5 = 85359^5 (Frye, 2004). ( 85359^5 is 4 531 548 087 264 753 520 490 799 )
		// Average speed with decimal type about  ~30 000 000 tests per second.
		// Average speed with Uint128 type about ~100 000 000 tests per second.
		// Average speed with Uint64  type about ~200 000 000 tests per second.

		static List<uint> foundItms = new List<uint>();

#if ( DECIUSE)
		static decimal p5( decimal x )
		{
			return x * x * x * x * x;
		}
		static decimal[] pa = new decimal[ N ];
		static Dictionary<decimal, uint> all = new Dictionary<decimal, uint>();
#endif
#if ( UINT64USE)
		static UInt64 p5( UInt64 x )
		{
			return x * x * x * x * x;
		}
		static UInt64[] pa = new UInt64[ N ];
		static Dictionary<UInt64, uint> all = new Dictionary<UInt64, uint>();
#endif
#if ( UINT128USE)
		static UInt128 p5( UInt128 x )
		{
			return x * x * x * x * x;
		}
		static UInt128[] pa = new UInt128[ N ];
		static Dictionary<UInt128, uint> all = new Dictionary<UInt128, uint>();
#endif

		static uint[] bitseta = new uint[ 2048 ];

#if ( DECIUSE)
		static bool findBit64(decimal vd )
		{
			UInt64 v = (UInt64)vd;
#endif
#if ( UINT64USE)
		static bool findBit64(UInt64 v )
		{
#endif
#if ( UINT128USE)
		static bool findBit64(UInt128 v )
		{
#endif
			uint bitval = ( ( uint )v.GetHashCode() ) & 0x0000FFFF;
			uint b = ( uint )1 << (int)( bitval & ( uint )0x1F );
			uint index = bitval >> 5;
			return ((bitseta[index] & b) > 0);
		}

#if ( DECIUSE)
		static void setBit64( decimal vd )
		{
			UInt64 v = ( UInt64 )vd;
#endif
#if ( UINT64USE)
		static void setBit64( UInt64 v )
		{
#endif
#if ( UINT128USE)
		static void setBit64( UInt128 v )
		{
#endif
			uint bitval = (( uint )v.GetHashCode()) & 0x0000FFFF;
			uint b = ( uint )1 << ( int )( bitval & ( uint )0x1F );
			uint index = bitval >> 5;
			bitseta[index] = bitseta[index] | b;
		}

		static void clearLine()
		{
			Console.SetCursorPosition( 0, Console.CursorTop - 1 );
			Console.Write( Enumerable.Repeat<char>( ' ', Console.BufferWidth ).ToArray() );
			Console.SetCursorPosition( 0, Console.CursorTop - 1 );
		}

		static void Main( string[] args )
		{
			var watch = System.Diagnostics.Stopwatch.StartNew();

			Console.WriteLine( "Building table of powers 1 - " + N + "^5\n");

			for ( uint i = 0; i < N; ++i )
			{
#if ( DECIUSE)
				pa[ i ] = p5( ( decimal )i );
#endif
#if ( UINT64USE)
				pa[ i ] = p5( ( UInt64 )i );
#endif
#if ( UINT128USE)
				pa[ i ] = p5( ( UInt128 )i );
#endif
				all[ pa[ i ] ] = ( uint )i;
				setBit64( (UInt64)pa[ i ] );
			}
			Console.WriteLine( "Table ready. Building time: " + ( watch.Elapsed.TotalMilliseconds / 1000 ) + "sec. Starting search...\n\n");


			UInt64 iterations = 0, hashHit = 0;

#if ( DECIUSE)
			decimal sum = 0, preSum = 0;
#endif
#if ( UINT64USE)
			UInt64 sum = 0, preSum = 0;
#endif
#if ( UINT128USE)
			UInt128 sum = 0, preSum = 0;
#endif

			uint ind0 = 0x02;
			uint ind1 = 0x02;
			uint ind2 = 0x02;
			uint ind3 = 0x02;
			uint foundIndex = 0;
			uint testIndex;

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
						foundIndex = all[ sum ];
						foreach ( uint index in foundItms )
						{
							testIndex = ( foundIndex / index );
							if (( testIndex * index ) == foundIndex )
							{
								foundIndex = 0;
								break;
							}
						}

						clearLine();
						if ( foundIndex != 0 )
						{
							Console.WriteLine( "Found: " + ind3 + " " + ind2 + " " + ind1 + " " + ind0 + " = " + foundIndex + " t: " + ( watch.Elapsed.TotalMilliseconds / 1000 ) + " sec. itr: " + iterations + "\n\n" );
							foundItms.Add( foundIndex );
						}
						else
						{
							// same value found
							Console.WriteLine( "Duplicate" );
						}
					}
				}


				// displaying some progress
				if ( ( iterations & 0x3FFFFF ) == 0 )
				{
					clearLine();
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
			clearLine();
			Console.WriteLine( "\nDone in: " + ( watch.Elapsed.TotalMilliseconds / 1000 ) + " sec. itr: " + iterations + " sp: " + ( ( UInt64 )iterations / ( UInt64 )watch.Elapsed.TotalMilliseconds ) + " hh: " + ( ( UInt64 )iterations / ( UInt64 )hashHit ) );

			//Console.WriteLine( "Press Enter to close\n" );
			//Console.Read();
		}
	}
}
