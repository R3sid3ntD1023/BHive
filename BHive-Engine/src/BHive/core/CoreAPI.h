#pragma once

#ifdef BUILD_SHARED
	#ifdef BUILD_DLL
		#define BHIVE_API __declspec(dllexport)
	#else
		#define BHIVE_API __declspec(dllimport)
	#endif
#endif

#ifndef BHIVE_API
	#define BHIVE_API
#endif
