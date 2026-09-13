#pragma once

#if defined(BUILD_DLL)
	#define MARIO_API __declspec(dllexport)
#else
	#define MARIO_API __declspec(dllimport)
#endif