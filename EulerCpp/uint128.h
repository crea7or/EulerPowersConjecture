#pragma once

typedef unsigned __int64 uint64;
typedef unsigned __int32 uint32;

#pragma warning(disable : 4244)



// enable multiply via BMI2: https://en.wikipedia.org/wiki/Bit_Manipulation_Instruction_Sets
// https://msdn.microsoft.com/en-us/library/hh977022.aspx
// otherwise regular mul via ASM x64
//#define BMI2MUL
//#include <intrin.h>
//#pragma intrinsic(_mul128)

extern "C" inline void Multiply128(uint64, uint64);
//extern "C" inline void Adc128(uint64, uint64);

struct uint128
{
public:
	uint128(uint64 hp = 0, uint64 lp = 0)
	{
		h = hp;
		l = lp;
	}

	uint128(const uint128& v)
	{
		h = v.h;
		l = v.l;
	}

	uint128(const uint64& v)
	{
		h = 0;
		l = v;
	}

	uint128(const uint32& v)
	{
		h = 0;
		l = v;
	}

	operator uint128()
	{
		return *this;
	}

	operator uint64()
	{
		return l;
	}

	operator uint32()
	{
		return l;
	}

	// basic arithmetic operators
	uint128& operator= (const uint128& v)
	{
		h = v.h;
		l = v.l;
		return *this;
	}

	uint128& operator= (const uint64& v)
	{
		h = 0;
		l = v;
		return *this;
	}

	uint128& operator= (const uint32& v)
	{
		h = 0;
		l = v;
		return *this;
	}

	friend uint128 operator*(const uint128& s, const uint128& d);
	friend uint128 operator+(const uint128& s, const uint128& d);

	uint128& operator+= (const uint128& v)
	{
		uint64 carry = 0;
		uint64 tmp = l + v.l;
		if (tmp < l)
		{
			carry = 1;
		}
		l = tmp;
		h = h + v.h + carry;

		// Adc128((uint64)this, (uint64)&v);
		// intristic use
		//unsigned char c = 0;
		//c = _addcarry_u64(c, l, v.l, &l);
		//_addcarry_u64(c, h, v.h, &h);

		return *this;
	}

#ifdef BMI2MUL
	uint128& operator*= (const uint128& v)
	{
		// intristic use
		uint64 h1 = 0;
		uint64 h2 = 0;
		uint64 l1 = 0;
		uint64 l2 = 0;
		l1 = _mulx_u64(l, v.l, &h1);
		h1 += _mulx_u64(l, v.h, &h2);
		h1 += _mulx_u64(h, v.l, &h2);
		l = l1;
		h = h1;
		return *this;
	}
#else
	uint128& operator*= (const uint128& v)
	{
		Multiply128((uint64)this, (uint64)&v);
		return *this;
	}
#endif

	uint128& operator++ ()
	{
		uint64 carry = 0;
		uint64 tmp = l + 1;
		if (tmp < l)
		{
			carry = 1;
		}
		l = tmp;
		h = h + carry;
		return *this;
	}


	uint128 operator++ (int)
	{
		uint128 temp(h, l);
		uint64 carry = 0;
		uint64 tmp = l + 1;
		if (tmp < l)
		{
			carry = 1;
		}
		l = tmp;
		h = h + carry;
		return temp;
	}

	// conditions
	bool operator==(const uint128 &v) const
	{
		return ( h == v.h ) && ( l == v.l );
	}

	uint32 GetHash16() const
	{
		uint32 h1 = ((uint32)(h >> 32) ^ (uint32)h) ^ ((uint32)(l >> 32) ^ (uint32)l);
		return (((h1 >> 16) ^ h1) & 0xFFFF);
	}

	uint32 GetHash32() const
	{
		return ((uint32)(h >> 32) ^ (uint32)h) ^ ((uint32)(l >> 32) ^ (uint32)l);
	}

	uint64 GetHash64() const
	{
		return h ^ l;
	}

	uint64 GetH()
	{
		return h;
	}

	uint64 GetL()
	{
		return l;
	}

	void Set(uint64 hv, uint64 lv)
	{
		h = hv;
		l = lv;
	}

	void SetH(uint64 hv)
	{
		h = hv;
	}

	void SetL(uint64 lv)
	{
		l = lv;
	}

#ifdef BMI2MUL
	friend uint128 operator*(const uint128& s, const uint128& d)
	{
		// intristic use
		uint64 h = 0;
		uint64 l = 0;
		uint64 h2 = 0;
		uint64 l2 = 0;
		l = _mulx_u64(d.l, s.l, &h);
		h += _mulx_u64(d.l, s.h, &h2);
		h += _mulx_u64(d.h, s.l, &h2);
		return uint128( h, l);
	}
#else

	friend uint128 operator*(const uint128& s, const uint128& d)
	{
		uint128 temp(s);
		Multiply128((uint64)&temp, (uint64)&d);
		return uint128( temp );
	}
#endif

	friend uint128 operator+(const uint128& s, const uint128& d)
	{
		uint128 temp(s);
		temp += d;
		return uint128(temp);
	}


#ifdef BMI2MUL
	uint128& pow( uint32 pw )
	{
		if (pw < 2)
		{
			if (pw == 0)
			{
				h = 0;
				l = 1;
			}
			return *this;
		}

		// intristic use
		uint64 hp = h;
		uint64 lp = l;

		uint64 h1 = 0;
		uint64 l1 = 0;
		uint64 h2 = 0;
		uint64 l2 = 0;

		while (--pw)
		{
			l = _mulx_u64(l, lp, &h1);
			h1 += _mulx_u64(l, hp, &h2);
			h1 += _mulx_u64(h, lp, &h2);
			h = h1;
		}
		return *this;
	}
#else

	uint128& pow( uint32 pw )
	{
		// not implt
		// uint128 temp(s);
		// Multiply128((uint64)&temp, (uint64)&d);
		return *this;
	}
#endif

private:

	uint64 h;
	uint64 l;

};


