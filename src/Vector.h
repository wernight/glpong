#ifndef INC_VECTOR_H_
#define INC_VECTOR_H_
/**
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

/**
 * Copyright (c) 2003 Werner BEROUX
 * Mail: werner@beroux.com
 * Web : www.beroux.com
 *
 * \date 2004.11.07
 */

#include "StdAfx.h"

///////////////////////////////////////////////////////////////////////////
// VECTOR2D
///////////////////////////////////////////////////////////////////////////

class CVector2D
{
public:
	float x, y;

    inline CVector2D()
	{  }
    inline CVector2D(const float X, const float Y)
    { x = X; y = Y; }
    inline CVector2D(const float v[2])
    { x = v[0]; y = v[1]; }

	inline operator const float*() const
	{ return &x; }

    inline CVector2D operator+(const CVector2D &v) const
    { return CVector2D(x+v.x, y+v.y); }                

    inline CVector2D operator-(const CVector2D &v) const
    { return CVector2D(x-v.x, y-v.y); }                

	inline CVector2D& operator+=(const CVector2D &v)
	{ x += v.x; y += v.y; return *this; }

	inline CVector2D& operator-=(const CVector2D &v)
	{ x -= v.x; y -= v.y; return *this; }

	inline CVector2D& operator-()
	{ x = -x; y = -y; return *this; }
	
    inline CVector2D operator*(const float f) const
    { return CVector2D(x*f, y*f); }                

	inline CVector2D operator/(float f) const
	{ f = 1.0f/f; return CVector2D(x*f, y*f); }

	inline CVector2D& operator*=(const float f)
	{ x *= f; y *= f; return *this; }

	inline CVector2D& operator/=(float f)
	{ f = 1.0f/f; x *= f; y *= f; return *this; }

	inline float Dot(const CVector2D &v) const
    { return x*v.x + y*v.y; }

	inline float DistTo(const CVector2D &v) const
	{ return sqrt((x-v.x)*(x-v.x) + (y-v.y)*(y-v.y)); }

	inline float Norm() const
	{ return sqrt(x*x + y*y); }
};


///////////////////////////////////////////////////////////////////////////
// VECTOR3D
///////////////////////////////////////////////////////////////////////////

class CVector3D
{
public:
	float x, y, z;

    inline CVector3D()
	{  }
    inline CVector3D(const float X, const float Y, const float Z)
    { x = X; y = Y; z = Z; }
    inline CVector3D(const float v[3])
    { x = v[0]; y = v[1]; z = v[2]; }

	inline operator const float*() const
	{ return &x; }

	inline CVector3D operator+(const CVector3D &v) const
    { return CVector3D(x+v.x, y+v.y, z+v.z); }                

    inline CVector3D operator-(const CVector3D &v) const
    { return CVector3D(x-v.x, y-v.y, z-v.z); }                

	inline CVector3D& operator+=(const CVector3D &v)
	{ x += v.x; y += v.y; z += v.z; return *this; }

	inline CVector3D& operator-=(const CVector3D &v)
	{ x -= v.x; y -= v.y; z-= v.z; return *this; }

	inline CVector3D& operator-()
	{ x = -x; y = -y; z = -z; return *this; }
	
    inline CVector3D operator*(const float f) const
    { return CVector3D(x*f, y*f, z*f); }                

	inline CVector3D operator/(float f) const
	{ f = 1.0f/f; return CVector3D(x*f, y*f, z*f); }

	inline CVector3D& operator*=(const float f)
	{ x *= f; y *= f; z *= f; return *this; }

	inline CVector3D& operator/=(float f)
	{ f = 1.0f/f; x *= f; y *= f; z *= f; return *this; }

	inline float Dot(const CVector3D &v) const
    { return x*v.x + y*v.y + z*v.z; }

	inline float DistTo(const CVector3D &v) const
	{ return sqrt((x-v.x)*(x-v.x) + (y-v.y)*(y-v.y) + (z-v.z)*(z-v.z)); }

	inline float Norm() const
	{ return sqrt(x*x + y*y + z*z); }
};

#endif

