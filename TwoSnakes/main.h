
#ifndef __MAIN_H__
#define __MAIN_H__

#include "uselei.h"
#include "windows.h"

#ifdef BUILD_DLL
#define DLL_EXPORT __declspec(dllexport)
#else
#define DLL_EXPORT __declspec(dllimport)
#endif

#ifdef __cplusplus
extern "C"
{
#endif

int DLL_EXPORT player_1(int *tmpData);
int DLL_EXPORT player_2(int *tmpData);

#ifdef __cplusplus
}
#endif

#endif
