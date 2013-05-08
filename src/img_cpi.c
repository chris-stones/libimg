/*
 * img_cpi.c
 *
 *  Created on: Oct 27, 2011
 *      Author: cds
 */

#if defined(WITH_CPI_WRITE) || defined(WITH_CPI_READ)

#include "img_cpi.h"
#include <stdlib.h>
#include <stdio.h>

#include<sys/types.h>
#include<sys/stat.h>
#include<unistd.h>
#include<string.h>

#include<lz4.h>

#define CHUNKSIZE (8<<20)

struct imgCpiHeader {

	char magic[8];		// ".CPIIMG"
	int  version;
	int  width;
	int  height;
	int  format;
	int  linesize[4];
	int	 cdata_offset[4];
	int	 cdata_size[4];
};

#if defined(WITH_CPI_WRITE) || defined(WITH_CPI_READ)
static int CrCbAdjustResolution(int res,int channel) {

	switch(channel) {
	case 1:
	case 2:
		return (res+1)>>1;
	default:
		return res;
	}
}
#endif

#ifdef WITH_CPI_WRITE

int imgWriteImgCpi(const char *filename, struct imgImage *img) {

	int err = IMG_ERROR;

	int channel,channels;
	char *buffer = NULL;
	struct imgCpiHeader header;
	FILE *file;

	if(!img || !filename)
		return IMG_ERROR;

	if(imgCheckFileExtension(filename, ".cpi") != 0)
		return IMG_BADEXT;

	if(!(file = fopen(filename, "wb")))
		return IMG_NOFILE;

	if(!(buffer = malloc(sizeof (char *) * CHUNKSIZE))) {
		fclose(file);
		return IMG_ERROR;
	}

	memset(&header,0, sizeof header);
	memcpy(header.magic,".CPIIMG", 8);

	header.width = img->width;
	header.height = img->height;
	header.version = 0;
	header.format = img->format;

	channels = imgGetChannels(img->format);

	if( fwrite(&header,sizeof header, 1, file) != 1)
		goto exit_err;

	for(channel=0;channel<channels;channel++) {

		header.linesize[channel] =
				CrCbAdjustResolution(img->width,channel) *
					imgGetBytesPerPixel(img->format, channel);

		header.cdata_size[channel] =
				LZ4_compress(img->data.channel[channel],
						buffer,
						img->linesize[channel] *
							CrCbAdjustResolution(img->height,channel));

//		printf("compress %d -> %d\n",
//				img->linesize[channel] * CrCbAdjustResolution(img->height,channel),
//				header.cdata_size[channel]);

		if(channel) {
			header.cdata_offset[channel] =
					header.cdata_offset	[channel-1] +
					header.cdata_size	[channel-1] ;
		}
		else
			header.cdata_offset[channel] = sizeof header;

		if( fwrite(buffer,header.cdata_size[channel], 1, file) != 1 )
			goto exit_err;
	}

	fseek(file,0,SEEK_SET);

	if( fwrite(&header,sizeof header, 1, file) != 1)
		goto exit_err;

	err = IMG_OKAY;

exit_err:
	free(buffer);
	fclose(file);
	return err;
}

#endif /* WITH_CPI_WRITE */

#ifdef WITH_CPI_READ

static int read_cpi(const char *filename, struct imgImage *img, int read) {

	struct imgCpiHeader header;

	FILE *file = NULL;

	int err = IMG_OKAY;

	if(!filename || !img)
		return IMG_ERROR;

	if(imgCheckFileExtension(filename, ".cpi") != 0)
		return IMG_BADEXT;

	if(!(file = fopen(filename, "rb")))
		return IMG_NOFILE;

	if( fread(&header, sizeof header, 1, file) == 1 ) {

		if(strncmp(header.magic, ".CPIIMG", 8) != 0) {
			err = IMG_BADMAGIC;
		}
		else {
			img->width = header.width;
			img->height = header.height;
			img->format = header.format;
			img->linesize[0] = header.linesize[0];
			img->linesize[1] = header.linesize[1];
			img->linesize[2] = header.linesize[2];
			img->linesize[3] = header.linesize[3];

	//		printf(".cpi img:\n");
	//		printf("\tw: %d\n", img->width);
	//		printf("\th: %d\n", img->height);
	//		printf("\tl: %d\n", img->linesize[0]);
//			printf("\tf: %d\n", img->format);

			if(read) {

				int channels,channel;
				struct stat s;
				char *buff;

				stat(filename, &s);

				fseek(file,0,SEEK_SET);

				buff = (char*)malloc(s.st_size);

				if(!buff) {
					err = IMG_ERROR;
					fclose(file);
					return IMG_ERROR;
				}

				if( fread(buff, s.st_size, 1, file) != 1 ) {

					free(buff);
					fclose(file);
					return IMG_ERROR;
				}

				channels = imgGetChannels(img->format);

				for(channel=0;channel<channels;channel++)
					LZ4_uncompress(buff + header.cdata_offset[channel],
							img->data.channel[channel],
							img->linesize[channel] *
								CrCbAdjustResolution(img->height, channel));

				free(buff);
			}
		}
	}
	else
		err = IMG_BADMAGIC;


	fclose(file);
	return err;
}

int imgStatImgCpi(const char *filename, struct imgImage *img) {

	return read_cpi(filename, img, 0);
}

int imgReadImgCpi(const char *filename, struct imgImage *img) {

	return read_cpi(filename, img, 1);
}

#endif
#endif

