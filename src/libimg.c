/*
 * libimg.c
 *
 *  Created on: Oct 24, 2011
 *      Author: cds
 */

#include "config.h"

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
#include<strings.h>
#include<linux/limits.h> // for PATH_MAX
#include<stdarg.h>
#include<stdio.h>

int imgAllocAndRead(struct imgImage **ret, const char *fn) {

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

int imgAllocAndStat(struct imgImage **ret, const char *fn)
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

void imgFreeAll(struct imgImage *img) {

	if(!img)
		return;

	imgFreePixelBuffers(img);
	imgFreeImage(img);
}

int imgCheckFileExtension(const char *fn, const char *ext)
{
	if(!fn || !ext)
		return -1;

	if(strlen(fn)<strlen(ext))
		return -1;

	return strcasecmp(fn + strlen(fn) - strlen(ext),ext);
}

int imgGetChannels(enum imgFormat format) {

	if(format & IMG_FMT_COMPONENT_PACKED)
		return 1;

	if(format & IMG_FMT_COMPONENT_PLANAR) {
		if(format & IMG_FMT_COMPONENT_ALPHA)
			return 4;
		return 3;
	}
	
	if(format & IMG_FMT_COMPONENT_DXTn)
	  return 1;
	
	assert(0);
	return 0;
}

int imgGetBytesPerPixel(enum imgFormat format, int channel) {

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
		if((format & IMG_FMT_COMPONENT_DXT1) == IMG_FMT_COMPONENT_DXT1)
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
		if((format & IMG_FMT_COMPONENT_DXT1) == IMG_FMT_COMPONENT_DXT1)
			block_size = 8;

		if(w<1) w = 1;
		if(h<1) h = 1;

		return block_size * ((w+3)/4) * ((h+3)/4);
	}

	return 	imgGetBytesPerPixel(format,channel) *
			CrCbAdjustResolution(w,channel) *
			CrCbAdjustResolution(h,channel)	;
}

int imgGetLinearSize( enum imgFormat format, int w, int h, int channel ) {
 
  return determineLinearSize(format, w, h, channel);
}

struct imgData imgGetPixel(const struct imgImage *img, int x, int y) {

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


int imgAllocImage(struct imgImage **img) {

	if(!img)
		return IMG_ERROR;

	if((*img = (struct imgImage*)calloc( 1, sizeof(struct imgImage))))
		return IMG_OKAY;

	return IMG_ERROR;
}

void imgFreeImage(struct imgImage *img) {

	free(img);
}

int imgAllocPixelBuffers(struct imgImage *img) {

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

int imgSetPixelBuffer(struct imgImage *img, void *buffer, int channel) {

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

int imgSetAllPixelBuffers(struct imgImage *img, void *buffer0, void* buffer1, void* buffer2, void* buffer3) {

	if(!img)
		return IMG_ERROR;

	imgSetPixelBuffer(img,buffer0,0);
	imgSetPixelBuffer(img,buffer1,1);
	imgSetPixelBuffer(img,buffer2,2);
	imgSetPixelBuffer(img,buffer3,3);

	return IMG_OKAY;
}

int imgFreePixelBuffers(struct imgImage *img) {

	int channel;

	if(!img)
		return IMG_ERROR;

	for(channel=0;channel<4;channel++)
		free( img->data.channel[channel] );

	return IMG_OKAY;
}

int imgReadFile(struct imgImage *img, const char* fn) {

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

int imgStatFile(struct imgImage *img, const char *fn) {

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

int imgWriteFile(struct imgImage *img, const char* fn) {

	int err = IMG_ERROR;

#ifdef WITH_DDS_WRITE
	if( (err = imgWriteImgDds( fn, img ) ) == IMG_OKAY )
			return err;
#endif

#ifdef WITH_CPI_WRITE
	if( (err = imgWriteImgCpi( fn, img ) ) == IMG_OKAY )
		return err;
#endif

#ifdef WITH_PNG_WRITE
	if( (err = imgWriteImgPng( fn, img ) ) == IMG_OKAY )
		return err;
#endif

#ifdef WITH_P6_WRITE
	if( (err = imgWriteImgP6( fn, img ) ) == IMG_OKAY )
		return err;
#endif

#ifdef WITH_TGA_WRITE
	if( (err = imgWriteImgTga( fn, img ) ) == IMG_OKAY )
		return err;
#endif

#ifdef WITH_BMP_WRITE
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
	int func##F (struct imgImage **img, const char* format, ...) { \
		int err; \
		va_list va; \
		va_start(va,format); \
		err = __format_and_call(&func, img, NULL,NULL, format, va); \
		va_end(va); \
		return err; \
	}

#define FUNC_F_P(func) \
	int func##F (struct imgImage *img, const char* format, ...) { \
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

