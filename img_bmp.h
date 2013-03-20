
#ifndef IMG_BMP_H_
#define IMG_BMP_H_

#include "libimg.h"

#if defined(__cplusplus)
extern "C" {
#endif

int imgStatImgBmp(const char *filename, struct imgImage *img);
int imgReadImgBmp(const char *filename, struct imgImage *img);
int imgWriteImgBmp(const char *filename, struct imgImage *img);

#if defined(__cplusplus)
}
#endif

#endif /* IMG_BMP_H_ */
