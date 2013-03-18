/*
 * img_tga.c
 *
 *  Created on: Sep 11, 2012
 *      Author: cds
 *
 *    __SHOULD__ be able to read and write uncompressed BBR24, BGRA32, and greyscale images.
 */


#include "libimg.h"

#include <string.h>
#include <assert.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <inttypes.h>


#define COLOUR_MAP_TYPE_NO_COLOUR_MAP 	0
#define COLOUR_MAP_TYPE_PRESENT			1

#define IMAGE_TYPE_UNCOMPRESSED_COLOUR_MAP	1
#define IMAGE_TYPE_RLE_TRUE_COLOUR 10
#define IMAGE_TYPE_UNCOMPRESSED_TRUE_COLOUR 2
#define IMAGE_TYPE_RLE_COLOUR_MAP 9

#define IMAGE_TYPE_NONE 0
#define IMAGE_TYPE_UNCOMPRESSED_GREYSCALE 3
#define IMAGE_TYPE_RLE_GREYSCALE 11

struct tga_header {

	unsigned char id_length;
	unsigned char colour_map_type;
	unsigned char image_type;
	unsigned short colour_map_spec_offset;
	unsigned short colour_map_length;
	unsigned char colour_map_bits_per_pixel;

	unsigned short x_origin;
	unsigned short y_origin;
	unsigned short width;
	unsigned short height;
	unsigned char bits_per_pixel;
	unsigned char image_descriptor;
}__attribute__((packed));

struct tga_extension {

	unsigned short extension_size; // always 495
	char author_name[41];
	char author_comment[324];
	char timestamp[12];
	char job_id[41];
	char job_time[6];
	char software_id[41];
	char software_version[3];
	char key_colour[4];
	int  pixel_aspect_ratio;
	int  gamma;
	int  colour_correction_offset;
	int  postage_stamp_offset;
	int  scanline_offset;
	char attributes_type; // Specifies the alpha channel

}__attribute__((packed));

struct tga_footer {

	unsigned int extension_offset;
	unsigned int developer_offset;
	char 		 sig[18]; /* "TRUEVISION-XFILE.\0" */
}__attribute__((packed));

#ifdef WITH_TGA_READ

// multiple of 32, on a 64bit cpu.
static int rle_read_32(unsigned char * in, int in_size, unsigned char * out, int out_size) {

	unsigned char len;

	while(out_size > 0) {

		if(in_size < 1)
			return IMG_ERROR;

		len = *(in++); --in_size;

		if(len & 0x80) {

			/*** RLE ***/

			uint32_t buffer32;
			uint64_t buffer64;

			len &= 0x7f;
			len++;

			if(in_size < 4)
				return IMG_ERROR;

			buffer32 = *(unsigned int *)in;
			buffer64 = buffer32;
			buffer64 = buffer64 << 8 | buffer32;
			in 		+= 4;
			in_size -= 4;

			if(sizeof(void*) == 8)
				while(len > 1) {

					if(out_size < 8)
						return IMG_ERROR;

					len 		-= 2;
					out 		+= 8;
					out_size	-= 8;

					*((uint64_t *)out) = buffer64;
				}


			while(len--) {

				if(out_size < 4)
					return IMG_ERROR;

				*((unsigned int *)out) = buffer32;
				out 		+= 4;
				out_size	-= 4;
			}
		}
		else {

			/*** RAW ***/

			int i = 4 * (len+1);

			if(out_size < i || in_size < i)
				return IMG_ERROR;

			memcpy(out, in, i);
			out += i;	out_size 	-= i;
			in  += i;	in_size		-= i;
		}
	}

	return IMG_OKAY;
}

static int rle_read(unsigned char * in, int in_size, unsigned char * out, int out_size, int bytes_per_pixel) {

	unsigned char len;

	if(bytes_per_pixel==4)
		return rle_read_32(in,in_size,out,out_size);

	assert(bytes_per_pixel <= 4);

	while(out_size > 0) {

		if(in_size < 1)
			return IMG_ERROR;

		len = *(in++); --in_size;

		if(len & 0x80) {

			/*** RLE ***/

			char buffer[4];

			len &= 0x7f;
			len++;

			if(in_size < bytes_per_pixel)
				return IMG_ERROR;

			memcpy(buffer, in, bytes_per_pixel);

			in 		+= bytes_per_pixel;
			in_size -= bytes_per_pixel;

			while(len--) {

				if(out_size < bytes_per_pixel)
					return IMG_ERROR;

				memcpy(out, buffer, bytes_per_pixel);
				out 		+= bytes_per_pixel;
				out_size	-= bytes_per_pixel;
			}
		}
		else {

			/*** RAW ***/

			int i = bytes_per_pixel * (len+1);

			if(out_size < i || in_size < i)
				return IMG_ERROR;

			memcpy(out, in, i);
			out += i;	out_size 	-= i;
			in  += i;	in_size		-= i;
		}
	}

	return IMG_OKAY;
}

static void set_colour_format(struct imgImage *img, int bpp) {

	switch(bpp)
	{
	case 16:	img->format = IMG_FMT_BGR16;	break;
	case 24:	img->format = IMG_FMT_BGR24;	break;
	case 32:	img->format = IMG_FMT_BGRA32;	break;
	}
	if(bpp==24)
		img->format = IMG_FMT_BGR24;
	else if(bpp==32)
		img->format = IMG_FMT_BGRA32;
}

static void set_grey_format(struct imgImage *img, int bpp) {

	if(bpp==8)
		img->format = IMG_FMT_GREY8;
	else if(bpp==16)
		img->format = IMG_FMT_GREY16;
}

static int _read_tga(struct imgImage *img, FILE *file, struct tga_header *header, struct tga_footer *footer, int read) {

	/* file is open, but at an undefined position */
	/* header exists, footer is NULL, or valid */
	int rle = 0;
	int map = 0;

	img->width = header->width;
	img->height = header->height;
	img->linesize[0] = header->width * (header->bits_per_pixel >> 3);
	img->linearsize[0] = img->linesize[0] * header->height;
	img->linesize[1] = img->linearsize[1] =
	img->linesize[2] = img->linearsize[2] =
	img->linesize[3] = img->linearsize[3] = 0;

	img->format = IMG_FMT_UNKNOWN;

	if(header->colour_map_type == COLOUR_MAP_TYPE_NO_COLOUR_MAP) {

		switch(header->image_type) {

		case IMAGE_TYPE_RLE_COLOUR_MAP:

			map = 1;
			rle = 1;
			set_colour_format(img, header->colour_map_bits_per_pixel);

			break;

		case IMAGE_TYPE_RLE_TRUE_COLOUR:
			rle = 1;
			// no break
		case IMAGE_TYPE_UNCOMPRESSED_TRUE_COLOUR:

			set_colour_format(img, header->bits_per_pixel);

			break;

		case IMAGE_TYPE_UNCOMPRESSED_COLOUR_MAP:

			map = 1;
			set_colour_format(img, header->colour_map_bits_per_pixel);

			break;

		case IMAGE_TYPE_RLE_GREYSCALE:

			rle = 1;

			// no break;

		case IMAGE_TYPE_UNCOMPRESSED_GREYSCALE:

			set_grey_format(img, header->bits_per_pixel);

			break;
		}
	}

	if(!read)
		return IMG_OKAY;

	if(img->format != IMG_FMT_UNKNOWN)
	{
		int map_offset =
			sizeof (struct tga_header) +
			header->id_length ;

		int data_offset =
			map_offset +
			header->colour_map_length;

		void * colour_map = NULL;
		if(map && header->colour_map_length)
			if(fseek(file, map_offset, SEEK_SET)==0)
				if((colour_map = malloc( header->colour_map_length )))
					if(fread(colour_map, header->colour_map_length, 1, file) != 1) {

						free(colour_map);
						colour_map = NULL;
					}

		if(fseek(file, data_offset, SEEK_SET)==0) {

			switch((map << 1) | rle) {

			case 0: /* !rle && !map */

				if(fread(img->data.channel[0], img->linearsize[0], 1, file)==1)
					return IMG_OKAY;

				break;

			case 1: /* rle && !map */
				{
					struct stat stat;

					if( fstat( fileno(file), &stat ) == 0 ) {

						int buffersize = stat.st_size - data_offset;

						void * buffer = malloc( buffersize );

						if( buffer && ( fread(buffer, buffersize, 1, file) == 1) ) {

							int err = rle_read(buffer, buffersize, img->data.channel[0], img->linearsize[0], header->bits_per_pixel >> 3 );

							free(buffer);

							return err;
						}
					}
				}
				break;

			case 2: /* map && !rle */

				free(colour_map);

				break;

			case 3: /* map && rle */

				free(colour_map);

				break;
			}
		}
	}

	return IMG_ERROR;
}

static int read_tga(const char *filename, struct imgImage *img, int read) {

	struct tga_header header;

	FILE *file = NULL;

	int err = IMG_ERROR;

	if(!filename || !img)
		return IMG_ERROR;

	if(imgCheckFileExtension(filename, ".tga") != 0)
		return IMG_BADEXT;

	if(!(file = fopen(filename, "rb")))
		return IMG_NOFILE;

	if( fread( &header, sizeof header, 1, file ) == 1) {

		struct tga_footer footer;

		if( fseek(file, sizeof footer, SEEK_END) == 0)
			if(fread(&footer, sizeof footer, 1, file ) == 1)
				if(strncmp(footer.sig, "TRUEVISION-XFILE.", 18 )==0) {

					err = _read_tga(img, file, &header, &footer, read);
					fclose(file);
					return err;
				}

		err = _read_tga(img, file,&header, NULL, read);
	}

	fclose(file);

	return err;
}


int imgStatImgTga(const char *filename, struct imgImage *img_data) {

	int err = read_tga(filename, img_data, 0);

	return err;
}

int imgReadImgTga(const char *filename, struct imgImage *img_data) {

	int err = read_tga(filename, img_data, 1);

	return err;
}

#endif /*** WITH_TGA_READ ***/

#ifdef WITH_TGA_WRITE

int imgWriteImgTga(const char *filename, struct imgImage *img) {

	int err = IMG_ERROR;

	struct tga_header header;

	if(imgCheckFileExtension(filename,".tga")!=0)
		return IMG_BADEXT;

	memset(&header,0,sizeof header);

	header.width = img->width;
	header.height = img->height;
	header.colour_map_type = COLOUR_MAP_TYPE_NO_COLOUR_MAP;

	switch( img->format ) {

	case IMG_FMT_BGR24:
		header.image_type = IMAGE_TYPE_UNCOMPRESSED_TRUE_COLOUR;
		header.bits_per_pixel = 24;
		break;
	case IMG_FMT_BGRA32:
		header.image_type = IMAGE_TYPE_UNCOMPRESSED_TRUE_COLOUR;
		header.bits_per_pixel = 32;
		header.image_descriptor = 8; // alpha depth... endianness?
		break;
	case IMG_FMT_GREY8:
		header.image_type = IMAGE_TYPE_UNCOMPRESSED_GREYSCALE;
		header.bits_per_pixel = 8;
		break;
	case IMG_FMT_GREY16:
		header.image_type = IMAGE_TYPE_UNCOMPRESSED_GREYSCALE;
		header.bits_per_pixel = 16;
		break;
	default:
		return IMG_ERROR;
	}

	{
		FILE *file;

		if(!(file = fopen(filename, "wb")))
			return IMG_NOFILE;

		if(fwrite(&header, sizeof header, 1, file) == 1)
		{
			unsigned char * p;
			unsigned char * t = (unsigned char *)img->data.channel[0];
			unsigned char * b = t + ((img->height-1) * img->linesize[0]);

			err = IMG_OKAY;

			for(p = b; p>=t; p-=img->linesize[0])
				if( fwrite(p, img->linesize[0], 1, file) != 1) {
					err = IMG_ERROR;
					break;
				}
		}

		fclose(file);
	}

	return err;
}
#endif /*** WRITE TGA ***/

