#pragma once

#include <cmath>

#define PI 3.14159265358979323846f

namespace Geom
{
	// This is mainly for 3d angles
	struct Vector2
	{
		float x, y;

		Vector2() : x(), y() {}
		Vector2(float x, float y) : x(x), y(y) {}
		//Vector2(Vector3 vec3) : x(vec3.x), y(vec3.y) {}

		Vector2  operator +		(const Vector2& rhs) { return Vector2(x + rhs.x, y + rhs.y).normalize(); }
		Vector2  operator -		(const Vector2& rhs) { return Vector2(x - rhs.x, y - rhs.y).normalize(); }
		Vector2  operator *		(const Vector2& rhs) { return Vector2(x * rhs.x, y * rhs.y).normalize(); }
		Vector2  operator /		(const Vector2& rhs) { return Vector2(x / rhs.x, y / rhs.y).normalize(); }
		Vector2  operator /		(const float	rhs) { return Vector2(x / rhs, y / rhs).normalize(); }
		Vector2& operator +=	(const Vector2& rhs) { return *this = *this + rhs; }
		Vector2& operator -=	(const Vector2& rhs) { return *this = *this - rhs; }
		Vector2& operator *=	(const Vector2& rhs) { return *this = *this * rhs; }
		Vector2& operator /=	(const Vector2& rhs) { return *this = *this / rhs; }

		// Binaries operator for 3d angle lengths
		const bool operator<	(Vector2& rhs) { return length() <  rhs.length(); }
		const bool operator<=	(Vector2& rhs) { return length() <= rhs.length(); }
		const bool operator>	(Vector2& rhs) { return length() >  rhs.length(); }
		const bool operator>=	(Vector2& rhs) { return length() >= rhs.length(); }
		const bool operator==	(Vector2& rhs) { return length() == rhs.length(); }
		const bool operator!=	(Vector2& rhs) { return length() != rhs.length(); }

		// Length but for 3d angles
		float length();
		float getDistance(const Vector2 other) { return (*this - other).length(); }
		Vector2 normalize();
	};

	// This is mainly for 3d position
	struct Vector3
	{
		float x, y, z;

		Vector3() : x(0), y(0), z(0) {}
		Vector3(float x, float y, float z) : x(x), y(y), z(z) {}
		Vector3(Vector2 vec2) : x(vec2.x), y(vec2.y), z(0) {}

		Vector3  operator +		(const Vector3& rhs) { return Vector3(x + rhs.x, y + rhs.y, z + rhs.z); }
		Vector3  operator -		(const Vector3& rhs) { return Vector3(x - rhs.x, y - rhs.y, z - rhs.z); }
		Vector3  operator *		(const Vector3& rhs) { return Vector3(x * rhs.x, y * rhs.y, z * rhs.z); }
		Vector3  operator /		(const Vector3& rhs) { return Vector3(x / rhs.x, y / rhs.y, z / rhs.z); }
		Vector3& operator +=	(const Vector3& rhs) { return *this = *this + rhs; }
		Vector3& operator -=	(const Vector3& rhs) { return *this = *this - rhs; }
		Vector3& operator *=	(const Vector3& rhs) { return *this = *this * rhs; }
		Vector3& operator /=	(const Vector3& rhs) { return *this = *this / rhs; }

		// Binaries operator for 3d distances
		const bool operator<	(Vector3& rhs) { return length() < rhs.length(); }
		const bool operator<=	(Vector3& rhs) { return length() <= rhs.length(); }
		const bool operator>	(Vector3& rhs) { return length() > rhs.length(); }
		const bool operator>=	(Vector3& rhs) { return length() >= rhs.length(); }
		const bool operator==	(Vector3& rhs) { return length() == rhs.length(); }
		const bool operator!=	(Vector3& rhs) { return length() != rhs.length(); }

		// Length for 3d distances
		float length() { return sqrt(pow(x, 2.0f) + pow(y, 2.0f) + pow(z, 2.0f)); }
		float getDistance(const Vector3 other) { return (*this - other).length(); }
		Vector2 toVector2() { return Vector2(this->x, this->y); }
	};

	Vector2 calcAngle(Vector3 posSrc, Vector3 posDst);

	struct Vector4
	{
		float x, y, z, w;

		Vector4() : x(0), y(0), z(0), w(0) {}
		Vector4(float x, float y, float z, float w) : x(x), y(y), z(z), w(w) {}
		Vector4(Vector2 vec2) : x(vec2.x), y(vec2.y), z(0), w(0) {}
		Vector4(Vector3 vec3) : x(vec3.x), y(vec3.y), z(vec3.z), w(0) {}

		Vector2 toVector2() { return Vector2(this->x, this->y); }
		Vector3 toVector3() { return Vector3(this->x, this->y, this->z); }
	};
}
