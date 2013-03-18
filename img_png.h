/*
 * img_png.h
 *
 *  Created on: Oct 26, 2011
 *      Author: cds
 */

#ifndef IMG_PNG_H_
#define IMG_PNG_H_

#include "libimg.h"

#if defined(__cplusplus)
extern "C" {
#endif

int imgStatImgPng(const char *filename, struct imgImage *img);
int imgReadImgPng(const char *filename, struct imgImage *img);
int imgWriteImgPng(const char *filename, struct imgImage *img);

#if defined(__cplusplus)
}
#endif

#endif /* IMG_PNG_H_ */
