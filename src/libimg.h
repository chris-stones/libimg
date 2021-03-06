/*
 * libimg.h
 *
 *  Created on: Oct 23, 2011
 *      Author: cds
 */

#pragma once

#if defined(_MSC_VER)
	#if defined(LIBIMG_EXPORTS)
		#define LIBIMG_DLL __declspec(dllexport)
	#else
		#define LIBIMG_DLL __declspec(dllimport)
	#endif
#else
	#define LIBIMG_DLL
#endif

#include <stdio.h>
#include <stdlib.h>

#define IMG_OKAY 0			/*** OKAY ***/
#define IMG_ERROR -1		/*** GENERAL ERROR ***/
#define IMG_NOFILE -2		/*** FILE ERROR ***/
#define IMG_BADMAGIC -3		/*** UNEXPECTED FILE MAGIC ***/
#define IMG_BADEXT -4		/*** UNEXTEPCTED FILENAME EXTENSION ***/
#define IMG_BADFORMAT -5	/*** INCOMPATABLE PIXEL FORMAT ***/

#define IMG_FMT_COMPONENT_RED			( 1<< 0)
#define IMG_FMT_COMPONENT_GREEN			( 1<< 1)
#define IMG_FMT_COMPONENT_BLUE			( 1<< 2)
#define IMG_FMT_COMPONENT_ALPHA			( 1<< 3)
#define IMG_FMT_COMPONENT_Y				( 1<< 4)
#define IMG_FMT_COMPONENT_CB			( 1<< 5)
#define IMG_FMT_COMPONENT_CR			( 1<< 6)
#define IMG_FMT_COMPONENT_PLANAR		( 1<< 7)
#define IMG_FMT_COMPONENT_420P			((1<< 8) | IMG_FMT_COMPONENT_PLANAR)
#define IMG_FMT_COMPONENT_PACKED		( 1<< 9)
#define IMG_FMT_COMPONENT_PACKED8		((1<<10) | IMG_FMT_COMPONENT_PACKED)
#define IMG_FMT_COMPONENT_PACKED15		((1<<11) | IMG_FMT_COMPONENT_PACKED)
#define IMG_FMT_COMPONENT_PACKED16		((1<<12) | IMG_FMT_COMPONENT_PACKED)
#define IMG_FMT_COMPONENT_PACKED24		((1<<13) | IMG_FMT_COMPONENT_PACKED)
#define IMG_FMT_COMPONENT_PACKED32		((1<<14) | IMG_FMT_COMPONENT_PACKED)
#define IMG_FMT_COMPONENT_PACKED48		((1<<15) | IMG_FMT_COMPONENT_PACKED)
#define IMG_FMT_COMPONENT_PACKED64		((1<<16) | IMG_FMT_COMPONENT_PACKED)
// NOTE - PACKED96/128 - see below.
#define IMG_FMT_COMPONENT_RGBA			( 1<<17) // packed channel order
#define IMG_FMT_COMPONENT_ARGB			( 1<<18) // packed channel order
#define IMG_FMT_COMPONENT_BGRA			( 1<<19) // packed channel order
#define IMG_FMT_COMPONENT_ABGR			( 1<<20) // packed channel order
#define IMG_FMT_COMPONENT_YCBCRA		( 1<<21)
#define IMG_FMT_COMPONENT_YCRCBA		( 1<<22)
#define IMG_FMT_COMPONENT_GREY			( 1<<23)

// bits 25,26,27 select up-to 8 compression formats. ( 0 .. 7 )
#define IMG_FMT_COMPONENT_COMPRESSED	( 1<<24)
#define IMG_FMT_COMPONENT_COMPRESSION_INDEX_MASK (7<<25)
#define IMG_FMT_COMPONENT_DXT1_INDEX     (1<<25)
#define IMG_FMT_COMPONENT_DXT1			((1<<25) | IMG_FMT_COMPONENT_COMPRESSED)
#define IMG_FMT_COMPONENT_DXT3_INDEX     (2<<25)
#define IMG_FMT_COMPONENT_DXT3			((2<<25) | IMG_FMT_COMPONENT_COMPRESSED)
#define IMG_FMT_COMPONENT_DXT5_INDEX     (3<<25)
#define IMG_FMT_COMPONENT_DXT5			((3<<25) | IMG_FMT_COMPONENT_COMPRESSED)
#define IMG_FMT_COMPONENT_ETC1_INDEX     (4<<25)
#define IMG_FMT_COMPONENT_ETC1			((4<<25) | IMG_FMT_COMPONENT_COMPRESSED)

#define IMG_FMT_COMPONENT_DXTn			(IMG_FMT_COMPONENT_DXT1 | IMG_FMT_COMPONENT_DXT3 | IMG_FMT_COMPONENT_DXT5)

#define IMG_FMT_COMPONENT_PMA			(1<<28)
#define IMG_FMT_COMPONENT_PACKED96		((1<<29) | IMG_FMT_COMPONENT_PACKED)
#define IMG_FMT_COMPONENT_PACKED128		((1<<30) | IMG_FMT_COMPONENT_PACKED)
#define IMG_FMT_COMPONENT_FLOAT			(1<<31)

#define IMG_FMT_COMPONENT_NULL			0

#define IMG_FMT_1(x) 				IMG_FMT_COMPONENT_ ## x
#define IMG_FMT_2(x1,x2) 			IMG_FMT_1(x1) | IMG_FMT_1(x2)
#define IMG_FMT_3(x1,x2,x3)			IMG_FMT_2(x1,x2) | IMG_FMT_1(x3)
#define IMG_FMT_4(x1,x2,x3,x4)			IMG_FMT_3(x1,x2,x3) | IMG_FMT_1(x4)
#define IMG_FMT_5(x1,x2,x3,x4,x5)		IMG_FMT_4(x1,x2,x3,x4) | IMG_FMT_1(x5)
#define IMG_FMT_6(x1,x2,x3,x4,x5,x6)		IMG_FMT_5(x1,x2,x3,x4,x5) | IMG_FMT_1(x6)
#define IMG_FMT_7(x1,x2,x3,x4,x5,x6,x7)		IMG_FMT_6(x1,x2,x3,x4,x5,x6) | IMG_FMT_1(x7)

#define IMG_FMT_COLS4(c1,c2,c3,c4) 	IMG_FMT_4(c1,c2,c3,c4)
#define IMG_FMT_COLS3(c1,c2,c3)		IMG_FMT_3(c1,c2,c3)


#ifdef __cplusplus
extern "C" {
#endif

enum imgFormat {

	IMG_FMT_UNKNOWN		=	0,

	IMG_FMT_RGB16		= 	IMG_FMT_5(PACKED16,RGBA,RED,GREEN,BLUE),
	IMG_FMT_BGR16		= 	IMG_FMT_5(PACKED16,BGRA,RED,GREEN,BLUE),

	IMG_FMT_RGB15		= 	IMG_FMT_5(PACKED15,RGBA,RED,GREEN,BLUE),
	IMG_FMT_BGR15		= 	IMG_FMT_5(PACKED15,BGRA,RED,GREEN,BLUE),

	IMG_FMT_RGB24		=	IMG_FMT_5(PACKED24,RGBA,RED,GREEN,BLUE),
	IMG_FMT_BGR24		=	IMG_FMT_5(PACKED24,BGRA,RED,GREEN,BLUE),
	
	/*** uncompressed formats with alpha channels ***/
	IMG_FMT_RGBA16		=	IMG_FMT_6(PACKED16,RGBA,RED,GREEN,BLUE,ALPHA),
	IMG_FMT_RGBA32		=	IMG_FMT_6(PACKED32,RGBA,RED,GREEN,BLUE,ALPHA),
	IMG_FMT_BGRA32		=	IMG_FMT_6(PACKED32,BGRA,RED,GREEN,BLUE,ALPHA),
	IMG_FMT_ARGB32		=	IMG_FMT_6(PACKED32,ARGB,RED,GREEN,BLUE,ALPHA),
	IMG_FMT_ABGR32		=	IMG_FMT_6(PACKED32,ABGR,RED,GREEN,BLUE,ALPHA),
	IMG_FMT_RGBA64		=	IMG_FMT_6(PACKED64,RGBA,RED,GREEN,BLUE,ALPHA),
	IMG_FMT_BGRA64		=	IMG_FMT_6(PACKED64,BGRA,RED,GREEN,BLUE,ALPHA),
	IMG_FMT_ARGB64		=	IMG_FMT_6(PACKED64,ARGB,RED,GREEN,BLUE,ALPHA),
	IMG_FMT_ABGR64		=	IMG_FMT_6(PACKED64,ABGR,RED,GREEN,BLUE,ALPHA),
	IMG_FMT_GREYA16		=	IMG_FMT_3(PACKED16,GREY,ALPHA),
	IMG_FMT_GREYA32		=	IMG_FMT_3(PACKED32,GREY,ALPHA),
	IMG_FMT_YUVA420P	=	IMG_FMT_6(420P,YCBCRA,Y,CB,CR,ALPHA),
	
	/*** pre-multiplied alpha versions of above ***/
	IMG_FMT_RGBA16_PMA	=	IMG_FMT_7(PACKED16,RGBA,RED,GREEN,BLUE,ALPHA,PMA),
	IMG_FMT_RGBA32_PMA	=	IMG_FMT_7(PACKED32,RGBA,RED,GREEN,BLUE,ALPHA,PMA),
	IMG_FMT_BGRA32_PMA	=	IMG_FMT_7(PACKED32,BGRA,RED,GREEN,BLUE,ALPHA,PMA),
	IMG_FMT_ARGB32_PMA	=	IMG_FMT_7(PACKED32,ARGB,RED,GREEN,BLUE,ALPHA,PMA),
	IMG_FMT_ABGR32_PMA	=	IMG_FMT_7(PACKED32,ABGR,RED,GREEN,BLUE,ALPHA,PMA),
	IMG_FMT_RGBA64_PMA	=	IMG_FMT_7(PACKED64,RGBA,RED,GREEN,BLUE,ALPHA,PMA),
	IMG_FMT_BGRA64_PMA	=	IMG_FMT_7(PACKED64,BGRA,RED,GREEN,BLUE,ALPHA,PMA),
	IMG_FMT_ARGB64_PMA	=	IMG_FMT_7(PACKED64,ARGB,RED,GREEN,BLUE,ALPHA,PMA),
	IMG_FMT_ABGR64_PMA	=	IMG_FMT_7(PACKED64,ABGR,RED,GREEN,BLUE,ALPHA,PMA),
	IMG_FMT_GREYA16_PMA	=	IMG_FMT_4(PACKED16,GREY,ALPHA,PMA),
	IMG_FMT_GREYA32_PMA	=	IMG_FMT_4(PACKED32,GREY,ALPHA,PMA),
	IMG_FMT_YUVA420P_PMA	=	IMG_FMT_7(420P,YCBCRA,Y,CB,CR,ALPHA,PMA), //hmm.. how would this work!?
	
	
	IMG_FMT_RGBX32		=	IMG_FMT_5(PACKED32,RGBA,RED,GREEN,BLUE),
	IMG_FMT_BGRX32		=	IMG_FMT_5(PACKED32,BGRA,RED,GREEN,BLUE),
	IMG_FMT_XRGB32		=	IMG_FMT_5(PACKED32,ARGB,RED,GREEN,BLUE),
	IMG_FMT_XBGR32		=	IMG_FMT_5(PACKED32,ABGR,RED,GREEN,BLUE),
	IMG_FMT_RGB48		=	IMG_FMT_5(PACKED48,RGBA,RED,GREEN,BLUE),
	IMG_FMT_BGR48		=	IMG_FMT_5(PACKED48,BGRA,RED,GREEN,BLUE),
	IMG_FMT_RGBX64		=	IMG_FMT_5(PACKED64,RGBA,RED,GREEN,BLUE),
	IMG_FMT_BGRX64		=	IMG_FMT_5(PACKED64,BGRA,RED,GREEN,BLUE),
	IMG_FMT_XRGB64		=	IMG_FMT_5(PACKED64,ARGB,RED,GREEN,BLUE),
	IMG_FMT_XBGR64		=	IMG_FMT_5(PACKED64,ABGR,RED,GREEN,BLUE),

	IMG_FMT_GREY8		=	IMG_FMT_2(PACKED8,GREY),
	IMG_FMT_GREY16		=	IMG_FMT_2(PACKED16,GREY),

	IMG_FMT_YUV420P		=	IMG_FMT_5(420P,YCBCRA,Y,CB,CR),

	// compressed formats
	IMG_FMT_DXT1  		=	IMG_FMT_4(DXT1,RED,GREEN,BLUE),
	IMG_FMT_DXT3 		= 	IMG_FMT_5(DXT3,RED,GREEN,BLUE,ALPHA),
	IMG_FMT_DXT5 		= 	IMG_FMT_5(DXT5,RED,GREEN,BLUE,ALPHA),
	IMG_FMT_ETC1		=	IMG_FMT_4(ETC1,RED,GREEN,BLUE),
	
	// compressed formats with pre-multiplied alpha
	IMG_FMT_DXT4 		= 	IMG_FMT_6(DXT5,RED,GREEN,BLUE,ALPHA,PMA),
	IMG_FMT_DXT2 		= 	IMG_FMT_6(DXT3,RED,GREEN,BLUE,ALPHA,PMA),
	
	// floating point image formats.
	IMG_FMT_FLOAT_RGB	=	IMG_FMT_6(FLOAT, PACKED96 , RGBA, RED, GREEN, BLUE       ),
	IMG_FMT_FLOAT_RGBA	=	IMG_FMT_7(FLOAT, PACKED128, RGBA, RED, GREEN, BLUE, ALPHA),
	IMG_FMT_FLOAT_RGBX	=	IMG_FMT_6(FLOAT, PACKED128, RGBA, RED, GREEN, BLUE       ),
	IMG_FMT_FLOAT_ARGB	=	IMG_FMT_7(FLOAT, PACKED128, RGBA, RED, GREEN, BLUE, ALPHA),
	IMG_FMT_FLOAT_XRGB	=	IMG_FMT_6(FLOAT, PACKED128, RGBA, RED, GREEN, BLUE       ),
	IMG_FMT_FLOAT_BGR	=	IMG_FMT_6(FLOAT, PACKED96,  RGBA, RED, GREEN, BLUE       ),
	IMG_FMT_FLOAT_BGRA	=	IMG_FMT_7(FLOAT, PACKED128, RGBA, RED, GREEN, BLUE, ALPHA),
	IMG_FMT_FLOAT_BGRX	=	IMG_FMT_6(FLOAT, PACKED128, RGBA, RED, GREEN, BLUE       ),
	IMG_FMT_FLOAT_ABGR	=	IMG_FMT_7(FLOAT, PACKED128, RGBA, RED, GREEN, BLUE, ALPHA),
	IMG_FMT_FLOAT_XBGR	=	IMG_FMT_6(FLOAT, PACKED128, RGBA, RED, GREEN, BLUE       ),
};

struct imgData {

	void* channel[4];
};

struct imgImage {

	int width;
	int height;

	enum	imgFormat	format;
	struct 	imgData 	data;

	int		linesize[4];
	int		linearsize[4];
};

int LIBIMG_DLL imgAllocAndRead(struct imgImage **img, const char* fn);
int LIBIMG_DLL imgAllocAndReadF(struct imgImage **img, const char* format, ...);

int LIBIMG_DLL imgAllocAndStat(struct imgImage **img, const char* fn);
int LIBIMG_DLL imgAllocAndStatF(struct imgImage **img, const char* format, ...);

int LIBIMG_DLL imgReadFile(struct imgImage  *img, const char* fn);
int LIBIMG_DLL imgReadFileF(struct imgImage  *img, const char* format, ...);

int LIBIMG_DLL imgStatFile(struct imgImage  *img, const char* fn);
int LIBIMG_DLL imgStatFileF(struct imgImage  *img, const char* format, ...);

int LIBIMG_DLL imgWriteFile(struct imgImage  *img, const char* fn);
int LIBIMG_DLL imgWriteFileF(struct imgImage  *img, const char* format, ...);

int LIBIMG_DLL imgAllocImage(struct imgImage **img);
void LIBIMG_DLL imgFreeImage(struct imgImage  *img);

int LIBIMG_DLL imgAllocPixelBuffers(struct imgImage  *img);
int LIBIMG_DLL imgFreePixelBuffers(struct imgImage  *img);
int LIBIMG_DLL imgSetPixelBuffer(struct imgImage  *img, void *buffer, int channel);
int LIBIMG_DLL imgSetAllPixelBuffers(struct imgImage  *img, void *buffer0, void* buffer1, void* buffer2, void* buffer3);

void LIBIMG_DLL imgFreeAll(struct imgImage  *img);

struct imgData LIBIMG_DLL imgGetPixel(const struct imgImage *img, int x, int y);

int LIBIMG_DLL imgCheckFileExtension(const char *fn, const char *ext);

int LIBIMG_DLL imgGetBytesPerPixel(enum imgFormat format, int channel);
int LIBIMG_DLL imgGetChannels(enum imgFormat format);
int LIBIMG_DLL imgGetLinearSize(enum imgFormat format, int w, int h, int channel);

enum imgFormat LIBIMG_DLL imgRecomendFormat(const char * fn, enum imgFormat hint, int allow_poorly_supported);

#ifdef __cplusplus
} /* extern "C" { */
#endif


