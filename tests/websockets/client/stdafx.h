// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#ifdef	WIN32
#pragma once


#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers
#include <stdio.h>
#include <tchar.h>

#endif

#ifndef	WIN32
typedef	char	_TCHAR;
#define	_T(_x_)	_x_
#define	_tcscmp	strcmp
#define	_tmain	main
typedef	const char* PCSTR;
#endif

// TODO: reference additional headers your program requires here
#define NELEM(a)	((int)(sizeof(a) / sizeof((a)[0])))
