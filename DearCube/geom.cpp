#include "geom.h"

using Geom::Vector2;
using Geom::Vector3;


float Vector2::length()
{
	// From https://guidedhacking.com/threads/correct-angle-comparison-for-fov-aimbot.15714/
	// I don't really understand those maths concepts
	// It get closer when facing back to them....
	//float l, r;
	//Vector2 rad;
	//Vector3 pos;

	//rad.x = x * PI / 180;
	//rad.y = y * PI / 180;
	//r = abs(cos(rad.x));
	//pos.y = sin(rad.x);
	//pos.x = r * sin(rad.y);
	//pos.z = r * cos(rad.y);
	//l = pos.getDistance(Vector3(0, 0, 1));

	//// angle length
	//return acos(1 - l * l / 2) * 180 / PI;
	
	// My way
	float length = sqrt(pow(x, 2.0f) + pow(y, 2.0f));
	return length;
}

//Vector2 Vector2::operator-(const Vector2& rhs)
//{
//	Vector2 delta(this->x - rhs.x, this->y - rhs.y);
//	if (delta.x > 180) delta.x -= 360;
//	if (delta.x < -180) delta.x += 360;
//
//	return delta;
//}

Vector2 Vector2::normalize()
{
	Vector2 result(*this);

	while (result.x > 180.0f) result.x -= 360.0f;
	while (result.x < -180.0f) result.x += 360.0f;

	while (result.y > 90.0f) result.y -= 180.0f;
	while (result.y < -90.0f) result.y += 180.0f;

	return result;
}

Vector2 Geom::calcAngle(Vector3 posSrc, Vector3 posDst)
{
	// Calculate positions difference
	Vector3 delta = posSrc - posDst;
	
	// To get the pitch angle (vertical angle),  we calculate :
	// ATAN(TAN) = angle in radians
	// TAN       = Opposite / Adjacent
	// Opposiote = delta.z (height difference)
	// Adjacent  = hypotenuse (of the right triangle representing the pos difference on the 2d plan of x and y (not the height))
	// atan here returns a value between [-PI/2, PI/2] radians which equivalent to [-90, 90] degrees.
	// radian -> degree = rad * 180 / PI
	float pitch = -atan(delta.z / sqrt(pow(delta.x, 2.0f) + pow(delta.y, 2.0f))) * 180 / PI;

	// To get the yaw angle (horizontal angle), we calculate :
	// ATAN(TAN) = angle in radians
	// TAN       = Opposite / Adjacent
	// Opposiote = delta.y
	// Adjacent  = delat.x
	// atan2 here wants the values as coordinates points : atan2(x, y);
	// atan2 here returns a value between [-PI, PI] radians which equivalent to [-180, 180] degrees (we can add 180 to obtain [0, 360]).
	// radian -> degree = rad * 180 / PI
	// Don't really know why I had to reverse delta.y
	float yaw = atan2(delta.x, -delta.y) * 180 / PI +180.0f;

	// In AC viewAngles are (yaw, pitch, roll)
	return Vector2(yaw, pitch);	// We don't use roll
}
