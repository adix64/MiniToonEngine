#pragma once

#ifdef ENGINE_DLL
#	ifdef ENGINE_EXPORT
#		define ENGINE_API __declspec(dllexport)
#	else
#		define ENGINE_API __declspec(dllimport)
#	endif
#else
#	define ENGINE_API
#endif