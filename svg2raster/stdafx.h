// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#include <stdio.h>

#ifdef _WIN32
#include "targetver.h"
#include <tchar.h>
#endif

// graphicsmagick
#define STATIC_MAGICK
#ifdef _WIN32
#ifdef _WIN64
#undef WIN64
#define WIN64
#else
#undef WIN32
#define WIN32
#endif
#endif