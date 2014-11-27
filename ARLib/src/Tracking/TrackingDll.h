#ifndef TRACKINGDLL_H
#define TRACKINGDLL_H

#ifdef TRACKINGDLL_EXPORTS
	#define TRACKINGDLL_API __declspec(dllexport)
	#define EXTERN_MOD
#else
	#define TRACKINGDLL_API __declspec(dllimport)
	#define EXTERN_MOD extern
#endif

#endif