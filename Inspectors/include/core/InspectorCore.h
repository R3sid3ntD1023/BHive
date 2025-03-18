#pragma once

#ifdef BUILD_INSPECTOR
	#define INSPECTOR_API _declspec(dllexport)
#else
	#define INSPECTOR_API _declspec(dllimport)
#endif // BUILD_INSPECTOR
