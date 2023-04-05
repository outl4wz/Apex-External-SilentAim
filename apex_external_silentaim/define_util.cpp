#pragma once
#include <math.h>
#include "Offsets.cpp"
#include "Memory.cpp"

struct SilentaimInfo {
	bool *has_lockedOnPlayer;
	float *silent_aim_x;
	float *silent_aim_y;
};

struct my_vector {
	float x;
	float y;
	float z;

	my_vector operator-(my_vector ape)
	{
		return { x - ape.x, y - ape.y, z - ape.z };
	}

	my_vector operator+(my_vector ape)
	{
		return { x + ape.x, y + ape.y, z + ape.z };
	}

	my_vector operator*(float ape)
	{
		return { x * ape, y * ape, z * ape };
	}

	my_vector operator/(float ape)
	{
		return { x / ape, y / ape, z / ape };
	}

	my_vector operator/=(float ape)
	{
		x /= ape;
		y /= ape;
		z /= ape;

		return *this;
	}

	my_vector operator+=(my_vector ape)
	{
		return { x += ape.x, y += ape.y, z += ape.z };
	}

	my_vector operator-=(my_vector ape)
	{
		return { x -= ape.x, y -= ape.y, z -= ape.z };
	}

	void Normalize()
	{
		while (x > 89.0f)
			x -= 180.f;

		while (x < -89.0f)
			x += 180.f;

		while (y > 180.f)
			y -= 360.f;

		while (y < -180.f)
			y += 360.f;
	}

	float Length()
	{
		return sqrt((x * x) + (y * y) + (z * z));
	}

	float Length2D()
	{
		return sqrt((x * x) + (y * y));
	}

	float DistTo(my_vector ape)
	{
		return (*this - ape).Length();
	}

	float Dist2D(my_vector ape)
	{
		return (*this - ape).Length2D();
	}
};

