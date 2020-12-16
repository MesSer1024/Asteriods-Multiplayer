#pragma once

#define _USE_MATH_DEFINES
#include <math.h>

#include <stdio.h>
#include <windows.h>


#if defined(DEBUG)
#define DUD_ASSERT( x ) if( !( x ) ) { MessageBoxA( 0, #x, "Debug Assertion Failed", MB_OK ); }
#else
#define DUD_ASSERT( x ) // stripped
#endif