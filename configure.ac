AC_PREREQ([2.69])
AC_INIT(libimg, 0.1, chris.stones@gmail.com)
AC_CONFIG_AUX_DIR(config)
AC_CONFIG_SRCDIR(src/libimg.c)
AC_CONFIG_HEADERS(src/config.h)

AC_CONFIG_MACRO_DIR([m4])

LT_INIT
AM_INIT_AUTOMAKE([subdir-objects])

AC_PROG_CC
AC_PROG_INSTALL

# DEFAULTS - ENABLE EVERYTHING.
with_libpng="yes"
enable_png_read="yes"
enable_png_write="yes"
enable_bmp_read="yes"
enable_bmp_write="yes"
enable_dds_read="yes"
enable_dds_write="yes"
enable_tga_read="yes"
enable_tga_write="yes"

AC_ARG_WITH([libpng], AS_HELP_STRING([--with-libpng], [Use libpng]))

AC_ARG_ENABLE([png-read],  AS_HELP_STRING([--enable-png-read], [Enable reading png]))
AC_ARG_ENABLE([png-write], AS_HELP_STRING([--enable-png-write],[Enable writing png]))
AC_ARG_ENABLE([bmp-read],  AS_HELP_STRING([--enable-bmp-read], [Enable reading bmp]))
AC_ARG_ENABLE([bmp-write], AS_HELP_STRING([--enable-bmp-write],[Enable writing bmp]))
AC_ARG_ENABLE([dds-read],  AS_HELP_STRING([--enable-dds-read], [Enable reading dds]))
AC_ARG_ENABLE([dds-write], AS_HELP_STRING([--enable-dds-write],[Enable writing dds]))
AC_ARG_ENABLE([tga-read],  AS_HELP_STRING([--enable-tga-read], [Enable reading tga]))
AC_ARG_ENABLE([tga-write], AS_HELP_STRING([--enable-tga-write],[Enable writing tga]))

AS_IF([test "x$enable_png_read"  = xyes],[with_libpng=yes],[])
AS_IF([test "x$enable_png_write" = xyes],[with_libpng=yes],[])

AS_IF([test "x$with_libpng" = xyes],
[
  AC_CHECK_HEADERS([png.h],[],[AC_MSG_ERROR([Missing png.h])])
  AC_SEARCH_LIBS([png_init_io],[png],[],[AC_MSG_ERROR([Unable to find libpng])])
],
[])

AC_CHECK_HEADERS([linux/limits.h],[],[AC_MSG_ERROR([Missing linux/limits.h])])

AS_IF([test "x$enable_png_read" = xyes],[AC_DEFINE([WITH_PNG_READ],[1],[PNG Read Support])],[])
AS_IF([test "x$enable_bmp_read" = xyes],[AC_DEFINE([WITH_BMP_READ],[1],[BMP Read Support])],[])
AS_IF([test "x$enable_dds_read" = xyes],[AC_DEFINE([WITH_DDS_READ],[1],[DDS Read Support])],[])
AS_IF([test "x$enable_tga_read" = xyes],[AC_DEFINE([WITH_TGA_READ],[1],[TGA Read Support])],[])

AS_IF([test "x$enable_png_write" = xyes],[AC_DEFINE([WITH_PNG_WRITE],[1],[PNG Write Support])],[])
AS_IF([test "x$enable_bmp_write" = xyes],[AC_DEFINE([WITH_BMP_WRITE],[1],[BMP Write Support])],[])
AS_IF([test "x$enable_dds_write" = xyes],[AC_DEFINE([WITH_DDS_WRITE],[1],[DDS Write Support])],[])
AS_IF([test "x$enable_tga_write" = xyes],[AC_DEFINE([WITH_TGA_WRITE],[1],[TGA Write Support])],[])

AM_CONDITIONAL([BUILD_PNG], [test "x$enable_png_read" = xyes -o "x$enable_png_write" = xyes])
AM_CONDITIONAL([BUILD_BMP], [test "x$enable_bmp_read" = xyes -o "x$enable_bmp_write" = xyes])
AM_CONDITIONAL([BUILD_DDS], [test "x$enable_dds_read" = xyes -o "x$enable_dds_write" = xyes])
AM_CONDITIONAL([BUILD_TGA], [test "x$enable_tga_read" = xyes -o "x$enable_tga_write" = xyes])

AC_CONFIG_FILES([
	Makefile
	src/Makefile
])
AC_OUTPUT

