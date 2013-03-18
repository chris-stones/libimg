
#ifndef IMG_P6_H_
#define IMG_P6_H_

#include "libimg.h"

#ifdef __cplusplus
extern "C" {
#endif

int imgStatImgP6(const char *filename, struct imgImage *img);
int imgReadImgP6(const char *filename, struct imgImage *img);
int imgWriteImgP6(const char *filename, struct imgImage *img);

#ifdef __cplusplus
}
#endif

#endif /* IMG_P6_H_ */

