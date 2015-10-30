/*
 * config.h
 *
 *  Created on: 11 Jan 2014
 *      Author: cds
 */

#pragma once

#if defined(_MSC_VER)
	#define PACKED_STRUCT( name, __Declaration__ ) __pragma( pack(push, 1) ) struct name __Declaration__ __pragma( pack(pop) )
#endif

#if !defined(_MSC_VER)
	#define PACKED_STRUCT( name, __Declaration__ ) struct name __Declaration__ __attribute__((__packed__))
#endif

#if defined(HAVE_CONFIG_H)
	// autotools build! use ./configure
	#include <config.h>
#else
	// NOT an auto-tools build... sane defaults!

#if defined(WIN32)
	#define _CRT_SECURE_NO_WARNINGS 1
	#define HAVE__stricmp 1
	#define PATH_MAX 260 /* HACK */
#endif

	// uncomment to add support for reading / writing following formats.
	#define WITH_BMP_READ 1
	#define WITH_BMP_WRITE 1
	//#define WITH_CPI_READ    // my toy planar lz4 compressed format.
	//#define WITH_CPI_WRITE
	//#define WITH_DDS_READ  1    // Microsoft Direct Draw Surface.
	//#define WITH_DDS_WRITE 1
	//#define WITH_P6_READ
	//#define WITH_P6_WRITE
	#define WITH_PNG_READ  1    // Portable Network Graphics - requires libpng.
	#define WITH_PNG_WRITE 1
	#define WITH_TGA_READ  1    // Truevision TGA
	#define WITH_TGA_WRITE 1
#endif

