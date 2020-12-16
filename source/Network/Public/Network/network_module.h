// copyright Daniel Dahlkvist (c) 2020 [github.com/messer1024]
#pragma once

namespace dud
{

#if defined(BUILD_COMPILE_DLL)
#if defined(BUILD_EXPORT_NETWORK_MODULE)
#define NETWORK_PUBLIC EXPORT_DLL
#else
#define NETWORK_PUBLIC IMPORT_DLL
#endif
#else
#define NETWORK_PUBLIC
#endif

#if defined(BUILD_INTERNAL_ACCESS_LIBRARY_MODULE) || defined(BUILD_EXPORT_LIBRARY_MODULE)
#define NETWORK_INTERNAL LIBRARY_PUBLIC
#else
#define NETWORK_INTERNAL
#endif

NETWORK_PUBLIC int dummy_bind(); // dummy to get something to compile

}