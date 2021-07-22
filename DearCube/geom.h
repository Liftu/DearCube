#pragma once


struct Vector2
{
	float x, y;

	Vector2 getDistance(Vector2 other)
	{
		return Vector2({ abs(this->x - other.x), abs(this->y - other.y) });
	}
};

struct Vector3
{
	float x, y, z;

	Vector3(float x, float y, float z) : x(x), y(y), z(z) {}

	//bool operator ==	(const Vector3 other) { return (this->x == other.x && this->y == other.y && this->z == other.z); }
	//bool operator !=	(const Vector3 other) { return !(this->x == other.x && this->y == other.y && this->z == other.z); }
	//bool operator <		(const Vector3 other) { return (this->x + this->y + this->z) < (other.x + other.y + other.y); }
	//bool operator <=	(const Vector3 other) { return (this->x + this->y + this->z) <= (other.x + other.y + other.y); }
	//bool operator >		(const Vector3 other) { return (this->x + this->y + this->z) > (other.x + other.y + other.y); }
	//bool operator >=	(const Vector3 other) { return (this->x + this->y + this->z) >= (other.x + other.y + other.y); }
	Vector3  operator +		(const Vector3& rhs) { return Vector3(this->x + rhs.x, this->y + rhs.y, this->z - rhs.z); }
	Vector3  operator -		(const Vector3& rhs) { return Vector3(x - rhs.x, y - rhs.y, z - rhs.z); }
	Vector3  operator *		(const Vector3& rhs) { return Vector3(x * rhs.x, y * rhs.y, z * rhs.z); }
	Vector3  operator /		(const Vector3& rhs) { return Vector3(x / rhs.x, y / rhs.y, z / rhs.z); }
	Vector3& operator +=	(const Vector3& rhs) { return *this = *this + rhs; }
	Vector3& operator -=	(const Vector3& rhs) { return *this = *this - rhs; }
	Vector3& operator *=	(const Vector3& rhs) { return *this = *this * rhs; }
	Vector3& operator /=	(const Vector3& rhs) { return *this = *this / rhs; }

	float getDistance(const Vector3 other) { return (*this - other).length(); }
	//float getDistance(const Vector3 other) { return Vector3(x - other.x, y - other.y, z - other.z).length(); }
	float length() { return sqrt(this->x * this->x + this->y * this->y + this->z + this->z); }
};

// Apparently you have to declare https://stackoverflow.com/questions/4421706/what-are-the-basic-rules-and-idioms-for-operator-overloading
//inline Vector3 operator +	(const Vector3& lhs, const Vector3& rhs) { return Vector3(lhs.x + rhs.x, lhs.y + rhs.y, lhs.z - rhs.z); }
//inline Vector3 operator -	(const Vector3& lhs, const Vector3& rhs) { return Vector3(lhs.x - rhs.x, lhs.y - rhs.y, lhs.z - rhs.z); }
//inline Vector3 operator *	(const Vector3& lhs, const Vector3& rhs) { return Vector3(lhs.x * rhs.x, lhs.y * rhs.y, lhs.z * rhs.z); }
//inline Vector3 operator /	(const Vector3& lhs, const Vector3& rhs) { return Vector3(lhs.x / rhs.x, lhs.y / rhs.y, lhs.z / rhs.z); }

//
//
//
//class Vector3
//{
//public:
//
//    float x, y, z;
//
//    Vector3() {};
//    Vector3(const float x, const float y, const float z) : x(x), y(y), z(z) {}
//    Vector3 operator + (const Vector3& rhs) const { return Vector3(x + rhs.x, y + rhs.y, z + rhs.z); }
//    Vector3 operator - (const Vector3& rhs) const { return Vector3(x - rhs.x, y - rhs.y, z - rhs.z); }
//    Vector3 operator * (const float& rhs) const { return Vector3(x * rhs, y * rhs, z * rhs); }
//    Vector3 operator / (const float& rhs) const { return Vector3(x / rhs, y / rhs, z / rhs); }
//    Vector3& operator += (const Vector3& rhs) { return *this = *this + rhs; }
//    Vector3& operator -= (const Vector3& rhs) { return *this = *this - rhs; }
//    Vector3& operator *= (const float& rhs) { return *this = *this * rhs; }
//    Vector3& operator /= (const float& rhs) { return *this = *this / rhs; }
//    float Length() const { return sqrtf(x * x + y * y + z * z); }
//    Vector3 Normalize() const { return *this * (1 / Length()); }
//    float Distance(const Vector3& rhs) const { return (*this - rhs).Length(); }
//};