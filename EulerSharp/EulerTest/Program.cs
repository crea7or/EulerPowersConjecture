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
// select value type
// Uncomment to use UInt64
//#define UINT64USE
// Uncomment to use UInt128
#define UINT128USE
// Uncomment to use decimal - slow and only for test reasons
//#define DECIUSE

// select search mode (only one!)
// search with range start index selector and plain traversal
#define SEARCHRANGE
// search with Dictionary (like unsorted_map from c++)
//#define SEARCHMAP
// prefilter SEARCHMAP with bitmap, works only with SEARCHMAP!
//#define BITMAPFILTER
// search with selfmade hash map
//#define SEARCHBITMAP

#if (SEARCHMAP)
#elif (BITMAPFILTER)
#error BITMAPFILTER can be used only with SEARCHHASH
#endif

#if (BITMAPFILTER)
#if (SEARCHMAP)
#else
#error BITMAPFILTER can be used only with SEARCHHASH
#endif
#endif

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
		const int N = 86000; // max number is ~7131 for 64 bit values ( 18 446 744 073 709 551 616 )
		// N=150 27 ^ 5 + 84 ^ 5 + 110 ^ 5 + 133 ^ 5 = 144 ^ 5 (Lander & Parkin, 1966).
		// const int N = 86000 (UInt32128 or decimal only)
		// N=86000 for 55^5 + 3183^5 + 28969^5 + 85282^5 = 85359^5 (Frye, 2004). ( 85359^5 is 4 531 548 087 264 753 520 490 799 )
		// Average speed with decimal type about  ~39 000 000 tests per second.
		// Average speed with Uint128 type about ~140 000 000 tests per second.
		// Average speed with Uint64  type about ~280 000 000 tests per second.

		static List<UInt32> foundItms = new List<UInt32>();

		const UInt32 SEARCHBITSETSIZE = 0x20000; // for SEARCHBITMAP and BITMAPFILTER

#if (SEARCHBITMAP)
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

		static List< CompValue >[] setMap = new List< CompValue >[ SEARCHBITSETSIZE ];
		static UInt32[] bitseta = new UInt32[ SEARCHBITSETSIZE / 4 ];

		static void setBit( mainType fivePower, UInt32 number )
		{
			UInt32 bitval = (( UInt32 )fivePower.GetHashCode()) & 0x0000FFFF;
			UInt32 b = ( UInt32 )1 << ( int )( bitval & ( UInt32 )0x1F );

			if ( setMap[ bitval ] == null )
			{
				setMap[ bitval ] = new List<CompValue>();
			}
			setMap[ bitval ].Add( new CompValue( fivePower, number ));

			UInt32 index = bitval >> 5;
			bitseta[index] = bitseta[index] | b;
		}

		static UInt32 findBit( mainType v )
		{
			UInt32 bitval = ( ( UInt32 )v.GetHashCode() ) & 0x0000FFFF;
			UInt32 b = ( UInt32 )1 << (int)( bitval & ( UInt32 )0x1F );
			UInt32 index = bitval >> 5;

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
#endif
		static mainType p5( mainType x )
		{
			return x * x * x * x * x;
		}
		static mainType[] powers = new mainType[ N ];

#if (SEARCHMAP)
		static Dictionary< mainType, UInt32> all = new Dictionary< mainType, UInt32>();
#endif

#if (BITMAPFILTER)
		static UInt32[] bitseta = new UInt32[ SEARCHBITSETSIZE / 4 ];

		[MethodImpl( MethodImplOptions.AggressiveInlining )]
		static bool findBit( mainType v )
		{
			UInt32 bitval = ( ( UInt32 )v.GetHashCode() ) & ( SEARCHBITSETSIZE - 1 );
			UInt32 b = ( UInt32 )1 << (int)( bitval & ( UInt32 )0x1F );
			UInt32 index = bitval >> 5;
			return ((bitseta[index] & b) > 0);
		}

		[MethodImpl( MethodImplOptions.AggressiveInlining )]
		static void setBit( mainType v )
		{
			UInt32 bitval = ( ( UInt32 )v.GetHashCode() ) & ( SEARCHBITSETSIZE - 1 );
			UInt32 b = ( UInt32 )1 << ( int )( bitval & ( UInt32 )0x1F );
			UInt32 index = bitval >> 5;
			bitseta[index] = bitseta[index] | b;
		}
#endif

#if (SEARCHRANGE)

		static UInt32 lastRangeIndex = 0x02;

		[MethodImpl( MethodImplOptions.AggressiveInlining )]
		static UInt32 findInRange( mainType fivePower, UInt32 startIndex )
		{
			while (startIndex < N)
			{
				lastRangeIndex = startIndex;
				if ( powers[ startIndex ] > fivePower )
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

			for ( UInt32 i = 0; i < N; ++i )
			{
				powers[ i ] = p5( i );
#if (SEARCHMAP)
				all[ powers[ i ] ] = ( UInt32 )i;
#endif
#if (BITMAPFILTER)
				setBit( powers[ i ] );
#endif
#if (SEARCHBITMAP)
				setBit( powers[ i ], i );
#endif
			}

			Console.WriteLine( "Table ready. Building time: " + ( watch.Elapsed.TotalMilliseconds / 1000 ) + "sec. Starting search...\n\n");

			UInt64 iterations = 1, hashHit = 1;
			mainType sum = 0, preSum = 0;

			UInt32 ind0 = 0x02;
			UInt32 ind1 = 0x02;
			UInt32 ind2 = 0x02;
			UInt32 ind3 = 0x02;
			UInt32 foundIndex = 0;

#if (SEARCHRANGE)
			lastRangeIndex = 0x02;
#endif
			preSum = powers[ ind1 ] + powers[ ind2 ] + powers[ ind3 ];

			while ( true )
			{
				iterations++;

				sum = preSum + powers[ ind0 ];

#if (SEARCHRANGE)
				foundIndex = findInRange( sum, lastRangeIndex );
				if ( foundIndex > 0 )
#endif
#if (SEARCHBITMAP)
				foundIndex = findBit( sum );
				if ( foundIndex > 0 )
#endif
#if (BITMAPFILTER)
				if ( findBit( sum ) )
#endif
				{
					hashHit++;
#if (SEARCHMAP)
					if ( all.ContainsKey( sum ) )
					{
						foundIndex = all[ sum ];
#endif
						foreach ( UInt32 index in foundItms )
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

							Console.WriteLine( "Found: " + ind3 + " " + ind2 + " " + ind1 + " " + ind0 + " = " + foundIndex + " t: " + String.Format( "{0:0.00}", ( watch.Elapsed.TotalMilliseconds / 1000 )) + " sec. itr: " + iterations + "\n\n" );
							foundItms.Add( foundIndex );
						}
						else
						{
							// same value found
							Console.WriteLine( "Duplicate" );
						}
#if (SEARCHMAP)
					}
#endif
				}

				// displaying some progress
				if ( ( iterations & 0x7FFFFF ) == 0 )
				{
					clearLine();
					Console.WriteLine( ind3 + "^5 " + ind2 + "^5 " + ind1 + "^5 " + ind0 + "  t: " + String.Format( "{0:0.00}", ( watch.Elapsed.TotalMilliseconds / 1000 ) ) + " itr: " + iterations + " spd: " + ( iterations / ( UInt64 )watch.Elapsed.TotalMilliseconds ));
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

				// refresh without ind0
				preSum = powers[ ind1 ] + powers[ ind2 ] + powers[ ind3 ];

#if (SEARCHRANGE)
				// fix lastRangeIndex
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
			}

			watch.Stop();
			clearLine();
			Console.WriteLine( "\nDone in: " + String.Format( "{0:0.00}", ( watch.Elapsed.TotalMilliseconds / 1000 ) ) + " sec. itr: " + iterations + " spd: " + ( iterations / ( UInt64 )watch.Elapsed.TotalMilliseconds ) + "\nhash hits: " + ( iterations / ( UInt64 )hashHit ) );

			//Console.WriteLine( "Press Enter to close\n" );
			//Console.Read();
		}
	}
}
