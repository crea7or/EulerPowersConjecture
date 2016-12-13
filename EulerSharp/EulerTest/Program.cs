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
using System.Runtime.CompilerServices;

#if ( DECIUSE)
using mainType = System.Decimal;
#endif
#if ( UINT64USE)
using mainType = System.UInt64;
#endif
#if ( UINT128USE)
using mainType = Dirichlet.Numerics.UInt128;
#endif


namespace EulerTest
{
	class Program
	{
		// number of ^5 powers to check
		const int N = 500; // max number is ~7131 for 64 bit values ( 18 446 744 073 709 551 616 )
		// N=150 27 ^ 5 + 84 ^ 5 + 110 ^ 5 + 133 ^ 5 = 144 ^ 5 (Lander & Parkin, 1966).
		// const int N = 86000 (uint128 or decimal only)
		// N=86000 for 55^5 + 3183^5 + 28969^5 + 85282^5 = 85359^5 (Frye, 2004). ( 85359^5 is 4 531 548 087 264 753 520 490 799 )
		// Average speed with decimal type about  ~39 000 000 tests per second.
		// Average speed with Uint128 type about ~140 000 000 tests per second.
		// Average speed with Uint64  type about ~280 000 000 tests per second.

		const UInt32 SEARCHBITSETSIZE = 0x20000; // bytes

		static List<uint> foundItms = new List<uint>();

		/* slower in c#
		struct CompValue
		{
			public CompValue( mainType f, UInt32 n )
			{
				fivePower = f;
				number = n;
			}
			public mainType fivePower;
			public UInt32 number;
		};
		static List< CompValue >[] setMap = new List< CompValue >[ 65536 ];
		*/
		static mainType p5( mainType x )
		{
			return x * x * x * x * x;
		}
		static mainType[] pa = new mainType[ N ];
		static Dictionary< mainType, uint> all = new Dictionary< mainType, uint>();
		static uint[] bitseta = new uint[ SEARCHBITSETSIZE / 4 ];

		[MethodImpl( MethodImplOptions.AggressiveInlining )]
		static bool findBit( mainType v )
		{
			uint bitval = ( ( uint )v.GetHashCode() ) & ( SEARCHBITSETSIZE - 1 );
			uint b = ( uint )1 << (int)( bitval & ( uint )0x1F );
			uint index = bitval >> 5;
			return ((bitseta[index] & b) > 0);
		}

		[MethodImpl( MethodImplOptions.AggressiveInlining )]
		static void setBit( mainType v )
		{
			uint bitval = ( ( uint )v.GetHashCode() ) & ( SEARCHBITSETSIZE - 1 );
			uint b = ( uint )1 << ( int )( bitval & ( uint )0x1F );
			uint index = bitval >> 5;
			bitseta[index] = bitseta[index] | b;
		}

		/* slower in c#
		static void setBit( mainType fivePower, UInt32 number )
		{
			uint bitval = (( uint )fivePower.GetHashCode()) & 0x0000FFFF;
			uint b = ( uint )1 << ( int )( bitval & ( uint )0x1F );

			if ( setMap[ bitval ] == null )
			{
				setMap[ bitval ] = new List<CompValue>();
			}
			setMap[ bitval ].Add( new CompValue( fivePower, number ));

			uint index = bitval >> 5;
			bitseta[index] = bitseta[index] | b;
		}

		static UInt32 findBit( mainType v )
		{
			uint bitval = ( ( uint )v.GetHashCode() ) & 0x0000FFFF;
			uint b = ( uint )1 << (int)( bitval & ( uint )0x1F );
			uint index = bitval >> 5;

			if((bitseta[index] & b) > 0)
			{
				foreach(var itm in setMap[bitval])
				{
					if (itm.fivePower == v)
					{
						return itm.number;
					}
				}
			}
			return 0;
		}
		*/

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
				pa[ i ] = p5( i );
				all[ pa[ i ] ] = ( uint )i;
				setBit( pa[ i ] );
			}
			Console.WriteLine( "Table ready. Building time: " + ( watch.Elapsed.TotalMilliseconds / 1000 ) + "sec. Starting search...\n\n");

			UInt64 iterations = 1, hashHit = 1;
			mainType sum = 0, preSum = 0;

			UInt32 ind0 = 0x02;
			UInt32 ind1 = 0x02;
			UInt32 ind2 = 0x02;
			UInt32 ind3 = 0x02;
			UInt32 foundIndex = 0;

			preSum = pa[ ind1 ] + pa[ ind2 ] + pa[ ind3 ];

			while ( true )
			{
				iterations++;

				sum = preSum + pa[ ind0 ];

				if ( findBit( sum ) )
				{
					hashHit++;
					if ( all.ContainsKey( sum ) )
					{
						foundIndex = all[ sum ];
						foreach ( uint index in foundItms )
						{
							if (( foundIndex % index ) == 0 )
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
				if ( ( iterations & 0x7FFFFF ) == 0 )
				{
					clearLine();
					Console.WriteLine( ind3 + "^5 " + ind2 + "^5 " + ind1 + "^5 " + ind0 + "  t: " + ( watch.Elapsed.TotalMilliseconds / 1000 ) + " sec. itr: " + iterations + " sp: " + (iterations / ( UInt64 )watch.Elapsed.TotalMilliseconds ) + " hh: " + ( iterations / hashHit ));
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
			Console.WriteLine( "\nDone in: " + ( watch.Elapsed.TotalMilliseconds / 1000 ) + " sec. itr: " + iterations + " sp: " + ( iterations / ( UInt64 )watch.Elapsed.TotalMilliseconds ) + " hh: " + ( iterations / ( UInt64 )hashHit ) );

			//Console.WriteLine( "Press Enter to close\n" );
			//Console.Read();
		}
	}
}
