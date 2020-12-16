#pragma once

#define _USE_MATH_DEFINES
#include <math.h>

#include <stdio.h>
#include <windows.h>


#ifndef RELEASE
#define assert( x ) if( !( x ) ) { MessageBoxA( 0, #x, "Debug Assertion Failed", MB_OK ); }
#endif