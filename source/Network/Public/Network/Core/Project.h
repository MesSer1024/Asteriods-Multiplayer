#pragma once

#define _USE_MATH_DEFINES
#include <math.h>

#include <stdio.h>
#include <windows.h>


#if defined(DEBUG)
#define DUD_ASSERT( x ) if( !( x ) ) { MessageBoxA( 0, #x, "Debug Assertion Failed", MB_OK ); DebugBreak(); }
#else
#define DUD_ASSERT( x ) // stripped
#endif

#if defined(DEBUG)
#define DUD_NOT_DEFINED { MessageBoxA( 0, "", "Functionality Not Implemented", MB_OK ); DebugBreak(); }
#else
#define DUD_NOT_DEFINED() // stripped
#endif