#include "geom.h"

using Geom::Vector2;
using Geom::Vector3;

// Copy paste from https://guidedhacking.com/threads/correct-angle-comparison-for-fov-aimbot.15714/
// as I don't really understand this maths concepts
// It seems that I only get the deistance on the same heigth
// And et get closer when facing back to them....
float Vector2::length()
{
	float xRad, yRad, l, r;
	Vector3 pos;

	xRad = x * PI / 180;
	yRad = y * PI / 180;
	r = abs(cos(xRad));
	pos.y = sin(xRad);
	pos.x = r * sin(yRad);
	pos.z = r * cos(yRad);
	l = pos.getDistance(Vector3(0, 0, 1));

	// angle
	return acos(1 - l * l / 2) * 180 / PI;
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
