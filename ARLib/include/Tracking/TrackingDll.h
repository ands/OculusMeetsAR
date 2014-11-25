#ifndef TRACKINGDLL_H
#define TRACKINGDLL_H

#ifdef TRACKINGDLL_EXPORTS
#define TRACKINGDLL_API __declspec(dllexport)
#else
#define TRACKINGDLL_API __declspec(dllimport)
#endif

#endif