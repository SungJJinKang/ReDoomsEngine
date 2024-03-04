#include <iostream>

struct Vector3
{
	FLOAT32 x, y, z;
};

struct alignas(32) Position_BoundingSphereRadius
{
	Vector3 Position;
	FLOAT32 BoundingSphereRadius;
};

int32_t main()
{
	Position_BoundingSphereRadius a;
	std::cout << sizeof(Position_BoundingSphereRadius);
}