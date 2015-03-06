/*
 * png.c
 *
 *  Created on: Oct 25, 2011
 *      Author: cds
 */

#if defined(HAVE_CONFIG_H)
#include<config.h>
#endif

#include "libimg.h"

#include<stdlib.h>
#include<stdio.h>
#include<assert.h>
#include<string.h>

#if defined(WITH_PNG_READ) || defined(WITH_PNG_WRITE)
	#include<png.h>
#endif

#ifdef WITH_PNG_WRITE

enum imgFormat imgRecomendFormatPng(const char * filename, enum imgFormat hint, int allow_poorly_supported) {

	if(imgCheckFileExtension(filename,".png")!=0)
		return IMG_FMT_UNKNOWN;

	if(hint == IMG_FMT_UNKNOWN)
		return IMG_FMT_RGB24;

	if(hint & IMG_FMT_COMPONENT_ALPHA) {

		if((hint & IMG_FMT_COMPONENT_PACKED64) == IMG_FMT_COMPONENT_PACKED64)
			return IMG_FMT_RGBA64;

		return IMG_FMT_RGBA32;
	}
	else {
		if((hint & IMG_FMT_COMPONENT_PACKED48) == IMG_FMT_COMPONENT_PACKED48)
			return IMG_FMT_RGB48;

		return IMG_FMT_RGB24;
	}
}

int imgWriteImgPng(const char *filename, struct imgImage *img_data) {

	int bd,ct,y;
	FILE *file;
	png_structp png_ptr;
	png_infop info_ptr;
	png_bytep *rows = NULL;

	if(imgCheckFileExtension(filename,".png")!=0)
		return IMG_BADEXT;

	if(!(rows = (png_bytep*)malloc( sizeof(png_bytep) * img_data->height )))
		return IMG_ERROR;

	if(!img_data)
		return IMG_ERROR;

	switch(img_data->format)
	{
		default:
			free(rows);
			return IMG_BADFORMAT;

		case IMG_FMT_RGBA32:		bd = 8 ; ct = PNG_COLOR_TYPE_RGB_ALPHA;	break;
		case IMG_FMT_RGB24:			bd = 8 ; ct = PNG_COLOR_TYPE_RGB      ;	break;
		case IMG_FMT_RGBA64:		bd = 16; ct = PNG_COLOR_TYPE_RGB_ALPHA;	break;
		case IMG_FMT_RGB48:			bd = 16; ct = PNG_COLOR_TYPE_RGB      ;	break;
	}

	file = fopen(filename, "wb");
	if (!file) {
		free(rows);
		return IMG_ERROR;
	}

	png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
	info_ptr = png_create_info_struct(png_ptr);

	if (!info_ptr || !png_ptr) {
		png_destroy_write_struct(&png_ptr,&info_ptr);
		free(rows);
		return IMG_ERROR;
	}

	if (setjmp(png_jmpbuf(png_ptr))) {
		png_destroy_write_struct(&png_ptr,&info_ptr);
		free(rows);
		return IMG_ERROR;
	}

	for(y=0;y<img_data->height;y++)
		rows[y] = ((png_bytep)img_data->data.channel[0]) + (img_data->linesize[0] * y);

	png_init_io(png_ptr, file);

	png_set_IHDR(
		png_ptr,
		info_ptr,
		img_data->width,
		img_data->height,
		bd,
		ct,
		PNG_INTERLACE_NONE,
		PNG_COMPRESSION_TYPE_DEFAULT,
		PNG_FILTER_TYPE_DEFAULT);

	png_write_info(png_ptr, info_ptr);
	png_write_image(png_ptr, (png_bytepp)(rows));
	png_write_end(png_ptr, NULL);

	png_destroy_write_struct(&png_ptr,&info_ptr);
	fclose(file);

	free(rows);
	return 0;
}

#endif /*** WITH_PNG_WRITE ***/

#ifdef WITH_PNG_READ
static int read_png(const char *filename, struct imgImage *img_data, int read) {

	png_uint_32 w,h;
	int bd,ct;
	png_structp pPng;
	png_infop pPngInfo;
	FILE *file;

	if(imgCheckFileExtension(filename, ".png") != 0)
		return IMG_BADEXT;

	assert(img_data);

	file = fopen(filename,"rb");
	if (file==NULL)
		return IMG_NOFILE;

	char magic[4];

	if((fread(magic,4,1,file)!=1) || (strncmp(magic,"\211PNG",4) != 0)) {
		fclose(file);
		return IMG_BADMAGIC;
	}

	//create pPng and info_png
	if ((pPng = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL))==0) {
		fclose(file);
		return IMG_ERROR;
	}

	pPngInfo = png_create_info_struct(pPng);
	if (!pPngInfo) {
		fclose(file);
		png_destroy_read_struct(&pPng,NULL, NULL);
		return IMG_ERROR;
	}

	if (setjmp(png_jmpbuf(pPng))) {
		png_destroy_read_struct(&pPng,NULL,NULL);
		fclose(file);
		return IMG_ERROR;
	}

	fseek(file,8, SEEK_SET);
	png_init_io(pPng, file);
	png_set_sig_bytes(pPng, 8);
	png_read_info(pPng, pPngInfo);
	png_get_IHDR(pPng, pPngInfo, &w, &h, &bd, &ct,NULL, NULL, NULL);

	if(ct == PNG_COLOR_TYPE_PALETTE)
		png_set_palette_to_rgb(pPng);
	if(ct == PNG_COLOR_TYPE_GRAY && bd < 8)
		png_set_expand_gray_1_2_4_to_8(pPng);
	if(png_get_valid(pPng, pPngInfo, PNG_INFO_tRNS))
		png_set_tRNS_to_alpha(pPng);

	png_read_update_info(pPng, pPngInfo);
	
	ct = png_get_color_type(pPng, pPngInfo);
	bd = png_get_bit_depth(pPng, pPngInfo);

	img_data->height = (int)h;
	img_data->width = (int)w;
	if(bd == 8 && ct == PNG_COLOR_TYPE_RGB_ALPHA)
		img_data->format = IMG_FMT_RGBA32;
	else if(bd == 8 && ct == PNG_COLOR_TYPE_RGB)
		img_data->format = IMG_FMT_RGB24;
	else if(bd == 16 && ct == PNG_COLOR_TYPE_RGB_ALPHA)
		img_data->format = IMG_FMT_RGBA64;
	else if(bd == 16 && ct == PNG_COLOR_TYPE_RGB)
		img_data->format = IMG_FMT_RGB48;
	else if(bd == 8 && ct == PNG_COLOR_TYPE_GRAY)
		img_data->format = IMG_FMT_GREY8;
	else if(bd == 16 && ct == PNG_COLOR_TYPE_GRAY)
		img_data->format = IMG_FMT_GREY16;
	else if(bd == 8 && ct == PNG_COLOR_TYPE_GRAY_ALPHA)
		img_data->format = IMG_FMT_GREYA16;
	else if(bd == 16 && ct == PNG_COLOR_TYPE_GRAY_ALPHA)
		img_data->format = IMG_FMT_GREYA32;
	else if(ct == PNG_COLOR_TYPE_PALETTE)
		assert("unexpected palette" && 0);

	img_data->linesize[0] =
		png_get_rowbytes(pPng, pPngInfo);

	img_data->linearsize[0] =
		img_data->linesize[0] * img_data->height;


//	printf(".png img:\n");
//	printf("\tw: %d\n", img_data->width);
//	printf("\th: %d\n", img_data->height);
//	printf("\tl: %d\n", img_data->linesize[0]);
//	printf("\tf: %d\n", img_data->format);

	if(read) {
		int y;
		png_bytep *row_pointers = (png_bytep *)malloc(sizeof(png_bytep) * img_data->height );

		if(row_pointers) {

			for(y=0;y<img_data->height;y++)
				row_pointers[y] = ((png_bytep)(img_data->data.channel[0])) + y * (img_data->linesize[0]);

			png_read_image(pPng, row_pointers);
			png_read_end(pPng, NULL);

			free(row_pointers);
		}
		else {

			png_destroy_read_struct(&pPng, &pPngInfo, NULL);
			fclose(file);

			return IMG_ERROR;
		}
	}

	png_destroy_read_struct(&pPng, &pPngInfo, NULL);
	fclose(file);

	return IMG_OKAY;
}

int imgStatImgPng(const char *filename, struct imgImage *img_data) {

	return read_png(filename, img_data, 0);
}

int imgReadImgPng(const char *filename, struct imgImage *img_data) {

	return read_png(filename, img_data, 1);
}

#endif /*** WITH_PNG_READ ***/

