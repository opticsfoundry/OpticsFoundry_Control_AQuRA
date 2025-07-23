/* basic typedefs only */

/*============================================================================*/
/*                 National Instruments / Data Acquisition                    */
/*----------------------------------------------------------------------------*/
/*    Copyright (c) National Instruments 2003-2014.  All Rights Reserved.     */
/*----------------------------------------------------------------------------*/
/*                                                                            */
/* Title:       NIDAQmx.h                                                     */
/* Purpose:     Include file for NI-DAQmx  library support.                */
/*                                                                            */
/*============================================================================*/

#ifndef ___typedefs_h___
#define ___typedefs_h___

#ifdef __cplusplus
	extern "C" {
#endif

#if defined(__linux__) || defined(__APPLE__)
#define __CFUNC
#define __CFUNC_C
#define __CFUNCPTRVAR
#define CVICDECL
#define CVICALLBACK     CVICDECL
#else
#define __CFUNC         __stdcall
#define __CFUNC_C       __cdecl
#define __CFUNCPTRVAR   __cdecl
#define CVICDECL        __cdecl
#define CVICALLBACK     CVICDECL
#endif


#if defined(_CVI_) && !defined(__TPC__)
#pragma EnableLibraryRuntimeChecking
#endif


// NI-DAQmx Typedefs
#ifndef _NI_int8_DEFINED_
#define _NI_int8_DEFINED_
	typedef signed char        int8;
#endif
#ifndef _NI_uInt8_DEFINED_
#define _NI_uInt8_DEFINED_
	typedef unsigned char      uInt8;
#endif
#ifndef _NI_int16_DEFINED_
#define _NI_int16_DEFINED_
	typedef signed short       int16;
#endif
#ifndef _NI_uInt16_DEFINED_
#define _NI_uInt16_DEFINED_
	typedef unsigned short     uInt16;
#endif
#ifndef _NI_int32_DEFINED_
#define _NI_int32_DEFINED_
#if ((defined(__GNUG__) || defined(__GNUC__)) && defined(__x86_64__))
	typedef signed int         int32;
#else
	typedef signed long        int32;
#endif
#endif
#ifndef _NI_uInt32_DEFINED_
#define _NI_uInt32_DEFINED_
#if ((defined(__GNUG__) || defined(__GNUC__)) && defined(__x86_64__))
	typedef unsigned int       uInt32;
#else
	typedef unsigned long      uInt32;
#endif
#endif
#ifndef _NI_float32_DEFINED_
#define _NI_float32_DEFINED_
	typedef float              float32;
#endif
#ifndef _NI_float64_DEFINED_
#define _NI_float64_DEFINED_
	typedef double             float64;
#endif
#ifndef _NI_int64_DEFINED_
#define _NI_int64_DEFINED_
#if defined(__linux__) || defined(__APPLE__)
	typedef long long int      int64;
#else
	typedef __int64            int64;
#endif
#endif
#ifndef _NI_uInt64_DEFINED_
#define _NI_uInt64_DEFINED_
#if defined(__linux__) || defined(__APPLE__)
	typedef unsigned long long uInt64;
#else
	typedef unsigned __int64   uInt64;
#endif
#endif

typedef uInt32             bool32;

typedef void*              TaskHandle;
typedef uInt32             CalHandle;

#ifndef TRUE
 #define TRUE            (1L)
#endif
#ifndef FALSE
 #define FALSE           (0L)
#endif
#ifndef NULL
 #define NULL            (0L)
#endif



#ifdef __cplusplus
	}
#endif

#endif // __typedefs_h__
