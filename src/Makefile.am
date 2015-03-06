lib_LTLIBRARIES = libimg.la
libimg_la_SOURCES = libimg.h libimg.c

if BUILD_PNG
	libimg_la_SOURCES += img_png.c img_png.h
endif

if BUILD_BMP
libimg_la_SOURCES += img_bmp.c img_bmp.h
endif

if BUILD_DDS
libimg_la_SOURCES += img_dds.c img_dds.h
endif

if BUILD_TGA
libimg_la_SOURCES += img_tga.c img_tga.h
endif

include_HEADERS = libimg.h
