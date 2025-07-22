#pragma once

#ifdef _WIN32
	#ifdef BUILD_DLL
		#ifdef BHIVE_DLL
			#define BHIVE __declspec(dllexport)
		#else
			#define BHIVE __declspec(dllimport)
		#endif
	#else
		#define BHIVE
	#endif
#endif
