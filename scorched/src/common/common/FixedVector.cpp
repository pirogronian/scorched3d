////////////////////////////////////////////////////////////////////////////////
//    Scorched3D (c) 2000-2009
//
//    This file is part of Scorched3D.
//
//    Scorched3D is free software; you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation; either version 2 of the License, or
//    (at your option) any later version.
//
//    Scorched3D is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public License
//    along with Scorched3D; if not, write to the Free Software
//    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
////////////////////////////////////////////////////////////////////////////////

#include <common/FixedVector.h>

static FixedVector nullVector;

const char *FixedVector::asQuickString()
{
	static int i=0;
	static char buffer[5][75];

	char *result = buffer[++i % 5];
	snprintf(result, 75, "%s,%s,%s", 
		V[0].asQuickString(), V[1].asQuickString(), V[2].asQuickString());
	return result;
}

FixedVector &FixedVector::getNullVector()
{
	nullVector.zero();
	return nullVector;
}

#define sqrt_step(shift) \
    if((0x40000000l >> shift) + root <= value)          \
    {                                                   \
        value -= (0x40000000l >> shift) + root;         \
        root = (root >> 1) | (0x40000000l >> shift);    \
    }                                                   \
    else                                                \
    {                                                   \
        root = root >> 1;                               \
    }

static Sint64 iSqrtLL(Sint64 value)
{
    Sint64 root = 0;

    sqrt_step( 0);
    sqrt_step( 2);
    sqrt_step( 4);
    sqrt_step( 6);
    sqrt_step( 8);
    sqrt_step(10);
    sqrt_step(12);
    sqrt_step(14);
    sqrt_step(16);
    sqrt_step(18);
    sqrt_step(20);
    sqrt_step(22);
    sqrt_step(24);
    sqrt_step(26);
    sqrt_step(28);
    sqrt_step(30);

    // round to the nearest integer, cuts max error in half

    if(root < value)
    {
        ++root;
    }

    return root;
}

fixed FixedVector::Magnitude()
{
	Sint64 a = V[0].getInternalData()*V[0].getInternalData()/FIXED_RESOLUTION;
	Sint64 b = V[1].getInternalData()*V[1].getInternalData()/FIXED_RESOLUTION;
	Sint64 c = V[2].getInternalData()*V[2].getInternalData()/FIXED_RESOLUTION;
	Sint64 res = a + b + c;
	Sint64 result = iSqrtLL(res);
	result *= 100;

	return fixed(true, result);
}
