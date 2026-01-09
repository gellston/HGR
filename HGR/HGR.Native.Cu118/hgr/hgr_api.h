#pragma once


#ifndef HGR_NATIVE_EXPORT
#define HGR_NATIVE_API __declspec(dllimport)
#else
#define HGR_NATIVE_API __declspec(dllexport)
#endif