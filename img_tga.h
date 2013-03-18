/*
 * img_tga.h
 *
 *  Created on: Sep 11, 2012
 *      Author: cds
 */

#ifndef IMG_TGA_H_
#define IMG_TGA_H_

int imgStatImgTga(const char *filename, struct imgImage *img);
int imgReadImgTga(const char *filename, struct imgImage *img);
int imgWriteImgTga(const char *filename, struct imgImage *img);

#endif /* IMG_TGA_H_ */
