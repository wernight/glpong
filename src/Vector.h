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

#pragma once

#include "StdAfx.h"

///////////////////////////////////////////////////////////////////////////
// VECTOR2D
///////////////////////////////////////////////////////////////////////////

class Vector2D
{
public:
  float x, y;

  inline Vector2D()
  {  }
  inline Vector2D(const float X, const float Y)
  { x = X; y = Y; }
  inline Vector2D(const float v[2])
  { x = v[0]; y = v[1]; }

  inline operator const float*() const
  { return &x; }

  inline Vector2D operator+(const Vector2D &v) const
  { return Vector2D(x+v.x, y+v.y); }

  inline Vector2D operator-(const Vector2D &v) const
  { return Vector2D(x-v.x, y-v.y); }

  inline Vector2D& operator+=(const Vector2D &v)
  { x += v.x; y += v.y; return *this; }

  inline Vector2D& operator-=(const Vector2D &v)
  { x -= v.x; y -= v.y; return *this; }

  inline Vector2D& operator-()
  { x = -x; y = -y; return *this; }

  inline Vector2D operator*(const float f) const
  { return Vector2D(x*f, y*f); }

  inline Vector2D operator/(float f) const
  { f = 1.0f/f; return Vector2D(x*f, y*f); }

  inline Vector2D& operator*=(const float f)
  { x *= f; y *= f; return *this; }

  inline Vector2D& operator/=(float f)
  { f = 1.0f/f; x *= f; y *= f; return *this; }

  inline float Dot(const Vector2D &v) const
  { return x*v.x + y*v.y; }

  inline float DistTo(const Vector2D &v) const
  { return sqrt((x-v.x)*(x-v.x) + (y-v.y)*(y-v.y)); }

  inline float Norm() const
  { return sqrt(x*x + y*y); }
};


///////////////////////////////////////////////////////////////////////////
// VECTOR3D
///////////////////////////////////////////////////////////////////////////

class Vector3D
{
public:
  float x, y, z;

  inline Vector3D()
  {  }
  inline Vector3D(const float X, const float Y, const float Z)
  { x = X; y = Y; z = Z; }
  inline Vector3D(const float v[3])
  { x = v[0]; y = v[1]; z = v[2]; }

  inline operator const float*() const
  { return &x; }

  inline Vector3D operator+(const Vector3D &v) const
  { return Vector3D(x+v.x, y+v.y, z+v.z); }

  inline Vector3D operator-(const Vector3D &v) const
  { return Vector3D(x-v.x, y-v.y, z-v.z); }

  inline Vector3D& operator+=(const Vector3D &v)
  { x += v.x; y += v.y; z += v.z; return *this; }

  inline Vector3D& operator-=(const Vector3D &v)
  { x -= v.x; y -= v.y; z-= v.z; return *this; }

  inline Vector3D& operator-()
  { x = -x; y = -y; z = -z; return *this; }

  inline Vector3D operator*(const float f) const
  { return Vector3D(x*f, y*f, z*f); }

  inline Vector3D operator/(float f) const
  { f = 1.0f/f; return Vector3D(x*f, y*f, z*f); }

  inline Vector3D& operator*=(const float f)
  { x *= f; y *= f; z *= f; return *this; }

  inline Vector3D& operator/=(float f)
  { f = 1.0f/f; x *= f; y *= f; z *= f; return *this; }

  inline float Dot(const Vector3D &v) const
  { return x*v.x + y*v.y + z*v.z; }

  inline float DistTo(const Vector3D &v) const
  { return sqrt((x-v.x)*(x-v.x) + (y-v.y)*(y-v.y) + (z-v.z)*(z-v.z)); }

  inline float Norm() const
  { return sqrt(x*x + y*y + z*z); }
};

