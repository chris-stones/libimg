
/**************************************************************************************************************
 * FIXME: This is a mess, I shoe-horned it in from another project.
 * 	It all needs testing, especially the version 4 stuff.
 * 
 * BGR24 should work.
 * 
 * An attempt has been made to support RGB24 BGR24 RGBA32 BGRA32 ARGB32 and ABGR32 via the version 4 header.
 * Still todo is 8bit and 16bit images.
 * 
 * It would appear that version-4 bitmap headers are poorly supported... stick to BGR24!
 *
 * I've probably got the endianness wrong on the version 4 BI_BITFIELD encoded images.
 * 
 * No attempt has been made to support JPEG, PNG, RLE, or any other formats that MSDN documents, but nobody uses.
 **************************************************************************************************************/

#include "img_config.h"

#if defined(WITH_BMP_READ) || defined(WITH_BMP_WRITE)
#define WITH_BMP
#endif

#include "libimg.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>

struct bmp_file_header {

	unsigned int size;
	short reserved1;
	short reserved2;
	unsigned int offset;
};

struct bmp_info_header_v1 {

	unsigned int header_size; // (12)
	  signed short width;
	  signed short height;
	unsigned short colour_planes;
	unsigned short bits_per_pixel;
};

struct bmp_info_header_v3 {

	unsigned int header_size; // (40)
	  signed int width;
	  signed int height;
	unsigned short colour_planes;
	unsigned short bits_per_pixel;
	unsigned int compression;
	unsigned int size;
	  signed int horizontal_resolution;
	  signed int verticle_resolution;
	unsigned int palette_colours;
	unsigned int important_colours;
};

struct bmp_info_header_v4 {
    
  unsigned int header_size;
  signed int width;
  signed int height;
  unsigned short colour_planes;
  unsigned short bits_per_pixel;
  unsigned int compression;
  unsigned int size;
  signed int horizontal_resolution;
  signed int verticle_resolution;
  unsigned int palette_colours;
  unsigned int important_colours;
  unsigned int red_mask;
  unsigned int green_mask;
  unsigned int blue_mask;
  unsigned int alpha_mask;
  unsigned int colour_space_type;
  long colour_space[9]; // TODO
  unsigned int red_gamma;
  unsigned int green_gamma;
  unsigned int blue_gamma;
};

enum bmp_info_v3_compression {

	BI_RGB=0,
	BI_RLE8=1,
	BI_RLE4=2,
	BI_BITFIELDS=3,
	BI_JPEG=4,
	BI_PNG=5,
};

typedef  enum 
{
  LCS_CALIBRATED_RGB = 0x00000000,
  LCS_sRGB = 0x73524742,
  LCS_WINDOWS_COLOR_SPACE = 0x57696E20
} LogicalColorSpace;


static unsigned int get_red_index( enum imgFormat fmt ) {
  
  switch(fmt) {
    case IMG_FMT_RGB24:		return 0;
    case IMG_FMT_BGR24:		return 2;
    case IMG_FMT_RGBA32:	return 0;
    case IMG_FMT_BGRA32:	return 2;
    case IMG_FMT_ARGB32:	return 1;
    case IMG_FMT_ABGR32:	return 3;
    default:
      return 4;
  }
}
static unsigned int get_green_index( enum imgFormat fmt ) {
  
  switch(fmt) {
    case IMG_FMT_RGB24:		return 1;
    case IMG_FMT_BGR24:		return 1;
    case IMG_FMT_RGBA32:	return 1;
    case IMG_FMT_BGRA32:	return 1;
    case IMG_FMT_ARGB32:	return 2;
    case IMG_FMT_ABGR32:	return 3;
    default:
      return 4;
  }
}
static unsigned int get_blue_index( enum imgFormat fmt ) {
  
  switch(fmt) {
    case IMG_FMT_RGB24:		return 2;
    case IMG_FMT_BGR24:		return 0;
    case IMG_FMT_RGBA32:	return 2;
    case IMG_FMT_BGRA32:	return 0;
    case IMG_FMT_ARGB32:	return 3;
    case IMG_FMT_ABGR32:	return 1;
    default:
      return 4;
  }
}
static unsigned int get_alpha_index( enum imgFormat fmt ) {
  
  switch(fmt) {
    case IMG_FMT_RGB24:		return 4;
    case IMG_FMT_BGR24:		return 4;
    case IMG_FMT_RGBA32:	return 3;
    case IMG_FMT_BGRA32:	return 3;
    case IMG_FMT_ARGB32:	return 0;
    case IMG_FMT_ABGR32:	return 0;
    default:
      return 4;
  }
}
static unsigned int get_mask( unsigned int index ) {

  static const unsigned int masks[] = {
    0x000000FF,
    0x0000FF00,
    0x00FF0000,
    0xFF000000,
    0x00000000,
  };
  return masks[index];
}

//static unsigned int get_mask_index(unsigned int mask) {
//
//  int i=0;
//  for(i=0;i<=4;i++)
//    if(get_mask(i)==mask)
//      return i;
//
//  return 5;
//}

static unsigned int get_red_mask	(enum imgFormat fmt) { return get_mask( get_red_index	(fmt) );  }
static unsigned int get_green_mask	(enum imgFormat fmt) { return get_mask( get_green_index	(fmt) );  }
static unsigned int get_blue_mask	(enum imgFormat fmt) { return get_mask( get_blue_index	(fmt) );  }
static unsigned int get_alpha_mask	(enum imgFormat fmt) { return get_mask( get_alpha_index	(fmt) );  }


static enum imgFormat get_masks_format( unsigned int red_mask, unsigned int green_mask, unsigned int blue_mask, unsigned int alpha_mask ) {

  int iformat;
  
  // TODO: add 16bit formats support
  enum imgFormat formats[] = {

    IMG_FMT_BGR24,
    IMG_FMT_RGB24,
    IMG_FMT_RGBA32,
    IMG_FMT_BGRA32,
    IMG_FMT_ARGB32,
    IMG_FMT_ABGR32,
  };

  for(iformat=0;iformat<((sizeof formats) /(sizeof formats[0]));iformat++) {

    enum imgFormat fmt = formats[iformat];

    if( red_mask != get_red_mask( fmt ) )
      continue;
    if( green_mask != get_green_mask( fmt ) )
     continue;
    if( blue_mask != get_blue_mask( fmt ) )
      continue;
    if( alpha_mask != get_alpha_mask( fmt ) )
      continue;

    return fmt;
  }

  return IMG_FMT_UNKNOWN;
}


/************************************************************************************************************************************************
	Write a microsoft bitmap version 1
************************************************************************************************************************************************/
/*
static int write_img_v1(FILE *file, struct imgImage *img, int row_bytes, int row_bytes_padded, int pad_bytes) {

	char buff[] = "BM0000000000";
	int y;
	int err=0;

	struct bmp_info_header_v1 info_header;
	struct bmp_file_header file_header;

	memset(&info_header, 0,sizeof info_header);
	memset(&file_header, 0,sizeof file_header);

	file_header.offset = 2 + sizeof file_header + sizeof info_header ;
//	if(img->palette)
//		file_header.offset += 768;
	file_header.size = file_header.offset +  row_bytes_padded * img->height;

	if(img->format == IMG_FMT_BGR24)
		info_header.bits_per_pixel = 24;
//	else if(img->pfmt == CGT_PIX_FMT_B8G8R8)
//		info_header.bits_per_pixel = 8;
	else
		return -1;

	info_header.header_size = sizeof info_header;
	info_header.width = img->width;
	info_header.height = img->height;
	info_header.colour_planes = 1;

	if(fwrite(buff,2,1,file) != 1)
		err = -1;
	else if(fwrite(&file_header, sizeof file_header, 1, file) != 1)
		err = -1;
	else if(fwrite(&info_header, sizeof info_header, 1, file) != 1)
		err = -1;
//	else if(img->palette) {
//		if(fwrite(img->palette, 3, 256, file) != 256)
//			err = -1;
//      }

	for(y=img->height-1; y>=0 && !err; --y) {
	  
	  void * src = ((char*)img->data.channel[0]) + (row_bytes * y);
	  
	  if(fwrite(src, row_bytes, 1, file) != 1)
	    err = -1;
	  else if(pad_bytes && (fwrite(buff,1,pad_bytes, file) != pad_bytes))
	    err = -1;
	}

	return err;
}
*/

/************************************************************************************************************************************************
	Write a microsoft bitmap version 3
************************************************************************************************************************************************/
#if defined(WITH_BMP_WRITE)
static int write_img_v3(FILE *file, struct imgImage *img, int row_bytes, int row_bytes_padded, int pad_bytes) {

	char buff[] = "BM0000000000";
	int y;
	int err = 0;

	struct bmp_info_header_v3 info_header;
	struct bmp_file_header file_header;
	
	if(img->format != IMG_FMT_BGR24)
	  return -1;
	
	memset(&info_header, 0,sizeof info_header);
	memset(&file_header, 0,sizeof file_header);

	file_header.offset = 2 + sizeof file_header + sizeof info_header ;
	file_header.size = file_header.offset +  row_bytes_padded * img->height;

	info_header.bits_per_pixel = imgGetBytesPerPixel(img->format,0) * 8;
	info_header.header_size = sizeof info_header;
	info_header.width = img->width;
	info_header.height = img->height;
	info_header.colour_planes = 1;
	info_header.compression = BI_RGB;
	info_header.size = row_bytes_padded * img->height;
	info_header.horizontal_resolution = 2835;
	info_header.verticle_resolution = 2835;

	if(fwrite(buff,2,1,file)!=1)
		err =-1;
	else if(fwrite(&file_header, sizeof file_header, 1, file) != 1)
		err = -1;
	else if(fwrite(&info_header, sizeof info_header, 1, file) != 1)
		err = -1;

	for(y=img->height-1; y>=0 && !err; --y) {

	  void * src = ((char*)img->data.channel[0]) + (img->linesize[0] * y);
	  
	  if(fwrite(src, row_bytes, 1, file) != 1)
	    err = -1;
	  else if(pad_bytes && (fwrite(buff,1,pad_bytes, file) != pad_bytes))
	    err = -1;
	}

	return err;
}

/************************************************************************************************************************************************
	Write a microsoft bitmap version 4
************************************************************************************************************************************************/
static int write_img_v4(FILE *file, struct imgImage *img, int row_bytes, int row_bytes_padded, int pad_bytes) {

	char buff[] = "BM0000000000";
	int y;
	int err = 0;

	struct bmp_info_header_v4 info_header;
	struct bmp_file_header file_header;
	
	memset(&info_header, 0,sizeof info_header);
	memset(&file_header, 0,sizeof file_header);

	file_header.offset = 2 + sizeof file_header + sizeof info_header ;
	file_header.size = file_header.offset +  row_bytes_padded * img->height;

	info_header.bits_per_pixel = imgGetBytesPerPixel(img->format,0) * 8;
	info_header.header_size = sizeof info_header;
	info_header.width = img->width;
	info_header.height = img->height;
	info_header.colour_planes = 1;
	info_header.size = row_bytes_padded * img->height;
	info_header.horizontal_resolution = 2835;
	info_header.verticle_resolution = 2835;
	info_header.colour_space_type = LCS_WINDOWS_COLOR_SPACE;
	
	// TODO: endienness.
	
	switch(img->format) {  
	  case IMG_FMT_BGR24:
	    info_header.compression = BI_RGB;
	    break;
	  case IMG_FMT_RGBA32:
	  case IMG_FMT_BGRA32:
	  case IMG_FMT_ARGB32:
	  case IMG_FMT_ABGR32:
	    
	    info_header.compression = BI_BITFIELDS;
	    info_header.red_mask 	= get_red_mask	(img->format);
	    info_header.green_mask 	= get_green_mask(img->format);
	    info_header.blue_mask 	= get_blue_mask	(img->format);
	    info_header.alpha_mask 	= get_alpha_mask(img->format);
	    break;
	  default:
	    // TODO: handle 16bit images.
		printf("libimg: write_img_v4: unsupported pixel format.\n");
	    return -1;
	}

	if(fwrite(buff,2,1,file)!=1) {
		err =-1;
		printf("libimg: write_img_v4: io error (buff,2,1,file)\n");
	}
	else if(fwrite(&file_header, sizeof file_header, 1, file) != 1) {
		err = -1;
		printf("libimg: write_img_v4: io error (&file_header, sizeof file_header, 1, file)\n");
	}
	else if(fwrite(&info_header, sizeof info_header, 1, file) != 1) {
		err = -1;
		printf("libimg: write_img_v4: io error (&info_header, sizeof info_header, 1, file)\n");
	}

	for(y=img->height-1; y>=0 && !err; --y) {

	  void * src = ((char*)img->data.channel[0]) + (img->linesize[0] * y);
	  
	  if(fwrite(src, row_bytes, 1, file) != 1) {
	    err = -1;
	    printf("libimg: write_img_v4: io error (src, row_bytes, 1, file)\n");
	  }
	  else if(pad_bytes && (fwrite(buff,1,pad_bytes, file) != pad_bytes)) {
	    err = -1;
	    printf("libimg: write_img_v4: io error (buff,1,pad_bytes, file)\n");
	  }
	}

	return err;
}

/************************************************************************************************************************************************
	Save Image
************************************************************************************************************************************************/
static int write_img(FILE *file, struct imgImage *img) {

	int pad_bytes = 0;
	
	if(img->linesize[0] % 4)
		pad_bytes = 4 - (img->linesize[0] % 4);

//	TODO: handle palette images - save as bitmap v1
//	if(img->pfmt == CGT_PIX_FMT_B8G8R8)
//		return write_img_v1(file, img, row_bytes, row_bytes_padded,pad_bytes);
	
	switch(img->format) {
	  case IMG_FMT_BGR24:
	    // version 3 is better supported ( i think? ) use it if possible.
	    return write_img_v3(file, img, img->linesize[0], img->linesize[0] + pad_bytes, pad_bytes);
	  default:
	    // else try version 4. i see no reason  to bother implementing version 5.
	    return write_img_v4(file, img, img->linesize[0], img->linesize[0] + pad_bytes, pad_bytes);
	}
	
	return -1;
}

enum imgFormat imgRecomendFormatBmp(const char * filename, enum imgFormat hint, int allow_poorly_supported) {

	if(imgCheckFileExtension(filename,".bmp")!=0)
		return IMG_FMT_UNKNOWN;

	if(hint == IMG_FMT_UNKNOWN)
		return IMG_FMT_BGR24;

	if(!allow_poorly_supported)
		return IMG_FMT_BGR24;

	if(hint & IMG_FMT_COMPONENT_ALPHA) {

		if(hint & IMG_FMT_COMPONENT_RGBA)
			return IMG_FMT_RGBA32;
		if(hint & IMG_FMT_COMPONENT_BGRA)
			return IMG_FMT_BGRA32;
		if(hint & IMG_FMT_COMPONENT_ARGB)
			return IMG_FMT_ARGB32;
		if(hint & IMG_FMT_COMPONENT_ABGR)
			return IMG_FMT_ABGR32;
	}
	else {
		if(hint & IMG_FMT_COMPONENT_RGBA)
			return IMG_FMT_RGB24;
		if(hint & IMG_FMT_COMPONENT_BGRA)
			return IMG_FMT_BGR24;
	}
	return IMG_FMT_BGR24;
}

/************************************************************************************************************************************************
	Save Image	
************************************************************************************************************************************************/
int imgWriteImgBmp(const char *filename, struct imgImage *img_data) {

      int err =0;
  
	FILE *file = NULL;
	
	if(imgCheckFileExtension(filename,".bmp")!=0)
		return IMG_BADEXT;

	if(!img_data)
		return -1;

#if defined(_MSC_VER)
	fopen_s(&file,filename,"wb");
#else
	file = fopen(filename, "wb");
#endif
	if (!file)
		return -1;

	err = write_img(file, img_data);

	fclose(file);

	return err;
}
#endif /*** DWITH_BMP_WRITE ***/


#if defined(WITH_BMP_READ)
/******************************************************************************************************************************
	read pixel data
******************************************************************************************************************************/
static
int cgt_readimg_bmp_px( FILE *file, struct imgImage *img, int bpp) {


	int y;
	int skip = 0;

	if(img->linesize[0] % 4)
		skip = 4 - (img->linesize[0] % 4);

	for(y=img->height-1; y>=0; --y) {
	  
		void * dst = ((char*)img->data.channel[0]) + (img->linesize[0] * y);

		if(fread(dst, img->linesize[0], 1, file)!=1)
			return -1;

		fseek(file, skip, SEEK_CUR);
	}

	return 0;
}

/******************************************************************************************************************************
	stat bitmaps with a 12 byte info header ( version 1 )
******************************************************************************************************************************/
static int cgt_statimg_bmp_v1( FILE *file, struct bmp_file_header* file_header, struct imgImage *_out ) {

	struct bmp_info_header_v1 info_header;

	if(fread( &info_header, sizeof info_header, 1, file ) != 1)
		return -1;

	switch(info_header.bits_per_pixel)
	{
	case 8:
	  // TODO:
	  break;
		
	case 24:

		_out->width = info_header.width;
		_out->height = info_header.height;
		_out->format = IMG_FMT_BGR24;
		return 0;

	default:
		break;
	}
	
	return -1;
}


/******************************************************************************************************************************
	read bitmaps with a 12 byte info header ( version 1 )
******************************************************************************************************************************/
static int cgt_readimg_bmp_v1( FILE *file, struct bmp_file_header* file_header, struct imgImage *img ) {

	int err = 0;

	struct bmp_info_header_v1 info_header;

	if(fread( &info_header, sizeof info_header, 1, file ) != 1)
		return -1;

	switch(info_header.bits_per_pixel)
	{
		case 8:
		{
		  // TODO: support palette images.
		  
		  break;
		}

		case 24:

			if((err = fseek(file, file_header->offset ,SEEK_SET)) != 0)
				return err;
			
			img->format = IMG_FMT_BGR24;
			img->width = info_header.width;
			img->height = info_header.height;

			return cgt_readimg_bmp_px( file, img, 3 );

		default:
			break;
	}
	return -1;
}

/******************************************************************************************************************************
	stat bitmaps with a 40 byte info header ( version 3 )
******************************************************************************************************************************/
static int cgt_statimg_bmp_v3(FILE *file, struct bmp_file_header* file_header, struct imgImage *img) {

	struct bmp_info_header_v3 info_header;

	if(fread( &info_header, sizeof info_header, 1, file ) != 1)
		return -1;

	switch(info_header.bits_per_pixel)
	{
	case  8:
		// TODO:
		break;

	case 24:

		img->format = IMG_FMT_BGR24;
		img->width = info_header.width;
		img->height = info_header.height;

		return 0;

	default:

		break;
	}
	return -1;
}

/******************************************************************************************************************************
	read bitmaps with a 40 byte info header ( version 3 )
******************************************************************************************************************************/
static int cgt_readimg_bmp_v3(FILE *file, struct bmp_file_header* file_header, struct imgImage *img) {

	int err = 0;

	struct bmp_info_header_v3 info_header;

	if(fread( &info_header, sizeof info_header, 1, file ) != 1)
		return -1;

	switch(info_header.bits_per_pixel)
	{
		case  8:
		{
		  // TODO:
		  break;
		}

		case 24:
	
		  if((err = fseek(file, file_header->offset ,SEEK_SET)) != 0)
			return err;
		  
		  img->format = IMG_FMT_BGR24;
		  img->width = info_header.width;
		  img->height = info_header.height;
	
		  return cgt_readimg_bmp_px( file, img, imgGetBytesPerPixel(img->format,0) );

		default:
			break;
	}
	
	return -1;
}
/******************************************************************************************************************************
	stat bitmaps with a 108 byte info header ( version 4 )
******************************************************************************************************************************/
static int cgt_statimg_bmp_v4(FILE *file, struct bmp_file_header* file_header, struct imgImage *img) {

	struct bmp_info_header_v4 info_header;

	if(fread( &info_header, sizeof info_header, 1, file ) != 1)
		return -1;

	switch(info_header.bits_per_pixel)
	{
	case  8:
		// TODO:
		break;
	case  16:
		// TODO:
		break;

	case 24:
	  
		img->format = IMG_FMT_UNKNOWN;

		if(info_header.compression == BI_RGB )
		  img->format = IMG_FMT_BGR24;
		else if(info_header.compression == BI_BITFIELDS )
		  img->format = get_masks_format( info_header.red_mask, info_header.green_mask, info_header.blue_mask, info_header.alpha_mask );
		  
		img->width = info_header.width;
		img->height = info_header.height;

		if(img->format == IMG_FMT_UNKNOWN)
		  break;
		
		if(imgGetBytesPerPixel(img->format,0) != 3)
		  break;
		
		return 0;
		
	case 32:

		img->format = IMG_FMT_UNKNOWN;

		if(info_header.compression == BI_RGB )
		  img->format = IMG_FMT_BGRX32;
		else if(info_header.compression == BI_BITFIELDS )
		  img->format = get_masks_format( info_header.red_mask, info_header.green_mask, info_header.blue_mask, info_header.alpha_mask );
		  
		img->width = info_header.width;
		img->height = info_header.height;

		if(img->format == IMG_FMT_UNKNOWN)
		  break;
		
		if(imgGetBytesPerPixel(img->format,0) != 4)
		  break;
		
		return 0;

	default:

		break;
	}
	return -1;
}

/******************************************************************************************************************************
	read bitmaps with a 108 byte info header ( version 4 )
******************************************************************************************************************************/
static int cgt_readimg_bmp_v4(FILE *file, struct bmp_file_header* file_header, struct imgImage *img) {

	int err = 0;

	struct bmp_info_header_v4 info_header;

	if(fread( &info_header, sizeof info_header, 1, file ) != 1)
		return -1;

	switch(info_header.bits_per_pixel)
	{
		case  8:
		{
		  // TODO:
		  break;
		}

		case 24:
	
		  if((err = fseek(file, file_header->offset ,SEEK_SET)) != 0)
			return err;
		  
		  img->format = IMG_FMT_UNKNOWN;

		  if(info_header.compression == BI_RGB )
		    img->format = IMG_FMT_BGR24;
		  else if(info_header.compression == BI_BITFIELDS )
		    img->format = get_masks_format( info_header.red_mask, info_header.green_mask, info_header.blue_mask, info_header.alpha_mask );
		    
		  img->width = info_header.width;
		  img->height = info_header.height;

		  if(img->format == IMG_FMT_UNKNOWN)
		    break;
		  
		  if(imgGetBytesPerPixel(img->format,0) != 3)
		    break;
	
		  return cgt_readimg_bmp_px( file, img, imgGetBytesPerPixel(img->format,0) );
		  
		  
		case 32:
		  
		  if((err = fseek(file, file_header->offset ,SEEK_SET)) != 0)
			return err;
		  
		  img->format = IMG_FMT_UNKNOWN;

		  if(info_header.compression == BI_RGB )
		    img->format = IMG_FMT_BGRX32;
		  else if(info_header.compression == BI_BITFIELDS )
		    img->format = get_masks_format( info_header.red_mask, info_header.green_mask, info_header.blue_mask, info_header.alpha_mask );
		    
		  img->width = info_header.width;
		  img->height = info_header.height;

		  if(img->format == IMG_FMT_UNKNOWN)
		    break;
		  
		  if(imgGetBytesPerPixel(img->format,0) != 4)
		    break;

	
		  return cgt_readimg_bmp_px( file, img, imgGetBytesPerPixel(img->format,0) );

		default:
			break;
	}
	
	return -1;
}
int imgStatImgBmp(const char *filename, struct imgImage *img) {

	int err_code = -1;
	FILE *file;
	short magic = 0;
	unsigned int header_size = 0;
	struct bmp_file_header file_header;

	if(imgCheckFileExtension(filename,".bmp")!=0)
	  return IMG_BADEXT;

#if defined(_MSC_VER)
	fopen_s(&file,filename,"rb");
#else
	file = fopen(filename, "rb");
#endif

	if(file==NULL)
		goto err;

	if(fread(&magic,2,1,file) != 1)
		goto err;

	if(magic != 0x4D42)
		goto err;

	if(fread(&file_header, sizeof file_header, 1, file) != 1)
		goto err;

	if(fread(&header_size, sizeof header_size, 1,file) != 1)
		goto err;

	fseek(file, 0x0E, SEEK_SET);

	switch(header_size) {
		case 12:
			err_code = cgt_statimg_bmp_v1( file, &file_header, img );
			break;
		case 40:
			err_code = cgt_statimg_bmp_v3( file, &file_header, img );
			break;
		default:
			err_code = cgt_statimg_bmp_v4( file, &file_header, img );
			break;
	};
    
err:
	if(file)
		fclose(file);
	
	img->linesize[0] = img->width * imgGetBytesPerPixel(img->format,0);
	img->linesize[1] = 0;
	img->linesize[2] = 0;
	img->linesize[3] = 0;
	
	img->linearsize[0] = img->linesize[0] * img->height;
	img->linearsize[1] = 0;
	img->linearsize[2] = 0;
	img->linearsize[3] = 0;

	return err_code;
}

int imgReadImgBmp(const char *filename, struct imgImage *img) {

	short magic = 0;
	struct bmp_file_header file_header;
	int err_code=-1;
	unsigned int header_size = 0;

	FILE *file = NULL;
	
	if(imgCheckFileExtension(filename,".bmp")!=0)
		return IMG_BADEXT;

	if(!img)
		goto err;

#if defined(_MSC_VER)
	fopen_s(&file,filename,"rb");
#else
	file = fopen(filename, "rb");
#endif

	if (file==NULL)
		goto err;
	
	if(fread(&magic,2,1,file) != 1)
		goto err;
	
	if(magic != 0x4D42)
		goto err;

	if(fread(&file_header, sizeof file_header, 1, file) != 1)
		goto err;

	if(fread(&header_size, sizeof header_size, 1,file) != 1)
		goto err;

	fseek(file, 0x0E, SEEK_SET);

	switch(header_size) {
		case 12:
			err_code = cgt_readimg_bmp_v1( file, &file_header, img );
			break;
		case 40:
			err_code = cgt_readimg_bmp_v3( file, &file_header, img );
			break;
		default:
			err_code = cgt_readimg_bmp_v4( file, &file_header, img );
			break;
	};

err:
	if(file)
		fclose(file);

	return err_code;
}

#endif /*** DWITH_BMP_READ ***/

