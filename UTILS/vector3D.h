#pragma once

#include <math.h>

#define M_PI 3.14159265358979323846
#define deg(a) a * 57.295779513082
#define CHECK_VALID( _v ) 0
#define Assert( _exp ) ((void)0)

class Vector
{
public:
	float x, y, z;
	//Vector(void);
	//Vector(float X, float Y, float Z);

	__forceinline float LengthSqr(void) const
	{
		CHECK_VALID(*this);
		return (this->x*this->x + this->y*this->y + this->z*this->z);
	}
	bool IsZero(float tolerance = 0.01f) const
	{
		return (x > -tolerance && x < tolerance &&
			y > -tolerance && y < tolerance &&
			z > -tolerance && z < tolerance);
	}
	inline float sqrt2(float sqr)
	{
		float root = 0;

		__asm
		{
			sqrtss xmm0, sqr
			movss root, xmm0
		}

		return root;
	}
	Vector VectorCrossProduct(const Vector &a, const Vector &b) const
	{
		return Vector(a.y*b.z - a.z*b.y, a.z*b.x - a.x*b.z, a.x*b.y - a.y*b.x);
	}

	//===============================================
	inline void Vector::Init(float ix, float iy, float iz)
	{
		x = ix; y = iy; z = iz;
		CHECK_VALID(*this);
	}
	//===============================================
	inline Vector::Vector(float X, float Y, float Z)
	{
		x = X; y = Y; z = Z;
		CHECK_VALID(*this);
	}
	//===============================================
	inline Vector::Vector(void) {}
	//===============================================
	inline void Vector::Zero()
	{
		x = y = z = 0.0f;
	}
	//===============================================
	inline void Vector::Rotate2D(const float &f)
	{
		float _x, _y;

		float s, c;

		float r = f * M_PI / 180.0;
		s = sin(r);
		c = cos(r);

		_x = x;
		_y = y;

		x = (_x * c) - (_y * s);
		y = (_x * s) + (_y * c);
	}
	//===============================================
	inline void VectorClear(Vector& a)
	{
		a.x = a.y = a.z = 0.0f;
	}
	//===============================================
	inline Vector& Vector::operator=(const Vector &vOther)
	{
		CHECK_VALID(vOther);
		x = vOther.x; y = vOther.y; z = vOther.z;
		return *this;
	}
	//===============================================
	inline float& Vector::operator[](int i)
	{
		Assert((i >= 0) && (i < 3));
		return ((float*)this)[i];
	}
	//===============================================
	inline float Vector::operator[](int i) const
	{
		Assert((i >= 0) && (i < 3));
		return ((float*)this)[i];
	}
	//===============================================
	inline bool Vector::operator==(const Vector& src) const
	{
		CHECK_VALID(src);
		CHECK_VALID(*this);
		return (src.x == x) && (src.y == y) && (src.z == z);
	}
	//===============================================
	inline bool Vector::operator!=(const Vector& src) const
	{
		CHECK_VALID(src);
		CHECK_VALID(*this);
		return (src.x != x) || (src.y != y) || (src.z != z);
	}
	//===============================================
	__forceinline void VectorCopy(const Vector& src, Vector& dst)
	{
		CHECK_VALID(src);
		dst.x = src.x;
		dst.y = src.y;
		dst.z = src.z;
	}
	//===============================================
	__forceinline  Vector& Vector::operator+=(const Vector& v)
	{
		CHECK_VALID(*this);
		CHECK_VALID(v);
		x += v.x; y += v.y; z += v.z;
		return *this;
	}
	//===============================================
	__forceinline  Vector& Vector::operator-=(const Vector& v)
	{
		CHECK_VALID(*this);
		CHECK_VALID(v);
		x -= v.x; y -= v.y; z -= v.z;
		return *this;
	}
	//===============================================
	__forceinline  Vector& Vector::operator*=(float fl)
	{
		x *= fl;
		y *= fl;
		z *= fl;
		CHECK_VALID(*this);
		return *this;
	}
	//===============================================
	__forceinline  Vector& Vector::operator*=(const Vector& v)
	{
		CHECK_VALID(v);
		x *= v.x;
		y *= v.y;
		z *= v.z;
		CHECK_VALID(*this);
		return *this;
	}
	//===============================================
	__forceinline Vector&	Vector::operator+=(float fl)
	{
		x += fl;
		y += fl;
		z += fl;
		CHECK_VALID(*this);
		return *this;
	}
	//===============================================
	__forceinline Vector&	Vector::operator-=(float fl)
	{
		x -= fl;
		y -= fl;
		z -= fl;
		CHECK_VALID(*this);
		return *this;
	}
	//===============================================
	__forceinline  Vector& Vector::operator/=(float fl)
	{
		Assert(fl != 0.0f);
		float oofl = 1.0f / fl;
		x *= oofl;
		y *= oofl;
		z *= oofl;
		CHECK_VALID(*this);
		return *this;
	}
	//===============================================
	__forceinline  Vector& Vector::operator/=(const Vector& v)
	{
		CHECK_VALID(v);
		Assert(v.x != 0.0f && v.y != 0.0f && v.z != 0.0f);
		x /= v.x;
		y /= v.y;
		z /= v.z;
		CHECK_VALID(*this);
		return *this;
	}
	//===============================================
	inline float Vector::Length(void) const
	{
		CHECK_VALID(*this);

		float root = 0.0f;

		float sqsr = x * x + y * y + z * z;

		__asm sqrtss xmm0, sqsr
		__asm movss root, xmm0

		return root;
	}
	//===============================================
	inline float Vector::Length2D(void) const
	{
		CHECK_VALID(*this);

		float root = 0.0f;

		float sqst = x * x + y * y;

		__asm
		{
			sqrtss xmm0, sqst
			movss root, xmm0
		}

		return root;
	}
	//===============================================
	inline float Vector::Length2DSqr(void) const
	{
		return (x*x + y * y);
	}
	//===============================================
	inline Vector Vector::Angle(Vector* up)
	{
		if (!x && !y)
			return Vector(0, 0, 0);

		float roll = 0;

		if (up)
		{
			Vector left = (*up).Cross(*this);
			roll = atan2f(left.z, (left.y * x) - (left.x * y)) * 180.0f / M_PI;
		}

		return Vector(atan2f(-z, sqrt2(x*x + y * y)) * 180.0f / M_PI, atan2f(y, x) * 180.0f / M_PI, roll);
	}
	inline Vector Vector::Angle2(Vector* up = 0)
	{
		if (!x && !y)
			return Vector(0, 0, 0);

		float roll = 0;

		if (up)
		{
			Vector left = (*up).Cross(*this);

			roll = deg(atan2f(left.z, (left.y * x) - (left.x * y)));
		}

		return Vector(deg(atan2f(-z, sqrtf(x*x + y * y))), deg(atan2f(y, x)), roll);
	}
	//===============================================
	inline Vector CrossProduct(const Vector& a, const Vector& b)
	{
		return Vector(a.y*b.z - a.z*b.y, a.z*b.x - a.x*b.z, a.x*b.y - a.y*b.x);
	}
	//===============================================
	float Vector::DistToSqr(const Vector &vOther) const
	{
		Vector delta;

		delta.x = x - vOther.x;
		delta.y = y - vOther.y;
		delta.z = z - vOther.z;

		return delta.LengthSqr();
	}
	float DistTo(const Vector &vOther) const
	{
		Vector delta;

		delta.x = x - vOther.x;
		delta.y = y - vOther.y;
		delta.z = z - vOther.z;

		return delta.Length();
	}
	float Vector::Dist(const Vector &vOther) const
	{
		Vector delta;

		delta.x = x - vOther.x;
		delta.y = y - vOther.y;
		delta.z = z - vOther.z;

		return delta.Length();
	}

	inline Vector Vector::Cross(const Vector& vOther) const
	{
		Vector res;
		return VectorCrossProduct(*this, vOther);
	}

	inline Vector Vector::Normalize()
	{
		Vector vector;
		float length = this->Length();

		if (length != 0)
		{
			vector.x = x / length;
			vector.y = y / length;
			vector.z = z / length;
		}
		else
			vector.x = vector.y = 0.0f; vector.z = 1.0f;

		return vector;
	}


	//===============================================
	inline float Vector::NormalizeInPlace()
	{
		Vector& v = *this;

		float iradius = 1.f / (this->Length() + 1.192092896e-07F); //FLT_EPSILON

		v.x *= iradius;
		v.y *= iradius;
		v.z *= iradius;

		return v.Length();
	}
	//===============================================
	inline float VectorNormalize(Vector& v)
	{
		Assert(v.IsValid());
		float l = v.Length();
		if (l != 0.0f)
		{
			v /= l;
		}
		else
		{
			v.x = v.y = 0.0f; v.z = 1.0f;
		}
		return l;
	}
	//===============================================
	FORCEINLINE float VectorNormalize(float * v)
	{
		return VectorNormalize(*(reinterpret_cast<Vector *>(v)));
	}

	//===============================================
	/*inline Vector Vector::Normalized() const
	{
		Vector norm = *this;
		VectorNormalize(norm);
		return norm;
	}*/
	Vector Normalized() const
	{
		Vector res = *this;
		float l = res.Length();
		if (l != 0.0f) {
			res /= l;
		}
		else {
			res.x = res.y = res.z = 0.0f;
		}
		return res;
	}
	//===============================================
	inline Vector Vector::operator+(const Vector& v) const
	{
		Vector res;
		res.x = x + v.x;
		res.y = y + v.y;
		res.z = z + v.z;
		return res;
	}

	//===============================================
	inline Vector Vector::operator-(const Vector& v) const
	{
		Vector res;
		res.x = x - v.x;
		res.y = y - v.y;
		res.z = z - v.z;
		return res;
	}
	//===============================================
	inline Vector Vector::operator*(float fl) const
	{
		Vector res;
		res.x = x * fl;
		res.y = y * fl;
		res.z = z * fl;
		return res;
	}
	//===============================================
	inline Vector Vector::operator*(const Vector& v) const
	{
		Vector res;
		res.x = x * v.x;
		res.y = y * v.y;
		res.z = z * v.z;
		return res;
	}
	//===============================================
	inline Vector Vector::operator/(float fl) const
	{
		Vector res;
		res.x = x / fl;
		res.y = y / fl;
		res.z = z / fl;
		return res;
	}
	//===============================================
	inline Vector Vector::operator/(const Vector& v) const
	{
		Vector res;
		res.x = x / v.x;
		res.y = y / v.y;
		res.z = z / v.z;
		return res;
	}
	inline float Vector::Dot(const Vector& vOther) const
	{
		const Vector& a = *this;

		return(a.x*vOther.x + a.y*vOther.y + a.z*vOther.z);
	}

	inline float Vector::Dot(const float* fOther) const
	{
		const Vector& a = *this;

		return(a.x*fOther[0] + a.y*fOther[1] + a.z*fOther[2]);
	}

	inline float* Vector::Base()
	{
		return (float*)this;
	}

	inline float const* Vector::Base() const
	{
		return (float const*)this;
	}

};

	/*void Init(float ix = 0.0f, float iy = 0.0f, float iz = 0.0f);
	bool IsValid() const;
	float operator[](int i) const;
	float& operator[](int i);
	inline void Zero();
	inline void Rotate2D(const float &f);
	bool operator==(const Vector& v) const;
	bool operator!=(const Vector& v) const;
	__forceinline Vector&	operator+=(const Vector &v);
	__forceinline Vector&	operator-=(const Vector &v);
	__forceinline Vector&	operator*=(const Vector &v);
	__forceinline Vector&	operator*=(float s);
	__forceinline Vector&	operator/=(const Vector &v);
	__forceinline Vector&	operator/=(float s);
	__forceinline Vector&	operator+=(float fl);
	__forceinline Vector&	operator-=(float fl);
	inline float	Length() const;
	__forceinline float LengthSqr(void) const
	{
		CHECK_VALID(*this);
		return (this->x*this->x + this->y*this->y + this->z*this->z);
	}
	bool IsZero(float tolerance = 0.01f) const
	{
		return (x > -tolerance && x < tolerance &&
			y > -tolerance && y < tolerance &&
			z > -tolerance && z < tolerance);
	}
	float	NormalizeInPlace();
	Vector	Normalize();
	inline Vector Normalized() const;
	__forceinline float	DistToSqr(const Vector &vOther) const;
	__forceinline float	Dist(const Vector &vOther) const;
	Vector	Cross(const Vector & vOther) const;
	float	Dot(const Vector& vOther) const;
	float	Dot(const float* fOther) const;
	float	Length2D(void) const;
	float	Length2DSqr(void) const;
	Vector	Angle(Vector* up = 0);
	Vector& operator=(const Vector &vOther);
	Vector	operator-(const Vector& v) const;
	Vector	operator+(const Vector& v) const;
	Vector	operator*(const Vector& v) const;
	Vector	operator/(const Vector& v) const;
	Vector	operator*(float fl) const;
	Vector	operator/(float fl) const;
	// Base address...
	float* Base();
	float const* Base() const;*/
//};

	/*float x, y, z;
	inline void Init(float ix, float iy, float iz)
	{
		x = ix; y = iy; z = iz;
	}
	Vector VectorCrossProduct(const Vector &a, const Vector &b) const
	{
		return Vector(a.y*b.z - a.z*b.y, a.z*b.x - a.x*b.z, a.x*b.y - a.y*b.x);
	}
	Vector() { x = 0; y = 0; z = 0; };
	Vector(float X, float Y, float Z) { x = X; y = Y; z = Z; };

	float operator[](int i) const { if (i == 0) return x; if (i == 1) return y; return z; };
	float& operator[](int i) { if (i == 0) return x; if (i == 1) return y; return z; };

	bool operator==(const Vector& v) { return true; }
	bool operator!=(const Vector& v) { return true; }

	inline Vector operator-(const Vector& v) { return Vector(x - v.x, y - v.y, z - v.z); }
	inline Vector operator+(const Vector& v) { return Vector(x + v.x, y + v.y, z + v.z); }
	inline Vector operator*(const Vector& v) { return Vector(x * v.x, y * v.y, z * v.z); }
	inline Vector operator/(const Vector& v) { return Vector(x / v.x, y / v.y, z / v.z); }
	inline Vector operator*(const int n) { return Vector(x*n, y*n, z*n); }
	inline Vector operator*(const float n) { return Vector(x*n, y*n, z*n); }
	inline Vector operator/(const int n) { return Vector(x / n, y / n, z / n); }
	inline Vector operator/(const float n) { return Vector(x / n, y / n, z / n); }


	inline Vector operator-() { return Vector(-x, -y, -z); }

	inline Vector& operator+=(const Vector &v) { x += v.x; y += v.y; z += v.z; return *this; }
	inline Vector& operator-=(const Vector &v) { x -= v.x; y -= v.y; z -= v.z; return *this; }
	inline Vector& operator*=(const Vector &v) { x *= v.x; y *= v.y; z *= v.z; return *this; }
	inline Vector& operator/=(const Vector &v) { x /= v.x; y /= v.y; z /= v.z; return *this; }

	float LengthSqr(void) { return (x*x + y*y + z*z); }
	float Length(void) { return sqrt(x*x + y*y + z*z); }

	inline float Length2D() const
	{
		return sqrt((x * x) + (y * y));
	}

	//T must be between 0 and 1
	Vector lerp(Vector target, float t)
	{
		return *this * (1 - t) + target * t;
	}

	void lerpme(Vector target, float t)
	{
		*this = *this * (1 - t) + target * t;
	}
	inline float Dot(const Vector &e) const
	{
		return (x * e.x) + (y * e.y) + (z * e.z);
	}
	float DistTo(const Vector &vOther) const
	{
		Vector delta;

		delta.x = x - vOther.x;
		delta.y = y - vOther.y;
		delta.z = z - vOther.z;

		return delta.Length();
	}
	float DistToSqr(const Vector &vOther) const
	{
		Vector delta;

		delta.x = x - vOther.x;
		delta.y = y - vOther.y;
		delta.z = z - vOther.z;

		return delta.LengthSqr();
	}
	Vector& operator/=(float fl)
	{
		x /= fl;
		y /= fl;
		z /= fl;
		return *this;
	}
	/*Vector Cross(const Vector& vOther) const
	{
		Vector res;
		VectorCrossProduct(*this, vOther, res);
		return res;
	}*/
	/*void crossproduct(Vector v1, Vector v2, Vector cross_p) const //ijk = xyz
	{
		cross_p.x = (v1.y*v2.z) - (v1.z*v2.y); //i
		cross_p.y = -((v1.x*v2.z) - (v1.z*v2.x)); //j
		cross_p.z = (v1.x*v2.y) - (v1.y*v2.x); //k
	}
	Vector Cross(const Vector& vOther) const
	{
		Vector res;
		crossproduct(*this, vOther, res);
		return res;
	}
	float NormalizeInPlace()
	{
		Vector& v = *this;

		float iradius = 1.f / (this->LengthSqr() + 1.192092896e-07F); //FLT_EPSILON

		v.x *= iradius;
		v.y *= iradius;
		v.z *= iradius;
		return iradius;
	}
	Vector Normalized() const
	{
		Vector res = *this;
		float l = res.Length();
		if (l != 0.0f) {
			res /= l;
		}
		else {
			res.x = res.y = res.z = 0.0f;
		}
		return res;
	}*/
//};
