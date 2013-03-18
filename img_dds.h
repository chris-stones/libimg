/*
 * img_dds.h
 *
 *  Created on: Mar 1, 2012
 *      Author: cds
 */

#ifndef IMG_DDS_H_
#define IMG_DDS_H_

#include "libimg.h"

#if defined(__cplusplus)
extern "C" {
#endif

int imgStatImgDds(const char *filename, struct imgImage *img);
int imgReadImgDds(const char *filename, struct imgImage *img);
int imgWriteImgDds(const char *filename, struct imgImage *img);

#if defined(__cplusplus)
}
#endif

#endif /* IMG_DDS_H_ */
