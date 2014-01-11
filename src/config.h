/*
 * config.h
 *
 *  Created on: 11 Jan 2014
 *      Author: cds
 */

#ifndef CONFIG_H_
#define CONFIG_H_

// uncomment to add support for reading / writing following formats.

#define WITH_BMP_READ
#define WITH_BMP_WRITE
//#define WITH_CPI_READ    // my toy planar lz4 compressed format.
//#define WITH_CPI_WRITE
#define WITH_DDS_READ      // Microsoft Direct Draw Surface.
#define WITH_DDS_WRITE
//#define WITH_P6_READ
//#define WITH_P6_WRITE
#define WITH_PNG_READ      // Portable Network Graphics - requires libpng.
#define WITH_PNG_WRITE
#define WITH_TGA_READ      // Truevision TGA
#define WITH_TGA_WRITE

#endif /* CONFIG_H_ */

