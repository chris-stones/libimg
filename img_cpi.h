/*
 * img_cpi.h
 *
 *  Created on: Oct 27, 2011
 *      Author: cds
 */

#ifndef IMG_CPI_H_
#define IMG_CPI_H_

#include "libimg.h"

#ifdef __cplusplus
extern "C" {
#endif

int imgStatImgCpi(const char *filename, struct imgImage *img);
int imgReadImgCpi(const char *filename, struct imgImage *img);
int imgWriteImgCpi(const char *filename, struct imgImage *img);

#ifdef __cplusplus
}
#endif

#endif /* IMG_CPI_H_ */
