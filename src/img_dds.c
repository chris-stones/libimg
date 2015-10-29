/*
 * img_dds.cpp
 *
 *  Created on: Mar 1, 2012
 *      Author: cds
 */

#define COPY_DUBIOUS_IMAGEMAGICK_BEHAVIOUR 1

#include "img_config.h"

#ifndef LIBIMG_DDS_C_
#define LIBIMG_DDS_C_

#include "libimg.h"

#include<stdio.h>
#include<string.h>
#include<assert.h>

#ifdef WITH_DDS_READ
#define WITH_DDS
#else
#ifdef WITH_DDS_WRITE
#define WITH_DDS
#endif
#endif

typedef unsigned int DWORD;
typedef unsigned int UINT;

#define FOURCC(c1,c2,c3,c4) \
	(c1 | (c2 << 8) | (c3 << 16) | (c4 << 24))


/* msdn.microsoft.com/en-us/library/windows/desktop/bb943982(v=vs.85).aspx */
#define DDSD_CAPS			0x1	// Required in every .dds file.
#define DDSD_HEIGHT			0x2	// Required in every .dds file.
#define DDSD_WIDTH			0x4	// Required in every .dds file.
#define DDSD_PITCH			0x8	// Required when pitch is provided for an uncompressed texture.
#define DDSD_PIXELFORMAT	0x1000		// Required in every .dds file.
#define DDSD_MIPMAPCOUNT	0x20000		// Required in a mipmapped texture.
#define DDSD_LINEARSIZE		0x80000		// Required when pitch is provided for a compressed texture.
#define DDSD_DEPTH			0x800000	// Required in a depth texture.

#ifdef WITH_DDS_READ
static
const char * __DDSD_ToString(int ddsd_) {
    switch(ddsd_) {
    case DDSD_CAPS:
        return "DDSD_CAPS";
    case DDSD_HEIGHT:
        return "DDSD_HEIGHT";
    case DDSD_WIDTH:
        return "DDSD_WIDTH";
    case DDSD_PITCH:
        return "DDSD_PITCH";
    case DDSD_PIXELFORMAT:
        return "DDSD_PIXELFORMAT";
    case DDSD_MIPMAPCOUNT:
        return "DDSD_MIPMAPCOUNT";
    case DDSD_LINEARSIZE:
        return "DDSD_LINEARSIZE";
    case DDSD_DEPTH:
        return "DDSD_LINEARSIZE";
    default:
        return "DDSD_ ?UNKNOWN?";
    }
}
#endif

#ifdef WITH_DDS_READ
static
void DDSD_Print(int ddsd_) {

    if(ddsd_ & DDSD_CAPS) 			{
        printf(" %s ",__DDSD_ToString( DDSD_CAPS			));
    }
    if(ddsd_ & DDSD_HEIGHT)			{
        printf(" %s ",__DDSD_ToString( DDSD_HEIGHT		));
    }
    if(ddsd_ & DDSD_WIDTH) 			{
        printf(" %s ",__DDSD_ToString( DDSD_WIDTH			));
    }
    if(ddsd_ & DDSD_PITCH) 			{
        printf(" %s ",__DDSD_ToString( DDSD_PITCH			));
    }
    if(ddsd_ & DDSD_PIXELFORMAT) 	{
        printf(" %s ",__DDSD_ToString( DDSD_PIXELFORMAT	));
    }
    if(ddsd_ & DDSD_MIPMAPCOUNT) 	{
        printf(" %s ",__DDSD_ToString( DDSD_MIPMAPCOUNT	));
    }
    if(ddsd_ & DDSD_LINEARSIZE) 	{
        printf(" %s ",__DDSD_ToString( DDSD_LINEARSIZE	));
    }
    if(ddsd_ & DDSD_DEPTH) 			{
        printf(" %s ",__DDSD_ToString( DDSD_DEPTH			));
    }
}
#endif

#define DDSCAPS_COMPLEX					0x8
#define DDSCAPS_MIPMAP					0x400000
#define DDSCAPS_TEXTURE					0x1000
#define DDSCAPS2_CUBEMAP				0x200
#define DDSCAPS2_CUBEMAP_POSITIVEX		0x400
#define DDSCAPS2_CUBEMAP_NEGATIVEX		0x800
#define DDSCAPS2_CUBEMAP_POSITIVEY		0x1000
#define DDSCAPS2_CUBEMAP_NEGATIVEY		0x2000
#define DDSCAPS2_CUBEMAP_POSITIVEZ		0x4000
#define DDSCAPS2_CUBEMAP_NEGATIVEZ		0x8000
#define DDSCAPS2_VOLUME					0x200000

#ifdef WITH_DDS_READ
static
const char *DDSCAPS_ToString(int caps) {

    switch(caps) {
    case DDSCAPS_COMPLEX:
        return "DDSCAPS_COMPLEX";
    case DDSCAPS_MIPMAP:
        return "DDSCAPS_MIPMAP";
    case DDSCAPS_TEXTURE:
        return "DDSCAPS_TEXTURE";
    default:
        return "DDSCAPS_ ?UNKNOWN?";
    }
}
#endif

#if defined(WITH_DDS_READ)
static
void DDSCAPS_Print(int caps) {

    if(caps & DDSCAPS_COMPLEX) 		{
        printf(" %s ",DDSCAPS_ToString( DDSCAPS_COMPLEX	));
    }
    if(caps & DDSCAPS_MIPMAP)		{
        printf(" %s ",DDSCAPS_ToString( DDSCAPS_MIPMAP	));
    }
    if(caps & DDSCAPS_TEXTURE) 		{
        printf(" %s ",DDSCAPS_ToString( DDSCAPS_TEXTURE	));
    }
}
#endif

#ifdef WITH_DDS_READ
static
const char *DDSCAPS2_ToString(int caps) {

    switch(caps) {
    case DDSCAPS2_CUBEMAP:
        return "DDSCAPS2_CUBEMAP";
    case DDSCAPS2_CUBEMAP_POSITIVEX:
        return "DDSCAPS2_CUBEMAP_POSITIVEX";
    case DDSCAPS2_CUBEMAP_NEGATIVEX:
        return "DDSCAPS2_CUBEMAP_NEGATIVEX";
    case DDSCAPS2_CUBEMAP_POSITIVEY:
        return "DDSCAPS2_CUBEMAP_POSITIVEY";
    case DDSCAPS2_CUBEMAP_NEGATIVEY:
        return "DDSCAPS2_CUBEMAP_NEGATIVEY";
    case DDSCAPS2_CUBEMAP_POSITIVEZ:
        return "DDSCAPS2_CUBEMAP_POSITIVEZ";
    case DDSCAPS2_CUBEMAP_NEGATIVEZ:
        return "DDSCAPS2_CUBEMAP_NEGATIVEZ";
    case DDSCAPS2_VOLUME:
        return "DDSCAPS2_VOLUME";
    default:
        return "DDSCAPS_ ?UNKNOWN?";
    }
}
#endif

#ifdef WITH_DDS_READ
static
void DDSCAPS2_Print(int caps) {

    if(caps & DDSCAPS2_CUBEMAP) 			{
        printf(" %s ",DDSCAPS2_ToString( DDSCAPS_COMPLEX	));
    }
    if(caps & DDSCAPS2_VOLUME)				{
        printf(" %s ",DDSCAPS2_ToString( DDSCAPS2_VOLUME	));
    }
    if(caps & DDSCAPS2_CUBEMAP_POSITIVEX)	{
        printf(" %s ",DDSCAPS2_ToString( DDSCAPS2_CUBEMAP_POSITIVEX	));
    }
    if(caps & DDSCAPS2_CUBEMAP_NEGATIVEX)	{
        printf(" %s ",DDSCAPS2_ToString( DDSCAPS2_CUBEMAP_NEGATIVEX	));
    }
    if(caps & DDSCAPS2_CUBEMAP_POSITIVEY)	{
        printf(" %s ",DDSCAPS2_ToString( DDSCAPS2_CUBEMAP_POSITIVEY	));
    }
    if(caps & DDSCAPS2_CUBEMAP_NEGATIVEY)	{
        printf(" %s ",DDSCAPS2_ToString( DDSCAPS2_CUBEMAP_NEGATIVEY	));
    }
    if(caps & DDSCAPS2_CUBEMAP_POSITIVEZ)	{
        printf(" %s ",DDSCAPS2_ToString( DDSCAPS2_CUBEMAP_POSITIVEZ	));
    }
    if(caps & DDSCAPS2_CUBEMAP_NEGATIVEZ)	{
        printf(" %s ",DDSCAPS2_ToString( DDSCAPS2_CUBEMAP_NEGATIVEZ	));
    }
}
#endif

/* http://msdn.microsoft.com/en-us/library/windows/desktop/bb943984%28v=vs.85%29.aspx */
#define DDPF_ALPHAPIXELS	0x1
#define DDPF_ALPHA			0x2
#define DDPF_FOURCC			0x4
#define DDPF_RGB			0x40
#define DDPF_YUV			0x200
#define DDPF_LUMINANCE		0x20000

#ifdef WITH_DDS_READ
static
const char* __DDPF_ToString(int ddpf_) {

    switch(ddpf_) {
    case DDPF_ALPHAPIXELS:
        return "DDPF_ALPHAPIXELS";
    case DDPF_ALPHA:
        return "DDPF_ALPHA";
    case DDPF_FOURCC:
        return "DDPF_FOURCC";
    case DDPF_RGB:
        return "DDPF_RGB";
    case DDPF_YUV:
        return "DDPF_YUV";
    case DDPF_LUMINANCE:
        return "DDPF_LUMINANCE";
    default:
        return "DDPF_ ?UNKNOWN?";
    }
}
#endif

#ifdef WITH_DDS_READ
static
void DDPF_Print(int ddpf_) {

    if( ddpf_ & DDPF_ALPHAPIXELS )  {
        printf(" %s ",__DDPF_ToString( DDPF_ALPHAPIXELS	));
    }
    if( ddpf_ & DDPF_ALPHA )		{
        printf(" %s ",__DDPF_ToString( DDPF_ALPHA			));
    }
    if( ddpf_ & DDPF_FOURCC )		{
        printf(" %s ",__DDPF_ToString( DDPF_FOURCC		));
    }
    if( ddpf_ & DDPF_RGB )			{
        printf(" %s ",__DDPF_ToString( DDPF_RGB			));
    }
    if( ddpf_ & DDPF_YUV )			{
        printf(" %s ",__DDPF_ToString( DDPF_YUV			));
    }
    if( ddpf_ & DDPF_LUMINANCE )	{
        printf(" %s ",__DDPF_ToString( DDPF_LUMINANCE		));
    }
}
#endif

/* http://msdn.microsoft.com/en-us/library/windows/desktop/bb943984%28v=vs.85%29.aspx */
typedef struct {
    DWORD dwSize;
    DWORD dwFlags;
    DWORD dwFourCC;
    DWORD dwRGBBitCount;
    DWORD dwRBitMask;
    DWORD dwGBitMask;
    DWORD dwBBitMask;
    DWORD dwABitMask;
} DDS_PIXELFORMAT;

/* http://msdn.microsoft.com/en-us/library/windows/desktop/bb943982%28v=vs.85%29.aspx */
typedef struct {
    DWORD           dwSize;
    DWORD           dwFlags;
    DWORD           dwHeight;
    DWORD           dwWidth;
    DWORD           dwPitchOrLinearSize;
    DWORD           dwDepth;
    DWORD           dwMipMapCount;
    DWORD           dwReserved1[11];
    DDS_PIXELFORMAT ddspf;
    DWORD           dwCaps;
    DWORD           dwCaps2;
    DWORD           dwCaps3;
    DWORD           dwCaps4;
    DWORD           dwReserved2;
} DDS_HEADER;

/* http://msdn.microsoft.com/en-us/library/windows/desktop/bb173059%28v=vs.85%29.aspx */
typedef enum DXGI_FORMAT {
    DXGI_FORMAT_UNKNOWN                      = 0,
    DXGI_FORMAT_R32G32B32A32_TYPELESS        = 1,
    DXGI_FORMAT_R32G32B32A32_FLOAT           = 2,
    DXGI_FORMAT_R32G32B32A32_UINT            = 3,
    DXGI_FORMAT_R32G32B32A32_SINT            = 4,
    DXGI_FORMAT_R32G32B32_TYPELESS           = 5,
    DXGI_FORMAT_R32G32B32_FLOAT              = 6,
    DXGI_FORMAT_R32G32B32_UINT               = 7,
    DXGI_FORMAT_R32G32B32_SINT               = 8,
    DXGI_FORMAT_R16G16B16A16_TYPELESS        = 9,
    DXGI_FORMAT_R16G16B16A16_FLOAT           = 10,
    DXGI_FORMAT_R16G16B16A16_UNORM           = 11,
    DXGI_FORMAT_R16G16B16A16_UINT            = 12,
    DXGI_FORMAT_R16G16B16A16_SNORM           = 13,
    DXGI_FORMAT_R16G16B16A16_SINT            = 14,
    DXGI_FORMAT_R32G32_TYPELESS              = 15,
    DXGI_FORMAT_R32G32_FLOAT                 = 16,
    DXGI_FORMAT_R32G32_UINT                  = 17,
    DXGI_FORMAT_R32G32_SINT                  = 18,
    DXGI_FORMAT_R32G8X24_TYPELESS            = 19,
    DXGI_FORMAT_D32_FLOAT_S8X24_UINT         = 20,
    DXGI_FORMAT_R32_FLOAT_X8X24_TYPELESS     = 21,
    DXGI_FORMAT_X32_TYPELESS_G8X24_UINT      = 22,
    DXGI_FORMAT_R10G10B10A2_TYPELESS         = 23,
    DXGI_FORMAT_R10G10B10A2_UNORM            = 24,
    DXGI_FORMAT_R10G10B10A2_UINT             = 25,
    DXGI_FORMAT_R11G11B10_FLOAT              = 26,
    DXGI_FORMAT_R8G8B8A8_TYPELESS            = 27,
    DXGI_FORMAT_R8G8B8A8_UNORM               = 28,
    DXGI_FORMAT_R8G8B8A8_UNORM_SRGB          = 29,
    DXGI_FORMAT_R8G8B8A8_UINT                = 30,
    DXGI_FORMAT_R8G8B8A8_SNORM               = 31,
    DXGI_FORMAT_R8G8B8A8_SINT                = 32,
    DXGI_FORMAT_R16G16_TYPELESS              = 33,
    DXGI_FORMAT_R16G16_FLOAT                 = 34,
    DXGI_FORMAT_R16G16_UNORM                 = 35,
    DXGI_FORMAT_R16G16_UINT                  = 36,
    DXGI_FORMAT_R16G16_SNORM                 = 37,
    DXGI_FORMAT_R16G16_SINT                  = 38,
    DXGI_FORMAT_R32_TYPELESS                 = 39,
    DXGI_FORMAT_D32_FLOAT                    = 40,
    DXGI_FORMAT_R32_FLOAT                    = 41,
    DXGI_FORMAT_R32_UINT                     = 42,
    DXGI_FORMAT_R32_SINT                     = 43,
    DXGI_FORMAT_R24G8_TYPELESS               = 44,
    DXGI_FORMAT_D24_UNORM_S8_UINT            = 45,
    DXGI_FORMAT_R24_UNORM_X8_TYPELESS        = 46,
    DXGI_FORMAT_X24_TYPELESS_G8_UINT         = 47,
    DXGI_FORMAT_R8G8_TYPELESS                = 48,
    DXGI_FORMAT_R8G8_UNORM                   = 49,
    DXGI_FORMAT_R8G8_UINT                    = 50,
    DXGI_FORMAT_R8G8_SNORM                   = 51,
    DXGI_FORMAT_R8G8_SINT                    = 52,
    DXGI_FORMAT_R16_TYPELESS                 = 53,
    DXGI_FORMAT_R16_FLOAT                    = 54,
    DXGI_FORMAT_D16_UNORM                    = 55,
    DXGI_FORMAT_R16_UNORM                    = 56,
    DXGI_FORMAT_R16_UINT                     = 57,
    DXGI_FORMAT_R16_SNORM                    = 58,
    DXGI_FORMAT_R16_SINT                     = 59,
    DXGI_FORMAT_R8_TYPELESS                  = 60,
    DXGI_FORMAT_R8_UNORM                     = 61,
    DXGI_FORMAT_R8_UINT                      = 62,
    DXGI_FORMAT_R8_SNORM                     = 63,
    DXGI_FORMAT_R8_SINT                      = 64,
    DXGI_FORMAT_A8_UNORM                     = 65,
    DXGI_FORMAT_R1_UNORM                     = 66,
    DXGI_FORMAT_R9G9B9E5_SHAREDEXP           = 67,
    DXGI_FORMAT_R8G8_B8G8_UNORM              = 68,
    DXGI_FORMAT_G8R8_G8B8_UNORM              = 69,
    DXGI_FORMAT_BC1_TYPELESS                 = 70,
    DXGI_FORMAT_BC1_UNORM                    = 71,
    DXGI_FORMAT_BC1_UNORM_SRGB               = 72,
    DXGI_FORMAT_BC2_TYPELESS                 = 73,
    DXGI_FORMAT_BC2_UNORM                    = 74,
    DXGI_FORMAT_BC2_UNORM_SRGB               = 75,
    DXGI_FORMAT_BC3_TYPELESS                 = 76,
    DXGI_FORMAT_BC3_UNORM                    = 77,
    DXGI_FORMAT_BC3_UNORM_SRGB               = 78,
    DXGI_FORMAT_BC4_TYPELESS                 = 79,
    DXGI_FORMAT_BC4_UNORM                    = 80,
    DXGI_FORMAT_BC4_SNORM                    = 81,
    DXGI_FORMAT_BC5_TYPELESS                 = 82,
    DXGI_FORMAT_BC5_UNORM                    = 83,
    DXGI_FORMAT_BC5_SNORM                    = 84,
    DXGI_FORMAT_B5G6R5_UNORM                 = 85,
    DXGI_FORMAT_B5G5R5A1_UNORM               = 86,
    DXGI_FORMAT_B8G8R8A8_UNORM               = 87,
    DXGI_FORMAT_B8G8R8X8_UNORM               = 88,
    DXGI_FORMAT_R10G10B10_XR_BIAS_A2_UNORM   = 89,
    DXGI_FORMAT_B8G8R8A8_TYPELESS            = 90,
    DXGI_FORMAT_B8G8R8A8_UNORM_SRGB          = 91,
    DXGI_FORMAT_B8G8R8X8_TYPELESS            = 92,
    DXGI_FORMAT_B8G8R8X8_UNORM_SRGB          = 93,
    DXGI_FORMAT_BC6H_TYPELESS                = 94,
    DXGI_FORMAT_BC6H_UF16                    = 95,
    DXGI_FORMAT_BC6H_SF16                    = 96,
    DXGI_FORMAT_BC7_TYPELESS                 = 97,
    DXGI_FORMAT_BC7_UNORM                    = 98,
    DXGI_FORMAT_BC7_UNORM_SRGB               = 99,
    DXGI_FORMAT_AYUV                         = 100,
    DXGI_FORMAT_Y410                         = 101,
    DXGI_FORMAT_Y416                         = 102,
    DXGI_FORMAT_NV12                         = 103,
    DXGI_FORMAT_P010                         = 104,
    DXGI_FORMAT_P016                         = 105,
    DXGI_FORMAT_420_OPAQUE                   = 106,
    DXGI_FORMAT_YUY2                         = 107,
    DXGI_FORMAT_Y210                         = 108,
    DXGI_FORMAT_Y216                         = 109,
    DXGI_FORMAT_NV11                         = 110,
    DXGI_FORMAT_AI44                         = 111,
    DXGI_FORMAT_IA44                         = 112,
    DXGI_FORMAT_P8                           = 113,
    DXGI_FORMAT_A8P8                         = 114,
    DXGI_FORMAT_B4G4R4A4_UNORM               = 115,
    DXGI_FORMAT_FORCE_UINT                   = 0xffffffffUL
} DXGI_FORMAT;

#ifdef WITH_DDS_READ
static const char * const DXGI_FORMAT_names[] = {
    "DXGI_FORMAT_UNKNOWN",//                      = 0,
    "DXGI_FORMAT_R32G32B32A32_TYPELESS",//        = 1,
    "DXGI_FORMAT_R32G32B32A32_FLOAT",//           = 2,
    "DXGI_FORMAT_R32G32B32A32_UINT",//            = 3,
    "DXGI_FORMAT_R32G32B32A32_SINT",//            = 4,
    "DXGI_FORMAT_R32G32B32_TYPELESS",//           = 5,
    "DXGI_FORMAT_R32G32B32_FLOAT",//              = 6,
    "DXGI_FORMAT_R32G32B32_UINT",//               = 7,
    "DXGI_FORMAT_R32G32B32_SINT",//               = 8,
    "DXGI_FORMAT_R16G16B16A16_TYPELESS",//        = 9,
    "DXGI_FORMAT_R16G16B16A16_FLOAT",//           = 10,
    "DXGI_FORMAT_R16G16B16A16_UNORM",//           = 11,
    "DXGI_FORMAT_R16G16B16A16_UINT",//            = 12,
    "DXGI_FORMAT_R16G16B16A16_SNORM",//           = 13,
    "DXGI_FORMAT_R16G16B16A16_SINT",//            = 14,
    "DXGI_FORMAT_R32G32_TYPELESS",//              = 15,
    "DXGI_FORMAT_R32G32_FLOAT",//                 = 16,
    "DXGI_FORMAT_R32G32_UINT",//                  = 17,
    "DXGI_FORMAT_R32G32_SINT",//                  = 18,
    "DXGI_FORMAT_R32G8X24_TYPELESS",//            = 19,
    "DXGI_FORMAT_D32_FLOAT_S8X24_UINT",//         = 20,
    "DXGI_FORMAT_R32_FLOAT_X8X24_TYPELESS",//     = 21,
    "DXGI_FORMAT_X32_TYPELESS_G8X24_UINT",//      = 22,
    "DXGI_FORMAT_R10G10B10A2_TYPELESS",//         = 23,
    "DXGI_FORMAT_R10G10B10A2_UNORM",//            = 24,
    "DXGI_FORMAT_R10G10B10A2_UINT",//             = 25,
    "DXGI_FORMAT_R11G11B10_FLOAT",//              = 26,
    "DXGI_FORMAT_R8G8B8A8_TYPELESS",//            = 27,
    "DXGI_FORMAT_R8G8B8A8_UNORM",//               = 28,
    "DXGI_FORMAT_R8G8B8A8_UNORM_SRGB",//          = 29,
    "DXGI_FORMAT_R8G8B8A8_UINT",//                = 30,
    "DXGI_FORMAT_R8G8B8A8_SNORM",//               = 31,
    "DXGI_FORMAT_R8G8B8A8_SINT",//                = 32,
    "DXGI_FORMAT_R16G16_TYPELESS",//              = 33,
    "DXGI_FORMAT_R16G16_FLOAT",//                 = 34,
    "DXGI_FORMAT_R16G16_UNORM",//                 = 35,
    "DXGI_FORMAT_R16G16_UINT",//                  = 36,
    "DXGI_FORMAT_R16G16_SNORM",//                 = 37,
    "DXGI_FORMAT_R16G16_SINT",//                  = 38,
    "DXGI_FORMAT_R32_TYPELESS",//                 = 39,
    "DXGI_FORMAT_D32_FLOAT",//                    = 40,
    "DXGI_FORMAT_R32_FLOAT",//                    = 41,
    "DXGI_FORMAT_R32_UINT",//                     = 42,
    "DXGI_FORMAT_R32_SINT",//                     = 43,
    "DXGI_FORMAT_R24G8_TYPELESS",//               = 44,
    "DXGI_FORMAT_D24_UNORM_S8_UINT",//            = 45,
    "DXGI_FORMAT_R24_UNORM_X8_TYPELESS",//        = 46,
    "DXGI_FORMAT_X24_TYPELESS_G8_UINT",//         = 47,
    "DXGI_FORMAT_R8G8_TYPELESS",//                = 48,
    "DXGI_FORMAT_R8G8_UNORM",//                   = 49,
    "DXGI_FORMAT_R8G8_UINT",//                    = 50,
    "DXGI_FORMAT_R8G8_SNORM",//                   = 51,
    "DXGI_FORMAT_R8G8_SINT",//                    = 52,
    "DXGI_FORMAT_R16_TYPELESS",//                 = 53,
    "DXGI_FORMAT_R16_FLOAT",//                    = 54,
    "DXGI_FORMAT_D16_UNORM",//                    = 55,
    "DXGI_FORMAT_R16_UNORM",//                    = 56,
    "DXGI_FORMAT_R16_UINT",//                     = 57,
    "DXGI_FORMAT_R16_SNORM",//                    = 58,
    "DXGI_FORMAT_R16_SINT",//                     = 59,
    "DXGI_FORMAT_R8_TYPELESS",//                  = 60,
    "DXGI_FORMAT_R8_UNORM",//                     = 61,
    "DXGI_FORMAT_R8_UINT",//                      = 62,
    "DXGI_FORMAT_R8_SNORM",//                     = 63,
    "DXGI_FORMAT_R8_SINT",//                      = 64,
    "DXGI_FORMAT_A8_UNORM",//                     = 65,
    "DXGI_FORMAT_R1_UNORM",//                     = 66,
    "DXGI_FORMAT_R9G9B9E5_SHAREDEXP",//           = 67,
    "DXGI_FORMAT_R8G8_B8G8_UNORM",//              = 68,
    "DXGI_FORMAT_G8R8_G8B8_UNORM",//              = 69,
    "DXGI_FORMAT_BC1_TYPELESS",//                 = 70,
    "DXGI_FORMAT_BC1_UNORM",//                    = 71,
    "DXGI_FORMAT_BC1_UNORM_SRGB",//               = 72,
    "DXGI_FORMAT_BC2_TYPELESS",//                 = 73,
    "DXGI_FORMAT_BC2_UNORM",//                    = 74,
    "DXGI_FORMAT_BC2_UNORM_SRGB",//               = 75,
    "DXGI_FORMAT_BC3_TYPELESS",//                 = 76,
    "DXGI_FORMAT_BC3_UNORM",//                    = 77,
    "DXGI_FORMAT_BC3_UNORM_SRGB",//               = 78,
    "DXGI_FORMAT_BC4_TYPELESS",//                 = 79,
    "DXGI_FORMAT_BC4_UNORM",//                    = 80,
    "DXGI_FORMAT_BC4_SNORM",//                    = 81,
    "DXGI_FORMAT_BC5_TYPELESS",//                 = 82,
    "DXGI_FORMAT_BC5_UNORM",//                    = 83,
    "DXGI_FORMAT_BC5_SNORM",//                    = 84,
    "DXGI_FORMAT_B5G6R5_UNORM",//                 = 85,
    "DXGI_FORMAT_B5G5R5A1_UNORM",//               = 86,
    "DXGI_FORMAT_B8G8R8A8_UNORM",//               = 87,
    "DXGI_FORMAT_B8G8R8X8_UNORM",//               = 88,
    "DXGI_FORMAT_R10G10B10_XR_BIAS_A2_UNORM",//   = 89,
    "DXGI_FORMAT_B8G8R8A8_TYPELESS",//            = 90,
    "DXGI_FORMAT_B8G8R8A8_UNORM_SRGB",//          = 91,
    "DXGI_FORMAT_B8G8R8X8_TYPELESS",//            = 92,
    "DXGI_FORMAT_B8G8R8X8_UNORM_SRGB",//          = 93,
    "DXGI_FORMAT_BC6H_TYPELESS",//                = 94,
    "DXGI_FORMAT_BC6H_UF16",//                    = 95,
    "DXGI_FORMAT_BC6H_SF16",//                    = 96,
    "DXGI_FORMAT_BC7_TYPELESS",//                 = 97,
    "DXGI_FORMAT_BC7_UNORM",//                    = 98,
    "DXGI_FORMAT_BC7_UNORM_SRGB",//               = 99,
    "DXGI_FORMAT_AYUV",//                         = 100,
    "DXGI_FORMAT_Y410",//                         = 101,
    "DXGI_FORMAT_Y416",//                         = 102,
    "DXGI_FORMAT_NV12",//                         = 103,
    "DXGI_FORMAT_P010",//                         = 104,
    "DXGI_FORMAT_P016",//                         = 105,
    "DXGI_FORMAT_420_OPAQUE",//                   = 106,
    "DXGI_FORMAT_YUY2",//                         = 107,
    "DXGI_FORMAT_Y210",//                         = 108,
    "DXGI_FORMAT_Y216",//                         = 109,
    "DXGI_FORMAT_NV11",//                         = 110,
    "DXGI_FORMAT_AI44",//                         = 111,
    "DXGI_FORMAT_IA44",//                         = 112,
    "DXGI_FORMAT_P8",//                           = 113,
    "DXGI_FORMAT_A8P8",//                         = 114,
    "DXGI_FORMAT_B4G4R4A4_UNORM",//               = 115,
};
#endif

#ifdef WITH_DDS_READ
static
const char* DXGI_FORMAT_ToString(int format) {

    if(format >=0 && format<=115)
        return DXGI_FORMAT_names[format];

    return "DGGI_FORMAT_ ?OVERFLOW?";
}
#endif

/* http://msdn.microsoft.com/en-us/library/windows/desktop/bb172411%28v=vs.85%29.aspx */
typedef enum D3D10_RESOURCE_DIMENSION {
    D3D10_RESOURCE_DIMENSION_UNKNOWN     = 0,
    D3D10_RESOURCE_DIMENSION_BUFFER      = 1,
    D3D10_RESOURCE_DIMENSION_TEXTURE1D   = 2,
    D3D10_RESOURCE_DIMENSION_TEXTURE2D   = 3,
    D3D10_RESOURCE_DIMENSION_TEXTURE3D   = 4
} D3D10_RESOURCE_DIMENSION;

/* http://msdn.microsoft.com/en-us/library/windows/desktop/bb943983%28v=vs.85%29.aspx */
typedef struct {
    DXGI_FORMAT              dxgiFormat;
    D3D10_RESOURCE_DIMENSION resourceDimension;
    UINT                     miscFlag;
    UINT                     arraySize;
    UINT                     reserved;
} DDS_HEADER_DXT10;

#ifdef WITH_DDS_READ
static
int block_size(const DDS_HEADER *header, const DDS_HEADER_DXT10 *header10) {

    if(header10) {
        switch(header10->dxgiFormat) {
        default:
            break;
        case DXGI_FORMAT_BC1_UNORM:
        case DXGI_FORMAT_BC1_UNORM_SRGB:
        case DXGI_FORMAT_BC1_TYPELESS:
            return 8;
        case DXGI_FORMAT_BC2_UNORM:
        case DXGI_FORMAT_BC2_UNORM_SRGB:
        case DXGI_FORMAT_BC2_TYPELESS:
        case DXGI_FORMAT_BC3_UNORM:
        case DXGI_FORMAT_BC3_UNORM_SRGB:
        case DXGI_FORMAT_BC3_TYPELESS:
            return 16;
        case DXGI_FORMAT_BC4_UNORM:
        case DXGI_FORMAT_BC4_SNORM:
        case DXGI_FORMAT_BC4_TYPELESS:
            return 8;
        case DXGI_FORMAT_BC5_UNORM:
        case DXGI_FORMAT_BC5_SNORM:
        case DXGI_FORMAT_BC5_TYPELESS:
        case DXGI_FORMAT_BC6H_TYPELESS:
        case DXGI_FORMAT_BC6H_UF16:
        case DXGI_FORMAT_BC6H_SF16:
        case DXGI_FORMAT_BC7_TYPELESS:
        case DXGI_FORMAT_BC7_UNORM:
        case DXGI_FORMAT_BC7_UNORM_SRGB:
            return 16;
        }
    }
    else if(header->ddspf.dwFlags & DDPF_FOURCC) {

        switch(header->ddspf.dwFourCC) {

        case FOURCC('D','X','T','1'):
            return 8;
        case FOURCC('D','X','T','2'):
        case FOURCC('D','X','T','3'):
        case FOURCC('D','X','T','4'):
        case FOURCC('D','X','T','5'):
            return 16;
        }
    }

    return 0; // NOT a block format
}
#endif

/****
http://msdn.microsoft.com/en-us/library/windows/desktop/bb943991%28v=vs.85%29.aspx

The D3DX library (for example, D3DX11.lib) and other similar libraries unreliably or inconsistently provide the pitch value in the dwPitchOrLinearSize member of the DDS_HEADER structure. Therefore, when you read and write to DDS files, we recommend that you compute the pitch in one of the following ways for the indicated formats:

For block-compressed formats, compute the pitch as:
	max( 1, ((width+3)/4) ) * block-size
The block-size is 8 bytes for DXT1, BC1, and BC4 formats, and 16 bytes for other block-compressed formats.
For R8G8_B8G8, G8R8_G8B8, legacy UYVY-packed, and legacy YUY2-packed formats, compute the pitch as:
	((width+1) >> 1) * 4
For other formats, compute the pitch as:
	( width * bits-per-pixel + 7 ) / 8
You divide by 8 for byte alignment.

Note  The pitch value that you calculate does not always equal the pitch that the runtime supplies, which is DWORD-aligned in some situations and byte-aligned in other situations. Therefore, we recommend that you copy a scan line at a time rather than try to copy the whole image in one copy.
*/

#ifdef WITH_DDS_READ
static
int getPitch(const DDS_HEADER *header, const DDS_HEADER_DXT10 *header10) {

    // Block compressed formats
    {
        int blockSize = block_size(header,header10);
        if(blockSize) {
            int w = ((header->dwWidth+3)/4);
            if(w<1)
                w = 1;
            return w * blockSize;
        }
    }

    // R8G8_B8G8, G8R8_G8B8, legacy UYVY-packed, and legacy YUY2-packed formats
    {
        int match = 0;
        if(header10) {
            switch(header10->dxgiFormat) {
            default:
                break;
            case DXGI_FORMAT_R8G8_B8G8_UNORM:
            case DXGI_FORMAT_G8R8_G8B8_UNORM:
                match = 1;
                break;
            }
        }

        if(match || (header->ddspf.dwFlags & DDPF_YUV))
            return ((header->dwWidth+1)>>1) * 4;
    }

    // Other formats
    {
        if(header->ddspf.dwFlags & (DDPF_RGB | DDPF_LUMINANCE | DDPF_YUV))
            return ((header->dwWidth * header->ddspf.dwRGBBitCount) + 7) / 8;
    }

    return 0;
}
#endif

#ifdef WITH_DDS_READ
static
int determineLinearSize(const DDS_HEADER *header, const DDS_HEADER_DXT10 *header10) {

    // Block compressed formats
    {
        int blockSize = block_size(header,header10);
        if(blockSize) {
            int w = ((header->dwWidth+3)/4);
            int h = ((header->dwHeight+3)/4);
            if(w<1) w = 1;
            if(h<1) h = 1;
            return w * h * blockSize;
        }
    }

    // R8G8_B8G8, G8R8_G8B8, legacy UYVY-packed, and legacy YUY2-packed formats
    {
        int match = 0;
        if(header10) {
            switch(header10->dxgiFormat) {
            default:
                break;
            case DXGI_FORMAT_R8G8_B8G8_UNORM:
            case DXGI_FORMAT_G8R8_G8B8_UNORM:
                match = 1;
                break;
            }
        }

        if(match || (header->ddspf.dwFlags & DDPF_YUV))
            return header->dwHeight * ((header->dwWidth+1)>>1) * 4; // complete guess!
    }

    // Other formats
    {
        if(header->ddspf.dwFlags & (DDPF_RGB | DDPF_LUMINANCE | DDPF_YUV))
            return header->dwHeight * (((header->dwWidth * header->ddspf.dwRGBBitCount) + 7) / 8);
    }

    return 0;
}
#endif

#ifdef WITH_DDS_READ
static
enum imgFormat getFormat10(const DDS_HEADER_DXT10 *header10) {

    switch(header10->dxgiFormat) {
    default:
        return IMG_FMT_UNKNOWN;
    case DXGI_FORMAT_UNKNOWN://                      = 0,
        return IMG_FMT_UNKNOWN;
        //case :DXGI_FORMAT_R32G32B32A32_TYPELESS://        = 1,
        //case :DXGI_FORMAT_R32G32B32A32_FLOAT://           = 2,
        //case :DXGI_FORMAT_R32G32B32A32_UINT://            = 3,
        //case :DXGI_FORMAT_R32G32B32A32_SINT://            = 4,
        //case :DXGI_FORMAT_R32G32B32_TYPELESS://           = 5,
        //case :DXGI_FORMAT_R32G32B32_FLOAT://              = 6,
        //case :DXGI_FORMAT_R32G32B32_UINT://               = 7,
        //case :DXGI_FORMAT_R32G32B32_SINT://               = 8,
        //case :DXGI_FORMAT_R16G16B16A16_TYPELESS://        = 9,
        //case :DXGI_FORMAT_R16G16B16A16_FLOAT://           = 10,
        //case :DXGI_FORMAT_R16G16B16A16_UNORM://           = 11,
        //case :DXGI_FORMAT_R16G16B16A16_UINT://            = 12,
        //case :DXGI_FORMAT_R16G16B16A16_SNORM://           = 13,
        //case :DXGI_FORMAT_R16G16B16A16_SINT://            = 14,
        //case :DXGI_FORMAT_R32G32_TYPELESS://              = 15,
        //case :DXGI_FORMAT_R32G32_FLOAT://                 = 16,
        //case :DXGI_FORMAT_R32G32_UINT://                  = 17,
        //case :DXGI_FORMAT_R32G32_SINT://                  = 18,
        //case :DXGI_FORMAT_R32G8X24_TYPELESS://            = 19,
        //case :DXGI_FORMAT_D32_FLOAT_S8X24_UINT://         = 20,
        //case :DXGI_FORMAT_R32_FLOAT_X8X24_TYPELESS://     = 21,
        //case :DXGI_FORMAT_X32_TYPELESS_G8X24_UINT://      = 22,
        //case :DXGI_FORMAT_R10G10B10A2_TYPELESS://         = 23,
        //case :DXGI_FORMAT_R10G10B10A2_UNORM://            = 24,
        //case :DXGI_FORMAT_R10G10B10A2_UINT://             = 25,
        //case :DXGI_FORMAT_R11G11B10_FLOAT://              = 26,
        //case :DXGI_FORMAT_R8G8B8A8_TYPELESS://            = 27,
        //case :DXGI_FORMAT_R8G8B8A8_UNORM://               = 28,
        //case :DXGI_FORMAT_R8G8B8A8_UNORM_SRGB://          = 29,
        //case :DXGI_FORMAT_R8G8B8A8_UINT://                = 30,
        //case :DXGI_FORMAT_R8G8B8A8_SNORM://               = 31,
        //case :DXGI_FORMAT_R8G8B8A8_SINT://                = 32,
        //case :DXGI_FORMAT_R16G16_TYPELESS://              = 33,
        //case :DXGI_FORMAT_R16G16_FLOAT://                 = 34,
        //case :DXGI_FORMAT_R16G16_UNORM://                 = 35,
        //case :DXGI_FORMAT_R16G16_UINT://                  = 36,
        //case :DXGI_FORMAT_R16G16_SNORM://                 = 37,
        //case :DXGI_FORMAT_R16G16_SINT://                  = 38,
        //case :DXGI_FORMAT_R32_TYPELESS://                 = 39,
        //case :DXGI_FORMAT_D32_FLOAT://                    = 40,
        //case :DXGI_FORMAT_R32_FLOAT://                    = 41,
        //case :DXGI_FORMAT_R32_UINT://                     = 42,
        //case :DXGI_FORMAT_R32_SINT://                     = 43,
        //case :DXGI_FORMAT_R24G8_TYPELESS://               = 44,
        //case :DXGI_FORMAT_D24_UNORM_S8_UINT://            = 45,
        //case :DXGI_FORMAT_R24_UNORM_X8_TYPELESS://        = 46,
        //case :DXGI_FORMAT_X24_TYPELESS_G8_UINT://         = 47,
        //case :DXGI_FORMAT_R8G8_TYPELESS://                = 48,
        //case :DXGI_FORMAT_R8G8_UNORM://                   = 49,
        //case :DXGI_FORMAT_R8G8_UINT://                    = 50,
        //case :DXGI_FORMAT_R8G8_SNORM://                   = 51,
        //case :DXGI_FORMAT_R8G8_SINT://                    = 52,
        //case :DXGI_FORMAT_R16_TYPELESS://                 = 53,
        //case :DXGI_FORMAT_R16_FLOAT://                    = 54,
        //case :DXGI_FORMAT_D16_UNORM://                    = 55,
        //case :DXGI_FORMAT_R16_UNORM://                    = 56,
        //case :DXGI_FORMAT_R16_UINT://                     = 57,
        //case :DXGI_FORMAT_R16_SNORM://                    = 58,
        //case :DXGI_FORMAT_R16_SINT://                     = 59,
        //case :DXGI_FORMAT_R8_TYPELESS://                  = 60,
        //case :DXGI_FORMAT_R8_UNORM://                     = 61,
        //case :DXGI_FORMAT_R8_UINT://                      = 62,
        //case :DXGI_FORMAT_R8_SNORM://                     = 63,
        //case :DXGI_FORMAT_R8_SINT://                      = 64,
        //case :DXGI_FORMAT_A8_UNORM://                     = 65,
        //case :DXGI_FORMAT_R1_UNORM://                     = 66,
        //case :DXGI_FORMAT_R9G9B9E5_SHAREDEXP://           = 67,
        //case :DXGI_FORMAT_R8G8_B8G8_UNORM://              = 68,
        //case :DXGI_FORMAT_G8R8_G8B8_UNORM://              = 69,
        //case :DXGI_FORMAT_BC1_TYPELESS://                 = 70,
        //case :DXGI_FORMAT_BC1_UNORM://                    = 71,
        //case :DXGI_FORMAT_BC1_UNORM_SRGB://               = 72,
        //case :DXGI_FORMAT_BC2_TYPELESS://                 = 73,
        //case :DXGI_FORMAT_BC2_UNORM://                    = 74,
        //case :DXGI_FORMAT_BC2_UNORM_SRGB://               = 75,
        //case :DXGI_FORMAT_BC3_TYPELESS://                 = 76,
        //case :DXGI_FORMAT_BC3_UNORM://                    = 77,
        //case :DXGI_FORMAT_BC3_UNORM_SRGB://               = 78,
        //case :DXGI_FORMAT_BC4_TYPELESS://                 = 79,
        //case :DXGI_FORMAT_BC4_UNORM://                    = 80,
        //case :DXGI_FORMAT_BC4_SNORM://                    = 81,
        //case :DXGI_FORMAT_BC5_TYPELESS://                 = 82,
        //case :DXGI_FORMAT_BC5_UNORM://                    = 83,
        //case :DXGI_FORMAT_BC5_SNORM://                    = 84,
        //case :DXGI_FORMAT_B5G6R5_UNORM://                 = 85,
        //case :DXGI_FORMAT_B5G5R5A1_UNORM://               = 86,
        //case :DXGI_FORMAT_B8G8R8A8_UNORM://               = 87,
        //case :DXGI_FORMAT_B8G8R8X8_UNORM://               = 88,
        //case :DXGI_FORMAT_R10G10B10_XR_BIAS_A2_UNORM://   = 89,
        //case :DXGI_FORMAT_B8G8R8A8_TYPELESS://            = 90,
        //case :DXGI_FORMAT_B8G8R8A8_UNORM_SRGB://          = 91,
        //case :DXGI_FORMAT_B8G8R8X8_TYPELESS://            = 92,
        //case :DXGI_FORMAT_B8G8R8X8_UNORM_SRGB://          = 93,
        //case :DXGI_FORMAT_BC6H_TYPELESS://                = 94,
        //case :DXGI_FORMAT_BC6H_UF16://                    = 95,
        //case :DXGI_FORMAT_BC6H_SF16://                    = 96,
        //case :DXGI_FORMAT_BC7_TYPELESS://                 = 97,
        //case :DXGI_FORMAT_BC7_UNORM://                    = 98,
        //case :DXGI_FORMAT_BC7_UNORM_SRGB://               = 99,
        //case :DXGI_FORMAT_AYUV://                         = 100,
        //case :DXGI_FORMAT_Y410://                         = 101,
        //case :DXGI_FORMAT_Y416://                         = 102,
        //case :DXGI_FORMAT_NV12://                         = 103,
        //case :DXGI_FORMAT_P010://                         = 104,
        //case :DXGI_FORMAT_P016://                         = 105,
        //case :DXGI_FORMAT_420_OPAQUE://                   = 106,
        //case :DXGI_FORMAT_YUY2://                         = 107,
        //case :DXGI_FORMAT_Y210://                         = 108,
        //case :DXGI_FORMAT_Y216://                         = 109,
        //case :DXGI_FORMAT_NV11://                         = 110,
        //case :DXGI_FORMAT_AI44://                         = 111,
        //case :DXGI_FORMAT_IA44://                         = 112,
        //case :DXGI_FORMAT_P8://                           = 113,
        //case :DXGI_FORMAT_A8P8://                         = 114,
        //case :DXGI_FORMAT_B4G4R4A4_UNORM://               = 115,
    }

    return IMG_FMT_UNKNOWN;
}
#endif

#ifdef WITH_DDS_READ
static
enum imgFormat getFormat(const DDS_HEADER *header, const DDS_HEADER_DXT10 *header10) {

    if(header10) {

        enum imgFormat fmt = getFormat10(header10);

        if(fmt != IMG_FMT_UNKNOWN)
            return fmt;
    }

    if(header->ddspf.dwFlags & DDPF_FOURCC) {

        switch(header->ddspf.dwFourCC) {

        case FOURCC('D','X','T','1'):
            return IMG_FMT_DXT1;
        case FOURCC('D','X','T','2'):
            return IMG_FMT_DXT2;
        case FOURCC('D','X','T','3'):
            return IMG_FMT_DXT3;
        case FOURCC('D','X','T','4'):
            return IMG_FMT_DXT4;
        case FOURCC('D','X','T','5'):
            return IMG_FMT_DXT5;
        }
    }

    if(header->ddspf.dwFlags & DDPF_RGB) {

        // uncompressed rgb(a)

        if(header->ddspf.dwFlags & DDPF_ALPHAPIXELS) {

            if(header->ddspf.dwRGBBitCount == 32) {

                if(header->ddspf.dwRBitMask == 0xff000000)
                    return IMG_FMT_RGBA32;
                else if(header->ddspf.dwRBitMask == 0x00ff0000)
                    return IMG_FMT_ARGB32;
                else if(header->ddspf.dwRBitMask == 0x0000ff00)
                    return IMG_FMT_BGRA32;
                else if(header->ddspf.dwRBitMask == 0x000000ff)
                    return IMG_FMT_ABGR32;
            }
        }

        if(header->ddspf.dwRGBBitCount == 24) {

            if(header->ddspf.dwRBitMask == 0x00ff0000)
                return IMG_FMT_RGB24;
            else if(header->ddspf.dwRBitMask == 0x000000ff)
                return IMG_FMT_BGR24;
        }
    }

    return IMG_FMT_UNKNOWN;
}
#endif

#ifdef WITH_DDS_READ

static
int read_dds(const char *filename, struct imgImage *img_data, int read) {

    FILE *file;

    if(imgCheckFileExtension(filename, ".dds") != 0)
        return IMG_BADEXT;

    assert(img_data);

    file = fopen(filename,"rb");
    if (file==NULL)
        return IMG_NOFILE;

    char magic[4];
    if((fread(magic,4,1,file) != 1) || (strncmp(magic,"DDS ",4) != 0)) {
//		printf("%s badmagic\n",filename);
        fclose(file);
        return IMG_BADMAGIC;
    }

    DDS_HEADER 			dds_Header;
    DDS_HEADER_DXT10	dds_Header_DXT10;

    if(fread(&dds_Header, sizeof dds_Header, 1, file) != 1) {
        fclose(file);
//		printf("%s read header error\n",filename);
        return IMG_ERROR;
    }

    if(dds_Header.dwSize != sizeof dds_Header)
    {
        printf("WARNING: header.dwSize==%d (expected 124)\n",dds_Header.dwSize);
        fseek(file, dds_Header.dwSize + 4, SEEK_SET);
    }

    int hasDXT10 = ((dds_Header.ddspf.dwFlags & DDPF_FOURCC) && (dds_Header.ddspf.dwFourCC == FOURCC('D','X','1','0')));

    if(hasDXT10)
        if(fread(&dds_Header_DXT10, sizeof dds_Header_DXT10, 1, file) != 1) {
            printf("%s read header2 error\n",filename);
            fclose(file);
            return IMG_ERROR;
        }

    img_data->format = getFormat(&dds_Header, hasDXT10 ? &dds_Header_DXT10 : NULL);
    img_data->width = dds_Header.dwWidth;
    img_data->height = dds_Header.dwHeight;
    img_data->linesize[0] = getPitch(&dds_Header, hasDXT10 ? &dds_Header_DXT10 : NULL);
    img_data->linearsize[0] = determineLinearSize(&dds_Header, hasDXT10 ? &dds_Header_DXT10 : NULL);

    printf("header.dwSize = %d\n", dds_Header.dwSize);
    printf("header.dwFlags = ");
    DDSD_Print( dds_Header.dwFlags );
    printf("\n");
    printf("header.dwWidth = %d\n",dds_Header.dwWidth);
    printf("header.dwHeight = %d\n",dds_Header.dwHeight);
    printf("header.dwPitchOrLinearSize = %d ( calculated pitch = %d )\n",dds_Header.dwPitchOrLinearSize, img_data->linesize[0]);
    printf("header.dwDepth = %d\n",dds_Header.dwDepth);
    printf("header.dwMipMapCount = %d\n",dds_Header.dwMipMapCount);
    printf("header.ddspf.dwSize = %d\n",dds_Header.ddspf.dwSize );
    printf("header.ddspf.dwFlags = ");
    DDPF_Print(dds_Header.ddspf.dwFlags);
    printf("\n");
    printf("header.ddspf.dwFourCC = %d",dds_Header.ddspf.dwFourCC);
    printf("('%c''%c''%c''%c')\n", dds_Header.ddspf.dwFourCC & 0xff, (dds_Header.ddspf.dwFourCC >> 8) & 0xff, (dds_Header.ddspf.dwFourCC >> 16) & 0xff, (dds_Header.ddspf.dwFourCC >> 24) & 0xff);
    printf("header.ddspf.dwRGBBitCount = %d\n",dds_Header.ddspf.dwRGBBitCount);
    printf("header.ddspf.dwRBitMask = %08x\n",dds_Header.ddspf.dwRBitMask);
    printf("header.ddspf.dwGBitMask = %08x\n",dds_Header.ddspf.dwGBitMask);
    printf("header.ddspf.dwBBitMask = %08x\n",dds_Header.ddspf.dwBBitMask);
    printf("header.ddspf.dwABitMask = %08x\n",dds_Header.ddspf.dwABitMask);
    printf("header.dwCaps = ");
    DDSCAPS_Print (dds_Header.dwCaps) ;
    printf("\n");
    printf("header.dwCaps2 = ");
    DDSCAPS2_Print(dds_Header.dwCaps2);
    printf("\n");
    printf("header.dwCaps3 = %d\n",dds_Header.dwCaps3);
    printf("header.dwCaps4 = %d\n",dds_Header.dwCaps4);
    if(hasDXT10)
        printf("header10.dxgiFormat = ");
    DXGI_FORMAT_ToString(dds_Header_DXT10.dxgiFormat);
    printf("\n");

    if(read) {
      
	int array_index = 0;
	int mip_index = 0;
	int arraySize = 1;
	int mipSize = 1;
//	size_t full_size = img_data->linearsize[0];
//	size_t this_size;
	
	if(hasDXT10 && dds_Header_DXT10.arraySize)
	  arraySize = dds_Header_DXT10.arraySize;
	
	// TODO: is mipCount is 1, is there the full image, and a mip image, or is there only the full image !?
	if(dds_Header.dwMipMapCount)
	  mipSize = 1;
	
	for(array_index = 0; array_index < arraySize; array_index++) {
	
	  //this_size = full_size;
	  
	  for(mip_index = 0; mip_index < mipSize; mip_index++) {

	    if(fread(img_data->data.channel[0],img_data->linearsize[0],1,file) != 1) {

		printf("%s read pixels error\n",filename);
		fclose(file);
		return IMG_ERROR;
	    }
	    
	    // TODO: finish handleing texture arrays and mip levels.
	    //  until then, bail out after the first image is read.
	    fclose(file);
	    return IMG_OKAY;
	    
	  }
	}
    }

    fclose(file);
    return IMG_OKAY;
}

int imgStatImgDds(const char *filename, struct imgImage *img_data) {

    return read_dds(filename, img_data, 0);
}

int imgReadImgDds(const char *filename, struct imgImage *img_data) {

    return read_dds(filename, img_data, 1);
}

#endif /*** WITH_DDS_READ ***/

#ifdef WITH_DDS_WRITE

enum imgFormat imgRecomendFormatDds(const char * filename, enum imgFormat hint, int allow_poorly_supported) {

	if(imgCheckFileExtension(filename,".dds")!=0)
		return IMG_FMT_UNKNOWN;

	switch(hint) {
		default:				break;
		case IMG_FMT_UNKNOWN:	return IMG_FMT_RGBA32;
		case IMG_FMT_DXT1:		return IMG_FMT_DXT1;
		case IMG_FMT_DXT3:		return IMG_FMT_DXT3;
		case IMG_FMT_DXT5:		return IMG_FMT_DXT5;
		case IMG_FMT_RGB24:		return IMG_FMT_RGB24;
		case IMG_FMT_BGR24:		return IMG_FMT_BGR24;
		case IMG_FMT_RGBA32:	return IMG_FMT_RGBA32;
		case IMG_FMT_ARGB32:	return IMG_FMT_ARGB32;
		case IMG_FMT_BGRA32:	return IMG_FMT_BGRA32;
		case IMG_FMT_ABGR32:	return IMG_FMT_ABGR32;
	}

	if(hint & IMG_FMT_COMPONENT_ALPHA)
		return IMG_FMT_RGBA32;
	else
		return IMG_FMT_RGB24;
}

int imgWriteImgDds(const char *filename, struct imgImage *img) {

    if(imgCheckFileExtension(filename,".dds")!=0)
        return IMG_BADEXT;

    if(!img || !filename)
        return IMG_ERROR;

    DDS_HEADER 			dds_Header;
    DDS_HEADER_DXT10	dds_Header_DXT10;

    memset(&dds_Header, 		0, sizeof dds_Header);
    memset(&dds_Header_DXT10, 	0, sizeof dds_Header_DXT10);

    dds_Header.dwSize 				= sizeof dds_Header;
    dds_Header.dwFlags				= DDSD_CAPS | DDSD_WIDTH | DDSD_HEIGHT | DDSD_PITCH | DDSD_PIXELFORMAT ;
    dds_Header.dwWidth				= img->width;
    dds_Header.dwHeight				= img->height;
    dds_Header.dwDepth				= 0;
    dds_Header.dwMipMapCount		= 0;
    dds_Header.ddspf.dwSize 		= sizeof dds_Header.ddspf;

    if(img->format == IMG_FMT_DXT1) {
        dds_Header.ddspf.dwFlags |= DDPF_FOURCC;
        dds_Header.ddspf.dwFourCC = FOURCC('D','X','T','1');
        dds_Header.dwPitchOrLinearSize = 8 * ((img->width+3)/4) + ((img->height+3)/4);
    }
    else if(img->format == IMG_FMT_DXT3) {
        dds_Header.ddspf.dwFlags |= DDPF_FOURCC;
        dds_Header.ddspf.dwFourCC = FOURCC('D','X','T','3');
        dds_Header.dwPitchOrLinearSize = 16 * ((img->width+3)/4) + ((img->height+3)/4);
    }
    else if(img->format == IMG_FMT_DXT5) {
        dds_Header.ddspf.dwFlags |= DDPF_FOURCC;
        dds_Header.ddspf.dwFourCC = FOURCC('D','X','T','5');
        dds_Header.dwPitchOrLinearSize = 16 * ((img->width+3)/4) + ((img->height+3)/4);
    }
    else if((img->format & IMG_FMT_COMPONENT_PACKED24) == IMG_FMT_COMPONENT_PACKED24) {
        dds_Header.ddspf.dwRGBBitCount = 24;
        dds_Header.ddspf.dwFlags |= DDPF_RGB;
        dds_Header.dwPitchOrLinearSize	= img->linesize[0];
    }
    else if((img->format & IMG_FMT_COMPONENT_PACKED32) == IMG_FMT_COMPONENT_PACKED32) {
        dds_Header.ddspf.dwRGBBitCount = 32;
        dds_Header.ddspf.dwFlags |= (DDPF_RGB | DDPF_ALPHAPIXELS);
        dds_Header.dwPitchOrLinearSize	= img->linesize[0];
    }
    else
        return IMG_BADFORMAT;

    switch(img->format & (IMG_FMT_COMPONENT_RGBA | IMG_FMT_COMPONENT_ARGB | IMG_FMT_COMPONENT_BGRA | IMG_FMT_COMPONENT_ABGR)) {

    default:
        return IMG_BADFORMAT;

    case IMG_FMT_COMPONENT_RGBA:

        /*** DIRECT3D 10+ / OpenGL Format ***/

        if(img->format & IMG_FMT_COMPONENT_ALPHA) {
            dds_Header.ddspf.dwRBitMask = 0xff000000;
            dds_Header.ddspf.dwGBitMask = 0x00ff0000;
            dds_Header.ddspf.dwBBitMask = 0x0000ff00;
            dds_Header.ddspf.dwABitMask = 0x000000ff;
        }
        else {
            dds_Header.ddspf.dwRBitMask = 0x00ff0000;
            dds_Header.ddspf.dwGBitMask = 0x0000ff00;
            dds_Header.ddspf.dwBBitMask = 0x000000ff;
        }
        break;
    case IMG_FMT_COMPONENT_BGRA:
        /*** DIRECT3D 11+ / OpenGL Format ***/
        if(img->format & IMG_FMT_COMPONENT_ALPHA) {
            dds_Header.ddspf.dwRBitMask = 0x0000ff00;
            dds_Header.ddspf.dwGBitMask = 0x00ff0000;
            dds_Header.ddspf.dwBBitMask = 0xff000000;
            dds_Header.ddspf.dwABitMask = 0x000000ff;
        }
        else {
            dds_Header.ddspf.dwRBitMask = 0x000000ff;
            dds_Header.ddspf.dwGBitMask = 0x0000ff00;
            dds_Header.ddspf.dwBBitMask = 0x00ff0000;
        }
        break;
	case IMG_FMT_COMPONENT_ARGB:
		/*** DIRECT3D 9 Format ***/
		dds_Header.ddspf.dwRBitMask = 0x00ff0000;
		dds_Header.ddspf.dwGBitMask = 0x0000ff00;
		dds_Header.ddspf.dwBBitMask = 0x000000ff;
		dds_Header.ddspf.dwABitMask = 0xff000000;
		break;
	case IMG_FMT_COMPONENT_ABGR:
		/*** DIRECT3D 9 Format ***/
		dds_Header.ddspf.dwRBitMask = 0x000000ff;
		dds_Header.ddspf.dwGBitMask = 0x0000ff00;
		dds_Header.ddspf.dwBBitMask = 0x00ff0000;
		dds_Header.ddspf.dwABitMask = 0xff000000;
		break;
    }

    dds_Header.dwCaps = DDSCAPS_TEXTURE;

    FILE *file = fopen(filename, "wb" );

    if(!file)
        return IMG_ERROR;

    int err = IMG_ERROR;

    if(fwrite("DDS ",4,1,file) == 1)
        if(fwrite(&dds_Header,sizeof dds_Header, 1, file) == 1) {

            int okay = 1;

            if((dds_Header.ddspf.dwFlags & DDPF_FOURCC) && (dds_Header.ddspf.dwFourCC == FOURCC('D','X','1','0')))
                okay = (fwrite(&dds_Header_DXT10, sizeof dds_Header_DXT10, 1, file) == 1);

            if(okay && (fwrite(img->data.channel[0], img->linesize[0] * img->height ,1, file) == 1))
                err = IMG_OKAY;
        }

    fclose(file);

    return err;
}

#endif


#endif /* LIBIMG_DDS_CPP_ */

