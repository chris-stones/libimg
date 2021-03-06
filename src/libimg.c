/*
 * libimg.c
 *
 *  Created on: Oct 24, 2011
 *      Author: cds
 */

#include "img_config.h"

#include "libimg.h"
#include "img_png.h"
#include "img_cpi.h"
#include "img_dds.h"
#include "img_tga.h"
#include "img_p6.h"
#include "img_bmp.h"

#include<assert.h>
#include<stdlib.h>
#include<string.h>
#if defined(HAVE_STRINGS_H)
#include<strings.h>
#endif
#if defined(HAVE_STRING_H)
#include<string.h>
#endif
#if defined(HAVE_LINUX_LIMITS_H)
#include<linux/limits.h> // for PATH_MAX
#endif
#include<stdarg.h>
#include<stdio.h>

int LIBIMG_DLL imgAllocAndRead(struct imgImage **ret, const char *fn) {

	int err = IMG_OKAY;

	if((err = imgAllocImage(ret)) != IMG_OKAY)
		return err;

	if((err = imgStatFile(*ret,fn)) != IMG_OKAY) {

		imgFreeImage(*ret);
		*ret = NULL;
		return err;
	}

	if((err = imgAllocPixelBuffers(*ret)) != IMG_OKAY) {
		imgFreeImage(*ret);
		*ret = NULL;
		return err;
	}

	if((err = imgReadFile(*ret, fn)) != IMG_OKAY ) {

		imgFreePixelBuffers(*ret);
		imgFreeImage(*ret);
		*ret = NULL;
		return err;
	}

	return err;
}

int LIBIMG_DLL imgAllocAndStat(struct imgImage **ret, const char *fn)
{
	int err = IMG_ERROR;

	if(!ret || !fn)
		return err;

	if((err = imgAllocImage(ret))!=IMG_OKAY)
		return err;

	if((err = imgStatFile(*ret, fn))!=IMG_OKAY) {
		imgFreeImage(*ret);
		*ret = NULL;
		return err;
	}

	return err;
}

void LIBIMG_DLL imgFreeAll(struct imgImage *img) {

	if(!img)
		return;

	imgFreePixelBuffers(img);
	imgFreeImage(img);
}

#if defined(_MSC_VER)
#include <ctype.h>
static int strcasecmp(const char *s1, const char *s2)
{
	unsigned i;

	for (i = 0; s1[i] && s2[i]; i++)
	{
		unsigned char c1 = tolower((unsigned char)s1[i]);
		unsigned char c2 = tolower((unsigned char)s2[i]);

		if (c1 < c2)
			return -1;
		else if (c1 > c2)
			return 1;
	}

	return !s2[i] - !s1[i];
}
#endif

int LIBIMG_DLL imgCheckFileExtension(const char *fn, const char *ext)
{
	if(!fn || !ext)
		return -1;

	if(strlen(fn)<strlen(ext))
		return -1;

	return strcasecmp(fn + strlen(fn) - strlen(ext),ext);
}

int LIBIMG_DLL imgGetChannels(enum imgFormat format) {

	if(format & IMG_FMT_COMPONENT_PACKED)
		return 1;

	if(format & IMG_FMT_COMPONENT_PLANAR) {
		if(format & IMG_FMT_COMPONENT_ALPHA)
			return 4;
		return 3;
	}
	
	if(format & IMG_FMT_COMPONENT_COMPRESSED)
	  return 1;
	
	assert(0);
	return 0;
}

int LIBIMG_DLL imgGetBytesPerPixel(enum imgFormat format, int channel) {

	if(format & IMG_FMT_COMPONENT_PACKED) {

		if(channel)
			return 0;

		int psize =
				(format & (	IMG_FMT_COMPONENT_PACKED8  |
							IMG_FMT_COMPONENT_PACKED15 |
							IMG_FMT_COMPONENT_PACKED16 |
							IMG_FMT_COMPONENT_PACKED24 |
							IMG_FMT_COMPONENT_PACKED32 |
							IMG_FMT_COMPONENT_PACKED48 |
							IMG_FMT_COMPONENT_PACKED64 |
							IMG_FMT_COMPONENT_PACKED96 |
							IMG_FMT_COMPONENT_PACKED128));

		switch(psize) {
		case IMG_FMT_COMPONENT_PACKED8: 	return  1;
		case IMG_FMT_COMPONENT_PACKED15:	return  2;
		case IMG_FMT_COMPONENT_PACKED16:	return  2;
		case IMG_FMT_COMPONENT_PACKED24:	return  3;
		case IMG_FMT_COMPONENT_PACKED32:	return  4;
		case IMG_FMT_COMPONENT_PACKED48:	return  6;
		case IMG_FMT_COMPONENT_PACKED64:	return  8;
		case IMG_FMT_COMPONENT_PACKED96:	return 12;
		case IMG_FMT_COMPONENT_PACKED128:	return 16;
		}

		assert(0);
	}

	if(format & IMG_FMT_COMPONENT_PLANAR) {

		if((channel==3) && !(format & IMG_FMT_COMPONENT_ALPHA))
			return 0;

		if(channel<=3)
			return 1;

		assert(0);
	}

	assert(0);

	return 0;
}

static int CrCbAdjustResolution(int res,int channel) {

	switch(channel) {
	case 1:
	case 2:
		return (res+1)>>1;
	default:
		return res;
	}
}

static int determineLineSize(enum imgFormat format, int w, int channel) {

	if(format & IMG_FMT_COMPONENT_COMPRESSED) {

		int block_size = 16;

		if((format & IMG_FMT_COMPONENT_COMPRESSION_INDEX_MASK) == IMG_FMT_COMPONENT_DXT1_INDEX)
			block_size = 8;

		if((format & IMG_FMT_COMPONENT_COMPRESSION_INDEX_MASK) == IMG_FMT_COMPONENT_ETC1_INDEX)
			block_size = 8;

		if(w<1) w = 1;

		return block_size * ((w+3)/4);
	}

	return 	imgGetBytesPerPixel(format, channel) *
			CrCbAdjustResolution(w,channel);
}

static int determineLinearSize(enum imgFormat format, int w, int h, int channel) {

	if(format & IMG_FMT_COMPONENT_COMPRESSED) {

		if(channel)
		  return 0;

		int block_size = 16;
		if((format & IMG_FMT_COMPONENT_COMPRESSION_INDEX_MASK) == IMG_FMT_COMPONENT_DXT1_INDEX)
			block_size = 8;
		if((format & IMG_FMT_COMPONENT_COMPRESSION_INDEX_MASK) == IMG_FMT_COMPONENT_ETC1_INDEX)
			block_size = 8;

		if(w<1) w = 1;
		if(h<1) h = 1;

		return block_size * ((w+3)/4) * ((h+3)/4);
	}

	return 	imgGetBytesPerPixel(format,channel) *
			CrCbAdjustResolution(w,channel) *
			CrCbAdjustResolution(h,channel)	;
}

int LIBIMG_DLL imgGetLinearSize(enum imgFormat format, int w, int h, int channel) {
 
  return determineLinearSize(format, w, h, channel);
}

struct imgData LIBIMG_DLL imgGetPixel(const struct imgImage *img, int x, int y) {

	struct imgData pixel;
	memset(&pixel,0,sizeof pixel);

	assert(img);
	assert(x < img->width);
	assert(y < img->height);

	if(img && (x < img->width) && (y<img->height)) {

		int c;
		int channels = imgGetChannels(img->format);

		assert(channels);
		assert(channels<=4);

		for(c=0; c<channels; c++) {
			if(img->data.channel[c]) {

				int is_chroma = (c+1) & 2;
				int ax = is_chroma ? x>>1 : x;
				int ay = is_chroma ? y>>1 : y;

				pixel.channel[c] = ((char*)(img->data.channel[c])) +
									(ay * img->linesize[c]) +
									(ax * imgGetBytesPerPixel(img->format, c));
			}
		}
	}


	return pixel;
}


int LIBIMG_DLL imgAllocImage(struct imgImage **img) {

	if(!img)
		return IMG_ERROR;

	if((*img = (struct imgImage*)calloc( 1, sizeof(struct imgImage))))
		return IMG_OKAY;

	return IMG_ERROR;
}

void LIBIMG_DLL imgFreeImage(struct imgImage *img) {

	free(img);
}

int LIBIMG_DLL imgAllocPixelBuffers(struct imgImage *img) {

	int channels,channel;

	if(!img)
		return IMG_ERROR;

	channels = imgGetChannels( img->format );

	assert(channels && channels <= 4);

	memset(img->data.channel, 0, sizeof img->data.channel);

	for(channel=0;channel<channels;channel++) {

		img->linesize[channel] =
			determineLineSize(img->format, img->width, channel);

		img->linearsize[channel] =
			determineLinearSize(img->format, img->width, img->height, channel);

		img->data.channel[channel] = malloc( img->linearsize[channel]  ); 
		
		if(!img->data.channel[channel]) {

			while(channel--) {
				free(img->data.channel[channel]);
				img->data.channel[channel] = NULL;
			}
			return IMG_ERROR;
		}
	}

	return IMG_OKAY;
}

int LIBIMG_DLL imgSetPixelBuffer(struct imgImage *img, void *buffer, int channel) {

	if(!img || channel >=4)
		return IMG_ERROR;

	img->data.channel[channel] = buffer;

	if(buffer) {
		img->linesize[channel] =
			determineLineSize(img->format,img->width,channel);

		img->linearsize[channel] =
			determineLinearSize(img->format, img->width, img->height, channel);
	}
	else
		img->linearsize[channel] = img->linesize[channel] = 0;

	return IMG_OKAY;
}

int LIBIMG_DLL imgSetAllPixelBuffers(struct imgImage *img, void *buffer0, void* buffer1, void* buffer2, void* buffer3) {

	if(!img)
		return IMG_ERROR;

	imgSetPixelBuffer(img,buffer0,0);
	imgSetPixelBuffer(img,buffer1,1);
	imgSetPixelBuffer(img,buffer2,2);
	imgSetPixelBuffer(img,buffer3,3);

	return IMG_OKAY;
}

int LIBIMG_DLL imgFreePixelBuffers(struct imgImage *img) {

	int channel;

	if(!img)
		return IMG_ERROR;

	for(channel=0;channel<4;channel++)
		free( img->data.channel[channel] );

	return IMG_OKAY;
}

int LIBIMG_DLL imgReadFile(struct imgImage *img, const char* fn) {

	int err = IMG_ERROR;

#ifdef WITH_P6_READ
	if( (err = imgReadImgP6( fn, img ) ) == IMG_OKAY )
		return err;
#endif

#ifdef WITH_DDS_READ
	if( (err = imgReadImgDds( fn, img ) ) == IMG_OKAY )
			return err;
#endif

#ifdef WITH_CPI_READ
	if( (err = imgReadImgCpi( fn, img ) ) == IMG_OKAY )
		return err;
#endif

#ifdef WITH_PNG_READ
	if( (err = imgReadImgPng( fn, img ) ) == IMG_OKAY )
		return err;
#endif

#ifdef WITH_TGA_READ
	if( (err = imgReadImgTga( fn, img ) ) == IMG_OKAY )
		return err;
#endif

#ifdef WITH_BMP_READ
	if( (err = imgReadImgBmp( fn, img ) ) == IMG_OKAY )
		return err;
#endif
	return err;
}

#ifdef WITH_P6_WRITE
enum imgFormat imgRecomendFormatP6 (const char * fn, enum imgFormat hint, int allow_poorly_supported);
#endif
#ifdef WITH_DDS_WRITE
enum imgFormat imgRecomendFormatDds(const char * fn, enum imgFormat hint, int allow_poorly_supported);
#endif
#ifdef WITH_CPI_WRITE
enum imgFormat imgRecomendFormatCpi(const char * fn, enum imgFormat hint, int allow_poorly_supported);
#endif
#ifdef WITH_PNG_WRITE
enum imgFormat imgRecomendFormatPng(const char * fn, enum imgFormat hint, int allow_poorly_supported);
#endif
#ifdef WITH_TGA_WRITE
enum imgFormat imgRecomendFormatTga(const char * fn, enum imgFormat hint, int allow_poorly_supported);
#endif
#ifdef WITH_BMP_WRITE
enum imgFormat imgRecomendFormatBmp(const char * fn, enum imgFormat hint, int allow_poorly_supported);
#endif

enum imgFormat LIBIMG_DLL imgRecomendFormat(const char * fn, enum imgFormat hint, int allow_poorly_supported) {

	int fmt = IMG_FMT_UNKNOWN;

#ifdef WITH_P6_WRITE
	if( (fmt = imgRecomendFormatP6( fn, hint, allow_poorly_supported ) ) != IMG_FMT_UNKNOWN )
		return fmt;
#endif

#ifdef WITH_DDS_WRITE
	if( (fmt = imgRecomendFormatDds( fn, hint, allow_poorly_supported ) ) != IMG_FMT_UNKNOWN )
	  return fmt;
#endif

#ifdef WITH_CPI_WRITE
	if( (fmt = imgRecomendFormatCpi( fn, hint, allow_poorly_supported ) ) != IMG_FMT_UNKNOWN )
		return fmt;
#endif

#ifdef WITH_PNG_WRITE
	if( (fmt = imgRecomendFormatPng( fn, hint, allow_poorly_supported ) ) != IMG_FMT_UNKNOWN )
		return fmt;
#endif

#ifdef WITH_TGA_WRITE
	if( (fmt = imgRecomendFormatTga( fn, hint, allow_poorly_supported ) ) != IMG_FMT_UNKNOWN )
		return fmt;
#endif

#ifdef WITH_BMP_WRITE
	if( (fmt = imgRecomendFormatBmp( fn, hint, allow_poorly_supported ) ) != IMG_FMT_UNKNOWN )
		return fmt;
#endif

	return fmt;
}

int LIBIMG_DLL imgStatFile(struct imgImage *img, const char *fn) {

	int err = IMG_ERROR;

#ifdef WITH_P6_READ
	if( (err = imgStatImgP6( fn, img ) ) == IMG_OKAY )
		return err;
#endif

#ifdef WITH_DDS_READ
	if( (err = imgStatImgDds( fn, img ) ) == IMG_OKAY )
	  return err;
#endif

#ifdef WITH_CPI_READ
	if( (err = imgStatImgCpi( fn, img ) ) == IMG_OKAY )
		return err;
#endif

#ifdef WITH_PNG_READ
	if( (err = imgStatImgPng( fn, img ) ) == IMG_OKAY )
		return err;
#endif

#ifdef WITH_TGA_READ
	if( (err = imgStatImgTga( fn, img ) ) == IMG_OKAY )
		return err;
#endif

#ifdef WITH_BMP_READ
	if( (err = imgStatImgBmp( fn, img ) ) == IMG_OKAY )
		return err;
#endif
	return err;
}

int LIBIMG_DLL imgWriteFile(struct imgImage *img, const char* fn) {

	int err = IMG_ERROR;

#ifdef WITH_DDS_WRITE
	if( imgCheckFileExtension( fn, ".dds") == 0 )
		if( (err = imgWriteImgDds( fn, img ) ) == IMG_OKAY )
			return err;
#endif

#ifdef WITH_CPI_WRITE
	if( imgCheckFileExtension( fn, ".cpi") == 0 )
	if( (err = imgWriteImgCpi( fn, img ) ) == IMG_OKAY )
		return err;
#endif

#ifdef WITH_PNG_WRITE
	printf("PNG? %s\n",fn);
	if( imgCheckFileExtension( fn, ".png") == 0 ) {
		printf("good extension\n");
		if( (err = imgWriteImgPng( fn, img ) ) == IMG_OKAY ) {
			printf("SUCCESS\n");
			return err;
		}
		printf("error!\n");
	}
	printf("NOT PNG\n");
#endif

#ifdef WITH_P6_WRITE
	if( imgCheckFileExtension( fn, ".p6") == 0 )
		if( (err = imgWriteImgP6( fn, img ) ) == IMG_OKAY )
			return err;
#endif

#ifdef WITH_TGA_WRITE
	if( imgCheckFileExtension( fn, ".tga") == 0 )
		if( (err = imgWriteImgTga( fn, img ) ) == IMG_OKAY )
			return err;
#endif

#ifdef WITH_BMP_WRITE
	if( imgCheckFileExtension( fn, ".bmp") == 0 )
		if( (err = imgWriteImgBmp( fn, img ) ) == IMG_OKAY )
			return err;
#endif
	return err;
}

typedef int (*ppImgFnFnPtr)(struct imgImage **, const char *);
typedef int ( *pImgFnFnPtr)(struct imgImage  *, const char *);

static int __format_and_call(
		ppImgFnFnPtr ppcall, 	struct imgImage **ppret,
		 pImgFnFnPtr  pcall,	struct imgImage  *pret,
		const char *format,
		va_list va )
{
	int err = IMG_OKAY;

	char *path = malloc(sizeof (char) * PATH_MAX);

	if(!path)
		return IMG_ERROR;

	if(vsnprintf(path,PATH_MAX,format,va)>=PATH_MAX)
		err = IMG_ERROR; /* truncated */

	if(err == IMG_OKAY) {

		err = IMG_ERROR;

		if(ppret && ppcall)
			err = ppcall(ppret,path);
		else if(pret && pcall)
			err = pcall(pret,path);
	}

	free(path);

	return err;
}

#define FUNC_F_PP(func) \
	int LIBIMG_DLL func##F (struct imgImage **img, const char* format, ...) {\
		int err; \
		va_list va; \
		va_start(va,format); \
		err = __format_and_call(&func, img, NULL,NULL, format, va); \
		va_end(va); \
		return err; \
	}

#define FUNC_F_P(func) \
	int LIBIMG_DLL func##F(struct imgImage *img, const char* format, ...) {\
		int err; \
		va_list va; \
		va_start(va,format); \
		err = __format_and_call(NULL, NULL, &func,img, format, va); \
		va_end(va); \
		return err; \
	}

FUNC_F_PP(imgAllocAndRead)
FUNC_F_PP(imgAllocAndStat)
FUNC_F_P(imgReadFile)
FUNC_F_P(imgStatFile)
FUNC_F_P(imgWriteFile)

#undef FUNC_F_P
#undef FUNC_F_PP

