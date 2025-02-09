#include "PlanetData.h"

uint32_t PlanetTime::ToSeconds()
{
	uint32_t seconds = 0;

	seconds = Years * 31'536'000;
	seconds += Days * 86'400;
	seconds += Hours * 3'600;
	seconds += Minutes * 60;
	seconds += Seconds;

	return seconds;
}