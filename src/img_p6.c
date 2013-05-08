/*
 * img_p6.c
 *
 *  Created on: Dec 28, 2012
 *      Author: cds
 */



#if defined(WITH_P6_WRITE) || defined(WITH_P6_READ)

#include "img_p6.h"
#include <stdlib.h>
#include <stdio.h>

#include<sys/types.h>
#include<sys/stat.h>
#include<unistd.h>
#include<string.h>

#if defined(WITH_P6_WRITE) || defined(WITH_P6_READ)
	static const char file_magick[] = {0x6d,0x68,0x77,0x61,0x6e,0x68,0x00,0x04};
#endif

#ifdef WITH_P6_WRITE

static const unsigned short magenta[] = {0x7c1f,0x7c1f,0x7c1f};
static const short bswapped_200 = 0xC800;

int imgWriteImgP6(const char *filename, struct imgImage *img_data) {

	FILE *file;
	unsigned short w=0;
	unsigned short h=0;
	int i;
	int y;
	int err=0;

	if(imgCheckFileExtension(filename,".bin")!=0)
		return IMG_BADEXT;

	if(!img_data)
		return -1;

	if (img_data->format != IMG_FMT_RGB15)
		return -2;

#if defined(_MSC_VER)
	fopen_s(&file,filename,"wb");
#else
	file = fopen(filename, "wb");
#endif

	if (!file)
		return -1;

	w = img_data->width;

	// file MUST be multiple of 4
	if(w%4)
		w+=4-(w%4);

	h = img_data->height;
	w = ((w>>8) | ((w&0xff)<<8));
	h = ((h>>8) | ((h&0xff)<<8));

	if(fseek(file, 0, SEEK_SET)!=0)
		err = -1;
	else if(fwrite(file_magick,8,1,file)!=1)	// file magic
		err = -1;
	else if(fwrite(&w, 2, 1, file)!=1)			// width (BIG ENDIAN)
		err = -1;
	else if(fwrite(&h, 2, 1, file)!=1)			// height (BIG ENDIAN)
		err = -1;
	else if(fwrite(&bswapped_200, sizeof bswapped_200, 1, file)!=1) // palette entries (BIG ENDIAN)
		err = -1;
	else if(fwrite(&bswapped_200, sizeof bswapped_200, 1, file)!=1) // horizontal DPI (BIG ENDIAN)
		err = -1;
	else if(fwrite(&bswapped_200, sizeof bswapped_200, 1, file)!=1) // Vertical DPI (BIG ENDIAN)
		err = -1;
	else {

		unsigned char * src_ptr =
				(unsigned char *)img_data->data.channel[0];

		/*** pad to address 800 ***/
		i = 800 - ftell(file);
		while(i--)
			if(fwrite("\0",1,1,file)!=1)
				err = -1;

		/*** write image data ***/
		for(y=0;y<img_data->height && !err;++y) {

			if(fwrite( src_ptr, 2, img_data->width, file ) != img_data->width)
				err = -1;
			else if(img_data->width%4) // magenta padding
				if(fwrite(magenta,2,4-(img_data->width%4),file) != 4-(img_data->width%4))
					err = -1;

			src_ptr += img_data->width * 2;
		}
	}

	fclose(file);

	return err;
}

#endif /* WITH_CPI_WRITE */

#ifdef WITH_P6_READ

static int read_p6(const char *filename, struct imgImage *img, int read) {

	char file_header[8] = {0,};

	unsigned short w=0;
	unsigned short h=0;
	int err=0;

	FILE *file;

	if(imgCheckFileExtension(filename,".bin")!=0)
		return IMG_BADEXT;

	if(!img || !filename)
		return -1;

#if defined(_MSC_VER)
	fopen_s(&file,filename,"rb");
#else
	file = fopen(filename, "rb");
#endif

	if (file==NULL)
		return -2;

	if(fread(file_header,8,1,file) != 1)
		err = -3;
	else if(memcmp(file_header, file_magick, sizeof file_magick))
		err = -4;
	else {

		if(fseek(file, 8, SEEK_SET) != 0)
			err = -5;
		else if(fread(&w, 2, 1, file) != 1)
			err = -6;
		else if(fread(&h, 2, 1, file) != 1)
			err = -7;
		else {

			img->width  = w = ((w>>8) | ((w&0xff)<<8));
			img->height = h = ((h>>8) | ((h&0xff)<<8));

			img->format = IMG_FMT_RGB15;

			img->linesize[0] = img->width * 2;
			img->linesize[1] =
			img->linesize[2] =
			img->linesize[3] = 0;

			img->linearsize[0] = img->linesize[0] * img->height;
			img->linearsize[1] =
			img->linearsize[2] =
			img->linearsize[3] = 0;

			if(read) {

				if(fseek(file, 800,SEEK_SET) != 0)
					err = -8;
				else if(fread(img->data.channel[0], 2, w * h,file)!=(w * h))
					err = -9;
			}

			//printf("img_p6==0 (%d,%d,%d,%d )\n", img->width, img->height, img->linesize[0], img->linearsize[0]);
		}
	}

	fclose(file);

	return err;
}

int imgStatImgP6(const char *filename, struct imgImage *img) {

	int err = read_p6(filename, img, 0);

//	if(err)
//		printf("read_p6(%s,%p,0)==%d\n", filename, img, err);

	return err;
}

int imgReadImgP6(const char *filename, struct imgImage *img) {

	int err = read_p6(filename, img, 1);

//	if(err)
//		printf("read_p6(%s,%p,1)==%d\n", filename, img, err);

	return err;
}

#endif
#endif

