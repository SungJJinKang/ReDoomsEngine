#pragma once
#include "Utils/cityhash/src/city.h"

#define ARRAY_LENGTH(ARRAY) (sizeof(ARRAY) / sizeof(ARRAY[0]))
#define CITY_HASH64(a) CityHash64(reinterpret_cast<const char*>(&a), sizeof(a))